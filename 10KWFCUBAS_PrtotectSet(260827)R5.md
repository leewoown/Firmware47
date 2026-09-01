---
name: 10KWFCUBAS_PrtotectSet(260827)R5
title: 배터리 보호 설정값 — 10kWFC NCM 22S1P Pack BMS
revision: 260827 R5  (직전: 260827 R4)
source_xlsx: F:/2026_Project/01 현대자동차_10kWFC_2차년도/00 설계검토/04 보호설정/10KWFCUBAS_PrtotectSet(260827)R5.xlsx
firmware: SysInclude/parameter.h  (Product_Version 16, 260810 반영)
can_id: 0x603 (BSA4) — Alarm bit0~15 / Protection bit16~39
items: 37  (Warning 15 / Fault 22)
generated: 2026-08-26
note: 엑셀이 master, 본 MD는 파생본. 값 수정은 엑셀에서 하고 MD를 다시 생성할 것.
---

# 배터리 보호 설정값 — 260827 R5

> 좌측이 **설정표(master)**, 우측이 **펌웨어 현행값**. 굵게 표시된 펌웨어 값은 설정표와 다릅니다.  
> 판정: `일치` / `반올림`(기능 영향 없음) / `값 불일치` / `Delay 불일치` / `미구현` / `2nd 이월` / `예약`

## R4 → R5 변경 (1건)

| 구분 | 항목 | R4 | R5 | 비고 |
|:--|:--|:--|:--|:--|
| 값 변경 | `Bsa_FltCellUnbalTmp` (셀 온도편차 폴트) | 10 ℃ | **20 ℃** | 경고(No.14) 임계 10 ℃ 와 중복 해소 — 폴트 임계 상향 **확정** |

이 변경으로 **펌웨어 `C_CellTempDIV_Fault` 20.0 과 일치**하게 되었습니다 (260810 펌웨어 선행 변경분을 설정표가 따라옴).  
경고 10 ℃ → 폴트 20 ℃ 로 분리되어, 경고 발생 후 차단까지의 대응 구간이 확보되었습니다.

**항목 수 37 유지** (Warning 15 / Fault 22). 폴트 항목 번호(No.16~34)와 0x603 비트 번호 일치(오프셋 0) 유지.

---

## 1. 판정 규칙

| 항목 | 내용 |
|:--|:--|
| 발생 판정 | `Hyst_On(V,S)  ((V) > (S))` — **초과**. 설정표는 "이상/이하" 표기이나 구현은 strict |
| 해제 판정 | `Hyst_Off(V,R) ((V) < (R))` — **미만**. 하한계 항목은 부호 반전 |
| 시간 기준 | main loop 1 ms (`ConfigCpuTimer` PRD = 80400 @ 80 MHz). Delay 100 = 100 ms |
| 해제 지연 | 전 항목 0 (즉시). `_WarnDelay` 만 존재하고 해제 카운터 없음 |
| 히스 산출 | Release = Trigger x (1 - Rate). 하한계 항목은 (1 + Rate) |
| 전류 부호 | **(+) = 충전 / (-) = 방전**. 과전류는 절대값 판정 |
| Warning | `BPA_Protect_Status = 0x03`, Rly = P·N On 유지, Warning flag 전송 |
| Fault | `BPA_Protect_Status = 0x04`, Rly = OPEN, 전원 OFF / `FCU_RESET=1` 전까지 latch |

---

## 2. Warning (경고) — 15 항목 · 0x603 Alarm bit0~14

