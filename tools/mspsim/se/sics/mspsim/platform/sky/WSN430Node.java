/**
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 * This file is part of MSPSim.
 *
 * $Id$
 *
 * -----------------------------------------------------------------
 *
 * WSN430Node
 *
 * Author  : Joakim Eriksson
 * Created : Sun Oct 21 22:00:00 2007
 * Updated : $Date$
 *           $Revision$
 */

package se.sics.mspsim.platform.sky;
import java.io.IOException;
import se.sics.mspsim.chip.FileStorage;
import se.sics.mspsim.chip.M25P80;
import se.sics.mspsim.core.IOPort;
import se.sics.mspsim.core.USARTSource;
import se.sics.mspsim.util.ArgumentManager;
import se.sics.mspsim.chip.Leds;
import se.sics.mspsim.chip.CC2420;
import se.sics.mspsim.chip.DS2411;
import se.sics.mspsim.core.PortListener;
import se.sics.mspsim.core.USART;
import se.sics.mspsim.core.USARTListener;
/**
 * Emulation of WSN430 Mote
 */
public class WSN430Node extends CC2420Node {

  private M25P80 flash;
  protected IOPort port3;
  
  public static final int MODE_LEDS_OFF = 0;
  public static final int MODE_LEDS_1 = 1;
  public static final int MODE_LEDS_2 = 2;
  public static final int MODE_LEDS_3 = 3;
  public static final int MODE_MAX = MODE_LEDS_3;
  // Port 2.
//  public static final int BUTTON_PIN = 7;

//  public static final int SHT11_CLK_PIN = 6;
//  public static final int SHT11_DATA_PIN = 5;
//
//  public static final int SHT11_CLK = 1 << SHT11_CLK_PIN;
//  public static final int SHT11_DATA = 1 << SHT11_DATA_PIN;

  private static final int[] LEDS = { 0xff2020, 0x40ff40, 0x4040ff };
  public static final int BLUE_LED = 0x40;
  public static final int GREEN_LED = 0x20;
  public static final int RED_LED = 0x10;

  /* P1.4: FIFOP from CC2420 */
  /* P1.3 - Input: FIFO from CC2420 */
  /* P1.6 - Input: CCA from CC2420 */
  /* P1.5 - Input: SFD from CC2420 */
  public static final int CC2420_FIFOP = 4; 
  public static final int CC2420_FIFO = 3;
  public static final int CC2420_CCA = 6;
  public static final int CC2420_SFD = 5;

  /* P3.0 - Output: VREG_EN to CC2420 */
  public static final int CC2420_VREG = 0;

  /* P4.2 - Output: SPI Chip Select (CS_N) */
  public static final int CC2420_CHIP_SELECT = 2;
  
  public boolean redLed;
  public boolean blueLed;
  public boolean greenLed;

  private Leds leds;
//  private Button button;
//  public SHT11 sht11;

  public WSN430Gui gui;

  public Leds getLeds() {
      return leds;
  }

//  public Button getButton() {
//      return button;
//  } 

//  @Deprecated
//  public void setButton(boolean buttonPressed) {
//      button.setPressed(buttonPressed);
//  }

  public void setupGUI() {
    if (gui == null) {
      gui = new WSN430Gui(this);
      registry.registerComponent("nodegui", gui);
    }
  }
  
  public int getModeMax() {
    return MODE_MAX;
  }

  /**
   * Creates a new <code>WSN430Node</code> instance.
   *
   */
  public WSN430Node() {
    super("IoTLab SenseNode WSN430");
  }

  public WSN430Node(String id) {
    super(id);
    setMode(MODE_LEDS_OFF);
  }
  
  public M25P80 getFlash() {
    return flash;
  }

  public void setFlash(M25P80 flash) {
    this.flash = flash;
    registry.registerComponent("xmem", flash);
  }

  // USART Listener
  public void dataReceived(USARTSource source, int data) {
    radio.dataReceived(source, data);
    flash.dataReceived(source, data);
    /* if nothing selected, just write back a random byte to these devs */
    if (!radio.getChipSelect() && !flash.getChipSelect()) {
      source.byteReceived(0);
    }
  }

  @Override
  public void portWrite(IOPort source, int data) {
    if (source == port4) {
        // Chip select = active low...
        radio.setChipSelect((data & CC2420_CHIP_SELECT) == 0);
        //radio.portWrite(source, data);
        flashWrite(source, data);
    } else if (source == port3) {
        radio.setVRegOn((data & CC2420_VREG) != 0);
        //radio.portWrite(source, data);
        flashWrite(source, data);
    } else if (source == port2) {
        ds2411.dataPin((data & DS2411_DATA) != 0);
    } else if (source == port5) {
      redLed = (data & RED_LED) == 0;
      blueLed = (data & BLUE_LED) == 0;
      greenLed = (data & GREEN_LED) == 0;
      leds.setLeds((redLed ? 1 : 0) + (greenLed ? 2 : 0) + (blueLed ? 4 : 0));
      int newMode = (redLed ? 1 : 0) + (greenLed ? 1 : 0) + (blueLed ? 1 : 0);
      setMode(newMode);
    } else if (source == port1) {
//      sht11.clockPin((data & SHT11_CLK) != 0);
//      sht11.dataPin((data & SHT11_DATA) != 0);
    }
  }
  
  public void setupNodePorts() {
    ds2411 = new DS2411(cpu);

    port1 = cpu.getIOUnit(IOPort.class, "P1");
    port1.addPortListener(this);

    port2 = cpu.getIOUnit(IOPort.class, "P2");
    ds2411.setDataPort(port2, DS2411_DATA_PIN);
    port2.addPortListener(this);

    port3 = cpu.getIOUnit(IOPort.class, "P3");
    port3.addPortListener(this);
    
    port4 = cpu.getIOUnit(IOPort.class, "P4");
    port4.addPortListener(this);

    port5 = cpu.getIOUnit(IOPort.class, "P5");
    port5.addPortListener(this);

    USART usart0 = cpu.getIOUnit(USART.class, "USART0");
    radio = new CC2420(cpu);
    radio.setCCAPort(port1, CC2420_CCA);
    radio.setFIFOPPort(port1, CC2420_FIFOP);
    radio.setFIFOPort(port1, CC2420_FIFO);
    radio.setSFDPort(port1, CC2420_SFD);

    usart0.addUSARTListener(this);

    USART usart = cpu.getIOUnit(USART.class, "USART1");
    if (usart != null) {
        registry.registerComponent("serialio", usart);
    }
    
    leds = new Leds(cpu, LEDS);
//    button = new Button("Button", cpu, port2, BUTTON_PIN, true);
//    sht11 = new SHT11(cpu);
//    sht11.setDataPort(port1, SHT11_DATA_PIN);
    
}

  @Override
  protected void flashWrite(IOPort source, int data) {
    flash.portWrite(source, data);
  }
  
  public static void main(String[] args) throws IOException {
    WSN430Node node = new WSN430Node();
    ArgumentManager config = new ArgumentManager();
    config.handleArguments(args);
    node.setupArgs(config);
  }

}
