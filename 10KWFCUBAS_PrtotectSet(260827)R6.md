---
name: 10KWFCUBAS_PrtotectSet(260827)R6
title: 배터리 보호 설정값 — 10kWFC NCM 22S1P Pack BMS
revision: 260827 R6  (직전: 260827 R5)
source_xlsx: F:/2026_Project/01 현대자동차_10kWFC_2차년도/00 설계검토/04 보호설정/10KWFCUBAS_PrtotectSet(260827)R6.xlsx
firmware: SysInclude/parameter.h  (Product_Version 16)
can_id: 0x603 (BSA4) — Alarm bit0~15 / Protection bit16~39
items: 36  (Warning 15 / Fault 21)
generated: 2026-08-27
note: 엑셀이 master, 본 MD는 파생본. 값 수정은 엑셀에서 하고 MD를 다시 생성할 것.
---

# 배터리 보호 설정값 — 260827 R6

> 좌측이 **설정표(master)**, 우측이 **`parameter.h` 상수값**. 굵게 표시된 값은 설정표와 다릅니다.  
> 판정: `일치` / `반올림`(기능 영향 없음) / `값 불일치` / `Delay 불일치` / `미반영` / `2nd 이월` / `예약` / `설정값 미정`

## R5 → R6 변경 (5건)

| 구분 | No | 항목 | R5 | R6 |
|:--|:--:|:--|:--|:--|
| 설정값 확정 | 8 | `Bsa_WrnUnbalPwr` | (공란) | **10 Sec 이상 발생 / 0 Sec 미만 해제, Delay 100 ms** |
| 설정값 확정 | 23 | `Bsa_FltUnbalPwr` | (공란) | **15 Sec** |
| 값 변경 | 33 | `Bsa_FltOcTimer` | 1 Sec | **10 Sec** (480 A 이상 10 sec 유지) |
| 값 변경 | 34 | `Bsa_FltOcTime_min` | 2 Count/min | **5 Count/min** |
| 항목 삭제 | — | `Bsa_FltRly_Err` (구 No.36) | 중복 등재 | **삭제** — No.30 단일화 |

**항목 수 37 → 36** (Warning 15 / Fault 21).  
중복 항목이 정리되어 **폴트 No.16~34 가 0x603 비트 번호와 1:1 일치**(오프셋 0)합니다.

불균형 전력 보호(`UnbalPwr`)가 경고 10 sec / 폴트 15 sec 로 확정되면서, 260727 이후 유지되던 **설정값 미정 2건이 해소**되었습니다.
판정 방식은 「측정 전류가 연속 충·방전 전류 제한 연산값 이상으로 지정 시간 유지」 입니다.

---

## 1. 판정 규칙

| 항목 | 내용 |
|:--|:--|
| 발생 판정 | 설정표 "이상/이하" 표기. 구현은 strict 비교(`>` / `<`) |
| 해제 판정 | 하한계 항목은 부호 반전 적용 |
| 시간 기준 | main loop 1 ms. Delay 100 = 100 ms |
| 해제 지연 | 전 항목 0 (즉시) |
| 히스 산출 | Release = Trigger x (1 - Rate). 하한계 항목은 (1 + Rate) |
| 전류 부호 | **(+) = 충전 / (-) = 방전**. 과전류는 절대값 판정 |
| Warning | `BPA_Protect_Status = 0x03`, Rly = P·N On 유지, Warning flag 전송 |
| Fault | `BPA_Protect_Status = 0x04`, Rly = OPEN, 전원 OFF / `FCU_RESET=1` 전까지 latch |

---

## 2. Warning (경고) — 15 항목 · 0x603 Alarm bit0~14

