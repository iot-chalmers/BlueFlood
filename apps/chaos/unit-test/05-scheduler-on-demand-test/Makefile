TARGET ?= sky

CONTIKI_PROJECT = scheduler-on-demand-test-app
CONTIKI_WITH_CHAOS = 1

CONTIKI = ../../../..
include $(CONTIKI)/Makefile.include

id: burn-nodeid.upload 

all: $(CONTIKI_PROJECT)

