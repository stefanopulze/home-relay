#include "command.h"

extern int RELAYS[];

bool handleCommand(Command cmd)
{
    int pin = RELAYS[cmd.relay];
    short currentState = (bool)digitalRead(pin);
    short desireState = (bool)(cmd.turn == Turn::On ? LOW : HIGH);

    if (cmd.turn == Turn::Toggle)
    {
        desireState = !currentState;
    }

    // Normal open configuration, the state is inverse
    Serial.printf("Current State: %d\n", !currentState);
    Serial.printf("Desiderate State: %d", !desireState);
    if (cmd.timer > 0)
    {
        Serial.printf(" for: %d millisec", cmd.timer);
    }
    Serial.println();

    digitalWrite(pin, desireState);
    if (cmd.timer > 5)
    {
        delay(cmd.timer);
        digitalWrite(pin, currentState);
    }

    return !(bool)digitalRead(pin);
}

Command parseRelayCommand(short relay, DynamicJsonDocument json)
{
    if (relay < 0 || relay > 3)
    {
        throw "Invalid 'relay' value. Valid values from 0 to 3";
    }

    Command cmd;
    cmd.relay = relay;

    String turn = json["turn"].as<String>();
    if (turn == "on")
    {
        cmd.turn = Turn::On;
    }
    else if (turn == "off")
    {
        cmd.turn = Turn::Off;
    }
    else if (turn == "toggle")
    {
        cmd.turn = Turn::Toggle;
    }
    else
    {
        throw "Invalid 'turn' value. Valid values: 'on', 'off', 'toggle'";
    }

    if (json.containsKey("timer"))
    {
        cmd.timer = json["timer"].as<uint16>();
    } else {
        cmd.timer = 0;
    }

    // #ifdef DEBUG
    cmd.printToSerial();
    // #endif // DEBUG

    return cmd;
}