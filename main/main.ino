//Include LCD, I2C and Wire library 
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "main.h"

//Some data types to help data managing with arduino

//Declaring some global variables
GyroscopeData data;
GyroscopeCalibration calibrationData;
long loop_timer;
int lcd_loop_counter;
Angle angle;
AngleBuffer angleBuffer;
bool set_gyro_angles;
Angle angleAcc;
Angle angleOutput;

//Declaring Leds const
const uint8_t PIN_STRISCIA = 9;   // Pin linea dati striscia
const uint8_t N_LEDS = 4; // Numero leds
const uint8_t PIN_FOTORES = A0; // Pin fotoresistore

//Initialize the LCD library
LiquidCrystal_I2C lcd(0x3F,16,2);

//Initialize the NeoPixel library
Adafruit_NeoPixel striscia = Adafruit_NeoPixel(N_LEDS, PIN_STRISCIA, NEO_RGB);

void setup() {
    Wire.begin();                                                        //Start I2C as master
    //Serial.begin(9600);                                                //Use only for debugging
    pinMode(13, OUTPUT);                                                 //Set output 13 (LED) as output

    setup_mpu_6050_registers();                                          //Setup the registers of the MPU-6050 (500dfs / +/-8g) and start the gyro

    digitalWrite(13, HIGH);                                               //Set digital output 13 high to indicate startup

    lcd.init();                                                           //Initialize the LCD
    lcd.backlight();                                                      //Activate backlight
    lcd.clear();                                                          //Clear the LCD

    lcd.setCursor(0,0);                                                  //Set the LCD cursor to position to position 0,0
    lcd.print("  MPU-6050 IMU");                                         //Print text to screen
    lcd.setCursor(0,1);                                                  //Set the LCD cursor to position to position 0,1
    lcd.print("     V1.1");                                              //Print text to screen

    delay(1500);                                                         //Delay 1.5 second to display the text
    lcd.clear();                                                         //Clear the LCD

    lcd.setCursor(0,0);                                                  //Set the LCD cursor to position to position 0,0
    lcd.print("Calibrating gyro");                                       //Print text to screen
    lcd.setCursor(0,1);                                                  //Set the LCD cursor to position to position 0,1
    for (int cal_int = 0; cal_int < 2000 ; cal_int++) {                  //Run this code 2000 times
        if(cal_int % 125 == 0) {
            lcd.print(".");
        }                              //Print a dot on the LCD every 125 readings
        read_mpu_6050_data();                                              //Read the raw acc and gyro data from the MPU-6050
        calibrationData.x += data.gyroX;                                              //Add the gyro x-axis offset to the gyro_x_cal variable
        calibrationData.y += data.gyroY;                                              //Add the gyro y-axis offset to the gyro_y_cal variable
        calibrationData.z += data.gyroZ;                                              //Add the gyro z-axis offset to the gyro_z_cal variable
        delay(3);                                                          //Delay 3us to simulate the 250Hz program loop
    }
    calibrationData.x /= 2000;                                                  //Divide the gyro_x_cal variable by 2000 to get the avarage offset
    calibrationData.y /= 2000;                                                  //Divide the gyro_y_cal variable by 2000 to get the avarage offset
    calibrationData.z /= 2000;                                                  //Divide the gyro_z_cal variable by 2000 to get the avarage offset

    lcd.clear();                                                         //Clear the LCD

    lcd.setCursor(0,0);                                                  //Set the LCD cursor to position to position 0,0
    lcd.print("Pitch:");                                                 //Print text to screen
    lcd.setCursor(0,1);                                                  //Set the LCD cursor to position to position 0,1
    lcd.print("Roll :");                                                 //Print text to screen

    digitalWrite(13, LOW);                                               //All done, turn the LED off

    loop_timer = micros();                                                //Reset the loop timer

    striscia.begin();
}

