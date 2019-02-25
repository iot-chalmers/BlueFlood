#ifndef _TESTBED_H_
#define _TESTBED_H_
#define HOME_TESTBED 1
#define IOTLAB_SACLAY 2
#define WIRED_TESTBED 3
#define HELLOWORLD_TESTBED 4

#ifndef INITATOR_NODE_INDEX
#define INITATOR_NODE_INDEX 0 //node 2
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

#elif TESTBED==WIRED_TESTBED

#define TESTBED_IDS (uint32_t[]){0x724d2a67UL, 0x7475364dUL, 0x74231ca7UL}
#define TESTBED_PI_IDS (uint8_t[]){20,21,22}
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