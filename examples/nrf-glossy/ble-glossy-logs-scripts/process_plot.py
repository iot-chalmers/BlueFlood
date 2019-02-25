import re
import time
import os
import sys
import gc
import numpy as np
#to dump objects
import cPickle as pickle
import operator

import matplotlib
matplotlib.use('PDF')
import matplotlib.pyplot as plt
import pylab
from numpy.core.function_base import linspace
from numpy.lib.function_base import meshgrid
from numpy.core.numeric import array
from matplotlib import colors
from mpl_toolkits.mplot3d.axes3d import Axes3D
#from mpl_toolkits import mplot3d
from matplotlib.collections import LineCollection, PolyCollection
from matplotlib.colors import colorConverter

from matplotlib import cm
from collections import OrderedDict, Set

#constants
experiment_time_stamp_idx=0
ble_mode_idx=1
tx_power_idx=2
tx_offset_idx=3
capture_idx=4
prr_1_idx=5
prr_2_idx=6
prr_ct_idx=7
rssi_avg_idx=8
rssi_stddev_idx=9
ts_delta_avg_idx=10
ts_delta_stddev_idx=11

#constatnts
Ble_1Mbit=3 #/*!< 1 Mbit/s Bluetooth Low Energy */
Ble_2Mbit=4 #/*!< 2 Mbit/s Bluetooth Low Energy */
Ble_LR125Kbit=5 #/*!< Long range 125 kbit/s (TX Only - RX supports both) */
Ble_LR500Kbit=6 #/*!< Long range 500 kbit/s (TX Only - RX supports both) */
ble_mode_str_list = ["1Mbps", "2Mbps", "LR-125Kbps", "LR-500Kbps"]

def ble_mode_str(m):
  return ble_mode_str_list[m-3] if (m>=3 or m<=6) else ("RF Mode Unknown %d" %(m))

plot_ts_delta_over = False
plot_rssi_diff_over = False

linestylesDict = OrderedDict(
    [('solid',               (0, ())),
     ('loosely dotted',      (0, (1, 10))),
     ('dotted',              (0, (1, 5))),
     ('densely dotted',      (0, (1, 1))),

     ('loosely dashed',      (0, (5, 10))),
     ('dashed',              (0, (5, 5))),
     ('densely dashed',      (0, (5, 1))),

     ('loosely dashdotted',  (0, (3, 10, 1, 10))),
     ('dashdotted',          (0, (3, 5, 1, 5))),
     ('densely dashdotted',  (0, (3, 1, 1, 1))),

     ('loosely dashdotdotted', (0, (3, 10, 1, 10, 1, 10))),
     ('dashdotdotted',         (0, (3, 5, 1, 5, 1, 5))),
     ('densely dashdotdotted', (0, (3, 1, 1, 1, 1, 1)))])
linestyles = linestylesDict.items()
markers = ['o', '8', 's', 'p', '*', 'h', 'H', 'D', 'd', 'v', '^', '<', '>',]

