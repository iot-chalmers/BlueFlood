/*
 * cooja-debug.h
 *
 *  Created on: Jan 12, 2011
 *      Author: simonduq
 */

#ifndef COOJA_DEBUG_H_
#define COOJA_DEBUG_H_

#if !ENABLE_COOJA_DEBUG

#define HEXC(c)
#define COOJA_DEBUG_STR(str)
#define COOJA_DEBUG_STRX(str, val, width)
#define COOJA_DEBUG_POINT
#define COOJA_DEBUG_CINT(c, val)
#define COOJA_DEBUG_INT(val)
#define COOJA_DEBUG_INTH(val)
#define COOJA_DEBUG_PRINTF(...)
#define COOJA_DEBUG_PRINT_INT(C)
#define COOJA_DEBUG_PRINT_HEX(C)
#define COOJA_DEBUG_ADDR(addr)
#define COOJA_DEBUG_LINE() //PRINTF("%s @ %d\n", __FILE__,__LINE__)

#else

#include <stdio.h>

volatile char *cooja_debug_ptr;
volatile char cooja_debug_point;
volatile uint32_t cooja_randomseed;
volatile uint16_t cooja_request_randomseed;

#define HEXC(c) (((c) & 0xf) <= 9 ? ((c) & 0xf) + '0' : ((c) & 0xf) + 'a' - 10)
#define COOJA_DEBUG_STR(str) do { cooja_debug_ptr = str; } while(0);
#define COOJA_DEBUG_STRX(msg, val, width)                      \
        do {                                            \
          char tmp[64] = msg;                           \
          uint32_t v = (uint32_t)(val);                 \
          int i,j; i = j = MIN( strlen(msg) + width + 1, 64 ); \
          tmp[--i] = 0;                                 \
          if(v == 0) tmp[--i] = '0';                    \
          while( i > j - width - 1) { tmp[--i] = HEXC(v%16); v /= 16; }  \
          cooja_debug_ptr = tmp;                        \
        } while(0);
#define COOJA_DEBUG_X(c, val, width)                    \
        do {                                            \
          char tmp[10]="L"; tmp[0]=c;                   \
          uint32_t v = (uint32_t)(val);                 \
          int i,j; i = j = MIN( 1 + width + 1, 10 );    \
          tmp[--i] = 0;                                 \
          if(v == 0) tmp[--i] = '0';                    \
          while( i > j - width - 1) { tmp[--i] = HEXC(v%16); v /= 16; }  \
          cooja_debug_ptr = tmp;                        \
        } while(0);
#define COOJA_DEBUG_LINE() COOJA_DEBUG_X('L',__LINE__,4)
#define COOJA_DEBUG_POINT do { cooja_debug_point = !cooja_debug_point; } while(0);
#define COOJA_DEBUG_CINT(c, val)
//#define COOJA_DEBUG_CINT(c, val) if(node_id == 3) { char tmp[11] = {0}; uint32_t v = (val); int i=10; if(v==0) tmp[--i] = '0'; while(v) { tmp[--i] = '0' + v%10; v /= 10; } tmp[--i] = (c); cooja_debug_ptr = tmp+i;  }
//#define COOJA_DEBUG_CINT(c, val) if(node_id == 6) { char tmp[11] = {0}; uint32_t v = (val); int i=10; if(v==0) tmp[--i] = '0'; while(v) { tmp[--i] = '0' + v%10; v /= 10; } tmp[--i] = (c); cooja_debug_ptr = tmp+i;  }
#define COOJA_DEBUG_INT(val) do { char tmp[10] = {0}; uint32_t v = (uint32_t)(val); int i=9; if(v==0) tmp[--i] = '0'; while(v) { tmp[--i] = '0' + v%10; v /= 10; } cooja_debug_ptr = tmp+i;  } while(0);
#define COOJA_DEBUG_INTH(val) do { char tmp[5] = {0}; uint16_t v = (uint16_t)(val); tmp[0] = HEXC(v>>12); tmp[1] = HEXC(v>>8); tmp[2] = HEXC(v>>4); tmp[3] = HEXC(v); cooja_debug_ptr = tmp;  } while(0);
#define COOJA_DEBUG_PRINTF(...) do { char tmp[200]; snprintf(tmp, sizeof(tmp), __VA_ARGS__); cooja_debug_ptr = tmp; } while(0);
#define COOJA_DEBUG_PRINT_INT(C) do { char tmp[5]; snprintf(tmp, sizeof(tmp), "%u", C); cooja_debug_ptr = tmp; } while(0);
#define COOJA_DEBUG_PRINT_HEX(C) do { char tmp[6]; snprintf(tmp, sizeof(tmp), "%x", C); cooja_debug_ptr = tmp; } while(0);
//#define COOJA_DEBUG_ADDR(addr) { int i; COOJA_DEBUG_STR("addr"); for(i=0;i<8;i++) COOJA_DEBUG_INT(((u8_t *)addr)[i]); COOJA_DEBUG_STR("end addr");}
//#define COOJA_DEBUG_ADDR16(addr) { int i; COOJA_DEBUG_STR("addr"); for(i=0;i<16;i++) COOJA_DEBUG_INT(((u8_t *)addr)[i]); COOJA_DEBUG_STR("end addr");}
#define COOJA_DEBUG_ADDR(addr) COOJA_DEBUG_PRINTF("addr %u %u %u %u %u %u %u %u\n", ((u8_t *)addr)[7], ((u8_t *)addr)[6], ((u8_t *)addr)[5], ((u8_t *)addr)[4], ((u8_t *)addr)[3], ((u8_t *)addr)[2], ((u8_t *)addr)[1], ((u8_t *)addr)[0]);
#define COOJA_DEBUG_ADDR16(addr) COOJA_DEBUG_PRINTF("ipaddr %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u", ((u8_t *)addr)[15], ((u8_t *)addr)[14], ((u8_t *)addr)[13], ((u8_t *)addr)[12], ((u8_t *)addr)[11], ((u8_t *)addr)[10], ((u8_t *)addr)[9], ((u8_t *)addr)[8], ((u8_t *)addr)[7], ((u8_t *)addr)[6], ((u8_t *)addr)[5], ((u8_t *)addr)[4], ((u8_t *)addr)[3], ((u8_t *)addr)[2], ((u8_t *)addr)[1], ((u8_t *)addr)[0]);

#endif /* DISABLE_COOJA_DEBUG */
#endif /* COOJA_DEBUG_H_ */
