---
name: 10KWFCUBAS_PrtotectSet(260830)R3
title: 배터리 보호 설정값 — 10kWFC NCM 22S1P Pack BMS
revision: 260830 R3  (기준 개정: 260827 R2 / 변경 기준: PrtotectSet260727)
source_xlsx: F:/2026_Project/01 현대자동차_10kWFC_2차년도/00 설계검토/04 보호설정/10KWFCUBAS_PrtotectSet(260830)R3.xlsx
firmware: SysInclude/parameter.h  (Product_Version 16, 260810 반영)
can_id: 0x603 (BSA4) — Alarm bit0~15 / Protection bit16~39
items: 38  (Warning 15 / Fault 23)
generated: 2026-08-26
note: 엑셀이 master, 본 MD는 파생본. 값 수정은 엑셀에서 하고 MD를 다시 생성할 것.
---

# 배터리 보호 설정값 — 260830 R3

> 좌측이 **설정표(master)**, 우측이 **펌웨어 현행값**입니다. 굵게 표시된 펌웨어 값은 설정표와 다릅니다.  
> 판정 열: `일치` / `반올림`(기능 영향 없음) / `Delay 불일치` / `값 불일치` / `미구현` / `예약`

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

## 3. Fault (보호/차단) — 23 항목 · 0x603 Protection bit16~34

| No | 항목 | Trigger | 단위 | 조건 | Delay | 0x603 | parameter.h | 펌웨어 현행값 | 판정 |
|:--:|:--|--:|:--:|:--:|--:|:--:|:--|:--|:--:|
| 16 | `Bsa_FltOc` | 505 | [A] | 이상 | 즉시 = 0 | bit 16 | `C_PackCTOV_Fault / _FaultDelay` | 505.0 / **1** | **Delay 불일치** |
| 17 | `Bsa_FltctSocH` | 100 | [%] | 이상 | 즉시 = 0 | bit 17 | `C_PackSOCOV_Fault / _FaultDelay` | 100.0 / 0 | **일치** |
| 18 | `Bsa_FlttctSocL` | 0 | [%] | 이하 | 즉시 = 0 | bit 18 | `C_PackSOCUN_Fault / _FaultDelay` | 0.0 / 0 | **일치** |
| 19 | `Bsa_FltOv` | 91.3 | [V] | 이상 | 즉시 = 0 | bit 19 | `C_PackVoltOV_Fault / _FaultDelay` | 91.3 / **2000** | **Delay 불일치** |
| 20 | `Bsa_FltUv` | 62.7 | [V] | 이하 | 즉시 = 0 | bit 20 | `C_PackVoltUN_Fault / _FaultDelay` | 62.7 / **2000** | **Delay 불일치** |
| 21 | `Bsa_FltOt` | 52 | [ºC] | 이상 | 즉시 = 0 | bit 21 | `C_PackTempOV_Fault / _FaultDelay` | 52.0 / 0 | **일치** |
| 22 | `Bsa_FltUt` | -35 | [ºC] | 이하 | 즉시 = 0 | bit 22 | `C_PackTempUN_Fault / _FaultDelay` | -35.0 / 0 | **일치** |
| 23 | `Bsa_FltUnbalPwr` | — | — | — | 즉시 = 0 | bit 23 | `(없음)` | 로직 없음 | **미구현** |
| 24 | `Bsa_FltCellOv` | 4.2 | [V] | 이상 | 즉시 = 0 | bit 24 | `C_CellVoltOV_Fault / _FaultDelay` | 4.20 / **2000** | **Delay 불일치** |
| 25 | `Bsa_FltCellUv` | 2.85 | [V] | 이하 | 즉시 = 0 | bit 25 | `C_CellVoltUN_Fault / _FaultDelay` | 2.85 / **2000** | **Delay 불일치** |
| 26 | `Bsa_FltCellUnbalVlt` | 350 | [mV] | 이상 | 즉시 = 0 | bit 26 | `C_CellVoltDIV_Fault / _FaultDelay` | 0.35 / **5000** | **Delay 불일치** |
| 27 | `Bsa_FltCellOt` | 60 | [ºC] | 이상 | 즉시 = 0 | bit 27 | `C_CellTempOV_Fault / _FaultDelay` | 60.0 / 0 | **일치** |
| 28 | `Bsa_FltCellUt` | -30 | [ºC] | 이하 | 즉시 = 0 | bit 28 | `C_CellTempUN_Fault / _FaultDelay` | -30.0 / 0 | **일치** |
| 29 | `Bsa_FltCharCellUt` | 15 | [ºC] | 이하 | 즉시 = 0 | **없음** | `(없음)` | 로직 없음 | **미구현** |
| 30 | `Bsa_FltCellUnbalTmp` | 10 | [ºC] | 이상 | 즉시 = 0 | bit 29 | `C_CellTempDIV_Fault / _FaultDelay` | **20.0** / **5000** | **값·Delay 불일치** |
| 31 | `Bsa_FltRly_Err` | — | — | — | — | bit 30 | `C_RleyCount` | 1  (판정 로직 미완 — FaultReg 미설정) | **미구현** |
| 32 | `BSA_Prtct_FAULT15` | — | — | — | — | bit 31 | `(예약)` | — | **예약** |
| 33 | `Bsa_FltCellIR_OV` | 15 | mΩ | 이상 | 즉시 = 0 | bit 32 | `(없음)` | 로직 없음 | **미구현** |
| 34 | `Bsa_FltOcTimer` | 1 | Sec | 480A 이상 1sec 이상 유지 | 즉시 = 0 | bit 33 | `C_PackOCTimer_Fault / C_PackOCTimerCount` | 480.0 / 1000 | **일치** |
| 35 | `Bsa_FltOcTime_min` | 2 | Count/min | 480A 이상 1분당 2회 발생 시 | 즉시 = 0 | bit 34 | `(없음)` | 로직 없음 | **미구현** |
| 36 | `BPA_FltISOSPI_Err` | 210 | CONT | 이상 | 즉시 = 0 | **없음** | `C_ISOSPICount` | **50** | **값 불일치** |
| 37 | `Bsa_FltRly_Err` | — | — | — | — | **없음** | `C_RleyCount` | 1  (판정 로직 미완 — FaultReg 미설정) | **미구현** |
| 38 | `Bsa_FltIMD_Err` | — | — | — | — | **없음** | `C_IOSresistanceFault` | 45000  (판정 로직 미완) | **미구현** |