void loop(){

    striscia.setBrightness(analogRead(PIN_FOTORES)/4);

    read_mpu_6050_data();                                                //Read the raw acc and gyro data from the MPU-6050

    data.gyroX -= calibrationData.x;                                                //Subtract the offset calibration value from the raw gyro_x value
    data.gyroY -= calibrationData.y;                                                //Subtract the offset calibration value from the raw gyro_y value
    data.gyroZ -= calibrationData.z;                                                //Subtract the offset calibration value from the raw gyro_z value

    //Gyro angle calculations
    //0.0000611 = 1 / (250Hz / 65.5)
    angle.pitch += data.gyroX * 0.0000611;                                   //Calculate the traveled pitch angle and add this to the angle.pitch variable
    angle.roll += data.gyroY * 0.0000611;                                    //Calculate the traveled roll angle and add this to the angle.roll variable

    //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
    angle.pitch += angle.roll * sin(data.gyroZ * 0.000001066);               //If the IMU has yawed transfer the roll angle to the pitch angle
    angle.roll -= angle.pitch * sin(data.gyroZ * 0.000001066);               //If the IMU has yawed transfer the pitch angle to the roll angle

    //Accelerometer angle calculations
    data.accTot = sqrt((data.accX*data.accX)+(data.accY*data.accY)+(data.accZ*data.accZ));  //Calculate the total accelerometer vector
    //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
    angleAcc.pitch = asin((float)data.accY/data.accTot)* 57.296;       //Calculate the pitch angle
    angleAcc.roll = asin((float)data.accX/data.accTot)* -57.296;       //Calculate the roll angle

    //Place the MPU-6050 spirit level and note the values in the following two lines for calibration
    angleAcc.pitch -= 0.0;                                              //Accelerometer calibration value for pitch
    angleAcc.roll -= 0.0;                                               //Accelerometer calibration value for roll

    if(set_gyro_angles) {                                                 //If the IMU is already started
        angle.pitch = angle.pitch * 0.9996 + angleAcc.pitch * 0.0004;     //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
        angle.roll = angle.roll * 0.9996 + angleAcc.roll * 0.0004;        //Correct the drift of the gyro roll angle with the accelerometer roll angle
    }
    else {                                                                //At first start
        angle.pitch = angleAcc.pitch;                                     //Set the gyro pitch angle equal to the accelerometer pitch angle 
        angle.roll = angleAcc.roll;                                       //Set the gyro roll angle equal to the accelerometer roll angle 
        set_gyro_angles = true;                                            //Set the IMU started flag
    }

    //To dampen the pitch and roll angles a complementary filter is used
    angleOutput.pitch = angleOutput.pitch * 0.9 + angle.pitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
    angleOutput.roll = angleOutput.roll * 0.9 + angle.roll * 0.1;      //Take 90% of the output roll value and add 10% of the raw roll value

    write_LCD();                                                         //Write the roll and pitch values to the LCD display

    while(micros() - loop_timer < 4000);                                 //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
    loop_timer = micros();                                               //Reset the loop timer

    setLed(angleOutput.pitch, angleOutput.roll);                       //set the led colors and show it 
}


void setLed(float GyX, float GyY) {
    bool Xflat, Yflat;
    const int INCL_TOLL = 3;
    uint8_t i;
    
    Xflat = (GyX < INCL_TOLL && GyX > (INCL_TOLL*-1));
    Yflat = (GyY < INCL_TOLL && GyY > (INCL_TOLL*-1));
    if(Xflat && Yflat) //in bolla
    {
        for(int i = 0; i < 4; i++)
        {
            striscia.setPixelColor(i, striscia.Color(255, 0, 0));
        }
    }
    else //non in bolla
    {
        if(Xflat) //x in bolla
        {
            if(GyY >= INCL_TOLL) //y maggiore
            {
                striscia.setPixelColor(0, striscia.Color(0, 255, 0));
                striscia.setPixelColor(1, striscia.Color(0, 255, 0));
                striscia.setPixelColor(2, striscia.Color(0, 0, 255));
                striscia.setPixelColor(3, striscia.Color(0, 0, 255));
            }
            else //y minore
            {
                striscia.setPixelColor(0, striscia.Color(0, 0, 255));
                striscia.setPixelColor(1, striscia.Color(0, 0, 255));
                striscia.setPixelColor(2, striscia.Color(0, 255, 0));
                striscia.setPixelColor(3, striscia.Color(0, 255, 0));
            }
        }
        else if(Yflat) //y in bolla
        {
            if(GyX >= INCL_TOLL) //x maggiore
            {
                striscia.setPixelColor(0, striscia.Color(0, 0, 255));
                striscia.setPixelColor(1, striscia.Color(0, 255, 0));
                striscia.setPixelColor(2, striscia.Color(0, 255, 0));
                striscia.setPixelColor(3, striscia.Color(0, 0, 255));
            }
            else //x minore
            {
                striscia.setPixelColor(0, striscia.Color(0, 255, 0));
                striscia.setPixelColor(1, striscia.Color(0, 0, 255));
                striscia.setPixelColor(2, striscia.Color(0, 0, 255));
                striscia.setPixelColor(3, striscia.Color(0, 255, 0));
            }
        }
        else
        {
            if(GyX >= INCL_TOLL) //x maggiore
            {
                if(GyY >= INCL_TOLL) //y maggiore
                {
                    striscia.setPixelColor(0, striscia.Color(255, 0, 0));
                    striscia.setPixelColor(1, striscia.Color(0, 255, 0));
                    striscia.setPixelColor(2, striscia.Color(255, 0, 0));
                    striscia.setPixelColor(3, striscia.Color(0, 0, 255));
                }
                else //y minore
                {
                    striscia.setPixelColor(0, striscia.Color(0, 0, 255));
                    striscia.setPixelColor(1, striscia.Color(255, 0, 0));
                    striscia.setPixelColor(2, striscia.Color(0, 255, 0));
                    striscia.setPixelColor(3, striscia.Color(255, 0, 0));
                }
            }
            else //x minore
            {
                if(GyY >= INCL_TOLL) //y maggiore
                {
                    striscia.setPixelColor(0, striscia.Color(0, 255, 0));
                    striscia.setPixelColor(1, striscia.Color(255, 0, 0));
                    striscia.setPixelColor(2, striscia.Color(0, 0, 255));
                    striscia.setPixelColor(3, striscia.Color(255, 0, 0));
                }
                else //y minore
                {
                    striscia.setPixelColor(0, striscia.Color(255, 0, 0));
                    striscia.setPixelColor(1, striscia.Color(0, 0, 255));
                    striscia.setPixelColor(2, striscia.Color(255, 0, 0));
                    striscia.setPixelColor(3, striscia.Color(0, 255, 0));
                }
            }
        }
    }

    striscia.show();
}

