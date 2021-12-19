#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#define GYRO_MAX_VAL 32750  // Massimo valore positivo letto dal giroscopio
#define INCLINATION_TOLLERANCE 100  // Tolleranza inclinazione

const uint8_t MPU = 0x68; // I2C address of the MPU-6050
const uint8_t PIN_STRISCIA = 4;   // Pin linea dati striscia
const uint8_t N_LEDS = 4; // Numero leds
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
    uint8_t i;

    // TODO: togliere operazioni e variabili inutili
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
    
    // TODO: num pixels solamente indicativi
    // La struttura è una bozza, da ottimizzare
    if(GyX > INCLINATION_TOLLERANCE &&  GyY > INCLINATION_TOLLERANCE)   // Angolo 0 in alto
    {
        striscia.setPixelColor(0, striscia.Color(0, (255*GYRO_MAX_VAL*2)/(GyX + GYRO_MAX_VAL), (255*GYRO_MAX_VAL*2)/(GyY + GYRO_MAX_VAL))); // g,r,b CONTROLLA SE è VERO
    }
    else if(GyX > INCLINATION_TOLLERANCE && GyY < INCLINATION_TOLLERANCE*(-1))  // Angolo 1 in alto
    {
        striscia.setPixelColor(1, striscia.Color(0, (255*GYRO_MAX_VAL*2)/(GyX + GYRO_MAX_VAL), (255*GYRO_MAX_VAL*2)/(GyY + GYRO_MAX_VAL)));
    }
    else if(GyX < INCLINATION_TOLLERANCE*(-1) && GyY > INCLINATION_TOLLERANCE)  // Angolo 2 in alto
    {
        striscia.setPixelColor(2, striscia.Color(0, (255*GYRO_MAX_VAL*2)/(GyX + GYRO_MAX_VAL), (255*GYRO_MAX_VAL*2)/(GyY + GYRO_MAX_VAL)));
    }
    else if(GyX < INCLINATION_TOLLERANCE*(-1) && GyY < INCLINATION_TOLLERANCE*(-1)) // Angolo 3 in alto
    {
        striscia.setPixelColor(3, striscia.Color(0, (255*GYRO_MAX_VAL*2)/(GyX + GYRO_MAX_VAL), (255*GYRO_MAX_VAL*2)/(GyY + GYRO_MAX_VAL)));
    }
    else if(GyX > INCLINATION_TOLLERANCE)   // Lato 01 in alto
    {
        striscia.setPixelColor(0, striscia.Color(0, (255*GYRO_MAX_VAL*2)/(GyX + GYRO_MAX_VAL), (255*GYRO_MAX_VAL*2)/(GyY + GYRO_MAX_VAL)));
        striscia.setPixelColor(1, striscia.Color(0, (255*GYRO_MAX_VAL*2)/(GyX + GYRO_MAX_VAL), (255*GYRO_MAX_VAL*2)/(GyY + GYRO_MAX_VAL)));
    }
    else if(GyX < INCLINATION_TOLLERANCE*(-1))  // Lato 23 in alto
    {
        striscia.setPixelColor(2, striscia.Color(0, (255*GYRO_MAX_VAL*2)/(GyX + GYRO_MAX_VAL), (255*GYRO_MAX_VAL*2)/(GyY + GYRO_MAX_VAL)));
        striscia.setPixelColor(3, striscia.Color(0, (255*GYRO_MAX_VAL*2)/(GyX + GYRO_MAX_VAL), (255*GYRO_MAX_VAL*2)/(GyY + GYRO_MAX_VAL)));
    }
    else if(GyY > INCLINATION_TOLLERANCE)   // Lato 02 in alto
    {
        striscia.setPixelColor(0, striscia.Color(0, (255*GYRO_MAX_VAL*2)/(GyX + GYRO_MAX_VAL), (255*GYRO_MAX_VAL*2)/(GyY + GYRO_MAX_VAL)));
        striscia.setPixelColor(2, striscia.Color(0, (255*GYRO_MAX_VAL*2)/(GyX + GYRO_MAX_VAL), (255*GYRO_MAX_VAL*2)/(GyY + GYRO_MAX_VAL)));
    }
    else if(GyY < INCLINATION_TOLLERANCE*(-1))  // Lato 13 in alto
    {
        striscia.setPixelColor(1, striscia.Color(0, (255*GYRO_MAX_VAL*2)/(GyX + GYRO_MAX_VAL), (255*GYRO_MAX_VAL*2)/(GyY + GYRO_MAX_VAL)));
        striscia.setPixelColor(3, striscia.Color(0, (255*GYRO_MAX_VAL*2)/(GyX + GYRO_MAX_VAL), (255*GYRO_MAX_VAL*2)/(GyY + GYRO_MAX_VAL)));
    }
    else    // Tutto in bolla
    {
        for(i=0; i < N_LEDS; i++)
        {
            striscia.setPixelColor(i, striscia.Color(0, 0, 0));
        }
    }


    striscia.show();
    delay(350);
}

