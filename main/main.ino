//Include LCD, I2C and Wire library 
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "main.h"

#define INCL_TOLL_X 3
#define INCL_TOLL_Y 2

//Declaring some global variables
GyroscopeData data;
GyroscopeCalibration calibrationData;
long loopTimer;
int lcdLoopCounter;
Angle angle;
AngleBuffer angleBuffer;
bool setGyroAngles;
Angle angleAcc;
Angle angleOutput;

//Initialize the NeoPixel library
Adafruit_NeoPixel ledString = Adafruit_NeoPixel(4, 9, NEO_RGB);
void setLed(float GyX, float GyY);
/* Settings readSettings();
String readDataFromSD(String fileName); */

void setup() {
    Serial.begin(9600);
    //settings.ledStringPin = 9;
    //4 = 4;
    //settings.photoresistorPin = A0;
    //settings.buzzerPin = 8;
    //INCL_TOLL_X = 3;
    //INCL_TOLL_Y = 2;
    //settings.propBrightness = 4;
    //settings.pitchCorrection = 0.0;
    //settings.rollCorrection = 0.0;
    //settings.xLenght = 42;
    //settings.yLenght = 82;
    //Serial.println("Entered setup()...");
    //Serial.println("Settings from SD:"); // Print all settings for debug
    //Serial.print("Led String Pin: "); Serial.println(settings.ledStringPin);
    //Serial.print("Leds Number: "); Serial.println(4);
    //Serial.print("Photoresistor Pin: "); Serial.println(settings.photoresistorPin);
    //Serial.print("Buzzer Pin: "); Serial.println(settings.buzzerPin);
    //Serial.print("X Inclination Tollerance: "); Serial.println(INCL_TOLL_X);
    //Serial.print("Y Inclination Tollerance: "); Serial.println(INCL_TOLL_Y);
    //Serial.print("Proportional Brightness: "); Serial.println(settings.propBrightness);
    //Serial.print("Pitch Correction: "); Serial.println(settings.pitchCorrection);
    //Serial.print("Roll Correction: "); Serial.println(settings.rollCorrection);
    //Serial.print("X Lenght: "); Serial.println(settings.xLenght);
    //Serial.print("Y Lenght: "); Serial.println(settings.yLenght);
    
    Wire.begin(); // Start I2C as master
    pinMode(13, OUTPUT);
    //pinMode(settings.buzzerPin, OUTPUT);
    Serial.println("Preparing the gyroscope...");
    setup_mpu_6050_registers(); // Setup the registers of the MPU-6050 and start the gyro
    Serial.println("Gyroscope is ready for calibration...");
    digitalWrite(13, HIGH); // Set digital output 13 high to indicate startup
    lcd.init(); lcd.backlight(); lcd.clear(); lcd.setCursor(0,0); lcd.print("  MPU-6050 IMU"); lcd.setCursor(0,1);
    lcd.print("     V1.2"); delay(500); lcd.clear();
    
    // Begin calibration
    lcd.setCursor(0,0); lcd.print("Calibrating gyro"); lcd.setCursor(0,1);// tone(settings.buzzerPin, 1000);
    delay(500); //noTone(settings.buzzerPin);
    ledString.begin(); // Begin ledString.
    //ledString.setBrightness(analogRead(settings.photoresistorPin)/settings.propBrightness); //Reading enviroinmental light
    ledString.setBrightness(255); //Reading enviroinmental light
    //Serial.print("Photoresistor reading/propBrightness: "); Serial.println(analogRead(settings.photoresistorPin)/settings.propBrightness);
    //Serial.print("Photoresistor reading/propBrightness: "); Serial.println(1024/settings.propBrightness);
    for(uint8_t i = 0; i < 4; i++) {
        Serial.print("Turning white led n."); Serial.print(i); Serial.println("...");
        ledString.setPixelColor(i, ledString.Color(255, 255, 255));
    }
    ledString.show();
    Serial.println("Showing leds color (Should be white)...");
    for (int cal_int = 0; cal_int < 2000 ; cal_int++) {
        if(cal_int % 125 == 0) { // Print a dot on the LCD every 125 readings
            lcd.print(".");
        }
        data = read_mpu_6050_data(); // Read the raw acc and gyro data from the MPU-6050
        calibrationData.x += data.gyroX; // Add the gyro axis offset to the calibration variable
        calibrationData.y += data.gyroY;
        calibrationData.z += data.gyroZ;
        delay(3);
    }
    calibrationData.x /= 2000; // Divide the variable by 2000 to get the avarage offset
    calibrationData.y /= 2000;
    calibrationData.z /= 2000;
    Serial.println("Gyroscope is calibrated");

    lcd.clear();
    // End calibration

    lcd.setCursor(0,0);
    lcd.print("Pitch:");
    lcd.setCursor(0,1);
    lcd.print("Roll :");

    digitalWrite(13, LOW); // All done, turn the LED off
    //tone(settings.buzzerPin, 1000);
    delay(500);
    //noTone(settings.buzzerPin);

    loopTimer = micros(); // Reset the loop timer
    Serial.println("Exiting from setup()");
}

