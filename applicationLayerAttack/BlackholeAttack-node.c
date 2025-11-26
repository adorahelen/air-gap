#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <string.h>

#include "sys/log.h"
#define LOG_MODULE "Malicious"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

/*
공격자 노드가 자신에게 전송되는 모든 패킷을 무시 => drop
: 네트워크 상에서는 공격자 노드가 "응답 없음"처럼 보임  
*/



static struct simple_udp_connection malicious_conn;

/*---------------------------------------------------------------------------*/
PROCESS(malicious_node_process, "Malicious Blackhole Node");
AUTOSTART_PROCESSES(&malicious_node_process);
/*---------------------------------------------------------------------------*/

// ★ 공격: 패킷을 드랍하는 콜백 함수
static void
malicious_rx_callback(struct simple_udp_connection *c,
                      const uip_ipaddr_t *sender_addr,
                      uint16_t sender_port,
                      const uip_ipaddr_t *receiver_addr,
                      uint16_t receiver_port,
                      const uint8_t *data,
                      uint16_t datalen)
{
  // 아무것도 하지 않음 = 패킷 드랍
  LOG_INFO("Dropping packet from ");
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(malicious_node_process, ev, data)
{
  PROCESS_BEGIN();

  LOG_INFO("Malicious node started (Blackhole attack)\n");

  /*
   * 공격자는 단순히 드랍만 하기 때문에
   * PORT 설정은 정상 노드와 동일하게 등록하면 됨.
   */
  simple_udp_register(&malicious_conn,
                      UDP_SERVER_PORT,  /* listen port */
                      NULL,
                      UDP_CLIENT_PORT,
                      malicious_rx_callback);

  // 공격자는 전송을 하지 않음 → 받기만 하고 drop
  while(1) {
    PROCESS_YIELD();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