| No | 항목 | Trigger | 단위 | 조건 | Delay | Hyst | Release | 조건 | 0x603 | parameter.h | 펌웨어 현행값 | 판정 |
|:--:|:--|--:|:--:|:--:|--:|--:|--:|:--:|:--:|:--|:--|:--:|
| 1 | `Bsa_WrnOC` | 450 | [A] | 이상 | 100 | 0.1 | 405 | 미만 | bit 0 | `C_PackCTOV_Warn / _WarnRst / _WarnDelay` | 450.0 / 405.0 / 100 | **일치** |
| 2 | `Bsa_WrnSocH` | 95 | [%] | 이상 | 100 | 0.03 | 92.15 | 미만 | bit 1 | `C_PackSOCOV_Warn / _WarnRst / _WarnDelay` | 95.0 / **92.2** / 100 | **반올림** |
| 3 | `Bsa_WrnSocL` | 5 | [%] | 이하 | 100 | 0.05 | 5.25 | 초과 | bit 2 | `C_PackSOCUN_Warn / _WarnRst / _WarnDelay` | 5.0 / 5.25 / 100 | **일치** |
| 4 | `Bsa_WrnOv` | 90.86 | [V] | 이상 | 100 | 0.03 | 88.1342 | 미만 | bit 3 | `C_PackVoltOV_Warn / _WarnRst / _WarnDelay` | **90.9** / **88.1** / 100 | **반올림** |
| 5 | `Bsa_WrnUv` | 66 | [V] | 이하 | 100 | 0.05 | 69.3 | 초과 | bit 4 | `C_PackVoltUN_Warn / _WarnRst / _WarnDelay` | 66.0 / 69.3 / 100 | **일치** |
| 6 | `Bsa_WrnOt` | 47 | [ºC] | 이상 | 100 | 0.05 | 44.65 | 미만 | bit 5 | `C_PackTempOV_Warn / _WarnRst / _WarnDelay` | 47.0 / **44.7** / 100 | **반올림** |
| 7 | `Bsa_WrnUt` | -20 | [ºC] | 이하 | 100 | 0.05 | -19 | 초과 | bit 6 | `C_PackTempUN_Warn / _WarnRst / _WarnDelay` | -20.0 / -19.0 / 100 | **일치** |
| 8 | `Bsa_WrnUnbalPwr` | — | — | — | — | — | — | — | bit 7 | `(없음)` | 로직 없음 | **미구현** |
| 9 | `Bsa_WrnCellOv` | 4.15 | [V] | 이상 | 100 | 0.005 | 4.12925 | 미만 | bit 8 | `C_CellVoltOV_Warn / _WarnRst / _WarnDelay` | 4.15 / **4.129** / 100 | **반올림** |
| 10 | `Bsa_WrnCellUv` | 3 | [V] | 이하 | 100 | 0.005 | 3.015 | 초과 | bit 9 | `C_CellVoltUN_Warn / _WarnRst / _WarnDelay` | 3.00 / 3.015 / 100 | **일치** |
| 11 | `Bsa_WrnCellUnbalV` | 200 | [mV] | 이상 | 100 | 0.66667 | 66.66667 | 미만 | bit 10 | `C_CellVoltDIV_Warn / _WarnRst / _WarnDelay` | 0.2 / **0.067** / 100 | **반올림** |
| 12 | `Bsa_WrnCellOt` | 55 | [ºC] | 이상 | 100 | 0.05 | 52.25 | 미만 | bit 11 | `C_CellTempOV_Warn / _WarnRst / _WarnDelay` | 55.0 / **52.3** / 100 | **반올림** |
| 13 | `Bsa_WrnCellUt` | -25 | [ºC] | 이하 | 100 | 0.05 | -23.75 | 초과 | bit 12 | `C_CellTempUN_Warn / _WarnRst / _WarnDelay` | -25.0 / **-23.8** / 100 | **반올림** |
| 14 | `Bsa_WrnCellUnbalTmp` | 10 | [ºC] | 이상 | 100 | 0.5 | 5 | 미만 | bit 13 | `C_CellTempDIV_Warn / _WarnRst / _WarnDelay` | 10.0 / 5.0 / 100 | **일치** |
| 15 | `BPA_WrnVcuCan_Err` | 10 | CONT | 이상 (1sec 이상 미수신) | 즉시 = 0 | - | 0 | 이하 (1sec 이상 정상 수신) | bit 14 | `C_VcuCommLostCount` | 10u  (100ms x 10 = 1s) | **일치** |

---

## 3. Fault (보호/차단) — 22 항목 · 0x603 Protection bit16~34

