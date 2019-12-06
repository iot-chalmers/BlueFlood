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
#define CTH_TESTBED 9

//minimal hello_world for extracting node IDs
#define TEST_HELLO_WORLD (TESTBED==HELLOWORLD_TESTBED)
//test CT with two nodes, where each node sends alone for 10 rounds to measure individual links
#define TWO_NODES_EXPERIMENT (TESTBED==WIRED_TESTBED)

#ifndef TESTBED_DIAMETER
#define TESTBED_DIAMETER (4)
#endif
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
#define TESTBED_DIAMETER (2)
//#define TESTBED_IDS (uint32_t[]){0x7b150560UL, 0xddf8a17fUL, 0x8e28b37dUL, 0x1c30a5f9UL, 0x70f96a53UL, 0x5cfdb15UL, 0x724d2a67UL, 0x7475364dUL, 0x74231ca7UL, 0x2fa5d86eUL}
//#define TESTBED_PI_IDS (uint8_t[]){4,5,6,7,8,10,20,21,22,28}
#define TESTBED_IDS (uint32_t[]){0x4c7a76aeUL, 0x3f55b1ca, 0x5cfdb15UL, 0x1204d131UL, 0xb9514f78UL, 0xe158ae56UL, 0x4568e2a6UL, 0x8d5a02cUL}
#define TESTBED_PI_IDS (uint8_t[]){2,3,10,12,16,17,19,20}
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#elif TESTBED==IOTLAB_SACLAY
#define TESTBED_DIAMETER (2)
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

#define MY_ADV_ADDRESS_HI 0xA1A1U

#define TESTBED_IDS (uint32_t[]){0x4da8f2cb, 0x7c2757b3}
#define TESTBED_PI_IDS (uint8_t[]){1,2}
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#elif TESTBED==GRAZ_TESTBED

#define TESTBED_DIAMETER (5)
#define TESTBED_IDS (uint32_t[]){ 0x310c76f7,  0x92dcf7d2,  0xbc490582,  0xab51cee7,  0x3ee55eab,  0x9daeea76,  0xdbf423a,  0x515bdc2a,  0x167c7c75,  0xe6a5f30e,  0xdded6301,  0xa01ae253,  0xa578336,  0x566c29d8,  0x58fc2226,  0xc6e11c58,  0x78ce6791,  0xd4f3c2f3,  0xe4324b97,  0x122611e5,  0xf111de36,  0x97d52940,  0x92c82336,  0xbd95c7bb,  0x54b410fb,  0x4993dd66,  0x48698a8b,  0x4692c6b5,  0xc6238a40,  0x157bc03e,  0x192e724,  0xcd0e34d4,  0xc13e545b,  0x55c93b01,  0x1723391c,  0x60256590,  0x1717d05,  0xc8937e27,  0x3a70e9ae,  0x80adfe3c,  0x73beb61a,  0x80d0b065,  0x506fcdaa,  0xcc49bcab,  0xd37b922f,  0x7424d2d8,  0x652725e7,  0xb3838267}

#define TESTBED_PI_IDS (uint8_t[]){100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227}

#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#elif TESTBED==CAU_TESTBED
#define TESTBED_DIAMETER (3)
// 1       #@ Sep 10 2019 16:58:01, ID: 0xa8d4c97d, 
// 2       #@ Sep 10 2019 16:58:01, ID: 0x921a4434, 
// 3       #@ Sep 10 2019 16:58:01, ID: 0x43c18ccc, 
// 4       #@ Sep 10 2019 16:58:01, ID: 0xb4fddd93, 
// 5       #@ Sep 10 2019 16:58:01, ID: 0x27e5a59, 
// 6       #@ Sep 10 2019 16:58:01, ID: 0x8036b5ea, 
// 7       #@ Sep 10 2019 16:58:01, ID: 0x4a8f1926, 
// 8       #@ Sep 10 2019 16:58:01, ID: 0x39f7b9e3, 
// 9       #@ Sep 10 2019 16:58:01, ID: 0xd73c5005,
// 10      #@ Sep 10 2019 16:58:01, ID: 0x7dd93314, 
// 12      #@ Sep 10 2019 16:58:01, ID: 0x9fb4c897, 
// 13      #@ Sep 10 2019 16:58:01, ID: 0x22087f8e, 
// 14      #@ Sep 10 2019 16:58:01, ID: 0x9463fd32, 
// 15      #@ Sep 10 2019 16:58:01, ID: 0xece2f14c, 
// 16      #@ Sep 10 2019 16:58:01, ID: 0x1add226c, 
// 17      #@ Sep 10 2019 16:58:01, ID: 0xbd08d46b, 
// 18      #@ Sep 10 2019 16:58:01, ID: 0xe7c767e3, 
// 19      #@ Sep 10 2019 16:58:01, ID: 0x1a36886, 
// 20      #@ Sep 10 2019 16:58:01, ID: 0x59c34e8f, 
// 21      #@ Oct 10 2019         , ID: 0xbda9a2e0, 

#define TESTBED_IDS (uint32_t[]){0xa8d4c97d,0x921a4434,0x43c18ccc, 0xb4fddd93, 0x27e5a59,0x8036b5ea,0x4a8f1926,0x39f7b9e3,0xd73c5005,0x7dd93314,0x9fb4c897,0x22087f8e,0x9463fd32,0xece2f14c,0x1add226c,0xbd08d46b,0xe7c767e3,0x1a36886,0x59c34e8f,0xbda9a2e0}
#define TESTBED_PI_IDS (uint8_t[]){1,2,3,4,5,6,7,8,9,10,12,13,14,15,16,17,18,19,20,21}
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#elif TESTBED==CTH_TESTBED

#define MY_ADV_ADDRESS_HI 0xA1A1U

#define mote10  (0x9b5fdb83UL)
#define mote11  (0xa004c2efUL)
#define mote12  (0xea33de4cUL)
#define mote13  (0x38c0457aUL)

#define TESTBED_IDS (uint32_t[]){mote10,mote11,mote12,mote13}
#define TESTBED_PI_IDS (uint8_t[]){10,11,12,13} 
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#elif TESTBED==TABLE_TESTBED

#define MY_ADV_ADDRESS_HI 0xA1A1U

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
#define TESTBED_IDS (uint32_t[]){\
mote20,mote21,mote22,mote24,\
mote20,mote21,mote22,mote24,\
mote20,mote21,mote22,mote24,\
mote20,mote21,mote22,mote24,\
mote20,mote21,mote22,mote24\
}
#define TESTBED_PI_IDS (uint8_t[]){\
20,21,22,24\
20,21,22,24\
20,21,22,24\
20,21,22,24\
20,21,22,24\
} 
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#elif TESTBED==WIRED_TESTBED

#define MY_ADV_ADDRESS_HI 0xA1A1U

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

#define MY_ADV_ADDRESS_HI 0xA1A1U

//does not really matter... Thoe goal of this mode is to extract the mote IDs to put in your testbed configuration
#define TESTBED_IDS (uint32_t[]){0x724d2a67UL}
#define TESTBED_PI_IDS (uint8_t[]){20}
#define TESTBED_SIZE sizeof(TESTBED_PI_IDS)

#else
#error "Define TESTBED!"
#endif

#endif /* _TESTBED_H_ */