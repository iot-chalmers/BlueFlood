#!/bin/sh

# Initialize all the option variables.
# This ensures we are not contaminated by variables from the environment.

TESTBED_HOSTNAME=""
TESTBED_PORT=
TESTBED_USERNAME=""
#default settings
DURATION=5
tx_power=-20
capture=0
ble_mode=3 #/*!< 1 Mbit/s Bluetooth Low Energy */
tx_offset=0
packet_size=38 #default PDU for standard ibeacon
n_channels=1 #number of RF channels to use
overrive_ch37=0 #use WiFi and Bluetooth free frequency for ch37
ntx=4
initiator=1
UART_BR_CONF=UART_BAUDRATE_BAUDRATE_Baud230400
TESTBED_CONF=HOME_TESTBED

die() {
    printf '%s\n' "$1" >&2
    exit 1
}

show_help () {
  echo "Usage: $0 <sync>: to sync logs, or\n$0 <run>: to compile, schedule and run on the testbed"
}

checkip () {
  myip=`dig TXT +short -4 o-o.myaddr.l.google.com @ns1.google.com`
  home_ip="46.239."
  chalmers_ip="129.16."
  if [[ -z "${myip##\"$chalmers_ip*}" ]]; then
  TESTBED_HOSTNAME="beshr.hopto.org"
  TESTBED_PORT=21382
  echo "Chalmers IP: ${myip}"
  elif [[ -z "${myip##\"$home_ip*}" ]]; then
  TESTBED_HOSTNAME="boo-p"
  TESTBED_PORT=9999
  echo "Home IP: ${myip}"
  else
  echo "Foreign IP: ${myip} -- Cannot connect from here."
  exit 1
  fi
  TESTBED_USERNAME="boo"
}

