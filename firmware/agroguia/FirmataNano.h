/*
  FirmataNano.h - FirmataNano library
  Based on FirmaLite by Jeff Hoefs
*/

#ifndef FirmataNano_h
#define FirmataNano_h

#include <Arduino.h>
#include <Stream.h>
#include <inttypes.h>

#define MAX_DATA_BYTES 32 // max number of data bytes in non-Sysex messages

// message command bytes (128-255/0x80-0xFF)
#define START_SYSEX             0xF0 // start a Sysex message
#define END_SYSEX               0xF7 // end a Sysex message

extern "C" {
    // callback function types
    typedef void (*sysexCallbackFunction)(byte command, byte argc, byte*argv);
}

/**
 * FirmataNano Class
 */
class FirmataNano
{
  public:
      FirmataNano();
      void init(Stream *streamReference);
      void changeStream(Stream *streamReference);
      void refresh();
      void sendSysexMessage(byte command, byte bytec, byte* bytev);
      void attachCallback(sysexCallbackFunction newFunction);
      void detachCallback();

  private:
      Stream *stream;
      byte storedInputData[MAX_DATA_BYTES]; // multi-byte data
      boolean parsingSysex;
      int sysexBytesRead;
      sysexCallbackFunction currentSysexCallback;

      void processInput(void);
      void clearBuffer(void);
      void sendValueAsTwo7bitBytes(int value);
};

//extern FirmataNano FirmataNano;

#endif /* FirmataNano_h */