void loop(){
    //ledString.setBrightness(analogRead(settings.photoresistorPin)/settings.propBrightness); //Reading enviroinmental light
    ledString.setBrightness(255); //Reading enviroinmental light
    //Serial.print("Photoresitor reading/propBrightness: "); Serial.println(analogRead(settings.photoresistorPin)/settings.propBrightness);
    //Serial.print("Photoresitor reading/propBrightness: "); Serial.println(1024/settings.propBrightness);

    Serial.println("Preparing to read data from gyroscope...");
    data = read_mpu_6050_data(); // Read the raw acc and gyro data from the MPU-6050
    Serial.println("Read data from gyroscope complete...");

    data.gyroX -= calibrationData.x; // Subtract the offset calibration value from the raw gyro value
    data.gyroY -= calibrationData.y;
    data.gyroZ -= calibrationData.z;

    angle.pitch += data.gyroX * 0.0000611; // Calculate the traveled angle and add this to the variable
    angle.roll += data.gyroY * 0.0000611;

    angle.pitch += angle.roll * sin(data.gyroZ * 0.000001066); // If the IMU has yawed transfer the roll angle to the pitch angle and viceversa
    angle.roll -= angle.pitch * sin(data.gyroZ * 0.000001066);

    data.accTot = sqrt((data.accX*data.accX)+(data.accY*data.accY)+(data.accZ*data.accZ)); // Calculate the total accelerometer vector

    angleAcc.pitch = asin((float)data.accY/data.accTot)* 57.296; // Calculate the pitch angle
    angleAcc.roll = asin((float)data.accX/data.accTot)* -57.296; // Calculate the roll angle

    angleAcc.pitch -= 0.0; // Accelerometer calibration value for pitch
    angleAcc.roll -= 0.0; // Accelerometer calibration value for roll

    if(setGyroAngles) { // If the IMU is already started
        angle.pitch = angle.pitch * 0.9996 + angleAcc.pitch * 0.0004; // Correct the drift of the gyro pitch angle with the accelerometer pitch angle
        angle.roll = angle.roll * 0.9996 + angleAcc.roll * 0.0004; // Correct the drift of the gyro roll angle with the accelerometer roll angle
    }
    else { // At first start
        angle.pitch = angleAcc.pitch; // Set the gyro pitch angle equal to the accelerometer pitch angle 
        angle.roll = angleAcc.roll; // Set the gyro roll angle equal to the accelerometer roll angle 
        setGyroAngles = true; // Set the IMU started flag
    }

    angleOutput.pitch = angleOutput.pitch * 0.9 + angle.pitch * 0.1; // Take 90% of the output pitch value and add 10% of the raw pitch value
    angleOutput.roll = angleOutput.roll * 0.9 + angle.roll * 0.1; // Take 90% of the output roll value and add 10% of the raw roll value

    write_LCD(&lcdLoopCounter, &angleBuffer, angleOutput); // Write the roll and pitch values to the LCD display

    while(micros() - loopTimer < 4000); // Wait until the loopTimer reaches 4000us (250Hz) before starting the next loop
    loopTimer = micros(); // Reset the loop timer

    setLed(angleOutput.pitch, angleOutput.roll); // Set the led colors and show them
    Serial.print("Pitch: "); Serial.println(angleOutput.pitch); Serial.print("Roll: "); Serial.println(angleOutput.roll);
}

