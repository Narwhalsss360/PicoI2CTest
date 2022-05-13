#include "NewWire.h"

NewWireSlave::NewWireSlave(uint8_t size)
{
    uint8_t newSize = size * NWD_SIZE;
    recvData = (NewWireData*)malloc(newSize);
    sendData = (NewWireData*)malloc(newSize);
    if (recvData == NULL || sendData == NULL)
        return;
    else
        nwdLength = size;
}

void NewWireSlave::clearMainBuffer()
{
    for (uint8_t i = 0; i < MAIN_BUFFER_SIZE; i++)
    {
        mainBuffer[i] = 0;
    }
}

uint32_t NewWireSlave::bytesToValue()
{
    byte start[DATA_SIZE];
    for (uint8_t i = 0; i < DATA_SIZE; i++)
    {
        start[i] = mainBuffer[i + 1];
    }
    return reinterpret_c_style(start);
}

uint8_t NewWireSlave::searchSend(uint8_t address)
{
    for (uint8_t i = 0; i < nwdLength; i++)
    {
        if (sendData[i].address == address)
            return i;
    }
    return nwdLength;
}

uint8_t NewWireSlave::searchRecv(uint8_t address)
{
    for (uint8_t i = 0; i < nwdLength; i++)
    {
        if (recvData[i].address == address)
            return i;
    }
    return nwdLength;
}

NewWireData NewWireSlave::onReceive()
{
    clearMainBuffer();
    for (uint8_t i = 0; i < MAIN_BUFFER_SIZE; i++)
    {
        if (!Wire.available())
            break;
        mainBuffer[i] = Wire.read();
    }
    printBuffer("AFTER:");
    if (mainBuffer[0] == 255)
    {
        txAddress = mainBuffer[1];
        return {0, 0};
    }
    else
    {
        uint8_t index = searchRecv(mainBuffer[0]);
        if (index < nwdLength)
        {
            recvData[index].address = mainBuffer[0];
            recvData[index].data = bytesToValue();
            lastError = 0;
            return recvData[index];
        }
        else if (recvDataPointer == nwdLength - 1)
            recvDataPointer = 0;
        else
            recvDataPointer++;
        recvData[recvDataPointer].address = mainBuffer[0];
        recvData[recvDataPointer].data = bytesToValue();
        lastError = 0;
        return recvData[recvDataPointer];
    }
    lastError = LOSS;
}

void NewWireSlave::onRequest()
{
    byte bytes[DATA_SIZE] = { 0 };
    uint8_t index = searchSend(txAddress);
    if (index < nwdLength)
    {
        memcpy(bytes, &sendData[index].data, DATA_SIZE);
        Wire.write(bytes, DATA_SIZE);
        lastError = 0;
    }
    else
    {
        Wire.write(bytes, DATA_SIZE);
        lastError = ADDRESS_NOT_FOUND;
    }
}

bool NewWireSlave::getData(NewWireData* nwd)
{
    uint8_t index = searchRecv(nwd->address);
    if (index < nwdLength)
    {
        *nwd = recvData[index];
        return true;
    }
    return false;
}

void NewWireSlave::add(NewWireData nwd)
{
    if (sendDataPointer == nwdLength - 1)
        sendDataPointer = 0;
    else
        sendDataPointer++;
    sendData[sendDataPointer] = nwd;
}

void NewWireSlave::printBuffer(String title)
{
    Serial.println(title);
    for (size_t i = 0; i < MAIN_BUFFER_SIZE; i++)
    {
        Serial.print("    ");
        Serial.print("[");
        Serial.print(i);
        Serial.print("]:");
        Serial.println(mainBuffer[i]);
    }
}