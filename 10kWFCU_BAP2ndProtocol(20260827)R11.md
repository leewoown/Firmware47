# 10kWFCU BAP2nd Protocol — R11 (2026-08-27)

> **문서 버전:** R11 (R10 기반, 0x603 Protection 확장 확정 · 펌웨어 `Product_Version 16` 대조 완료)  
> **양식:** 10kWFCU_BAP2ndProtocol R32 준용  
> **물리계층:** CAN 2.0A Standard(11-bit ID) · LSB=intel · DLC 8 byte  
> **대조 펌웨어:** `D:/47 HDFC10kWNCM_PackBMSR1/F28069PackBMS` — `main.c` · `C2806Xinclude/DSP28x_Project.h` (Product_Version 16, 260827 반영)  
> **R10 → R11 변경:** 0x603 Protection CAN31 = `BSA_Prtct_FAULT15`(예약비트) 확정, CAN35 `PackISO_ERR` · CAN36 `PackIMD_ERR` 추가, 0x604 단위 kW → **A**(전류 한계) 변경, 0x608 신호 8bit 분할 및 Slave1/2_Err 추가

---

## SW 대조 요약

| 구분 | 건수 | 상태 |
|:--|--:|:--|
| 0x603 보호 비트 (Alarm 15 + Protection 22) | 37 | **전수 일치** |
| 펌웨어 ↔ 규약 불일치 | 7 | 조치 필요 |
| 규약 문서 자체 결함 | 6 | 규약 정정 필요 |
| 미송신 · 미사용 | 2 | 정리 대상 |

