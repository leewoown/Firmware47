# 10kWFCU BAP2nd Protocol — R10 (2026-08-27)

> **문서 버전:** R10 (R9 원복본 기반, 0x603 Protection을 펌웨어 SystemProtect_BIT 현재 정의에 일치)  
> **양식:** 10kWFCU_BAP2ndProtocol R32 준용  
> **물리계층:** CAN 2.0A Standard(11-bit ID) · LSB=intel · DLC 8byte  
> **변경:** 0x603 Protection 비트명 SystemProtect_BIT 정합 + 신규 4비트(PackOcTime_Err/PrtcOcEvent_Err/PackISO_ERR/PackIMD_ERR, CAN33~36) 추가, CAN31=FAULT15 예약비트

---

## CAN 통신규약 (NCM)

| ECU | Message | ID | DLC | Cycle[ms] | Signal | Len[bit] | StartBit | ByteOrder | ValueType | Initial | Factor | Offset | Min | Max | Unit | Value Table | Comment | VCU | BSA | 비고 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| IFCU | IFCU_BSA | 0x450 | 8 | 100 | Ifcu_BSARun | 1 | 8 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=stop;<br>0x1=run; | 0:BSA OFF, 1: BSA On | T | R |  |
| IFCU | IFCU_BSA | 0x450 | 8 | 100 | Ifcu_BSAReset | 1 | 9 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=No Action<br>0x1=Reset; | 0:BSA No Action, 1: BSA Fault Reset | T | R |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Type | 8 | 0 | LSB | Unsigned | 3 | 1 | 0 | 0 | 10 |  | 0x0=EV Bus;<br>0x1=Ship;<br>0x2=Subway;<br>0x3=E-Mobility; | 00:EV BUS, 01:Ship, 02:Subway, 03:E-Mobility | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_SW_Ver | 8 | 8 | LSB | Unsigned | 0 | 0.1 | 0 | 1 | 99 | VER |  | Software Version | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Normal_Volt | 16 | 16 | LSB | Unsigned |  | 0.1 | 0 | 0 | 100 | V |  | 80V Battery pack nominal voltage(공칭전압) | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Capacity | 16 | 32 | LSB | Unsigned | 56 | 0.1 | 0 | 30 | 60 | Ah |  | 80V Battery pack battery capacity | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Serial | 8 | 48 | LSB | Unsigned | 22 | 1 | 0 | 0 | 200 | S |  | 80V Battery pack CELL configuration | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Parallel | 8 | 56 | LSB | Unsigned | 1 | 1 | 0 | 0 | 10 | P |  | 80V Battery pack CELL configuration | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_Volt_Total | 16 | 0 | LSB | Unsigned |  | 0.1 | 0 | 40 | 120 | V |  | 80V Battery Pack Voltage | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_Curr_Total | 16 | 16 | LSB | Signed | 0 | 0.1 | 0 | -700 | 700 | A |  | 80V Battery Pack Current | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_SOC | 16 | 32 | LSB | Unsigned | 50 | 0.1 | 0 | 0 | 120 | % |  | 80V Battery Pack SOC | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_SOH | 16 | 48 | LSB | Unsigned | 100 | 0.1 | 0 | 0 | 100 | % |  | 80V Battery Pack SOH | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Ste | 3 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 4 |  | 0x0=Init;<br>0x1=Ready;<br>0x2=Run;<br>0x3=Warning;<br>0x4=Protection; | BMS State Protection level <br>0 : Init, 1: Ready, 2 : Run, 3 : Warning, 4 : Protection | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Balance | 1 | 3 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Stop;<br>0x1=Run; | 0 : Balance On, 1 : Balance Off | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Neg_Rly | 1 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Open;<br>0x1=Close; | 0 : Open, 1 : Close | R | T | 직결 구조로 Neg_Rly는 항상 1(Close) 고정 보고 |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Pos_Rly | 1 | 17 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Open;<br>0x1=Close; | 0 : Open, 1 : Close | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_PreChar_Rly | 1 | 18 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Open;<br>0x1=Close; | 0 : Open, 1 : Close | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Ah | 16 | 32 | LSB | Signed | 0 | 0.1 | 0 | -56 | 56 | Ah |  | BSA의 실시간 Ah 적산값으로, SOC 검증 및 에너지 사용량 확인용 | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OC | 1 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_SOC_OV | 1 | 1 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_SOC_Un | 1 | 2 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OV | 1 | 3 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UV | 1 | 4 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OT | 1 | 5 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UT | 1 | 6 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UnbalancePower | 1 | 7 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_OV | 1 | 8 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UV | 1 | 9 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UnbalV | 1 | 10 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_OT | 1 | 11 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UT | 1 | 12 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UnbalT | 1 | 13 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_FCUCAN_ERR | 1 | 14 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T | VER15, 적용함 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackOC | 1 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackOC (CAN16/int0) — 과전류 보호 | R | T | 명칭 일치(R9 _OC) |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackSOC_OV | 1 | 17 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackSOC_OV (CAN17/int1) — SOC 과충전 보호 | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackSOC_UN | 1 | 18 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackSOC_UN (CAN18/int2) — SOC 과방전 보호 | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackVolt_OV | 1 | 19 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackVolt_OV (CAN19/int3) — 팩 과전압 보호 | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackVolt_UN | 1 | 20 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackVolt_UN (CAN20/int4) — 팩 저전압 보호 | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackTemp_OV | 1 | 21 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackTemp_OV (CAN21/int5) — 팩 과온도 보호 | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackTemp_UN | 1 | 22 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackTemp_UN (CAN22/int6) — 팩 저온도 보호 | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackUnPWR_BL | 1 | 23 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackUnPWR_BL (CAN23/int7) — 출력 불균형 보호 | R | T | 명칭 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellVolt_OV | 1 | 24 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellVolt_OV (CAN24/int8) — 셀 과전압 보호 | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellVolt_UN | 1 | 25 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellVolt_UN (CAN25/int9) — 셀 저전압 보호 | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellVolt_BL | 1 | 26 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellVolt_BL (CAN26/int10) — 셀 전압편차 보호 | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellTemp_OV | 1 | 27 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellTemp_OV (CAN27/int11) — 셀 과온도 보호 | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellTemp_UN | 1 | 28 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellTemp_UN (CAN28/int12) — 셀 저온도 보호 | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellTemp_BL | 1 | 29 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellTemp_BL (CAN29/int13) — 셀 온도편차 보호 | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackRLY_ERR | 1 | 30 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackRLY_ERR (CAN30/int14) — 릴레이 에러 보호 | R | T | 명칭 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_FAULT15 | 1 | 31 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Reserved; | SystemProtect_BIT.FAULT15 (CAN31/int15) — CAN31 예약비트 (보호 판정 제외) | R | T | 의도적 불일치(260827 예약비트화, R9 CanTmOut) |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellIR_OV | 1 | 32 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.CellIR_OV (CAN32/int16) — 셀 내부저항 과대 보호 | R | T | 명칭 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackOcTime_Err | 1 | 33 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackOcTime_Err (CAN33/int17) — 과전류 시간 보호 | R | T | 규약 밖 확장 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PrtcOcEvent_Err | 1 | 34 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PrtcOcEvent_Err (CAN34/int18) — 과전류 횟수 보호 | R | T | 규약 밖 확장 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackISO_ERR | 1 | 35 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackISO_ERR (CAN35/int19) — ISOSPI 통신 에러 | R | T | 규약 밖 확장 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackIMD_ERR | 1 | 36 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | SystemProtect_BIT.PackIMD_ERR (CAN36/int20) — IMD 절연저항 에러 | R | T | 규약 밖 확장 |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Charge_Cont_PL | 16 | 0 | LSB | Unsigned | 2.5 | 0.1 | 0 | 10 | 20 | kW |  | 80V BATTERY Pack Charge continuity out Power limit | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Discharge_Cont_PL | 16 | 16 | LSB | Unsigned | 2.5 | 0.1 | 0 | 10 | 20 | kW |  | 80V BATTERY Pack Discharge continuity out Power limit | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Charge_Peak_PL | 16 | 32 | LSB | Unsigned | 2.5 | 0.1 | 0 | 10 | 20 | kW |  | 80V BATTERY Pack Charge Peak out Power limit | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Discharge_Peak_PL | 16 | 48 | LSB | Unsigned | 2.5 | 0.1 | 0 | 10 | 20 | kW |  | 80V BATTERY Pack Discharge Peak out Power limit | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_MaxV | 16 | 0 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 80V BATTERY Pack 배터리셀_최대전압 | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_MinV | 16 | 16 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 80V BATTERY Pack 배터리셀_최저전압 | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_AVGV | 16 | 32 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 80V BATTERY Pack 배터리셀_평균전압 | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_DeviV | 16 | 48 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1000 | mV |  | 80V BATTERY Pack 배터리셀_전압편차 | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_MaxT | 16 | 0 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 80V BATTERY Pack 배터리셀_최대온도 | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_MinT | 16 | 16 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 80V BATTERY Pack 배터리셀_최저온도 | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_AVGT | 16 | 32 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 80V BATTERY Pack 배터리셀_평균온도 | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_DeviT | 16 | 48 | LSB | Unsigned | 0 | 0.1 | 0 | 0 | 10 | ℃ |  | 80V BATTERY Pack 배터리셀_온도편차 | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BMS_Cell_MaxIR | 16 | 0 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BMS_Cell_MinIR | 16 | 16 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BMS_Cell_AVGIR | 16 | 32 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BSA_Cell_DeviIR | 16 | 48 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_CANRxCount | 8 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | BMS 패킷 수신 정상 여부 감시 ;수신 시 1씩 증가;200 도달 시 0 리셋 | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_CT_RxCount | 8 |  | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | 전류 센서 수신 정상 여부 감시 ;수신 시 1씩 증가;200 도달 시 0 리셋 | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_VCU_RxCount | 8 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | IFCU 수신 정상 여부 감시 ;수신 시 1씩 증가;200 도달 시 0 리셋 | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_Slave1_Err | 8 |  | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | SalveBatIC_ERR : ISOSPIERR 1씩 증가 |  |  |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_Slave2_Err | 8 |  | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | SalveBatIC_ERR : ISOSPIERR 1씩 증가 |  |  |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BMS_Debugging1 | 64 | 0 | LSB | Unsigned |  |  |  |  |  |  |  | Undisclosed | R | T |  |
| BSA | BSA11 | 0x60A | 8 | 100 | BMS_Debugging2 | 64 | 0 | LSB | Unsigned |  |  |  |  |  |  |  | Undisclosed | R | T |  |

