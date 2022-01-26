//Include LCD, I2C and Wire library 
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "main.h"


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
Adafruit_NeoPixel ledString = Adafruit_NeoPixel(N_LEDS, LED_STRING_PIN, NEO_RGB);

void setup() {
    Wire.begin(); // Start I2C as master
    //Serial.begin(9600); // Use only for debugging
    pinMode(13, OUTPUT);

    setup_mpu_6050_registers(); // Setup the registers of the MPU-6050 and start the gyro

    digitalWrite(13, HIGH); // Set digital output 13 high to indicate startup

    lcd.init();
    lcd.backlight();
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("  MPU-6050 IMU");
    lcd.setCursor(0,1);
    lcd.print("     V1.2");

    delay(1500);
    lcd.clear();
    
    // Begin calibration
    lcd.setCursor(0,0);
    lcd.print("Calibrating gyro");
    lcd.setCursor(0,1);
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

    lcd.clear();
    // End calibration

    lcd.setCursor(0,0);
    lcd.print("Pitch:");
    lcd.setCursor(0,1);
    lcd.print("Roll :");

    digitalWrite(13, LOW); // All done, turn the LED off

    loopTimer = micros(); // Reset the loop timer

    ledString.begin(); // Begin 
}

void loop(){

    ledString.setBrightness(analogRead(PHOTORESISTOR_PIN)/4); //Reading enviroinmental light 

    data = read_mpu_6050_data(); // Read the raw acc and gyro data from the MPU-6050

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
}


void setLed(float GyX, float GyY) {
    bool Xflat, Yflat;
    const int INCL_TOLL = 3;
    uint8_t i;
    
    Xflat = (GyX < INCL_TOLL && GyX > (INCL_TOLL*-1));
    Yflat = (GyY < INCL_TOLL && GyY > (INCL_TOLL*-1));
    if(Xflat && Yflat) //in bolla
    {
        for(i = 0; i < 4; i++)
        {
            ledString.setPixelColor(i, ledString.Color(255, 0, 0));
        }
    }
    else //non in bolla
    {
        if(Xflat) //x in bolla
        {
            if(GyY >= INCL_TOLL) //y maggiore
            {
                ledString.setPixelColor(0, ledString.Color(0, 255, 0));
                ledString.setPixelColor(1, ledString.Color(0, 255, 0));
                ledString.setPixelColor(2, ledString.Color(0, 0, 255));
                ledString.setPixelColor(3, ledString.Color(0, 0, 255));
            }
            else //y minore
            {
                ledString.setPixelColor(0, ledString.Color(0, 0, 255));
                ledString.setPixelColor(1, ledString.Color(0, 0, 255));
                ledString.setPixelColor(2, ledString.Color(0, 255, 0));
                ledString.setPixelColor(3, ledString.Color(0, 255, 0));
            }
        }
        else if(Yflat) //y in bolla
        {
            if(GyX >= INCL_TOLL) //x maggiore
            {
                ledString.setPixelColor(0, ledString.Color(0, 0, 255));
                ledString.setPixelColor(1, ledString.Color(0, 255, 0));
                ledString.setPixelColor(2, ledString.Color(0, 255, 0));
                ledString.setPixelColor(3, ledString.Color(0, 0, 255));
            }
            else //x minore
            {
                ledString.setPixelColor(0, ledString.Color(0, 255, 0));
                ledString.setPixelColor(1, ledString.Color(0, 0, 255));
                ledString.setPixelColor(2, ledString.Color(0, 0, 255));
                ledString.setPixelColor(3, ledString.Color(0, 255, 0));
            }
        }
        else
        {
            if(GyX >= INCL_TOLL) //x maggiore
            {
                if(GyY >= INCL_TOLL) //y maggiore
                {
                    ledString.setPixelColor(0, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(1, ledString.Color(0, 255, 0));
                    ledString.setPixelColor(2, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(3, ledString.Color(0, 0, 255));
                }
                else //y minore
                {
                    ledString.setPixelColor(0, ledString.Color(0, 0, 255));
                    ledString.setPixelColor(1, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(2, ledString.Color(0, 255, 0));
                    ledString.setPixelColor(3, ledString.Color(255, 0, 0));
                }
            }
            else //x minore
            {
                if(GyY >= INCL_TOLL) //y maggiore
                {
                    ledString.setPixelColor(0, ledString.Color(0, 255, 0));
                    ledString.setPixelColor(1, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(2, ledString.Color(0, 0, 255));
                    ledString.setPixelColor(3, ledString.Color(255, 0, 0));
                }
                else //y minore
                {
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

