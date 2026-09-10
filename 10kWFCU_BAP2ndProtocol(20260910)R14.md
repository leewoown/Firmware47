# 10kWFCU BAP2nd Protocol — R14 (2026-09-10)

> **문서 버전:** R14  
> **양식:** 10kWFCU_BAP2ndProtocol R32 준용 · **물리계층:** CAN 2.0A Standard(11-bit ID) · LSB=intel · DLC 8byte  
> **기준 펌웨어:** 47 HDFC 10kW NCM Pack BMS (F28069) — main.c 260910 반영 (VER 0.21)  
> **검증:** 122 시그널 / 12 메시지, StartBit·Length·겹침 오류 **0건**

---

## 📌 R13 → R14 변경사항

| 구분 | ID | 항목 | R13 | R14 |
|---|---|---|---|---|
| 변경 | 0x608 | 카운터 길이 | **16bit×3 + 8bit×2** (bit0/16/32/48/56) | **8bit×4 + 예약16bit + 8bit×2** (bit0/8/16/24 · 예약32~47 · 48/56) |
| 신규 | 0x608 | `BMS_SpiTimeout` | 없음 | **bit24~31 (byte3)** — SPI 대기 타임아웃 누적, 0~200 포화 |
| 변경 | 0x608 | `BMS_Slave1_Err`/`BMS_Slave2_Err` (8bit×2, bit48/56) | 온도 GPIO 경로만 집계 | **`BMS_IsoSpiErr` (16bit, bit48~63)** — `ltc_error_count`, LTC6804 read 전 경로 |
| 구현 | 0x603 | `BSA_Prtct_PackISO_ERR` | bit35 정의만, 펌웨어 세팅 로직 없음(항상 0) | **판정 구현** — `ltc_state==0` 또는 `ltc_error_count>200` 시 세트 |

### 변경 사유

BATIC(LTC6804) isoSPI 통신 장애를 **디버거 연결 없이 CAN 만으로 진단**하기 위한 확장이다.
260910 통신 불능 사례에서, 부팅 중 NVRAM 접근이 타임아웃 없는 SPI 대기 루프에 걸려
`main()` 의 `while(1)` 진입 자체가 실패했고, 이를 외부에서 관측할 수단이 없었다.

- `BMS_SpiTimeout` : SPI 대기 루프 타임아웃 누적. **0 이면 정상.**
- `BSA_Prtct_PackISO_ERR` : isoSPI 통신 실패의 **보호(Fault) 단일 판정 비트.**
- 0x608 프레임 **부재 자체**도 진단 정보 — 부팅 중 정지 시 송신이 끊긴다.

카운터 3종(`BMS_CANRxCount`/`BMS_CT_RxCount`/`BMS_VCU_RxCount`)은 모두 **0~200 범위**로
운용되므로 8bit 로 축소해도 값 손실이 없다. 이로써 확보한 byte3 에 `BMS_SpiTimeout` 을 배치했다.

> ⚠️ **R13 대비 0x608 파싱이 바뀐다.** 상위 장치(FCU) DBC 갱신이 필요하다.

### 0x608 최종 배치 (R14, 펌웨어 정합)

#### (1) 전송 규칙 — CANATX 인자 ↔ 페이로드 바이트

펌웨어는 `CANATX(ID, DLC, D0, D1, D2, D3)` 로 송신하며, **D0~D3 는 각각 16bit 워드**다.
각 워드는 CAN 페이로드에 **LSB first(intel)** 로 실리므로 배치는 아래와 같이 고정된다.

| CANATX 인자 | 펌웨어 변수 | 페이로드 바이트 | 비고 |
|---|---|---|---|
| D0 | `CANARegs.CANTxA` | byte0 = 하위, byte1 = 상위 | bit0~15 |
| D1 | `CANARegs.CANTxB` | byte2 = 하위, byte3 = 상위 | bit16~31 |
| D2 | `CANARegs.CANTxC` | byte4 = 하위, byte5 = 상위 | bit32~47 |
| D3 | `CANARegs.CANTxD` | byte6 = 하위, byte7 = 상위 | bit48~63 |

워드 조립에 쓰는 매크로 정의 (`DSP28x_Project.h:79`):

```c
#define ComBine(Val_H, Val_L)   (((Val_H) << 8) | (Val_L))
```