void read_mpu_6050_data(){                                             //Subroutine for reading the raw gyro and accelerometer data
    Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
    Wire.write(0x3B);                                                    //Send the requested starting register
    Wire.endTransmission();                                              //End the transmission
    Wire.requestFrom(0x68,14);                                           //Request 14 bytes from the MPU-6050
    while(Wire.available() < 14);                                        //Wait until all the bytes are received
    data.accX = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_x variable
    data.accY = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_y variable
    data.accZ = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_z variable
    data.temp = Wire.read()<<8|Wire.read();                            //Add the low and high byte to the temperature variable
    data.gyroX = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_x variable
    data.gyroY = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_y variable
    data.gyroZ = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_z variable
}

void write_LCD(){                                                      //Subroutine for writing the LCD
    //To get a 250Hz program loop (4us) it's only possible to write one character per loop
    //Writing multiple characters is taking to much time
    if(lcd_loop_counter == 14) {
        lcd_loop_counter = 0;
    }                      //Reset the counter after 14 characters
    lcd_loop_counter ++;                                                 //Increase the counter
    if(lcd_loop_counter == 1) {
        angleBuffer.pitch = angleOutput.pitch * 10;                      //Buffer the pitch angle because it will change
        lcd.setCursor(6,0);                                                //Set the LCD cursor to position to position 0,0
    }
    if(lcd_loop_counter == 2){
        if(angleBuffer.pitch < 0) {
            lcd.print("-");
        }                          //Print - if value is negative
        else {
            lcd.print("+");
        }                                               //Print + if value is negative
    }
    if(lcd_loop_counter == 3) {
        lcd.print(abs(angleBuffer.pitch)/1000);
    }    //Print first number
    if(lcd_loop_counter == 4) {
        lcd.print((abs(angleBuffer.pitch)/100)%10);
    }//Print second number
    if(lcd_loop_counter == 5) {
        lcd.print((abs(angleBuffer.pitch)/10)%10);
    } //Print third number
    if(lcd_loop_counter == 6) {
        lcd.print(".");
    }                             //Print decimal point
    if(lcd_loop_counter == 7) {
        lcd.print(abs(angleBuffer.pitch)%10);
    }      //Print decimal number

    if(lcd_loop_counter == 8){
        angleBuffer.roll = angleOutput.roll * 10;
        lcd.setCursor(6,1);
    }
    if(lcd_loop_counter == 9){
        if(angleBuffer.roll < 0) {
            lcd.print("-");
        }                           //Print - if value is negative
        else {
            lcd.print("+");
        }                                               //Print + if value is negative
    }
    if(lcd_loop_counter == 10) {
        lcd.print(abs(angleBuffer.roll)/1000);
    }    //Print first number
    if(lcd_loop_counter == 11) {
        lcd.print((abs(angleBuffer.roll)/100)%10);
    }//Print second number
    if(lcd_loop_counter == 12) {
        lcd.print((abs(angleBuffer.roll)/10)%10);
    } //Print third number
    if(lcd_loop_counter == 13) {
        lcd.print(".");
    }                            //Print decimal point
    if(lcd_loop_counter == 14) {
        lcd.print(abs(angleBuffer.roll)%10);
    }      //Print decimal number
}

void setup_mpu_6050_registers(){
    //Activate the MPU-6050
    Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
    Wire.write(0x6B);                                                    //Send the requested starting register
    Wire.write(0x00);                                                    //Set the requested starting register
    Wire.endTransmission();                                              //End the transmission
    //Configure the accelerometer (+/-8g)
    Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
    Wire.write(0x1C);                                                    //Send the requested starting register
    Wire.write(0x10);                                                    //Set the requested starting register
    Wire.endTransmission();                                              //End the transmission
    //Configure the gyro (500dps full scale)
    Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
    Wire.write(0x1B);                                                    //Send the requested starting register
    Wire.write(0x08);                                                    //Set the requested starting register
    Wire.endTransmission();                                              //End the transmission
}
