
/*-----------------------------------------------------------------------------
--  SOURCE FILE:    Packet.cpp
--
--  PROGRAM:        Wireless Protocol (GRVM)
--
--  FUNCTIONS:      CHAR* Packetize(FILE*, int)
--                  BOOL PacketCheck(HWND, char[1024], int*)
--                              
--  
--
--  DATE:           2013/11/18
--
--  REVISIONS:      2013/11/20 - German Villarreal
--                  Added PacketCheck function
--					2013/11/24
--					Packetize parameter FILE* changed to CHAR*
--
--  DESIGNER:       
--
--  PROGRAMMER:     German Villarreal   
--
--  NOTES:          
--      Current problems:
--          -if NACK was received, packetCheck must have a way to resend the last packet
--          -Receive/PacketCheck may need a better way to know what type of packet to expect next
--          -Receive thread needs hwnd to pass to packetCheck to pass to Display if the packet was received successfully
--          -unsure of trailer bytes to add in packetize
--
-----------------------------------------------------------------------------*/

#include "Packet.h"

BOOL Packetize(CHAR* bufferWithFile, int sentPacketCounter, CHAR* packet)
{
	char data[PACKET_BYTES_DATA];
	//size_t ndx;
	size_t fileSize = strlen(bufferWithFile);
	BOOL isDone = FALSE;
	//1020 x sentPacketCounter = startingLocation
	int StartLoc = PACKET_BYTES_DATA * sentPacketCounter;
	if(StartLoc * PACKET_BYTES_DATA > (int)fileSize)
	{
		fprintf(stderr, "%s", "Cannot seek to this location..");
		return 0;
	}

	for(size_t i = StartLoc; i < PACKET_BYTES_DATA; i++)
	{
		if(bufferWithFile[i] == '\0' || bufferWithFile[i] == EOF)
		{
			data[i] = '\0';
			isDone = TRUE;
		}
		data[i] = bufferWithFile[i];
	}
	
	// Add control bytes to the packet
	packet[1] = (sentPacketCounter % 2 == 0) ? DC1 : DC2;

	// Add data bytes to the packet
	for(size_t i = 0; i < PACKET_BYTES_DATA; i++)
		packet[i+2] = data[i];
	
	// Add the trailer bytes to the packet (CRC)
	
	char pktral[2];
	//char* GenerateCRC(char pkt[GENERATE_CRC_TEST_SIZE], char generatedCRC[2]){ // GENERATE_CRC_TEST_SIZE = 1020
	GenerateCRC(packet, pktral);
	packet[PACKET_BYTES_DATA+2] = pktral[0];
	packet[PACKET_BYTES_DATA+3] = pktral[1];

	return isDone;
}

BOOL PacketCheck(HWND hwnd, char packet[1024])
{
	// Make sure we're getting our own packets, not some other packet
	//switch (packet[0])
	//{
	//case SYN:
	//	break;
	//default: // discard packet
	//	return FALSE;
	//}

//HAD TO COMMENT THIS OUT TO TEST COMPILE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	switch (packet[1])
	{
	//case ENQ:
	//	bENQReceived = TRUE;
	//	SendControl(hComm, ACK);
	//	//Set "what we're waiting for" flag to DC1
	//	waitForType = DC1;
	//break;
	//case ACK:
	//	// check if we wanted an ACK
	//	if (waitForType == ACK)
	//	{
	//		// check if we're actually bidding for the line
	//		if (bWantLine)
	//			ReleaseSemaphore(hWaitForLineSemaphore, 1, NULL);
	//		else
	//		// check if we're sending a file
	//			ReleaseSemaphore(hACKWaitSemaphore, 1, NULL);
	//	}
	//break;

	case DC1:
		//if we're waiting for a DC2 packet

		char p[1022];
		for(size_t i = 2; i < 1024; i++)
			p[i - 2] = packet[i];

		if(waitForType == DC2 || !ErrorCheck(p))//pass data+crcbits
		{
			SendControl(hComm, NAK);
			break;
		}
	
		SendControl(hComm, ACK);
		//add to buffer
		char p2[1020];
		for(size_t i = 2; i < 1022; i++)
			p2[i - 2] = packet[i];
		AddToBuffer(p2);

		//Display(hwnd, );//read the remaining 1020 characters 
	break;
	
			
	case DC2:
		// if we're waiting for a DC1 packet
		if (waitForType == DC1)
		{
			SendControl(hComm, NAK);
			break;
		}

		char p3[1022];
		for(size_t i = 2; i < 1024; i++)
			p3[i - 2] = packet[i];


		if (!ErrorCheck(p3))
		{
			SendControl(hComm, NAK);
			break;
		}
		SendControl(hComm, ACK);
		//Display();//read the remaining 1020 characters 
	break;

	case NAK:
		//Set "What we're waiting for" flag to ACK
		waitForType = ACK;
		SendData(hComm, packetToSend); // send the previous packet
	break;
		
	//case EOT:
	//	bENQReceived = FALSE;
	//	if (bENQToSend)
	//	{
	//		ReleaseSemaphore(hWaitForLineSemaphore, 1, NULL);
	//		break;
	//	}
	//	// GO back to IDLE state
	//	waitForType = ENQ;
	//break;
    }
	return TRUE;
}