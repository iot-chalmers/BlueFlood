SHELL:=/bin/sh

GCC_INSTALL_ROOT	:= ~/opt/gnu-mcu-eclipse/arm-none-eabi-gcc/7.2.1-1.1-20180401-0515
GCC_VERSION			:= 7.2.1
GCC_PREFIX			:= arm-none-eabi
USER:=ban

#UART_BAUDRATE_BAUDRATE_Baud115200=0x01D7E000 #/*!< 115200 baud (actual rate: 115942) */
#UART_BAUDRATE_BAUDRATE_Baud230400=0x03AFB000 #/*!< 230400 baud (actual rate: 231884) */
#UART_BAUDRATE_BAUDRATE_Baud250000=0x04000000 #/*!< 250000 baud */
#UART_BAUDRATE_BAUDRATE_Baud460800=0x075F7000 #/*!< 460800 baud (actual rate: 470588) */
#UART_BAUDRATE_BAUDRATE_Baud921600=0x0EBED000 #/*!< 921600 baud (actual rate: 941176) */
#UART_BAUDRATE_BAUDRATE_Baud1M=0x10000000 #/*!< 1Mega baud */

UART_BR_CONF?=UART_BAUDRATE_BAUDRATE_Baud115200 #UART bitrate
uartbr=115200

Nrf_1Mbit=0 #/*!< 1 Mbit/s Nordic proprietary radio mode */
Nrf_2Mbit=1 #/*!< 2 Mbit/s Nordic proprietary radio mode */
Ble_1Mbit=3 #/*!< 1 Mbit/s Bluetooth Low Energy */
Ble_2Mbit=4 #/*!< 2 Mbit/s Bluetooth Low Energy */
Ble_LR125Kbit=5 #/*!< Long range 125 kbit/s (TX Only - RX supports both) */
Ble_LR500Kbit=6 #/*!< Long range 500 kbit/s (TX Only - RX supports both) */
Ieee802154_250Kbit=15 #/*!< IEEE 802.15.4-2006 250 kbit/s */

tx_power ?= 0 #in dB
capture ?= 0 #evaluate capture mode
ble_mode ?= $(Ble_1Mbit) #Bluetooth mode
tx_offset ?= 0 #inject TX delay
packet_size ?= 38 #packet size in bytes
#default PDU for standard ibeacon
n_channels ?= 1 #how many channels to use. Up to 40.
overrive_ch37 ?= 0 #use a special frequency for channel 37 to avoid Bluetooth traffic
ntx ?= 4 #how many repititions
initiator?=0 #initiator node ID
test_mode_tx_carrier?=0 #do not tx/rx data, but transmit a constant unmodulated carrier (by keeping the radio in idle mode in slots instead of starting tx/rx)
#unnecessary
DURATION ?= 120 #duration of the experiment
TESTBED_CONF?=WIRED_TESTBED #Default testbed 
round_robin_initiator ?= 0 #switch initiator every round
cpu_busywait ?= 1

sleep_node_forever?=0

CONTIKI = ../..
ifndef TARGET
#TARGET=Core51822
TARGET=nrf52840dk
endif

CONTIKI_PROJECT = dirty-channel
PROJECT_SOURCEFILES += nrf-radio-driver.c hamming-encoder-decoder.c crc24.c

CONTIKI_WITH_CHAOS = 0
CONTIKI_WITH_IPV4 = 0
CONTIKI_WITH_IPV6 = 0
CONTIKI_WITH_RIME = 1
CFLAGS += -DTEST_SLEEP_NODE_FOREVER=$(sleep_node_forever) -DBLUEFLOOD_BUSYWAIT=$(cpu_busywait) -DROUND_ROBIN_INITIATOR=$(round_robin_initiator) -DRADIO_TEST_TX_CARRIER=$(test_mode_tx_carrier) -DINITATOR_NODE_INDEX=$(initiator) -DUART_BAUDRATE=$(UART_BR_CONF) -DOVERRIDE_BLE_CHANNEL_37$(overrive_ch37) -DNTX=$(ntx) -DNUMBER_OF_CHANNELS=$(n_channels) -DPACKET_TEST_SIZE_CONF=$(packet_size) -DBLE_DEFAULT_RF_POWER=$(tx_power) -DTEST_CE=$(capture) -DRADIO_MODE_CONF=$(ble_mode) -DARTIFICIAL_TX_OFFSET=$(tx_offset)
ifdef TESTBED_CONF
CFLAGS += -DTESTBED=$(TESTBED_CONF)
endif

#macros to remove spaces
empty:=
space:=$(empty) $(empty)
##usage example:
##$(subst $(space),$(comma),$(foo))

