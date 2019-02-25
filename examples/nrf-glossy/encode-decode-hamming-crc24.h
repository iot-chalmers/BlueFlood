#ifndef ENCODE_DECODE_HAMMING_CRC24_H
#define ENCODE_DECODE_HAMMING_CRC24_H

#include "ble-beacon-header.h"
#define USE_HAMMING_CODE 0

#if USE_HAMMING_CODE
uint32_t crc24_calc(uint8_t *cp, unsigned int len);
uint8_t decode_ble_packet(uint8_t* msgin, ble_beacon_t* msgout);
uint8_t encode_ble_packet(ble_beacon_t* msgin, uint8_t* msgout);
#endif //USE_HAMMING_CODE

#endif /* ENCODE_DECODE_HAMMING_CRC24_H */

