/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Utility to store a node id in the flash
 * \author
 *         Coen Roest <coen@student.chalmers.se>
 */

#include "sys/node-id.h"
#include "contiki-conf.h"
#include "dev/xmem.h"

#include "nrf51.h"

unsigned short node_id = 0;
void flash_addr_init(void);
void flash_page_erase(uint32_t *page_address);
void flash_word_write(uint32_t *address, uint32_t value);

uint32_t *addr;
uint32_t pg_size;
uint32_t pg_num;

/*---------------------------------------------------------------------------*/
void flash_addr_init(void)
{
  pg_size = NRF_FICR->CODEPAGESIZE;
  pg_num  = NRF_FICR->CODESIZE - 1;    // Use last page in flash
  // Start address:
  addr = (uint32_t *)(pg_size * pg_num);
}
/*---------------------------------------------------------------------------*/
void flash_page_erase(uint32_t *page_address)
{
  // Turn on flash erase enable and wait until the NVMC is ready:
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

  // Erase page:
  NRF_NVMC->ERASEPAGE = (uint32_t) page_address;
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

  // Turn off flash erase enable and wait until the NVMC is ready:
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
}
/*---------------------------------------------------------------------------*/
void flash_word_write(uint32_t *address, uint32_t value)
{
    // Turn on flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

    *address = value;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

    // Turn off flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

}



/*---------------------------------------------------------------------------*/
void
node_id_restore(void)
{
  flash_addr_init();
  node_id = (uint8_t)*addr;
  /*
  unsigned char buf[4];
  xmem_pread(buf, 4, NODE_ID_XMEM_OFFSET);
  if(buf[0] == 0xad &&
     buf[1] == 0xde) {
    node_id = (buf[2] << 8) | buf[3];
  } else {
    node_id = 0;
  }
  */
}
/*---------------------------------------------------------------------------*/
void
node_id_burn(unsigned short id)
{

  flash_addr_init();
  flash_page_erase(addr);
  flash_word_write(addr, (uint32_t) id);


  /*
  unsigned char buf[4];
  buf[0] = 0xad;
  buf[1] = 0xde;
  buf[2] = id >> 8;
  buf[3] = id & 0xff;
  xmem_erase(XMEM_ERASE_UNIT_SIZE, NODE_ID_XMEM_OFFSET);
  xmem_pwrite(buf, 4, NODE_ID_XMEM_OFFSET);
  */
}
/*---------------------------------------------------------------------------*/