sync () {
  SYNCPATH="/Users/beshr/work/chaos/examples/nrf-glossy/testbedjobscopy/"
  rsync --progress -avz --rsh="ssh -p${TESTBED_PORT}" ${TESTBED_USERNAME}@${TESTBED_HOSTNAME}:/home/${TESTBED_USERNAME}/jobs/* ${SYNCPATH};
}

run () {
  make TARGET=nrf52840dk TESTBED_CONF=${TESTBED_CONF} initiator=${initiator} UART_BR_CONF=${UART_BR_CONF} ntx=${ntx} overrive_ch37=${overrive_ch37} n_channels=${n_channels} packet_size=${packet_size} ble_mode=${ble_mode} tx_power=${tx_power} tx_offset=${tx_offset} capture=${capture} compile

  ##save firmware with timestamp and parameters
  DATE=`date '+%Y_%m_%d_%H_%M_%S'`
  EXPNAME=${DATE}_'ble_mode'_${ble_mode}_'txpower'_${tx_power}_'txoffset'_${tx_offset}_'capture'_${capture}_'packet_size'_${packet_size}_'nch'_${n_channels}_'och'_${overrive_ch37}_'ntx'_${ntx}
  #echo ${EXPNAME}
  FIRMWARE_NAME="dirty-channel_${EXPNAME}.nrf52.hex"
  echo ${FIRMWARE_NAME}
  if [ ! -f dirty-channel.hex ]; then
    die "Compile errors"
  fi
  cp dirty-channel.hex testbedhex/${FIRMWARE_NAME}
  # FIRMWARE_NAME=$1
  # EXPNAME=${DATE}_$2
  scp -P${TESTBED_PORT} testbedhex/${FIRMWARE_NAME} ${TESTBED_USERNAME}@${TESTBED_HOSTNAME}:/tmp/
  ssh -p${TESTBED_PORT} ${TESTBED_USERNAME}@${TESTBED_HOSTNAME} "python /usr/testbed/scripts/testbed.py create --name '${EXPNAME}' --platform 'nrf52' --duration ${DURATION} --copy-from /tmp/${FIRMWARE_NAME}"
  ssh -p${TESTBED_PORT} ${TESTBED_USERNAME}@${TESTBED_HOSTNAME} "python /usr/testbed/scripts/testbed.py start"
}

stop () {
  ssh -p${TESTBED_PORT} ${TESTBED_USERNAME}@${TESTBED_HOSTNAME} "python /usr/testbed/scripts/testbed.py stop"
}

command () {
  ssh -p${TESTBED_PORT} ${TESTBED_USERNAME}@${TESTBED_HOSTNAME} "python /usr/testbed/scripts/testbed.py $1"
}

evaluate() {
    capture=0
    tx_offset=0
    overrive_ch37=0

    for n_channels in 40; #3 8 16 32 40;
    do
        for ble_mode in 4 5 6; #3 4 5 6; ##evaluate the different BLE modes
        do
            for tx_power in `seq -16 4 -12`; #-20 -16 .. 4 
            do
                for packet_size in 38; #76 152 230;
                do
                    for ntx in 4 8 12 16; #76 152 230;
                    do
                        run
                    done
                done
            done
        done
    done
}

while :; do
    case $1 in
        -h|-\?|--help)
            show_help;    # Display a usage synopsis.
            exit
            ;;
        -c|--checkip)
            checkip;
            ;;
        -s|--sync)
            checkip;
            sync;
            ;;
        -r|--run)
            checkip;
            run;
            ;;
        -p|--stop)
            checkip;
            stop;
            ;;
        --cmd)
            checkip;
            command $2;
            ;;
        -e|--evaluate)
            checkip;
            evaluate;
            ;;
        -d|--duration)       # Takes an option argument; ensure it has been specified.
            if [ "$2" ]; then
                DURATION=$2
                shift
            else
                die 'ERROR: "--duration" requires a non-empty option argument.'
            fi
            ;;
        --ble)       # Takes an option argument; ensure it has been specified.
            if [ "$2" ]; then
                ble_mode=$2
                shift
            else
                die 'ERROR: "--ble" requires a non-empty option argument.'
            fi
            ;;
        --txpower)       # Takes an option argument; ensure it has been specified.
            if [ "$2" ]; then
                tx_power=$2
                shift
            else
                die 'ERROR: "--txpower" requires a non-empty option argument.'
            fi
            ;;
        --txoffset)       # Takes an option argument; ensure it has been specified.
            if [ "$2" ]; then
                tx_offset=$2
                shift
            else
                die 'ERROR: "--txoffset" requires a non-empty option argument.'
            fi
            ;;
        --packet)       # Takes an option argument; ensure it has been specified.
            if [ "$2" ]; then
                packet_size=$2
                shift
            else
                die 'ERROR: "--packet" requires a non-empty option argument.'
            fi
            ;;
        --capture)       # Takes an option argument; ensure it has been specified.
            if [ "$2" ]; then
                capture=$2
                shift
            else
                die 'ERROR: "--capture" requires a non-empty option argument.'
            fi
            ;;        
        --nch)       # Takes an option argument; ensure it has been specified.
            if [ "$2" ]; then
                n_channels=$2
                shift
            else
                die 'ERROR: "--nch" requires a non-empty option argument.'
            fi
            ;;
        --och)       # Takes an option argument; ensure it has been specified.
            if [ "$2" ]; then
                overrive_ch37=$2
                shift
            else
                die 'ERROR: "--och" requires a non-empty option argument.'
            fi
            ;;
        --ntx)       # Takes an option argument; ensure it has been specified.
            if [ "$2" ]; then
                ntx=$2
                shift
            else
                die 'ERROR: "--ntx" requires a non-empty option argument.'
            fi
            ;;
        --)              # End of all options.
            shift
            break
            ;;
        -?*)
            printf 'WARN: Unknown option (ignored): %s. Use %s -h to display help.\n' "$1" "$0" >&2
            ;;
        *)               # Default case: No more options, so break out of the loop.
            exit $E_OPTERROR;
            break
    esac

    shift
done
exit