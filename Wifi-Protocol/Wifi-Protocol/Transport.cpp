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
#include "CircularBuffer.h"
INT waitForType = ENQ;
INT sentPacketCounter = 0;	/* Counter to keep track of our file location as
							 as well as how many packets we've sent. */
BOOL bHaveFileToSend = FALSE;
BOOL bENQToSend	= FALSE;
BOOL bENQReceived = FALSE;
BOOL bWantLine = FALSE;

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
	DWORD	dwenqTimeout;
	
	sentPacketCounter = 0;
	bHaveFileToSend = TRUE;

	do
	{
		// Send ENQ - for getting the right-of-way to send
		bENQToSend = TRUE;
		if (bENQReceived) // if The other guy is sending something
		{
			WaitForSingleObject(hWaitForLineSemaphore, INFINITE);
		}
		
		SendControl(hComm, ENQ);// REQUEST DA LINE

		//// Wait for ACK
		waitForType = ACK;
		bWantLine = TRUE;
		dwenqTimeout = WaitForSingleObject(hWaitForLineSemaphore, INFINITE);
		if(dwenqTimeout == WAIT_OBJECT_0)
		{
			bWantLine = FALSE;
		
			while (++sentPacketCounter % 6 != 0) // ++ mod 6 allows sending of 5 packets, ++ mod 5 allows 4
			{	
				
				//if(WaitForSingleObject(hACKWaitSemaphore, INFINITE) == WAIT_TIMEOUT)
				//{
				//	sentPacketCounter--;
				//}

				//if(WaitForSingleObject(hACKWaitSemaphore, INFINITE) == WAIT_OBJECT_0)				
				//{
					//bDoneSending = Packetize(file, (sentpacketcounter));
					bDoneSending = Packetize(file, (sentPacketCounter - 1));	
					// set "what we're waiting for" to ack
					waitForType = ACK;
					//Sleep(5000);
					SendData(hComm, Packet); // send data to serial port
					//Sleep(5000);	
					
				//}
				//else
				//{
				//	sentPacketCounter--;
				//}
				
			}
			SendControl(hComm, EOT);
			 //another semaphore to determine when to start again.
			Sleep(1000);
			//WaitForSingleObject(hFileWaitSemaphore, INFINITE);
		} 
		// DONE SENDING
		bENQToSend = FALSE;
	} while(!bDoneSending); //file not done
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
-	PARAMETERS:	LPVOID lphwnd - reference to the edit window's handle (hEdit in main)
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
	cb mycb1;
	cb *my_cb = &mycb1;
	initBuffer(my_cb, 1024);
	CHAR dataBufferToVerifyWhatItShouldBe[1024] = {};
	DWORD nBytesRead = 0, dwEvent, dwError, dwWaitValue;
	COMSTAT* cs = (COMSTAT*)malloc(sizeof(COMSTAT));
	char* packetBuffer = (char*) malloc(1024);
	char displayBufferVariable[1024];
	int lengthOfPacketBuffer = strlen(packetBuffer);

	HANDLE m_hThreadTerm = CreateEvent(0,0,0,0);
	
	
	// Create the overlapped event. MUST CLOSE!!!
	memset(&ov,0, sizeof(ov));
	ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ov.hEvent == NULL)
	{
		MessageBox(NULL, TEXT("Error creating Overlapped event"), NULL, MB_OK);
		ExitThread(EXIT_FAILURE);
	}
	HANDLE handletoTwoEvents[2];
	handletoTwoEvents[0] = m_hThreadTerm;
	DWORD dwWait;
	BOOL waitCommEventValue = true;

	// Set our Listening/Reading parameter for the serial port, want CHAR events
	SetCommMask(hComm, EV_TXEMPTY | EV_RXCHAR);
	
	while(1)
	{
		waitCommEventValue = WaitCommEvent(hComm, &dwEvent, &ov);
		if(waitCommEventValue)
		{
			break;
		}
		handletoTwoEvents[1] = ov.hEvent;
		//dwWaitValue = WaitForMultipleObjects(2, handletoTwoEvents, FALSE, INFINITE);

		dwWaitValue = WaitForSingleObject(ov.hEvent,INFINITE);
		ClearCommError(hComm, &dwError, cs);
		switch ( dwWaitValue )
		{
			/*case WAIT_OBJECT_0:
			
				//_endthreadex(1);
				MessageBox(NULL, TEXT("HOW????"), NULL, NULL);
				TerminateThread(handletoTwoEvents[0], 0);
				MessageBox(NULL, TEXT("WHY????"), NULL, NULL);
			break;*/
			//case WAIT_OBJECT_0 + 1:
			case WAIT_OBJECT_0:
				do
				{
					ResetEvent(ov.hEvent);
					nBytesRead = 0;
					if( (cs->cbInQue >= 1024) && ReadSerialPort(hComm, packetBuffer, 1024, &nBytesRead) 
						&& (nBytesRead != 0) )
					{
						// DC1/Dc2
						PacketCheck(*(HWND*)lphwnd, packetBuffer);
						for(int i = 0; i < cs->cbInQue; i++)
						{
							writeBuffer(my_cb, packetBuffer[i]);
							
						}
					}
					else if (cs->cbInQue == 2 && ReadSerialPort(hComm, packetBuffer, cs->cbInQue, &nBytesRead))
					{
						PacketCheck(*(HWND*)lphwnd, packetBuffer);
						//restartBuffer(my_cb);
						// ENQ/ACK/NAK/EOT
						//MessageBox(NULL, TEXT("2 bytes"), NULL, MB_OK);
					}
					else
					{
						PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
					}
					
						//send to packet check
					/*for (int q = 0; q < 1024; q++)
					{
						dataBufferToVerifyWhatItShouldBe[q] = packetBuffer[q];
					}*/
					 
						//PacketCheck(*(HWND*)lphwnd, packetBuffer);
					ClearCommError(hComm, &dwError, cs);
					dwWait = cs->cbInQue;
				} while (nBytesRead > 0);
				// endif "signaled"
				
				/*if(!isEmpty(my_cb)){
					while (my_cb->first < 1024)
					{	
					
							for (int i = 0; i < 1024; i++){
								displayBufferVariable[i] = readBuffer(my_cb);
							}
					
					}
				}*/
				int random = 0;
				//AddToBuffer(packetBuffer);
			
				restartBuffer(my_cb);
				DisplayText(*(HWND*)lphwnd, displayBuffer);
				Sleep(500);
		}
		int randomAgain = 0;
	}


	// Set our Listening/Reading parameter for the serial port, want CHAR events
	SetCommMask(hComm, EV_TXEMPTY | EV_RXCHAR);
	













	/*
	// overlapped
	while (1) //forever
	{
		while (bWantToRead) // while we want to read
		{
			CHAR packetBuffer[8] = {};
			ResetEvent(ov.hEvent);
			WaitCommEvent(hComm, &dwEvent, &ov);
			dwWaitValue = WaitForSingleObject(ov.hEvent, INFINITE);
			ClearCommError(hComm, &dwError, cs);
			//MessageBox(NULL, TEXT("Test 1"), NULL, MB_OK); //debug
			if (dwWaitValue == WAIT_OBJECT_0)
			{
				do
				{
					if(ReadSerialPort(hComm, packetBuffer, cs->cbInQue, &nBytesRead) 
						&& nBytesRead != 0){
						for(int i = 0; i < cs->cbInQue; i++)
						{
							writeBuffer(my_cb, packetBuffer[i]);
							
						}
						
						//send to packet check
					for (int q = 0; q < 1024; q++){
						dataBufferToVerifyWhatItShouldBe[q] = packetBuffer[q];
					}
					 
						//PacketCheck(*(HWND*)lphwnd, packetBuffer);
					}
				} while (nBytesRead > 0);

			} // endif "signaled"
			else // Event Object was signaled with an error
			{
				//MessageBox(NULL, TEXT("Test WFSO ERROR"), NULL, MB_OK); //debug
			}
			
			
			while (my_cb->first < 1024)
			{	
				for (int i = 0; i < 1024; i++){
					displayBufferVariable[i] = readBuffer(my_cb);
				}
			}
			//AddToBuffer(displayBufferVariable);
			
			restartBuffer(my_cb);

			
		} //while want to read
		
		
	} //while forever

	DisplayText(*(HWND*)lphwnd, displayBufferVariable);
	CloseHandle(ov.hEvent);
	ExitThread(EXIT_SUCCESS);
	*/
}