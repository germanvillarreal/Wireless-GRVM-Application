/*-----------------------------------------------------------------------------
--  SOURCE FILE:    Physical.cpp
--
--  PROGRAM:        Wireless Protocol (GRVM)
--
--  FUNCTIONS:      
--                              
--  
--
--  DATE:           
--
--  REVISIONS:      ...
--
--  DESIGNER:       
--
--  PROGRAMMER:     
--
--  NOTES:       
--		Provides the actual sending and receiving of data functionality.
--		
-----------------------------------------------------------------------------*/
#include "Physical.h"


BOOL ReadSerialPort(HANDLE hComm, char* packetBuffer[1024], DWORD dwBytesToRead,
					LPDWORD lpdwBytesRead)
{
	if (ReadFile(hComm, *packetBuffer, dwBytesToRead, lpdwBytesRead, NULL))
	{
		return TRUE;
	}
	return FALSE;
}