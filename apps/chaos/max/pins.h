/*
 * pins.h
 *
 *  Created on: 15 Jan 2017
 *      Author: olaf
 */

#ifndef APPS_ALARM_TEST_ALARM_PINS_H_
#define APPS_ALARM_TEST_ALARM_PINS_H_

#include "contiki.h"
#include <legacymsp430.h>

/* configure the lamp port */
/* flocklab GPIO from FE glossy */
#define SET_IOPIN(a,b)          do { P##a##OUT |=  BV(b); } while (0)
#define UNSET_IOPIN(a,b)        do { P##a##OUT &= ~BV(b); } while (0)
#define TOGGLE_IOPIN(a,b)       do { P##a##OUT ^=  BV(b); } while (0)
#define INIT_IOPIN_IN(a,b)      do { P##a##SEL &= ~BV(b); P##a##DIR &= ~BV(b); } while (0)
#define INIT_IOPIN_OUT(a,b)     do { P##a##SEL &= ~BV(b); P##a##DIR |=  BV(b); } while (0)
#define IOPIN_IS_SET(a,b)       ( P##a##IN & BV(b) )

/* single point gpio functions (ADC2/GIO1) */
#define INIT2_IOPIN_IN(...)  INIT_IOPIN_IN(__VA_ARGS__)
#define INIT2_IOPIN_OUT(...)  INIT_IOPIN_OUT(__VA_ARGS__)
#define INIT3_IOPIN_1(...)  INIT2_IOPIN_IN(__VA_ARGS__)
#define INIT3_IOPIN_0(...)  INIT2_IOPIN_OUT(__VA_ARGS__)
#define INIT_IOPIN_CONFIGURABLE(c,...) INIT3_IOPIN_##c(__VA_ARGS__)
//#define INIT_IOPIN_CONFIGURABLE3(c,a,b) INIT_IOPIN_##c(a,b)
#define single_point_init_io(...) INIT_IOPIN_CONFIGURABLE(__VA_ARGS__)

#define GPIO17 6,0
#define GPIO4 6,1
#define GPIO18 6,2
#define GPIO27 6,3
#define GPIO22 6,7
#define GPIO23 2,3
#define GPIO24 2,6
#define GPIO25 6,6

//place of GPIO value in the 8bit value
#define IGPIO17 (7)
#define IGPIO4 (6)
#define IGPIO18 (5)
#define IGPIO27 (4)
#define IGPIO22 (3)
#define IGPIO23 (2)
#define IGPIO24 (1)
#define IGPIO25 (0)
#define IGPIOXTRA_117 (8u)
#define IGPIOXTRA_207 (9u)
#define IGPIOXTRA_219 (10u)

#define ADC0 (6,0)
#define ADC1 (6,1)
#define ADC2 (6,2)
#define GIO1 (6,2)
#define ADC3 (6,3)
#define GIO0 (6,3)
#define SVSin (6,7)
#define ADC7 (6,7)
#define GIO2 (2,3)
#define GIO3 (2,6)
#define SVSout (2,6)
#define ADC6 (6,6)
#define DAC0 (6,6)

#define NEED3ARGS(a1,a2,a3) ( "[" #a1 " + " #a2 " + " #a3 "]" )
#define INVOKE_NEED3ARGS(...) NEED3ARGS(__VA_ARGS__)
#define MULTIARG()  ARG1, ARG2, ARG3



#define single_point_init_io_dst(...)  INIT_IOPIN_OUT(__VA_ARGS__)
#define single_point_init_io_src(...)  INIT_IOPIN_IN(__VA_ARGS__)
#define single_point_sample(...)       IOPIN_IS_SET(__VA_ARGS__)
#define single_point_on(...)           SET_IOPIN(__VA_ARGS__)
#define single_point_off(...)          UNSET_IOPIN(__VA_ARGS__)
//#define single_point_init_dst(...)     do{ single_point_init_io_dst(__VA_ARGS__); single_point_off(__VA_ARGS__); } while( 0 )
//#define single_point_init_src()     do{ single_point_init_io_src(__VA_ARGS__); } while( 0 )

//assignment of GPIO pin to node
#define N118 GPIO18
#define N209 GPIO18
#define N119 GPIO23
#define N217 GPIO23
#define N224 GPIO23
#define N206 GPIO24
#define N210 GPIO24
#define N117 GPIO27
#define N207 GPIO27
#define N226 GPIO27
#define N222 GPIO27

#define IN_DIR 1
#define OUT_DIR 0

