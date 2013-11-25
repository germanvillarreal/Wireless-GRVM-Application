#ifndef PHYSICAL_H
#define PHYSICAL_H
#include "Includes.h"
#include "Vars.h"
#include "Packet.h"

// Function Prototypes
BOOL ReadSerialPort(HANDLE hComm, char packetBuffer[1024], DWORD dwBytesToRead, LPDWORD lpdwBytesRead, LPOVERLAPPED lpOV);
BOOL SendControl(HANDLE, int, LPOVERLAPPED);
BOOL SendData (HANDLE, char [1024], LPOVERLAPPED);

#endif