> ⚠️ **첫 인자가 상위바이트(홀수 byte), 둘째 인자가 하위바이트(짝수 byte)** 다.
> 예: `ComBine(B, A)` → byte(짝) = A, byte(홀) = B.

#### (2) 시그널 배치

| bit | byte | 길이 | 시그널 | 워드 위치 | 펌웨어 소스 | 범위 | 정상값 |
|---|---|---|---|---|---|---|---|
| 0~7 | **0** | 8 | `BMS_CANRxCount` | `CANTxA` 하위 | `CANARegs.MailBoxRxCount & 0x00FF` | 0~200 | 증가 |
| 8~15 | **1** | 8 | `BMS_CT_RxCount` | `CANTxA` 상위 | `CANARegs.MailBox0RxCount & 0x00FF` (전류센서 0x3C5, MBOX0) | 0~200 | 증가 |
| 16~23 | **2** | 8 | `BMS_VCU_RxCount` | `CANTxB` 하위 | `CANARegs.MailBox2RxCount & 0x00FF` (IFCU 0x450) | 0~200 | 증가 |
| 24~31 | **3** | 8 | **`BMS_SpiTimeout`** | `CANTxB` 상위 | `SpiTimeoutCount & 0x00FF` (`F2806x_Spi.c`) **[R14 신규]** | 0~200 포화 | **0** |
| 32~39 | **4** | 8 | 예약 | `CANTxC` 하위 | `0` 고정 | 0 | 0 |
| 40~47 | **5** | 8 | 예약 | `CANTxC` 상위 | `0` 고정 | 0 | 0 |
| 48~63 | **6~7** | 16 | **`BMS_IsoSpiErr`** | `CANTxD` 전체 | `ltc_error_count` (`BAT_LTC6802.c`) — LTC6804 read 전 경로 연속 PEC 실패 **[R14 변경]** | 0~65535 | **0** |

#### (3) 펌웨어 구현 (`main.c`, 100ms 주기 case 23)

```c
CANARegs.CANTxA = ComBine((CANARegs.MailBox0RxCount & 0x00FF),(CANARegs.MailBoxRxCount  & 0x00FF));
CANARegs.CANTxB = ComBine((SpiTimeoutCount          & 0x00FF),(CANARegs.MailBox2RxCount & 0x00FF));
CANARegs.CANTxC = 0x0000;                              /* byte4~5 예약 */
CANARegs.CANTxD = (unsigned int)ltc_error_count;       /* byte6~7 IsoSpiErr (16bit) */
CANATX(0x608, 8, CANARegs.CANTxA, CANARegs.CANTxB, CANARegs.CANTxC, CANARegs.CANTxD);
```

#### (4) 판독 예시

수신 페이로드가 `7B 64 32 00 00 00 00 00` 인 경우:

| byte | 값(hex) | 값(dec) | 해석 |
|---|---|---|---|
| 0 | 7B | 123 | `BMS_CANRxCount` = 123 |
| 1 | 64 | 100 | `BMS_CT_RxCount` = 100 |
| 2 | 32 | 50 | `BMS_VCU_RxCount` = 50 |
| 3 | 00 | 0 | `BMS_SpiTimeout` = 0 → **SPI 정상** |
| 4~5 | 00 00 | 0 | 예약 |
| 6~7 | 00 00 | 0 | `BMS_IsoSpiErr` = 0 → **isoSPI 통신 정상** |

**이상 판정 기준**

| 관측 | 의미 | 조치 |
|---|---|---|
| byte3 > 0 | SPI 대기 타임아웃 발생 (NVRAM 무응답 등) | SPI 버스·NVRAM 점검. BATIC 통신은 유지됨 |
| byte6~7 > 0 | isoSPI read PEC 실패 (연속) | 값이 계속 오르면 LTC6820·isoSPI 배선 점검 |
| byte6~7 > 200 | 연속 실패 200 초과 | 0x603 `BSA_Prtct_PackISO_ERR` 가 함께 세트됨 |
| **0x608 프레임 부재** | 부팅 중 정지 또는 CAN 송신 불가 | 전원·펌웨어 기동 확인 |

---

## 📌 R12 → R13 변경사항