def plot0(wdir, results):
  plot_rows = 6
  plot_cols = 2
  plot_rows = 1
  plot_cols = 2
  plot_idx = 1
  norm = matplotlib.colors.Normalize(vmin=1, vmax=4)
  linewidth=0.3
  #fig = plt.figure(figsize=(4, 6))  #width, height
  fig, axx = plt.subplots(ncols=2, sharey=True, sharex=True,figsize=(6,1))
  axmain = fig.add_subplot(111,frameon=False)
  # Turn off axis lines and ticks of the big subplot
  axmain.tick_params(labelcolor='none', top='off', bottom='off', left='off', right='off')

  labels = ()
  titles = ()
  txoffset = 0
  plotfilename="evaluate_txpower_plot0.pdf"
  file_name = os.path.join(wdir, plotfilename)
  print "plotting: " + file_name
  for capture in [0,1]:
    plot_idx =1
    ax = axx[capture]
    ax.set_ylim(-0.5, 100.5)
    ax.set_xlim(-16, 8)
    ax.grid(True, which="both")
    if capture == 0:
      title = ax.set_title("Same data")
    else:
      title = ax.set_title("Different data")
    titles += (title,)
    linestyle='--'
    for ble_mode in [Ble_1Mbit, Ble_2Mbit, Ble_LR125Kbit, Ble_LR500Kbit]:
      prr_txpower_list = [[r[tx_power_idx][1]-r[tx_power_idx][0], 100*r[prr_ct_idx], r[rssi_avg_idx][1]-r[rssi_avg_idx][0], r[rssi_stddev_idx][2], r[ts_delta_avg_idx][1]-r[ts_delta_avg_idx][0]] for r in results if (r[ble_mode_idx] == ble_mode) and (r[tx_offset_idx] == txoffset) and (r[capture_idx] == capture) ]
      prr_txpower_list.sort()
      prr_txpower = np.array(prr_txpower_list)
      if len(prr_txpower) > 0:
        #print plot_idx, ble_mode, prr_txpower
        legend = "%s" %(ble_mode_str(ble_mode))
        marker=markers[plot_idx]
        color=plt.get_cmap('Accent')(norm(plot_idx))
        plot_idx +=1
        plot_prr_txpower0(ax, prr_txpower, legend, linestyle, marker, color, linewidth)
        
  handles, labels = axx[0].get_legend_handles_labels()
  axx[0].locator_params(nbins=4, axis='y')
  axx[0].legend(handles, labels, loc="lower left",  ncol=1)#, bbox_to_anchor=(0.46, 1.3)) #prop={'size':12}, )
  x_label = axmain.set_xlabel('TX power delta [dB]')
  y_label = axmain.set_ylabel('PRR %')
  labels = (x_label,y_label)
  fig.subplots_adjust(left=0.2, top=2, hspace=0)
  plt.savefig(file_name, bbox_extra_artists=labels, bbox_inches='tight', transparent=True)
  plt.close(fig)

def plot(wdir, results):
  plot_rows = 6
  plot_cols = 2
  plot_idx = 1
  fig = plt.figure(figsize=(20, 22))  #width, height
  
  labels = ()
  titles = ()
  txoffset = 0
  plotfilename="evaluate_txpower_plot.pdf"
  file_name = os.path.join(wdir, plotfilename)
  print "plotting: " + file_name
  for capture in [0,1]:
    for ble_mode in [Ble_1Mbit, Ble_2Mbit, Ble_LR125Kbit, Ble_LR500Kbit]:
      prr_txpower_list = [[r[tx_power_idx][1]-r[tx_power_idx][0], 100*r[prr_ct_idx], r[rssi_avg_idx][1]-r[rssi_avg_idx][0], r[rssi_stddev_idx][2], r[ts_delta_avg_idx][1]-r[ts_delta_avg_idx][0]] for r in results if (r[ble_mode_idx] == ble_mode) and (r[tx_offset_idx] == txoffset) and (r[capture_idx] == capture) ]
      prr_txpower_list.sort()
      prr_txpower = np.array(prr_txpower_list)
      if len(prr_txpower) > 0:
        #print plot_idx, ble_mode, prr_txpower
        subtitle = "ble_mode_%s_txoffset_%d_capture_%d" %(ble_mode_str(ble_mode), txoffset, capture)
        x_label_tx, y_label_tx, title = plot_prr_txpower(fig, prr_txpower, plot_rows, plot_cols, plot_idx, subtitle)
        labels += (x_label_tx,y_label_tx,)
        titles += (title,)
        plot_idx +=1

  plt.subplots_adjust(left=0.2, top=2)
  plt.savefig(file_name, bbox_extra_artists=labels, bbox_inches='tight')
  plt.close(fig)

  plot_idx = 1
  fig = plt.figure(figsize=(20, 22))  #width, height
  
  labels = ()
  titles = ()
  plotfilename="evaluate_txpower_txoffset_plot.pdf"
  file_name = os.path.join(wdir, plotfilename)
  print "plotting: " + file_name
  for capture in [0,1]:
    for ble_mode in [Ble_1Mbit, Ble_2Mbit, Ble_LR125Kbit, Ble_LR500Kbit]:
      prr_txpower_list = [[r[tx_power_idx][1]-r[tx_power_idx][0], r[tx_offset_idx], 100*r[prr_ct_idx], r[rssi_avg_idx][1]-r[rssi_avg_idx][0], r[rssi_stddev_idx][2], r[ts_delta_avg_idx][1]-r[ts_delta_avg_idx][0]] for r in results if (r[ble_mode_idx] == ble_mode) and (r[capture_idx] == capture) ]
      prr_txpower_list.sort(key = operator.itemgetter(1,0))
      prr_txpower = np.array(prr_txpower_list)
      if len(prr_txpower) > 0:
        #print plot_idx, ble_mode, prr_txpower
        subtitle = "ble_mode_%s_capture_%d" %(ble_mode_str(ble_mode), capture)
        x_label_tx, y_label_tx, z_label, title = plot_prr_txpower_tsdelta_3d(fig, prr_txpower, plot_rows, plot_cols, plot_idx, subtitle)
        labels += (x_label_tx,y_label_tx,z_label,)
        titles += (title,)
        plot_idx +=1
  plt.subplots_adjust(left=0.2, top=2)
  plt.savefig(file_name, bbox_extra_artists=labels, bbox_inches='tight')
  plt.close(fig)

