#ifndef SESSION_H
#define SESSION_H

#include "Includes.h"

// Function Prototypes
bool SetupPort (LPTSTR);
bool ConfigurePort (HWND*, TCHAR*);

// Vars
extern HANDLE hComm;

#endif