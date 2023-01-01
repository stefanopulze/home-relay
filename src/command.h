#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

enum Turn { On, Off, Toggle};

struct Command {
    int relay;
    Turn turn;
    uint16 timer;

    void printToSerial() {
        Serial.printf("command: {relay:%d, turnOn: %d, timer: %d}\n", relay, turn, timer);
    }
};

bool handleCommand(Command cmd);

Command parseCommand(DynamicJsonDocument json);

Command parseRelayCommand(short relay, DynamicJsonDocument json);

