#TARGET ?= sky

CONTIKI_PROJECT = paxos-app
CONTIKI_WITH_CHAOS = 1
CHAOS_NODE_DYNAMIC=$(dynamic)

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include

CONTIKI_TARGET_SOURCEFILES += contiki-wsn430-platform.c
include $(CONTIKI)/platform/wsn430/Makefile.common


id: burn-nodeid.upload


all: $(CONTIKI_PROJECT)


coojatest: clean
coojatest:
	echo "hi"
	$(MAKE) cooja log=0 printf=1 tx=31 mch=1 pch=0 sec=0 src=1 sync=0 failure=0 dynamic=2 initiator=3 interval=29 max_node_count=15 duration=5 description="paxos-test"