> Fault 해제는 전 항목 공통 : **전원 OFF 또는 `FCU_RESET=1` 전까지 FAULT 래치 유지**

---

## 4. 0x603 (BSA4) 비트 배치

```
Alarm  bit 0~15
   0 WrnOC          1 WrnSocH        2 WrnSocL         3 WrnOv
   4 WrnUv          5 WrnOt          6 WrnUt           7 WrnUnbalPwr
   8 WrnCellOv      9 WrnCellUv     10 WrnCellUnbalV  11 WrnCellOt
  12 WrnCellUt     13 WrnCellUnbalTmp                14 WrnVcuCan_Err
  15 (예약)

Protection  bit 16~39
  16 FltOc         17 FltctSocH     18 FlttctSocL     19 FltOv
  20 FltUv         21 FltOt         22 FltUt          23 FltUnbalPwr
  24 FltCellOv     25 FltCellUv     26 FltCellUnbalVlt 27 FltCellOt
  28 FltCellUt     29 FltCellUnbalTmp
  30 FltRly_Err    31 (예약 = BSA_Prtct_FAULT15)
  32 FltCellIR_OV  33 FltOcTimer    34 FltOcTime_min
  35~39 (예약)
```

**규약 비트 미할당 3건** — `Bsa_FltCharCellUt` · `BPA_FltISOSPI_Err` · `Bsa_FltIMD_Err`  
→ 발생해도 VCU 통보 수단이 없음. 통신규약 R10 반영 요청 대상.

---

## 5. 260727 → 260830 R3 변경 내역

### 5-1. Warning 값 변경 7건

