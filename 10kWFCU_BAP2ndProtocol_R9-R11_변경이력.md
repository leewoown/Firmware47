# 10kW FCU ↔ BAP 통신규약 변경이력 (R9 → R11)

- 기준 파일: `10kWFCU_BAP2ndProtocol20260809R9.xlsx` (2026-08-09)
- 변경 파일: `10kWFCU_BAP2ndProtocol20260827R11.xlsx` (2026-08-27)
- 비교 범위: `CAN 통신규약(NCM)` 시트 (R9의 LFP 시트는 R11에서 제외됨)
- 요약: **동일 43건 / 필드 변경 13건 / 명칭 변경 17건 / 신규 추가 4건 / 삭제 0건**

CAN 구성: Standard 11bit, LSB(Intel). 삭제된 신호는 없으며, 모든 명칭 변경은 동일 ID·비트 위치에서 이름만 바뀐 것임.

---

## 1. 시트 구성 변경

| 항목 | R9 | R11 |
|---|---|---|
| CAN 통신규약(NCM) | 있음 | 있음 (본 비교 대상) |
| CAN 통신규약(LFP) | 있음 | **삭제** |
| 배터리 제작 규격서(NCM) | 있음 | 있음 |

## 2. 0x600 BSA1 — 제품 정보

| 신호 | 변경 내용 |
|---|---|
| BSA_Type | Value Table 교체: `0x0=EV Bus / 0x1=Ship / 0x2=Subway / 0x3=E-Mobility` → `0x0=Development / 0x1=Production` |
| BSA_SW_Ver | Factor 0.1 → **1**, Min 1 → 0, Max 99 → **255** (8bit 표현범위 정합화) |
| BSA_Normal_Volt | 초기값 80.96 → **80.9** (Factor 0.1 정밀도 정합화) |

## 3. 0x601 BSA2 — 팩 전압/전류/SOC/SOH

| 신호 | 변경 내용 |
|---|---|
| BSA_Volt_Total | 초기값 80.96 → **80.9** |

(BSA_Curr_Total, BSA_SOC, BSA_SOH 변경 없음)

## 4. 0x602 BSA3 — 상태/릴레이

| 신호 | 변경 내용 |
|---|---|
| BSA_Balance | Comment 의미 정정: `0: Balance On, 1: Off` → `0: BalanceOff, 1: BalanceOn` (Value Table 0=Stop/1=Run과 정합) |

설계 메모: 직결 구조로 `BSA_Neg_Rly`는 항상 1(Close) 고정 보고.

## 5. 0x603 BSA4 — 경고/보호 비트

### 5.1 보호비트 명칭 체계 변경 (bit16~32, 17건 — 위치·기능 동일, 이름만 변경)

| Bit | R9 명칭 | R11 명칭 |
|---|---|---|
| 16 | BSA_Prtct_OC | BSA_Prtct_PackOC |
| 17 | BSA_Prtct_SOC_OV | BSA_Prtct_PackSOC_OV |
| 18 | BSA_Prtct_SOC_Un | BSA_Prtct_PackSOC_UN |
| 19 | BSA_Prtct_OV | BSA_Prtct_PackVolt_OV |
| 20 | BSA_Prtct_UV | BSA_Prtct_PackVolt_UN |
| 21 | BSA_Prtct_OT | BSA_Prtct_PackTemp_OV |
| 22 | BSA_Prtct_UT | BSA_Prtct_PackTemp_UN |
| 23 | BSA_Prtct_UnbalancePower | BSA_Prtct_PackUnPWR_BL |
| 24 | BSA_Prtct_Cell_OV | BSA_Prtct_CellVolt_OV |
| 25 | BSA_Prtct_Cell_UV | BSA_Prtct_CellVolt_UN |
| 26 | BSA_Prtct_Cell_UnbalV | BSA_Prtct_CellVolt_BL |
| 27 | BSA_Prtct_Cell_OT | BSA_Prtct_CellTemp_OV |
| 28 | BSA_Prtct_Cell_UT | BSA_Prtct_CellTemp_UN |
| 29 | BSA_Prtct_Cell_UnbalT | BSA_Prtct_CellTemp_BL |
| 30 | BSA_Prtct_Rly_Err | BSA_Prtct_PackRLY_ERR |
| 31 | Bsa_PrtctCanTmOut | **BSA_Prtct_FAULT15** — 기능 변경: CAN 타임아웃 보호 → 예약비트화(보호 판정 제외, 260827) |
| 32 | Bsa_Prtct_CellIR_OV | BSA_Prtct_CellIR_OV (접두 표기 통일) |

펌웨어 `SystemProtect_BIT` 구조체 명칭과 1:1 매칭되도록 Pack/Cell 구분 체계로 정비. Comment에 구조체 멤버·CAN 비트·int 인덱스 병기.

### 5.2 신규 추가 (bit33~36, 규약 밖 확장 4건)

| Bit | 신호 | 내용 |
|---|---|---|
| 33 | BSA_Prtct_PackOcTime_Err | 과전류 시간 보호 |
| 34 | BSA_Prtct_PrtcOcEvent_Err | 과전류 횟수 보호 |
| 35 | BSA_Prtct_PackISO_ERR | ISOSPI 통신 에러 |
| 36 | BSA_Prtct_PackIMD_ERR | IMD 절연저항 에러 |

경고비트: bit14 `BSA_Wn_FCUCAN_ERR` — VER15에서 적용 확정(R9와 정의 동일).

## 6. 0x604 BSA5 — 충·방전 출력 리미트 (재정의, FCU 파서 영향 大)

| 항목 | R9 | R11 |
|---|---|---|
| Value Type | Unsigned | **Signed** |
| 단위 | kW | **A** |
| 충전 범위 | 10 ~ 20 | 10 ~ 500 |
| 방전 범위 | 10 ~ 20 | **-500 ~ -10** (음수 방전 표현) |
| 초기값 | 2.5 | 충전 10 / 방전 -10 |

## 7. 0x608 BSA9 — 통신 감시 카운터 (비트 레이아웃 변경)

| 신호 | R9 StartBit | R11 StartBit | 비고 |
|---|---|---|---|
| BMS_CANRxCount | 0 | 0 | 변경 없음 |
| BMS_CT_RxCount | 8 | **16** | |
| BMS_VCU_RxCount | 16 | **32** | |
| BMS_Slave1_Err | 24 | **48** | VCU/BSA R·T 지정 추가 |
| BMS_Slave2_Err | 32 | **56** | VCU/BSA R·T 지정 추가 |

## 8. 변경 없음

0x450 IFCU_BSA(Run/Reset), 0x601 전류·SOC·SOH, 0x602 상태·릴레이(Balance Comment 제외), 0x603 경고비트(bit0~14), 0x605 셀 전압, 0x606 셀 온도, 0x607 셀 내부저항, 0x609/0x60A 디버깅.

---

## 검증 결과 (R11 최종본 기준)

- 비트 중첩, DLC 초과, ID·신호명 중복: 없음
- 잔여 참고사항: `BSA_Type` 초기값 3은 Value Table(0x0/0x1) 밖, 0x604 PL 초기값(±10)과 SW_Ver 초기값 0의 Min/Max 관계는 운용상 확인 필요, 0x607·0x608의 `BMS_`/`BSA_` 접두 혼용