#define REPRESENT_SAMPLE_IO(X,S) (single_point_sample(X) ? (uint16_t)(1u << (uint16_t)(S)) : 0)
#define EWSN_OUT_IO_BIT(X,S,G,F) do {if( (F) != 0 ) {\
  if((uint8_t)(1 << (S)) & (G)) {single_point_on(X);} else {single_point_off(X);}}}while(0)
static ALWAYS_INLINE void ewsn_init_gpio(uint16_t id);
static ALWAYS_INLINE uint16_t ewsn_sample_gpio(uint16_t id);
static ALWAYS_INLINE uint16_t ewsn_output_gpio(uint16_t id, uint16_t gpio, uint16_t gpio_old, uint16_t flags);
static ALWAYS_INLINE uint16_t ewsn_check_gpio(uint16_t id, uint16_t flags, uint16_t flags_old, uint16_t gpio, uint16_t gpio_old);

/*
• Point-to-point communication from node 118 to 209 (GPIO18)
• Point-to-multipoint communication from 119 to 217 & 224 (GPIO23)
• Point-to-point communication from node 206 to 210 (GPIO24)
• Multipoint-to-point communication from nodes
  117 & 207 & 226 to 222 (GPIO27)
*/
static ALWAYS_INLINE void ewsn_init_gpio(uint16_t id){
  switch(id){
  case 118:
    single_point_init_io(IN_DIR,GPIO18);
    break;;
  case 209:
    single_point_init_io(OUT_DIR,GPIO18);
    single_point_init_io(OUT_DIR,GPIO22);
    break;;
  case 119:
    single_point_init_io(IN_DIR,GPIO23);
    break;;
  case 217:
    single_point_init_io(OUT_DIR,GPIO23);
    break;;
  case 224:
    single_point_init_io(OUT_DIR,GPIO23);
    single_point_init_io(OUT_DIR,GPIO22);
    break;;
  case 206:
    single_point_init_io(IN_DIR,GPIO24);
    break;;
  case 210:
    single_point_init_io(OUT_DIR,GPIO24);
    break;;
  case 117:
    single_point_init_io(IN_DIR,GPIO27);
    break;;
  case 207:
    single_point_init_io(IN_DIR,GPIO27);
    break;;
  case 226:
    single_point_init_io(IN_DIR,GPIO27);
    break;;
  case 222:
    single_point_init_io(OUT_DIR,GPIO27);
    break;;
  case 213:
    single_point_init_io(IN_DIR,GPIO4);
    single_point_init_io(IN_DIR,GPIO17);
    break;;
  case 225:
    single_point_init_io(OUT_DIR,GPIO4);
    single_point_init_io(OUT_DIR,GPIO22);
    break;;
  case 108:
  case 200:
    single_point_init_io(OUT_DIR,GPIO17);
    break;;
  case 219:
  case 110:
    single_point_init_io(IN_DIR,GPIO25);
    break;;
  case 220:
    single_point_init_io(OUT_DIR,GPIO25);
    break;;
  case 201:
    single_point_init_io(IN_DIR,GPIO22);
    break;;
  case 211:
    single_point_init_io(OUT_DIR,GPIO22);
    break;;
  }
  single_point_off(GPIO18);
  single_point_off(GPIO23);
  single_point_off(GPIO24);
  single_point_off(GPIO27);
  single_point_off(GPIO4);
  single_point_off(GPIO17);
  single_point_off(GPIO25);
  single_point_off(GPIO22);
}


static ALWAYS_INLINE uint16_t ewsn_sample_gpio(uint16_t id){
  switch(id){
  case 118:
    return REPRESENT_SAMPLE_IO(GPIO18, IGPIO18);
  case 119:
    return REPRESENT_SAMPLE_IO(GPIO23, IGPIO23);
  case 206:
    return REPRESENT_SAMPLE_IO(GPIO24, IGPIO24);
  case 117:
    return REPRESENT_SAMPLE_IO(GPIO27, IGPIOXTRA_117);
  case 207:
    return REPRESENT_SAMPLE_IO(GPIO27, IGPIOXTRA_207);
  case 226:
    return REPRESENT_SAMPLE_IO(GPIO27, IGPIO27);
  case 213:
    return REPRESENT_SAMPLE_IO(GPIO4, IGPIO4) | REPRESENT_SAMPLE_IO(GPIO17, IGPIO17);
  case 219:
    return REPRESENT_SAMPLE_IO(GPIO25, IGPIOXTRA_219);
  case 110:
    return REPRESENT_SAMPLE_IO(GPIO25, IGPIO25);
  case 201:
    return REPRESENT_SAMPLE_IO(GPIO22, IGPIO22);
  }
  return 0;
}

