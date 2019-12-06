#!/bin/bash

startdatefile=".started"; durationfile="duration";\

for i in `seq $2 $3`; do \
  tot_pdr=0; avg_pdr=0; tot_all=0; tot_misses=0; started=0; duration=0; all=0; misses=0;\
  pushd $1/$i*; \
  connected=0; \
  disconnected=0; \
  for i in `seq 1 10` `seq 12 21`; do \
    logfile=$(printf "logs/raspi%02d/log.txt" $i);\
    if [ -f "$logfile" ] ; then \
      all=`cat $logfile |grep -a "{tx-.*}" |wc -l`; \
      misses=`cat $logfile |grep -a "{tx-.*}" |grep -av "B" |wc -l`; \
      started=`cat $startdatefile`; \
      duration=`cat $durationfile`; \
    else \
      started=0; duration=0; all=0; misses=0; \
    fi; \
    if [ "$all" -eq 0 ] ; then \
      pdr=0; disconnected=$((disconnected + 1)); \
      printf "! Node %02d: Disconnected\n" $i; \
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
  done; \
  tot_pdr=$(echo "100*(1-(${tot_misses}/${tot_all}))"|bc -l); \
  avg_pdr=$(echo "((${avg_pdr}/${connected}))"|bc -l); \
  # echo ${tot_pdr}
  printf "Avg PDR: %.3f PDR: %.3f: misses %d out of %d rounds for %d nodes: connected %d, disconnected %d\nExperiment started at %s\nDuration: %s\n" $avg_pdr ${tot_pdr} ${tot_misses} ${tot_all} $((connected + disconnected)) $connected ${disconnected} ${started} ${duration}; \
  popd; \
done;
