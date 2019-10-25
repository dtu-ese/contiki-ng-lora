CONTIKI_PROJECT = coap-example-server
all: $(CONTIKI_PROJECT)

# Do not try to build on Sky because of code size limitation
PLATFORMS_EXCLUDE = sky z1

# Include the CoAP implementation
MODULES += os/net/app-layer/coap
MODULES += arch/dev/sx1272
MODULES += os/services/orchestra

# Include RPL BR module
MODULES += os/services/rpl-border-router
MAKE_NET = MAKE_NET_IPV6
MAKE_ROUTING = MAKE_ROUTING_RPL_LITE
MAKE_MAC = MAKE_MAC_TSCH

# Include CoAP resources
MODULES_REL += ./resources

CONTIKI=../../../..
include $(CONTIKI)/Makefile.include
