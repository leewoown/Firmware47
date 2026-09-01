# SOC 재기동 점프 — 원인 확정 및 수정사항

> **R1 / 2026-08-31** | 작성 이우원 사업본부장
> 대상 펌웨어 : `D:\47 HDFC10kWNCM_PackBMSR1\F28069PackBMS`
> 근거 : P9 실차 로그 `BSA_Data.csv`(109분) + 모사장치 V3.13 N1 실기 시험(260831)

---

## 1. 결론 요약

**부팅 시 NVR(MRAM `LastSOC`) 복원이 동작하지 않아, 매 재기동마다 OCV로 SOC를 재초기화한다.**
OCV 초기화 자체는 정확하나, 부팅 시점 셀 전압이 Relaxation 미수렴 상태면 그 오차가 그대로 초기값이 되고
운전 중 재보정 경로가 없어 오차가 영구 유지된다. 재기동 시 그 누적 오차가 한 번에 드러난 것이 "SOC 점프"다.

| 구분 | 판정 | 근거 |
|---|---|---|
| NVR 저장·복원 | **미동작 (확정)** | N1 실기 — 재기동 SOC 가 적산값 40.0 % 가 아닌 OCV 49.8 % |
| 전류 적산 | 정상 | 159 s × −100 A = 4.417 Ah = 9.81 %p ↔ 관측 9.8 %p |
| OCV 변환 | 정상 | 셀 3.713 V → 50.0 % (펌웨어 49.8 %, 차 0.2 %p) |
| 운전 중 재보정 | 없음 | `CalMeth=1` 고정, OCV 재보정 경로 부재 |

---

## 2. N1 실기 시험 결과 (2026-08-31)

시험 조건 : 셀 전압 **3.713 V 고정**(SOC 50 %) 상태에서 **−100 A** 적산으로 BMS SOC 만 40 % 로 이동 → 전원 OFF/ON

| # | 스냅샷 | 경과[s] | BMS SOC[%] | 모델 SOC[%] | 셀전압[V] | OCV환산[%] | 비고 |
|---|---|---|---|---|---|---|---|
| 1 | `S_ocv` | 20.0 | 49.8 | 59.9 | 3.713 | 50.0 | 기준점 |
| 2 | `S_before` | 179.0 | **40.0** | 50.2 | 3.713 | 50.0 | 적산 −9.8 %p 완료 |
| 3 | `S_stab` | 189.0 | **40.0** | 50.2 | 3.713 | 50.0 | **차단 직전** |
| 4 | `S_boot` | 201.9 | **49.8** | 50.2 | 3.713 | 50.0 | **OFF 9.1 s 후 부팅** |
| 5 | `S_after` | 216.9 | 49.8 | 50.2 | 3.713 | 50.0 | 부팅 15 s 후 |

### 판정 — **FAIL**

```
적산 SOC 40.0 %  /  OCV 환산 50.0 %   →  delta = 10.0 %p
펌웨어 판정식 (main.c:485)
    if (NVRSocInitF < 0.0F) || (delta > 20.0F)  →  SysSocInitF = SOCbufF   (OCV)
    else                                        →  SysSocInitF = NVRSocInitF (NVR)

delta 10.0 ≤ 20.0  →  NVR(40.0 %) 이 채택됐어야 함
실제 재기동 SOC 49.8 %  ≈ OCV 50.0 %   →  NVR 미채택
```

재기동값이 **정확히 OCV** 라는 것은 `delta > 20` 이 아니라 **`NVRSocInitF < 0`(무효 마커) 경로**를 탔음을 뜻한다.
즉 `main.c:438~441` 의 무효 조건 중 하나가 성립하고 있다.

```c
/* main.c:438 — 이 중 하나라도 걸리면 NVRSocInitF = -1.0F (무효) → OCV 강제 */
if((NVRZoneARDRegs.LastSOC   <  0)      ||   /* 음수 (0xFFFF 포함) */
   (NVRZoneARDRegs.LastSOC   >  1000)   ||   /* 100 % 초과        */
   (NVRZoneARDRegs.MetaVersion == 0xFFFF)||  /* 전부 1 = 칩 무응답 */
   (NVRZoneARDRegs.MetaVersion == 0))        /* 전부 0 = MISO Low  */
```