| No | 항목 | 260727 | 260830 R3 |
|:--:|:--|:--|:--|
| 2 | `Bsa_WrnSocH` | 100 / 97 % | **95 / 92.15 %** |
| 4 | `Bsa_WrnOv` | 90.2 / 87.494 V | **90.86 / 88.1342 V** |
| 7 | `Bsa_WrnUt` | -25 / -23.75 ℃ | **-20 / -19 ℃**  ※No.13 과 교체 |
| 9 | `Bsa_WrnCellOv` | 4.10 / 4.0795 V | **4.15 / 4.12925 V** |
| 11 | `Bsa_WrnCellUnbalV` | 300 / 100 mV | **200 / 66.667 mV** |
| 13 | `Bsa_WrnCellUt` | -20 / -19 ℃ | **-25 / -23.75 ℃**  ※No.7 과 교체 |
| 15 | `BPA_WrnVcuCan_Err` | Fault 등급 · 50 CONT (5 s) / 해제 10 | **Warning 등급 · 10 CONT (1 s) / 해제 0** |

### 5-2. Fault 값 변경 8건 + 지연 통일 5건

| No | 항목 | 260727 | 260830 R3 |
|:--:|:--|:--|:--|
| 16 | `Bsa_FltOc` | 500 A | **505 A** |
| 17 | `Bsa_FltctSocH` | 101 % | **100 %**  (SOC 클램프 0~100 내 검출 가능) |
| 18 | `Bsa_FlttctSocL` | -0.1 % | **0 %**  (SOC 클램프 0~100 내 검출 가능) |
| 19 | `Bsa_FltOv` | 91.3 V / Delay 2000 ms | 91.3 V / **즉시 0** |
| 20 | `Bsa_FltUv` | 61.6 V / Delay 2000 ms | **62.7 V** / **즉시 0** |
| 24 | `Bsa_FltCellOv` | 4.15 V / Delay 2000 ms | **4.20 V** / **즉시 0** |
| 25 | `Bsa_FltCellUv` | 2.80 V / Delay 2000 ms | **2.85 V** / **즉시 0** |
| 26 | `Bsa_FltCellUnbalVlt` | 500 mV / Delay 5000 ms | **350 mV** / **즉시 0** |

### 5-3. 항목 증감 · 재배열

| 구분 | 내용 |
|:--|:--|
| 신규 3건 | No.31 `Bsa_FltRly_Err` (규약 bit30 대응) · No.32 `BSA_Prtct_FAULT15` (예약=bit31) · No.38 `Bsa_FltIMD_Err` |
| 삭제 1건 | 구 No.35 `BPA_FltCan_Err` (1000 CONT) — No.15 경고로 등급 변경되어 폴트 항목 삭제 |
| 번호 재배열 10건 | 폴트 번호를 0x603 비트 순서로 정렬. `Bsa_FltOcTimer` 17→34, `Bsa_FltOcTime_min` 18→35 |

---

## 6. 펌웨어 대비 불일치 요약

### 6-1. 값 불일치 (조치 필요)

| 항목 | 설정표 R3 | parameter.h | 조치 |
|:--|:--|:--|:--|
| `Bsa_FltCellUnbalTmp` | 10 ℃ | **20.0 ℃** | 260810 펌웨어가 선행 변경 (경고 10 ℃와 동일 문제 해소). **설정표 갱신 필요** |
| `BPA_FltISOSPI_Err` | 210 CONT | **50** | 기준 확정 후 일치화 |

### 6-2. Delay 불일치 — 설정표 전 항목 「즉시 = 0」 ↔ 펌웨어 지연 존재

| 항목 | parameter.h | 조치 |
|:--|--:|:--|
| `C_PackCTOV_FaultDelay` | 1 ms | → 0 |
| `C_PackVoltOV_FaultDelay` / `C_PackVoltUN_FaultDelay` | 2000 ms | → 0 |
| `C_CellVoltOV_FaultDelay` / `C_CellVoltUN_FaultDelay` | 2000 ms | → 0 |
| `C_CellVoltDIV_FaultDelay` | 5000 ms | → 0 |
| `C_CellTempDIV_FaultDelay` | 5000 ms | → 0 |

