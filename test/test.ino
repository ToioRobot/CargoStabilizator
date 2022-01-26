#include <SD.h>
#include "main.h"

Settings settings;

void setup() {
    SD.begin(4);
    settings = readSettings();
    Serial.begin(9600);
    Serial.println(settings.ledsNumber);
    Serial.println(settings.ledStringPin);
    Serial.println(settings.photoresistorPin);
}

void loop() {
}