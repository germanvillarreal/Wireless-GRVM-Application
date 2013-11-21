
/*-----------------------------------------------------------------------------
--  SOURCE FILE:    Packet.cpp
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
--
-----------------------------------------------------------------------------*/

#include "Packet.h"

CHAR* packetize(FILE* bufferWithFile, int SentPacketCounter)
{
	char data[PACKET_BYTES_DATA];
	char packet[PACKET_BYTES_TOTAL];
	size_t ndx;
	//1020 x sentPacketCounter = startingLocation
	int StartLoc = PACKET_BYTES_DATA * SentPacketCounter;

	// Go to the begining of the line
	if(fseek(bufferWithFile, StartLoc, SEEK_SET) != 0)
	{
		fprintf(stderr, "%s", "Unable to seek to the next line..");
		//return
	}

	// Read 1020 chars from the file buffer, starting at startingLocation into packet string
	if(fgets(data, PACKET_BYTES_DATA, bufferWithFile) == NULL)
	{
		fprintf(stderr, "%s", "Unable to read the next line..");
		//return 
	}

	// If we encounter eof
	for(ndx = 0; ndx < PACKET_BYTES_DATA; ndx++)
	{
		if(data[ndx] == EOF)
			break;
	}

	// Pad remains Bytes with null
	while( ndx < PACKET_BYTES_DATA)
	{
		data[ndx] = '\0';
	}

	packet[1] = (SentPacketCounter % 2 == 0) ? DC1 : DC2;
	packet[2] = data[PACKET_BYTES_DATA];
	packet[PACKET_BYTES_DATA];

//If (sentPacketBuffer % 2 == 0)
//	Packet[1] = DC1
//Else
//	Packet[1] = DC2

   //create return string returnstr
   //add control bytes to returnstr
   //if s[i] != eof
   //   returnstr += s[i]
   //
   // while i != 1022
   //    returnstr += '\0'		
   // returnstr += trailer bytes
   // return returnstr
}

BOOL PacketCheck(HWND hwnd, char packet[1024])
{
    //switch (char[1])
	switch(packet[1])
	{
	case ENQ:
		//send(ACK);
		//Set "what we're waiting for" flag to PACKET_DC1
	break;
	case DC1:
		//if we're waiting for a DC2 packet:
			//send (NAK);
			//break;
			
		if (!ErrorCheck(packet[1022], packet[1023]))
		{
			send(NAK);
			break;
		}
	
		send (ACK);
		Display();//read the remaining 1020 characters 
	break;
			
	case DC2:
		if ("what we're waiting for" is a PACKET_DC1)
		{
			send (NAK);
			break;
		}
		if (!ErrorCheck(char[1022], char[1023]))
		{
			sendControlPacket (NAK);
			break;
		}
		send (ACK);
		Display();//read the remaining 1020 characters 
		break;
		
		case NAK:
			//Set "What we're waiting for" flag to ACK
			send (previous packet); //need a way to keep that
			break;
		
		case EOT:
			// GO back to IDLE state
			Set "what we're waiting for" flag to ENQ
			break;
    }
}