### 실차 로그와의 일치

| 항목 | P9 실차 로그 | N1 실기 |
|---|---|---|
| 재기동 초기값 | OCV 계산값과 ±0.04 %p 일치 (2회 모두) | OCV 계산값과 0.2 %p 일치 |
| delta | 9.8 %p (≤ 20) → NVR 채택 기대 | 10.0 %p (≤ 20) → NVR 채택 기대 |
| 실제 채택 | OCV | OCV |

동일 현상이 벤치에서 100 % 재현되었다.

---

## 3. 코드 결함 (수정 대상)

### ★ F-1 `NVR_Init()` / `NVR_UnLock()` 호출처 0건 — 최우선

| 항목 | 내용 |
|---|---|
| 위치 | 정의 `NVRAM.c:386, 391` / 선언 `NVRAM.h:190~192` |
| 문제 | 전체 소스에서 **호출처 0건**. `main()` 초기화부(`main.c:164~205`)에 `InitSpi()` 는 있으나 `NVR_Init()` 없음 |
| 영향 | MR25H40(Everspin MRAM) 상태레지스터의 **BP1:BP0(bit3:2)는 비휘발성**. 보호가 걸려 있으면 `WRITE` 가 전부 무시되어 `LastSOC` 가 영원히 갱신되지 않음 |
| 결과 | 부팅마다 stale/무효 값 → OCV 재초기화 |

### F-2 읽기 유효성 검사 전체 주석 처리

| 항목 | 내용 |
|---|---|
| 위치 | `NVRAM.c:250~275` |
| 문제 | `MetaVersion != Product_Version` 시 복구 블록이 `/* */` 안에 있음 → 읽기 실패를 감지하지 못하고 원시 바이트가 그대로 상위로 전달 |
| 추가 위험 | 주석 안의 복구값이 `LastSOC = 300`(=30.0 %) — **가짜 유효값**이라 주석을 그대로 해제하면 안 됨 |

### F-3 자체 진단 함수 무력화

| 위치 | 문제 |
|---|---|
| `NVRAM.c:62~63` | `for` 루프 본문(`txBuf[i] = i+1`)이 주석 → **루프 본문이 :66 WREN 이 됨**(WREN 16회), `txBuf` 는 전부 0 |
| `NVRAM.c:71` | `TestState = 2;` 주석 → 상태 전이 없음, WRITE 만 무한 반복 |
| `NVRAM.c:85~92` | 비교 루프 주석 → 검증 자체가 없음 |
| — | `NVRAM_StateTest()` **호출처도 0건** |

### F-4 SPI 설정 미복원 (부수)

| 위치 | 문제 |
|---|---|
| `NVRAM.c:430, 472` | `SPIBRR = 50` 으로 변경 후 LTC6804 용 **60(`InitSpi`)으로 복원하지 않음** |
| 동상 | `SPICCR` 를 `SPISWRESET = 0` 없이 변경 (TI 권장 절차 위배) |

> SPI 모드 자체는 문제없음. `CLKPOLARITY=1 / CLK_PHASE=0` = SPI Mode 3 으로, MR25H40·LTC6804 모두와 호환된다.

### F-5 부팅 OCV 초기화에 휴지 판정 없음

| 위치 | 문제 |
|---|---|
| `BATAlgorithm.c:131` `CellP56AhSocInit()` | 부팅 시 셀 평균 전압을 **곧바로 OCV 로 간주**. 휴지시간·직전 전류 이력 검사 없음 |
| 실차 영향 | 부팅 시점 −68 mV 눌림 → 초기 SOC **−9.6 %p** 오차 |

### F-6 운전 중 재보정 경로 없음

| 위치 | 문제 |
|---|---|
| `BATAlgorithm.c:322` `CalSocHandle()` | `CalMeth = 1` 고정. 무부하가 아무리 길어도 OCV 재보정 없음 → 초기 오차가 영구 유지 |

### F-7 적산분 무조건 폐기

| 위치 | 문제 |
|---|---|
| `main.c:519~521` | 부팅 시 `SysAhF = SysAhOldF = SysAhNewF = 0.0F`. NVR 유효 채택 시에도 누적 Ah 를 버려 추가 단차 발생(실차 −1.6 Ah = 3.6 %p) |

