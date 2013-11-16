#ifndef SESSION_H
#define SESSION_H

#include "Includes.h"

// Function Prototypes
bool SetupPort (LPTSTR);
bool ConfPort (HWND*, LPTSTR);

// Vars
extern HANDLE hComm;

#endif