| No | 항목 | Trigger | 단위 | 조건 | Delay | 0x603 | parameter.h | 펌웨어 현행값 | 판정 |
|:--:|:--|--:|:--:|:--:|--:|:--:|:--|:--|:--:|
| 16 | `Bsa_FltOc` | 505 | [A] | 이상 | 즉시 = 0 | bit 16 | `C_PackCTOV_Fault / _FaultDelay` | 505.0 / **1** | **Delay 불일치** |
| 17 | `Bsa_FltctSocH` | 100 | [%] | 이상 | 즉시 = 0 | bit 17 | `C_PackSOCOV_Fault / _FaultDelay` | 100.0 / 0 | **일치** |
| 18 | `Bsa_FlttctSocL` | 0 | [%] | 이하 | 즉시 = 0 | bit 18 | `C_PackSOCUN_Fault / _FaultDelay` | 0.0 / 0 | **일치** |
| 19 | `Bsa_FltOv` | 91.3 | [V] | 이상 | 즉시 = 0 | bit 19 | `C_PackVoltOV_Fault / _FaultDelay` | 91.3 / **2000** | **Delay 불일치** |
| 20 | `Bsa_FltUv` | 58.3 | [V] | 이하 | 즉시 = 0 | bit 20 | `C_PackVoltUN_Fault / _FaultDelay` | **62.7** / **2000** | **값·Delay 불일치** |
| 21 | `Bsa_FltOt` | 52 | [ºC] | 이상 | 즉시 = 0 | bit 21 | `C_PackTempOV_Fault / _FaultDelay` | 52.0 / 0 | **일치** |
| 22 | `Bsa_FltUt` | -35 | [ºC] | 이하 | 즉시 = 0 | bit 22 | `C_PackTempUN_Fault / _FaultDelay` | -35.0 / 0 | **일치** |
| 23 | `Bsa_FltUnbalPwr` | — | — | — | 즉시 = 0 | bit 23 | `(없음)` | 로직 없음 | **미구현** |
| 24 | `Bsa_FltCellOv` | 4.2 | [V] | 이상 | 즉시 = 0 | bit 24 | `C_CellVoltOV_Fault / _FaultDelay` | 4.20 / **2000** | **Delay 불일치** |
| 25 | `Bsa_FltCellUv` | 2.65 | [V] | 이하 | 즉시 = 0 | bit 25 | `C_CellVoltUN_Fault / _FaultDelay` | **2.85** / **2000** | **값·Delay 불일치** |
| 26 | `Bsa_FltCellUnbalVlt` | 350 | [mV] | 이상 | 즉시 = 0 | bit 26 | `C_CellVoltDIV_Fault / _FaultDelay` | 0.35 / **5000** | **Delay 불일치** |
| 27 | `Bsa_FltCellOt` | 60 | [ºC] | 이상 | 즉시 = 0 | bit 27 | `C_CellTempOV_Fault / _FaultDelay` | 60.0 / 0 | **일치** |
| 28 | `Bsa_FltCellUt` | -30 | [ºC] | 이하 | 즉시 = 0 | bit 28 | `C_CellTempUN_Fault / _FaultDelay` | -30.0 / 0 | **일치** |
| 29 | `Bsa_FltCellUnbalTmp` | 20 | [ºC] | 이상 | 즉시 = 0 | bit 29 | `C_CellTempDIV_Fault / _FaultDelay` | 20.0 / **5000** | **Delay 불일치** |
| 30 | `Bsa_FltRly_Err` | — | — | — | — | bit 30 | `C_RleyCount` | 1  (FaultReg 미설정 — 차단 로직 없음) | **미구현** |
| 31 | `BSA_Prtct_FAULT15` | — | — | — | — | bit 31 | `(예약)` | — | **예약** |
| 32 | `Bsa_FltCellIR_OV` | 15 | mΩ | 이상 | 즉시 = 0 | bit 32 | `(없음)` | 로직 없음 | **2nd 이월** |
| 33 | `Bsa_FltOcTimer` | 1 | Sec | 480A 이상 1sec 이상 유지 | 즉시 = 0 | bit 33 | `C_PackOCTimer_Fault / C_PackOCTimerCount` | 480.0 / 1000 | **일치** |
| 34 | `Bsa_FltOcTime_min` | 2 | Count/min | 480A 이상 1분당 2회 발생 시 | 즉시 = 0 | bit 34 | `(없음)` | 로직 없음 | **미구현** |
| 35 | `BPA_FltISOSPI_Err` | 210 | CONT | 이상 | 즉시 = 0 | **없음** | `C_ISOSPICount` | **50** | **값 불일치** |
| 36 | `Bsa_FltRly_Err` | — | — | — | — | **없음** | `C_RleyCount` | 1  (FaultReg 미설정 — 차단 로직 없음) | **미구현** |
| 37 | `Bsa_FltIMD_Err` | — | — | — | — | **없음** | `C_IOSresistanceFault` | 45000  (판정 분리 안 됨) | **2nd 이월** |

> Fault 해제는 전 항목 공통 : **전원 OFF 또는 `FCU_RESET=1` 전까지 FAULT 래치 유지**

---

## 4. 0x603 (BSA4) 비트 배치

```
Alarm  bit 0~15
   0 WrnOC          1 WrnSocH        2 WrnSocL          3 WrnOv
   4 WrnUv          5 WrnOt          6 WrnUt            7 WrnUnbalPwr
   8 WrnCellOv      9 WrnCellUv     10 WrnCellUnbalV   11 WrnCellOt
  12 WrnCellUt     13 WrnCellUnbalTmp                 14 WrnVcuCan_Err
  15 (예약)

Protection  bit 16~39      ※ R4 부터 항목 No = 비트 번호 (오프셋 0)
  16 FltOc         17 FltctSocH     18 FlttctSocL      19 FltOv
  20 FltUv         21 FltOt         22 FltUt           23 FltUnbalPwr
  24 FltCellOv     25 FltCellUv     26 FltCellUnbalVlt 27 FltCellOt
  28 FltCellUt     29 FltCellUnbalTmp                 30 FltRly_Err
  31 (예약 = BSA_Prtct_FAULT15)
  32 FltCellIR_OV  33 FltOcTimer    34 FltOcTime_min
  35~39 (예약)
```

**규약 비트 미할당 3건** — No.35 `BPA_FltISOSPI_Err` · No.36 `Bsa_FltRly_Err`(중복) · No.37 `Bsa_FltIMD_Err`  
→ 발생해도 VCU 통보 수단 없음. R3 의 `Bsa_FltCharCellUt` 는 삭제로 해소.

---

## 5. 펌웨어 대비 불일치

### 5-1. 값 불일치 3건 — 조치 필요

| 항목 | 설정표 R4 | parameter.h | 조치 |
|:--|:--|:--|:--|
| `Bsa_FltUv` | **58.3 V** | 62.7 | R4 변경분 미반영 — `C_PackVoltUN_Fault` 58.3 으로 수정 |
| `Bsa_FltCellUv` | **2.65 V** | 2.85 | R4 변경분 미반영 — `C_CellVoltUN_Fault` 2.65 로 수정 |
| `BPA_FltISOSPI_Err` | 210 CONT | **50** | 기준 확정 후 일치화 |

### 5-2. Delay 불일치 — 설정표 전 항목 「즉시 = 0」 ↔ 펌웨어 지연 존재

