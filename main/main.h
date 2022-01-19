struct GyroscopeData {
    short signed int gyroX;
    short signed int gyroY;
    short signed int gyroZ;
    long signed int accX;
    long signed int accY;
    long signed int accZ;
    long signed int accTot;
    short signed int temp;
};
struct GyroscopeCalibration {
    long int x;
    long int y;
    long int z;
};
struct Angle {
    float pitch;
    float roll;
};
struct AngleBuffer {
    short signed int pitch;
    short signed int roll;
};