| No | 항목 | Trigger | 단위 | 조건 | Delay | Hyst | Release | 조건 | 0x603 | parameter.h | 상수값 | 판정 |
|:--:|:--|--:|:--:|:--:|--:|--:|--:|:--:|:--:|:--|:--|:--:|
| 1 | `Bsa_WrnOC` | 450 | [A] | 이상 | 100 | 0.1 | 405 | 미만 | bit 0 | `C_PackCTOV_Warn / _WarnRst / _WarnDelay` | 450.0 / 405.0 / 100 | **일치** |
| 2 | `Bsa_WrnSocH` | 95 | [%] | 이상 | 100 | 0.03 | 92.15 | 미만 | bit 1 | `C_PackSOCOV_Warn / _WarnRst / _WarnDelay` | 95.0 / **92.2** / 100 | **반올림** |
| 3 | `Bsa_WrnSocL` | 5 | [%] | 이하 | 100 | 0.05 | 5.25 | 초과 | bit 2 | `C_PackSOCUN_Warn / _WarnRst / _WarnDelay` | 5.0 / 5.25 / 100 | **일치** |
| 4 | `Bsa_WrnOv` | 90.86 | [V] | 이상 | 100 | 0.03 | 88.1342 | 미만 | bit 3 | `C_PackVoltOV_Warn / _WarnRst / _WarnDelay` | **90.9** / **88.1** / 100 | **반올림** |
| 5 | `Bsa_WrnUv` | 66 | [V] | 이하 | 100 | 0.05 | 69.3 | 초과 | bit 4 | `C_PackVoltUN_Warn / _WarnRst / _WarnDelay` | 66.0 / 69.3 / 100 | **일치** |
| 6 | `Bsa_WrnOt` | 47 | [ºC] | 이상 | 100 | 0.05 | 44.65 | 미만 | bit 5 | `C_PackTempOV_Warn / _WarnRst / _WarnDelay` | 47.0 / **44.7** / 100 | **반올림** |
| 7 | `Bsa_WrnUt` | -20 | [ºC] | 이하 | 100 | 0.05 | -19 | 초과 | bit 6 | `C_PackTempUN_Warn / _WarnRst / _WarnDelay` | -20.0 / -19.0 / 100 | **일치** |
| 8 | `Bsa_WrnUnbalPwr` | 10 | Sec | 이상 | 100 | — | 0 | 미만 | bit 7 | `(없음)` | 상수 없음 | **미반영** |
| 9 | `Bsa_WrnCellOv` | 4.15 | [V] | 이상 | 100 | 0.005 | 4.12925 | 미만 | bit 8 | `C_CellVoltOV_Warn / _WarnRst / _WarnDelay` | 4.15 / **4.129** / 100 | **반올림** |
| 10 | `Bsa_WrnCellUv` | 3 | [V] | 이하 | 100 | 0.005 | 3.015 | 초과 | bit 9 | `C_CellVoltUN_Warn / _WarnRst / _WarnDelay` | 3.00 / 3.015 / 100 | **일치** |
| 11 | `Bsa_WrnCellUnbalV` | 200 | [mV] | 이상 | 100 | 0.66667 | 66.66667 | 미만 | bit 10 | `C_CellVoltDIV_Warn / _WarnRst / _WarnDelay` | 0.2 / **0.067** / 100 | **반올림** |
| 12 | `Bsa_WrnCellOt` | 55 | [ºC] | 이상 | 100 | 0.05 | 52.25 | 미만 | bit 11 | `C_CellTempOV_Warn / _WarnRst / _WarnDelay` | 55.0 / **52.3** / 100 | **반올림** |
| 13 | `Bsa_WrnCellUt` | -25 | [ºC] | 이하 | 100 | 0.05 | -23.75 | 초과 | bit 12 | `C_CellTempUN_Warn / _WarnRst / _WarnDelay` | -25.0 / **-23.8** / 100 | **반올림** |
| 14 | `Bsa_WrnCellUnbalTmp` | 10 | [ºC] | 이상 | 100 | 0.5 | 5 | 미만 | bit 13 | `C_CellTempDIV_Warn / _WarnRst / _WarnDelay` | 10.0 / 5.0 / 100 | **일치** |
| 15 | `BPA_WrnVcuCan_Err` | 10 | CONT | 이상 (1sec 이상 미수신) | 즉시 = 0 | - | 0 | 이하 (1sec 이상 정상 수신) | bit 14 | `C_VcuCommLostCount` | 10u  (100ms x 10 = 1s) | **일치** |

---

## 3. Fault (보호/차단) — 21 항목 · 0x603 Protection bit16~34

