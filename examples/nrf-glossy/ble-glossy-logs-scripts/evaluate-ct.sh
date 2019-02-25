#!/bin/sh
#*******************************************************************************
# BSD 3-Clause License
#
# Copyright (c) 2017 Beshr Al Nahas and Olaf Landsiedel.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of the copyright holder nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#*******************************************************************************
#working directory
WDIR="/Users/beshr/work/chaos/examples/nrf-glossy"
cd ${WDIR}

echo $0
#constatnts
Ble_1Mbit=3 #/*!< 1 Mbit/s Bluetooth Low Energy */
Ble_2Mbit=4 #/*!< 2 Mbit/s Bluetooth Low Energy */
Ble_LR125Kbit=5 #/*!< Long range 125 kbit/s (TX Only - RX supports both) */
Ble_LR500Kbit=6 #/*!< Long range 500 kbit/s (TX Only - RX supports both) */

#length of each experiment in seconds
EXP_DURATION=120 
#constant tx power of the second transmitter and the initiator
TX_POWER_CONST=0

packet_size=38 #default PDU for standard ibeacon
n_channels=1 #number of RF channels to use
overrive_ch37=0 #use WiFi and Bluetooth free frequency for ch37
ntx=6
initiator=0
UART_BR_CONF=UART_BAUDRATE_BAUDRATE_Baud230400
TESTBED_CONF=WIRED_TESTBED

#read parameters to resume execution
#initialize saved variables
capture_saved=-100
ble_mode_saved=-100
tx_offset_saved=-100
tx_power_saved=-100
#read parameters
#[ -f "$0_resume" ] && source "$0_resume"
if [ -f "$0_resume" ]
then
  # while IFS= read -r line;do
  #   fields=($(printf "%s" "$line"|cut -d',' --output-delimiter=' ' -f1-))
  #   # ble_mode_saved=4
  #   # capture_saved=0
  #   # tx_offset_saved=4
  #   # tx_power_saved=8
  #   capture_saved=${fields[0]}
  #   ble_mode_saved=${fields[1]}
  #   tx_offset_saved=${fields[2]}
  #   tx_power_saved=${fields[3]}
  # done < "$0_resume"
  source "$0_resume"
  echo "resuming execution..."
  echo "capture_saved=${capture_saved}; ble_mode_saved=${ble_mode_saved}; tx_offset_saved=${tx_offset_saved}; tx_power_saved=${tx_power_saved};"
fi 

for capture in 1; ##evaluate CT with same (0) and different data (1)
do
  ### skip done experiments  ###
  if [ "$capture" -lt "$capture_saved" ]
  then
    continue  ### resumes iteration of an enclosing for loop ###
  elif [ "$capture" -gt "$capture_saved" ]
  then
    ble_mode_saved=-100
    tx_offset_saved=-100
    tx_power_saved=-100
  fi

  for ble_mode in 3 4 5 6; ##evaluate the different BLE modes
  do
    ### skip done experiments  ###
    if [ "$ble_mode" -lt "$ble_mode_saved" ]
    then
      continue  ### resumes iteration of an enclosing for loop ###
    elif [ "$ble_mode" -gt "$ble_mode_saved" ]
    then
      tx_offset_saved=-100
      tx_power_saved=-100
    fi

    ##make and flash the firmware for nodes 20 (initiator) and 21 (slave 1)
    make TARGET=nrf52840dk TESTBED_CONF=${TESTBED_CONF} initiator=${initiator} UART_BR_CONF=${UART_BR_CONF} ntx=${ntx} overrive_ch37=${overrive_ch37} n_channels=${n_channels} packet_size=${packet_size} ble_mode=${ble_mode} tx_power=${TX_POWER_CONST} tx_offset=0 capture=${capture} compile
      ##save firmware with timestamp and parameters
    DATE=`date '+%Y_%m_%d_%H_%M_%S'`
    EXPNAME=${DATE}_'ble_mode'_${ble_mode}_'txpower'_${TX_POWER_CONST}_'txoffset'_0_'capture'_${capture}_mote_20_21
    cp ${WDIR}/dirty-channel.hex ${WDIR}/hex/dirty-channel.${EXPNAME}.hex
    git add -f ${WDIR}/hex/dirty-channel.${EXPNAME}.hex
    git commit -m"dirty-channel.${EXPNAME}.hex"
    make p20 p21
    for tx_offset in 0 4 8 16 20 28;
    do
      ### skip done experiments  ###
      if [ "$tx_offset" -lt "$tx_offset_saved" ]
      then
        continue  ### resumes iteration of an enclosing for loop ###
      elif [ "$tx_offset" -gt "$tx_offset_saved" ]
      then
        tx_power_saved=-100
      fi
      
      for tx_power in -40 -20 -16 -8 0 2 4  #`seq -40 4 8`;
      do
        ### skip done experiments  ###
        if [ "$tx_power" -le "$tx_power_saved" ]
        then
          continue  ### resumes iteration of an enclosing for loop ###
        fi
        ##make and flash the firmware for node22 (slave 2) - it has a different txpower and txoffset
        make TARGET=nrf52840dk TESTBED_CONF=${TESTBED_CONF} initiator=${initiator} UART_BR_CONF=${UART_BR_CONF} ntx=${ntx} overrive_ch37=${overrive_ch37} n_channels=${n_channels} packet_size=${packet_size} ble_mode=${ble_mode} tx_power=${tx_power} tx_offset=${tx_offset} capture=${capture} compile
        make p22
        ##save firmware with timestamp and parameters
        DATE=`date '+%Y_%m_%d_%H_%M_%S'`
        EXPNAME=${DATE}_'ble_mode'_${ble_mode}_'txpower'_${TX_POWER_CONST}_${tx_power}_'txoffset'_${tx_offset}_'capture'_${capture}
        echo ${EXPNAME}
        cp ${WDIR}/dirty-channel.hex ${WDIR}/hex/dirty-channel.${EXPNAME}.hex
        git add -f ${WDIR}/hex/dirty-channel.${EXPNAME}.hex
	      git commit -m"dirty-channel.${EXPNAME}.hex"
        rm -f ${WDIR}/exp-logs/exp${EXPNAME}.log
        touch ${WDIR}/exp-logs/exp${EXPNAME}.log
        ##reset nodes
        make resetall -j
        ##log the intiator to file and kill the logging process after EXP_DURATION seconds
        #gtimeout -s ABRT --kill-after=7 --foreground 5 picocom -fh -b 230400 --imap lfcrlf /dev/tty.usbmodem142271 --logfile ./exp-logs/exptest.log
        gtimeout -s ABRT --kill-after=$((${EXP_DURATION}+2)) --foreground ${EXP_DURATION} picocom --noreset -fh -b 230400 --imap lfcrlf /dev/tty.usbmodem0006835708091 --logfile ${WDIR}/exp-logs/exp${EXPNAME}.log
        git add -f ${WDIR}/exp-logs/exp${EXPNAME}.log
        git commit -m"exp${EXPNAME}.log"
        #pause ${duration}
        #killall -c picocom

        #save state
        #echo "${capture},${ble_mode},${tx_offset},${tx_power}" >"$0_resume"
        echo "capture_saved=${capture}; ble_mode_saved=${ble_mode}; tx_offset_saved=${tx_offset}; tx_power_saved=${tx_power};" >"$0_resume"
      done
    done
  done
done