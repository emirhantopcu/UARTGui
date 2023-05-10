#include "Serial.h"

#include <windows.h>
#include <iostream>
#include <stdio.h>

Serial::Serial(LPCWSTR port)
{
    this->port = port;

    hSerial = CreateFile(port,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);

    if (hSerial == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            printf("Serial port does not exist.");
        }
        printf("Error1");
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        //error getting state
    }
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        //error setting serial port state


    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hSerial, &timeouts)) {
        //error occureed. Inform 
    }
}

Serial::~Serial()
{
}


void Serial::send(uint8_t* buffer, uint8_t len)
{



    DWORD dwBytesRead = 0;

    WriteFile(hSerial, buffer, len, &dwBytesRead, NULL);

}

uint8_t* Serial::receive() {


    DWORD dwBytesRead = 0;

    uint8_t buffer_len;

    int iResult;

    iResult = ReadFile(hSerial, &buffer_len, 0, &dwBytesRead, NULL);

    uint8_t len = buffer_len;

    uint8_t* buffer = new uint8_t[len];

    iResult = ReadFile(hSerial, buffer, len, &dwBytesRead, NULL);

    int msg_length = (int)buffer[0];



    uint8_t* msg = new uint8_t[msg_length];

    uint8_t temp;

    for (int i = 0; i < msg_length; i++)
    {
        temp = buffer[i];
        msg[i] = temp;
    }


    return msg;
}
