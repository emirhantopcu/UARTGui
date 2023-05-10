#ifndef SERIAL_H
#define SERIAL_H


#include <windows.h>
#include <iostream>
#include <stdio.h>

using namespace std;


class Serial
{
public:
	Serial(LPCWSTR port);
	~Serial();
	void send(uint8_t* buffer, uint8_t len);
	uint8_t* receive();
	HANDLE hSerial;

private:
	LPCWSTR port;
	//HANDLE hSerial;
};

#endif
