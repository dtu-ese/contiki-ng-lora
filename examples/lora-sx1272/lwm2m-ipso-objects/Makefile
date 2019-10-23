CONTIKI_PROJECT = example-ipso-objects-ordinary

CONTIKI_SOURCEFILES += serial-protocol.c example-ipso-temperature.c

PLATFORMS_EXCLUDE = sky z1

all: $(CONTIKI_PROJECT)

MODULES += os/services/shell
MODULES += arch/dev/sx1272
MODULES += os/services/orchestra

MAKE_NET = MAKE_NET_IPV6
MAKE_ROUTING = MAKE_ROUTING_RPL_LITE
MAKE_MAC = MAKE_MAC_TSCH

MODULES_REL += $(TARGET)

MODULES += os/net/app-layer/coap
MODULES += os/services/lwm2m
MODULES += os/services/ipso-objects

CONTIKI=../../..
include $(CONTIKI)/Makefile.identify-target
MODULES_REL += $(TARGET)

include $(CONTIKI)/Makefile.include
