/*-----------------------------------------------------------------------------
--  SOURCE FILE:    Transport.cpp
--
--  PROGRAM:        Wireless Protocol (GRVM)
--
--  FUNCTIONS:      void Transmit(char* lpszFileBuffer)
--                  bool PacketCheck(char[1024] packet)
--  
--
--  DATE:			Nov. 16th, 2013
--
--  REVISIONS:      ...
--
--  DESIGNER:       Vincent Lau
--
--  PROGRAMMER:     
--
--  NOTES:			
--		Provides the logic for data transfer between the computer and the 
--		wireless device (which would be transmitting to the other wireless 
--		device conneceted to the other computer). Error checking will be done
--		via open-source CRC.
-----------------------------------------------------------------------------*/

#include "Transport.h"
int waitForType = NUL;
int sentPacketCounter = 0;	/* Counter to keep track of our file location as
							 as well as how many packets we've sent. */
/*-----------------------------------------------------------------------------
-	FUNCTION:	Transmit
-
-	DATE:		November 21st, 2013
-
-	REVISIONS:	...
-
-	DESIGNER:	Vincent Lau
-
-	PROGRAMMER:	Vincent Lau
-
-	INTERFACE:	void Transmit (char* file)
-		
-	RETURNS:	void, nothing.
-				
-	PARAMETERS:	LPSTR* file - a dynamic char buffer that contains the whole file
-							we are trying to send over the serial port.
-
-	NOTES:	This is the function that contains the logic for transmitting data
-			and following those rules for doing so, such as the upper send limit
-			per round (5 packets per round in the (BE CREATIVE) Protocol).
-			It will stop when it has sent one packet out and wait for a response.
-			
-
-----------------------------------------------------------------------------*/

void Transmit(LPSTR* file)
{
	char*	packetToSend = "A";
	BOOL	bDoneSending = FALSE;

	do
	{

//***************************HAD TO COMMENT THIS CODE OUT TO GET IT RUNNING**********************************//
		//packetToSend = Packetize(file, sentPacketCounter);
		while (sentPacketCounter % 5 != 0)
		{
					
			while(1) // Send packet to serial port
			{	
				if(SendData(hComm, packetToSend))
					break;
			}
			// Set "What we're waiting for" to ACK
			waitForType = ACK;

			// semaphore decrement (minus 1, should try to equal -1 and block)
			WaitForSingleObject(hACKWaitSemaphore, INFINITE);

			// Increment sent counter
			++sentPacketCounter;
		}
		// Another semaphore to determine when to start again.
	} while(!bDoneSending); //file not done
}


/*-----------------------------------------------------------------------------
-	FUNCTION:	ReceiveThread
-
-	DATE:		November 22st, 2013
-
-	REVISIONS:	...
-
-	DESIGNER:	Vincent Lau
-
-	PROGRAMMER:	Vincent Lau
-
-	INTERFACE:	DWORD WINAPI ReceiveThread (LPVOID lphwnd)
-		
-	RETURNS:	DWORD - a success or failure value on thread exit.
-				
-	PARAMETERS:	LPVOID lphwnd - reference to the main window's handle (HWND)
-
-	NOTES:	This is the primary thread for receiving packets from the serial
-			port. It runs forever until the program exits, but does not run its
-			read loop until the port has been configured. 
-			This is to be fully event-driven for serial ports. 
-			References were used, mainly the event-driven tty given earlier.
-
-----------------------------------------------------------------------------*/
DWORD WINAPI ReceiveThread(LPVOID lphwnd)
{
	char packetBuffer[1024];
	DWORD nBytesRead = 0, dwEvent, dwError;
	COMSTAT cs;

	// Set our Listening/Reading parameter for the serial port, want CHAR events
	SetCommMask(hComm, EV_RXCHAR);

	while (1) // forever
	{
		while (bWantToRead) // while we want to read
		{
			// wait for event
			if (WaitCommEvent(hComm, &dwEvent, NULL))
			{
				// Comm Event happened!
				// reset the comm errors
				ClearCommError(hComm, &dwError, &cs);
				if ((dwEvent & EV_RXCHAR) && cs.cbInQue)
				{
					// read from serial port
					if(!ReadSerialPort(hComm, packetBuffer, cs.cbInQue, &nBytesRead))
					{
						// error
					}
					else
					{
						// read success
						// PacketCheck
					}
				}
			}
		}
		Sleep(250);
	}

	ExitThread(EXIT_SUCCESS);
}