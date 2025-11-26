```

좋아. 불필요한 코드 없이, 네가 정확히 “어느 파일의 어느 부분을 수정해야 하는지”만 딱 집어서 알려줄게.
Copycat 공격은 3개 파일만 수정하면 된다.

⸻

✅ 1. rpl-icmp6.c (가장 중요)

👉 DIO 수신 함수(dio_input()) 안에서 DIO를 ‘저장’하는 코드 추가
👉 저장한 DIO를 나중에 재전송하기 위해 버퍼/길이/원본 IP 저장

수정 위치:

rpl-icmp6.c  
└── static void dio_input(void)

여기 안에서:
	•	DIO가 처음 들어왔을 때 → stored_dio[]에 복사
	•	stored_len = uip_len;
	•	stored_src = UIP_IP_BUF->srcipaddr;
	•	have_dio = 1;

이 네 줄이 핵심.

⸻

✅ 2. rpl-timers.c

👉 Trickle Timer가 원래 조절하던 DIO 전송 주기를 무력화하고,
고정된 interval에서 재전송되도록 수정

수정 위치:

rpl-timers.c  
└── static void handle_dio_timer(struct rpl_instance *instance)

여기서 기존의:

new_interval = trickle_timer_interval();
etimer_set(&instance->dio_timer, new_interval);

⬇⬇⬇ 삭제 또는 주석 처리

대신:

etimer_set(&instance->dio_timer, FIXED_REPLAY_INTERVAL);
post_event_to_replay_DIO();

이 코드가 하나 들어가면 됨.

⸻

✅ 3. uip-icmp6.c

👉 (선택사항: 스푸핑 공격 시만 필요)
👉 저장된 DIO를 재전송할 때 Source IP를 공격자 아닌
“원래 DIO 보낸 정상 노드 IP”로 위조

수정 위치:

uip-icmp6.c  
└── uip_icmp6_send()  또는 tcpip_ipv6_output() 직전

여기서:

UIP_IP_BUF->srcipaddr = stored_src_ip;

만 넣어주면 됨.

⸻

📌 3개 파일 요약

파일	위치	역할
rpl-icmp6.c	dio_input() 내부	수신한 DIO 저장
rpl-timers.c	handle_dio_timer() 내부	DIO를 고정 주기마다 재전송
uip-icmp6.c	uip_icmp6_send() 내부	(옵션) Source IP 스푸핑


⸻

📌 한 줄 요약

DIO 저장은 rpl-icmp6.c, 재전송 타이머는 rpl-timers.c,
Source IP 위조는 uip-icmp6.c 에서 수정하면 된다.

⸻

원하면:
✔ 3개 파일 각각 정확한 줄 번호 기반 삽입 코드
✔ 너의 Contiki 버전 기준 정확히 맞춘 패치 파일(.patch)
✔ 공격자 전체 코드 구성도

바로 만들어줄게.

```