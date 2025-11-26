# airgap
에어갭 연구과제 

https://ubuntu.com/download/server/arm

<img width="2672" height="1095" alt="image" src="https://github.com/user-attachments/assets/026467b5-983e-4e13-b178-32af1736da66" />

## 설치

```
git clone --recursive https://github.com/contiki-ng/contiki-ng.git
```
- recurisve 안쓰면, cooja 텅빔 -> 하위모듈 누락
- wsl build, docker, vm 
- mac은 arm64<->X86 문제로 호환성 이슈 있음 

## 실행

```
rpl-udp/
 ├── Makefile
 ├── udp-client.c        ← Normal sensor node
 ├── udp-server.c        ← Border router + root 역할
 ├── rpl-udp-cooja.csc   ← Cooja 시뮬레이션 설정파일(멀티노드)
 ├── rpl-udp-sky.csc
 ├── rpl-udp.resc        ← 자동 실행 스크립트
 ├── rpl-udp.robot       ← 테스팅 자동화
```
- 이를 기반으로 악성 노드를 추가하거나 코드를 변조해 공격을 주입 

## 공격 분류 
- 12 카테고리, 37개 공격 존재
- 그중 Cooja에서 구현이 쉬운 공격들을 위주로 구현

### RPL 라우팅 프로토콜 공격 유형 요약

| 공격 종류 | 난이도 | 구현 방식 | 논문 |
| :--- | :--- | :--- | :--- |
| **Rank Decrease Attack** | 쉬움 | 공격 노드가 **Rank**를 인위적으로 낮춰 광고하여 부모 노드가 되도록 유도 | |
| **Sinkhole** | 쉬움 | 낮은 **Rank** 광고 + **Selective Forwarding** (선택적 포워딩)을 통해 트래픽 흡수 | |
| **DIS Flooding** | 쉬움 | 네트워크 부하 유발을 위해 무한 **DIS** (DODAG Information Solicitation) 전송 | |
| **Blackhole** | 쉬움 | 수신한 모든 데이터 **패킷을 Drop** (삭제) | |
| **Grayhole** | 쉬움 | 수신한 데이터 **패킷을 일부만 Drop** (선택적으로 삭제) | |
| **Version Number Attack** | 보통 | **DIO**에 **Version Number**를 급격히 증가시켜 전체 트리 리빌드 유발 | |
| **DAO Dropping / DAO Insider** | 어려움 | **DAO** (Destination Advertisement Object)를 조작하거나 전송 중에 고의적으로 Drop | |
| **Copycat Attack** | 보통 | 이웃 노드의 **DIO**를 도용하여 **Replay** (재전송) | |


<img width="279" height="366" alt="image" src="https://github.com/user-attachments/assets/8f0c708c-6da5-44be-99b9-635760314165" />

- Flooding 공격은, Application Layer에서 동작 
- Copycat 등은 Network Layer에서 동작

