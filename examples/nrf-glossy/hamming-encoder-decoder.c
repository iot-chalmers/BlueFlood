/* Based on code from:
David Cook
// RobotRoom.com
// June 20, 2014
// LoFi project for TheHackADayPrize
// http://hackaday.io/project/1552-LoFi
*/
#include <inttypes.h>
#include "contiki.h"
#include "nrf-radio-driver.h" /* For logs config and PRINTF() */
#include "ble-beacon-header.h"
#include "encode-decode-hamming-crc24.h"

#if USE_HAMMING_CODE
// This contains all of the precalculated parity values for a byte (8 bits).
// This is very fast, but takes up more program space than calculating on the fly.
static const uint8_t _hammingCalculateParityFast128[256]=
{ /* four-bits parity */
	 0,  3,  5,  6,  6,  5,  3,  0,  7,  4,  2,  1,  1,  2,  4,  7,
	 9, 10, 12, 15, 15, 12, 10,  9, 14, 13, 11,  8,  8, 11, 13, 14,
	10,  9, 15, 12, 12, 15,  9, 10, 13, 14,  8, 11, 11,  8, 14, 13,
	 3,  0,  6,  5,  5,  6,  0,  3,  4,  7,  1,  2,  2,  1,  7,  4,
	11,  8, 14, 13, 13, 14,  8, 11, 12, 15,  9, 10, 10,  9, 15, 12,
	 2,  1,  7,  4,  4,  7,  1,  2,  5,  6,  0,  3,  3,  0,  6,  5,
	 1,  2,  4,  7,  7,  4,  2,  1,  6,  5,  3,  0,  0,  3,  5,  6,
	 8, 11, 13, 14, 14, 13, 11,  8, 15, 12, 10,  9,  9, 10, 12, 15,
	12, 15,  9, 10, 10,  9, 15, 12, 11,  8, 14, 13, 13, 14,  8, 11,
	 5,  6,  0,  3,  3,  0,  6,  5,  2,  1,  7,  4,  4,  7,  1,  2,
	 6,  5,  3,  0,  0,  3,  5,  6,  1,  2,  4,  7,  7,  4,  2,  1,
	15, 12, 10,  9,  9, 10, 12, 15,  8, 11, 13, 14, 14, 13, 11,  8,
	 7,  4,  2,  1,  1,  2,  4,  7,  0,  3,  5,  6,  6,  5,  3,  0,
	14, 13, 11,  8,  8, 11, 13, 14,  9, 10, 12, 15, 15, 12, 10,  9,
	13, 14,  8, 11, 11,  8, 14, 13, 10,  9, 15, 12, 12, 15,  9, 10,
	 4,  7,  1,  2,  2,  1,  7,  4,  3,  0,  6,  5,  5,  6,  0,  3,
}; 

// Given a byte to transmit, this returns the parity as a nibble
uint8_t DL_HammingCalculateParity128(uint8_t value)
{
	return _hammingCalculateParityFast128[value];
}

// Given two bytes to transmit, this returns the parity
// as a byte with the lower nibble being for the first byte,
// and the upper nibble being for the second byte.
uint8_t DL_HammingCalculateParity2416(uint8_t first, uint8_t second)
{
	return (_hammingCalculateParityFast128[second]<<4) | _hammingCalculateParityFast128[first];
}

// If transmitting/writing only, you don't need to include this file.
// If receiving/reading, then this provides the methods to correct bit errors.

#define UNCORRECTABLE	0xFF
#define ERROR_IN_PARITY	0xFE
#define NO_ERROR		0x00

// Private table. Faster and more compact than multiple if statements.
static const uint8_t _hammingCorrect128Syndrome[16] =
{
	NO_ERROR,			// 0
	ERROR_IN_PARITY,	// 1
	ERROR_IN_PARITY,	// 2
	0x01,				// 3
	ERROR_IN_PARITY,	// 4
	0x02,				// 5
	0x04,				// 6
	0x08,				// 7
	ERROR_IN_PARITY,	// 8
	0x10,				// 9
	0x20,				// 10
	0x40,				// 11
	0x80,				// 12
	UNCORRECTABLE,		// 13
	UNCORRECTABLE,		// 14
	UNCORRECTABLE,		// 15
};

// Private method
// Give a pointer to a received byte,
// and given a nibble difference in parity (parity ^ calculated parity)
// this will correct the received byte value if possible.
// It returns the number of bits corrected:
// 0 means no errors
// 0 means one corrected error
// 1 means corrections not possible
static uint8_t DL_HammingCorrect128Syndrome(uint8_t* value, uint8_t syndrome)
{
	// Using only the lower nibble (& 0x0F), look up the bit
	// to correct in a table
	uint8_t correction = _hammingCorrect128Syndrome[syndrome & 0x0F];

	if (correction != NO_ERROR){
		if (correction == UNCORRECTABLE || value == NULL){
			return 1; // Non-recoverable error
		} else {
			if (correction != ERROR_IN_PARITY){
				*value ^= correction;
			}
			return 0; // 1-bit recoverable error;
		}
	}
	return 0; // No errors
}

