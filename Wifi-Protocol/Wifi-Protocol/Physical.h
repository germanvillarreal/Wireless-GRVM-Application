#ifndef PHYSICAL_H
#define PHYSICAL_H
#include "Includes.h"

// Function Prototypes
BOOL ReadSerialPort(HANDLE, &char[1024], DWORD cbInQue, LPDWORD);
BOOL WriteToSerialPort(char*);

#endif