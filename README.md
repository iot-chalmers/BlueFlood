# BlueFlood

# Concurrent Transmissions for Multi-Hop Bluetooth 5

## Intro

This repo hosts the source code of the BlueFlood protocol that we published in the EWSN 2019 conference.

Beshr Al Nahas, Simon Duquennoy and Olaf Landsiedel. 2019. 
"Concurrent Transmissions for Multi-Hop Bluetooth 5". 
_In Proceedings of the International Conference on Embedded Wireless Systems and Networks (EWSN)_.
[paper](https://research.chalmers.se/en/publication/507874), [talk](./2019_02_27_ewsn_blueflood_github.pdf).

## Abstract

Bluetooth is an omnipresent communication technology, available on billions of connected devices today.
While it has been traditionally limited to peer-to-peer and star network topology, the recent Bluetooth 5 standard introduces new operating modes to allow for increased reliability and Bluetooth Mesh supports multi-hop networking based on message flooding.
In this paper, we present BlueFlood.
It adapts concurrent transmissions, as introduced by Glossy, to Bluetooth.
The result is fast and efficient network-wide data dissemination in multi-hop Bluetooth networks.
Moreover, we show that BlueFlood floods can be reliably received by off-the-shelf Bluetooth devices such as smart phones, opening new applications of concurrent transmissions and seamless integration with existing technologies. 

We present an in-depth experimental feasibility study of concurrent transmissions over Bluetooth PHY in a controlled environment.
Further, we build a small-scale testbed where we evaluate BlueFlood in real-world settings of a residential environment.
We show that BlueFlood achieves 99% end-to-end delivery ratio in multi-hop networks with a duty cycle of 0.13% for 1-second intervals.

## Implementation

We implement BlueFlood in C for the Contiki OS targeting Bluetooth nodes.
We support Nordic Semiconductor nRF52840 and the series nRF52x and nRF51x.

## Code structure

### Contiki port and platform drivers

Under [platform/Core51822](./platform/Core51822/), [platform/nrf52840dk](./platform/nrf52840dk/), [cpu/arm/nrf51822](./cpu/arm/nrf51822/) and [cpu/arm/nrf52x](./cpu/arm/nrf52x/).

We borrow some code from [Coen Roest's contiki-nRF51-port](https://github.com/coenroest/contiki-nRF51-port).

### BlueFlood code
Under [examples/nrf-glossy](./examples/nrf-glossy/).
Main file and protocol logic: [examples/nrf-glossy/dirty-channel.c](./examples/nrf-glossy/dirty-channel.c). 

### Experiment logs and processing scripts
Wired experiments for concurrent transmissions characterization on Bluetooth PHY: [examples/nrf-glossy/exp-logs](./examples/nrf-glossy/exp-logs).

Testbed experiments when running Blueflood: [examples/nrf-glossy/testbedjobscopy](./examples/nrf-glossy/testbedjobscopy).

Script used to process the logs and plot the results: [examples/nrf-glossy/ble-glossy-logs-scripts/process_plot_results.py](./examples/nrf-glossy/ble-glossy-logs-scripts/process_plot_results.py).

## Use the code on HW

You need to follow these steps:

### Preparations: Extract the CPU IDs and write in the testbed configuration file

You need to extract the *CPU IDs* which the code uses to identify different motes, then you need to make a list of the motes in the network, and give them numerical *node IDs* to refer to them in the rest of the configurations, and in the logs.
For example, you need to specify the initiator node ID later, to define the coordinator of the network that starts the flood.

To extract the CPU IDs, we compile and flash the HELLOWORLD mode, which prints the configuration of the motes.
Note that the main C file and the resulting executable is called dirty-channel.

1. Go to [examples/nrf-glossy](./examples/nrf-glossy/) and compile using:
```
make clean && make all TESTBED=HELLOWORLD_TESTBED
```

2. To program the firmware on the connected mote:
```
make dirty-channel.flash
```

3. Then, collect the CPU IDs from the logs. Look at the numbers between / ... /:
```
#R /round number/, ID: /cpu ID/, master: /cpu id of the initiator/, tx power: /txpower in dBm/, channel number /n/= /frequency/ MHz (std), msg: /B/ bytes, mode: /BLE mode/, CE: /capture enabled or not/, @ /date and timestamp of the compilation of the firmware/
```

4. Last, fill in the file [examples/nrf-glossy/testbed.h](./examples/nrf-glossy/testbed.h):
You need to define two lists one for *CPU IDs (TESTBED_IDS)* and one for the *node IDs (TESTBED_PI_IDS)*.
You can define these two lists under the configuration item `TESTBED==HOME_TESTBED`, for example.
Then, you need to use the configuration parameter `TESTBED=HOME_TESTBED` when you compile the firmware.

Alternatively, fill under `TESTBED==WIRED_TESTBED` if you want to reproduce the feasibility study results using a wired antenna setup (with coaxial cables), and compile with the configuration parameter `TESTBED=WIRED_TESTBED`.

### Configuring, compiling, flashing and running on motes

#### Toolchain: Compiler
To compile, you need to install the GCC compiler toolchain and tell the Makefile where to find it.

<!-- We use [gcc-arm-none-eabi-7-2017-q4-major](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads) -->
For the compiler, we use [gcc-arm-none-eabi-7-2017-q4-major](https://github.com/gnu-mcu-eclipse/arm-none-eabi-gcc/releases).
Download the binaries of this version of the compiler suitable to your system, then extract the archive on your system.
Consult [GNU MCU Eclipse page](https://gnu-mcu-eclipse.github.io/toolchain/arm/install/) for help.

You need to modify the following variable in the Makefile in accordance to your installation:
```
GCC_INSTALL_ROOT	:= ~/opt/gnu-mcu-eclipse/arm-none-eabi-gcc/7.2.1-1.1-20180401-0515
GCC_VERSION		:= 7.2.1
GCC_PREFIX		:= arm-none-eabi
```

#### Toolchain: Segger JLink

To upload the program on the board, you need to install [Segger JLink](https://www.segger.com/downloads/jlink).
If for some reasons it stll complains about JLink after installing it, redefine NRF_JLINK_PATH [here](./cpu/arm/nrf52x/Makefile.nrf52x#L79).

#### Make parameters
We have the following configuration parameters to specify in compile time, with the `make` command:
```
TESTBED=WIRED_TESTBED #name of testbed configuration
tx_power ?= 0 #in dB
capture ?= 0 #evaluate capture mode
ble_mode ?= 3 #Bluetooth mode. Supported modes: 4 for Ble_2Mbit, 3 for Ble_1Mbit, 6 for Ble_LR500Kbit, 5 for Ble_LR125Kbit.
tx_offset ?= 0 #inject a fixed delay in packet transmission time. We used this option to test resiliense to desynchronization in the feasibility study.
packet_size ?= 38 #packet size in bytes, # 38 is the default for standard-complient iBeacon packets
n_channels ?= 1 #how many channels to use. Up to 40.
overrive_ch37 ?= 0 #use a special frequency for channel 37 to avoid Bluetooth traffic
ntx ?= 4 #how many TX repetitions after receiving the packet
initiator?=0 #initiator node ID. Use an ID from the TESTBED_PI_IDS list.
```

#### Make command examples
Compile command example:
```
make clean && make TESTBED_CONF=HOME_TESTBED tx_power=-16 capture=0 ble_mode=4 initiator=3 n_channels=40 
```

To program the connected mote:
```
make dirty-channel.flash
```

If you need to change the firmware configuration parameters, you need to clean and recompile with the new parameters, then flash, as done in the previous commands.

