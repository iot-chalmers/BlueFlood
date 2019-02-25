#!/bin/python

import csv
import requests
import sys
import datetime
import random

def generateRandomChannelListWithSpacing():
  lo=11 
  hi=26
  Nch=16
  DIST=4
  
  num=range(Nch)
  print num
  ch = [x+lo for x in num]
  chs=random.sample(ch, len(ch))
  print(chs)
  random.shuffle(ch)
  print(ch)
  
  sss=[0]*(16*16+16*3+1)
  for j in range(Nch):
    for i in range(Nch):
      for k in range(3):
        sss[j*16+i*3+k]=((chs[j]+i+5*k) % Nch) + lo
        
  #print cch
  print sss[:256], len(sss[:256])

def generateCoojaMoteList():
  template='''
  <mote>
    <breakpoints />
    <interface_config>
      org.contikios.cooja.interfaces.Position
      <x>%f</x>
      <y>%f</y>
      <z>0.0</z>
    </interface_config>
    <interface_config>
      org.contikios.cooja.mspmote.interfaces.MspClock
      <deviation>1.0</deviation>
    </interface_config>
    <interface_config>
      org.contikios.cooja.mspmote.interfaces.MspMoteID
      <id>%d</id>
    </interface_config>
    <motetype_identifier>sky1</motetype_identifier>
  </mote>
  '''
  nlist=[118,209,119,217,224,206,210,117,207,226,222,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,150,151,152,153,200,201,202,203,204,205,208,211,212,213,214,215,216,218,219,220,221,223,225]
  
  x=0.0
  y=0.0
  yspace=50
  xspace=50
  rows=3
  ymod=rows*yspace
  for n in nlist:
    print template % (x+random.randint(-xspace/2, xspace/2),y+random.randint(-yspace/2, yspace/2),n)
    y = (y + yspace) % ymod 
    if (y==0):
      x = x + xspace
      
      
def fetchEWSNListAndUpdateConfig():
  headerFileName='ewsn-nodes.h'
  fname = 'List_of_node_addresses.txt'
  url = 'https://iti-testbed.tugraz.at/static/upload/' + fname
  
  parseMac= lambda x: [int(r,16) for r in str(x).split(':')[4:8]]
  
  timenow=datetime.datetime.now()
  sys.stdout.write("//file: %s\n" %headerFileName)
  sys.stdout.write("//url: %s\n" %url)
  sys.stdout.write("//time: %s\n" %timenow)
  
  r = requests.get(url)
  
  
  open(fname , 'wb').write(r.content)
  
  with open(fname, 'rb') as csvfile:
    idsreader = csv.reader(csvfile, delimiter=' ')
    next(idsreader, None)  # skip the headers  
    idsdata=[[int(row[0],10), row[1], row[2]] for row in idsreader]
    
  macs=[parseMac(x[2]) for x in idsdata]
  ids=[x[0] for x in idsdata]
  
  
  sys.stdout.write("\n#ifndef __%s__ " %headerFileName.upper())
  sys.stdout.write("\n#define __%s__ " %headerFileName.upper())
  sys.stdout.write('\n#warning "ID list last updated at __%s__"\n ' %timenow)
  
  sys.stdout.write("\n#define EWSN_IDS ((uint16_t[]){")
  for id in ids[:len(ids) - 1]:
    sys.stdout.write("%d," %(id))
  sys.stdout.write("%d}) \n" %(ids[-1]))
  
  sys.stdout.write("\n#define EWSN_MACS ((uint8_t[][4]){")
  for m in macs:
    sys.stdout.write("{0x%02x, 0x%02x, 0x%02x, 0x%02x},\\\n" %(m[0], m[1], m[2], m[3]))
  sys.stdout.write("}) \n")
  
  sys.stdout.write("\n#endif\n")
  
  sys.stdout.flush()
  
if __name__ == '__main__':
  fetchEWSNListAndUpdateConfig()



