#include "Presentation.h"
#define pos  1020

CHAR* displayBuffer;
int counterPacket = 0;
int oldLen;	//stores the current length of the buffer
int newLen;		//stores the new reallocated length
BOOL AddToBuffer(const char* packet)
{


	int sum = 0;
	if(displayBuffer == NULL)
	{
		oldLen = 0;
	} 
	else
	{
		//oldLen = 1020*startingPosition;

		//oldLen = strlen(displayBuffer);
		//oldLen = 1;

		oldLen = newLen;
	}
	int y = oldLen;
	counterPacket++;
	if(counterPacket>0){
		sum = pos * counterPacket;
	}
	//displayBuffer = (LPSTR)realloc(displayBuffer, oldLen + strlen(packet));
	//displayBuffer = (CHAR*)realloc(displayBuffer, oldLen + strlen(packet));
	displayBuffer = (CHAR*)realloc(displayBuffer, sum);
	int x = oldLen + strlen(packet);
	//newLen = strlen(displayBuffer);
	newLen = sum;
	
	for (size_t i = oldLen, j = 0; j < 1020; i++, j++)
	{
		if (packet[j] == '\0' || packet[j] == 'Í' || packet[j] == 'Í')
		{
			displayBuffer[i] = '\0';
			break;
		}
		displayBuffer[i] = packet[j];
	}
	//displayBuffer[sum] = '\0';
	
	return TRUE;

 }
