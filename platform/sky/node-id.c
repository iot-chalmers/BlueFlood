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
 *         Utility to store a node id in the external flash
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "sys/node-id.h"
#include "contiki-conf.h"
#include "dev/xmem.h"
#include "dev/ds2411/ds2411.h"
#include "net/mac/chaos/node/testbed.h"

#if((TESTBED==motes)||(TESTBED==ewsn)||(TESTBED==cooja))
#include "string.h"
#include "apps/chaos/max/ewsn-nodes.h"
#include "net/mac/chaos/node/node.h"
uint16_t ewsn_ids[] = EWSN_IDS;
uint8_t ewsn_macs[][4] = EWSN_MACS;
#endif

unsigned short node_id = 0;


/*---------------------------------------------------------------------------*/
void
node_id_restore(void)
{
#if((TESTBED==motes)||(TESTBED==ewsn)||(TESTBED==cooja))
  #warning "artificial node id based on ds2411 list"
  uint8_t i, match = 0;
  for(i=0; i < sizeof(ewsn_ids)/sizeof(uint16_t); i++){
    if( ds2411_id[4] == ewsn_macs[i][0] && ds2411_id[5] == ewsn_macs[i][1] && ds2411_id[6] == ewsn_macs[i][2] && ds2411_id[7] == ewsn_macs[i][3] ){
        node_id = ewsn_ids[i];
        //chaos_node_index = i;
        //chaos_has_node_index = 1;
        match=1;
      }
  }

  if(!match){
    node_id = (ds2411_id[6] << 8) + ds2411_id[7];
#if !COOJA
    if( node_id < 255 ){
      node_id += 0xf000;
    }
#endif
  }

#else
  unsigned char buf[2];
  xmem_pread(buf, 2, NODE_ID_XMEM_OFFSET+2);
  node_id = (buf[0] << 8) | buf[1];
  /*unsigned char buf[4];
  xmem_pread(buf, 4, NODE_ID_XMEM_OFFSET);
  if(buf[0] == 0xad &&
     buf[1] == 0xde) {
    node_id = (buf[2] << 8) | buf[3];
  } else {
    node_id = 0;
  }*/
#endif /* ((TESTBED!=ewsn)&&(TESTBED!=cooja)) */
}
/*---------------------------------------------------------------------------*/
void
node_id_burn(unsigned short id)
{
#if((TESTBED!=ewsn)&&(TESTBED!=cooja))
  unsigned char buf[4];
  buf[0] = 0xad;
  buf[1] = 0xde;
  buf[2] = id >> 8;
  buf[3] = id & 0xff;
  xmem_erase(XMEM_ERASE_UNIT_SIZE, NODE_ID_XMEM_OFFSET);
  xmem_pwrite(buf, 4, NODE_ID_XMEM_OFFSET);
#endif
}
/*---------------------------------------------------------------------------*/
