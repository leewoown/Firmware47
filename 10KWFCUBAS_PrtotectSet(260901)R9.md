---
name: 10KWFCUBAS_PrtotectSet(260901)R9
description: 현대자동차 10kWFC 2차년도 22S1P NCM 보조배터리 팩 보호설정 R9 — 설정값·시험 방법·트레이스 분석 데이터
project: HDFC 10kW FC 2nd / F28069 Pack BMS
revision: R9
date: 2026-09-01
sources: [10KWFCUBAS_PrtotectSet(260901)R9.xlsx, 10kWFCU_BAP2ndProtocol(20260827)R11.md]
---

# 10KWFCUBAS 보호설정 R9 (260901)

## 1. R8 → R9 변경 (18건)

| No | 항목 | 구분 | 변경 전 (260827 R8) | 변경 후 (260901 R9) |
|---|---|---|---|---|
| 5 | Bsa_WrnUv | 설정값 변경 (논리 오류) | 발생 66.00 V (셀 3.000 V) · 해제 69.30 V | **65.45 V** (셀 2.975 V) · 해제 **68.7 V** |
| 5 | Bsa_WrnUv (해제) | 펌웨어 정합 | 해제 68.7225 V (`=C7+(C7*G7)`) | 해제 **68.7 V** (`=ROUND(C7*(1+G7),1)`) |
| 22 | Bsa_FltUt | 설정값 변경 (논리 오류) | 발생 -25.0 ℃ | **-27.0 ℃** |
| 13 | Bsa_WrnCellUt | 설정값 변경 | 히스율 5 % · 해제 -23.75 ℃ | 히스율 **6 %** · 해제 **-23.5 ℃** |
| 22 | Bsa_FltUt | 조건 정정 | Trigger 조건 「이상」 | Trigger 조건 **「이하」** |
| 23 | Bsa_FltUnbalPwr | 적용 시점 변경 | Shut down(0x04, Rly OPEN) | 261220 이후 반영 |
| 26 | Bsa_FltCellUnbalVlt | 설정값 변경 | 발생 350 mV | 발생 **400 mV** |
| 30 | Bsa_FltRly_Err | 적용 시점 변경 | Shut down(0x04, Rly OPEN) | 261220 이후 반영 |
| 32 | Bsa_FltCellIR_OV | 적용 시점 변경 | Shut down(0x04, Rly OPEN) | 261220 이후 반영 |
| 34 | Bsa_FltOcTime_min | 조건 정정 | 「480A 이상 1분당 2회 발생 시」 | **「480A 이상 1분당 5회 이상 발생 시」** |
| 35 | BPA_FltISOSPI_Err | 항목 재검토 | Release 10 CONT · Effect 0x02 | Release · Effect 공란 (확정 필요) |
| 36 | Bsa_FltIMD_Err | 적용 시점 변경 | Shut down(0x04, Rly OPEN) | 261220 이후 반영 |
| 문서 | M열 시험 방법 | 열 작성 | No.1 만 기재 | 36항목 전체 |
| 문서 | O열 시험 조건 | 열 작성 | 공란 | 36항목 초기 SET 조건 |
| 문서 | P열 트레이스 분석 | 열 신설 | — | 36항목 (5단 구성) |
| 시험 | 온도 8항목 램프 | 시험 조건 변경 | 전 구간 1~2 ℃/s | 판정 임계 ±1 ℃ 구간만 **0.5 ℃/s** |
| 7·13 | 팩·셀 저온 경고 | 펌웨어 반영 예정 | 펌웨어 팩 -25.0/-23.8 · 셀 -20.0/-23.75 | 시트 팩 -20/-19 · 셀 -25/-23.5 |

### 1.1 No.22 조건 정정 근거

팩 저온 차단 -25 ℃ 에 「이상」이면 상온 25 ℃ 에서 조건이 성립해 Run 진입 즉시 차단된다.
하한 계열 전 항목(No.3·5·7·10·13·18·20·25·28)이 「이하」이며, 260830 실측도 냉각 중 -24.8 ℃ 에서 bit22 SET 로 「이하」 동작이 확인되었다. → **E24 = `이하`**

### 1.2 온도 램프 분할 근거

| 램프 | 온도 신호 계단 | ±0.5 ℃ 판정 |
|---|--:|---|
| 2 ℃/s (260830 실시) | 0.60 ℃ | 불가 |
| 1 ℃/s | 0.30 ℃ | 가능 |
| **0.5 ℃/s (R9 적용)** | **0.15 ℃** | 여유 확보 |