void setLed(float GyX, float GyY) {
    bool Xflat, Yflat;
    uint8_t i;
    
    Xflat = (GyX < INCL_TOLL_X && GyX > (INCL_TOLL_X * -1)); // X and Y angles respect tollerance
    Yflat = (GyY < INCL_TOLL_Y && GyY > (INCL_TOLL_Y * -1));
    if(Xflat && Yflat) { // flat
        for(i = 0; i < 4; i++) {
            ledString.setPixelColor(i, ledString.Color(255, 0, 0));
        }
    }
    else { // not flat
        if(Xflat) { // x flat
            if(GyY >= INCL_TOLL_Y) { //y too much
                ledString.setPixelColor(0, ledString.Color(0, 255, 0));
                ledString.setPixelColor(1, ledString.Color(0, 255, 0));
                ledString.setPixelColor(2, ledString.Color(0, 0, 255));
                ledString.setPixelColor(3, ledString.Color(0, 0, 255));
            }
            else { // y too less
                ledString.setPixelColor(0, ledString.Color(0, 0, 255));
                ledString.setPixelColor(1, ledString.Color(0, 0, 255));
                ledString.setPixelColor(2, ledString.Color(0, 255, 0));
                ledString.setPixelColor(3, ledString.Color(0, 255, 0));
            }
        }
        else if(Yflat) { // y flat
            if(GyX >= INCL_TOLL_X) { //x too much
                ledString.setPixelColor(0, ledString.Color(0, 0, 255));
                ledString.setPixelColor(1, ledString.Color(0, 255, 0));
                ledString.setPixelColor(2, ledString.Color(0, 255, 0));
                ledString.setPixelColor(3, ledString.Color(0, 0, 255));
            }
            else { // x too less
                ledString.setPixelColor(0, ledString.Color(0, 255, 0));
                ledString.setPixelColor(1, ledString.Color(0, 0, 255));
                ledString.setPixelColor(2, ledString.Color(0, 0, 255));
                ledString.setPixelColor(3, ledString.Color(0, 255, 0));
            }
        }
        else {
            if(GyX >= INCL_TOLL_X) { // x too much
                if(GyY >= INCL_TOLL_Y) { // y too much
                    ledString.setPixelColor(0, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(1, ledString.Color(0, 255, 0));
                    ledString.setPixelColor(2, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(3, ledString.Color(0, 0, 255));
                }
                else { // y too less
                    ledString.setPixelColor(0, ledString.Color(0, 0, 255));
                    ledString.setPixelColor(1, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(2, ledString.Color(0, 255, 0));
                    ledString.setPixelColor(3, ledString.Color(255, 0, 0));
                }
            }
            else { // x too less
                if(GyY >= INCL_TOLL_Y) { //y too much
                    ledString.setPixelColor(0, ledString.Color(0, 255, 0));
                    ledString.setPixelColor(1, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(2, ledString.Color(0, 0, 255));
                    ledString.setPixelColor(3, ledString.Color(255, 0, 0));
                }
                else { // y too less
                    ledString.setPixelColor(0, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(1, ledString.Color(0, 0, 255));
                    ledString.setPixelColor(2, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(3, ledString.Color(0, 255, 0));
                }
            }
        }
    }
    ledString.show();
}

/* Settings readSettings() { // Reads the settings from various files in the SD card
    Settings settings;
    settings.ledStringPin = readDataFromSD("lpin.txt").toInt();
    4 = readDataFromSD("ledn.txt").toInt();
    settings.photoresistorPin = readDataFromSD("ppin.txt").toInt() + A0;
    settings.buzzerPin = readDataFromSD("bpin.txt").toInt();
    INCL_TOLL_X = readDataFromSD("xint.txt").toInt();
    INCL_TOLL_Y = readDataFromSD("yint.txt").toInt();
    settings.propBrightness = readDataFromSD("lbad.txt").toInt();
    settings.pitchCorrection = readDataFromSD("acpi.txt").toFloat() / 10;
    settings.rollCorrection = readDataFromSD("acro.txt").toFloat() / 10;
    settings.xLenght = readDataFromSD("xlen.txt").toInt();
    settings.yLenght = readDataFromSD("ylen.txt").toInt();
    return settings;
}

String readDataFromSD(String fileName) { // Genaral function to read the content of a file
    String data;
    if(SD.begin(4)) {
        File dataFile = SD.open(fileName, FILE_READ);
        if(dataFile) {
            while (dataFile.available()) {
                data = data + char(dataFile.read());
            }
            dataFile.close();
        }
    }
    return data;
} */