| No | 항목 | Trigger | 단위 | 조건 | Delay | 0x603 | parameter.h | 상수값 | 판정 |
|:--:|:--|--:|:--:|:--:|--:|:--:|:--|:--|:--:|
| 16 | `Bsa_FltOc` | 505 | [A] | 이상 | 즉시 = 0 | bit 16 | `C_PackCTOV_Fault / _FaultDelay` | 505.0 / **1** | **Delay 불일치** |
| 17 | `Bsa_FltctSocH` | 100 | [%] | 이상 | 즉시 = 0 | bit 17 | `C_PackSOCOV_Fault / _FaultDelay` | 100.0 / 0 | **일치** |
| 18 | `Bsa_FlttctSocL` | 0 | [%] | 이하 | 즉시 = 0 | bit 18 | `C_PackSOCUN_Fault / _FaultDelay` | 0.0 / 0 | **일치** |
| 19 | `Bsa_FltOv` | 91.3 | [V] | 이상 | 즉시 = 0 | bit 19 | `C_PackVoltOV_Fault / _FaultDelay` | 91.3 / **2000** | **Delay 불일치** |
| 20 | `Bsa_FltUv` | 58.3 | [V] | 이하 | 즉시 = 0 | bit 20 | `C_PackVoltUN_Fault / _FaultDelay` | **62.7** / **2000** | **값·Delay 불일치** |
| 21 | `Bsa_FltOt` | 52 | [ºC] | 이상 | 즉시 = 0 | bit 21 | `C_PackTempOV_Fault / _FaultDelay` | 52.0 / 0 | **일치** |
| 22 | `Bsa_FltUt` | -35 | [ºC] | 이하 | 즉시 = 0 | bit 22 | `C_PackTempUN_Fault / _FaultDelay` | -35.0 / 0 | **일치** |
| 23 | `Bsa_FltUnbalPwr` | 15 | Sec | — | 즉시 = 0 | bit 23 | `(없음)` | 상수 없음 | **미반영** |
| 24 | `Bsa_FltCellOv` | 4.2 | [V] | 이상 | 즉시 = 0 | bit 24 | `C_CellVoltOV_Fault / _FaultDelay` | 4.20 / **2000** | **Delay 불일치** |
| 25 | `Bsa_FltCellUv` | 2.65 | [V] | 이하 | 즉시 = 0 | bit 25 | `C_CellVoltUN_Fault / _FaultDelay` | **2.85** / **2000** | **값·Delay 불일치** |
| 26 | `Bsa_FltCellUnbalVlt` | 350 | [mV] | 이상 | 즉시 = 0 | bit 26 | `C_CellVoltDIV_Fault / _FaultDelay` | 0.35 / **5000** | **Delay 불일치** |
| 27 | `Bsa_FltCellOt` | 60 | [ºC] | 이상 | 즉시 = 0 | bit 27 | `C_CellTempOV_Fault / _FaultDelay` | 60.0 / 0 | **일치** |
| 28 | `Bsa_FltCellUt` | -30 | [ºC] | 이하 | 즉시 = 0 | bit 28 | `C_CellTempUN_Fault / _FaultDelay` | -30.0 / 0 | **일치** |
| 29 | `Bsa_FltCellUnbalTmp` | 20 | [ºC] | 이상 | 즉시 = 0 | bit 29 | `C_CellTempDIV_Fault / _FaultDelay` | 20.0 / **5000** | **Delay 불일치** |
| 30 | `Bsa_FltRly_Err` | — | — | — | — | bit 30 | `C_RleyCount` | 1 | **설정값 미정** |
| 31 | `BSA_Prtct_FAULT15` | — | — | — | — | bit 31 | `(예약)` | — | **예약** |
| 32 | `Bsa_FltCellIR_OV` | 15 | mΩ | 이상 | 즉시 = 0 | bit 32 | `(없음)` | 상수 없음 | **2nd 이월** |
| 33 | `Bsa_FltOcTimer` | 10 | Sec | 480A 이상 10sec 이상 유지 | 즉시 = 0 | bit 33 | `C_PackOCTimer_Fault / C_PackOCTimerCount` | 480.0 / **1000 (1 s)** | **값 불일치** |
| 34 | `Bsa_FltOcTime_min` | 5 | Count/min | 480A 이상 1분당 2회 발생 시 | 즉시 = 0 | bit 34 | `(없음)` | 상수 없음 | **미반영** |
| 35 | `BPA_FltISOSPI_Err` | 210 | CONT | 이상 | 즉시 = 0 | **없음** | `C_ISOSPICount` | **50** | **값 불일치** |
| 36 | `Bsa_FltIMD_Err` | — | — | — | — | **없음** | `C_IOSresistanceFault` | 45000 | **2nd 이월** |

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