0x606 온도 신호 갱신 주기가 0.30 s 이므로 램프 속도가 곧 관측 분해능이 된다.
260830 No.13 은 2 ℃/s 로 계단이 0.60 ℃ 였고, -24.4 다음 값이 -23.7 로 설정 -23.75 가 계단 사이에 들어가 관측 자체가 불가능했다.
판정 임계 ±1 ℃ 구간만 0.5 ℃/s 로 낮추면 항목당 +6 초로 해결된다. 대상 : No.6·7·12·13·21·22·27·28

## 2. 판정 규칙

- 판정 매크로 : `Hyst_On(V,S) = (V) > (S)` 초과 / `Hyst_Off(V,R) = (V) < (R)` 미만
- 전류 부호 : (+) 충전 / (−) 방전, 과전류는 절대값 판정
- 상태 : `BSA_Ste`(0x602) 0=Init / 1=Ready / 2=Run / 3=Warning / 4=Protection
- 경고 : Delay 100 ms, 히스테리시스 자동 해제, 릴레이 Close 유지
- 차단 : Delay 즉시(0 ms), 전원 OFF 또는 `Ifcu_BSAReset`=1(0x450) 전까지 LATCH, 릴레이 OPEN

## 3. Warning 15 항목

| No | 항목 | 발생 | 조건 | Delay | 히스 | 해제 | 조건 | 0x603 |
|--:|---|--:|:--:|--:|--:|--:|:--:|:--:|
| 1 | `Bsa_WrnOC` | 450 [A] | 이상 | 100 | 0.1 | 405 [A] | 미만 | bit 0 |
| 2 | `Bsa_WrnSocH` | 95 [%] | 이상 | 100 | 0.03 | 92.15 [%] | 미만 | bit 1 |
| 3 | `Bsa_WrnSocL` | 5 [%] | 이하 | 100 | 0.05 | 5.25 [%] | 초과 | bit 2 |
| 4 | `Bsa_WrnOv` | 90.86 [V] | 이상 | 100 | 0.03 | 88.1342 [V] | 미만 | bit 3 |
| 5 | `Bsa_WrnUv` | 66 [V] | 이하 | 100 | 0.05 | 69.3 [V] | 초과 | bit 4 |
| 6 | `Bsa_WrnOt` | 47 [ºC] | 이상 | 100 | 0.05 | 44.65 [ºC] | 미만 | bit 5 |
| 7 | `Bsa_WrnUt` | -20 [ºC] | 이하 | 100 | 0.05 | -19 [ºC] | 초과 | bit 6 |
| 8 | `Bsa_WrnUnbalPwr` | 10 Sec | 이상 | 100 | — | 0 Sec | 미만 | bit 7 |
| 9 | `Bsa_WrnCellOv` | 4.15 [V] | 이상 | 100 | 0.005 | 4.12925 [V] | 미만 | bit 8 |
| 10 | `Bsa_WrnCellUv` | 3 [V] | 이하 | 100 | 0.005 | 3.015 [V] | 초과 | bit 9 |
| 11 | `Bsa_WrnCellUnbalV` | 200 [mV] | 이상 | 100 | 0.66667 | 66.66667 [mV] | 미만 | bit 10 |
| 12 | `Bsa_WrnCellOt` | 55 [ºC] | 이상 | 100 | 0.05 | 52.25 [ºC] | 미만 | bit 11 |
| 13 | `Bsa_WrnCellUt` | -25 [ºC] | 이하 | 100 | 0.06 | -23.5 [ºC] | 초과 | bit 12 |
| 14 | `Bsa_WrnCellUnbalTmp` | 10 [ºC] | 이상 | 100 | 0.5 | 5 [ºC] | 미만 | bit 13 |
| 15 | `BPA_WrnVcuCan_Err` | 10 CONT | 이상 (1sec 이상 미수신) | 즉시 = 0 | - | 0 Count | 이하 (1sec 이상 정상 수신) | bit 14 |

## 4. Fault 21 항목