def plot_prr_txpower_tsdelta_3d(fig, prr_txpower, plot_rows, plot_cols, plot_idx_, subtitle=None):
  ax = fig.add_subplot(plot_rows, plot_cols, plot_idx_, projection='3d')
  x=prr_txpower[:,0]
  y=prr_txpower[:,1]
  zs=prr_txpower[:,2]
  #ax.plot3D(x,y,zs)
  ax.plot_trisurf(x,y, zs, cmap=cm.viridis, linewidth=0.2)

  # vv=np.column_stack((x, y))
  # print vv.shape, vv.shape[1], vv.ndim
  # alpha_=0.5  # to accentuate the issue
  # #cc = lambda arg: colorConverter.to_rgba(arg, alpha=alpha_)
  # poly = PolyCollection(vv)
  # poly.set_alpha(alpha_)
  # ax.add_collection3d(vv, zs=zs, zdir='z')

  ax.set_zlim(-0.01, 100.1)
  ax.set_xlim(-40.01, 10)
  ax.set_ylim(-0.01, 28)
  ax.grid()
  x_label = ax.set_xlabel('TX power delta [dB] - configured')
  y_label = ax.set_ylabel('TS [1/16 us]')
  z_label = ax.set_zlabel('PRR %')
  title = ax.set_title(subtitle)
  return x_label, y_label, z_label, title

def plot_prr_txpower(fig, prr_txpower, plot_rows, plot_cols, plot_idx_, subtitle=None):
  ax = fig.add_subplot(plot_rows, plot_cols, plot_idx_)
  #print prr_txpower[:,0], prr_txpower[:,1]
  ax.plot(prr_txpower[:,0], prr_txpower[:,1], linestyle='--', marker='o', color='b')
  ax.grid()
  if plot_rssi_diff_over:
    ax2 = ax.twinx()  # instantiate a second axes that shares the same x-axis
    color2 = 'tab:grey'
    ax2.bar(prr_txpower[:,0], prr_txpower[:,2], yerr=prr_txpower[:,3], color=color2)    
    ax2.tick_params(axis='y', labelcolor=color2)
    ax2.grid()
    y2_label = ax2.set_ylabel('RX power delta [dB]', color=color2)
  if plot_ts_delta_over:
    ax3 = ax.twinx()  # instantiate a second axes that shares the same x-axis
    color3 = 'tab:grey'
    ax3.bar(prr_txpower[:,0], prr_txpower[:,4], color=color3)    
    ax3.tick_params(axis='y', labelcolor=color3)
  #ax2.grid()
  x_label = ax.set_xlabel('TX power delta [dB] - configured')
  y_label = ax.set_ylabel('PRR %')
  title = ax.set_title(subtitle)
  return x_label, y_label, title

