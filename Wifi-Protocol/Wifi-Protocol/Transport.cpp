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
	char*	packetToSend;
	bool	bDoneSending = false;

	do
	{
		packetToSend = Packetize(file, sentPacketCounter);
		while (sentPacketCounter % 5 != 0)
		{
			// semaphore decrement (minus 1, should equal 0)
			WaitForSingleObject(hACKWaitSemaphore, INFINITE);
			// sendDataPacket(packetToSend);
			sentPacketCounter = 0;
			// Set "What we're waiting for" to ACK
			waitForType = ACK;
			// semaphore decrement (minus 1, should try to equal -1 and block)
			WaitForSingleObject(hACKWaitSemaphore, INFINITE);
		}
		// Another semaphore to determine when to start again.
	} while(!bDoneSending); //file not done
}
