#ifndef NewWire
#define NewWire

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#ifndef TwoWire_h
#include <Wire.h>
#endif

#define reinterpret_c_style(origin) *((unsigned long*)origin)

#define MAIN_BUFFER_SIZE 5
#define DATA_SIZE 4
#define NWD_SIZE sizeof(NewWireData)
#define DEFAULT_NWD_LENGTH 32
#define INVALID_NWD(nwd) (nwd.address == 0 && nwd.data == 0) ? true : false

enum NWERRORS
{
    ADDRESS_NOT_FOUND = 1,
    LOSS
};

struct NewWireData
{
    uint8_t address;
    uint32_t data;
};

class NewWireSlave
{
public:
    NewWireData* recvData;
    NewWireData* sendData;
    uint8_t recvDataPointer;
    uint8_t sendDataPointer;
    uint8_t nwdLength;
    uint8_t txAddress;
    uint8_t mainBuffer[5];

    uint8_t lastError;
    void clearMainBuffer();
    uint32_t bytesToValue();
    uint8_t searchSend(uint8_t);
    uint8_t searchRecv(uint8_t);

    NewWireSlave();
    NewWireSlave(uint8_t);
    NewWireData onReceive();
    void onRequest();
    bool getData(NewWireData*);
    void add(NewWireData);
    uint8_t getLastError() const { return lastError; }
    void printBuffer(String);
};
#endif