def plot_prr_txpower0(ax, prr_txpower, label=None, linestyle='--', marker='o', color='b', linewidth=0.2):
  #print prr_txpower[:,0], prr_txpower[:,1]
  ax.plot(prr_txpower[:,0], prr_txpower[:,1], label = label, linestyle=linestyle, marker=marker, color=color, linewidth=linewidth)
  ax.grid()
  if plot_rssi_diff_over:
    ax2 = ax.twinx()  # instantiate a second axes that shares the same x-axis
    color2 = 'tab:grey'
    ax2.bar(prr_txpower[:,0], prr_txpower[:,2], yerr=prr_txpower[:,3], color=color2)    
    ax2.tick_params(axis='y', labelcolor=color2)
    ax2.grid()
    y2_label = ax2.set_ylabel('RX power delta [dB]', color=color2)
  if plot_ts_delta_over:
    ax3 = ax.twinx()  # instantiate a second axes that shares the same x-axis
    color3 = 'tab:grey'
    ax3.bar(prr_txpower[:,0], prr_txpower[:,4], color=color3)    
    ax3.tick_params(axis='y', labelcolor=color3)
  #ax2.grid()

  #return title

def timing(f):
  def wrap(*args, **kwargs):
    time1 = time.time()
    ret = f(*args, **kwargs)
    time2 = time.time()
    print '%s function took %0.3f ms' % (f.func_name, (time2-time1)*1000.0)
    return ret
  return wrap

def saveObject(userobj, filename):
    with open(filename, 'wb') as outputFile:
        pickle.dump(userobj, outputFile, pickle.HIGHEST_PROTOCOL)

def loadObject(filename):
    userobj = None
    with open(filename, 'rb') as inputFile:
        userobj = pickle.load(inputFile)
    return userobj

#1534763630098560        ID:10   {tx-3} :8-----------------------
line_regex = re.compile("(\d+)\s+ID\:(\d+)\s+\{(\w+)-(\d+)\}\s(.*)")
testbed_node_ids=[2,3,10,12,16,17,19,20]
def parse_experiment_line(line):
  #93_2018_08_20_13_12_56_ble_mode_3_txpower_-20_txoffset_0_capture_0
  res = line_regex.match(line)
  parsed = None
  if res:
    ts = int(res.group(1))
    nid = int(res.group(2))
    subject = str(res.group(3))
    rd = int(res.group(4))
    lline = str(res.group(5))
    parsed = ts, nid, subject, rd, lline
  return parsed

dirname_regex = re.compile("(\d+)_(\d+)_(\d+)_(\d+)_(\d+)_(\d+)_(\d+)_ble_mode_(\d)_txpower_(-?\d+)_txoffset_(-?\d+)_capture_(\d)")
def parse_experiment_dirname(dirname):
  #93_2018_08_20_13_12_56_ble_mode_3_txpower_-20_txoffset_0_capture_0
  res = dirname_regex.match(dirname)
  print dirname
  parsed = None
  if res:
    exp_number = int(res.group(1))
    ts = [int(res.group(2)), int(res.group(3)), int(res.group(4)), int(res.group(5)), int(res.group(6)),int(res.group(7))]
    ble_mode = int(res.group(8))
    txpower = int(res.group(9))
    txoffset = int(res.group(10))
    capture = int(res.group(11))
    parsed = [exp_number, ts, ble_mode, txpower, txoffset, capture]
  return parsed

filename_regex = re.compile("exp(\d+)_(\d+)_(\d+)_(\d+)_(\d+)_(\d+)_ble_mode_(\d)_txpower_(-?\d+)_(-?\d+)_txoffset_(-?\d+)_capture_(\d).log")
def parse_experiment_filename(log):
  res = filename_regex.match(log)
  parsed = None
  if res:
    #timestamp = float(res.group(1))
    ts = [int(res.group(1)), int(res.group(2)), int(res.group(3)), int(res.group(4)), int(res.group(5)), int(res.group(6))]
    ble_mode = int(res.group(7))
    txpower = [int(res.group(8)), int(res.group(9))]
    txoffset = int(res.group(10))
    capture = int(res.group(11))
    parsed = [ts, ble_mode, txpower, txoffset, capture]
  return parsed

def tx_status_map_bad_rx(t):
  ''' 
  '-': rx ok
  '1': mote 1 tx
  '2': mote 2 tx
  'B': both tx
  ':': skipped slot
  ''' 
  if(t in ['-', '1', '2', 'B', ':', 'X', '\0']):
    return 0
  else:
    return 1

def tx_status_map_tx(t):
  ''' 
  '-': rx ok
  '1': mote 1 tx
  '2': mote 2 tx
  'B': both tx
  ':': skipped slot
  ''' 
  if(t in ['1', '2', 'B', 'X']):
    return 1
  else:
    return 0

