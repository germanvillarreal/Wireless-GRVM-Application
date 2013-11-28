/*-----------------------------------------------------------------------------
--  SOURCE FILE:    Transport.cpp
--
--  PROGRAM:        Wireless Protocol (GRVM)
--
--  FUNCTIONS:      void Transmit(LPSTR* lpszFileBuffer)
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
INT waitForType = ENQ;
INT sentPacketCounter = 0;	/* Counter to keep track of our file location as
							 as well as how many packets we've sent. */
BOOL bHaveFileToSend = FALSE;
BOOL bENQToSend	= FALSE;
BOOL bENQReceived = FALSE;
BOOL bWantLine = FALSE;
LPSTR	packetToSend;		/* Global packet buffer */

/*-----------------------------------------------------------------------------
-	FUNCTION:	Transmit
-
-	DATE:		November 21st, 2013
-
-	REVISIONS:	
-	2013/11/25 - Vincent - Semaphore for blocking when hitting maximum packets sent
-	2013/11/26 - Vincent - Added line bidding
-   2013/11/27 - Vincent/German - Modified line bidding solve issues with simultaneous file sending
-								- Modified Packetize/sentPacketCounter call order
-								- Added basic timeout for end of 5 packet transmission
-								- Added line rebidding
-
-	DESIGNER:	Vincent Lau
-
-	PROGRAMMER:	Vincent Lau
-				German Villarreal
-
-	INTERFACE:	DWORD WINAPI TransmitThread(LPVOID param)
-		
-	RETURNS:	void, nothing.
-				
-	PARAMETERS:	LPVOID param - a dynamic char buffer that contains the whole file
-							we are trying to send over the serial port.
-
-	NOTES:	This is the function that contains the logic for transmitting data
-			and following those rules for doing so, such as the upper send limit
-			per round (5 packets per round in the (BE CREATIVE) Protocol).
-			It will stop when it has sent one packet out and wait for a response.
-			
-
-----------------------------------------------------------------------------*/

DWORD WINAPI TransmitThread(LPVOID param)
{
	
	LPSTR	file = (LPSTR)param;
	BOOL	bDoneSending = FALSE;
	
	sentPacketCounter = 0;
	bHaveFileToSend = TRUE;
	
	do
	{
		// Send ENQ - for getting the right-of-way to send
		//bENQToSend = TRUE;
		//if (bENQReceived) // if The other guy is sending something
		//{
		//	WaitForSingleObject(hWaitForLineSemaphore, INFINITE);
		//}
		//
		SendControl(hComm, ENQ);// REQUEST DA LINE
		SendData(hComm, "TEST");
		//// Wait for ACK
		//waitForType = ACK;
		//bWantLine = TRUE;
		//WaitForSingleObject(hWaitForLineSemaphore, INFINITE);
		//bWantLine = FALSE;
		
		while (++sentPacketCounter % 6 != 0) // ++ mod 6 allows sending of 5 packets, ++ mod 5 allows 4
		{
			// semaphore decrement 
			//WaitForSingleObject(hACKWaitSemaphore, INFINITE);

			Packetize(file, (sentPacketCounter - 1), packetToSend);
				
			SendData(hComm, packetToSend); // Send data to Serial Port
				
			// Set "What we're waiting for" to ACK
			waitForType = ACK;

		}
		SendControl(hComm, EOT);
		// Another semaphore to determine when to start again.
		Sleep(200);
		//WaitForSingleObject(hFileWaitSemaphore, INFINITE);
	} while(!bDoneSending); //file not done

	// DONE SENDING
	bENQToSend = FALSE;

	ExitThread(EXIT_SUCCESS);
}


/*-----------------------------------------------------------------------------
-	FUNCTION:	ReceiveThread
-
-	DATE:		November 22st, 2013
-
-	REVISIONS:	
-	2013/11/24 - Vincent - Overlapped i/o 
-	2013/11/25 - Vincent - Semaphore for releasing file sending block, when we've received 5 packets 
-							(other person hit their maximum)
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
	CHAR packetBuffer[1024] = {};
	DWORD nBytesRead = 0, dwEvent, dwError, dwWaitValue;
	COMSTAT cs;

	// Create the overlapped event. MUST CLOSE!!!
	ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ov.hEvent == NULL)
	{
		MessageBox(NULL, TEXT("Error creating Overlapped event"), NULL, MB_OK);
		ExitThread(EXIT_FAILURE);
	}
	

	// Set our Listening/Reading parameter for the serial port, want CHAR events
	SetCommMask(hComm, EV_RXCHAR);


	// overlapped
	while (1) //forever
	{
		while (bWantToRead) // while we want to read
		{
			WaitCommEvent(hComm, &dwEvent, &ov);
			dwWaitValue = WaitForSingleObject(ov.hEvent, INFINITE);
			ClearCommError(hComm, &dwError, &cs);
			if (dwWaitValue == WAIT_OBJECT_0 && (dwEvent & EV_RXCHAR)) // signaled with event char receive
			{
				// read SUCCESS!!!!!
				//MessageBox(NULL, TEXT("WFSO success"), NULL, MB_OK); //debug
				if (ReadSerialPort(hComm, packetBuffer, cs.cbInQue, &nBytesRead))
				{	
					// Successful read, send to packet check
					PacketCheck(*(HWND*)lphwnd, packetBuffer);

				}
			}
			else // Event Object was signaled with an error
			{
				
			}
			ResetEvent(ov.hEvent);
		} //while want to read
	} //while forever

	
	CloseHandle(ov.hEvent);
	ExitThread(EXIT_SUCCESS);
}