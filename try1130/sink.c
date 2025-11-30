#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"

#define LOG_MODULE "SINK"
#define LOG_LEVEL LOG_LEVEL_INFO
#define UDP_PORT 1234

static struct simple_udp_connection udp_conn;

PROCESS(sink_process, "RPL Root Node");
AUTOSTART_PROCESSES(&sink_process);

PROCESS_THREAD(sink_process, ev, data)
{
  PROCESS_BEGIN();

  // RPL Root
  NETSTACK_ROUTING.root_start();

  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT,
                      NULL);

  PROCESS_END();
}