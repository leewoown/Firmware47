# SOC 재기동 점프 — 그래프 판독 및 SW 변경 검토

> **SOC_FailR3 / 2026-08-31** | 작성 이우원 사업본부장
> 선행 문서 : [SOC_FailR1.md](SOC_FailR1.md) — N1 실기 FAIL, NVR 미동작 확정, 결함 F-1~F-7
> 본 문서 : ① TSMaster 그래프(BATSYS_Infro) 판독 ② 260831 SW 변경분(0.18) 적용 전 검토 R-1~R-7

---

## 1. 그래프 판독 (BATSYS_Infro, BSA_Data 로그와 동일 데이터)

우측 끝 값 일치로 동일 데이터 확인 : SOC 71.80 % / 팩 85.90 V / Ah 0.20 / 셀 평균 3.91 V

### 1-1. 시간 순 구간 해석

| 구간 | 그래프 관측 | 실제 상황 |
|---|---|---|
| ① 좌측 절반 | 전 전압선 완만한 우하향, SOC 서서히 하강 | 소전류 방전 (SOC 65 → 50 %) |
| ② 중간 | 전압선 급상승, 전류(청록) +54 A 상승 | 충전 (SOC 50 → 61.4 %) |
| ③ 충전 직후 | **전류 0 A 인데 전압이 계속 하강** | **Relaxation 진행 중 — 핵심 증거** |
| ④ 중앙 공백 | 신호 두절, 수평 보간선만 | 제어전원 OFF (0x601 단절 7.14 s + PWRHOLD 297 s) |
| ⑤ 우측 | SOC(분홍)만 한 계단 상승 | 재기동 → SOC 61.4 → 71.2 % **점프** |

### 1-2. ③ 구간이 중요한 이유

전류 0 A 에서 셀 전압 : 충전 직후 3.955 V → 5분 후 3.918 V → 3.897 V (부하 없이 계속 하강)

배터리는 충·방전 직후 전압이 바로 멈추지 않는다. 이 구간이 바로 **전압 회복(Relaxation) 미완료** 상태이며,
이 로그의 **시작 시점**도 같은 상태였기 때문에 부팅 OCV 초기값이 −9.6 %p 틀어졌다 (R1 §1 참조).

### 1-3. 배터리 이상이 아님을 보이는 근거

| 신호 | 값 | 판정 |
|---|---|---|
| 셀 전압 편차 `BSA_Cell_DeviV` | 23 mV | 정상 (경고 임계 200 mV) |
| 셀 온도 편차 `BSA_Cell_DeviT` | 1.4 ℃ | 정상 |
| 셀 Max / Min | 3.91 / 3.89 V | 정상 |
| **점프 순간 전류** | **0.00 A** | **전류적산으로 SOC 변화 설명 불가 → 초기값 재산출** |

### 1-4. 점프 메커니즘 요약 (R1 결론 재확인)

```
부팅(로그 시작) : Relaxation 미수렴 전압으로 OCV 초기화 → 65.1 % (참값 74.7 %, −9.6 %p)
109분 운전      : 전류적산 정상 — 그러나 출발점 오차 −9.6 %p 그대로 유지
재기동          : 이번엔 346 s 휴지 후라 전압 회복 완료 → OCV 71.2 % (정답)
                  → 틀린 값이 맞는 값으로 한 번에 보정 = "점프"로 관측
NVR(LastSOC) 복원이 동작했다면 delta 9.8 ≤ 20 → 61.4 % 유지, 점프 없음 (N1 실기로 미동작 확정)
```

---

## 2. SW 변경분(260831, 0.18) 적용 전 검토 — R-1~R-7

대상 : `NVR_Init()` 호출 추가(F-1) · `NVRAM_SelfTest()` 신설(F-3) · 메타 36 byte 확장(F-5/F-7) ·
읽기 유효성 복구(F-2) · SPIBRR 복원(F-4) · 운전 중 완만 보정(F-6)

| # | 항목 | 판정 | 요지 |
|---|---|---|---|
| **R-1** | NVR 초기화 호출 시점 | ★ **치명** | `MemCopy()` 이전 호출 — `delay_us()` 가 RAM 미복사 상태에서 실행 |
| **R-2** | SPIBRR 복원값 119 | ★ **치명** | LTC6804 실사용 값은 60 — 119 는 미호출 함수의 값 |
| **R-3** | Product_Version 18 유지 | 주의 | 확장 영역(byte 32~35) 미기록 값 오독 — 첫 부팅 1회 F-5 무효 |
| R-4 | 진단 주소 0x0000E0 | 양호 | 메타(0~35)·로그(0x000100) 미침범, MRAM 쓰기수명 무제한 |
| R-5 | SPI 버스 배타 | 양호 | ISR 내 SPI 접근 없음, `SPI_Sema`·CS 처리 정상 |
| R-6 | SysAhF 미복원 유지 | 양호 | LastSOC 는 적산분 포함 최종값 → 복원 시 이중 가산. 0 유지가 정답 |
| R-7 | 완만 보정 진입 조건 | 검토 | 무부하 1800 s 는 실차에서 성립 희박 — 완화 권장 |

