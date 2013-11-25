#ifndef PACKET_H
#define PACKET_H

#include "Includes.h"
#include "Vars.h"

#define DATA_PACKET			6000
#define CTL_PACKET			6001

#define PACKET_BYTES_CTL	2
#define PACKET_BYTES_DATA	1020
#define PACKET_BYTES_TOTAL	PACKET_BYTES_CTL + PACKET_BYTES_DATA



// Function Prototypes
CHAR* Packetize(CHAR*, int, BOOL*);
BOOL PacketCheck(HWND, char[1024], int*);

// vars
extern HANDLE hACKWaitSemaphore;

#endif