#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#define GYRO_MAX_VAL 32750  // Massimo valore positivo letto dal giroscopio
#define INCL_TOLL 100  // Tolleranza inclinazione

const uint8_t MPU = 0x68; // I2C address of the MPU-6050
const uint8_t PIN_STRISCIA = 4;   // Pin linea dati striscia
const uint8_t N_LEDS = 4; // Numero leds
__INT16_TYPE__ AcX, AcY, Tmp, GyX, GyY;
bool Xflat, Yflat;

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
    Wire.requestFrom(MPU, 10); // request a total of 14 registers (7*2)
    AcX = Wire.read() << 8 | Wire.read();
    AcY = Wire.read() << 8 | Wire.read();
    Tmp = Wire.read() << 8 | Wire.read();
    GyX = Wire.read() << 8 | Wire.read();
    GyY = Wire.read() << 8 | Wire.read();

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

    Xflat = (GyX < INCL_TOLL && GyX > (INCL_TOLL*-1));
    Yflat = (GyY < INCL_TOLL && GyY > (INCL_TOLL*-1));
    if(Xflat && Yflat) //in bolla
    {
        for(int i = 0; i < 4; i++)
        {
            striscia.setPixelColor(i, striscia.color(255, 0, 0));
        }
    }
    else //non in bolla
    {
        if(Xflat) //x in bolla
        {
            if(GyY >= INCL_TOLL) //y maggiore
            {
                striscia.setPixelColor(0, striscia.color(0, 0, 255));
                striscia.setPixelColor(3, striscia.color(0, 0, 255));
                striscia.setPixelColor(1, striscia.color(0, 255, 0));
                striscia.setPixelColor(2, striscia.color(0, 255, 0));
            }
            else //y minore
            {
                striscia.setPixelColor(1, striscia.color(0, 0, 255));
                striscia.setPixelColor(2, striscia.color(0, 0, 255));
                striscia.setPixelColor(0, striscia.color(0, 255, 0));
                striscia.setPixelColor(3, striscia.color(0, 255, 0));
            }
        }
        else if(Yflat) //y in bolla
        {
            if(GyX >= INCL_TOLL) //x maggiore
            {
                striscia.setPixelColor(2, striscia.color(0, 0, 255));
                striscia.setPixelColor(3, striscia.color(0, 0, 255));
                striscia.setPixelColor(0, striscia.color(0, 255, 0));
                striscia.setPixelColor(1, striscia.color(0, 255, 0));
            }
            else //x minore
            {
                striscia.setPixelColor(2, striscia.color(0, 0, 255));
                striscia.setPixelColor(3, striscia.color(0, 0, 255));
                striscia.setPixelColor(0, striscia.color(0, 255, 0));
                striscia.setPixelColor(1, striscia.color(0, 255, 0));
            }
        }
        else
        {
            if(GyX >= INCL_TOLL) //x maggiore
            {
                if(GyY >= INCL_TOLL) //y maggiore
                {
                    striscia.setPixelColor(0, striscia.color(255, 0, 0));
                    striscia.setPixelColor(1, striscia.color(0, 255, 0));
                    striscia.setPixelColor(2, striscia.color(255, 0, 0));
                    striscia.setPixelColor(3, striscia.color(0, 0, 255));
                }
                else //y minore
                {
                    striscia.setPixelColor(0, striscia.color(0, 255, 0));
                    striscia.setPixelColor(1, striscia.color(255, 0, 0));
                    striscia.setPixelColor(2, striscia.color(0, 0, 255));
                    striscia.setPixelColor(3, striscia.color(255, 0, 0));
                }
            }
            else //x minore
            {
                if(GyY >= INCL_TOLL) //y maggiore
                {
                    striscia.setPixelColor(0, striscia.color(0, 0, 255));
                    striscia.setPixelColor(1, striscia.color(255, 0, 0));
                    striscia.setPixelColor(2, striscia.color(0, 255, 0));
                    striscia.setPixelColor(3, striscia.color(255, 0, 0));
                }
                else //y minore
                {
                    striscia.setPixelColor(0, striscia.color(0, 0, 255));
                    striscia.setPixelColor(1, striscia.color(255, 0, 0));
                    striscia.setPixelColor(2, striscia.color(255, 0, 0));
                    striscia.setPixelColor(3, striscia.color(0, 255, 0));
                }
            }
        }
    }

    striscia.show();
    delay(350);
}