//TESTBED_MAPPING {110,117,118,119,206,207,213,219,226,201,108,200,209,210,217,220,222,224,225,211,100,101,102,103,104,105,106,107,109,111,112,113,114,115,116,150,151,152,153,202,203,204,205,208,212,214,215,216,218,221,223}
  /* *
   * sources      110,117,118,119,206,207,213,219,226,201
   * destinations 108,200,209,210,217,220,222,224,225,211
   * */

#define F110 0
#define F117 1
#define F118 2
#define F119 3
#define F206 4
#define F207 5
#define F213 6
#define F219 7
#define F226 8
#define F201 9
#define N_SOURCES 10

static ALWAYS_INLINE uint16_t ewsn_output_gpio(uint16_t id, uint16_t gpio, uint16_t gpio_old, uint16_t flags){
  uint8_t output = 0;
  uint16_t FXXX = 0, IXXX = 0, FXXX2 = 0, IXXX2 = 0;
  uint16_t new_value = 0, old_value = 0, estimation = 0, est1 = 0, est2 = 0;

  output = gpio; //reduce 16bit to 8bit

  switch(id){
    case 209:
      FXXX = (1u << F118);
      IXXX = (1u << IGPIO18);
      FXXX2 = (1u << F201);
      IXXX2 = (1u << IGPIO22);
      EWSN_OUT_IO_BIT(GPIO18,IGPIO18,output,FXXX & flags);
      EWSN_OUT_IO_BIT(GPIO22,IGPIO22,output,FXXX2 & flags);
      break;;
    case 217:
      FXXX = (1u << F119);
      IXXX =  (1u << IGPIO23);
      EWSN_OUT_IO_BIT(GPIO23,IGPIO23,output,FXXX & flags);
      break;;
    case 224:
      FXXX = (1u << F119);
      IXXX =  (1u << IGPIO23);
      FXXX2 = (1u << F201);
      IXXX2 = (1u << IGPIO22);
      EWSN_OUT_IO_BIT(GPIO23,IGPIO23,output,FXXX & flags);
      EWSN_OUT_IO_BIT(GPIO22,IGPIO22,output,FXXX2 & flags);
      break;;
    case 210:
      FXXX =  (1u << F206);
      IXXX =  (1u << IGPIO24);
      EWSN_OUT_IO_BIT(GPIO24,IGPIO24,output,FXXX & flags);
      break;;
    case 222:
      //117 & 207 & 226
      FXXX =  (1u << F226) | (1u << F117) | (1u << F207);
      IXXX =  (1u << IGPIO27) | (1u<<IGPIOXTRA_117) | (1u<<IGPIOXTRA_207);

      est1 = ((flags & (1u << F117))) ? gpio & (1u << IGPIOXTRA_117) : gpio_old & (1u << IGPIOXTRA_117);
      est2 = ((flags & (1u << F207))) ? gpio & (1u << IGPIOXTRA_207) : gpio_old & (1u << IGPIOXTRA_207);
      estimation = ((flags & (1u << F226))) ? gpio & (1u << IGPIO27) : gpio_old & (1u << IGPIO27);
      estimation |= est1 | est2;
      output =( estimation & IXXX ) ? (1u<<IGPIO27) : 0;
      EWSN_OUT_IO_BIT(GPIO27,IGPIO27,output,FXXX & flags);
      break;;
    case 225:
      FXXX =  (1u << F213);
      IXXX =  (1u << IGPIO4);
      FXXX2 = (1u << F201);
      IXXX2 = (1u << IGPIO22);
      EWSN_OUT_IO_BIT(GPIO4,IGPIO4,output,FXXX & flags);
      EWSN_OUT_IO_BIT(GPIO22,IGPIO22,output,FXXX2 & flags);
      break;;
    case 108:
    case 200:
      FXXX =  (1u << F213);
      IXXX =  (1u << IGPIO17);
      EWSN_OUT_IO_BIT(GPIO17,IGPIO17,output,FXXX & flags);
      break;;
    case 220:
      FXXX =  (1u << F219) | (1u << F110);
      IXXX =  (1u << IGPIO25)| (1u<<IGPIOXTRA_219);

      est1 = ((flags & (1u << F219))) ? gpio & (1u << IGPIOXTRA_219) : gpio_old & (1u << IGPIOXTRA_219);
      estimation = ((flags & (1u << F110))) ? gpio & (1u << IGPIO25) : gpio_old & (1u << IGPIO25);
      estimation |= est1;
      output = (estimation & IXXX ) ? (1u<<IGPIO25) : 0;
      EWSN_OUT_IO_BIT(GPIO25,IGPIO25,output,FXXX & flags);
      break;;
    case 211:
      FXXX =  (1u << F201);
      IXXX =  (1u << IGPIO22);
      EWSN_OUT_IO_BIT(GPIO22,IGPIO22,output,FXXX & flags);
      break;;
    }

  if( FXXX && !estimation ){
    new_value = IXXX & gpio;
    old_value = IXXX & gpio_old;
    if( (flags & FXXX) == FXXX ){
      estimation = new_value;
    } else {
      estimation = old_value;
    }
  }
  if( FXXX2 && !estimation ){
    new_value = IXXX2 & gpio;
    old_value = IXXX2 & gpio_old;
    if( (flags & FXXX2) == FXXX2 ){
      estimation |= new_value;
    } else {
      estimation |= old_value;
    }
  }
  return estimation;
}

