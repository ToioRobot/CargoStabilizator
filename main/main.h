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