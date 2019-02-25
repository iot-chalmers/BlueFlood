/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
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
 */

package org.contikios.cooja.mspmote.interfaces;

import java.util.Collection;

import javax.swing.JPanel;

import org.apache.log4j.Logger;
import org.jdom.Element;

import org.contikios.cooja.ClassDescription;
import org.contikios.cooja.Mote;
import org.contikios.cooja.interfaces.Log;
import org.contikios.cooja.mote.memory.MemoryInterface;
import org.contikios.cooja.mote.memory.VarMemory;
import org.contikios.cooja.mspmote.MspMote;
import se.sics.mspsim.core.Memory;
import se.sics.mspsim.core.MemoryMonitor;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
/**
 * Observes writes to a special (hardcoded) Contiki variable: cooja_debug_ptr.
 * When the pointer is changed, the string that the pointer points to 
 * is outputted as log output from this mote interface.
 * 
 * Contiki code example:
 *  cooja_debug_ptr = "Almost non-intrusive debug output";
 * or simply:
 *  COOJA_DEBUG("Almost non-intrusive debug output");
 *  
 * @author Fredrik Osterlind
 */
@ClassDescription("Debugging output")
public class MspDebugOutput extends Log {
  private static Logger logger = Logger.getLogger(MspDebugOutput.class);

  private final static String CONTIKI_POINTER = "cooja_debug_ptr";
  
  private final static  String CONTIKI_REQ_SEED_POINTER = "cooja_request_randomseed";
  private final static  String CONTIKI_SEED_POINTER = "cooja_randomseed";
  
  private MspMote mote;
  private VarMemory mem;
  
  private String lastLog = null;
  private MemoryMonitor memoryMonitor = null;
  
  private MemoryMonitor requestSeedMemoryMonitor = null;
  
  public MspDebugOutput(Mote mote) {
    this.mote = (MspMote) mote;
    this.mem = new VarMemory(this.mote.getMemory());

    if (!mem.variableExists(CONTIKI_POINTER)) {
      /* Disabled */
      return;
    }
    this.mote.getCPU().addWatchPoint((int) mem.getVariableAddress(CONTIKI_POINTER),
        memoryMonitor = new MemoryMonitor.Adapter() {
        @Override
        public void notifyWriteAfter(int adr, int data, Memory.AccessMode mode) {
          String msg = extractString(MspDebugOutput.this.mote.getMemory(), data);
          if (msg != null && msg.length() > 0) {
            //setRandomSeed(MspDebugOutput.this.mote, (int) mote.getSimulation().getRandomGenerator().nextInt());
            lastLog = "DEBUG: " + msg;
            setChanged();
            notifyObservers(MspDebugOutput.this.mote);
          }
      }
    });
    
    setRandomSeedWatchPoint();

  }
  
  /** 
   * Monitor a special (hardcoded) Contiki flag: cooja_request_randomseed
   * When the variable is set to anything but zero, 
   * write a random number to the variable cooja_randomseed
   * and reset the request flag
   *  */
  private boolean setRandomSeedWatchPoint() {
    if (!(mem.variableExists(CONTIKI_SEED_POINTER) && mem.variableExists(CONTIKI_REQ_SEED_POINTER))) {
      /* Disabled */
      return false;
    }
    
    this.mote.getCPU().addWatchPoint((int) mem.getVariableAddress(CONTIKI_REQ_SEED_POINTER),
    		requestSeedMemoryMonitor = new MemoryMonitor.Adapter() {
        @Override
        public void notifyWriteAfter(int adr, int data, Memory.AccessMode mode) {
          if (data != 0) {
            /* Set the random number in the mote memory */ 
            long address = MspDebugOutput.this.mem.getVariableAddress(MspDebugOutput.CONTIKI_SEED_POINTER);
            int seed = (int) MspDebugOutput.this.mote.getSimulation().getRandomGenerator().nextInt();
            ByteBuffer b = ByteBuffer.allocate(4);
            b.order(ByteOrder.LITTLE_ENDIAN); /* Compulsory, the initial order of a byte buffer is always BIG_ENDIAN. */
            b.putInt(seed);
            MspDebugOutput.this.mote.getMemory().setMemorySegment(address, b.array());
            
            /* Turn off the request flag */ 
            address = MspDebugOutput.this.mem.getVariableAddress(MspDebugOutput.CONTIKI_REQ_SEED_POINTER);
            b.clear();
            b.putInt(0);
            MspDebugOutput.this.mote.getMemory().setMemorySegment(address, b.array());

            setChanged();
            notifyObservers(MspDebugOutput.this.mote);
          }
      }
    });
    return true;
  }
  
  public static boolean setRandomSeed(MspMote mote, int seed) {
    VarMemory mem = new VarMemory(mote.getMemory());
    if (!mem.variableExists(CONTIKI_SEED_POINTER)) {
      /* Disabled */
      return false;
    }
    
    long address = mem.getVariableAddress(CONTIKI_SEED_POINTER);
    
    ByteBuffer b = ByteBuffer.allocate(4);
    b.order(ByteOrder.LITTLE_ENDIAN); // compulsory, the initial order of a byte buffer is always BIG_ENDIAN.
    b.putInt(seed);
  		byte[] data = b.array();
  		
  		mote.getMemory().setMemorySegment(address, data);
    return true;
  }

  private String extractString(MemoryInterface mem, int address) {
    StringBuilder sb = new StringBuilder();
    while (true) {
      byte[] data = mem.getMemorySegment(address, 8);
      address += 8;
      for (byte b: data) {
        if (b == 0) {
          return sb.toString();
        }
        sb.append((char)b);
        if (sb.length() > 128) {
          /* Maximum size */
          return sb.toString() + "...";
        }
      }
    }
  }
  
  public Mote getMote() {
    return mote;
  }

  public String getLastLogMessage() {
    return lastLog;
  }

  public Collection<Element> getConfigXML() {
    return null;
  }

  public void setConfigXML(Collection<Element> configXML, boolean visAvailable) {
    /* Observed Contiki pointer is hardcoded */
  }

  public JPanel getInterfaceVisualizer() {
    return null;
  }

  public void releaseInterfaceVisualizer(JPanel panel) {
  }

  public void removed() {
    super.removed();

    if (memoryMonitor != null) {
      mote.getCPU().removeWatchPoint((int) mem.getVariableAddress(CONTIKI_POINTER), memoryMonitor);
    }
  }
}