### 6-3. 반올림 차이 (기능 영향 없음, 표기 통일 권고)

`WrnSocH` 92.15→92.2 ｜ `WrnOv` 90.86→90.9, 88.1342→88.1 ｜ `WrnOt` 44.65→44.7 ｜
`WrnCellOv` 4.12925→4.129 ｜ `WrnCellUnbalV` 66.667→67 mV ｜ `WrnCellOt` 52.25→52.3 ｜ `WrnCellUt` -23.75→-23.8

### 6-4. 미구현 7건

`Bsa_WrnUnbalPwr` · `Bsa_FltUnbalPwr` (설정값 미정) ｜ `Bsa_FltCharCellUt` ｜ `Bsa_FltCellIR_OV` ｜
`Bsa_FltOcTime_min` ｜ `Bsa_FltRly_Err` (플래그만 set, FaultReg 미설정) ｜ `Bsa_FltIMD_Err`

### 6-5. 상위 이슈 — 설정값과 무관, SW 조치 필수

| 항목 | 내용 |
|:--|:--|
| **Protect 판정 전면 미동작** | `main.c:921` `// Cal80VSysFaultCheck(&SysRegs);` 주석 처리 → **본 문서의 Fault 23 항목 전체가 동작하지 않음**. C13 시험(셀 온도편차 10.1 ℃ 20 s 유지 → Fault 미발생)으로 실측 확인 |
| Fault 레지스터 폭 | `BAT80VFaultReg.all` 이 16-bit 비교라 bit16 이상(2번째 워드) 플래그가 판정·클리어에서 누락 (`main.c:922, 955`) |
| 릴레이 융착 | `DSP28x_Project.c:903-909` 에서 `FaulBuftReg.PackRLY_ERR` 만 set, `FaultReg` 미설정 → 차단 없음. 절연저항 이상도 같은 비트에 오매핑 |
| 항목명 중복 | `Bsa_FltRly_Err` 가 No.31 · No.37 에 중복 등재 — 정리 필요 |
| 셀 저전압 폴트 | `Bsa_FltCellUv` 2.85 V 가 방전종지 2.8 V 를 상회 → 셀 편차 시 조기 차단 가능성. 검증 필요 |

---

## 7. parameter.h 정렬 패치 (설정표 R3 기준)

```c
/* ── 260830 R3 반영 ── */

/* [1] Fault Delay — 설정표는 전 항목 즉시(0) */
#define C_PackCTOV_FaultDelay      0      /* 현재 1    */
#define C_PackVoltOV_FaultDelay    0      /* 현재 2000 */
#define C_PackVoltUN_FaultDelay    0      /* 현재 2000 */
#define C_CellVoltOV_FaultDelay    0      /* 현재 2000 */
#define C_CellVoltUN_FaultDelay    0      /* 현재 2000 */
#define C_CellVoltDIV_FaultDelay   0      /* 현재 5000 */
#define C_CellTempDIV_FaultDelay   0      /* 현재 5000 */

/* [2] 셀 온도편차 폴트 — 펌웨어 20.0 선행. 설정표(10)와 확정 필요 */
#define C_CellTempDIV_Fault        20.0   /* 유지 시 설정표를 20 으로 갱신 */

/* [3] ISOSPI 카운트 — 설정표 기준 정렬 시 */
#define C_ISOSPICount              210    /* 현재 50 */

/* [4] 최우선 — 주석 해제 없이는 위 설정 전부 무효 */
/* main.c:921   Cal80VSysFaultCheck(&SysRegs);   ← 주석 해제 */

/* [5] 미구현 — 로직 신규 작성 */
/* Bsa_FltCharCellUt   15 [degC]      충전 모드 게이팅 저온 충전 금지 */
/* Bsa_FltCellIR_OV    15 [mOhm]      셀 내부저항 과대                */
/* Bsa_FltOcTime_min    2 [Count/min] 480A 이상 1분당 2회             */
/* Bsa_FltRly_Err                     FaultReg 설정 + 차단 로직        */
/* Bsa_FltIMD_Err                     C_IOSresistanceFault 판정 분리   */
/* Bsa_Wrn/FltUnbalPwr                설정값 미정                      */
```

