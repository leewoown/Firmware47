# 10kWFCU BAP2nd Protocol — H15 (2026-09-10)

> **문서 버전:** H15 (고객 제출용)  
> **0x608 / 0x609 / 0x60A:** BMS_Debugging1~3 — BMS Debugging Data (Undisclosed), 64bit (당사 내부 진단용)  
> **물리계층:** CAN 2.0A Standard(11-bit ID) · LSB=intel · DLC 8byte  
> **검증:** 74 시그널 / 12 메시지, 오류 0건

---

## 📌 H14 → H15 변경사항

| No | ID | 항목 | H14 | H15 | FCU 영향 |
|---|---|---|---|---|---|
| — | — | 고객 수록 대상 변경 없음 | — | — | **없음** |

> R14→R15 변경은 모두 0x608(당사 내부 진단용 디버깅 메시지) 내부 신호 추가이므로, 고객 제출본 기준 **실질 변경 사항이 없습니다.**  
> 고객 수록 대상: 0x450 · 0x600 ~ 0x607 (0x608·0x609·0x60A는 BMS Debugging으로 비공개).

---

## CAN 통신규약 (NCM)

| ECU | Message | ID | DLC | Cycle[ms] | Signal | Len[bit] | StartBit | ByteOrder | ValueType | Initial | Factor | Offset | Min | Max | Unit | Value Table | Comment | Mux | VCU | BSA | 비고 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| IFCU | IFCU_BSA | 0x450 | 8 | 100 | Ifcu_BSARun | 1 | 8 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=stop;<br>0x1=run; | 0:BSA OFF, 1: BSA On |  | T | R |  |
| IFCU | IFCU_BSA | 0x450 | 8 | 100 | Ifcu_BSAReset | 1 | 9 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=No Action<br>0x1=Reset; | 0:BSA No Action, 1: BSA Fault Reset |  | T | R |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Type | 8 | 0 | LSB | Unsigned | 3 | 1 | 0 | 0 | 10 |  | 0x0=D;<br>0x1=P; | 0x0=Development <br>0x1=Production |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_SW_Ver | 8 | 8 | LSB | Unsigned | 0 | 1 | 0 | 0 | 255 | VER |  | Software Version |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Normal_Volt | 16 | 16 | LSB | Unsigned | 80.9 | 0.1 | 0 | 0 | 100 | V |  | 80V Battery pack nominal voltage(공칭전압) |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Capacity | 16 | 32 | LSB | Unsigned | 56 | 0.1 | 0 | 30 | 60 | Ah |  | 80V Battery pack battery capacity |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Serial | 8 | 48 | LSB | Unsigned | 22 | 1 | 0 | 0 | 200 | S |  | 80V Battery pack CELL configuration |  | R | T |  |
| BSA | BSA1 | 0x600 | 8 | 1000 | BSA_Parallel | 8 | 56 | LSB | Unsigned | 1 | 1 | 0 | 0 | 10 | P |  | 80V Battery pack CELL configuration |  | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_Volt_Total | 16 | 0 | LSB | Unsigned | 80.9 | 0.1 | 0 | 40 | 120 | V |  | 80V Battery Pack Voltage |  | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_Curr_Total | 16 | 16 | LSB | Signed | 0 | 0.1 | 0 | -700 | 700 | A |  | 80V Battery Pack Current |  | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_SOC | 16 | 32 | LSB | Unsigned | 50 | 0.1 | 0 | 0 | 120 | % |  | 80V Battery Pack SOC |  | R | T |  |
| BSA | BSA2 | 0x601 | 8 | 10 | BSA_SOH | 16 | 48 | LSB | Unsigned | 100 | 0.1 | 0 | 0 | 100 | % |  | 80V Battery Pack SOH |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Ste | 3 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 4 |  | 0x0=Init;<br>0x1=Rdy;<br>0x2=Run;<br>0x3=Wn;<br>0x4=Prt; | BMS State Protection level <br>0 : Init, 1: Ready, 2 : Run, 3 : Warning, 4 : Protection |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Balance | 1 | 3 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=Stop;<br>0x1=Run; | 0 : BalanceOff, 1 : BalanceOn |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Neg_Rly | 1 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=OP;<br>0x1=CL; | 0 : Open, 1 : Close |  | R | T | 직결 구조로 Neg_Rly는 항상 1(Close) 고정 보고 |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Pos_Rly | 1 | 17 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=OP;<br>0x1=CL; | 0 : Open, 1 : Close |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_PreChar_Rly | 1 | 18 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 |  | 0x0=OP;<br>0x1=CL; | 0 : Open, 1 : Close |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BSA_Ah | 16 | 32 | LSB | Signed | 0 | 0.1 | 0 | -56 | 56 | Ah |  | BSA의 실시간 Ah 적산값으로, SOC 검증 및 에너지 사용량 확인용 |  | R | T |  |
| BSA | BSA3 | 0x602 | 8 | 100 | BMS_BATIC_Temp | 16 | 48 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | BATIC 측정 배터리팩 내부온도. BSA_Wn_OT/UT, BSA_Prtct_PackTemp_OV/UN 판정에 사용 |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OC | 1 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_SOC_OV | 1 | 1 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_SOC_Un | 1 | 2 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OV | 1 | 3 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UV | 1 | 4 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_OT | 1 | 5 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UT | 1 | 6 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_UnbalancePower | 1 | 7 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_OV | 1 | 8 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UV | 1 | 9 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UnbalV | 1 | 10 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_OT | 1 | 11 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UT | 1 | 12 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_Cell_UnbalT | 1 | 13 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T |  |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Wn_FCUCAN_ERR | 1 | 14 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Wn; | 0:Normal, 1:Warning |  | R | T | VER15, 적용함 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackOC | 1 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.PackOC (CAN16/int0) — 과전류 보호 |  | R | T | 명칭 일치(R9 _OC) |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackSOC_OV | 1 | 17 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.PackSOC_OV (CAN17/int1) — SOC 과충전 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackSOC_UN | 1 | 18 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.PackSOC_UN (CAN18/int2) — SOC 과방전 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackVolt_OV | 1 | 19 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.PackVolt_OV (CAN19/int3) — 팩 과전압 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackVolt_UN | 1 | 20 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.PackVolt_UN (CAN20/int4) — 팩 저전압 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackTemp_OV | 1 | 21 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.PackTemp_OV (CAN21/int5) — 팩 과온도 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackTemp_UN | 1 | 22 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.PackTemp_UN (CAN22/int6) — 팩 저온도 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackUnPWR_BL | 1 | 23 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.PackUnPWR_BL (CAN23/int7) — 출력 불균형 보호 |  | R | T | 명칭 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellVolt_OV | 1 | 24 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.CellVolt_OV (CAN24/int8) — 셀 과전압 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellVolt_UN | 1 | 25 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.CellVolt_UN (CAN25/int9) — 셀 저전압 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellVolt_BL | 1 | 26 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.CellVolt_BL (CAN26/int10) — 셀 전압편차 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellTemp_OV | 1 | 27 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.CellTemp_OV (CAN27/int11) — 셀 과온도 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellTemp_UN | 1 | 28 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.CellTemp_UN (CAN28/int12) — 셀 저온도 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellTemp_BL | 1 | 29 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.CellTemp_BL (CAN29/int13) — 셀 온도편차 보호 |  | R | T | 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackRLY_ERR | 1 | 30 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.PackRLY_ERR (CAN30/int14) — 릴레이 에러 보호 |  | R | T | 명칭 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_FAULT15 | 1 | 31 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.FAULT15 (CAN31/int15) — CAN31 예약비트 (보호 판정 제외) |  | R | T | 의도적 불일치(260827 예약비트화, R9 CanTmOut) |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_CellIR_OV | 1 | 32 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.CellIR_OV (CAN32/int16) — 셀 내부저항 과대 보호 |  | R | T | 명칭 일치 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackOcTime_Err | 1 | 33 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.PackOcTime_Err (CAN33/int17) — 과전류 시간 보호 |  | R | T | 규약 밖 확장 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PrtcOcEvent_Err | 1 | 34 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.PrtcOcEvent_Err (CAN34/int18) — 과전류 횟수 보호 |  | R | T | 규약 밖 확장 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackISO_ERR | 1 | 35 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.PackISO_ERR (CAN35/int19) — ISOSPI 통신 에러. [R14] 판정 구현: ltc_state==0 또는 ltc_error_count>200 |  | R | T | 규약 밖 확장 |
| BSA | BSA4 | 0x603 | 8 | 100 | BSA_Prtct_PackIMD_ERR | 1 | 36 | LSB | Unsigned | 0 | 1 | 0 | 0 | 1 | flag | 0x0=Nor;<br>0x1=Prt; | SystemProtect_BIT.PackIMD_ERR (CAN36/int20) — IMD 절연저항 에러 |  | R | T | 규약 밖 확장 |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Charge_Cont_PL | 16 | 0 | LSB | Signed | 10 | 0.1 | 0 | 10 | 500 | A |  | 80V BATTERY Pack Charge continuity out Power limit |  | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Discharge_Cont_PL | 16 | 16 | LSB | Signed | -10 | 0.1 | 0 | -500 | -10 | A |  | 80V BATTERY Pack Discharge continuity out Power limit |  | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Charge_Peak_PL | 16 | 32 | LSB | Signed | 10 | 0.1 | 0 | 10 | 500 | A |  | 80V BATTERY Pack Charge Peak out Power limit |  | R | T |  |
| BSA | BSA5 | 0x604 | 8 | 100 | BSA_Discharge_Peak_PL | 16 | 48 | LSB | Signed | -10 | 0.1 | 0 | -500 | -10 | A |  | 80V BATTERY Pack Discharge Peak out Power limit |  | R | T |  |
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
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_Debugging1 | 64 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 0 | - |  | BMS Debugging Data (Undisclosed) |  | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BMS_Debugging2 | 64 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 0 | - |  | BMS Debugging Data (Undisclosed) |  | R | T |  |
| BSA | BSA11 | 0x60A | 8 | 100 | BMS_Debugging3 | 64 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 0 | - |  | BMS Debugging Data (Undisclosed) |  | R | T |  |