// Given a pointer to a received byte and the received parity (as a lower nibble),
// this calculates what the parity should be and fixes the recevied value if needed.
// It returns the number of bits corrected:
// 0 means no errors
// 0 means one corrected error
// 1 means corrections not possible
uint8_t DL_HammingCorrect128(uint8_t* value, uint8_t parity)
{
	uint8_t syndrome;

	if (value == NULL)	{
		return 1; // Non-recoverable error
	}

	syndrome = DL_HammingCalculateParity128(*value) ^ parity;

	if (syndrome != 0){
		return DL_HammingCorrect128Syndrome(value, syndrome);
	}

	return 0; // No errors
}


// Given a pointer to a first value and a pointer to a second value and
// their combined given parity (lower nibble first parity, upper nibble second parity),
// this calculates what the parity should be and fixes the values if needed.
// It returns the number of bits corrected:
// 0 means no errors
// 0 means one corrected error
// 0 means two corrected errors
// 1 means corrections not possible
uint8_t DL_HammingCorrect2416(uint8_t* first, uint8_t* second, uint8_t parity)
{
	if (first == NULL || second == NULL){
		return 1; // Non-recoverable error
	}

	uint8_t syndrome = DL_HammingCalculateParity2416(*first, *second) ^ parity;

	if (syndrome != 0){
		return DL_HammingCorrect128Syndrome(first, syndrome) + DL_HammingCorrect128Syndrome(second, syndrome >> 4);
	}

	return 0; // No errors
}

static uint8_t encode_packet(uint8_t* msgin, uint8_t len, uint8_t* msgout){
  if(len % 2){
    PRINTF("Cannot encode due to odd packet length! %d\n", len);
    return 1; //works with even sizes only
  }
  uint32_t crc = crc24_calc(msgin, len);
  int i, j;
  uint8_t b1, b2, h;
  for(i = 0, j = 0; i < len - 1; i+=2){
    b1 = msgin[i]; 
    b2 = msgin[i+1];
    h = DL_HammingCalculateParity2416(b1, b2);
    msgout[j++] = b1;
    msgout[j++] = b2;
    msgout[j++] = h;
  }

  //for the 3bytes CRC 
  b2 = (crc >> 8UL) & 0xff;
  b1 = crc & 0xff;
  h = DL_HammingCalculateParity2416(b1, b2);
  msgout[j++] = b1;
  msgout[j++] = b2;
  msgout[j++] = h;

  b1 = (crc >> 16UL) & 0xff;
  h = DL_HammingCalculateParity128(b1);
  msgout[j++] = b1;
  msgout[j++] = h;

  return 0;
}

/* return number of errors + 128 if CRC error */
static uint8_t decode_packet(uint8_t* msgin, uint8_t *olen, uint8_t* msgout){
  /* First correct LEN */
  int i=0, j=0;
  uint8_t b1, b2, h, c = 0;
  b1 = msgin[i]; b2 = msgin[i+1]; h = msgin[i+2];
  c += DL_HammingCorrect2416(&b1, &b2, h);
  msgout[j++] = b1;
  msgout[j++] = b2;
  int len = b2;
  
  /*if(len % 3){
    PRINTF("Cannot decode due to wrong packet length!\n");
    return 1; //works with even sizes only
  }*/

  for(i = 3, j = 2; i < len - 2 /* loop 3 bytes */ -5 /*CRC*/; i+=3){
    b1 = msgin[i]; b2 = msgin[i+1]; h = msgin[i+2];
    c += DL_HammingCorrect2416(&b1, &b2, h);
    msgout[j++] = b1;
    msgout[j++] = b2;
  }
  //for the 3bytes CRC 
  b1 = msgin[len - 5]; 
  b2 = msgin[len - 4]; h = msgin[len - 3];
  c += DL_HammingCorrect2416(&b1, &b2, h);
  msgout[j++] = b1;
  uint32_t crc_rx = b1 + (b2 << 8UL);
  b1 = msgin[len - 2]; 
  h = msgin[len - 1];
  c += DL_HammingCorrect128(&b1, h);
  crc_rx += (b1 << 16UL);
  uint8_t decoded_msg_len = ((len-5)*2)/3; /* remove received crc and add the last byte */
  uint32_t crc = crc24_calc(msgout, decoded_msg_len);
  *olen = decoded_msg_len - 1;
  c += (crc != crc_rx)*128;
  return c;
}

uint8_t encode_ble_packet(ble_beacon_t* msgin, uint8_t* msgout){
  uint8_t len = msgin->radio_len+1;
  msgin->radio_len = ((len)*3)/2+ 5 /* encoded CRC*/;
  uint8_t c = encode_packet(msgin, len, msgout);
  msgin->radio_len = len - 1;
  return c;
}

uint8_t decode_ble_packet(uint8_t* msgin, ble_beacon_t* msgout){
  uint8_t len = 0;
  uint8_t c = decode_packet(msgin, &len, msgout);
  msgout->radio_len = len;
  return c;
}

#endif /*USE_HAMMING_CODE*/
