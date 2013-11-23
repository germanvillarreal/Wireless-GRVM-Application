#ifndef PHYSICAL_H
#define PHYSICAL_H
#include "Includes.h"
#include "Vars.h"
#include "Packet.h"

// Function Prototypes
BOOL ReadSerialPort(HANDLE, char[1024], DWORD cbInQue, LPDWORD);
BOOL SendControl(HANDLE, int);
BOOL SendData (HANDLE, char [1024]);

#endif