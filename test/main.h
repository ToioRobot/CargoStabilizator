#include <SD.h>

struct Settings {
    short unsigned int ledStringPin, ledsNumber, photoresistorPin;
};

String readDataFromSD(String fileName);

Settings readSettings() {
    Settings settings;
    settings.ledStringPin = readDataFromSD("lpin.txt").toInt();
    settings.ledsNumber = readDataFromSD("ledn.txt").toInt();
    settings.photoresistorPin = readDataFromSD("ppin.txt").toInt();
    return settings;
}

String readDataFromSD(String fileName) {
    String data;
    if(SD.begin(4)) {
        File dataFile = SD.open(fileName, FILE_READ);
        if(dataFile) {
            while (dataFile.available()) {
                data = data + char(dataFile.read());
            }
            dataFile.close();
        }
    }
    return data;
}