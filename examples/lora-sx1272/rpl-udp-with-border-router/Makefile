CONTIKI_PROJECT = udp-client udp-server
all: $(CONTIKI_PROJECT)

MODULES += os/services/shell
MODULES += arch/dev/sx1272
MODULES += os/services/orchestra

# Include RPL BR module
MODULES += os/services/rpl-border-router
# Include webserver module
MODULES_REL += webserver
MODULES_REL += $(TARGET)


MAKE_NET = MAKE_NET_IPV6
MAKE_ROUTING = MAKE_ROUTING_RPL_LITE
MAKE_MAC = MAKE_MAC_TSCH

CONTIKI=../../..
include $(CONTIKI)/Makefile.include
