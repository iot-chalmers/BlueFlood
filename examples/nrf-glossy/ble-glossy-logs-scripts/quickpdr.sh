#!/bin/bash

###arguments:
###$1: dirname format; e.g., dir/%d*
###$2, $3: experiment range
###$4: log file name format, e.g., "logs/raspi%02d/log.txt"
###$5, $6: node range

#a node shall receive a minimum number of masseges to be considered connected
number_of_msgs_threshold=50;\

#kiel testbed stores start date and duration:
startdatefile=".started"; durationfile="duration";\

for i in `seq $2 $3`; do \
  tot_pdr=0; avg_pdr=0; tot_all=0; tot_misses=0; started=0; duration=0; all=0; misses=0;\
  dirnameformat=$(printf $1 $i); \
  # echo $dirnameformat; \
  pushd $dirnameformat; \
  connected=0; \
  disconnected=0; disconnected_nodes="";\
  for i in `seq $5 $6`; do \
    logfile=$(printf $4 $i);\
    # echo $logfile; \
    if [ -f "$logfile" ] ; then \
      all=`cat $logfile |grep -a "{tx-.*}" |wc -l`; \
      misses=`cat $logfile |grep -a "{tx-.*}" |grep -av "B" |wc -l`; \
      if [ -f "$startdatefile" ] ; then \
      started=`cat $startdatefile`; \
      duration=`cat $durationfile`; \
      else \
      started=0; duration=0; \
      fi; \
      if [ "$all" -lt $number_of_msgs_threshold ] ; then \
        pdr=0; disconnected=$((disconnected + 1)); \
        #printf "! Node %02d: Disconnected\n" $i; \
        disconnected_nodes=$(echo "$disconnected_nodes, $i"); \
      else \
        if [ "${misses}" -gt "${all}" ] ; then \
          pdr=0; disconnected=$((disconnected + 1)); \
          echo "wrong !"; \
          printf "! Node %02d: output is wrong: all %d < misses %d\n" $i $all $misses; \
        else \
          pdr=$(echo "100*(1-(${misses}/${all}))"|bc -l); \
          avg_pdr=$(echo "${avg_pdr}+${pdr}"|bc -l); \
          tot_all=$(echo "${tot_all}+${all}"|bc -l); \
          tot_misses=$(echo "${tot_misses}+${misses}"|bc -l); \
          # tot_pdr=$(echo "${tot_pdr}+${pdr}"|bc -l); \
          connected=$((connected + 1)); \
        fi; \
      fi; \
      if [ "$misses" -gt 0 ] ; then \
        printf "Node %02d: %d of %d PDR %.3f\n" $i ${misses} ${all} $pdr; \
      fi; \
    else \
      started=0; duration=0; all=0; misses=0; \
    fi; \
  done; \
  tot_pdr=$(echo "100*(1-(${tot_misses}/${tot_all}))"|bc -l); \
  avg_pdr=$(echo "((${avg_pdr}/${connected}))"|bc -l); \
  # echo ${tot_pdr}
  echo "Disconnected nodes list: $disconnected_nodes"; \
  printf "Avg PDR: %.3f PDR: %.3f: misses %d out of %d rounds for %d nodes: connected %d, disconnected %d\nExperiment started at %s\nDuration: %s\n" $avg_pdr ${tot_pdr} ${tot_misses} ${tot_all} $((connected + disconnected)) $connected ${disconnected} ${started} ${duration}; \
  popd; \
done;