| 구분 | ID | 항목 | R12 | R13 |
|---|---|---|---|---|
| 구조변경 | 0x609 | 셀 전압/온도 | `BSA_CellMux` + A/B/C 슬롯 6신호, **Mux 태그 없음 → 겹침 48건** | **표준 Multiplexed 전개**: `BSA_CellMux`(M) + `BSA_CellVolt00~21`(m0~m7) + `BSA_CellTemp00~21`(m8~m15) |
| 이동 | 0x602 | `BMS_BATIC_Temp` | 0x609/0x60A 소속 | **0x602 bit48** (D3), Signed·0.1℃·−40~120 |
| 변경 | 0x60A | `BMS_Debugging1` | 48bit (bit16~63) | **64bit (bit0~63)** 전체 |
| 오류수정 | 0x608 | StartBit 수식 잔존 | `BMS_CT_RxCount`/`BMS_Slave1_Err`/`BMS_Slave2_Err` StartBit 미계산(수식) | 값 확정 (16 / 48 / 56) |
| 변경 | 0x608 | 카운터 길이 | 8bit×5 (byte0/2/4/6/7, **미사용 24bit**) | **16bit×3 + 8bit×2** (bit0/16/32/48/56), 미사용 비트 **0** |

### 0x609 멀티플렉스 규칙

| Mux 값 | 내용 | 슬롯 배치 | 셀 번호 |
|---|---|---|---|
| 0~7 | 셀 전압 그룹 | A=bit8, B=bit24, C=bit40 | Mux×3 + slot (0~21) |
| 8~15 | 셀 온도 그룹 | A=bit8, B=bit24, C=bit40 | (Mux−8)×3 + slot (0~21) |

