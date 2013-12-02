/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: ErrorCheck.cpp
--
-- PROGRAM: Wireless-MRGV
--
-- FUNCTIONS:
-- unsigned short CRCCCITT(unsigned char *data, size_t length, unsigned short seed, unsigned short final)
-- BOOL ErrorCheck(char pkt[ERROR_CHECK_TEST_SIZE])
-- char* GenerateCRC(char pkt[GENERATE_CRC_TEST_SIZE], char generatedCRC[2])
-- 
-- DATE: November 18, 2013
--
-- REVISIONS:  
-- 
--
-- DESIGNER: Robin Hsieh
--
-- PROGRAMMER: Robin Hsieh
--
-- NOTES:
-- Using the CRC-CCITT code, taken from http://automationwiki.com/index.php?title=CRC-16-CCITT.
-- 
--
----------------------------------------------------------------------------------------------------------------------*/
#include "Includes.h"
#include "resource.h"
#include "Main.h"
#include "ErrorCheck.h"



/**************************************************************************
//
// crc16.c - generate a ccitt 16 bit cyclic redundancy check (crc)
//
//      The code in this module generates the crc for a block of data.
//
//                                16  12  5
// The CCITT CRC 16 polynomial is X + X + X + 1.
// In binary, this is the bit pattern 1 0001 0000 0010 0001, and in hex it
//  is 0x11021.
// A 17 bit register is simulated by testing the MSB before shifting
//  the data, which affords us the luxury of specifiy the polynomial as a
//  16 bit value, 0x1021.
// Due to the way in which we process the CRC, the bits of the polynomial
//  are stored in reverse order. This makes the polynomial 0x8408.
// note: when the crc is included in the message, the valid crc is:
//      0xF0B8, before the compliment and byte swap,
//      0x0F47, after compliment, before the byte swap,
//      0x470F, after the compliment and the byte swap.
//
// crc16() - generate a 16 bit crc
//
//
// PURPOSE
//      This routine generates the 16 bit remainder of a block of
//      data using the ccitt polynomial generator.
//
// CALLING SEQUENCE
//      crc = crc16(data, len);
//
// PARAMETERS
//      data    <-- address of start of data block
//      len     <-- length of data block
//
// RETURNED VALUE
//      crc16 value. data is calcuated using the 16 bit ccitt polynomial.
//
// NOTES
//      The CRC is preset to all 1's to detect errors involving a loss
//        of leading zero's.
//      The CRC (a 16 bit value) is generated in LSB MSB order.
//      Two ways to verify the integrity of a received message
//        or block of data:
//        1) Calculate the crc on the data, and compare it to the crc
//           calculated previously. The location of the saved crc must be
//           known.
/         2) Append the calculated crc to the end of the data. Now calculate
//           the crc of the data and its crc. If the new crc equals the
//           value in "crc_ok", the data is valid.
//
// PSEUDO CODE:
//      initialize crc (-1)
//      DO WHILE count NE zero
//        DO FOR each bit in the data byte, from LSB to MSB
//          IF (LSB of crc) EOR (LSB of data)
//            crc := (crc / 2) EOR polynomial
//          ELSE
//            crc := (crc / 2)
//          FI
//        OD
//      OD
//      1's compliment and swap bytes in crc
//      RETURN crc
//
**************************************************************************/
unsigned short crc16(char *data_p, unsigned short length)
{
       unsigned char i;
       unsigned int data;
       unsigned int crc;
        
       crc = 0xffff;
        
       if (length == 0)
              return (~crc);
        
       do {
              for (i = 0, data = (unsigned int)0xff & *data_p++; i < 8; i++, data >>= 1) {
                    if ((crc & 0x0001) ^ (data & 0x0001))
                           crc = (crc >> 1) ^ POLY;
                    else
                           crc >>= 1;
              }
       } while (--length);
        
       crc = ~crc;
        
       data = crc;
       crc = (crc << 8) | (data >> 8 & 0xFF);
        
       return (crc);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: GenerateCRC
--
-- DATE: November 27, 2013
--
-- REVISIONS: 
-- November 28, 2013 - Robin Hsieh: Modified CRC16 being used.
--
-- DESIGNER: Robin Hsieh
--
-- PROGRAMMER: Robin Hsieh
--
-- INTERFACE: char* GenerateCRC(char pkt[GENERATE_CRC_TEST_SIZE], char generatedCRC[PACKET_BYTES_CRC])
--				char pkt[GENERATE_CRC_TEST_SIZE]:			Packet of data that needs to be error checked using the CRC16 algorithm.
--				char generatedCRC[PACKET_BYTES_CRC]:		The two crc bytes generated.
--
-- RETURNS: Returns the 2 CRC characters.
--
-- NOTES:
-- This function generates the 2 CRC characters so it can be appended to the end of every packet being sent.
------------------------------------------------------------------------------------------------------------------*/
void GenerateCRC(char pkt[GENERATE_CRC_TEST_SIZE], char generatedCRC[PACKET_BYTES_CRC]){ // GENERATE_CRC_TEST_SIZE = 1020
	unsigned short the_crc;

	the_crc = crc16(pkt, PACKET_BYTES_DATA);
	generatedCRC[0] = (unsigned char)((the_crc >> 8) & 0xff);
	generatedCRC[1] = (unsigned char)(the_crc & 0xff);

}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ErrorCheck
--
-- DATE: November 18, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Robin Hsieh
--
-- PROGRAMMER: Robin Hsieh
--
-- INTERFACE: BOOL ErrorCheck(char pkt[ERROR_CHECK_TEST_SIZE])
--				char pkt[1020]:					Packet of data that needs to be error checked using the CRC16 algorithm
--				unsigned short the_crc:			Remainder for the crc
--				unsigned short checking_crc:	Used to check the crc
--
-- RETURNS: Returns the true if error check passes.
--
-- NOTES:
-- This function does the error checking on the packets received.
------------------------------------------------------------------------------------------------------------------*/
BOOL ErrorCheck(char pkt[PACKET_BYTES_DATA + PACKET_BYTES_CRC]){ // ERROR_CHECK_TEST_SIZE = 1022

	unsigned short the_crc, checking_crc;
	unsigned short first_crc, second_crc;

	//first_crc = pkt [10];
	//second_crc = pkt [11];

	//checking_crc = (unsigned char)((first_crc << 8) + second_crc);
	
	the_crc = crc16(pkt, PACKET_BYTES_DATA + PACKET_BYTES_CRC);
	//the_crc = CRCCCITT(pkt, 12, 0xffff, checking_crc);
	//printf("Final CRC value is 0x%04X\n", the_crc);
	
	// If the remainder of "the_crc" should be 0 if everything is correct.
	if(the_crc == crc_ok)
	{
		printf("TRUE\t");
		return TRUE;
	}
	MessageBox(NULL, TEXT("ErrorCheck Fail"), NULL, NULL);
	return FALSE;
}






/*

static unsigned short crc_table [256] = {

	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5,
	0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
	0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210,
	0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c,
	0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
	0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b,
	0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6,
	0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
	0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5,
	0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969,
	0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
	0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc,
	0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03,
	0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
	0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6,
	0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,
	0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
	0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1,
	0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c,
	0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2,
	0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb,
	0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447,
	0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
	0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2,
	0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9,
	0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
	0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c,
	0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0,
	0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,
	0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07,
	0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba,
	0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
	0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};*/

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: CRCCITT
--
-- DATE: November 18, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Robin Hsieh
--
-- PROGRAMMER: Robin Hsieh
--
-- INTERFACE: unsigned short CRCCCITT(unsigned char *data, size_t length, unsigned short seed, unsigned short final)
--				unsigned char *data:		The data that needs to be generated
--				size_t length:				The length of the generation.
--				unsigned short seed:		The seed that is referenced within the crc table.
--				unsigned short final:		Bitwise exclusive or with the CRC generated from the table.
--
-- RETURNS: Returns the generated CRC code.
--
-- NOTES:
-- This function creates the generated CRC code. Taken from http://automationwiki.com/index.php?title=CRC-16-CCITT
------------------------------------------------------------------------------------------------------------------*/
/*unsigned short CRCCCITT(char *data, size_t length, unsigned short seed, unsigned short final)
{ 
	size_t count;
	unsigned int crc = seed;
	unsigned int temp;
	for (count = 0; count < length; ++count){
		temp = (*data++ ^ (crc >> 8)) & 0xff;
		crc = crc_table[temp] ^ (crc << 8);
	}
	return (unsigned short)(crc ^ final);
} 
*/
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ErrorCheck
--
-- DATE: November 18, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Robin Hsieh
--
-- PROGRAMMER: Robin Hsieh
--
-- INTERFACE: BOOL ErrorCheck(char pkt[ERROR_CHECK_TEST_SIZE])
--				char pkt[1020]:					Packet of data that needs to be error checked using the CRC16 algorithm
--				unsigned short the_crc:			Remainder for the crc
--				unsigned short checking_crc:	Used to check the crc
--
-- RETURNS: Returns the true if error check passes.
--
-- NOTES:
-- This function does the error checking on the packets received.
------------------------------------------------------------------------------------------------------------------*//*
BOOL ErrorCheck(char pkt[ERROR_CHECK_TEST_SIZE]){ // ERROR_CHECK_TEST_SIZE = 1022

	unsigned short the_crc, checking_crc;
	unsigned short first_crc, second_crc;

	first_crc = pkt [ERROR_CHECK_TEST_SIZE - 2];
	second_crc = pkt [ERROR_CHECK_TEST_SIZE - 1];

	checking_crc = (unsigned char)((first_crc << 8) + second_crc);

	the_crc = CRCCCITT(pkt, ERROR_CHECK_TEST_SIZE - 2, 0xffff, checking_crc);
	printf("Final CRC value is 0x%04X\n", the_crc);
	
	// If the remainder of "the_crc" should be 0 if everything is correct.
	if(the_crc == 0)
	{
		return true;
	}

	return false;
}
*/
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: GenerateCRC
--
-- DATE: November 27, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Robin Hsieh
--
-- PROGRAMMER: Robin Hsieh
--
-- INTERFACE: char* GenerateCRC(char pkt[GENERATE_CRC_TEST_SIZE], char generatedCRC[2])
--				char pkt[1020]:				Packet of data that needs to be error checked using the CRC16 algorithm.
--				unsigned short the_crc:		Remainder for the crc.
--				char generatedCRC[2]:		The two crc bytes generated.
--
-- RETURNS: Returns the 2 CRC characters.
--
-- NOTES:
-- This function generates the 2 CRC characters so it can be appended to the end of every packet being sent.
------------------------------------------------------------------------------------------------------------------*//*
void GenerateCRC(char pkt[GENERATE_CRC_TEST_SIZE], char generatedCRC[2]){ // GENERATE_CRC_TEST_SIZE = 1020
	unsigned short the_crc;

	the_crc = CRCCCITT(pkt, GENERATE_CRC_TEST_SIZE, 0xffff, 0);
	printf("Initial CRC value is 0x%04X\n", the_crc);

	generatedCRC[0] = (unsigned char)((the_crc >> 8) & 0xff);
	generatedCRC[1] = (unsigned char)(the_crc & 0xff);


}*/