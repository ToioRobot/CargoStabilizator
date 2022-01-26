#include <SD.h>

struct Settings {
    short unsigned int ledStringPin, ledsNumber, photoresistorPin;
};

String readDataFromSD(String fileName);

Settings readSettings() {
    Settings settings;
    settings.ledStringPin = readDataFromSD("ledStringPin.config").toInt();
    settings.ledsNumber = readDataFromSD("ledsNumber.config").toInt();
    settings.photoresistorPin = readDataFromSD("photoresistorPin.config").toInt();
    return settings;
}

String readDataFromSD(String fileName) {
    String data;
    File dataFile = SD.open(fileName);
    if(dataFile) {
        while (dataFile.available()) {
            data = data + char(dataFile.read());
        }
        dataFile.close();
    }
    return data;
}