Protection  bit 16~39      ※ 항목 No = 비트 번호 (No.16~34, 오프셋 0)
  16 FltOc         17 FltctSocH     18 FlttctSocL      19 FltOv
  20 FltUv         21 FltOt         22 FltUt           23 FltUnbalPwr
  24 FltCellOv     25 FltCellUv     26 FltCellUnbalVlt 27 FltCellOt
  28 FltCellUt     29 FltCellUnbalTmp                 30 FltRly_Err
  31 (예약 = BSA_Prtct_FAULT15)
  32 FltCellIR_OV  33 FltOcTimer    34 FltOcTime_min
  35~39 (예약)
```

**규약 비트 미할당 2건** — No.35 `BPA_FltISOSPI_Err` · No.36 `Bsa_FltIMD_Err`  
※ 통신규약 R11 에서는 각각 CAN35 · CAN36 으로 정의되어 있어, 설정표 번호와 규약 비트를 함께 확인 필요.

---

## 5. parameter.h 상수 대비 차이

### 5-1. 값 불일치 4건

| 항목 | 설정표 R6 | parameter.h | 조치 |
|:--|:--|:--|:--|
| `Bsa_FltUv` | **58.3 V** | 62.7 | R4 변경분 미반영 |
| `Bsa_FltCellUv` | **2.65 V** | 2.85 | R4 변경분 미반영 |
| `Bsa_FltOcTimer` | **10 Sec** | `C_PackOCTimerCount` 1000 (= 1 s) | **R6 신규** — 10000 으로 변경 필요 |
| `BPA_FltISOSPI_Err` | 210 CONT | **50** | 기준 확정 후 일치화 |

### 5-2. Delay 차이 — 설정표 전 항목 「즉시 = 0」

| parameter.h | 현재 |
|:--|--:|
| `C_PackCTOV_FaultDelay` | 1 ms |
| `C_PackVoltOV_FaultDelay` / `C_PackVoltUN_FaultDelay` | 2000 ms |
| `C_CellVoltOV_FaultDelay` / `C_CellVoltUN_FaultDelay` | 2000 ms |
| `C_CellVoltDIV_FaultDelay` / `C_CellTempDIV_FaultDelay` | 5000 ms |

### 5-3. 반올림 차이 (기능 영향 없음)

`WrnSocH` 92.15→92.2 ｜ `WrnOv` 90.86→90.9, 88.1342→88.1 ｜ `WrnOt` 44.65→44.7 ｜
`WrnCellOv` 4.12925→4.129 ｜ `WrnCellUnbalV` 66.667→67 mV ｜ `WrnCellOt` 52.25→52.3 ｜ `WrnCellUt` -23.75→-23.8

### 5-4. 상수 미반영 · 이월

| 구분 | 항목 |
|:--|:--|
| 상수 미반영 (R6 신규 확정분 포함) | `Bsa_WrnUnbalPwr` 10 Sec ｜ `Bsa_FltUnbalPwr` 15 Sec ｜ `Bsa_FltOcTime_min` 5 Count/min |
| 설정값 미정 | `Bsa_FltRly_Err` (No.30) |
| 2nd 이월 | `Bsa_FltCellIR_OV` ｜ `Bsa_FltIMD_Err` |

---

## 6. 원본 엑셀 반영 필요 (비고란 정합)

| No | 항목 | 내용 |
|:--:|:--|:--|
| 20 | `Bsa_FltUv` | 비고 「발생 61.6 → **62.7** V」 ↔ 실제 값 **58.3 V** — 미갱신 |
| 23 | `Bsa_FltUnbalPwr` | 비고 「… 15sec 이상 유지 시 **경고** 발생」 — 폴트 항목이므로 **차단**으로 정정 |
| 34 | `Bsa_FltOcTime_min` | Trigger 조건 「480A 이상 1분당 **2회** 발생 시」 ↔ 실제 값 **5 Count/min** — 미갱신 |
| 33·34 | `Bsa_FltOcTimer` · `_Time_min` | 비고 「항목 번호 17 → **34** / 18 → **35** 이동」 ↔ 현재 번호 **33 / 34** — 미갱신 |
| 35 | `BPA_FltISOSPI_Err` | 비고 「항목 번호 34 → **36** 이동」 ↔ 현재 번호 **35** — 미갱신 |

---

## 7. parameter.h 정렬안 (설정표 R6 기준)

```c
/* ── 260827 R6 반영 ── */

