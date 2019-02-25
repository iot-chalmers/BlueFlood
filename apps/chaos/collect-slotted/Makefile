TARGET ?= sky

CONTIKI_PROJECT = collect-slotted-app
CONTIKI_WITH_CHAOS = 1
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include

id: burn-nodeid.upload 

all: $(CONTIKI_PROJECT)
