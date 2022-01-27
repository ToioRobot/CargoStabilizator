#include <SD.h>
#include "../main/main.h"

Settings settings;

void setup() {
    Serial.begin(9600);
    settings = readSettings();
    Serial.println(settings.ledsNumber);
    Serial.println(settings.ledStringPin);
    Serial.println(settings.photoresistorPin);
    Serial.println(settings.buzzerPin);
}

void loop() {
}