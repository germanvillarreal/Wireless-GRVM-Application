#ifndef PACKET_H
#define PACKET_H

#include "Includes.h"
#include "Vars.h"
#include "Physical.h"
#include "Main.h"
#include "Presentation.h"

#define DATA_PACKET			6000
#define CTL_PACKET			6001

#define PACKET_BYTES_CTL	2
#define PACKET_BYTES_DATA	1020
#define PACKET_BYTES_CRC	2
#define PACKET_BYTES_TOTAL	PACKET_BYTES_CTL + PACKET_BYTES_DATA + PACKET_BYTES_CRC




// Function Prototypes
BOOL Packetize(CHAR*, int);
BOOL PacketCheck(HWND, char[1024]);


// vars
extern HANDLE hWaitForLineSemaphore;
extern HANDLE hACKWaitSemaphore;
extern HANDLE hFileWaitSemaphore;
extern HANDLE hComm;
extern INT waitForType;
extern BOOL bENQToSend;
extern BOOL bENQReceived;
extern BOOL bWantLine;
extern LPSTR packetToSend;
extern LPSTR displayBuffer;

#endif