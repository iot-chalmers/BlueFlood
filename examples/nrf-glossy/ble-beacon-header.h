#ifndef BLE_BEACON_HEADER_H
#define BLE_BEACON_HEADER_H

#include "nrf-radio-driver.h"

#ifdef PACKET_TEST_SIZE_CONF
  #if ((PACKET_TEST_SIZE_CONF) > 22)
  #define SIZE_OF_UUID ((PACKET_TEST_SIZE_CONF)-22)
  #else 
  #define SIZE_OF_UUID MAX(((PACKET_TEST_SIZE_CONF)-12),0)
  #endif /*(PACKET_TEST_SIZE_CONF > 22)*/
#else
#define SIZE_OF_UUID (16)
#endif /* PACKET_TEST_SIZE_CONF */
/* 
#define PACKET_IBEACON_FORMAT ((RADIO_MODE_CONF != RADIO_MODE_MODE_Ieee802154_250Kbit) \
                              && ( !defined(PACKET_TEST_SIZE_CONF) \
                                  || (PACKET_TEST_SIZE_CONF > 22) ) )
*/
#define PACKET_IBEACON_FORMAT ( !defined(PACKET_TEST_SIZE_CONF) \
                                || (PACKET_TEST_SIZE_CONF > 22) )

#if (PACKET_IBEACON_FORMAT)
typedef struct __attribute__((packed)) ble_beacon_struct {
  #if (RADIO_MODE_CONF == RADIO_MODE_MODE_Ieee802154_250Kbit)
  /** LEN field shall come first, but otherwise the packet does not follow standard 802.15.4 */
  uint8_t radio_len; //LEN
  uint8_t pdu_header; //S0
  #else
  uint8_t pdu_header; //S0
  uint8_t radio_len; //LEN
  #endif

/*   union {
    struct {
      //uint16_t rfu_2:2, pdu_length:6, rx_add:1, tx_add:1, rfu:2, pdu_type:4;
      uint8_t rx_add:1, tx_add:1, rfu:2, pdu_type:4;
    };
    uint8_t pdu_header;
  }; */
  uint32_t adv_address_low;
  uint16_t adv_address_hi;
  /* ibeacon */
  uint8_t ad_flags_length;
  uint8_t ad_flags_type;
  uint8_t ad_flags_data;
  uint8_t ad_length;
  uint8_t ad_type;
  uint16_t company_id;
  uint16_t beacon_type;
  uint8_t uuid[SIZE_OF_UUID];
  union{
    uint16_t major;
    uint16_t round;
  };
  union{
    uint16_t minor;
    struct{
      uint8_t slot;
      uint8_t turn;
    };
  };
  uint8_t power;
  #if (RADIO_MODE_CONF == RADIO_MODE_MODE_Ieee802154_250Kbit)
  union{
    uint16_t CRC_OR_RSSI_LQI;
    struct{
      uint8_t lqi;
      uint8_t DUMMY;
    }
  }
  #endif
} ble_beacon_t;
#else
typedef struct __attribute__((packed)) ble_beacon_struct {
  #if (RADIO_MODE_CONF == RADIO_MODE_MODE_Ieee802154_250Kbit)
  /** LEN field shall come first, but otherwise the packet does not follow standard 802.15.4 */
  uint8_t radio_len; //LEN
  uint8_t pdu_header; //S0
  #else
  uint8_t pdu_header; //S0
  uint8_t radio_len; //LEN
  #endif

  uint32_t adv_address_low;
  uint16_t adv_address_hi;
  uint8_t uuid[SIZE_OF_UUID];
  union{
    uint16_t major;
    uint16_t round;
  };
  union{
    uint16_t minor;
    struct{
      uint8_t slot;
      uint8_t turn;
    };
  };
  #if (RADIO_MODE_CONF == RADIO_MODE_MODE_Ieee802154_250Kbit)
  union{
    uint16_t CRC_OR_RSSI_LQI;
    struct{
      uint8_t lqi;
      uint8_t DUMMY;
    }
  }
  #endif
} ble_beacon_t;
#endif

#endif /* BLE_BEACON_HEADER_H */
