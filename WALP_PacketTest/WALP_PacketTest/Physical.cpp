/*-----------------------------------------------------------------------------
--  SOURCE FILE:    Physical.cpp
--
--  PROGRAM:        Wireless Protocol (GRVM)
--
--  FUNCTIONS:      ReadSerialPort(HANDLE hComm, char packetBuffer[1024], DWORD dwBytesToRead,
--						LPDWORD lpdwBytesRead)
--					SendControl(HANDLE hComm, int controlType)
--                  SendData(HANDLE hComm, char* packet)
--  
--
--  DATE:           Nov. 22-24, 2013
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

OVERLAPPED ov = {0};
/*-----------------------------------------------------------------------------
-	FUNCTION:	ReadSerialPort
-
-	DATE:		November 23, 2013
-
-	REVISIONS:	...
-
-	DESIGNER:	Vincent Lau
-
-	PROGRAMMER:	Vincent Lau
-
-	INTERFACE:	BOOL ReadSerialPort(HANDLE hComm, char packetBuffer[1024], DWORD dwBytesToRead,
-					LPDWORD lpdwBytesRead, LPOVERLAPPED lpOV)
-		
-	RETURNS:	BOOL - TRUE on Reading sucess, FALSE if reading from serial port failed
-				
-	PARAMETERS: HANDLE hComm - Handle to the serial port
-				char packetBuffer[1024] - the buffer we want to fill and send back
-				DWORD dwBytesToRead - How many bytes are waiting to be read at the serial port
-				LPDWORD lpdwBytesRead - how many bytes were read, pass this back in case useful
-				
-
-	NOTES:	The physical function to read from the serial port
-
-----------------------------------------------------------------------------*/
BOOL ReadSerialPort(HANDLE hComm, char packetBuffer[1024], DWORD dwBytesToRead,
					LPDWORD lpdwBytesRead)
{
	if (!ReadFile(hComm, packetBuffer, dwBytesToRead, lpdwBytesRead, &ov))
	{
		return FALSE;
	}
	return TRUE;
}

/*-----------------------------------------------------------------------------
-	FUNCTION:	SendControl
-
-	DATE:		November 24, 2013
-
-	REVISIONS:	...
-
-	DESIGNER:	Vincent Lau
-
-	PROGRAMMER:	Vincent Lau
-
-	INTERFACE:	BOOL SendControl(HANDLE hComm, int controlType)
-
-	RETURNS:	BOOL -	TRUE if writing to the serial port was successful
-						FALSE if could not write to serial port
-				
-	PARAMETERS: HANDLE hComm	- Handle to the serial port
-				int controlType - Type of control packet to send. (ACK, ENQ, etc.)
-				LPOVERLAPPED lpOV - pointer to Overlapped structure for Event i/o
-
-	NOTES:	This function takes care of sending a simple 2-BYTE control packet
-			as per the design of the (BE CREATIVE) protocol.
-			
-			
-----------------------------------------------------------------------------*/
BOOL SendControl(HANDLE hComm, int controlType)
{
	DWORD dwBytesSent;
	char packetToSend[2] = {SYN, controlType};
	
	if (!WriteFile(hComm, packetToSend, PACKET_BYTES_CTL, &dwBytesSent, &ov))
		return FALSE;
	return TRUE;
}

/*-----------------------------------------------------------------------------
-	FUNCTION:	SendData
-
-	DATE:		November 23, 2013
-
-	REVISIONS:	...
-
-	DESIGNER:	Vincent Lau
-
-	PROGRAMMER:	Vincent Lau
-
-	INTERFACE:	BOOL SendData(HANDLE hComm, char packet[1024])
-
-	RETURNS:	BOOL -	TRUE if writing to the serial port was successful
-						FALSE if could not write to serial port
-				
-	PARAMETERS: HANDLE hComm - Handle to the serial port
-				char packet[1024] - the packet to send, including the 2B header
-									containing the control chars and 2B trailer
-									containing the CRC
-
-	NOTES:	Sending a data packet formed by a Packetize function. Packet should
-			already be filled in.
-
-----------------------------------------------------------------------------*/
BOOL SendData(HANDLE hComm, char packetToSend[1024])
{
	DWORD dwBytesSent;

	if (!WriteFile(hComm, packetToSend, PACKET_BYTES_TOTAL, &dwBytesSent, &ov))
		return FALSE;
	return TRUE;
}