| No | 항목 | 발생 | 조건 | Delay | 해제 | 0x603 | Effect |
|--:|---|--:|:--:|--:|---|:--:|---|
| 16 | `Bsa_FltOc` | 505 [A] | 이상 | 즉시 = 0 | FCU_RESET | bit 16 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 17 | `Bsa_FltctSocH` | 100 [%] | 이상 | 즉시 = 0 | FCU_RESET | bit 17 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 18 | `Bsa_FlttctSocL` | 0 [%] | 이하 | 즉시 = 0 | FCU_RESET | bit 18 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 19 | `Bsa_FltOv` | 91.3 [V] | 이상 | 즉시 = 0 | FCU_RESET | bit 19 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 20 | `Bsa_FltUv` | 58.3 [V] | 이하 | 즉시 = 0 | FCU_RESET | bit 20 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 21 | `Bsa_FltOt` | 52 [ºC] | 이상 | 즉시 = 0 | FCU_RESET | bit 21 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 22 | `Bsa_FltUt` | -25 [ºC] | 이하 | 즉시 = 0 | FCU_RESET | bit 22 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 23 | `Bsa_FltUnbalPwr` | 15 Sec | — | 즉시 = 0 | FCU_RESET | bit 23 | 261220이후 반영 |
| 24 | `Bsa_FltCellOv` | 4.2 [V] | 이상 | 즉시 = 0 | FCU_RESET | bit 24 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 25 | `Bsa_FltCellUv` | 2.65 [V] | 이하 | 즉시 = 0 | FCU_RESET | bit 25 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 26 | `Bsa_FltCellUnbalVlt` | 400 [mV] | 이상 | 즉시 = 0 | FCU_RESET | bit 26 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 27 | `Bsa_FltCellOt` | 60 [ºC] | 이상 | 즉시 = 0 | FCU_RESET | bit 27 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 28 | `Bsa_FltCellUt` | -30 [ºC] | 이하 | 즉시 = 0 | FCU_RESET | bit 28 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 29 | `Bsa_FltCellUnbalTmp` | 20 [ºC] | 이상 | 즉시 = 0 | FCU_RESET | bit 29 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 30 | `Bsa_FltRly_Err` | — — | — | — | FCU_RESET | bit 30 | 261220이후 반영 |
| 31 | `BSA_Prtct_FAULT15` | — — | — | — | FCU_RESET | bit 31 | — |
| 32 | `Bsa_FltCellIR_OV` | 15 mΩ | 이상 | 즉시 = 0 | FCU_RESET | bit 32 | 261220이후 반영 |
| 33 | `Bsa_FltOcTimer` | 10 Sec | 480A 이상 10sec 이상 유지 | 즉시 = 0 | FCU_RESET | bit 33 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 34 | `Bsa_FltOcTime_min` | 5 Count/min | 480A 이상 1분당 5회 이상  발생 시 | 즉시 = 0 | FCU_RESET | bit 34 | Shut down(BPA_Protect_Status=0x04, Rly stat  |
| 35 | `BPA_FltISOSPI_Err` | 210 CONT | 이상 | 즉시 = 0 | FCU_RESET | bit 35 | — |
| 36 | `Bsa_FltIMD_Err` | — — | — | — | FCU_RESET | bit 36 | 261220이후 반영 |

## 5. 260830 실측 결과 (29항목 시험)

| 결과 | 항목 수 | 항목 |
|---|--:|---|
| 합격 | 27 | No.1~7 · 9~12 · 14~22 · 24·25 · 27~29 · 33·34 |
| 재시험 | 2 | **No.13** (램프 분해능 · 펌웨어 반영 후) · **No.26** (350 → 400 mV 임계 상향) |
| 미시험 | 7 | No.8 · 23 · 30 · 31 · 32 · 35 · 36 |

- 전 항목 `BSA_Ste` 천이 및 릴레이 거동 사양 일치
- 폴트 21항목 중 시험분 전건 LATCH 유지 및 `Ifcu_BSAReset` 복귀 확인
- 상세 : `20260831_10kWFC_보호검증시험보고서R8_AI.pptx` (61장)

## 6. 펌웨어 대조 결과 (`SysInclude/parameter.h`, `Product_Version 19`)

**27항목 전수 일치 — 불일치 0건** (260901 검증)

| 구분 | 상수 | 값 | 시트 |
|---|---|--:|:--:|
| 팩 저온 경고 | `C_PackTempUN_Warn` / `_WarnRst` | −20.0 / −19.0 | 일치 |
| 셀 저온 경고 | `C_CellTempUN_Warn` / `_WarnRst` | −25.0 / −23.5 | 일치 |
| 팩 저전압 경고 | `C_PackVoltUN_Warn` / `_WarnRst` | 65.45 / 68.7 | 일치 |
| 팩 과전압 차단 | `C_PackVoltOV_Fault` | 91.3 | 일치 (R9 92.4 미반영, 91.3 유지 결정) |
| 팩 저온 차단 | `C_PackTempUN_Fault` | −27.0 | 일치 |
| 셀 전압편차 차단 | `C_CellVoltDIV_Fault` | 0.40 (400 mV) | 일치 |
| 과전류 횟수 | `C_PackOCEventCount` / `_Window` | 5 / 60000 | 일치 |
| 과전류 시간 | `C_PackOCTimer_Fault` / `Count` | 480.0 / 10000 | 일치 |
| Delay | 경고 100 ms · 폴트 0 ms · UnbalPwr 10000 / 15000 | — | 일치 |

### 6.1 값은 있으나 판정 로직이 없는 항목

| No | 항목 | 펌웨어 상태 | 시트 |
|--:|---|---|---|
| 30 | `Bsa_FltRly_Err` | `C_RleyCount 1` — 임시값, 설정 미확정 | 261220 이후 반영 — 정합 |
| 32 | `Bsa_FltCellIR_OV` | **상수 미정의** | 261220 이후 반영 — 정합 |
| 35 | `BPA_FltISOSPI_Err` | `C_ISOSPICount 210` — **판정 로직 미구현** | Release·Effect 공란 — 값은 일치하나 검증 불가 |
| 36 | `Bsa_FltIMD_Err` | `C_IOSresistanceFault 45000` — 임시값 | 261220 이후 반영 — 정합 |

## 7. 설정값 미확정 항목

| No | 항목 | 상태 | 제안값 |
|--:|---|---|---|
| 30 | `Bsa_FltRly_Err` | 261220 이후 반영 | 지령-피드백 불일치 연속 500 ms, 해제 없음(RESET 전용) |
| 31 | `BSA_Prtct_FAULT15` | 예약 슬롯 | 미사용 — bit31 상시 0 |
| 32 | `Bsa_FltCellIR_OV` | 261220 이후 반영 | 15 mΩ 유지 (DCIR = ΔV/ΔI) |
| 35 | `BPA_FltISOSPI_Err` | Release·Effect 공란 | 해제 10 CONT 이하, Effect 「BPA_Protect_Status=0x02, Rly stat=OPEN」 |
| 36 | `Bsa_FltIMD_Err` | 261220 이후 반영 | 45 kΩ 미만 연속 500 ms |

## 8. 시험 방법 요약 (M·O·P 열)

공통 프리앰블 — 초기 SET → CAN 전송 → READY 10 s → `Ifcu_BSARun`=1(0x450) → Run 10 s → 항목별 인가 → 발생/해제 판정

| 물리량 | 표준 램프 | 판정 구간 |
|---|---|---|
| 셀 전압 | 10 mV/s (팩 20 mV/s) | 전 구간 |
| 온도 | 2 ℃/s | **임계 ±1 ℃ 는 0.5 ℃/s** |
| 전류 | 스텝(≤100 ms) | — |
| SOC | 충·방전 전류로 제어 | — |

- 발생·해제 각 30 s 유지 · **전 항목 10분 이내** (최장 No.2 약 4분 30초)
- P열 트레이스 분석은 [필수 로깅] · [산출 지표] · [판정 기준] · [보고서 그래프] · [260830 실측] 5단 구성

## 9. R9 논리 검증 결과 (260901 조치 반영)

엑셀 `02 논리검증(R9)` 시트와 동일 내용.

### 9.1 보호 계층 순서 (셀 전압 환산 · 균일 인가 기준)

| 구분 | 계층 순서 | 판정 |
|---|---|---|
| 충전 상한 | 팩 경고 4.1300 → **셀 경고 4.1500 = 팩 차단 4.1500** → 셀 차단 4.2000 | △ 현행 유지(260901 결정) — 경고와 차단 동시 발생, `bit8`·`bit19` 동시 SET 기록으로 판정 |
| 방전 하한 | 셀 경고 3.0000 → **팩 경고 2.9750** → 셀 차단 2.6500 = 팩 차단 2.6500 | 해소 — 팩 경고 66.00 → 65.45 V |
| 고온 | 팩 경고 47 → 팩 차단 52 → 셀 경고 55 → 셀 차단 60 | △ 조건부 유지 — 센서 물리 분리, [치환] 인가로 회피 |
| 저온 | 팩 경고 -20 → 셀 경고 -25 → **팩 차단 -27** → 셀 차단 -30 | 해소 — 팩 차단 -25 → -27 ℃ |
| SOC | 경고 5 / 95 % → 차단 0 / 100 % | 정상 |
| 전류 | 경고 450 → 타이머 480(10 s) → 횟수 480(5회/min) → 차단 505 A | 정상 |
| 전압 편차 | 경고 200 → 차단 400 mV | 정상 |
| 온도 편차 | 경고 10 → 차단 20 ℃ | 정상 |

### 9.2 임계 충돌 조치 결과

| No | 항목 | 변경 전 | 충돌 상대 | 변경 후 | 사유 |
|--:|---|--:|---|--:|---|
| 19 | `Bsa_FltOv` | 91.30 V (셀 4.150) | No.9 셀 경고 4.150 | **91.30 V 유지** | 260901 검토 후 현행 유지 결정. 대안 92.40 V(= 셀 차단 × 22S)는 보류 |
| 5 | `Bsa_WrnUv` | 66.00 V (셀 3.000) | No.10 셀 경고 3.000 | **65.45 V** `=(C12-0.025)*22` | 셀 경고와 동일 → 동시 발생. 셀 −25 mV × 22S |
| 22 | `Bsa_FltUt` | −25.0 ℃ | No.13 셀 경고 −25.0 | **−27.0 ℃** `=-27` | 셀 경고와 동일 → 경고·차단 동시. 2~3 ℃ 분리 |
| 22 | `Bsa_FltUt` (조건) | 이상 | — | **이하** (E24) | 「이상」이면 상온 25 ℃에서 조건 성립 |
| 20 | `Bsa_FltUv` | 58.30 V | No.25 셀 차단 2.650 | 유지 | P56 사양 하한 — 백업 계층, 동시 발생 정상 |
| 21 | `Bsa_FltOt` | 52.0 ℃ | No.12 셀 경고 55.0 | 유지(조건부) | 센서 물리 분리 — [치환] 인가로 회피 |

### 9.3 모사장치 단독 검출 가능성 (조치 후)

| 구분 | 항목 | 여유(margin) | 판정 방법 |
|---|---|---|---|
| 가능 | No.9 · 24 셀 과전압 | 편차 160 mV · TV 여유 2.7 / 2.0 V | 단독 판정 |
| 가능 | No.4 팩 과전압 경고 | 셀 경고까지 **10 mV** | 인가 정밀도 ±5 mV 필요 |
| 가능 | No.26 셀 편차 차단 | 편차 410 mV · 셀 경고까지 40 mV | 단독 판정 |
| 가능 | **No.22 팩 저온 차단** | 셀 차단까지 3 ℃ · 셀 경고와 2 ℃ 분리 | 단독 판정 — **−27 ℃ 반영으로 해소** |
| 순서 | **No.5 팩 저전압 경고** | 셀 경고 대비 −25 mV | bit9 → bit4 순서 판정 — **65.45 V 반영으로 동시 → 순서 전환** |
| 동시 | No.19 팩 과전압 차단 | **셀 경고와 동일** | `bit8` · `bit19` 동시 SET 기록 — △ 경고 예고 기능 없음 |
| 동시 | No.20 팩 저전압 차단 | 셀 차단과 동일 | bit20 · bit25 동시 SET 기록 (사양 한계, 정상) |
| 치환 | No.12 · 13 · 27 · 28 셀 온도 | 평균 < 팩 임계 · 편차 < 경고 임계 | Cell#1 가변 + 나머지 셀 추종 인가 필수 |
| 제외 | No.8 · 23 · 30 · 31 · 32 · 35 · 36 | — | 펌웨어 차단 / 261220 이후 / 설정 미확정 |

### 9.4 조치 순서

| 순서 | 구분 | 대상 | 내용 |
|:--:|---|---|---|
| ① | 엑셀 **완료** | E24 · C7 · H7 · C24 | 조건 「이하」 · 65.45 / 68.7 V · −27.0 ℃  (C21 = 91.30 V 현행 유지) |
| ② | MD **완료** | 본 문서 | 설정값 · 시험 방법 · 논리 검증 반영 |
| ③ | 펌웨어 **완료** | `parameter.h` (`Product_Version 19`) | 6항 — 27항목 전수 일치 확인 |
| ④ | 펌웨어 **완료** | 팩 전압·온도 상수 | `C_PackVoltUN_Warn` 65.45 / `_WarnRst` 68.7 · `C_PackTempUN_Fault` −27.0 · `C_PackVoltOV_Fault` 91.3 |
| ⑤ | 시험 도구 | 온도 8항목 | 판정 임계 ±1 ℃ 구간 램프 0.5 ℃/s |
| ⑥ | 재시험 | No.5 · 13 · 22 · 26 | 설정 변경 5건 + 램프 변경 항목 |
| ⑦ | 보고서 | `20260831_...보고서R8_AI.pptx` | 재시험 결과 반영 후 R9 판번 |
