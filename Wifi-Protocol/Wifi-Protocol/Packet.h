#ifndef PACKET_H
#define PACKET_H

#include "Includes.h"

#define DATA_PACKET			6000
#define CTL_PACKET			6001

#define PACKET_BYTES_CTL	2
#define PACKET_BYTES_DATA	1020
#define PACKET_BYTES_TOTAL	PACKET_BYTES_CTL + PACKET_BYTES_DATA

#define DC1		9
#define DC2		9

// Function Prototypes
CHAR* packetize(FILE*, int);
BOOL PacketCheck(HWND, char[1024]);

#endif