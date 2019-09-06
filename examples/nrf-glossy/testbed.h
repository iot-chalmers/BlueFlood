#ifndef _TESTBED_H_
#define _TESTBED_H_

/* testbeds definitions */
#define HOME_TESTBED 1
#define IOTLAB_SACLAY 2
#define WIRED_TESTBED 3
#define HELLOWORLD_TESTBED 4
#define EXT_TESTBED_LSB 5
#define EXT_TESTBED_TABLE 6
#define CAU_TESTBED 7
#define TABLE_TESTBED 8

//minimal hello_world for extracting node IDs
#define TEST_HELLO_WORLD (TESTBED==HELLOWORLD_TESTBED)
//test CT with two nodes, where each node sends alone for 10 rounds to measure individual links
#define TWO_NODES_EXPERIMENT (TESTBED==WIRED_TESTBED)

/* Put the node in sleep forever: for disabling nodes after jobs are done in the testbed. 
* Do not enable this for normal use. This disables the mote.
*/
#ifndef TEST_SLEEP_NODE_FOREVER
#define TEST_SLEEP_NODE_FOREVER 0
#endif

#ifndef ROUND_ROBIN_INITIATOR
#define ROUND_ROBIN_INITIATOR 0
#endif

#ifndef INITATOR_NODE_INDEX
#define INITATOR_NODE_INDEX 0 
#endif

#if TESTBED==HOME_TESTBED

//#define TESTBED_IDS (uint32_t[]){0x7b150560UL, 0xddf8a17fUL, 0x8e28b37dUL, 0x1c30a5f9UL, 0x70f96a53UL, 0x5cfdb15UL, 0x724d2a67UL, 0x7475364dUL, 0x74231ca7UL, 0x2fa5d86eUL}
//#define TESTBED_PI_IDS (uint8_t[]){4,5,6,7,8,10,20,21,22,28}
#define TESTBED_IDS (uint32_t[]){0x4c7a76aeUL, 0x3f55b1ca, 0x5cfdb15UL, 0x1204d131UL, 0xb9514f78UL, 0xe158ae56UL, 0x4568e2a6UL, 0x8d5a02cUL}
#define TESTBED_PI_IDS (uint8_t[]){2,3,10,12,16,17,19,20}
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#elif TESTBED==IOTLAB_SACLAY

#define TESTBED_IDS (uint32_t[]){0x7e32d644, 0xa54d2ff3, 0x45d4fcf2, 0xda3bbcd5, 0x8d503740, 0xe9363f67, 0xdae77c6c, 0x27327a1d, 0xc94a0a7f, 0xd531cac5}
#define TESTBED_PI_IDS (uint8_t[]){1,2,3,4,5,6,7,8,9,10}
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#elif TESTBED==EXT_TESTBED_LSB

#define TESTBED_IDS (uint32_t[]){0x30a1f91b, 0xa5bb378d, 0x53486dc5, 0x05838afd, 0x87748c61, \ 
                                0x9bd0657d, 0xc184d1d6, 0xa6a0dac7, 0x14aeccfb, 0xfb424437, \ 
                                0x0ef20233, 0xbd3b390c, 0xd29516a8, 0xe41838b8, 0xc53a20ee}
#define TESTBED_PI_IDS (uint8_t[]){1,2,3,4,5,6,8,9,10,11,12,13,14,15,16}
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#elif TESTBED==EXT_TESTBED_TABLE

#define TESTBED_IDS (uint32_t[]){0x4da8f2cb, 0x7c2757b3}
#define TESTBED_PI_IDS (uint8_t[]){1,2}
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#elif TESTBED==CAU_TESTBED

// #define TESTBED_IDS (uint32_t[]){0x43c18ccc, 0xb4fddd93, 0x27e5a59}
// #define TESTBED_PI_IDS (uint8_t[]){3,4,5}

#define TESTBED_IDS (uint32_t[]){0xa8d4c97d,0x43c18ccc, 0xb4fddd93, 0x27e5a59,0x9fb4c897,0x22087f8e,0x9463fd32,0xece2f14c,0x1add226c,0xbd08d46b}
//note that nodes 12,13 ids are wrong! update them!
#define TESTBED_PI_IDS (uint8_t[]){1,3,4,5,12,13,14,15,16,17}
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#elif TESTBED==TABLE_TESTBED

#define mote4  (0x7b150560UL)
#define mote5  (0xddf8a17fUL)
#define mote6  (0x8e28b37dUL)
#define mote7  (0x1c30a5f9UL)
#define mote8  (0x70f96a53UL)
#define mote28  (0x2fa5d86eUL)
/* 52840 boards */
#define pi10     (0x5cfdb15UL)
#define mote20  (0x724d2a67UL)
#define mote21  (0x7475364dUL)
#define mote22  (0x74231ca7UL)
#define mote24  (0xef70521UL)

//#define TESTBED_IDS (uint32_t[]){0x7b150560UL, 0xddf8a17fUL, 0x8e28b37dUL, 0x1c30a5f9UL, 0x70f96a53UL, 0x5cfdb15UL, 0x724d2a67UL, 0x7475364dUL, 0x74231ca7UL, 0x2fa5d86eUL}
//#define TESTBED_PI_IDS (uint8_t[]){4,5,6,7,8,10,20,21,22,28}
#define TESTBED_IDS (uint32_t[]){mote20,mote21,mote22,mote24}
#define TESTBED_PI_IDS (uint8_t[]){20,21,22,24} 
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#elif TESTBED==WIRED_TESTBED

#define mote4  (0x7b150560UL)
#define mote5  (0xddf8a17fUL)
#define mote6  (0x8e28b37dUL)
#define mote7  (0x1c30a5f9UL)
#define mote8  (0x70f96a53UL)
#define mote28  (0x2fa5d86eUL)
/* 52840 boards */
#define pi10     (0x5cfdb15UL)
#define mote20  (0x724d2a67UL)
#define mote21  (0x7475364dUL)
#define mote22  (0x74231ca7UL)
#define mote24  (0xef70521UL)

//#define TESTBED_IDS (uint32_t[]){0x7b150560UL, 0xddf8a17fUL, 0x8e28b37dUL, 0x1c30a5f9UL, 0x70f96a53UL, 0x5cfdb15UL, 0x724d2a67UL, 0x7475364dUL, 0x74231ca7UL, 0x2fa5d86eUL}
//#define TESTBED_PI_IDS (uint8_t[]){4,5,6,7,8,10,20,21,22,28}
#define TESTBED_IDS (uint32_t[]){mote20,mote21,mote22,mote24}
#define TESTBED_PI_IDS (uint8_t[]){20,21,22,24} 
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#elif TESTBED==HELLOWORLD_TESTBED

//does not really matter... Thoe goal of this mode is to extract the mote IDs to put in your testbed configuration
#define TESTBED_IDS (uint32_t[]){0x724d2a67UL}
#define TESTBED_PI_IDS (uint8_t[]){20}
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#else
#error "Define TESTBED!"
#endif

#endif /* _TESTBED_H_ */