---

## CAN 통신규약 (LFP)  ※ LFP는 변경 없음(펌웨어 NCM 기준)

| ECU | Message | ID | DLC | Cycle[ms] | Signal | Len[bit] | StartBit | ByteOrder | ValueType | Initial | Factor | Offset | Min | Max | Unit | Value Table | Comment | VCU | BSA | 비고 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| IFCU | IFCU_BSA | 0x450 | 8 | 100 | Ifcu_BSARun | 1 | 8 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=stop;<br>0x1=run; | 0:BSA OFF, 1: BSA On | T | R |  |
| IFCU | IFCU_BSA | 0x450 | 8 | 100 | Ifcu_BSAReset | 1 | 9 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=No Action<br>0x1=Reset; | 0:BSA No Action, 1: BSA Fault Reset | T | R |  |
| HMI | NVR_SET | 0X400 | 8 | 100 | NVR_SETUP | 1 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=No Action<br>0x1=Set |  |  |  |  |
| HMI | NVR_SET | 0X400 | 8 | 100 | NVR_INITSOC | 16 | 16 | LSB | Unsigned | 50 | 0.1 | 0 | 0 | 120 | % |  |  |  |  |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Type | 8 | 0 | LSB | Unsigned | 3 | 1 | 0 | 0 | 10 |  | 0x0=EV Bus;<br>0x1=Ship;<br>0x2=Subway;<br>0x3=E-Mobility; | 00:EV BUS, 01:Ship, 02:Subway, 03:E-Mobility | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_SW_Ver | 8 | 8 | LSB | Unsigned | 0 | 0.1 | 0 | 1 | 99 | VER |  | Software Version | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Normal_Volt | 16 | 16 | LSB | Unsigned |  | 0.1 | 0 | 0 | 100 | V |  | 80V Battery pack nominal voltage(공칭전압) | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Capacity | 16 | 32 | LSB | Unsigned | 40 | 0.1 | 0 | 30 | 60 | Ah |  | 80V Battery pack battery capacity | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Serial | 8 | 48 | LSB | Unsigned | 24 | 1 | 0 | 0 | 200 | S |  | 80V Battery pack CELL configuration | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Parallel | 8 | 56 | LSB | Unsigned | 1 | 1 | 0 | 0 | 10 | P |  | 80V Battery pack CELL configuration | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_Volt_Total | 16 | 0 | LSB | Unsigned | 76.8 | 0.1 | 0 | 40 | 120 | V |  | 80V Battery Pack Voltage | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_Curr_Total | 16 | 16 | LSB | Signed | 0 | 0.1 | 0 | -700 | 700 | A |  | 80V Battery Pack Current | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_SOC | 16 | 32 | LSB | Unsigned | 50 | 0.1 | 0 | 0 | 120 | % |  | 80V Battery Pack SOC | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_SOH | 16 | 48 | LSB | Unsigned | 100 | 0.1 | 0 | 0 | 100 | % |  | 80V Battery Pack SOH | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Ste | 3 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 4 |  | 0x0=Init;<br>0x1=Ready;<br>0x2=Run;<br>0x3=Warning;<br>0x4=Protection; | BMS State Protection level <br>0 : Init, 1: Ready, 2 : Run, 3 : Warning, 4 : Protection | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Balance | 1 | 3 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Stop;<br>0x1=Run; | 0 : Balance On, 1 : Balance Off | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Neg_Rly | 1 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Open;<br>0x1=Close; | 0 : Open, 1 : Close | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Pos_Rly | 1 | 17 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Open;<br>0x1=Close; | 0 : Open, 1 : Close | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_PreChar_Rly | 1 | 18 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Open;<br>0x1=Close; | 0 : Open, 1 : Close | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Ah | 16 | 32 | LSB | Signed | 0 | 0.1 | 0 | -40 | 40 | Ah |  | SOC 전류적산 확인 | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OC | 1 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_SOC_OV | 1 | 1 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_SOC_Un | 1 | 2 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OV | 1 | 3 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UV | 1 | 4 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OT | 1 | 5 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UT | 1 | 6 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UnbalancePower | 1 | 7 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_OV | 1 | 8 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UV | 1 | 9 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UnbalV | 1 | 10 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_OT | 1 | 11 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UT | 1 | 12 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UnbalT | 1 | 13 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Warning; | 0:Normal, 1:Warning | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_OC | 1 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_SOC_OV | 1 | 17 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_SOC_Un | 1 | 18 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_OV | 1 | 19 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_UV | 1 | 20 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_OT | 1 | 21 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_UT | 1 | 22 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_UnbalancePower | 1 | 23 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Cell_OV | 1 | 24 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Cell_UV | 1 | 25 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Cell_UnbalV | 1 | 26 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Cell_OT | 1 | 27 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Cell_UT | 1 | 28 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Cell_UnbalT | 1 | 29 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_Rly_Err | 1 | 30 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | Bsa_PrtctCanTmOut | 1 | 31 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | Bsa_Prtct_CellIR_OV | 1 | 32 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Normal;<br>0x1=Protection; | 0:Normal, 1:Protection | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Charge_Cont_PL | 16 | 0 | LSB | Unsigned | 2.5 | 0.1 | 0 | 10 | 20 | kW |  | 80V BATTERY Pack Charge continuity out Power limit | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Discharge_Cont_PL | 16 | 16 | LSB | Unsigned | 2.5 | 0.1 | 0 | 10 | 20 | kW |  | 80V BATTERY Pack Discharge continuity out Power limit | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Charge_Peak_PL | 16 | 32 | LSB | Unsigned | 2.5 | 0.1 | 0 | 10 | 20 | kW |  | 80V BATTERY Pack Charge Peak out Power limit | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Discharge_Peak_PL | 16 | 48 | LSB | Unsigned | 2.5 | 0.1 | 0 | 10 | 20 | kW |  | 80V BATTERY Pack Discharge Peak out Power limit | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_MaxV | 16 | 0 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 80V BATTERY Pack 배터리셀_최대전압 | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_MinV | 16 | 16 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 80V BATTERY Pack 배터리셀_최저전압 | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_AVGV | 16 | 32 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 80V BATTERY Pack 배터리셀_평균전압 | R | T |  |
| BSA | BSA6 | 0x605 | 8 | 100 | BSA_Cell_DeviV | 16 | 48 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1000 | mV |  | 80V BATTERY Pack 배터리셀_전압편차 | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_MaxT | 16 | 0 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 80V BATTERY Pack 배터리셀_최대온도 | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_MaxT | 16 | 16 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 80V BATTERY Pack 배터리셀_최저온도 | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_AVGT | 16 | 32 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 80V BATTERY Pack 배터리셀_평균온도 | R | T |  |
| BSA | BSA7 | 0x606 | 8 | 100 | BSA_Cell_DeviT | 16 | 48 | LSB | Unsigned | 0 | 0.1 | 0 | 0 | 10 | ℃ |  | 80V BATTERY Pack 배터리셀_온도편차 | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BMS_Cell_MaxIR | 16 | 0 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BMS_Cell_MinIR | 16 | 16 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BMS_Cell_AVGIR | 16 | 32 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed | R | T |  |
| BSA | BSA8 | 0x607 | 8 | 100 | BSA_Cell_DeviIR | 16 | 48 | LSB | Unsigned | 4 | 1 | 0 | 0 | 500 | mΩ |  | Undisclosed | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_CANRxCount | 16 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | Undisclosed | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_CT_RxCount | 16 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | Undisclosed | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_VCU_RxCount | 16 | 32 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | Undisclosed | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BMS_Debugging1 | 64 | 0 | LSB | Unsigned |  |  |  |  |  |  |  | Undisclosed | R | T |  |
| BSA | BSA11 | 0x60A | 8 | 100 | BMS_Debugging2 | 64 | 0 | LSB | Unsigned |  |  |  |  |  |  |  | Undisclosed | R | T |  |