---

## 4. 수정사항

### A. 원인 제거 — 즉시 (NVR 복구)

#### A-1 `NVR_Init()` 호출 추가 ★

```c
/* main.c — 초기화부, InitSpi() 직후 */
    InitSpi();
    NVR_Init();          /* 260831 추가 : MR25H40 쓰기보호(BP) 해제 — 미호출 시 WRITE 무시됨 */
    InitECanGpio();
```

#### A-2 해제 결과 검증 (RDSR)

```c
/* NVRAM.c — NVR_UnLock() 말미에 추가 */
void NVR_UnLock(void)
{
    Uint8 buf[10];
    Uint8 sr = 0U;
    NVR_SPIWrite(NVR_WREN_CMD, NVR_NO_ADDR, NULL, 0);
    buf[0] = 0x02;                       /* SRWD=0, BP1:BP0=00 → 전 영역 해제 */
    NVR_SPIWrite(NVR_WRSR_CMD, NVR_NO_ADDR, buf, 1);

    /* 260831 추가 : 해제 확인 — 진단값을 보존해 부팅 로그/CAN 으로 확인 */
    NVR_SPIRead(NVR_RDSR_CMD, NVR_NO_ADDR, &sr, 1);
    NVRAllRegs.SRStatus = (Uint16)sr;
    /*  0xFF            : 칩 무응답 (미실장·CS(GPIO11)·전원)
     *  0x00            : MISO Low 고정 (배선)
     *  (sr & 0x0C)!=0  : BP 잔존 → 쓰기보호 해제 실패
     *  정상            : BP=00, WREN 직후 WEL(bit1)=1                       */
}
```

#### A-3 읽기 유효성 검사 복구 — **무효 마커 방식으로**

```c
/* NVRAM.c:250~275 주석 블록을 아래로 교체 */
    if (p->MetaVersion != (Uint16)Product_Version)
    {
        /* 260831 : 복구값을 30 % 로 채우면 '가짜 유효값' 이 되어 위험하다.
           무효 마커(-1)로 표시해 main.c 가 OCV 로 폴백하도록 한다. */
        p->LastSOC = (int16)-1;
        p->MetaCRC = 0u;
    }
```

#### A-4 부팅 1회 NVR 자체 진단 (write→read 왕복)

```c
/* NVRAM.c : NVRAM_StateTest() 를 아래로 정정 (for 본문·상태전이·비교 복구) */
void NVRAM_SelfTest(void)                 /* 부팅 1회 호출 */
{
    Uint16 k;
    for(k = 0; k < TEST_LEN; k++) { txBuf[k] = (Uint8)(k + 1U); }   /* ← 본문 복구 */
    NVR_SPIWrite(NVR_WREN_CMD,  NVR_NO_ADDR, NULL,   0);
    NVR_SPIWrite(NVR_WRITE_CMD, TEST_ADDR,   txBuf,  TEST_LEN);
    memset(rxBuf, 0, TEST_LEN);
    NVR_SPIRead (NVR_READ_CMD,  TEST_ADDR,   rxBuf,  TEST_LEN);
    TestError = 0U;
    for(k = 0; k < TEST_LEN; k++)
    {
        if(txBuf[k] != rxBuf[k]) { TestError = 1U; break; }         /* ← 비교 복구 */
    }
    NVRAllRegs.NvrOk = (TestError == 0U) ? 1U : 0U;
}
```

> `TEST_ADDR = 0x000100` 은 `NVR_LOG_BASE_ADDR` 와 동일하다. 로그 영역 첫 레코드를 덮으므로
> 진단 전용 주소(예 `0x0000E0`, 메타 영역 256 B 내 여유)로 옮길 것.

#### A-5 SPI 설정 복원

```c
/* NVRAM.c — NVR_SPIWrite() / NVR_SPIRead() 종료 직전 */
    NVR_CE_H;
    delay_us(1);
    SpiaRegs.SPIBRR = 60;      /* 260831 추가 : InitSpi() 기본값 복원 (LTC6804 통신 영향 차단) */
    SPI_Sema = 0U;
```

#### A-6 진단값 CAN 송출 (선택, 확인용)

`0x609`·`0x60A` 는 셀전압·셀온도 순환 전송에 사용 중이므로 **`0x608`(BSA9) 여유 바이트**를 활용한다.