/* destination nodes: check if any of the monitored sources has contributed */
static ALWAYS_INLINE uint16_t ewsn_check_gpio(uint16_t id, uint16_t flags, uint16_t flags_old, uint16_t gpio, uint16_t gpio_old){
  uint16_t FXXX = 0, IXXX = 0, FXXX2 = 0, IXXX2 = 0;
  uint16_t new_value = 0, old_value = 0, estimation = 0, est1 = 0, est2 = 0;
  uint8_t set_new=0, full_new = 0;

  switch(id){
  case 209:
    FXXX = (1u << F118);
    IXXX = (1u << IGPIO18);
    FXXX2 = (1u << F201);
    IXXX2 = (1u << IGPIO22);
    break;;
  case 217:
    FXXX = (1u << F119);
    IXXX =  (1u << IGPIO23);
    break;;
  case 224:
    FXXX = (1u << F119);
    IXXX =  (1u << IGPIO23);
    FXXX2 = (1u << F201);
    IXXX2 = (1u << IGPIO22);
    break;;
  case 210:
    FXXX =  (1u << F206);
    IXXX =  (1u << IGPIO24);
    break;;
  case 222:
    //117 & 207 & 226
    FXXX =  (1u << F226) | (1u << F117) | (1u << F207);
    IXXX =  (1u << IGPIO27) | (1u<<IGPIOXTRA_117) | (1u<<IGPIOXTRA_207);
    est1 = ((flags & (1u << F117))) ? gpio & (1u << IGPIOXTRA_117) : gpio_old & (1u << IGPIOXTRA_117);
    est2 = ((flags & (1u << F207))) ? gpio & (1u << IGPIOXTRA_207) : gpio_old & (1u << IGPIOXTRA_207);
    estimation = ((flags & (1u << F226))) ? gpio & (1u << IGPIO27) : gpio_old & (1u << IGPIO27);
    estimation |= est1 | est2;
    break;;
  case 225:
    FXXX =  (1u << F213);
    IXXX =  (1u << IGPIO4);
    FXXX2 = (1u << F201);
    IXXX2 = (1u << IGPIO22);
    break;;
  case 108:
  case 200:
    FXXX =  (1u << F213);
    IXXX =  (1u << IGPIO17);
    break;;
  case 220:
    FXXX =  (1u << F219) | (1u << F110);
    IXXX =  (1u << IGPIO25)| (1u<<IGPIOXTRA_219);
    est1 = ((flags & (1u << F219))) ? gpio & (1u << IGPIOXTRA_219) : gpio_old & (1u << IGPIOXTRA_219);
    estimation = ((flags & (1u << F110))) ? gpio & (1u << IGPIO25) : gpio_old & (1u << IGPIO25);
    estimation |= est1;
    break;;
  case 211:
    FXXX =  (1u << F201);
    IXXX =  (1u << IGPIO22);
    break;;
  }
  if( FXXX && !estimation ){
    new_value = IXXX & gpio;
    old_value = IXXX & gpio_old;
    set_new = (new_value != 0) && ((flags & FXXX) != 0);
    full_new = (flags & FXXX) == FXXX;
    if( set_new || full_new ){
      estimation = new_value;
    } else {
      estimation = old_value;
    }
  }
  if( FXXX2 && !estimation ){
    new_value = IXXX2 & gpio;
    old_value = IXXX2 & gpio_old;
    set_new = (new_value != 0) && ((flags & FXXX2) != 0);
    full_new = (flags & FXXX2) == FXXX2;
    if( set_new || full_new ){
      estimation |= new_value;
    } else {
      estimation |= old_value;
    }
  }
  //estimation = ((flags & FXXX) ? new_value:0) | ((~flags & flags_old & FXXX)? :0)

  return estimation;
}
#define ACK_SIZE (N_SOURCES+6)
#define MAX_ACK_FLAGS_SET (0xffffu) //((1uL << ACK_SIZE)-1)