---

## 배터리 제작 규격서 (NCM)

|  |  | 항목 |  |  | 내용 | 비고 |
|---|---|---|---|---|---|---|
|  | 1 | 제품명(과제명) |  |  | 수소지게차 보조배터리 팩 |  |
|  | 2 | 전지<br>구성 | 전지 구성 |  | 22S1P |  |
|  | 3 |  | 전지종류/타입 |  | NCM/파우치 |  |
|  | 4 |  | 동작 전압[V] |  | 66.~90.2V | DoD 80% |
|  | 6 |  | 공칭 전압[V] |  | 80.9 | Cell:3.659(DoD80%) |
|  | 7 |  | 전지 용량[Ah] |  | 56 |  |
|  | 8 |  | 총 에너지 용량[kWh] |  |  | 권장 조건 충방전 시 |
|  | 9 |  | 정격 에너지 용량[kWh,DoD 80%] |  |  | 권장 조건 충방전 시 |
|  | 10 |  | 보관 온도 |  | 0 ~ 35℃ |  |
|  | 11 |  | 충전 | 충전모드 | CV-CC |  |
|  | 12 |  |  | 권장 전류[A] | 18,7A |  |
|  | 13 |  |  | 연속 전류[A] | 56A |  |
|  | 14 |  |  | 최대 전류[A] | 310A (SOC 30~70%), 10sec, 셀 평가 후 제시 |  |
|  | 15 |  |  | 동작 온도(T≥0℃) | 권장 및 연속, 최대 전류 조건 |  |
|  |  |  |  | 동작 온도(T≤0℃) | 충전 전류 디레이팅(Charging Current Derating) |  |
|  | 16 |  | 방전 | 권장 전류 | 56A |  |
|  | 17 |  |  | 연속 전류 | 150A |  |
|  | 18 |  |  | 최대 전류 | 500A(SOC50%↑). 10sec |  |
|  | 19 |  |  | 동작 온도 | .-30 ~ 60℃ |  |
|  | 20 | 외함구조 | 보호 등급 |  | IP54 |  |
|  | 21 |  | 진동 기준 |  | 국토환경부 축전지 48장 충족 |  |
|  | 22 |  | 부식 기준 |  | 고객사 확인 필요 |  |
|  | 23 |  | 냉각 방식 |  | 자연 냉각 방식 |  |
|  | 24 |  | 가스 배출 |  | KC 62619 5.3 참조 |  |
|  | 25 |  | 충돌 기준 |  | KC 62619 7.2.2, KC 62619 7.2.3 충족 |  |
|  | 26 |  | 낙하 기준 |  | KC 62619 7.2.2, KC 62619 7.2.3 충족 |  |
|  | 27 |  | 연소 기준 |  | 국토환경부 축전지 48장 충족 |  |
|  | 28 |  | 열 폭주 기준 |  | 한국선급 배터리 시스템 동일 설계 적용 |  |
|  | 29 |  | 외함 재질 |  | 아연 도금 |  |
|  |  |  | 외함 도색 |  | 흑색 |  |
|  | 30 |  | 외함 치수 |  | 250×490×190 |  |
|  | 31 | 제어 | MCU |  | 32bit Float Microprocessor, 80Mhz |  |
|  | 32 |  | 셀 전압 |  | 측정범위: 0~5V/오차율: ±3mV/해상도/0.001V |  |
|  | 33 |  | 셀 온도 |  | 측정범위: ±150A/오차율: ±0.5%/해상도: 0.1A |  |
|  | 34 |  | 셀 전류 |  | 측정범위: ±150A/오차율: ±0.5%/해상도: 0.1A |  |
|  | 35 |  | 팩 전압 |  | 측정범위: 0 ~ 100V/오차율: ±10mV/해상도: 0.1V |  |
|  | 36 |  | 셀 내부 저항 |  | 측정범위: 0~10mΩ/오차율: T.B.D/해상도: 0.1mΩ |  |
|  | 37 |  | SOC |  | 측정범위: -10~100%/오차율: ±10%/해상도: 0.1% |  |
|  | 38 |  | SOH |  | 측정범위: -10~100%/오차율: ±15%/해상도: 0.1% |  |
|  | 39 |  | 내부  통신 |  | 절연형 SPI |  |
|  | 40 |  | 외부 통신 |  | 절연형 CAN |  |