| 바이트 | 신호 | 정상값 |
|---|---|---|
| D0 | `MetaVersion` | **18** (`Product_Version`) |
| D1 | `LastSOC` | 차단 직전 SOC × 10 |
| D2.L | `SRStatus` | BP=00, 0xFF/0x00 아님 |
| D2.H | `NvrOk` | 1 |
| D3 | `SysTimeTick` 하위 16bit | 재기동마다 **증가** |

### B. 재발 방지 — 알고리즘 개선

#### B-1 차단 직전 셀 전압 저장·비교 ★ 즉시 효과

MRAM A영역은 32 B 만 사용하고 **224 B 가 비어 있다**(`NVR_META_SIZE_BYTES = 256`).

```c
/* NVRAM.h : NVRZoneAReg 에 추가 */
    Uint16 LastCellV;          /* [mV] 마지막 저장 시점 셀 평균전압 */

/* main.c:804 부근 (NVRAM_AZoneSave) */
    NVRZoneAWRRegs.LastSOC   = (int16)(SysRegs.Bat80VSOCF * 10.0F);
    NVRZoneAWRRegs.LastCellV = (Uint16)(SysRegs.Bat80VCellAgvVoltageF * 1000.0F);

/* main.c INIT — CellP56AhSocInit() 앞 */
    dV = fabs(SysRegs.Bat80VCellAgvVoltageF * 1000.0F
              - (float32)NVRZoneARDRegs.LastCellV);
    /* dV ≤ 5 mV : 껐다 켠 사이 상태 변화 없음 → NVR 우선 (+ SysAhF 도 복원) */
```

| 판정 | 조치 |
|---|---|
| `dV ≤ 5 mV` | 짧은 정전 → **NVR(LastSOC) 채택** + `SysAhF` 복원 |
| `dV > 5 mV` | 변화 있음 → 기존 delta 판정 경로 |

**실차 사고 대입** : 차단 직전 3.897 V, 재기동 3.897 V → `dV = 0 mV` → NVR 채택 → **점프 없음**.
휴지시간을 재지 않고 "상태가 안 변했으니 이전 값을 믿는다"로 우회하므로 **RTC·HW 추가가 불필요**하다.

#### B-2 운전 중 완만 보정 (점프 → 램프)

**진입 조건 (전부 AND)**

| 조건 | 값 | 근거 |
|---|---|---|
| 무부하 지속 | `|I| < 0.5 A` 가 **1800 s** 이상 | τ₃ 1200 s × 1.5 |
| 전압 수렴 | `|ΔV(120 s)| ≤ 2 mV` | 잔차 21 mV 이하 |
| 셀 편차 | `CellVltDv < 50 mV` | 특정 셀 이상 배제 |
| 온도 | 10 ~ 40 ℃ | OCV 테이블 25 ℃ 기준 |
| 상태 | STANDBY / READY | RUN 중 대전류 진입 방지 |

**동작**

```
SOC_ocv = OCVtoSOC_P56Ah(CellAgvVoltageF)
err     = SOC_ocv - SysSOCF

|err| ≤ 1.0 %p   → 보정 없음                     (데드밴드 — 헌팅 방지)
|err| >  1.0 %p   → SysSocInitF 를 err 방향으로 램프
                    램프율 0.01 %p/s
                    |err| ≤ 0.5 %p 에서 정지      (히스테리시스)
1회 무부하 세션당 최대 5.0 %p, 초과 시 경고 플래그 후 중단
```

**수치 근거**

| 항목 | 값 |
|---|---|
| 램프율 0.01 %p/s 의 전류 환산 | 45 Ah × 0.01 % = **16 mA 상당** — 전류 노이즈 수준, 사용자 인지 불가 |
| 실차 오차 9.6 %p 흡수 시간 | 960 s (16분) — 무부하 30분 조건 내 |
| 1회 상한 5 %p | OCV 오판 시 피해 봉쇄. 반복 세션으로 분할 수렴 |
| 데드밴드 1.0 %p | 0x601 SOC 양자화(0.1 %p)의 10배 — 표시 떨림 방지 |
| OCV 기울기 (3.90 V 부근) | 1 mV ≈ **0.148 %p**(DoD80) |

