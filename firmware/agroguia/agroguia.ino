// Import libraries
#include <SoftwareSerial.h>
#include "Utils.h"
#include "FirmataNano.h"

// Definition of used pins
#define PIN_SWITCH  2
#define PIN_TX      8
#define PIN_RX      9

// Protocol requests
#define PROTOCOL_CMD_REQUEST_SWITCH_STATE       0x01

// Protocol responses
#define PROTOCOL_CMD_REPORT_SWITCH_STATE        0x01


// Creates software serial for wifly module communication
SoftwareSerial wifiSerial = SoftwareSerial(PIN_RX, PIN_TX);

// Switch state variable modified only from interrupt handler
volatile uint8_t switchState = LOW;

FirmataNano firmataProcessor = FirmataNano();


// Setup
void setup()
{
  // Start serial ports
  Serial.begin(9600);
  wifiSerial.begin(9600);

  firmataProcessor.init((Stream*)&wifiSerial);
  firmataProcessor.attachCallback(sysexCallback);

  // Set pinmode for switch
  pinMode(PIN_SWITCH, INPUT);

  // Read initial state for switch
  switchState = digitalRead(PIN_SWITCH);

  // Attach change interrupts on pin 2 (INT0)
  attachInterrupt(0, onSwitchChange, CHANGE);
}

void loop()
{
  firmataProcessor.refresh();
}

void terminal()
{
    Serial.println("Terminal ready");
    while (1) {
      if (wifiSerial.available() > 0) {
          Serial.write(wifiSerial.read());
      }
      if (Serial.available()) {
          wifiSerial.write(Serial.read());
      }
    }
}

void onSwitchChange()
{
  uint8_t newSwitchState = digitalRead(PIN_SWITCH);
  if (newSwitchState != switchState) {
    switchState = newSwitchState;
    reportSwitchState();
  }
}

void reportSwitchState()
{
    // Prepare message
    uint8_t data[1] = {
        switchState
    };

    firmataProcessor.sendSysexMessage(PROTOCOL_CMD_REPORT_SWITCH_STATE, 1, data);
}

void sysexCallback(byte command, byte argc, byte *argv)
{
    switch(command)
    {
            case PROTOCOL_CMD_REQUEST_SWITCH_STATE:
                reportSwitchState();
                break;

            default: break;
    }
}