/* destination nodes: acknowledge reception from all monitored sources */
static ALWAYS_INLINE uint16_t ewsn_ack_gpio(uint16_t id, uint8_t my_node_index, uint16_t flags){
  uint16_t FXXX = 0, IXXX = 0, FXXX2 = 0, IXXX2 = 0;
  uint16_t ack = 0;

  switch(id){
  case 209:
    FXXX = (1u << F118);
    FXXX2 = (1u << F201);
    IXXX2 = (1u << (N_SOURCES+0));
    IXXX = FXXX;
    break;;
  case 217:
    FXXX = (1u << F119);
    IXXX = FXXX;
    break;;
  case 224:
    FXXX = (1u << F119);
    IXXX = (1u << (N_SOURCES+1));
    FXXX2 = (1u << F201);
    IXXX2 = (1u << (N_SOURCES+2));
    break;;
  case 210:
    FXXX =  (1u << F206);
    IXXX = FXXX;
    break;;
  case 222:
    //117 & 207 & 226
    FXXX =  (1u << F117) | (1u << F207) | (1u << F226);
    IXXX = FXXX;
    break;;
  case 225:
    FXXX =  (1u << F213);
    IXXX = FXXX;
    FXXX2 = (1u << F201);
    IXXX2 = (1u << (N_SOURCES+3));
    break;;
  case 108:
    FXXX =  (1u << F213);
    IXXX = (1u << (N_SOURCES+4));
    break;;
  case 200:
    FXXX =  (1u << F213);
    IXXX = (1u << (N_SOURCES+5));
    break;;
  case 220:
    FXXX =  (1u << F219) | (1u << F110);
    IXXX = FXXX;
    break;;
  case 211:
    FXXX =  (1u << F201);
    IXXX =  FXXX;
    break;;
    /* source nodes */
  /*
  case 118:
  case 119:
  case 206:
  case 117:
  case 207:
  case 226:
    //FXXX = 0x9999;FXXX = 0;
    //break;;
  */
  default:
    //FXXX = 0;
    break;;

  }
  if(FXXX != 0){ //one of the nodes above == a destination node
    if(IXXX == FXXX){
      ack = (flags & FXXX);
    } else {
      ack = (flags & FXXX) ? IXXX : 0;
    }
  }
  if(FXXX2 != 0){ //one of the nodes above == a destination node
    if(IXXX2 == FXXX2){
      ack |= (flags & FXXX2);
    } else {
      ack |= (flags & FXXX2) ? IXXX2 : 0;
    }
  }
  return (ack);
}

#if TESTBED == ewsn

#define SET_PIN(a,b)          do { } while (0)
#define UNSET_PIN(a,b)        do { } while (0)
#define TOGGLE_PIN(a,b)       do { } while (0)
#define INIT_PIN_IN(a,b)      do { } while (0)
#define INIT_PIN_OUT(a,b)     do { } while (0)
#define PIN_IS_SET(a,b)       ( 0 )

#else

#define SET_PIN(a,b)          SET_IOPIN(a,b)
#define UNSET_PIN(a,b)        UNSET_IOPIN(a,b)
#define TOGGLE_PIN(a,b)       TOGGLE_IOPIN(a,b)
#define INIT_PIN_IN(a,b)      INIT_IOPIN_IN(a,b)
#define INIT_PIN_OUT(a,b)     INIT_IOPIN_OUT(a,b)
#define PIN_IS_SET(a,b)       IOPIN_IS_SET(a,b)

#endif

// UserINT (P2.7)
#define SET_PIN_USERINT      SET_PIN(2,7)
#define UNSET_PIN_USERINT    UNSET_PIN(2,7)
#define TOGGLE_PIN_USERINT   TOGGLE_PIN(2,7)
#define INIT_PIN_USERINT_IN  INIT_PIN_IN(2,7)
#define INIT_PIN_USERINT_OUT INIT_PIN_OUT(2,7)
#define PIN_USERINT_IS_SET   PIN_IS_SET(2,7)

