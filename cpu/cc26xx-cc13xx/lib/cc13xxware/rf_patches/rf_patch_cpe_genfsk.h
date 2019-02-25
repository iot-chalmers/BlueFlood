/******************************************************************************
*  Filename:       rf_patch_cpe_genfsk.h
*  Revised:        2015-06-09 10:47:36 +0100 (ti, 09 Jun 2015)
*  Revision:       
*
*  Description:    Patch for CC13XX GenFSK
*
*  Copyright (c) 2015, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#ifndef _APPLY_PATCH_H
#define _APPLY_PATCH_H

#include <stdint.h>
#include <string.h>

#ifndef CPE_PATCH_TYPE
#define CPE_PATCH_TYPE static const uint32_t
#endif

#ifndef SYS_PATCH_TYPE
#define SYS_PATCH_TYPE static const uint32_t
#endif

#ifndef PATCH_FUN_SPEC
#define PATCH_FUN_SPEC static inline
#endif


CPE_PATCH_TYPE patchImage[] = {
   0x2100043d,
   0x210005ff,
   0x21000661,
   0x21000679,
   0x4710b5f8,
   0x460eb5f8,
   0x25012100,
   0x473004ad,
   0x4603b570,
   0x29014615,
   0x2900d006,
   0x4a11d006,
   0xf7ff4628,
   0xbd70ffed,
   0xe000480f,
   0x2405480f,
   0xd8034283,
   0x1e640840,
   0xdcf92c00,
   0x200140e3,
   0x18180340,
   0x29010b82,
   0x4906d007,
   0x31802300,
   0xf7ff4628,
   0xb2e0ffd7,
   0x4902bd70,
   0x316c4b04,
   0x0000e7f6,
   0x00005c83,
   0x2386bca0,
   0x230d8300,
   0x210006c0,
   0x4860b570,
   0x7bc46800,
   0xd01e2cff,
   0xd1032cfe,
   0x6840485d,
   0x0ec40340,
   0x2020495c,
   0x46054788,
   0x0f0006e0,
   0x300f0100,
   0x0209210d,
   0x4e584308,
   0x47b02101,
   0x211907e0,
   0x01c90f00,
   0x21011840,
   0x495247b0,
   0x311e4628,
   0xbd704788,
   0x4c51b5f8,
   0x78204d51,
   0x4e524f51,
   0xd0112800,
   0xd04d2801,
   0x47804850,
   0xd10a2800,
   0x21406878,
   0x60784308,
   0x28027820,
   0x2200d052,
   0x484b2102,
   0xbdf847a8,
   0x7980484a,
   0x483f70a0,
   0x49496800,
   0x28fe7b80,
   0x28ffd009,
   0x483cd003,
   0x6bc03040,
   0x6888e004,
   0x0ec000c0,
   0x6888e002,
   0x0ec002c0,
   0x20027060,
   0x21095620,
   0x428843c9,
   0x3020da01,
   0x200170a0,
   0x42885620,
   0x3020da01,
   0x20017060,
   0x56202102,
   0x42885661,
   0x2001dd04,
   0xf89cf000,
   0xe0052001,
   0xda194288,
   0xf0002000,
   0x20fff895,
   0x687870e0,
   0x43882140,
   0x68706078,
   0x70202001,
   0x47804829,
   0xd0bc2801,
   0x3020481f,
   0x07807ac0,
   0x2003d50a,
   0x68707020,
   0xe01e307f,
   0xff72f7ff,
   0x21012200,
   0xe7ab0288,
   0x78e178a0,
   0x18404a20,
   0x211fb240,
   0x43010209,
   0x326070a0,
   0x68758191,
   0x56612101,
   0xd1064288,
   0x70202002,
   0xff5af7ff,
   0x30d44628,
   0x4628e001,
   0x4a173034,
   0x47902100,
   0x8801bdf8,
   0x0f92050a,
   0xd1032a02,
   0x0d890589,
   0xd0012916,
   0x47084911,
   0x21044803,
   0x49108501,
   0x20016241,
   0x00004770,
   0x210000ec,
   0x500010c0,
   0x00003f8d,
   0x00004349,
   0x21000144,
   0x00000507,
   0x40090000,
   0x40043000,
   0x00006133,
   0x00000405,
   0x40086200,
   0x50001380,
   0x00005fd9,
   0x00002645,
   0x210004ed,
   0x4910b570,
   0x480e4c0d,
   0x7d6582c8,
   0x756020ff,
   0x4780480d,
   0xbd707565,
   0x480cb570,
   0x7ac54c07,
   0x20ff7e66,
   0x480a7660,
   0x76664780,
   0xd50307a8,
   0x49042007,
   0x82c80200,
   0x0000bd70,
   0x21000360,
   0x00000707,
   0x40086260,
   0x00004a55,
   0x21000260,
   0x00004cd1,
   0x4801b403,
   0xbd019001,
   0x0000718b,
   0x00000000,
   0x00030001,
   0x001f000a,
   0x00eb0059,
   0x04ea0239,
   0x129709f9,
   0x32a11feb,
   0x660a4a78,
   0x9e8c82fa,
   0xc917b663,
   0xdeedd664,
   0xe5e0e3c1,
};
#define _NWORD_PATCHIMAGE 181

#define _NWORD_PATCHSYS 0



#define _SYS_PATCH_FIXED_ADDR 0x20000000

PATCH_FUN_SPEC void enterCpePatch(void)
{
   uint32_t *pPatchVec = (uint32_t *) 0x2100041C;

#if (_NWORD_PATCHIMAGE > 0)
   memcpy(pPatchVec, patchImage, sizeof(patchImage));
#endif
}

PATCH_FUN_SPEC void enterSysPatch(void)
{
}

PATCH_FUN_SPEC void configurePatch(void)
{
   uint8_t *pParserPatchTab = (uint8_t *) 0x21000338;
   uint8_t *pPatchTab = (uint8_t *) 0x21000340;

   pPatchTab[80] = 0;
   pParserPatchTab[0] = 1;
   pPatchTab[53] = 2;
   pPatchTab[57] = 3;
}

PATCH_FUN_SPEC void rf_patch_cpe_genfsk(void)
{
   enterSysPatch();
   enterCpePatch();
   configurePatch();
}

PATCH_FUN_SPEC void refreshPatch(void)
{
   enterCpePatch();
   configurePatch();
}


#endif
