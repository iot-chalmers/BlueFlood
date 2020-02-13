#!/bin/sh

# Initialize all the option variables.
# This ensures we are not contaminated by variables from the environment.


#default settings
REPEAT=1
DURATION=10
period=200
tx_power=-8
capture=0
ble_mode=3 #/*!< 1 Mbit/s Bluetooth Low Energy */
tx_offset=0
packet_size=38 #default PDU for standard ibeacon
n_channels=3 #number of RF channels to use
overrive_ch37=0 #use WiFi and Bluetooth free frequency for ch37
ntx=4
initiator=0
#UART_BR_CONF=UART_BAUDRATE_BAUDRATE_Baud115200
TESTBED_CONF=CAU_TESTBED
CONTIKI_PROJECT=dirty-channel
round_robin_initiator=0

die() {
    printf '%s\n' "$1" >&2
    exit 1
}

show_help () {
  echo "Usage: $0 <sync>: to sync logs, or\n$0 <run>: to compile, schedule and run on the testbed"
}

#takes $1: logs path, $2: log file name format; e.g., log_%d.txt and $3 $4: the range of the nodes id numbers;
extract_testbed_ids () {
    idlist=""; cpuidlist="";
    # echo $1, $2, $3, $4
    for i in $(seq $3 $4); do 
        file=$(printf "$2" $i)
        if [ -f "$1/$file" ]; then 
            cpuid=$(head $1/$file | grep -a "ID:" | cut -d':' -f6 |sort -u|cut -d',' -f1); 
            # printf "0x%x, " $cpuid; 
            if [ "$idlist" == "" ]; then 
                idlist="${i}"; cpuidlist="${cpuid}";
            else
                idlist="${idlist}, ${i}"; cpuidlist="${cpuidlist}, ${cpuid}";
            fi; 
        fi;
    done; 
    printf "#define TESTBED_IDS (uint32_t[]){%s}\n\n#define TESTBED_PI_IDS (uint8_t[]){%s}\n" "${cpuidlist}" "${idlist}";
    # echo ${idlist};
    # echo ${cpuidlist};
    # printf "\n";
    # for i in $(seq $1 $2); do 
    #     if [ -f  logs_21439/log_$i.txt ]; then printf "%d, " $i; fi; 
}

checkip () {
    if [ "$TESTBED_CONF" == "CAU_TESTBED" ]; then 
        TESTBED_HOSTNAME="sunlight.ds.informatik.uni-kiel.de"
        TESTBED_PORT=17122
        TESTBED_USERNAME="ban"
        SYNCPATH="/Users/beshr/work/chaos/examples/nrf-glossy/cau_testbed/"
        # make_target=testbedschedule;
        # start_testbed_target=testbedstart;
        # sync_target=testbedsync;

    elif [ "$TESTBED_CONF" == "HOME_TESTBED" ]; then 
        # make_target=testbedschedulehome;
        # start_testbed_target=testbedstarthome;
        # sync_target=testbedsynchome
        SYNCPATH="/Users/beshr/work/chaos/examples/nrf-glossy/home_testbed/"
        TESTBED_USERNAME="boo"
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
    else
        echo "Testbed config is missing: $TESTBED_CONF";
        exit 1;
    fi
}

sync () {
  rsync --progress -ravz -e "ssh -p${TESTBED_PORT}" ${TESTBED_USERNAME}@${TESTBED_HOSTNAME}:/home/${TESTBED_USERNAME}/jobs/* ${SYNCPATH};
}

run () {
    make clean && make TESTBED_CONF=${TESTBED_CONF} initiator=${initiator} round_robin_initiator=${round_robin_initiator} n_channels=${n_channels} overrive_ch37=${overrive_ch37} cpu_busywait=0 ble_mode=${ble_mode} tx_power=${tx_power} tx_offset=${tx_offset} capture=${capture} ntx=${ntx} packet_size=${packet_size} period=${period} all -j4
    ##save firmware with timestamp and parameters
    DATE=$(date +'%Y_%m_%d_%H_%M_%S')
    EXPERIMENT_PARAM_TMP=ble_mode_${ble_mode}_txpower_${tx_power}_txoffset_${tx_offset}_capture_${capture}_packet_size_${packet_size}_nch_${n_channels}_och_${overrive_ch37}_ntx_${ntx}_i_${initiator}_testbed_${TESTBED_CONF}
    EXPNAME=${CONTIKI_PROJECT}_${EXPERIMENT_PARAM_TMP}
    FIRMWARE_NAME="${DATE}_${EXPNAME}"
    #FIRMWARE_NAME="${EXPNAME}"
    echo ${FIRMWARE_NAME}
    if [ ! -f dirty-channel.hex ]; then
        die "Compile errors"
    fi

    cp dirty-channel.hex testbedhex/${FIRMWARE_NAME}.hex
    cp dirty-channel.hex ${FIRMWARE_NAME}.hex

    #make ${make_target} FNAME=dirty-channel DURATION=${DURATION} NAME=${FIRMWARE_NAME}

    scp -P${TESTBED_PORT} ${FIRMWARE_NAME}.hex ${TESTBED_USERNAME}@${TESTBED_HOSTNAME}:/home/${TESTBED_USERNAME}/newjob.nrf52.hex
    tries=${REPEAT};
	rpt=0; while [ "${rpt}" -lt "${tries}" ] ; do 
		echo "Repeatition ${rpt}:"; 
		rpt=$(( ${rpt} + 1 )); 
	    ssh -p${TESTBED_PORT} ${TESTBED_USERNAME}@${TESTBED_HOSTNAME} "python /usr/testbed/scripts/testbed.py create --name '${FIRMWARE_NAME}' --platform 'nrf52' --duration ${DURATION} --copy-from /home/ban/newjob.nrf52.hex --hosts /home/ban/all-hosts"; 
    done;
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

    for n_channels in 3;
    do
        for ble_mode in 3 4 5 6 15; ##evaluate the different BLE modes
        do
            for tx_power in -8 -4 0 4 8 #`seq -16 4 8`; #-20 -16 .. 4 
            do
                for packet_size in 19 76;
                do
                    if [ "$packet_size" == "76" ]; then 
                        period=400;
                    else
                        period=200;
                    fi; 
                    for ntx in 4;
                    do
                        run
                    done
                done
            done
        done
    done
    command start
}

while :; do
    case $1 in
        -h|-\?|--help)
            show_help;    # Display a usage synopsis.
            exit
            ;;
        -x|--extract_testbed_ids)
            #takes $2: logs path, $3: log file name format; e.g., log_%d.txt and $4 $5: the range of the nodes id numbers;
            extract_testbed_ids $2 $3 $4 $5;
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
            if [ "$2" ]; then
                REPEAT=$2
                shift
            fi
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
        --testbed)       # Takes an option argument; ensure it has been specified.
            if [ "$2" ]; then
                TESTBED_CONF=$2
                shift
            else
                die 'ERROR: "--testbed" shall be either CAU_TESTBED or HOME_TESTBED'
            fi
            ;;
            --initiator)       # Takes an option argument; ensure it has been specified.
            if [ "$2" ]; then
                initiator=$2
                shift
            else
                die 'ERROR: "--initiator" shall have a value'
            fi
            ;;      
            --roundrobin)       # Takes an option argument; ensure it has been specified.
            if [ "$2" ]; then
                round_robin_initiator=$2
                shift
            else
                die 'ERROR: "--roundrobin" shall be 0 or 1'
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