def tx_status_map_tx2(t):
  ''' 
  '-': rx ok
  '1': mote 1 tx
  '2': mote 2 tx
  'B': both tx
  ':': skipped slot
  ''' 
  if(t in ['-', ':']):
    return 0
  elif(t in ['1', '2', 'B', 'X']):  
    return 1
  else:
    return 2

ROUND_LEN=24
def load_testbed_log_file(in_filename, dirname):
  #experiment_parameters
  dn = os.path.basename(os.path.normpath(dirname))
  exp_number, ts, ble_mode, txpower, txoffset, capture = parse_experiment_dirname(dn)

  print dn, [exp_number, ts, ble_mode, txpower, txoffset, capture]
  ts_delta = np.zeros((len(testbed_node_ids),ROUND_LEN))
  tx_status = np.zeros((len(testbed_node_ids),ROUND_LEN))
  rounds = []
  tx_status_translated_acc = [0]*24

  total_b_rx = 0
  total_b_ok = 0

  rssi=[]*len(testbed_node_ids)
  ts_delta=[]
  rssi_tmp = []
  rssi_tot = []*len(testbed_node_ids)
  ts_delta_tot = []*len(testbed_node_ids)
  start_processing = False
  nodes = set()
  
  
  for lline in open(os.path.join(dirname, in_filename), 'r').readlines():
    if lline:
      sline=lline.strip()
      parsed = parse_experiment_line(sline)
      if parsed:
        print parsed
        ts, nid, subject, rd, lline = parsed
      else:
        continue

      node_index = testbed_node_ids.index(nid)
      if(nid not in nodes):
        nodes.add(nid)

      #np.zeros((len(testbed_node_ids),24))

      if subject.startswith("rssi"):
        sstr = lline.split(',')
        rssi_tmp = [int(r) for r in sstr[2:len(sstr)-1]]
        
        #print rssi
      elif subject.startswith("td"):
        sstr = lline.split(',')
        ts_delta_tmp = [int(r) for r in sstr[2:len(sstr)-1]]
        #print ts_delta
      elif subject.startswith("tx"):
        lline=str(lline)
        if len(lline) > 1:
          ch=lline[1]
          lline=lline.replace('.',ch) #replace .
          #print lline
        else:
          continue

        tx_status = lline[1:]
        tx_status_bad_rx = [tx_status_map_bad_rx(t) for t in tx_status[1:]]
        tx_status_tx = [tx_status_map_tx(t) for t in tx_status[1:]]
        tot=len(tx_status[1:])
        tot_tx = sum(tx_status_tx)
        tot_not_ok = sum(tx_status_bad_rx)
        tot_ok = tot - tot_not_ok - tot_tx
        tot_rx = tot - tot_tx
        #print tx_status[1:], tx_status_tx, tot, tot_tx, tot_rx, tot_ok, tot_not_ok
        
        rssi_ok = []
        ts_ok = []
        if len(tx_status) > 1:
          for i in range(1,len(tx_status)-1):
            if tx_status[i] == '-':
              rssi_ok.append(rssi_tmp[i])
              ts_ok.append(ts_delta_tmp[i])
              
        #rssi_ok = [rssi_tmp[i] for i in range(1,len(tx_status)-1) if tx_status[i] == '-'] if len(tx_status) > 1 and len(rssi_tmp) > 1 else []
        #ts_ok = [ts_delta_tmp[i] for i in range(1,len(tx_status)-1) if tx_status[i] == '-'] if len(tx_status) > 1 and len(ts_delta_tmp) > 1 else []

        if(tx_status[0] == 'B'):
          total_b_rx += tot_rx
          total_b_ok += tot_ok
        
        if len(rssi_ok) > 0:
          rssi.extend(rssi_ok)
        if len(ts_ok) > 0:
          ts_delta.extend(ts_ok)
  #result = [ts, ble_mode, txpower, txoffset, capture, 1.0*total_1_ok/total_1_rx, 1.0*total_2_ok/total_2_rx,1.0*total_b_ok/total_b_rx] if start_processing else [ts, ble_mode, txpower, txoffset, capture, 0,0,0]
  prr3=1.0*total_b_ok/total_b_rx if total_b_rx != 0 else 0

  rssi_arr = np.array(rssi)
  #print rssi_arr
  rssi_tot = np.average(rssi_arr)
  rssi_stddev = np.std(rssi_arr)
  #print rssi_tot, rssi_stddev
  ts_delta_arr = np.array(ts_delta)
  #print ts_delta_arr
  ts_delta_tot = np.average(ts_delta_arr)
  ts_delta_stddev = np.std(ts_delta_arr)
  #print ts_delta_tot, ts_delta_stddev
  result = [ts, ble_mode, txpower, txoffset, capture, prr3]
  result.append(rssi_tot)
  result.append(rssi_stddev)
  result.append(ts_delta_tot)
  result.append(ts_delta_stddev)
  print result
  #print rssi_tot
  #print total_1_rx, total_1_ok, total_2_rx, total_2_ok, total_b_rx, total_b_ok
  return result