### R-1. `NVR_Init()` / `NVRAM_SelfTest()` 호출이 MemCopy 이전 ★

```c
/* 현재 main.c — 문제 순서 */
InitSpi();                 // :199
NVR_Init();                // :207  ← delay_us() 사용
NVRAM_SelfTest();          // :208  ← delay_us() 사용
...
MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);   // :213 ← ramfuncs 복사가 여기
InitFlash();
```

`delay_us()` → `DSP28x_usDelay` 는 `.sect "ramfuncs"` (F2806x_usDelay.asm:95),
map 상 LOAD 0x3DC44F(Flash) → **RUN 0x8000(RAM)**. 복사 전에 0x8000 으로 점프하면 부정 코드 실행
— 재현성 없는 부팅 불량의 전형.

**수정** — 두 줄을 `InitFlash()` 뒤로 이동:

```c
MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
InitFlash();
NVR_Init();                     /* 260831 : ramfuncs(delay_us) 복사 후로 이동 */
NVRAM_SelfTest();
```

### R-2. SPIBRR 복원값 119 → LTC6804 통신 84 % 느려짐 ★

#### 반영 경로 확인 (NVRAM.c 현재 상태)

```
:512  SpiaRegs.SPIBRR = 50;    ← NVR_SPIWrite 진입
:541  SpiaRegs.SPIBRR = 119;   ← 종료 시 복원 (신규)      ← 문제
:560  SpiaRegs.SPIBRR = 50;    ← NVR_SPIRead 진입
:584  SpiaRegs.SPIBRR = 119;   ← 종료 시 복원 (신규)      ← 문제
```

SPIBRR 설정처는 위 4곳 + `InitSpi()`(=60) 뿐. NVRAM 저장은 100 ms 주기(`SEQTimeTick>100`)로 돌므로
**NVRAM 트랜잭션 종료 시 값이 곧 LTC6804 실사용 값**이 된다.

#### 119 가 잘못된 이유

| 함수 | 위치 | SPIBRR | 호출 여부 |
|---|---|---|---|
| `InitSpi()` | F2806x_Spi.c:72 | **60** | main.c:199 **호출됨** |
| `BAT_InitSPI()` | BAT_LTC6802.c:72 | 119 | **호출처 0건** |

주석의 "InitSpi() 설정값 119(BAT_LTC6802.c:72)" 는 두 함수를 혼동한 것.
`BAT_InitSPI()` 는 실행 경로에 없다.

#### 시간축 — LTC6804 가 겪는 클럭 (LSPCLK 20 MHz, Baud = LSPCLK/(SPIBRR+1))

| 시점 | 변경 전 | 변경 후(현재) | 수정안 |
|---|---|---|---|
| InitSpi 직후 | 60 (328 kHz) | 60 | 60 |
| 첫 NVRAM 저장(≈100 ms) 이후 **영구** | 50 (392 kHz, 미복원 버그 부작용) | **119 (167 kHz)** | **60 (328 kHz)** |

#### 정량 영향 — 셀전압 취득 2 IC (ADCV 1 + RDCVA~D 4/IC, write 90 B + read 64 B, read 는 delay_us(30)/B 고정 포함)

| SPIBRR | 클럭 | 셀전압 2 IC | 온도 3 명령 |
|---|---|---|---|
| 50 (변경 전 실효) | 392 kHz | 5.06 ms | 3.52 ms |
| 60 (설계값) | 328 kHz | 5.68 ms | 3.90 ms |
| **119 (현재)** | **167 kHz** | **9.31 ms (+84 %)** | **6.33 ms** |

셀전압 100 ms / 온도 50 ms 주기 한도 내라 **당장 깨지진 않으나** 여유 축소 + 명백한 성능 후퇴.

**수정 (권장)** — 호출자 설정 보존/원복:

```c
Uint8 NVR_SPIWrite(Uint8 cmd, Uint32 addr, Uint8* buf, Uint16 len)
{
    Uint16 i = 0U;
    Uint16 brr_save;                         /* 260831 : 호출자 설정 보존 */
    ...
    brr_save = SpiaRegs.SPIBRR;              /* 진입 시 저장 */
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;
    SpiaRegs.SPIBRR = 50;
    ...
    NVR_CE_H;
    delay_us(1);
    SpiaRegs.SPIBRR = brr_save;              /* 119 하드코딩 → 원복 */
    SPI_Sema = 0U;
    return 1U;
}
/* NVR_SPIRead() 동일 적용 */
```

간단 대안 : :541 / :584 의 `119` → `60` 고정.
※ 변경 전 실효 392 kHz(=50) 는 미복원 버그의 부작용이지 설계값이 아님 — 60 으로 정상화가 원칙.
  실기 타이밍이 392 kHz 기준으로 검증돼 있었다면 InitSpi 의 60 → 50 통일이 대안.