| parameter.h | 현재 | 조치 |
|:--|--:|:--|
| `C_PackCTOV_FaultDelay` | 1 ms | → 0 |
| `C_PackVoltOV_FaultDelay` / `C_PackVoltUN_FaultDelay` | 2000 ms | → 0 |
| `C_CellVoltOV_FaultDelay` / `C_CellVoltUN_FaultDelay` | 2000 ms | → 0 |
| `C_CellVoltDIV_FaultDelay` | 5000 ms | → 0 |
| `C_CellTempDIV_FaultDelay` | 5000 ms | → 0 |

### 5-3. 반올림 차이 (기능 영향 없음)

`WrnSocH` 92.15→92.2 ｜ `WrnOv` 90.86→90.9, 88.1342→88.1 ｜ `WrnOt` 44.65→44.7 ｜
`WrnCellOv` 4.12925→4.129 ｜ `WrnCellUnbalV` 66.667→67 mV ｜ `WrnCellOt` 52.25→52.3 ｜ `WrnCellUt` -23.75→-23.8

### 5-4. 미구현 · 이월

| 구분 | 항목 |
|:--|:--|
| 미구현 (2차년도 대상) | `Bsa_WrnUnbalPwr` · `Bsa_FltUnbalPwr` (설정값 미정) ｜ `Bsa_FltOcTime_min` ｜ `Bsa_FltRly_Err` (플래그만 set, FaultReg 미설정) |
| 2nd 이월 | `Bsa_FltCellIR_OV` ｜ `Bsa_FltIMD_Err` (배터리 팩 증가 · Pack BMS 변경 시 반영) |

---

## 6. 설계 검토 사항

| 구분 | 내용 |
|:--|:--|
| **Protect 판정 전면 미동작** | `main.c:921` `// Cal80VSysFaultCheck(&SysRegs);` 주석 처리 → **본 문서 Fault 22 항목 전체가 동작하지 않음**. C13 시험(셀 온도편차 10.1 ℃ 20 s 유지 → Fault 미발생)으로 실측 확인 |
| Fault 레지스터 폭 | `BAT80VFaultReg.all` 이 16-bit 비교라 bit16 이상(2번째 워드) 플래그가 판정·클리어에서 누락 (`main.c:922, 955`) |
| 릴레이 융착 | `DSP28x_Project.c:903-909` 에서 `FaulBuftReg.PackRLY_ERR` 만 set, `FaultReg` 미설정 → 차단 없음. 절연저항 이상도 같은 비트에 오매핑 |
| **팩↔셀 연동 비대칭** | 저전압측은 `FltCellUv` 2.65 V × 22S = `FltUv` 58.3 V 로 정확히 연동. 과전압측은 `FltCellOv` 4.20 V × 22S = 92.4 V 인데 `FltOv` 는 91.3 V (= 4.15 V × 22S) → **팩 과전압 폴트가 셀 과전압 폴트보다 먼저 동작**. 의도 여부 확인 필요 |
| 셀 저전압 폴트 여유 | 2.65 V 는 P56 상온 방전종지 2.8 V 미만, 저온대 하한 2.5 V 초과 구간. R3(2.85 V)의 조기 차단 우려는 해소되었으나 상온 과방전 보호 여유는 축소됨 |
| 항목명 중복 | `Bsa_FltRly_Err` 가 No.30 · No.36 에 중복 등재 — 정리 필요 |
| ✔ 해소 (R5) | 셀 온도편차 경고 10 ℃ = 폴트 10 ℃ 동일 문제 → 폴트 20 ℃ 로 분리 확정. C13 시험에서 제기된 건 |

### 원본 엑셀 반영 필요 2건

| 위치 | 내용 |
|:--|:--|
| No.20 `Bsa_FltUv` 비고 | 「발생 61.6 → **62.7** V」 로 기재되어 있으나 실제 값은 **58.3 V** — 비고 미갱신 |
| 하단 개정 이력 요약표 | 제목이 「260827 **R3**」, 확인 요청 「6건」 으로 남아 있음 — R4 기준(잔여 3건)으로 갱신 필요 |

---

## 7. parameter.h 정렬 패치 (설정표 R4 기준)

```c
/* ── 260827 R5 반영 ── */
/* C_CellTempDIV_Fault 20.0 : R5 에서 설정표가 펌웨어에 맞춰짐 — 수정 불필요 */

/* [1] R4 변경분 — 펌웨어 미반영 */
#define C_PackVoltUN_Fault         58.3   /* 현재 62.7  (= 2.65V x 22S) */
#define C_CellVoltUN_Fault         2.65   /* 현재 2.85                  */

/* [2] Fault Delay — 설정표는 전 항목 즉시(0) */
#define C_PackCTOV_FaultDelay      0      /* 현재 1    */
#define C_PackVoltOV_FaultDelay    0      /* 현재 2000 */
#define C_PackVoltUN_FaultDelay    0      /* 현재 2000 */
#define C_CellVoltOV_FaultDelay    0      /* 현재 2000 */
#define C_CellVoltUN_FaultDelay    0      /* 현재 2000 */
#define C_CellVoltDIV_FaultDelay   0      /* 현재 5000 */
#define C_CellTempDIV_FaultDelay   0      /* 현재 5000 */

/* [3] 기준 확정 후 정렬 */
#define C_ISOSPICount              210    /* 현재 50               */

/* [4] 삭제 — R4 에서 항목 제거됨 */
/* Bsa_FltCharCellUt (저온 충전 금지 15 degC) : 설정표에서 삭제, 구현 불필요 */

/* [5] 최우선 — 주석 해제 없이는 위 설정 전부 무효 */
/* main.c:921   Cal80VSysFaultCheck(&SysRegs);   <- 주석 해제 */

/* [6] 미구현 — 로직 신규 작성 (2차년도) */
/* Bsa_FltOcTime_min    2 [Count/min]  480A 이상 1분당 2회        */
/* Bsa_FltRly_Err                      FaultReg 설정 + 차단 로직   */
/* Bsa_Wrn/FltUnbalPwr                 설정값 미정                 */
```

