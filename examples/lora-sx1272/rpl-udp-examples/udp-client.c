#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"
#include "sys/energest.h"
//#include "sx1272-driver-wrapper-for-tsch.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define BYTES_PER_MINUTE  50
#define PACKAGE_SIZE      110
//#define SEND_INTERVAL		  (60 * CLOCK_SECOND)
#define PRINT_INTERVAL		  (60 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{

  LOG_INFO("Received response '%.*s' from ", datalen, (char *) data);
  LOG_INFO_6ADDR(sender_addr);
#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  uint8_t buffer[PACKAGE_SIZE];
  static struct etimer print_timer;
  static struct etimer periodic_timer;
  uip_ipaddr_t dest_ipaddr;

  PROCESS_BEGIN();
  energest_init();
  NETSTACK_MAC.on();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  etimer_set(&periodic_timer, (CLOCK_SECOND*60*PACKAGE_SIZE/BYTES_PER_MINUTE) + CLOCK_SECOND/20*(1-(random_rand() % 3)));
  etimer_set(&print_timer, PRINT_INTERVAL);
  static uint64_t energest_startt = 0;
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    //printf("RSSIREG: %d\n", SX1272Read(REG_LR_RSSIVALUE));
    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
      if (energest_startt == 0){
        energest_startt = ENERGEST_GET_TOTAL_TIME();
      }
      /* Send to DAG root */
      LOG_INFO("Sending data to ");
      LOG_INFO_6ADDR(&dest_ipaddr);
      LOG_INFO_("\n");
      uint8_t checksum = 0;
      for (int i = 0; i < PACKAGE_SIZE; i++){
        buffer[i] = random_rand() % 256;
        checksum += buffer[i];
      }
      simple_udp_sendto(&udp_conn, buffer, PACKAGE_SIZE, &dest_ipaddr);
      LOG_INFO("Sent checksum %d and length%d\n", checksum, PACKAGE_SIZE);
    } else {
      LOG_INFO("Reachable: %d, getIP: %d, actual ip: ", NETSTACK_ROUTING.node_is_reachable(), NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr));
            LOG_INFO_6ADDR(&dest_ipaddr);
      LOG_INFO("\n");
    }

    if (etimer_expired(&print_timer)){
        energest_flush();
        uint64_t energest_time = (float)ENERGEST_GET_TOTAL_TIME()-energest_startt;
    LOG_INFO("Bytes pr min: %d, Packet Size: %d\n", BYTES_PER_MINUTE, PACKAGE_SIZE);

    LOG_INFO("\nEnergest:\n");
    LOG_INFO(" CPU          %llu LPM      %llu DEEP LPM %llu\n",
           energest_type_time(ENERGEST_TYPE_CPU),
           energest_type_time(ENERGEST_TYPE_LPM),
           energest_type_time(ENERGEST_TYPE_DEEP_LPM));
    /*LOG_INFO(" Radio LISTEN %llu TRANSMIT %llu OFF      %llu\n",
           energest_type_time(ENERGEST_TYPE_SX1272_TRANSMIT),
           energest_type_time(ENERGEST_TYPE_SX1272_RX),
           
           (ENERGEST_GET_TOTAL_TIME()
                      - energest_type_time(ENERGEST_TYPE_SX1272_TRANSMIT)
                      - energest_type_time(ENERGEST_TYPE_SX1272_RX)));
  */
    LOG_INFO("Total time: %d minutes, %d seconds, %d ticks pr sec\n", (int)energest_time/ENERGEST_SECOND/60, (int)energest_time/ENERGEST_SECOND % 60, (int)ENERGEST_SECOND);
    /* Add some jitter */
    etimer_set(&print_timer, PRINT_INTERVAL
      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
    }
  etimer_set(&periodic_timer, (CLOCK_SECOND*60*PACKAGE_SIZE/BYTES_PER_MINUTE) + CLOCK_SECOND/20*(1-(random_rand() % 3)));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