/* [1] 값 불일치 */
#define C_PackVoltUN_Fault         58.3     /* 현재 62.7   (= 2.65V x 22S) */
#define C_CellVoltUN_Fault         2.65     /* 현재 2.85                   */
#define C_PackOCTimerCount         10000    /* 현재 1000   (1s -> 10s)     */
#define C_ISOSPICount              210      /* 현재 50     (기준 확정 후)   */

/* [2] Fault Delay — 설정표는 전 항목 즉시(0) */
#define C_PackCTOV_FaultDelay      0        /* 현재 1    */
#define C_PackVoltOV_FaultDelay    0        /* 현재 2000 */
#define C_PackVoltUN_FaultDelay    0        /* 현재 2000 */
#define C_CellVoltOV_FaultDelay    0        /* 현재 2000 */
#define C_CellVoltUN_FaultDelay    0        /* 현재 2000 */
#define C_CellVoltDIV_FaultDelay   0        /* 현재 5000 */
#define C_CellTempDIV_FaultDelay   0        /* 현재 5000 */

/* [3] R6 신규 확정 — 상수 신설 필요 */
/* Bsa_WrnUnbalPwr    10 [Sec]        연속 전류제한 초과 유지 시간(경고) */
/* Bsa_FltUnbalPwr    15 [Sec]        연속 전류제한 초과 유지 시간(차단) */
/* Bsa_FltOcTime_min   5 [Count/min]  1분당 발생 횟수                    */

/* [4] 설정값 미정 · 2nd 이월 */
/* Bsa_FltRly_Err                     설정값 확정 요청                   */
/* Bsa_FltCellIR_OV   15 [mOhm]       2nd 이월                          */
/* Bsa_FltIMD_Err                     2nd 이월                          */
```

---

## 8. 기계 판독용 데이터

```json
{
 "revision": "260827R6",
 "source": "10KWFCUBAS_PrtotectSet(260827)R6.xlsx",
 "firmware": "parameter.h Product_Version 16",
 "can_id": "0x603",
 "counts": {
  "warning": 15,
  "fault": 21,
  "total": 36
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
   "trigger": 10.0,
   "unit": "Sec",
   "trigger_cond": "이상",
   "delay_ms": "100",
   "hysteresis_rate": null,
   "release": 0.0,
   "release_cond": "미만",
   "can_bit": 7,
   "fw_symbol": "(없음)",
   "fw_value": "상수 없음",
   "match": "미반영",
   "remark": "[260727 대비 변경] 측정전류가 연속 충방전 전류 제한 연산값 이상으로 10sec 이상 유지 시 경고 발생"
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
   "trigger": 15.0,
   "unit": "Sec",
   "trigger_cond": "",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "",
   "can_bit": 23,
   "fw_symbol": "(없음)",
   "fw_value": "상수 없음",
   "match": "미반영",
   "remark": "[260727 대비 변경] 측정전류가 연속 충방전 전류 제한 연산값 이상으로 15sec 이상 유지 시 경고 발생"
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
   "fw_value": "1",
   "match": "설정값 미정",
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
   "fw_value": "상수 없음",
   "match": "2nd 이월",
   "remark": "2nd 반영 예정"
  },
  {
   "no": 33,
   "name": "Bsa_FltOcTimer",
   "class": "fault",
   "trigger": 10.0,
   "unit": "Sec",
   "trigger_cond": "480A 이상 10sec 이상 유지",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 33,
   "fw_symbol": "C_PackOCTimer_Fault / C_PackOCTimerCount",
   "fw_value": "480.0 / 1000 (1 s)",
   "match": "값 불일치",
   "remark": "[R10 신규확장] CAN33  |  [260727 대비] 항목 번호 17 → 34 이동"
  },
  {
   "no": 34,
   "name": "Bsa_FltOcTime_min",
   "class": "fault",
   "trigger": 5.0,
   "unit": "Count/min",
   "trigger_cond": "480A 이상 1분당 2회 발생 시",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 34,
   "fw_symbol": "(없음)",
   "fw_value": "상수 없음",
   "match": "미반영",
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
   "fw_value": "45000",
   "match": "2nd 이월",
   "remark": "[260827 신규] 2nd 반영, 배터리 팩 증가 Pack BMS 변경 통해 반영"
  }
 ]
}
```
