#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"

#define LOG_MODULE "NORMAL"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_PORT 1234

static struct simple_udp_connection udp_conn;

PROCESS(normal_node_process, "Normal RPL node");
AUTOSTART_PROCESSES(&normal_node_process);

PROCESS_THREAD(normal_node_process, ev, data)
{
  static struct etimer periodic;

  PROCESS_BEGIN();

  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);

  etimer_set(&periodic, CLOCK_SECOND * 30);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic));

    if(NETSTACK_ROUTING.node_is_reachable()) {
      uip_ipaddr_t dest;
      NETSTACK_ROUTING.get_root_ipaddr(&dest);
      LOG_INFO("Sending data to root\n");
      simple_udp_sendto(&udp_conn, "hello", 5, &dest);
    }

    etimer_reset(&periodic);
  }

  PROCESS_END();
}