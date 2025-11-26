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
- 그중 Cooja에서 구현이 쉬운 공격들을 위주로 구현 ㅇ