def load_log_file(in_filename, dirname):
# rx_ok 23, crc 0, none 0, tx 1: OK 5244 of 5244, berr b0 p0 r0 0
# Rssi: -111, -127, -22, -23, -27, -22, -27, -28, -30, -23, -25, -22, -28, -25, -29, -23, -25, -28, -22, -25, -22, -22, -24, -23, -22, dB
# Ts delta: 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, ticks
# Tx status: :B-----------------------
# R 226, ID: 0x724d2a67, master: 0x724d2a67, tx power: 0 dBm, channel 37 = 2498 MHz (not std), msg: 38 bytes, mode: Ble_2Mbit, CE: 0, @ Jun 13 2018 13:01:06
#exp2018_06_14_15_06_58_ble_mode_4_txpower_0_3_txoffset_20_capture_0
  
  #experiment_parameters
  [ts, ble_mode, txpower, txoffset, capture] = parse_experiment_filename(in_filename)
  #print in_filename, [ts, ble_mode, txpower, txoffset, capture]
  ts_delta = []
  tx_status = []
  rounds = []
  tx_status_translated_acc = [0]*24
  total_1_rx = 0
  total_2_rx = 0
  total_b_rx = 0
  total_b_ok = 0
  total_1_ok = 0
  total_2_ok = 0
  rssi=[[],[],[]]
  ts_delta=[[],[],[]]
  rssi_tmp = []
  rssi_tot = [0]*3
  ts_delta_tot = [0]*3
  start_processing = False

  for lline in open(os.path.join(dirname, in_filename), 'r').readlines():
    if lline:
      line=lline.strip()
      if line.startswith("Testing"):
        start_processing = True
        #print line
        continue

      if not start_processing:
        continue
      
      if line.startswith("Rssi: "):
        sstr = line.split("Rssi: ")[1].split(',')
        rssi_tmp = [int(r) for r in sstr[2:len(sstr)-1]]
        #print rssi
      elif line.startswith("Ts delta: "):
        sstr = line.split("Ts delta: ")[1].split(',')
        ts_delta_tmp = [int(r) for r in sstr[2:len(sstr)-1]]
        #print ts_delta
      elif line.startswith("Tx status: "):
        tx_status = line.split("Tx status: ")[1][1:]
        tx_status_bad_rx = [tx_status_map_bad_rx(t) for t in tx_status[1:]]
        tx_status_tx = [tx_status_map_tx(t) for t in tx_status[1:]]
        tot=len(tx_status[1:])
        tot_tx = sum(tx_status_tx)
        tot_not_ok = sum(tx_status_bad_rx)
        tot_ok = tot - tot_not_ok - tot_tx
        tot_rx = tot - tot_tx
        #print tx_status[1:], tx_status_tx, tot, tot_tx, tot_rx, tot_ok, tot_not_ok
        
        rssi_ok = []
        ts_ok = []
        if len(tx_status) > 1:
          for i in range(1,len(tx_status)-1):
            if tx_status[i] == '-':
              rssi_ok.append(rssi_tmp[i])
              ts_ok.append(ts_delta_tmp[i])
              
        #rssi_ok = [rssi_tmp[i] for i in range(1,len(tx_status)-1) if tx_status[i] == '-'] if len(tx_status) > 1 and len(rssi_tmp) > 1 else []
        #ts_ok = [ts_delta_tmp[i] for i in range(1,len(tx_status)-1) if tx_status[i] == '-'] if len(tx_status) > 1 and len(ts_delta_tmp) > 1 else []

        if(tx_status[0] == '1'):
          i=0
          total_1_rx += tot_rx
          total_1_ok += tot_ok
        elif(tx_status[0] == '2'):
          i=1
          total_2_rx += tot_rx
          total_2_ok += tot_ok
        elif(tx_status[0] == 'B'):
          i=2
          total_b_rx += tot_rx
          total_b_ok += tot_ok
        else:
          i=3
          print line
        
        if i < 3 and len(rssi_ok) > 0:
          rssi[i].extend(rssi_ok)
        if i < 3 and len(ts_ok) > 0:
          ts_delta[i].extend(ts_ok)
  #result = [ts, ble_mode, txpower, txoffset, capture, 1.0*total_1_ok/total_1_rx, 1.0*total_2_ok/total_2_rx,1.0*total_b_ok/total_b_rx] if start_processing else [ts, ble_mode, txpower, txoffset, capture, 0,0,0]
  prr1=1.0*total_1_ok/total_1_rx if total_1_rx != 0 else 0
  prr2=1.0*total_2_ok/total_2_rx if total_2_rx != 0 else 0
  prr3=1.0*total_b_ok/total_b_rx if total_b_rx != 0 else 0
  if(prr1 == 0 or prr2 == 0): #extend with rssi_failed and ts_failed
    result = [ts, ble_mode, txpower, txoffset, capture, -1, -1, prr3,[-127]*3,[0]*3,[0]*3,[0]*3]
    print "!Error: repeat experiment in_filename" 
  else:
    for i in range(0,3):
      if len(rssi[i]) == 0:
        rssi[i].extend([0])      
      if len(ts_delta[i]) == 0:
        ts_delta[i].extend([0])
    rssi_arr = np.array(rssi)
    #print rssi_arr
    rssi_tot = [np.average(rssi_arr[i]) for i in range(0,3)]
    rssi_stddev = [np.std(rssi_arr[i]) for i in range(0,3)]
    #print rssi_tot, rssi_stddev
    ts_delta_arr = np.array(ts_delta)
    #print ts_delta_arr
    ts_delta_tot = [np.average(ts_delta_arr[i]) for i in range(0,3)]
    ts_delta_stddev = [np.std(ts_delta_arr[i]) for i in range(0,3)]
    #print ts_delta_tot, ts_delta_stddev
    result = [ts, ble_mode, txpower, txoffset, capture, prr1, prr2, prr3]
    result.append(rssi_tot)
    result.append(rssi_stddev)
    result.append(ts_delta_tot)
    result.append(ts_delta_stddev)

  #print rssi_tot
  #print total_1_rx, total_1_ok, total_2_rx, total_2_ok, total_b_rx, total_b_ok
  return result

