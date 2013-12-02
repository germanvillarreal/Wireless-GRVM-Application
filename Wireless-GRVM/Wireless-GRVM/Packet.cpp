
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


CHAR Packet[PACKET_BYTES_TOTAL];

BOOL Packetize(CHAR* bufferWithFile, int sentPacketCounter)
{
	//CHAR* data = (CHAR*)malloc(PACKET_BYTES_DATA);
	//data[1024] = '\0';
	char data[1024];
	BOOL isDone = FALSE;
	size_t fileSize = strlen(bufferWithFile);
	size_t StartLoc = PACKET_BYTES_DATA * sentPacketCounter; //1020 x sentPacketCounter = startingLocation
	//free(data);
	if(StartLoc > fileSize)
	{
		fprintf(stderr, "%s", "Cannot seek to this location..");
		return TRUE; //WE'RE DONE.
	}

	for (size_t n = 0; n < PACKET_BYTES_TOTAL; n++)
		Packet[n] = ' ';


	for(size_t i = StartLoc, j = 0; j < PACKET_BYTES_DATA; i++, j++)
	{
		
		if((bufferWithFile[i] == EOF || bufferWithFile[i] == '\0'))
		//if(bufferWithFile[i] == '\0' || bufferWithFile[i] == EOF)
		//if(bufferWithFile[i] == '\0')
		{
			//data[j] = 'W';
			data[j] = '\0';
			isDone = TRUE;
			i--;
		}
		else
		{
			data[j] = bufferWithFile[i];
		}
	}

	
	// Add control bytes to the packet
	Packet[0] = SYN;
	Packet[1] = (sentPacketCounter % 2 == 0) ? DC1 : DC2;

	// Add data bytes to the packet
	for(size_t i = 0; i < PACKET_BYTES_DATA; i++)
		Packet[i+2] = data[i];
	
	// Add the trailer bytes to the packet (CRC)
	char pktral[2];
	//char* GenerateCRC(char pkt[GENERATE_CRC_TEST_SIZE], char generatedCRC[2]){ // GENERATE_CRC_TEST_SIZE = 1020
	GenerateCRC(data, pktral);
	Packet[PACKET_BYTES_TOTAL-2] = pktral[0];
	Packet[PACKET_BYTES_TOTAL-1] = pktral[1];

//	free(data); // Done with the data portion buffer
	char test = data[500];
	char test2 = data[800];

	return isDone;
}

BOOL PacketCheck(HWND hwnd, CHAR* packet)
{
	char* data = (CHAR*) malloc(1020);
	data[1020] = '\0';
	int lengthOfData = strlen(data);
	
	 //Make sure we're getting our own packets, not some other packet
	switch (packet[0])
	{
	case SYN:
		break;
	default: // discard packet
		return FALSE;
	}

	switch (packet[1])
	{
	case ENQ:
		bENQReceived = TRUE;
		//SendControl(hComm, ACK);
		bWantToSendACK = TRUE;
		//	Set "what we're waiting for" flag to DC1
		waitForType = DC1;
	break;
	case ACK:
		// check if we wanted an ACK
		if (waitForType == ACK)
		{
			// check if we're actually bidding for the line
			if (bWantLine)
				ReleaseSemaphore(hWaitForLineSemaphore, 1, NULL);
			else
			// check if we're sending a file
				ReleaseSemaphore(hACKWaitSemaphore, 1, NULL);
		}
	break;

	case DC1:
		//if we're waiting for a DC2 packet

		if(waitForType == DC2 || !ErrorCheck(packet+2))//pass data+crcbits
		{
			SendControl(hComm, NAK);
			break;
		}
		waitForType = DC2;
		bWantToSendACK = TRUE;
		GetData(packet, data);
		AddToBuffer(data);
	break;
	case DC2:
		// if we're waiting for a DC1 packet
		if (waitForType == DC1 || !ErrorCheck(packet+2))
		{
			SendControl(hComm, NAK);
			break;
		}
		waitForType = DC1;
		bWantToSendACK = TRUE;
		GetData(packet, data);
		AddToBuffer(data);
	break;
	case NAK:
		//Set "What we're waiting for" flag to ACK
		waitForType = ACK;
		SendData(hComm, Packet); // send the previous packet
	break;
	case EOT:
		bENQReceived = FALSE;
		if (bENQToSend)
		{
			ReleaseSemaphore(hWaitForLineSemaphore, 1, NULL);
			break;
		}
	    // GO back to IDLE state
		waitForType = ENQ;
	break;
    }
	return TRUE;
}

BOOL PacketCheckControl(HWND hwnd, CHAR packet[2])
{
	
	//MessageBox(NULL, TEXT("PacketCheckControl Error"), NULL, NULL);
	 //Make sure we're getting our own packets, not some other packet
	switch (packet[0])
	{
	case SYN:
		break;
	default: // discard packet
		return FALSE;
	}

//HAD TO COMMENT THIS OUT TO TEST COMPILE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	switch (packet[1])
	{
	case ENQ:
		bENQReceived = TRUE;
		//SendControl(hComm, ACK);
		bWantToSendACK = TRUE;
		//	Set "what we're waiting for" flag to DC1
		waitForType = DC1;
	break;
	case ACK:
		// check if we wanted an ACK
		//MessageBox(NULL, TEXT("Got An ACK"), NULL, NULL);
		if (waitForType == ACK)
		{
			// check if we're actually bidding for the line
			if (bWantLine){
				//MessageBox(NULL, TEXT("ReleaseSemaphoreACK BWANTLINE FALSE"), NULL, NULL);
				ReleaseSemaphore(hWaitForLineSemaphore, 1, NULL);}
			else{
			// check if we're sending a file
			//MessageBox(NULL, TEXT("ReleaseSemaphoreACK BWANTLINE FALSE"), NULL, NULL);
			ReleaseSemaphore(hACKWaitSemaphore, 1, NULL);}
			//MessageBox(NULL, TEXT("ThreadReleaseSemaphoreACK BWANTLINE FALSE"), NULL, NULL);
		}
	break;
	case NAK:
		//Set "What we're waiting for" flag to ACK
		waitForType = ACK;
		SendData(hComm, Packet); // send the previous packet
	break;
		
	case EOT:
		bENQReceived = FALSE;
		if (bENQToSend)
		{
			ReleaseSemaphore(hWaitForLineSemaphore, 1, NULL);
			break;
		}
	    // GO back to IDLE state
		waitForType = ENQ;
	break;
    }
	//MessageBox(NULL, TEXT("Unable to return True of PacketCheck "), NULL, NULL);
	return TRUE;
}

void GetData(CHAR* packet, CHAR* dataOut)
{
	//size_t d = 0, p = 2;
	size_t d = 0, p = 2;
	int sizeOfDataOut = strlen(dataOut);
	int sizeOfpacket = strlen(packet);
	//while (packet[p] != '\0' ||
	//	p < PACKET_BYTES_TOTAL - PACKET_BYTES_CRC) 

//	while (packet[p] != '\0' ||
//		p < 1022) 

	while (p < 1022) 
	{
		if (p == 1020)
			int df = 0;
		dataOut[d++] = packet[p++];
	}
}