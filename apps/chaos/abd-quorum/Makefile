TARGET ?= sky

CONTIKI_PROJECT = abd-quorum-app
CHAOS_NODE_DYNAMIC=$(dynamic)

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include

id: burn-nodeid.upload

all: $(CONTIKI_PROJECT)
