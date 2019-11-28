#!/bin/bash

for i in `seq $2 $3`; do \
  tot_pdr=0; \
  pushd $1/$i*; \
  connected=0; \
  disconnected=0; \
  for i in `seq 1 10` `seq 12 21`; do \
    logfile=$(printf "logs/raspi%02d/log.txt" $i);\
    if [ -f "$logfile" ] ; then \
      all=`cat $logfile |grep -a "{tx-.*}" |wc -l`; \
      rec=`cat $logfile |grep -a "{tx-.*}" |grep -av "{tx-.*B}" |wc -l`; \
    else \
      all=0; rec=0; \
    fi; \
    if [ "$all" -eq 0 ] ; then \
      pdr=0; disconnected=$((disconnected + 1)); \
      printf "! Node %02d: Disconnected\n" $i; \
    else \
      if [ "${rec}" -gt "${all}" ] ; then \
        pdr=0; disconnected=$((disconnected + 1)); \
        echo "wrong !"; \
        printf "! Node %02d: output is wrong: all %d < received %d\n" $i $all $rec; \
      else \
        pdr=$(echo "100*${rec}/${all}"|bc -l); \
        tot_pdr=$(echo "${tot_pdr}+${pdr}"|bc -l); \
        connected=$((connected + 1)); \
      fi; \
    fi; \
    printf "Node %02d: %d of %d PDR %.3f\n" $i ${rec} ${all} $pdr; \
  done; \
  tot_pdr=$(echo "${tot_pdr}/${connected}"|bc -l); \
  echo ${tot_pdr}
  printf "Total %d: connected %d, disconnected %d\n" $((connected + disconnected)) $connected ${disconnected}; \
  popd; \
done;
