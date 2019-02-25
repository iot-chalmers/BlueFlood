#ifndef BLE_BEACON_HEADER_H
#define BLE_BEACON_HEADER_H

#ifdef PACKET_TEST_SIZE_CONF
#define SIZE_OF_UUID (PACKET_TEST_SIZE_CONF-22)
#else
#define SIZE_OF_UUID (16)
#endif

typedef struct __attribute__((packed)) ble_beacon_struct {
  uint8_t pdu_header; //S0
  uint8_t radio_len; //LEN
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
} ble_beacon_t;
#endif /* BLE_BEACON_HEADER_H */