#### B-3 적산분 이월

```c
/* main.c:519 — NVR 유효 채택 시에는 SysAhF 도 복원 */
if (nvr_adopted) { Farasis56AhSocRegs.SysAhF = (float32)NVRZoneARDRegs.LastAh / 10.0F; }
else             { Farasis56AhSocRegs.SysAhF = 0.0F; }
```

#### 개선안 비교

| 안 | 추가 HW | 코드 규모 | 이번 사고 차단 | 누적 오차 해소 |
|---|---|---|---|---|
| A (NVR 복구) | 없음 | 약 30줄 | **○ 완전** | ✕ |
| B-1 (LastCellV) | 없음 | 약 20줄 | **○ 완전** | ✕ |
| B-2 (완만 보정) | 없음 | 약 80줄 | ✕ | **○** |

> **A + B-1 즉시 적용, B-2 근본 해소** 를 권장한다. A 가 복구되지 않으면 B-1·B-3 도 동작하지 않는다(같은 MRAM 사용).

### C. 모사장치 수정 (V3.14 예정)

| # | 항목 | 사유 |
|---|---|---|
| C-1 | **시험 시작 시 모델 SOC 를 BMS SOC 에 정렬** | N1 실기에서 `S_ocv` 시점 모델 59.9 % ↔ BMS 49.8 % 로 **10.1 %p 불일치**(이전 시험 잔류값). N1 은 BMS SOC 기준이라 판정 무관하나, **N4 는 모델을 참값으로 쓰므로 정렬 필수** |
| C-2 | OFF 최소 유지시간 가드 | 안내는 "10 초 이상"인데 실측 **9.1 s** 로 진행됨. 지정 시간 미달 시 경고 표시 |
| C-3 | 판정 문구에 무효/stale 구분 추가 | 재기동값이 OCV 와 일치하면 `NVRSocInitF < 0`(무효) 경로임을 명시 |

---

## 5. 수정 후 재검증 계획

| 순서 | 시험 | 기대 결과 |
|---|---|---|
| 1 | **N1** (OFF 10 s 이상) | 재기동 SOC 40.0 % → **PASS** |
| 2 | **N2** (OFF 3~5 s) | 재기동 SOC 40.0 % → PASS |
| 3 | **N3** delta 경계 | 19.5 %p → NVR 채택 / 20.5 %p → OCV 채택 |
| 4 | **N4** 휴지시간별 | 전 구간 오차 ≤ 2 %p (A 적용 시) |
| 5 | 0x608 진단 프레임 | MetaVersion 18 / SysTimeTick 재기동마다 증가 / NvrOk 1 |

**N4 참고 — 드라이런 예측 (NVR 미동작 상태에서 측정 시)**

| 휴지시간 | 0 s | 60 s | 300 s | 1800 s |
|---|---|---|---|---|
| 초기화 오차 | **−7.1 %p** | −2.7 %p | −0.6 %p | −0.1 %p |

휴지가 짧을수록 오차가 커지는 곡선이 확인되면 **Relaxation 미수렴 전압을 OCV 로 사용**한 것이 실측 입증된다.

---

## 6. 참조

| 구분 | 위치 |
|---|---|
| 실차 로그 | `00 설계검토\07 SW 보고서\20260831_BSA_Data_AI.blf` + `20260831_BSA_P9_BSA1-8_R11_AI.dbc` |
| 모사장치 | `00 설계검토\11 P56 모사장치\03_GUI\PackBMS_SOC_Simulator_Qt.py` **V3.13** |
| N1 트레이스 | `00 설계검토\11 P56 모사장치\05_Trace\NVR_N1_*.csv` |
| 펌웨어 | `D:\47 HDFC10kWNCM_PackBMSR1\F28069PackBMS` |
| 관련 코드 | `main.c:426~523`(부팅 초기화) / `main.c:768~856`(NVR 상태머신) / `NVRAM.c` 전체 / `BATAlgorithm.c:105~380` |

---

## 7. 개정 이력

| Rev | 일자 | 내용 |
|---|---|---|
| R1 | 2026-08-31 | 최초 작성 — N1 실기 FAIL 로 NVR 미동작 확정. 코드 결함 F-1~F-7, 수정사항 A/B/C 정리 |