상세는 문서 하단 [SW 대조 상세](#sw-대조-상세) 참조.

---

## CAN 통신규약 (NCM)

| ECU | Message | ID | DLC | Cycle[ms] | Signal | Len[bit] | StartBit | ByteOrder | ValueType | Initial | Factor | Offset | Min | Max | Unit | Value Table | Comment | Multiplexing | VCU | BSA | 비고 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| IFCU | IFCU_BSA | 0x450 | 8 | 100 | Ifcu_BSARun | 1 | 8 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=stop;<br>0x1=run; | 0:BSA OFF, 1: BSA On |  | T | R |  |
| IFCU | IFCU_BSA | 0x450 | 8 | 100 | Ifcu_BSAReset | 1 | 9 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=No Action<br>0x1=Reset; | 0:BSA No Action, 1: BSA Fault Reset |  | T | R |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Type | 8 | 0 | LSB | Unsigned | 3 | 1 | 0 | 0 | 10 |  | 0x0=D;<br>0x1=P; | 0x0=Development <br>0x0=Production |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_SW_Ver | 8 | 8 | LSB | Unsigned | 0 | 1 | 0 | 1 | 99 | VER |  | Software Version |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Normal_Volt | 16 | 16 | LSB | Unsigned | 80.96 | 0.1 | 0 | 0 | 100 | V |  | 80V Battery pack nominal voltage(공칭전압) |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Capacity | 16 | 32 | LSB | Unsigned | 56 | 0.1 | 0 | 30 | 60 | Ah |  | 80V Battery pack battery capacity |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Serial | 8 | 48 | LSB | Unsigned | 22 | 1 | 0 | 0 | 200 | S |  | 80V Battery pack CELL configuration |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Parallel | 8 | 56 | LSB | Unsigned | 1 | 1 | 0 | 0 | 10 | P |  | 80V Battery pack CELL configuration |  | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_Volt_Total | 16 | 0 | LSB | Unsigned | 80.96 | 0.1 | 0 | 40 | 120 | V |  | 80V Battery Pack Voltage |  | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_Curr_Total | 16 | 16 | LSB | Signed | 0 | 0.1 | 0 | -700 | 700 | A |  | 80V Battery Pack Current |  | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_SOC | 16 | 32 | LSB | Unsigned | 50 | 0.1 | 0 | 0 | 120 | % |  | 80V Battery Pack SOC |  | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_SOH | 16 | 48 | LSB | Unsigned | 100 | 0.1 | 0 | 0 | 100 | % |  | 80V Battery Pack SOH |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Ste | 3 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 4 |  | 0x0=Init;<br>0x1=Ready;<br>0x2=Run;<br>0x3=Warning;<br>0x4=Protection; | BMS State Protection level <br>0 : Init, 1: Ready, 2 : Run, 3 : Warning, 4 : Protection |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Balance | 1 | 3 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Stop;<br>0x1=Run; | 0 : Balance On, 1 : Balance Off |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Neg_Rly | 1 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Open;<br>0x1=Close; | 0 : Open, 1 : Close |  | R | T | 직결 구조로 Neg_Rly는 항상 1(Close) 고정 보고 |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Pos_Rly | 1 | 17 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Open;<br>0x1=Close; | 0 : Open, 1 : Close |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_PreChar_Rly | 1 | 18 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Open;<br>0x1=Close; | 0 : Open, 1 : Close |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Ah | 16 | 32 | LSB | Signed | 0 | 0.1 | 0 | -56 | 56 | Ah |  | BSA의 실시간 Ah 적산값으로, SOC 검증 및 에너지 사용량 확인용 |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OC | 1 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_SOC_OV | 1 | 1 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_SOC_Un | 1 | 2 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OV | 1 | 3 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UV | 1 | 4 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OT | 1 | 5 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UT | 1 | 6 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UnbalancePower | 1 | 7 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_OV | 1 | 8 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UV | 1 | 9 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UnbalV | 1 | 10 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_OT | 1 | 11 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UT | 1 | 12 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UnbalT | 1 | 13 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_FCUCAN_ERR | 1 | 14 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T | VER15, 적용함 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackOC | 1 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackOC (CAN16/int0) — 과전류 보호 |  | R | T | 명칭 일치(R9 _OC) |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackSOC_OV | 1 | 17 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackSOC_OV (CAN17/int1) — SOC 과충전 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackSOC_UN | 1 | 18 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackSOC_UN (CAN18/int2) — SOC 과방전 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackVolt_OV | 1 | 19 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackVolt_OV (CAN19/int3) — 팩 과전압 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackVolt_UN | 1 | 20 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackVolt_UN (CAN20/int4) — 팩 저전압 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackTemp_OV | 1 | 21 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackTemp_OV (CAN21/int5) — 팩 과온도 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackTemp_UN | 1 | 22 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackTemp_UN (CAN22/int6) — 팩 저온도 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackUnPWR_BL | 1 | 23 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackUnPWR_BL (CAN23/int7) — 출력 불균형 보호 |  | R | T | 명칭 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellVolt_OV | 1 | 24 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellVolt_OV (CAN24/int8) — 셀 과전압 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellVolt_UN | 1 | 25 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellVolt_UN (CAN25/int9) — 셀 저전압 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellVolt_BL | 1 | 26 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellVolt_BL (CAN26/int10) — 셀 전압편차 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellTemp_OV | 1 | 27 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellTemp_OV (CAN27/int11) — 셀 과온도 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellTemp_UN | 1 | 28 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellTemp_UN (CAN28/int12) — 셀 저온도 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellTemp_BL | 1 | 29 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellTemp_BL (CAN29/int13) — 셀 온도편차 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackRLY_ERR | 1 | 30 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackRLY_ERR (CAN30/int14) — 릴레이 에러 보호 |  | R | T | 명칭 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_FAULT15 | 1 | 31 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Reserved; | SystemProtect_BIT.FAULT15 (CAN31/int15) — CAN31 예약비트 (보호 판정 제외) |  | R | T | 의도적 불일치(260827 예약비트화, R9 CanTmOut) |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellIR_OV | 1 | 32 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellIR_OV (CAN32/int16) — 셀 내부저항 과대 보호 |  | R | T | 명칭 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackOcTime_Err | 1 | 33 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackOcTime_Err (CAN33/int17) — 과전류 시간 보호 |  | R | T | 규약 밖 확장 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PrtcOcEvent_Err | 1 | 34 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PrtcOcEvent_Err (CAN34/int18) — 과전류 횟수 보호 |  | R | T | 규약 밖 확장 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackISO_ERR | 1 | 35 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackISO_ERR (CAN35/int19) — ISOSPI 통신 에러 |  | R | T | 규약 밖 확장 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackIMD_ERR | 1 | 36 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackIMD_ERR (CAN36/int20) — IMD 절연저항 에러 |  | R | T | 규약 밖 확장 |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Charge_Cont_PL | 16 | 0 | LSB | Signed | 0 | 0.1 | 0 | 10 | 500 | A |  | 80V BATTERY Pack Charge continuity out Power limit |  | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Discharge_Cont_PL | 16 | 16 | LSB | Signed | 0 | 0.1 | 0 | 10 | .-500 | A |  | 80V BATTERY Pack Discharge continuity out Power limit |  | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Charge_Peak_PL | 16 | 32 | LSB | Signed | 0 | 0.1 | 0 | 10 | 500 | A |  | 80V BATTERY Pack Charge Peak out Power limit |  | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Discharge_Peak_PL | 16 | 48 | LSB | Signed | 0 | 0.1 | 0 | 10 | .-500 | A |  | 80V BATTERY Pack Discharge Peak out Power limit |  | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_MaxV | 16 | 0 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 80V BATTERY Pack 배터리셀_최대전압 |  | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_MinV | 16 | 16 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 80V BATTERY Pack 배터리셀_최저전압 |  | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_AVGV | 16 | 32 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 80V BATTERY Pack 배터리셀_평균전압 |  | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_DeviV | 16 | 48 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1000 | mV |  | 80V BATTERY Pack 배터리셀_전압편차 |  | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_MaxT | 16 | 0 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 80V BATTERY Pack 배터리셀_최대온도 |  | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_MinT | 16 | 16 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 80V BATTERY Pack 배터리셀_최저온도 |  | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_AVGT | 16 | 32 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 80V BATTERY Pack 배터리셀_평균온도 |  | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_DeviT | 16 | 48 | LSB | Unsigned | 0 | 0.1 | 0 | 0 | 10 | ℃ |  | 80V BATTERY Pack 배터리셀_온도편차 |  | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BMS_Cell_MaxIR | 16 | 0 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed |  | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BMS_Cell_MinIR | 16 | 16 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed |  | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BMS_Cell_AVGIR | 16 | 32 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed |  | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BSA_Cell_DeviIR | 16 | 48 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed |  | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_CANRxCount | 8 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | BMS 패킷 수신 정상 여부 감시 ;수신 시 1씩 증가;200 도달 시 0 리셋 |  | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_CT_RxCount | 8 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | 전류 센서 수신 정상 여부 감시 ;수신 시 1씩 증가;200 도달 시 0 리셋 |  | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_VCU_RxCount | 8 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | IFCU 수신 정상 여부 감시 ;수신 시 1씩 증가;200 도달 시 0 리셋 |  | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_Slave1_Err | 8 | 48 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | SalveBatIC_ERR : ISOSPIERR 1씩 증가 |  |  |  |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_Slave2_Err | 8 | 64 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | SalveBatIC_ERR : ISOSPIERR 1씩 증가 |  |  |  |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BMS_Debugging1 | 64 | 0 | LSB | Unsigned |  |  |  |  |  |  |  | Undisclosed |  | R | T |  |
| BSA | BSA11 | 0x60A | 8 | 100 | BMS_Debugging2 | 64 | 0 | LSB | Unsigned |  |  |  |  |  |  |  | Undisclosed |  | R | T |  |

---

## CAN 통신규약 (LFP)  ※ LFP는 변경 없음(펌웨어 NCM 기준)

| ECU | Message | ID | DLC | Cycle[ms] | Signal | Len[bit] | StartBit | ByteOrder | ValueType | Initial | Factor | Offset | Min | Max | Unit | Value Table | Comment | Multiplexing | VCU | BSA | 비고 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| IFCU | IFCU_BSA | 0x450 | 8 | 100 | Ifcu_BSARun | 1 | 8 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=stop;<br>0x1=run; | 0:BSA OFF, 1: BSA On |  | T | R |  |
| IFCU | IFCU_BSA | 0x450 | 8 | 100 | Ifcu_BSAReset | 1 | 9 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=No Action<br>0x1=Reset; | 0:BSA No Action, 1: BSA Fault Reset |  | T | R |  |
| HMI | NVR_SET | 0X400 | 8 | 100 | NVR_SETUP | 1 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=No Action<br>0x1=Set |  |  |  |  |  |
| HMI | NVR_SET | 0X400 | 8 | 100 | NVR_INITSOC | 16 | 16 | LSB | Unsigned | 50 | 0.1 | 0 | 0 | 120 | % |  |  |  |  |  |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Type | 8 | 0 | LSB | Unsigned | 3 | 1 | 0 | 0 | 10 |  | 0x0=EV Bus;<br>0x1=Ship;<br>0x2=Subway;<br>0x3=E-Mobility; | 00:EV BUS, 01:Ship, 02:Subway, 03:E-Mobility |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_SW_Ver | 8 | 8 | LSB | Unsigned | 0 | 0.1 | 0 | 1 | 99 | VER |  | Software Version |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Normal_Volt | 16 | 16 | LSB | Unsigned | 76.8 | 0.1 | 0 | 0 | 100 | V |  | 80V Battery pack nominal voltage(공칭전압) |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Capacity | 16 | 32 | LSB | Unsigned | 40 | 0.1 | 0 | 30 | 60 | Ah |  | 80V Battery pack battery capacity |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Serial | 8 | 48 | LSB | Unsigned | 24 | 1 | 0 | 0 | 200 | S |  | 80V Battery pack CELL configuration |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Parallel | 8 | 56 | LSB | Unsigned | 1 | 1 | 0 | 0 | 10 | P |  | 80V Battery pack CELL configuration |  | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_Volt_Total | 16 | 0 | LSB | Unsigned | 76.8 | 0.1 | 0 | 40 | 120 | V |  | 80V Battery Pack Voltage |  | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_Curr_Total | 16 | 16 | LSB | Signed | 0 | 0.1 | 0 | -700 | 700 | A |  | 80V Battery Pack Current |  | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_SOC | 16 | 32 | LSB | Unsigned | 50 | 0.1 | 0 | 0 | 120 | % |  | 80V Battery Pack SOC |  | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_SOH | 16 | 48 | LSB | Unsigned | 100 | 0.1 | 0 | 0 | 100 | % |  | 80V Battery Pack SOH |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Ste | 3 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 4 |  | 0x0=Init;<br>0x1=Ready;<br>0x2=Run;<br>0x3=Warning;<br>0x4=Protection; | BMS State Protection level <br>0 : Init, 1: Ready, 2 : Run, 3 : Warning, 4 : Protection |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Balance | 1 | 3 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Stop;<br>0x1=Run; | 0 : Balance On, 1 : Balance Off |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Neg_Rly | 1 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Open;<br>0x1=Close; | 0 : Open, 1 : Close |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Pos_Rly | 1 | 17 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Open;<br>0x1=Close; | 0 : Open, 1 : Close |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_PreChar_Rly | 1 | 18 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Open;<br>0x1=Close; | 0 : Open, 1 : Close |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Ah | 16 | 32 | LSB | Signed | 0 | 0.1 | 0 | -40 | 40 | Ah |  | SOC 전류적산 확인 |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OC | 1 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_SOC_OV | 1 | 1 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_SOC_Un | 1 | 2 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OV | 1 | 3 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UV | 1 | 4 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OT | 1 | 5 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UT | 1 | 6 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UnbalancePower | 1 | 7 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_OV | 1 | 8 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UV | 1 | 9 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UnbalV | 1 | 10 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_OT | 1 | 11 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UT | 1 | 12 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UnbalT | 1 | 13 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_OC | 1 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_SOC_OV | 1 | 17 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_SOC_Un | 1 | 18 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_OV | 1 | 19 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_UV | 1 | 20 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_OT | 1 | 21 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_UT | 1 | 22 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_UnbalancePower | 1 | 23 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Cell_OV | 1 | 24 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Cell_UV | 1 | 25 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Cell_UnbalV | 1 | 26 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Cell_OT | 1 | 27 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Cell_UT | 1 | 28 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Cell_UnbalT | 1 | 29 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Rly_Err | 1 | 30 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | Bsa_PrtctCanTmOut | 1 | 31 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | Bsa_Prtct_CellIR_OV | 1 | 32 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection |  | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Charge_Cont_PL | 16 | 0 | LSB | Unsigned | 2.5 | 0.1 | 0 | 10 | 20 | kW |  | 80V BATTERY Pack Charge continuity out Power limit |  | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Discharge_Cont_PL | 16 | 16 | LSB | Unsigned | 2.5 | 0.1 | 0 | 10 | 20 | kW |  | 80V BATTERY Pack Discharge continuity out Power limit |  | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Charge_Peak_PL | 16 | 32 | LSB | Unsigned | 2.5 | 0.1 | 0 | 10 | 20 | kW |  | 80V BATTERY Pack Charge Peak out Power limit |  | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Discharge_Peak_PL | 16 | 48 | LSB | Unsigned | 2.5 | 0.1 | 0 | 10 | 20 | kW |  | 80V BATTERY Pack Discharge Peak out Power limit |  | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_MaxV | 16 | 0 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 80V BATTERY Pack 배터리셀_최대전압 |  | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_MinV | 16 | 16 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 80V BATTERY Pack 배터리셀_최저전압 |  | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_AVGV | 16 | 32 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 80V BATTERY Pack 배터리셀_평균전압 |  | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_DeviV | 16 | 48 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1000 | mV |  | 80V BATTERY Pack 배터리셀_전압편차 |  | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_MaxT | 16 | 0 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 80V BATTERY Pack 배터리셀_최대온도 |  | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_MaxT | 16 | 16 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 80V BATTERY Pack 배터리셀_최저온도 |  | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_AVGT | 16 | 32 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 80V BATTERY Pack 배터리셀_평균온도 |  | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_DeviT | 16 | 48 | LSB | Unsigned | 0 | 0.1 | 0 | 0 | 10 | ℃ |  | 80V BATTERY Pack 배터리셀_온도편차 |  | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BMS_Cell_MaxIR | 16 | 0 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed |  | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BMS_Cell_MinIR | 16 | 16 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed |  | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BMS_Cell_AVGIR | 16 | 32 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed |  | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BSA_Cell_DeviIR | 16 | 48 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed |  | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_CANRxCount | 16 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | Undisclosed |  | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_CT_RxCount | 16 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | Undisclosed |  | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_VCU_RxCount | 16 | 32 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | Undisclosed |  | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BMS_Debugging1 | 64 | 0 | LSB | Unsigned |  |  |  |  |  |  |  | Undisclosed |  | R | T |  |
| BSA | BSA11 | 0x60A | 8 | 100 | BMS_Debugging2 | 64 | 0 | LSB | Unsigned |  |  |  |  |  |  |  | Undisclosed |  | R | T |  |

---

## SW 대조 상세

### 1. 0x603 보호 비트 — 전수 일치 ✔

| CAN bit | 규약 R11 신호 | 펌웨어 구조체 · 멤버 | 판정 |
|:--:|:--|:--|:--:|
| 0 | `BSA_Wn_OC` | `SystemAlarm_BIT.PackOC` (bit0) | 일치 |
| 1 | `BSA_Wn_SOC_OV` | `SystemAlarm_BIT.PackSOC_OV` (bit1) | 일치 |
| 2 | `BSA_Wn_SOC_Un` | `SystemAlarm_BIT.PackSOC_UN` (bit2) | 일치 |
| 3 | `BSA_Wn_OV` | `SystemAlarm_BIT.PackVolt_OV` (bit3) | 일치 |
| 4 | `BSA_Wn_UV` | `SystemAlarm_BIT.PackVolt_UN` (bit4) | 일치 |
| 5 | `BSA_Wn_OT` | `SystemAlarm_BIT.PackTemp_OV` (bit5) | 일치 |
| 6 | `BSA_Wn_UT` | `SystemAlarm_BIT.PackTemp_UN` (bit6) | 일치 |
| 7 | `BSA_Wn_UnbalancePower` | `SystemAlarm_BIT.PackUnPWR_BL` (bit7) | 일치 |
| 8 | `BSA_Wn_Cell_OV` | `SystemAlarm_BIT.CellVolt_OV` (bit8) | 일치 |
| 9 | `BSA_Wn_Cell_UV` | `SystemAlarm_BIT.CellVolt_UN` (bit9) | 일치 |
| 10 | `BSA_Wn_Cell_UnbalV` | `SystemAlarm_BIT.CellVolt_BL` (bit10) | 일치 |
| 11 | `BSA_Wn_Cell_OT` | `SystemAlarm_BIT.CellTemp_OV` (bit11) | 일치 |
| 12 | `BSA_Wn_Cell_UT` | `SystemAlarm_BIT.CellTemp_UN` (bit12) | 일치 |
| 13 | `BSA_Wn_Cell_UnbalT` | `SystemAlarm_BIT.CellTemp_BL` (bit13) | 일치 |
| 14 | `BSA_Wn_FCUCAN_ERR` | `SystemAlarm_BIT.PackFcu_CANErr` (bit14) | 일치 |
| 15 | (미정의) | `SystemAlarm_BIT.Alarm15` (bit15) | 예약 일치 |
| 16 | `BSA_Prtct_PackOC` | `SystemFault_BIT.PackOC` (bit0) | 일치 |
| 17 | `BSA_Prtct_PackSOC_OV` | `SystemFault_BIT.PackSOC_OV` (bit1) | 일치 |
| 18 | `BSA_Prtct_PackSOC_UN` | `SystemFault_BIT.PackSOC_UN` (bit2) | 일치 |
| 19 | `BSA_Prtct_PackVolt_OV` | `SystemFault_BIT.PackVolt_OV` (bit3) | 일치 |
| 20 | `BSA_Prtct_PackVolt_UN` | `SystemFault_BIT.PackVolt_UN` (bit4) | 일치 |
| 21 | `BSA_Prtct_PackTemp_OV` | `SystemFault_BIT.PackTemp_OV` (bit5) | 일치 |
| 22 | `BSA_Prtct_PackTemp_UN` | `SystemFault_BIT.PackTemp_UN` (bit6) | 일치 |
| 23 | `BSA_Prtct_PackUnPWR_BL` | `SystemFault_BIT.PackUnPWR_BL` (bit7) | 일치 |
| 24 | `BSA_Prtct_CellVolt_OV` | `SystemFault_BIT.CellVolt_OV` (bit8) | 일치 |
| 25 | `BSA_Prtct_CellVolt_UN` | `SystemFault_BIT.CellVolt_UN` (bit9) | 일치 |
| 26 | `BSA_Prtct_CellVolt_BL` | `SystemFault_BIT.CellVolt_BL` (bit10) | 일치 |
| 27 | `BSA_Prtct_CellTemp_OV` | `SystemFault_BIT.CellTemp_OV` (bit11) | 일치 |
| 28 | `BSA_Prtct_CellTemp_UN` | `SystemFault_BIT.CellTemp_UN` (bit12) | 일치 |
| 29 | `BSA_Prtct_CellTemp_BL` | `SystemFault_BIT.CellTemp_BL` (bit13) | 일치 |
| 30 | `BSA_Prtct_PackRLY_ERR` | `SystemFault_BIT.PackRLY_ERR` (bit14) | 일치 |
| 31 | `BSA_Prtct_FAULT15` | `SystemFault_BIT.FAULT15` (bit15) | 예약 일치 |
| 32 | `BSA_Prtct_CellIR_OV` | `SystemFault_BIT.CellIR_OV` (bit16) | 일치 |
| 33 | `BSA_Prtct_PackOcTime_Err` | `SystemFault_BIT.PackOcTime_Err` (bit17) | 일치 |
| 34 | `BSA_Prtct_PrtcOcEvent_Err` | `SystemFault_BIT.PrtcOcEvent_Err` (bit18) | 일치 |
| 35 | `BSA_Prtct_PackISO_ERR` | `SystemFault_BIT.PackISO_ERR` (bit19) | 일치 |
| 36 | `BSA_Prtct_PackIMD_ERR` | `SystemFault_BIT.PackIMD_ERR` (bit20) | 일치 |

**페이로드 배치** — `main.c:1159`  
```c
CANATX(0x603, 8, BAT80VAlarmReg.all,          /* byte0~1 : Alarm  bit0~15  */
                 BAT80VFaultReg.Word.DataL,   /* byte2~3 : CAN bit16~31    */
                 BAT80VFaultReg.Word.DataH,   /* byte4~5 : CAN bit32~47    */
                 0x0000);                     /* byte6~7 : 예약            */
```
규약 최상위 정의 비트(CAN36 `PackIMD_ERR` = `DataH` bit4)까지 정상 수용됩니다.

### 2. 이전 지적 사항 해소 ✔

| 항목 | 이전 상태 | 현재 (Product_Version 16) |
|:--|:--|:--|
| `main.c:921` | `// Cal80VSysFaultCheck(&SysRegs);` 주석 처리 → Protection 전 항목 미동작 | **주석 해제 완료** — 보호 판정 활성 |
| `SystemFault_REG.all` | `unsigned int`(16-bit) → CAN32 이상 비트가 `all != 0` 판정에서 누락 | **`unsigned long`(32-bit) 확장** — bit16~20 판정 포함 |

> C13 시험(셀 온도편차 10.1 ℃ 20 s 유지 → Fault 미발생)에서 실측으로 확인된 결함입니다. 코드상 해소되었으므로 **보호 검증 재시험이 필요**합니다.

### 3. 펌웨어 ↔ 규약 불일치 7건 — 조치 필요

| No | ID | 항목 | 규약 R11 | 펌웨어 | 조치 |
|:--:|:--|:--|:--|:--|:--|
| 1 | 0x608 | `BMS_CT_RxCount` | Start Bit 16 (byte2) | byte1 (`ComBine(MailBox0RxCount, MailBoxRxCount)`) | 배치 일치화 |
| 2 | 0x608 | `BMS_Slave1_Err` / `BMS_Slave2_Err` | @48 / @64 | byte4 / byte5 (`ComBine(Slave2Err, Slave1Err)`) | 배치 일치화 |
| 3 | 0x600 | `BSA_Normal_Volt` | 80.96 V | `Product_Voltage 768` → **76.8 V** | 상수 정정 (22S × 3.68 V = 80.96 → 810) |
| 4 | 0x600 | `BSA_Capacity` | 56 Ah | `Product_Capacity 450` → **45.0 Ah** | 상수 정정 (P56 56.4 Ah → 564) |
| 5 | 0x601 | `BSA_SOH` | 0~100 %, 계산값 | `CANATX(0x601,...,1000)` **리터럴 고정** → 항상 100.0 % | `CANARegs.BAT80VSOH` 로 교체 |
| 6 | 0x602 | byte6~7 | **미정의** | `BAT80VStateReg.all` 송신 | 규약 추가 또는 송신 제거 |
| 7 | 0x608 | `BMS_VCU_RxCount` | 0~200 Cnt | 통신 두절 시 **248** 보고 | `SysCanRxCount` 센티넬 값 정정 |

`Product_Voltage 768` 은 상수 주석(`3.664*22` = 80.6 V)과도 어긋납니다.

#### 7번 상세 — `BMS_VCU_RxCount` 범위 초과

`ComBine(Val_H, Val_L) = ((Val_H) << 8) | (Val_L)` (`DSP28x_Project.h:79`) 로 두 카운터를 1 워드에 바이트 단위로 담습니다.
나머지 세 카운터는 200 초과 시 0 으로 랩되어 8 bit 에 안전하게 들어갑니다.

```c
CANARegs.MailBoxRxCount++;  if(CANARegs.MailBoxRxCount  > 200){ ... = 0; }   /* main.c:1430 */
CANARegs.MailBox0RxCount++; if(CANARegs.MailBox0RxCount > 200){ ... = 0; }   /* main.c:1436 */
CANARegs.MailBox2RxCount++; if(CANARegs.MailBox2RxCount > 200){ ... = 0; }   /* main.c:1459 */
```

반면 `SysCanRxCount` 는 랩 처리가 없고, VCU 통신 두절 판정 시 센티넬로 **11000** 이 대입됩니다.

```c
SysRegs.SysCanRxCount++;                       /* main.c:1119, 100 ms 주기      */
if(SysRegs.SysCanRxCount >= 10) {              /* 1 s 미수신 → 통신 알람        */
    SysRegs.BAT80VAlarmReg.bit.PackFcu_CANErr = 1;
    SysRegs.SysCanRxCount = 11000;             /* main.c:1136  ← 8 bit 범위 초과 */
}
CANARegs.CANTxB = ComBine(SysRegs.SysCanRxCount, CANARegs.MailBox2RxCount);   /* main.c:1238 */
```

11000 = `0x2AF8` → `<< 8` 후 16 bit 절단 시 상위 바이트가 `0xF8` = **248** 로 남습니다.
즉 **통신 두절 구간 동안 `BMS_VCU_RxCount` 가 248 로 보고**되어 규약 정의 범위(0~200)를 벗어납니다.
센티넬을 200 이하 값(예: 200) 으로 바꾸거나, 별도 플래그로 분리하는 것을 권고합니다.

### 4. 규약 문서 자체 결함 6건 — 규약 정정 필요

| No | 위치 | 내용 |
|:--:|:--|:--|
| 1 | 0x608 `BMS_Slave2_Err` | **Start Bit 64** — 8 byte 프레임(bit0~63) 범위 초과, 배치 불가 |
| 2 | 0x608 `BMS_CT_RxCount` / `BMS_VCU_RxCount` | **둘 다 Start Bit 16** — 중복 배치 |
| 3 | 0x606 `BSA_Cell_DeviT` | Maximum **10 ℃** — 보호설정 R5 온도편차 **폴트 20 ℃** 를 표현 불가 |
| 4 | 0x605 `BSA_Cell_MinV` | Minimum **2.8 V** — 보호설정 R5 셀 저전압 **폴트 2.65 V** 가 범위 밖 |
| 5 | 0x604 `BSA_Discharge_Cont_PL` / `_Peak_PL` | Maximum 표기 `.-500` (선행 마침표) 오타 |
| 6 | 시트 A1 | 제목이 「R9 (1차년도 제품규격서 통신규약, 원복본)」 로 남아 있음 |

### 5. 미송신 · 미사용 2건

| 항목 | 내용 |
|:--|:--|
| `BAT80VProtectReg` (`SystemProtect_REG`) | 선언만 존재, `main.c` 어디에서도 사용되지 않음. `BAT80VFaultReg` 가 실제 역할 수행 |
| 0x609 / 0x60A | 규약상 `BMS_Debugging1/2` (Undisclosed). 펌웨어는 **22셀 전압·온도를 3셀씩 순환 전송** (`main.c:1266~1308`). 실제 포맷 문서화 필요 |

### 6. 보호설정표(260827 R5) 연계 — 규약 비트 미할당

| 설정표 항목 | 규약 비트 | 비고 |
|:--|:--|:--|
| `BPA_FltISOSPI_Err` | CAN35 `BSA_Prtct_PackISO_ERR` | **R11에서 할당 완료** |
| `Bsa_FltIMD_Err` | CAN36 `BSA_Prtct_PackIMD_ERR` | **R11에서 할당 완료** |
| `Bsa_FltOcTimer` | CAN33 `BSA_Prtct_PackOcTime_Err` | 할당 완료, SW 로직은 존재 (`C_PackOCTimer_Fault 480.0` / `C_PackOCTimerCount 1000`) |
| `Bsa_FltOcTime_min` | CAN34 `BSA_Prtct_PrtcOcEvent_Err` | 할당 완료, **SW 로직 미구현** |
| `Bsa_FltCellIR_OV` | CAN32 `BSA_Prtct_CellIR_OV` | 할당 완료, **SW 로직 미구현 (2nd 이월)** |
| `Bsa_FltRly_Err` | CAN30 `BSA_Prtct_PackRLY_ERR` | 할당 완료, **`FaultReg` 미설정 → 차단 동작 없음** |
| `Bsa_Wrn/FltUnbalPwr` | CAN7 / CAN23 | 할당 완료, **설정값 미정 · SW 미구현** |

> R11에서 `Bsa_FltCharCellUt`(저온 충전 금지) 비트 미할당 건은 **보호설정표 R4에서 항목 자체가 삭제**되어 해소되었습니다.