#save firmware with timestamp and parameters
##note: use := with the shell keyword to evaluate the expression on the first assignment
DATE:=$(shell date +'%Y_%m_%d_%H_%M_%S')
EXPERIMENT_PARAM_TMP:=_ble_mode_${ble_mode}_txpower_${tx_power}_txoffset_${tx_offset}_capture_${capture}_packet_size_${packet_size}_nch_${n_channels}_och_${overrive_ch37}_ntx_${ntx}_i_${initiator}_testbed_${TESTBED_CONF}
EXPERIMENT_PARAM:=$(subst $(space),,$(EXPERIMENT_PARAM_TMP))
EXPNAME:=$(CONTIKI_PROJECT)_$(EXPERIMENT_PARAM)
FIRMWARE_NAME:=$(DATE)_$(EXPNAME)
FIRMWARE_FULL_PATH:=testbedhex/$(FIRMWARE_NAME).elf

##JLINK serial numbers of motes to be able to flash one specific
##you only need it if you have several motes connected to the same computer
ifeq ($(MOTE),4)
NRF_JLINK_SN=681964645
endif
ifeq ($(MOTE),5)
NRF_JLINK_SN=681604107
endif
ifeq ($(MOTE),6)
NRF_JLINK_SN=681765623
endif
ifeq ($(MOTE),7)
NRF_JLINK_SN=681471568
endif
ifeq ($(MOTE),10)
NRF_JLINK_SN=683897098
endif
ifeq ($(MOTE),11)
NRF_JLINK_SN=683062141
endif
ifeq ($(MOTE),12)
NRF_JLINK_SN=683844332
endif
ifeq ($(MOTE),13)
NRF_JLINK_SN=683582322
endif
ifeq ($(MOTE),20)
NRF_JLINK_SN=683570809
endif
ifeq ($(MOTE),21)
NRF_JLINK_SN=683808382
endif
ifeq ($(MOTE),22)
NRF_JLINK_SN=683014029
endif
ifeq ($(MOTE),23)
NRF_JLINK_SN=683912157
endif
ifeq ($(MOTE),24)
NRF_JLINK_SN=683378022
endif
ifeq ($(MOTE),99)
#NRF_JLINK_SN=683085088
uart_device=tty.usbmodem1431102
endif
ifeq ($(MOTE),100)
NRF_JLINK_SN=683179444
endif

ifndef uart_device
uart_device=tty.usbmodem000$(NRF_JLINK_SN)1
endif

SMALL=1

all: $(CONTIKI_PROJECT)
	cp $(CONTIKI_PROJECT).hex $(FIRMWARE_NAME).hex

start_jobs:
	ssh -p17122 testbed@sunlight.ds.informatik.uni-kiel.de "python /usr/testbed/scripts/testbed.py start"

#make testbedschedule NAME=hello DURATION=2 FNAME=dirty-channel
testbedsync:
	rsync -ravz -e "ssh -p17122" ${USER}@sunlight.ds.informatik.uni-kiel.de:/home/${USER}/jobs/ ./cau_testbed/

testbedschedule2: 
	scp -P17122 $(FNAME).hex testbed@sunlight.ds.informatik.uni-kiel.de:/home/testbed/newjob.nrf52.hex
	ssh -p17122 testbed@sunlight.ds.informatik.uni-kiel.de "python /usr/testbed/scripts/testbed.py create --name '${NAME}' --platform 'nrf52' --duration ${DURATION} --copy-from /home/testbed/newjob.nrf52.hex"

testbedstart: 
	ssh -p17122 ban@sunlight.ds.informatik.uni-kiel.de "python /usr/testbed/scripts/testbed.py status"
	ssh -p17122 ban@sunlight.ds.informatik.uni-kiel.de "python /usr/testbed/scripts/testbed.py start"

testbedschedule: 
	scp -P17122 $(FNAME).hex ban@sunlight.ds.informatik.uni-kiel.de:/home/ban/newjob.nrf52.hex
	ssh -p17122 ban@sunlight.ds.informatik.uni-kiel.de "python /usr/testbed/scripts/testbed.py create --name '${NAME}' --platform 'nrf52' --duration ${DURATION} --copy-from /home/ban/newjob.nrf52.hex"

testbedjob: $(CONTIKI_PROJECT)
	cp $(CONTIKI_PROJECT).hex $(FIRMWARE_NAME).hex
	scp -P17122 ${FIRMWARE_NAME}.hex ban@sunlight.ds.informatik.uni-kiel.de:/home/ban/newjob.nrf52.hex
	ssh -p17122 ban@sunlight.ds.informatik.uni-kiel.de "python /usr/testbed/scripts/testbed.py create --name '${EXPNAME}' --platform 'nrf52' --duration ${DURATION} --copy-from /home/ban/newjob.nrf52.hex"

