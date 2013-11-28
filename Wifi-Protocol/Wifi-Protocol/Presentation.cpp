#include "Presentation.h"


LPSTR displayBuffer = NULL;

BOOL AddToBuffer(char packet[1020])
{
	int oldLen;	//stores the current length of the buffer
	int newLen;		//stores the new reallocated length

	if(displayBuffer != NULL)
	{
		oldLen = 0;
	} else
		oldLen = strlen(displayBuffer);

	displayBuffer = (LPSTR)realloc(displayBuffer, oldLen + strlen(packet));
	newLen = strlen(displayBuffer);

	for(int i = oldLen; i < newLen; i++)
	{
		displayBuffer[i] = packet[i-oldLen];
	}

	return TRUE;

}
