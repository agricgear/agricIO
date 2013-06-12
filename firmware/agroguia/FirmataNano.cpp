/*
  FirmataNano.h - FirmataNano library
  Based on Firmalite by Jeff Hoefs
*/
#include "Arduino.h"
#include "Stream.h"
#include "FirmataNano.h"

// extern "C" {
//   #include <string.h>
//   #include <stdlib.h>
// }

/**
 * Class constructor
 */
FirmataNano::FirmataNano()
{
  parsingSysex = false;
  sysexBytesRead = 0;
  clearBuffer();
}

/**
 * Initialize
 *
 * @param streamReference Reference to the serial port
 *
 * @public
 */
void FirmataNano::init(Stream *streamReference)
{
  stream = streamReference;
}

/**
 * Changes the stream
 *
 * @param streamReference Reference to the serial port
 *
 * @public
 */
void FirmataNano::changeStream(Stream *streamReference){
  stream = streamReference;
}

/**
 * Look for new bytes available to process
 *
 * @public
 */
void FirmataNano::refresh(void){

  while(stream->available()) processInput();
}

/**
 * Sends a sysex command of bytes
 *
 * @param command [description]
 * @param bytec   [description]
 * @param bytev   [description]
 *
 * @public
 */
void FirmataNano::sendSysexMessage(byte command, byte bytec, byte* bytev)
{
  byte i;

  stream->write(START_SYSEX);
  stream->write(command);
  for(i=0; i<bytec; i++) {
    sendValueAsTwo7bitBytes(bytev[i]);
  }

  stream->write(END_SYSEX);
}

/**
 * Attach the 'sysex command received' function callback
 *
 * @param command     [description]
 * @param newFunction [description]
 *
 * @public
 */
void FirmataNano::attachCallback(sysexCallbackFunction newFunction)
{
  currentSysexCallback = newFunction;
}

/**
 * Detach callback
 *
 * @param command [description]
 *
 * @public
 */
void FirmataNano::detachCallback()
{
    currentSysexCallback = NULL;
}

/**
 * Process input from stream
 * Called from method refresh when data is available
 *
 * @private
 */
void FirmataNano::processInput(void)
{
  byte read = stream->read();

  //Serial.write(read);

  if(parsingSysex && sysexBytesRead < MAX_DATA_BYTES){

    if(read == START_SYSEX){
       // a new command start
       // clear buffer
       clearBuffer();
       parsingSysex = true;
       sysexBytesRead = 1;
       storedInputData[0] = read;
    }
    else if (read == END_SYSEX){

      storedInputData[sysexBytesRead] = read;
      sysexBytesRead++;

      int command = storedInputData[1];

      byte numOfValues = (sysexBytesRead-3)/2;
      byte values[numOfValues];

      int valuesIndex = 0;
      int storedInputDataIndex = 1;

      while(valuesIndex < numOfValues){
        int LSB = storedInputData[storedInputDataIndex];
        int MSB = storedInputData[storedInputDataIndex+1];

        values[valuesIndex] = MSB*128+LSB;
        valuesIndex++;
        storedInputDataIndex = storedInputDataIndex+2;
      }

      if(currentSysexCallback)
        (*currentSysexCallback)(command, numOfValues, values);

      clearBuffer();
      parsingSysex = false;
      sysexBytesRead = 0;
    }
    else {
      storedInputData[sysexBytesRead] = read;
      sysexBytesRead++;
    }


  } else {

    if(read == START_SYSEX){
       parsingSysex = true;
       clearBuffer();
       sysexBytesRead = 1;
       storedInputData[0] = read;
    } else {
      // waste this bytes if not in a sysex parsing
      //sysexBytesRead = 0;
    }
  }
}

/**
 * Sends values in two bytes of 7 bits
 *
 * @param value [description]
 *
 * @private
 */
void FirmataNano::sendValueAsTwo7bitBytes(int value)
{
  stream->write(value & B01111111); // LSB
  stream->write(value >> 7 & B01111111); // MSB
}

// resets the system state upon a SYSTEM_RESET message from the host software
/**
 * Resets the state when a SYSTEM_RESET message is received
 */
void FirmataNano::clearBuffer(void)
{
  byte i;

  for(i=0; i<MAX_DATA_BYTES; i++) {
    storedInputData[i] = 0;
  }
}