if __name__ == '__main__':
  wdir="/Users/beshr/work/chaos/examples/nrf-glossy/exp-logs"
  use_saved_results = 1
  results =[]
  if len(sys.argv) >= 2:
    use_saved_results = int(sys.argv[1])
  if use_saved_results:
    results = loadObject(os.path.join(wdir, "results.pickle"))
  else:
    for infilename in os.listdir(wdir):
      if infilename.endswith(".log"): 
        # print(os.path.join(directory, filename))
        #infilename="exp2018_06_14_15_06_58_ble_mode_4_txpower_0_3_txoffset_0_capture_0.log"
        #infilename="exp2018_06_14_12_39_32_ble_mode_4_txpower_0_3_txoffset_4_capture_0.log"
        print '#loading: ' + infilename
        #[ts, ble_mode, txpower, txoffset, capture, prr1, prr2, prr3, rssi_tot, rssi_stddev]
        result = load_log_file(infilename, wdir)
        #result = [ble_mode, txpower[1]-txpower[0], txoffset, capture, 100*prr1, 100*prr2, 100*prr3, rssi_tot, rssi_stddev]
        if result[prr_1_idx] != -1:
          results.append(result)
          print result
      else:
        continue
    saveObject(results, os.path.join(wdir, "results.pickle"))

  plot(wdir, results)
  plot0(wdir, results)