// GIO2 (P2.3)
#define SET_PIN_GIO2         SET_PIN(2,3)
#define UNSET_PIN_GIO2       UNSET_PIN(2,3)
#define TOGGLE_PIN_GIO2      TOGGLE_PIN(2,3)
#define INIT_PIN_GIO2_IN     INIT_PIN_IN(2,3)
#define INIT_PIN_GIO2_OUT    INIT_PIN_OUT(2,3)
#define PIN_GIO2_IS_SET      PIN_IS_SET(2,3)

// ADC0 (P6.0)
#define SET_PIN_ADC0         SET_PIN(6,0)
#define UNSET_PIN_ADC0       UNSET_PIN(6,0)
#define TOGGLE_PIN_ADC0      TOGGLE_PIN(6,0)
#define INIT_PIN_ADC0_IN     INIT_PIN_IN(6,0)
#define INIT_PIN_ADC0_OUT    INIT_PIN_OUT(6,0)
#define PIN_ADC0_IS_SET      PIN_IS_SET(6,0)

// ADC1 (P6.1)
#define SET_PIN_ADC1         SET_PIN(6,1)
#define UNSET_PIN_ADC1       UNSET_PIN(6,1)
#define TOGGLE_PIN_ADC1      TOGGLE_PIN(6,1)
#define INIT_PIN_ADC1_IN     INIT_PIN_IN(6,1)
#define INIT_PIN_ADC1_OUT    INIT_PIN_OUT(6,1)
#define PIN_ADC1_IS_SET      PIN_IS_SET(6,1)

// ADC2 (P6.2) -> LED3
#define SET_PIN_ADC2         SET_PIN(6,2)
#define UNSET_PIN_ADC2       UNSET_PIN(6,2)
#define TOGGLE_PIN_ADC2      TOGGLE_PIN(6,2)
#define INIT_PIN_ADC2_IN     INIT_PIN_IN(6,2)
#define INIT_PIN_ADC2_OUT    INIT_PIN_OUT(6,2)
#define PIN_ADC2_IS_SET      PIN_IS_SET(6,2)

// ADC3 (P6.3)
#define SET_PIN_ADC3         SET_PIN(6,3)
#define UNSET_PIN_ADC3       UNSET_PIN(6,3)
#define TOGGLE_PIN_ADC3      TOGGLE_PIN(6,3)
#define INIT_PIN_ADC3_IN     INIT_PIN_IN(6,3)
#define INIT_PIN_ADC3_OUT    INIT_PIN_OUT(6,3)
#define PIN_ADC3_IS_SET      PIN_IS_SET(6,3)

// DAC0/ADC6 (P6.6) -> LED2
#define SET_PIN_ADC6         SET_PIN(6,6)
#define UNSET_PIN_ADC6       UNSET_PIN(6,6)
#define TOGGLE_PIN_ADC6      TOGGLE_PIN(6,6)
#define INIT_PIN_ADC6_IN     INIT_PIN_IN(6,6)
#define INIT_PIN_ADC6_OUT    INIT_PIN_OUT(6,6)
#define PIN_ADC6_IS_SET      PIN_IS_SET(6,6)

// SVSin/ADC7 (P6.7) -> LED1
#define SET_PIN_ADC7         SET_PIN(6,7)
#define UNSET_PIN_ADC7       UNSET_PIN(6,7)
#define TOGGLE_PIN_ADC7      TOGGLE_PIN(6,7)
#define INIT_PIN_ADC7_IN     INIT_PIN_IN(6,7)
#define INIT_PIN_ADC7_OUT    INIT_PIN_OUT(6,7)
#define PIN_ADC7_IS_SET      PIN_IS_SET(6,7)

// GPIO3/SVSout (P2.6)
#define SET_PIN_GPIO3        SET_PIN(2,6)
#define UNSET_PIN_GPIO3       UNSET_PIN(2,6)
#define TOGGLE_PIN_GPIO3      TOGGLE_PIN(2,6)
#define INIT_PIN_GPIO3_IM     INIT_PIN_IN(2,6)
#define INIT_PIN_GPIO3_OUT    INIT_PIN_OUT(2,6)
#define PIN_GPIO3_IS_SET      PIN_IS_SET(2,6)

#endif /* APPS_ALARM_TEST_ALARM_PINS_H_ */
