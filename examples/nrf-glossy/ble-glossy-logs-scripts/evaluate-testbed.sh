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
WDIR="/Users/beshr/work/blueflood/examples/nrf-glossy"
pushd ${WDIR}

echo $0
#constatnts
Ble_1Mbit=3 #/*!< 1 Mbit/s Bluetooth Low Energy */
Ble_2Mbit=4 #/*!< 2 Mbit/s Bluetooth Low Energy */
Ble_LR125Kbit=5 #/*!< Long range 125 kbit/s (TX Only - RX supports both) */
Ble_LR500Kbit=6 #/*!< Long range 500 kbit/s (TX Only - RX supports both) */
IEEE802_15_4=15 #/*!< IEEE 802.15.4 250 kbit/s */

#length of each experiment in minutes
DURATION=5
initiator=0
CONTIKI_PROJECT=dirty-channel
TESTBED_CONF=$1
round_robin_initiator=0
n_channels=3
overrive_ch37=0
packet_size=38
ntx=2

if [ "$TESTBED_CONF" == "HOME_TESTBED" ]; then 
  make_target=testbedschedulehome;
  start_testbed_target=testbedstarthome;
elif [ "$TESTBED_CONF" == "CAU_TESTBED" ]; then 
  make_target=testbedschedule;
  start_testbed_target=testbedstart;
else
  echo "Testbed config is missing: $TESTBED_CONF";
  exit 1;
fi;

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
  source "$0_resume"
  echo "resuming execution..."
  echo "capture_saved=${capture_saved}; ble_mode_saved=${ble_mode_saved}; tx_offset_saved=${tx_offset_saved}; tx_power_saved=${tx_power_saved};"
fi 

for capture in 0; ##evaluate CT with same (0) and different data (1)
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

  for ble_mode in 3 4 5 6 15; ##evaluate the different BLE modes
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
    for tx_offset in 0;
    do
      ### skip done experiments  ###
      if [ "$tx_offset" -lt "$tx_offset_saved" ]
      then
        continue  ### resumes iteration of an enclosing for loop ###
      elif [ "$tx_offset" -gt "$tx_offset_saved" ]
      then
        tx_power_saved=-100
      fi
      
      for tx_power in -8 -4 0 #-20 -16 -8 -4 0 4 8 #`seq -40 4 8`;
      do
        ### skip done experiments  ###
        if [ "$tx_power" -le "$tx_power_saved" ]
        then
          continue  ### resumes iteration of an enclosing for loop ###
        fi
        make clean && make TESTBED_CONF=${TESTBED_CONF} initiator=${initiator} round_robin_initiator=${round_robin_initiator} n_channels=${n_channels} overrive_ch37=${overrive_ch37} cpu_busywait=0 ble_mode=${ble_mode} tx_power=${tx_power} tx_offset=${tx_offset} capture=${capture} ntx=${ntx} packet_size=${packet_size} all -j4
        ##save firmware with timestamp and parameters
        DATE=$(date +'%Y_%m_%d_%H_%M_%S')
        EXPERIMENT_PARAM_TMP=ble_mode_${ble_mode}_txpower_${tx_power}_txoffset_${tx_offset}_capture_${capture}_packet_size_${packet_size}_nch_${n_channels}_och_${overrive_ch37}_ntx_${ntx}_i_${initiator}_testbed_${TESTBED_CONF}
        EXPNAME=${CONTIKI_PROJECT}_${EXPERIMENT_PARAM_TMP}
        FIRMWARE_NAME="${DATE}_${EXPNAME}"
        #FIRMWARE_NAME="${EXPNAME}"
        echo ${FIRMWARE_NAME}
        cp dirty-channel.hex testbedhex/${FIRMWARE_NAME}.hex
        cp dirty-channel.hex ${FIRMWARE_NAME}.hex
        #exit
        # FIRMWARE_NAME=$1
        # EXPNAME=${DATE}_$2
        # scp -P9999 testbedhex/${FIRMWARE_NAME} boo@boo-p:/tmp/
        # ssh -p9999 boo@boo-p "python /usr/testbed/scripts/testbed.py create --name '${EXPNAME}' --platform 'nrf52' --duration ${DURATION} --copy-from /tmp/${FIRMWARE_NAME}"

        # git add -f testbedhex/${FIRMWARE_NAME}
	      # git commit -m"${EXPNAME} ${FIRMWARE_NAME}"
        echo "capture_saved=${capture}; ble_mode_saved=${ble_mode}; tx_offset_saved=${tx_offset}; tx_power_saved=${tx_power};" >"$0_resume"
        make ${make_target} FNAME=dirty-channel DURATION=${DURATION} NAME=${FIRMWARE_NAME}

        #exit
      done
    done
  done
done

make ${start_testbed_target}
popd