### R-3. Product_Version 18 유지 → 첫 부팅 1회 F-5 무효

기존 칩의 byte 32~35 (LastCellV/LastAh) 는 미기록 영역. MetaVersion==18 이라 유효 판정을 통과하고,
`LastCellV != 0` 가드도 0xFFFF 는 통과 → dV = |3897−65535| → 5 mV 조건 불성립 → F-5 무효 (구 경로 폴백).

**수정** — `parameter.h:299` `Product_Version 18 → 19`. 첫 부팅에서 무효 마커(F-2 복구분) 경로를 타고
그 부팅부터 새 포맷으로 기록되어 정상화.

### R-7. 완만 보정(F-6) 진입 조건 — 실차 성립 희박

- `C_SocOcvAdjRestCount = 36000`(1800 s) : 실차 로그 109분 내 1800 s 무부하 = **0건** (최장 346 s)
- 전압 수렴 조건이 이미 강한 게이트 : 잔차 68 mV 에서 `ΔV(120s) ≤ 2 mV` 성립에는
  `68·e^(−t/1200)·0.0952 ≤ 2 → t ≥ 1409 s (23.5분)` 필요 → 1800 s 조건은 중복이며 더 엄격
- **권장** : `36000 → 12000`(600 s). 안전성은 전압 수렴 조건이 담보
- 상태 조건(READY 만 허용)은 타당 — 실차 로그 97분이 READY

### 양호 항목 상세

- **R-4** : `NVR_SELFTEST_ADDR 0x0000E0` + 16 B = 0xF0 ≤ 0xFF (메타 영역 256 B 내 여유), 로그 영역(0x000100) 미침범
- **R-5** : `cpu_timer0_isr` 에 SPI 접근 없음 → NVRAM–LTC6804 인터리브 불가. CLKPOLARITY 는 양쪽 다 1(Mode 3)
- **R-6** : `NVRZoneAWRRegs.LastSOC = Bat80VSOCF×10` 는 `SysSocInitF + SysAhF×Norm` 이 이미 합산된 값.
  부팅 시 `SysAhF` 까지 복원하면 이중 가산 → **SysAhF=0 유지 판단이 정확** (LastAh 는 이력 기록용으로만)

---

## 3. 적용 순서 및 재검증

| 순서 | 작업 | 확인 |
|---|---|---|
| 1 | R-1 수정 — `NVR_Init()`/`SelfTest()` 를 `InitFlash()` 뒤로 | 빌드 후 정상 부팅 반복 10회 |
| 2 | R-2 수정 — brr_save 원복 (또는 119→60) | CCS `SpiaRegs.SPIBRR` = 60 유지 확인 |
| 3 | R-3 수정 — `Product_Version 19` | 첫 부팅 무효 마커 경로 → 2회차부터 NVR 정상 |
| 4 | 부팅 진단 확인 | `NVRAllRegs.SRStatus` BP(bit3:2)=00 / `NvrOk`=1 |
| 5 | **BAT IC 회귀** | 0x605/0x606 통계 갱신 주기 불변, 0x608 `Slave1/2 ErrorCount` 증가 없음 |
| 6 | **모사장치 N1 재시험** | 재기동 SOC 40.0 % → PASS 기대 |
| 7 | N2(짧은 정전) → N4(휴지시간별) → N3(delta 경계) | R1 §5 판정표 |

---

## 4. 참조

| 구분 | 위치 |
|---|---|
| 선행 분석 | `SOC_FailR1.md` (동일 폴더) |
| 실차 로그 | `00 설계검토\07 SW 보고서\20260831_BSA_Data_AI.blf` + `20260831_BSA_P9_BSA1-8_R11_AI.dbc` |
| 그래프 | TSMaster BATSYS_Infro 패널 (BSA_Data 재생) |
| 모사장치 | `00 설계검토\11 P56 모사장치\03_GUI\PackBMS_SOC_Simulator_Qt.py` V3.13 (N1~N4) |
| 고객 보고 | `00 설계검토\100 진도보고_건\20260831_SOC재기동점프_원인규명및개선_AI.pptx` |
| 검토 대상 코드 | `main.c:195~215`(초기화 순서) / `NVRAM.c:505~590`(SPI) / `BATAlgorithm.c`(F-6) / `parameter.h:299` |

---

## 5. 개정 이력

| Rev | 일자 | 내용 |
|---|---|---|
| R1 (SOC_FailR1.md) | 2026-08-31 | N1 실기 FAIL — NVR 미동작 확정, 결함 F-1~F-7, 수정안 A/B/C |
| **R3** | 2026-08-31 | 그래프(BATSYS_Infro) 판독 추가. SW 변경분(0.18) 검토 R-1~R-7 — 치명 2건(R-1 호출 순서, R-2 SPIBRR 119→60), 주의 1건(R-3 버전), 완화 권고 1건(R-7) |