---

## 8. 기계 판독용 데이터

```json
{
 "revision": "260827R5",
 "source": "10KWFCUBAS_PrtotectSet(260827)R5.xlsx",
 "firmware": "parameter.h Product_Version 16",
 "can_id": "0x603",
 "counts": {
  "warning": 15,
  "fault": 22,
  "total": 37
 },
 "bit_offset_note": "Fault: item No == 0x603 bit number for No.16~34",
 "items": [
  {
   "no": 1,
   "name": "Bsa_WrnOC",
   "class": "warning",
   "trigger": 450.0,
   "unit": "A",
   "trigger_cond": "이상",
   "delay_ms": "100",
   "hysteresis_rate": 0.1,
   "release": 405.0,
   "release_cond": "미만",
   "can_bit": 0,
   "fw_symbol": "C_PackCTOV_Warn / _WarnRst / _WarnDelay",
   "fw_value": "450.0 / 405.0 / 100",
   "match": "일치",
   "remark": ""
  },
  {
   "no": 2,
   "name": "Bsa_WrnSocH",
   "class": "warning",
   "trigger": 95.0,
   "unit": "%",
   "trigger_cond": "이상",
   "delay_ms": "100",
   "hysteresis_rate": 0.03,
   "release": 92.15,
   "release_cond": "미만",
   "can_bit": 1,
   "fw_symbol": "C_PackSOCOV_Warn / _WarnRst / _WarnDelay",
   "fw_value": "95.0 / 92.2 / 100",
   "match": "반올림",
   "remark": "[260727 대비 변경] 발생 100 → 95 %, 해제 97 → 92.15 % (히스 3 %)"
  },
  {
   "no": 3,
   "name": "Bsa_WrnSocL",
   "class": "warning",
   "trigger": 5.0,
   "unit": "%",
   "trigger_cond": "이하",
   "delay_ms": "100",
   "hysteresis_rate": 0.05,
   "release": 5.25,
   "release_cond": "초과",
   "can_bit": 2,
   "fw_symbol": "C_PackSOCUN_Warn / _WarnRst / _WarnDelay",
   "fw_value": "5.0 / 5.25 / 100",
   "match": "일치",
   "remark": ""
  },
  {
   "no": 4,
   "name": "Bsa_WrnOv",
   "class": "warning",
   "trigger": 90.86,
   "unit": "V",
   "trigger_cond": "이상",
   "delay_ms": "100",
   "hysteresis_rate": 0.03,
   "release": 88.1342,
   "release_cond": "미만",
   "can_bit": 3,
   "fw_symbol": "C_PackVoltOV_Warn / _WarnRst / _WarnDelay",
   "fw_value": "90.9 / 88.1 / 100",
   "match": "반올림",
   "remark": "[260727 대비 변경] 발생 90.2 → 90.86 V, 해제 87.494 → 88.1342 V (셀 4.13 V × 22S 환산, 히스 3 %)"
  },
  {
   "no": 5,
   "name": "Bsa_WrnUv",
   "class": "warning",
   "trigger": 66.0,
   "unit": "V",
   "trigger_cond": "이하",
   "delay_ms": "100",
   "hysteresis_rate": 0.05,
   "release": 69.3,
   "release_cond": "초과",
   "can_bit": 4,
   "fw_symbol": "C_PackVoltUN_Warn / _WarnRst / _WarnDelay",
   "fw_value": "66.0 / 69.3 / 100",
   "match": "일치",
   "remark": ""
  },
  {
   "no": 6,
   "name": "Bsa_WrnOt",
   "class": "warning",
   "trigger": 47.0,
   "unit": "ºC",
   "trigger_cond": "이상",
   "delay_ms": "100",
   "hysteresis_rate": 0.05,
   "release": 44.65,
   "release_cond": "미만",
   "can_bit": 5,
   "fw_symbol": "C_PackTempOV_Warn / _WarnRst / _WarnDelay",
   "fw_value": "47.0 / 44.7 / 100",
   "match": "반올림",
   "remark": ""
  },
  {
   "no": 7,
   "name": "Bsa_WrnUt",
   "class": "warning",
   "trigger": -20.0,
   "unit": "ºC",
   "trigger_cond": "이하",
   "delay_ms": "100",
   "hysteresis_rate": 0.05,
   "release": -19.0,
   "release_cond": "초과",
   "can_bit": 6,
   "fw_symbol": "C_PackTempUN_Warn / _WarnRst / _WarnDelay",
   "fw_value": "-20.0 / -19.0 / 100",
   "match": "일치",
   "remark": "[260727 대비 변경] 발생 -25 → -20 ℃, 해제 -23.75 → -19 ℃  ※No.13 셀 저온과 임계 교체"
  },
  {
   "no": 8,
   "name": "Bsa_WrnUnbalPwr",
   "class": "warning",
   "trigger": null,
   "unit": "",
   "trigger_cond": "",
   "delay_ms": "",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "",
   "can_bit": 7,
   "fw_symbol": "(없음)",
   "fw_value": "로직 없음",
   "match": "미구현",
   "remark": ""
  },
  {
   "no": 9,
   "name": "Bsa_WrnCellOv",
   "class": "warning",
   "trigger": 4.15,
   "unit": "V",
   "trigger_cond": "이상",
   "delay_ms": "100",
   "hysteresis_rate": 0.005,
   "release": 4.12925,
   "release_cond": "미만",
   "can_bit": 8,
   "fw_symbol": "C_CellVoltOV_Warn / _WarnRst / _WarnDelay",
   "fw_value": "4.15 / 4.129 / 100",
   "match": "반올림",
   "remark": "[260727 대비 변경] 발생 4.10 → 4.15 V, 해제 4.0795 → 4.12925 V (히스 0.5 %)"
  },
  {
   "no": 10,
   "name": "Bsa_WrnCellUv",
   "class": "warning",
   "trigger": 3.0,
   "unit": "V",
   "trigger_cond": "이하",
   "delay_ms": "100",
   "hysteresis_rate": 0.005,
   "release": 3.015,
   "release_cond": "초과",
   "can_bit": 9,
   "fw_symbol": "C_CellVoltUN_Warn / _WarnRst / _WarnDelay",
   "fw_value": "3.00 / 3.015 / 100",
   "match": "일치",
   "remark": ""
  },
  {
   "no": 11,
   "name": "Bsa_WrnCellUnbalV",
   "class": "warning",
   "trigger": 200.0,
   "unit": "mV",
   "trigger_cond": "이상",
   "delay_ms": "100",
   "hysteresis_rate": 0.66667,
   "release": 66.66667,
   "release_cond": "미만",
   "can_bit": 10,
   "fw_symbol": "C_CellVoltDIV_Warn / _WarnRst / _WarnDelay",
   "fw_value": "0.2 / 0.067 / 100",
   "match": "반올림",
   "remark": "[260727 대비 변경] 발생 300 → 200 mV, 해제 100 → 66.667 mV"
  },
  {
   "no": 12,
   "name": "Bsa_WrnCellOt",
   "class": "warning",
   "trigger": 55.0,
   "unit": "ºC",
   "trigger_cond": "이상",
   "delay_ms": "100",
   "hysteresis_rate": 0.05,
   "release": 52.25,
   "release_cond": "미만",
   "can_bit": 11,
   "fw_symbol": "C_CellTempOV_Warn / _WarnRst / _WarnDelay",
   "fw_value": "55.0 / 52.3 / 100",
   "match": "반올림",
   "remark": ""
  },
  {
   "no": 13,
   "name": "Bsa_WrnCellUt",
   "class": "warning",
   "trigger": -25.0,
   "unit": "ºC",
   "trigger_cond": "이하",
   "delay_ms": "100",
   "hysteresis_rate": 0.05,
   "release": -23.75,
   "release_cond": "초과",
   "can_bit": 12,
   "fw_symbol": "C_CellTempUN_Warn / _WarnRst / _WarnDelay",
   "fw_value": "-25.0 / -23.8 / 100",
   "match": "반올림",
   "remark": "[260727 대비 변경] 발생 -20 → -25 ℃, 해제 -19 → -23.75 ℃  ※No.7 팩 저온과 임계 교체"
  },
  {
   "no": 14,
   "name": "Bsa_WrnCellUnbalTmp",
   "class": "warning",
   "trigger": 10.0,
   "unit": "ºC",
   "trigger_cond": "이상",
   "delay_ms": "100",
   "hysteresis_rate": 0.5,
   "release": 5.0,
   "release_cond": "미만",
   "can_bit": 13,
   "fw_symbol": "C_CellTempDIV_Warn / _WarnRst / _WarnDelay",
   "fw_value": "10.0 / 5.0 / 100",
   "match": "일치",
   "remark": ""
  },
  {
   "no": 15,
   "name": "BPA_WrnVcuCan_Err",
   "class": "warning",
   "trigger": 10.0,
   "unit": "CONT",
   "trigger_cond": "이상 (1sec 이상 미수신)",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": 0.0,
   "release_cond": "이하 (1sec 이상 정상 수신)",
   "can_bit": 14,
   "fw_symbol": "C_VcuCommLostCount",
   "fw_value": "10u  (100ms x 10 = 1s)",
   "match": "일치",
   "remark": "[260727 대비 변경] Fault → Warning 등급 변경. 발생 50 → 10 CONT (미수신 5 s → 1 s), 해제 10 → 0 Count"
  },
  {
   "no": 16,
   "name": "Bsa_FltOc",
   "class": "fault",
   "trigger": 505.0,
   "unit": "A",
   "trigger_cond": "이상",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 16,
   "fw_symbol": "C_PackCTOV_Fault / _FaultDelay",
   "fw_value": "505.0 / 1",
   "match": "Delay 불일치",
   "remark": "[260727 대비 변경] 발생 500 → 505 A"
  },
  {
   "no": 17,
   "name": "Bsa_FltctSocH",
   "class": "fault",
   "trigger": 100.0,
   "unit": "%",
   "trigger_cond": "이상",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 17,
   "fw_symbol": "C_PackSOCOV_Fault / _FaultDelay",
   "fw_value": "100.0 / 0",
   "match": "일치",
   "remark": "[260727 대비 변경] 발생 101 → 100 %  ※SOC 표시 범위(0~100 %) 내에서 검출되도록 조정"
  },
  {
   "no": 18,
   "name": "Bsa_FlttctSocL",
   "class": "fault",
   "trigger": 0.0,
   "unit": "%",
   "trigger_cond": "이하",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 18,
   "fw_symbol": "C_PackSOCUN_Fault / _FaultDelay",
   "fw_value": "0.0 / 0",
   "match": "일치",
   "remark": "[260727 대비 변경] 발생 -0.1 → 0 %  ※SOC 표시 범위(0~100 %) 내에서 검출되도록 조정"
  },
  {
   "no": 19,
   "name": "Bsa_FltOv",
   "class": "fault",
   "trigger": 91.3,
   "unit": "V",
   "trigger_cond": "이상",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 19,
   "fw_symbol": "C_PackVoltOV_Fault / _FaultDelay",
   "fw_value": "91.3 / 2000",
   "match": "Delay 불일치",
   "remark": "[260727 대비 변경] Delay 2000 → 0 ms (즉시)"
  },
  {
   "no": 20,
   "name": "Bsa_FltUv",
   "class": "fault",
   "trigger": 58.3,
   "unit": "V",
   "trigger_cond": "이하",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 20,
   "fw_symbol": "C_PackVoltUN_Fault / _FaultDelay",
   "fw_value": "62.7 / 2000",
   "match": "값·Delay 불일치",
   "remark": "[260727 대비 변경] 발생 61.6 → 62.7 V, Delay 2000 → 0 ms (즉시)"
  },
  {
   "no": 21,
   "name": "Bsa_FltOt",
   "class": "fault",
   "trigger": 52.0,
   "unit": "ºC",
   "trigger_cond": "이상",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 21,
   "fw_symbol": "C_PackTempOV_Fault / _FaultDelay",
   "fw_value": "52.0 / 0",
   "match": "일치",
   "remark": ""
  },
  {
   "no": 22,
   "name": "Bsa_FltUt",
   "class": "fault",
   "trigger": -35.0,
   "unit": "ºC",
   "trigger_cond": "이하",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 22,
   "fw_symbol": "C_PackTempUN_Fault / _FaultDelay",
   "fw_value": "-35.0 / 0",
   "match": "일치",
   "remark": ""
  },
  {
   "no": 23,
   "name": "Bsa_FltUnbalPwr",
   "class": "fault",
   "trigger": null,
   "unit": "",
   "trigger_cond": "",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "",
   "can_bit": 23,
   "fw_symbol": "(없음)",
   "fw_value": "로직 없음",
   "match": "미구현",
   "remark": "설정값 미정 — 기준 확정 요청"
  },
  {
   "no": 24,
   "name": "Bsa_FltCellOv",
   "class": "fault",
   "trigger": 4.2,
   "unit": "V",
   "trigger_cond": "이상",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 24,
   "fw_symbol": "C_CellVoltOV_Fault / _FaultDelay",
   "fw_value": "4.20 / 2000",
   "match": "Delay 불일치",
   "remark": "[260727 대비 변경] 발생 4.15 → 4.20 V, Delay 2000 → 0 ms (즉시) 셀 기준 보호 최대치 변경을 즉시 차단 변경함"
  },
  {
   "no": 25,
   "name": "Bsa_FltCellUv",
   "class": "fault",
   "trigger": 2.65,
   "unit": "V",
   "trigger_cond": "이하",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 25,
   "fw_symbol": "C_CellVoltUN_Fault / _FaultDelay",
   "fw_value": "2.85 / 2000",
   "match": "값·Delay 불일치",
   "remark": "[260727 대비 변경] 발생 2.80 → 2.65 V, Delay 2000 → 0 ms (즉시) 셀 기준 보호 최대치 변경을 즉시 차단 변경함"
  },
  {
   "no": 26,
   "name": "Bsa_FltCellUnbalVlt",
   "class": "fault",
   "trigger": 350.0,
   "unit": "mV",
   "trigger_cond": "이상",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 26,
   "fw_symbol": "C_CellVoltDIV_Fault / _FaultDelay",
   "fw_value": "0.35 / 5000",
   "match": "Delay 불일치",
   "remark": "[260727 대비 변경] 발생 500 → 350 mV, Delay 5000 → 0 ms (즉시)"
  },
  {
   "no": 27,
   "name": "Bsa_FltCellOt",
   "class": "fault",
   "trigger": 60.0,
   "unit": "ºC",
   "trigger_cond": "이상",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 27,
   "fw_symbol": "C_CellTempOV_Fault / _FaultDelay",
   "fw_value": "60.0 / 0",
   "match": "일치",
   "remark": ""
  },
  {
   "no": 28,
   "name": "Bsa_FltCellUt",
   "class": "fault",
   "trigger": -30.0,
   "unit": "ºC",
   "trigger_cond": "이하",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 28,
   "fw_symbol": "C_CellTempUN_Fault / _FaultDelay",
   "fw_value": "-30.0 / 0",
   "match": "일치",
   "remark": ""
  },
  {
   "no": 29,
   "name": "Bsa_FltCellUnbalTmp",
   "class": "fault",
   "trigger": 20.0,
   "unit": "ºC",
   "trigger_cond": "이상",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 29,
   "fw_symbol": "C_CellTempDIV_Fault / _FaultDelay",
   "fw_value": "20.0 / 5000",
   "match": "Delay 불일치",
   "remark": "[260727 대비 변경] 발생 10 → 20 ℃ (경고 No.14 임계 10 ℃와 중복 해소, 폴트 임계 상향 확정)"
  },
  {
   "no": 30,
   "name": "Bsa_FltRly_Err",
   "class": "fault",
   "trigger": null,
   "unit": "",
   "trigger_cond": "",
   "delay_ms": "",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "",
   "can_bit": 30,
   "fw_symbol": "C_RleyCount",
   "fw_value": "1  (FaultReg 미설정 — 차단 로직 없음)",
   "match": "미구현",
   "remark": "[260827 신규] 릴레이 장애 — 통신규약 Fault bit30 대응 항목 신설. 설정값 확정 요청"
  },
  {
   "no": 31,
   "name": "BSA_Prtct_FAULT15",
   "class": "fault",
   "trigger": null,
   "unit": "",
   "trigger_cond": "",
   "delay_ms": "",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "",
   "can_bit": 31,
   "fw_symbol": "(예약)",
   "fw_value": "—",
   "match": "예약",
   "remark": "[260827 신규] 예약 슬롯 (통신규약 Fault bit31)"
  },
  {
   "no": 32,
   "name": "Bsa_FltCellIR_OV",
   "class": "fault",
   "trigger": 15.0,
   "unit": "mΩ",
   "trigger_cond": "이상",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 32,
   "fw_symbol": "(없음)",
   "fw_value": "로직 없음",
   "match": "2nd 이월",
   "remark": "2nd 반영 예정"
  },
  {
   "no": 33,
   "name": "Bsa_FltOcTimer",
   "class": "fault",
   "trigger": 1.0,
   "unit": "Sec",
   "trigger_cond": "480A 이상 1sec 이상 유지",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 33,
   "fw_symbol": "C_PackOCTimer_Fault / C_PackOCTimerCount",
   "fw_value": "480.0 / 1000",
   "match": "일치",
   "remark": "[R10 신규확장] CAN33  |  [260727 대비] 항목 번호 17 → 34 이동"
  },
  {
   "no": 34,
   "name": "Bsa_FltOcTime_min",
   "class": "fault",
   "trigger": 2.0,
   "unit": "Count/min",
   "trigger_cond": "480A 이상 1분당 2회 발생 시",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 34,
   "fw_symbol": "(없음)",
   "fw_value": "로직 없음",
   "match": "미구현",
   "remark": "[R10 신규확장] CAN34  |  [260727 대비] 항목 번호 18 → 35 이동"
  },
  {
   "no": 35,
   "name": "BPA_FltISOSPI_Err",
   "class": "fault",
   "trigger": 210.0,
   "unit": "CONT",
   "trigger_cond": "이상",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": 10.0,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": null,
   "fw_symbol": "C_ISOSPICount",
   "fw_value": "50",
   "match": "값 불일치",
   "remark": "[260727 대비] 항목 번호 34 → 36 이동 (설정값 변경 없음)"
  },
  {
   "no": 36,
   "name": "Bsa_FltRly_Err",
   "class": "fault",
   "trigger": null,
   "unit": "",
   "trigger_cond": "",
   "delay_ms": "",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "",
   "can_bit": null,
   "fw_symbol": "C_RleyCount",
   "fw_value": "1  (FaultReg 미설정 — 차단 로직 없음)",
   "match": "미구현",
   "remark": "[260827 신규] 2nd 릴레이 변경을 통해 기능 반영  ※No.31과 항목명 중복 — 정리 필요"
  },
  {
   "no": 37,
   "name": "Bsa_FltIMD_Err",
   "class": "fault",
   "trigger": null,
   "unit": "",
   "trigger_cond": "",
   "delay_ms": "",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "",
   "can_bit": null,
   "fw_symbol": "C_IOSresistanceFault",
   "fw_value": "45000  (판정 분리 안 됨)",
   "match": "2nd 이월",
   "remark": "[260827 신규] 2nd 반영, 배터리 팩 증가 Pack BMS 변경 통해 반영"
  }
 ]
}
```
