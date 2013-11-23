#ifndef _TRANSPORTS_H_
#define _TRANSPORTS_H_
#include "Includes.h"
#include "Vars.h"
#include "Packet.h"

// Function prototypes
void Transmit(char*);
BOOL PacketCheck(HWND, char[1024], int*);
DWORD WINAPI ReceiveThread	(LPVOID);

// Vars
extern int waitForType;
extern HANDLE hACKWaitSemaphore;
extern HANDLE hComm;
extern bool bWantToRead;
#endif
