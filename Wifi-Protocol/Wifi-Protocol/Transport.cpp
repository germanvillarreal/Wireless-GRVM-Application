/*-----------------------------------------------------------------------------
--  SOURCE FILE:    Transport.cpp
--
--  PROGRAM:        Wireless Protocol (GRVM)
--
--  FUNCTIONS:      void Transmit(char* lpszFileBuffer)
--                  bool PacketCheck(char[1024] packet)
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
--		Provides the logic for data transfer between the computer and the 
--		wireless device (which would be transmitting to the other wireless 
--		device conneceted to the other computer). Error checking will be done
--		via open-source CRC.
-----------------------------------------------------------------------------*/
#include <crc.hpp>

/*
hello vincent
*/