#ifndef MAIN_H
#define MAIN_H

#include "Includes.h"
#include "Packet.h"
#include <commctrl.h>

#define UNTITLED TEXT ("(untitled)")
#define EDITID   1
#define WINDOW_HEIGHT	400
#define WINDOW_WIDTH	600
#define GENERATE_CRC_TEST_SIZE	10
#define ERROR_CHECK_TEST_SIZE	12
#define crc_ok  0x470F
// Function Prototypes
BOOL ErrorCheck(char*);
void GenerateCRC(char*, char*);
unsigned short crc16(char *data_p, unsigned short length);

// Variables
const LPTSTR lpszCommName	= TEXT("COM1");

#endif
