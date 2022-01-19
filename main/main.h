#include <Wire.h>
#include <LiquidCrystal_I2C.h>

struct GyroscopeData { // Data read from gyroscope
    short signed int gyroX;
    short signed int gyroY;
    short signed int gyroZ;
    long signed int accX;
    long signed int accY;
    long signed int accZ;
    long signed int accTot;
    short signed int temp;
};
struct GyroscopeCalibration { // Data for calibration
    long int x;
    long int y;
    long int z;
};
struct Angle { // General angle
    float pitch;
    float roll;
};
struct AngleBuffer { // Angle buffer
    short signed int pitch;
    short signed int roll;
};

//Declaring Leds const
const uint8_t PIN_STRISCIA = 9; // Pin led string
const uint8_t N_LEDS = 4; // Led number
const uint8_t PIN_FOTORES = A0; // Pin photoresistor

GyroscopeData read_mpu_6050_data() { // Subroutine for reading the raw gyro and accelerometer data
    GyroscopeData data;
    Wire.beginTransmission(0x68);
    Wire.write(0x3B);
    Wire.endTransmission();
    Wire.requestFrom(0x68,14); // Request 14 bytes from the MPU-6050
    while(Wire.available() < 14); // Wait until all the bytes are received
    data.accX = Wire.read()<<8|Wire.read();
    data.accY = Wire.read()<<8|Wire.read();
    data.accZ = Wire.read()<<8|Wire.read();
    data.temp = Wire.read()<<8|Wire.read();
    data.gyroX = Wire.read()<<8|Wire.read();
    data.gyroY = Wire.read()<<8|Wire.read();
    data.gyroZ = Wire.read()<<8|Wire.read();
    return data;
}

void setup_mpu_6050_registers() {
    Wire.beginTransmission(0x68);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.beginTransmission(0x68);
    Wire.write(0x1C);
    Wire.write(0x10);
    Wire.endTransmission();
    Wire.beginTransmission(0x68);
    Wire.write(0x1B);
    Wire.write(0x08);
    Wire.endTransmission();
}

void write_LCD(int *lcdLoopCounter, AngleBuffer *angleBuffer, Angle *angleOutput) { // Subroutine for writing the LCD
    if(*lcdLoopCounter == 14) {
        *lcdLoopCounter = 0;
    }
    *lcdLoopCounter++;
    switch(*lcdLoopCounter) {
        case 1:
            angleBuffer->pitch = angleOutput->pitch * 10;
            lcd.setCursor(6,0);
            break;
        case 2:
            if(angleBuffer->pitch < 0) {
                lcd.print("-");
            }
            else {
                lcd.print("+");
            }
            break;
        case 3:
            lcd.print(abs(angleBuffer->pitch)/1000);
            break;
        case 4:
            lcd.print((abs(angleBuffer->pitch)/100)%10);
            break;
        case 5:
            lcd.print((abs(angleBuffer->pitch)/10)%10);
            break;
        case 6:
            lcd.print(".");
            break;
        case 7:
            lcd.print(abs(angleBuffer->pitch)%10);
            break;
        case 8:
            angleBuffer->roll = angleOutput->roll * 10;
            lcd.setCursor(6,1);
            break;
        case 9:
            if(angleBuffer->roll < 0) {
                lcd.print("-");
            }
            else {
                lcd.print("+");
            }
            break;
        case 10:
            lcd.print(abs(angleBuffer->roll)/1000);
            break;
        case 11:
            lcd.print((abs(angleBuffer->roll)/100)%10);
            break;
        case 12:
            lcd.print((abs(angleBuffer->roll)/10)%10);
            break;
        case 13:
            lcd.print(".");
            break;
        case 14:
            lcd.print(abs(angleBuffer->roll)%10);
            break;
        default:
    }
}