---

## 8. 기계 판독용 데이터

```json
{
 "revision": "260830R3",
 "source": "10KWFCUBAS_PrtotectSet(260830)R3.xlsx",
 "firmware": "parameter.h Product_Version 16",
 "can_id": "0x603",
 "counts": {
  "warning": 15,
  "fault": 23,
  "total": 38
 },
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
   "trigger": 62.7,
   "unit": "V",
   "trigger_cond": "이하",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 20,
   "fw_symbol": "C_PackVoltUN_Fault / _FaultDelay",
   "fw_value": "62.7 / 2000",
   "match": "Delay 불일치",
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
   "remark": "[260727 대비 변경] 발생 4.15 → 4.20 V, Delay 2000 → 0 ms (즉시)"
  },
  {
   "no": 25,
   "name": "Bsa_FltCellUv",
   "class": "fault",
   "trigger": 2.85,
   "unit": "V",
   "trigger_cond": "이하",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 25,
   "fw_symbol": "C_CellVoltUN_Fault / _FaultDelay",
   "fw_value": "2.85 / 2000",
   "match": "Delay 불일치",
   "remark": "[260727 대비 변경] 발생 2.80 → 2.85 V, Delay 2000 → 0 ms (즉시)  ※방전종지 2.8 V 대비 상회 — 셀 편차 시 조기 차단 가능성 검토 필요"
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
   "name": "Bsa_FltCharCellUt",
   "class": "fault",
   "trigger": 15.0,
   "unit": "ºC",
   "trigger_cond": "이하",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": null,
   "fw_symbol": "(없음)",
   "fw_value": "로직 없음",
   "match": "미구현",
   "remark": "통신규약 Fault 비트 미할당 — 규약 반영 요청"
  },
  {
   "no": 30,
   "name": "Bsa_FltCellUnbalTmp",
   "class": "fault",
   "trigger": 10.0,
   "unit": "ºC",
   "trigger_cond": "이상",
   "delay_ms": "즉시 = 0",
   "hysteresis_rate": null,
   "release": null,
   "release_cond": "전원 OFF, FCU_RESET=1 전까지 FAULT 유지",
   "can_bit": 29,
   "fw_symbol": "C_CellTempDIV_Fault / _FaultDelay",
   "fw_value": "20.0 / 5000",
   "match": "값·Delay 불일치",
   "remark": "경고(No.14) 임계 10 ℃와 폴트 임계가 동일 — 폴트 임계 20 ℃ 상향 검토 중, 확정 요청"
  },
  {
   "no": 31,
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
   "fw_value": "1  (판정 로직 미완 — FaultReg 미설정)",
   "match": "미구현",
   "remark": "[260827 신규] 릴레이 장애 — 통신규약 Fault bit30 대응 항목 신설. 설정값 확정 요청"
  },
  {
   "no": 32,
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
   "no": 33,
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
   "match": "미구현",
   "remark": "SW 미구현 — 적용 여부 확인 요청"
  },
  {
   "no": 34,
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
   "remark": "적용 여부 확인 요청  |  [R10 신규확장] CAN33  |  [260727 대비] 항목 번호 17 → 34 이동"
  },
  {
   "no": 35,
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
   "remark": "적용 여부 확인 요청  |  [R10 신규확장] CAN34  |  [260727 대비] 항목 번호 18 → 35 이동"
  },
  {
   "no": 36,
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
   "no": 37,
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
   "fw_value": "1  (판정 로직 미완 — FaultReg 미설정)",
   "match": "미구현",
   "remark": "[260827 신규] 2nd 릴레이 변경을 통해 기능 반영  ※No.31과 항목명 중복 — 정리 필요"
  },
  {
   "no": 38,
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
   "fw_value": "45000  (판정 로직 미완)",
   "match": "미구현",
   "remark": "[260827 신규] 절연저항(IMD) 장애 — 2nd 릴레이 변경을 통해 기능 반영. 설정값 확정 요청"
  }
 ]
}
```
