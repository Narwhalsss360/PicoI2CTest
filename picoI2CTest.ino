#include "NewWire.h"

#define WIRE_ADDR 0x10
#define HIGH_SPEED_MODE 3400000
#define DEFAULT_SPEED_MODE 100000
#define LED_PIN 11
#define reinterpret_c_style(origin) *((unsigned long*)origin)
#define LED_PIN 11

#define NBL 5
#define VL 4

byte wireBuffer[NBL];
uint32_t val;
byte txLength = 0;
byte txAddr = 0;
uint32_t dAddr[127];
void (*recv) (byte, uint32_t) = NULL;

NewWireSlave WireSlave(DEFAULT_NWD_LENGTH);
NewWireData lastRecv;
NewWireData LED_DATA = { 1, 0 };
uint64_t last = 0;

void afterParse(byte address, uint32_t data)
{

}

void getVal(const byte* buf)
{
    byte start[VL];
    for (size_t i = 0; i < VL; i++)
    {
        start[i] = buf[i + 1];
    }
    val = reinterpret_c_style(start);
    if (recv != NULL)
    {
        recv(buf[0], val);
    }
}

void printBuffer()
{
    Serial.println("RECV:");
    for (size_t i = 0; i < NBL; i++)
    {
        Serial.print("    ");
        Serial.print("[");
        Serial.print(i);
        Serial.print("]:");
        Serial.println(wireBuffer[i]);
    }
    Serial.print("    VAL: ");
    Serial.println(val);
}

void clearBuffer()
{
    for (size_t i = 0; i < NBL; i++)
    {
        wireBuffer[i] = 0;
    }
}

void WireOnReceive(int bytes)
{
    clearBuffer();
    for (size_t i = 0; i < NBL; i++)
    {
        if (!Wire.available())
            break;
        byte t = Wire.read();
        wireBuffer[i] = t;
    }
    getVal(wireBuffer);
    if (wireBuffer[0] == 255)
    {
        txAddr = wireBuffer[1];
    }
}

void WireOnRequest()
{
    byte bytes[4];
    memcpy(bytes, &dAddr[txAddr], 4);
    Wire.write(bytes, 4);
}

void NWRecv(int bytes)
{
    lastRecv = WireSlave.onReceive();
    if (!INVALID_NWD(lastRecv))
    {
        Serial.print("RECV:\n    ADDRESS:");
        Serial.println(lastRecv.address);
        Serial.print("    DATA:");
        Serial.println(lastRecv.data);
        Serial.println();
    }
}

void NWReq()
{
    WireSlave.onRequest();
}

void setup()
{
    Wire.onReceive(NWRecv);
    Wire.onRequest(NWReq);
    Wire.begin(WIRE_ADDR);
    Serial.begin(1000000);
    recv = afterParse;
    dAddr[23] = 123;
    NewWireData toSend = { 3, 3323 };
    WireSlave.add(toSend);
    pinMode(LED_PIN, OUTPUT);
}

void loop2()
{
    if (millis() - last >= 3000)
    {
        last = millis();
        NewWireData nwd = { 4, 0 };
        if (WireSlave.getData(&nwd))
        {
            Serial.print("LOOP:\n    ADDRESS:");
            Serial.println(nwd.address);
            Serial.print("    DATA:");
            Serial.println(nwd.data);
            Serial.println();
        }
        else
        {
            Serial.println("NO DATA");
        }
    }
}

void loop3()
{
    if (millis() - last >= 3000)
    {
        last = millis();
        uint8_t index = WireSlave.searchSend(3);
        if (index < WireSlave.nwdLength)
        {
            Serial.print("Found at index: ");
            Serial.println(index);
        }
        else
        {
            Serial.println("Not found");
        }
    }
}

void loop4()
{
    if (WireSlave.getData(&LED_DATA))
    {
        digitalWrite(LED_PIN, LED_DATA.data);
    }
}

void loop()
{
    loop4();
}