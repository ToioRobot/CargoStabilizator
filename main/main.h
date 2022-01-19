#include <Wire.h>

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
    return data;
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