date:
	echo "$(DATE)" $(COUNT_ACTIVE_NODES)

sync-logs:
	rsync --progress -avz ./ble-glossy-logs-scripts/ /Volumes/beshr/beshr/ble-glossy-logs-scripts/

programext:
	#make dirty-channel.flash MOTE=100
	ls -db /Volumes/DAPLINK*/ | gxargs -n 1 -d '\n' cp dirty-channel.hex 

logcommit:
	rm -f exp-logs/exp$(EXP).log
	touch exp-logs/exp$(EXP).log
	picocom -fh -b $(uartbr) --imap lfcrlf /dev/${uart_device} | tee exp-logs/exp$(EXP).log
	git add -f exp-logs/exp$(EXP).log
	git commit -m"exp$(EXP).log"

log:
	picocom -fh -b $(uartbr) --imap lfcrlf /dev/${uart_device} | tee exp-logs/exp$(EXP).log

program:
	make FIRMWARE=$(CONTIKI_PROJECT).hex upload 

flashall:
	for M in 10 11 12 13 ; do \
		echo $$M ; \
		make MOTE=$$M FIRMWARE=$(CONTIKI_PROJECT).hex upload ; \
	done

eraseall:
	for M in 10 11 12 13 ; do \
		echo $$M ; \
		make MOTE=$$M erase ; \
	done

doall: clean all flashall resetall

compile: clean all

resetall: 
	for M in 10 11 12 13 ; do \
		echo $$M ; \
		make MOTE=$$M reset ; \
	done

logtimeoutcommit:
	rm -f exp-logs/exp$(EXP).log
	touch exp-logs/exp$(EXP).log
	gtimeout -s ABRT --kill-after=$$((${DURATION}+2)) --foreground $(DURATION) picocom --noreset -fh -b $(uartbr) --imap lfcrlf /dev/${uart_device} --logfile ./exp-logs/exp$(EXP).log	
	git add -f exp-logs/exp$(EXP).log
	git commit -m"exp$(EXP).log"

include $(CONTIKI)/Makefile.include


##Targets to test on IoTLab. Needs another script
# user?=alnahas
# site?=saclay
# COUNT_ALIVE_NODES:=$(shell experiment-cli info --site saclay -l --archi "nrf52840dk:multi" --state "Alive" | grep "network_address" | sort -u | wc -l)
# COUNT_BUSY_NODES:=$(shell experiment-cli info --site saclay -l --archi "nrf52840dk:multi" --state "Busy" | grep "network_address" | sort -u | wc -l)
# COUNT_ACTIVE_NODES:=$$(( $(COUNT_ALIVE_NODES) + $(COUNT_BUSY_NODES) ))

# dosch: iotlab
# 	ID:=$(shell experiment-cli submit --name $(EXPNAME) --duration $(duration) --site-association $(site),script=/Users/beshr/work/chaos-support/eval/iotlab-clitools/aggregator_script.sh,scriptconfig=/Users/beshr/work/chaos-support/eval/iotlab-clitools/aggregator_script_config --list $(COUNT_ACTIVE_NODES),archi=nrf52840dk:multi+site=saclay,$(FIRMWARE_FULL_PATH)); IDD:=$(shell echo "$(ID)"| grep "id"| cut -d':'); echo "$(DATE), $(IDD), $(COUNT_ACTIVE_NODES), $(duration), $(EXPNAME)" >>experiment_reg; cat experiment_reg
	
# %.schedule:
# 	python /Users/beshr/work/chaos-support/eval/iotlab-clitools/iotlab-helper.py schedule --archi="nrf52840dk:multi" --site=$(site) --firmware=$*.elf --duration=$(duration) --run --user=$(user)
	
# schedule-iotlab: iotlab
# 	python /Users/beshr/work/chaos-support/eval/iotlab-clitools/iotlab-helper.py schedule --archi="nrf52840dk:multi" --site=$(site) --firmware=${FIRMWARE_NAME}.elf --duration=$(duration) --run --user=$(user)

# update-iotlab-nodes:
# 	IOTLAB_MAPPING=$$(python /Users/beshr/work/chaos-support/eval/iotlab-clitools/iotlab-helper.py list make --site=$(site) --archi="nrf52840dk:multi"); echo "#define IOTLAB_MAPPING $${IOTLAB_MAPPING}" > iotlabnodes.h
# 	cat iotlabnodes.h 
	
# iotlab-fresh: update-iotlab-nodes
# 	make TARGET=wsn430 clean
# 	make TARGET=wsn430 iotlab 

# iotlab: CFLAGS+= -DTESTBED=IOTLAB_SACLAY -DCOOJA=0
# iotlab: compile
# 	cp $(CONTIKI_PROJECT).elf $(FIRMWARE_FULL_PATH)