> 22셀 구성이므로 Mux7·Mux15의 슬롯C(#23)는 미사용.

### 0x608 최종 배치 (펌웨어 정합)

| bit | 길이 | 시그널 | 펌웨어 |
|---|---|---|---|
| 0~15 | 16 | `BMS_CANRxCount` | `CANTxA = MailBoxRxCount` |
| 16~31 | 16 | `BMS_CT_RxCount` | `CANTxB = MailBox0RxCount` (전류센서 0x3C5, MBOX0) |
| 32~47 | 16 | `BMS_VCU_RxCount` | `CANTxC = MailBox2RxCount` (IFCU 0x450) |
| 48~55 | 8 | `BMS_Slave1_Err` | `CANTxD` 하위바이트 |
| 56~63 | 8 | `BMS_Slave2_Err` | `CANTxD` 상위바이트 |

> 펌웨어는 이미 본 배치와 일치하여 **코드 변경 불필요**. 카운터는 200 초과 시 0 리셋(main.c:1496 등).  
> 다만 주석 문구 정정 권장: `byte2 CT_RxCount` → **byte2~3(16bit)**, `byte4 VCU_RxCount` → **byte4~5(16bit)**.

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
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_CANRxCount | 8 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | byte0 — CANTxA 하위바이트 = MailBoxRxCount & 0x00FF. 전체 CAN Rx 카운터 |  | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_CT_RxCount | 8 | 8 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | byte1 — CANTxA 상위바이트 = MailBox0RxCount & 0x00FF. 전류센서(0x3C5, MBOX0) Rx 카운터 |  | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_VCU_RxCount | 8 | 16 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | byte2 — CANTxB 하위바이트 = MailBox2RxCount & 0x00FF. IFCU(0x450) Rx 카운터 |  | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_SpiTimeout | 8 | 24 | LSB | Unsigned | 0 | 1 | 0 | 0 | 200 | Cnt |  | byte3 — CANTxB 상위바이트 = SpiTimeoutCount & 0x00FF (F2806x_Spi.c). SPI 대기 타임아웃 누적, 0=정상 / 200 포화 [R14 신규] |  | R | T |  |
| BSA | BSA9 | 0x608 | 8 | 100 | BMS_IsoSpiErr | 16 | 48 | LSB | Unsigned | 0 | 1 | 0 | 0 | 65535 | Cnt |  | byte6~7 — CANTxD 전체 = ltc_error_count (BAT_LTC6802.c). LTC6804 read 전 경로의 연속 PEC 실패, 성공 시 0 리셋. 200 초과 시 0x603 PackISO_ERR 세트 [R14 변경: Slave1/2_Err 8bit×2 → 16bit 통합] |  | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellMux | 8 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 15 | - | 0x0~0x7=Cell Volt Grp;<br>0x8~0xF=Cell Temp Grp; | [Multiplexer] 0~7=셀전압 그룹, 8~15=셀온도 그룹 (그룹당 3셀: A/B/C) | M | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt00 | 16 | 8 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 1 전압 (Mux=0, slot A) | m0 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt01 | 16 | 24 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 2 전압 (Mux=0, slot B) | m0 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt02 | 16 | 40 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 3 전압 (Mux=0, slot C) | m0 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt03 | 16 | 8 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 4 전압 (Mux=1, slot A) | m1 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt04 | 16 | 24 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 5 전압 (Mux=1, slot B) | m1 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt05 | 16 | 40 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 6 전압 (Mux=1, slot C) | m1 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt06 | 16 | 8 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 7 전압 (Mux=2, slot A) | m2 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt07 | 16 | 24 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 8 전압 (Mux=2, slot B) | m2 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt08 | 16 | 40 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 9 전압 (Mux=2, slot C) | m2 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt09 | 16 | 8 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 10 전압 (Mux=3, slot A) | m3 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt10 | 16 | 24 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 11 전압 (Mux=3, slot B) | m3 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt11 | 16 | 40 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 12 전압 (Mux=3, slot C) | m3 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt12 | 16 | 8 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 13 전압 (Mux=4, slot A) | m4 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt13 | 16 | 24 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 14 전압 (Mux=4, slot B) | m4 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt14 | 16 | 40 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 15 전압 (Mux=4, slot C) | m4 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt15 | 16 | 8 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 16 전압 (Mux=5, slot A) | m5 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt16 | 16 | 24 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 17 전압 (Mux=5, slot B) | m5 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt17 | 16 | 40 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 18 전압 (Mux=5, slot C) | m5 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt18 | 16 | 8 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 19 전압 (Mux=6, slot A) | m6 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt19 | 16 | 24 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 20 전압 (Mux=6, slot B) | m6 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt20 | 16 | 40 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 21 전압 (Mux=6, slot C) | m6 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellVolt21 | 16 | 8 | LSB | Unsigned | 3.2 | 0.001 | 0 | 2.8 | 4.5 | V |  | 셀 22 전압 (Mux=7, slot A) | m7 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp00 | 16 | 8 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 1 온도 (Mux=8, slot A) | m8 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp01 | 16 | 24 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 2 온도 (Mux=8, slot B) | m8 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp02 | 16 | 40 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 3 온도 (Mux=8, slot C) | m8 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp03 | 16 | 8 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 4 온도 (Mux=9, slot A) | m9 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp04 | 16 | 24 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 5 온도 (Mux=9, slot B) | m9 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp05 | 16 | 40 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 6 온도 (Mux=9, slot C) | m9 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp06 | 16 | 8 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 7 온도 (Mux=10, slot A) | m10 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp07 | 16 | 24 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 8 온도 (Mux=10, slot B) | m10 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp08 | 16 | 40 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 9 온도 (Mux=10, slot C) | m10 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp09 | 16 | 8 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 10 온도 (Mux=11, slot A) | m11 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp10 | 16 | 24 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 11 온도 (Mux=11, slot B) | m11 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp11 | 16 | 40 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 12 온도 (Mux=11, slot C) | m11 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp12 | 16 | 8 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 13 온도 (Mux=12, slot A) | m12 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp13 | 16 | 24 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 14 온도 (Mux=12, slot B) | m12 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp14 | 16 | 40 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 15 온도 (Mux=12, slot C) | m12 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp15 | 16 | 8 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 16 온도 (Mux=13, slot A) | m13 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp16 | 16 | 24 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 17 온도 (Mux=13, slot B) | m13 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp17 | 16 | 40 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 18 온도 (Mux=13, slot C) | m13 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp18 | 16 | 8 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 19 온도 (Mux=14, slot A) | m14 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp19 | 16 | 24 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 20 온도 (Mux=14, slot B) | m14 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp20 | 16 | 40 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 21 온도 (Mux=14, slot C) | m14 | R | T |  |
| BSA | BSA10 | 0x609 | 8 | 100 | BSA_CellTemp21 | 16 | 8 | LSB | Signed | 25 | 0.1 | 0 | -40 | 120 | ℃ |  | 셀 22 온도 (Mux=15, slot A) | m15 | R | T |  |
| BSA | BSA11 | 0x60A | 8 | 100 | BMS_Debugging1 | 64 | 0 | LSB | Unsigned | 0 | 1 | 0 | 0 | 0 | - |  | Undisclosed (64bit 전체) |  | R | T |  |

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
