TARGET ?= sky

CONTIKI_PROJECT = rx-test-app
CONTIKI_WITH_CHAOS = 1

## default parameters
dynamic=0
log=1
printf=1
logflags=0
src=1
dst=0
rank=0
interval=40
ch=26
mch=0
pch=0
tx=TXPW0

CHAOS_NODE_DYNAMIC=$(dynamic)

CFLAGS += -D_param_log=$(log)
CFLAGS += -D_param_printf=$(printf)
CFLAGS += -D_param_logflags=$(logflags)
CFLAGS += -D_param_src=$(src)
CFLAGS += -D_param_dst=$(dst)
CFLAGS += -D_param_rank=$(rank)
CFLAGS += -D_param_interval=$(interval)
CFLAGS += -D_param_ch=$(ch)
CFLAGS += -D_param_mch=$(mch)
CFLAGS += -D_param_pch=$(pch)
CFLAGS += -D_param_txpw=$(tx)

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

EXPERIMENT_PARAM=log$(log)_printf$(printf)_logflags$(logflags)_src$(src)_dst$(dst)_rank$(rank)_interval$(interval)_ch$(ch)_mch$(mch)_pch$(pch)_tx$(tx)

CONTIKI = ../../../..
include $(CONTIKI)/Makefile.include

id: burn-nodeid.upload 

all: $(CONTIKI_PROJECT)

