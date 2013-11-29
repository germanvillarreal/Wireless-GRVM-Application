#include "Packet.h"
CHAR packet[PACKET_BYTES_TOTAL];

BOOL Packetize(CHAR* bufferWithFile, int sentPacketCounter)
{
	CHAR* data = (CHAR*)malloc(PACKET_BYTES_DATA);
	size_t fileSize = strlen(bufferWithFile);
	BOOL isDone = FALSE;

	//1020 x sentPacketCounter = startingLocation
	size_t StartLoc = PACKET_BYTES_DATA * sentPacketCounter;
	if(StartLoc > fileSize)
	{
		fprintf(stderr, "%s", "Cannot seek to this location..");
		return TRUE;
	}


	for(size_t i = StartLoc, j = 0; j < PACKET_BYTES_DATA; i++, j++)
	{
		if(bufferWithFile[i] == '\0' || bufferWithFile[i] == EOF)
		{
			data[j] = '\0';
			isDone = TRUE;
		}
		else
		{
			data[j] = bufferWithFile[i];
		}
	}

	/*if(packet != NULL)
		free(packet);
	packet = (CHAR*)malloc(12);*/
	
	// Add control bytes to the packet
	packet[0] = SYN;
	packet[1] = (sentPacketCounter % 2 == 0) ? DC1 : DC2;

	// Add data bytes to the packet

	for(size_t i = 0; i < PACKET_BYTES_DATA; i++)
		packet[i+2] = data[i];

	
	// Add the trailer bytes to the packet (CRC)
	
	char pktral[2];
	//GenerateCRC(char pkt[GENERATE_CRC_TEST_SIZE], char generatedCRC[2]){ // GENERATE_CRC_TEST_SIZE = 1020
	GenerateCRC(data, pktral);
	packet[PACKET_BYTES_TOTAL-2] = pktral[0];
	packet[PACKET_BYTES_TOTAL-1] = pktral[1];
	
	free (data); // done with data portion

	return isDone;
}

int main(void)
{
	BOOL done = FALSE;	
	CHAR* file = "123456789abcdef12345123451p234512345123451234512345123451234512345123451234512345123451234512345123451234512345";
	INT	  sent = 0;

	

	do
	{
		done = Packetize(file, sent++);
		CHAR dataCRC[PACKET_BYTES_DATA + PACKET_BYTES_CRC];
		printf("%s\n", packet);

		

		/*for(size_t i = 2, j = 0; j < PACKET_BYTES_DATA + PACKET_BYTES_CRC;
			i++, j++)
		{
			dataCRC[j] = packet[i];
		}
*/

		printf("%d\n",ErrorCheck(packet+2));

		Sleep(1000);
	}while(!done);

	return 0;
}
