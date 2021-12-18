#include <Wire.h>
#include <Adafruit_NeoPixel.h>

const __uint8_t MPU = 0x68; // I2C address of the MPU-6050
const __uint8_t PIN_STRISCIA = 4;   // Pin linea dati striscia
const __uint8_t N_LEDS = 4;
__INT16_TYPE__ AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

Adafruit_NeoPixel striscia = Adafruit_NeoPixel(N_LEDS, PIN_STRISCIA, NEO_RGB);

void setup()
{
    Wire.begin();
    Wire.beginTransmission(MPU);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission();    
    Serial.begin(9600);
    striscia.begin();
    striscia.show();
}

void loop()
{
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission();
    Wire.requestFrom(MPU, 14); // request a total of 14 registers (7*2)
    AcX = Wire.read() << 8 | Wire.read();
    AcY = Wire.read() << 8 | Wire.read();
    AcZ = Wire.read() << 8 | Wire.read();
    Tmp = Wire.read() << 8 | Wire.read();
    GyX = Wire.read() << 8 | Wire.read();
    GyY = Wire.read() << 8 | Wire.read();
    GyZ = Wire.read() << 8 | Wire.read();

    Serial.print("Accelerometer: ");
    Serial.print("X = "); Serial.print(AcX);
    Serial.print(" | Y = "); Serial.print(AcY);
    Serial.print(" | Z = "); Serial.println(AcZ);
    
    //equation for temperature in degrees C from datasheet
    Serial.print("Temperature: "); Serial.print(Tmp / 340.00 + 36.53); Serial.println(" C ");

    Serial.print("Gyroscope: ");
    Serial.print("X = "); Serial.print(GyX);
    Serial.print(" | Y = "); Serial.print(GyY);
    Serial.print(" | Z = "); Serial.println(GyZ);
    Serial.println(" ");
    delay(350);
}

