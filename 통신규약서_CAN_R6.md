# Pack BMS CAN 통신 규약서 (R6)

- **대상 제품**: 47 HDFC 10kW NCM Pack BMS (F28069)
- **제품 코드**: Product_Type = 3, Product_Version = 10 (`parameter.h`)
- **문서 버전**: R6
- **기준 소스**: `SysSoure/main.c`, `SysSoure/DSP28x_Project.c`, `C2806XSrc/F2806x_ECan.c`, `C2806Xinclude/DSP28x_Project.h`

---

## 1. 물리 계층 (Physical Layer)

| 항목 | 값 | 비고 |
|------|-----|------|
| 컨트롤러 | TI F28069 eCAN-A | `ECanaRegs` |
| 프레임 포맷 | CAN 2.0A (Standard, 11-bit ID) | `MSGID.bit.IDE = 0` |
| 통신 속도 | **500 kbps** | `CAN_500KBPS = 1` (`F2806x_ECan.c`) |
| CAN 모듈 클럭 | 30 MHz | SYSCLKOUT 80 MHz 기준 |
| 비트 타이밍 | BRP=3, TSEG1=13, TSEG2=4, SAM=1 (3-sampling) | `CANBTC` |
| DLC | 8 byte (전 메시지 고정) | |

> ※ 다른 속도(1 Mbps / 250 kbps)는 `F2806x_ECan.c` 상단 매크로로 전환 가능하나 R6 양산 설정은 500 kbps.

---

## 2. 데이터 바이트 배열 규칙 (Byte Order)

송신 함수 `CANATX(ID, Length, Data0, Data1, Data2, Data3)` 는 16-bit 워드 4개를
**리틀 엔디안(하위 바이트 먼저)** 으로 8바이트에 배치한다. (`DSP28x_Project.c`)

| 인자 | Byte0 | Byte1 | Byte2 | Byte3 | Byte4 | Byte5 | Byte6 | Byte7 |
|------|-------|-------|-------|-------|-------|-------|-------|-------|
| 매핑 | D0_L | D0_H | D1_L | D1_H | D2_L | D2_H | D3_L | D3_H |

- 각 워드 = `(ByteH << 8) | ByteL`
- 신호 단위의 스케일/부호는 각 메시지 표의 "단위" 열 참조.

---

## 3. 송신 메시지 (BMS → VCU/PMS)

> 송신 조건: 초기화 완료 후 `CANCOMEnable = 1` 일 때 주기 송신.
> ID 범위: **0x600 ~ 0x60A**

### 3.1 메시지 목록

| ID | 이름 | 주기 | 내용 |
|----|------|------|------|
| 0x600 | 제품/SW 정보 | 1 s + 초기화 | SW 버전, 정격전압/용량, 구성 |
| 0x601 | 팩 기본 측정 | 10 ms | 전압/전류/SOC |
| 0x602 | 팩 상태 | 10 ms | 상태/릴레이 출력/누적Ah/시스템상태 |
| 0x603 | 알람/폴트 | 100 ms | Alarm, Fault |
| 0x604 | 전력 한계 | 100 ms | 충/방전 연속·피크 전력 |
| 0x605 | 셀 전압 통계 | 100 ms | Max/Min/Avg/Div |
| 0x606 | 셀 온도 통계 | 100 ms | Max/Min/Avg/Div |
| 0x607 | 셀 내부저항 통계 | 100 ms | Max/Min/Avg/Div (현재 고정값) |
| 0x608 | 통신 진단 카운터 | 100 ms | 메일박스/슬레이브 수신·오류 카운트 |
| 0x609 | 셀 개별 데이터 | 100 ms | 인덱스별 온도/전압 |
| 0x60A | 셀 온도(3개 묶음) | 100 ms | 인덱스별 온도 3개 |

### 3.2 메시지 상세

#### ▶ 0x600 — 제품/SW 정보
| 워드 | 신호 | 단위/스케일 | 비고 |
|------|------|-------------|------|
| D0 | SW Type/Version | — | Type=3, Version=10 |
| D1 | 정격 전압 코드 | `Product_Voltage` = 768 | |
| D2 | 정격 용량 | `Product_Capacity` = 400 (40.0 Ah) | |
| D3 | 구성 코드 `BAT80VConfing` | 0x0118 (24S 구성) | |

#### ▶ 0x601 — 팩 기본 측정 (10 ms)
| 워드 | 신호 | 단위/스케일 | 부호 |
|------|------|-------------|------|
| D0 | 팩 전압 | 0.1 V (×10) | U |
| D1 | 팩 전류 | 0.1 A (×10) | **S** |
| D2 | SOC | 0.1 % (×10) | U |
| D3 | SOH | 0.1 % → **1000 고정(100.0%)** | U |

#### ▶ 0x602 — 팩 상태 (10 ms)
| 워드 | 신호 | 설명 |
|------|------|------|
| D0 | `BAT80VStatus` | bit[2:0] BATStatus, bit3 BalanceEN |
| D1 | `BAT80VDigitalOutPutReg` | b0 NRlyOUT, b1 PRlyOUT, b2 CHARlyOUT, b8 LEDAlarmOUT, b9 LEDFaultOUT … |
| D2 | `BAT80VStateReg` | 시스템 상태 비트(아래 4.1) |
| D3 | 0x000 (예약) | — |

#### ▶ 0x603 — 알람/폴트 (100 ms)
| 워드 | 신호 | 설명 |
|------|------|------|
| D0 | `BAT80VAlarmReg` | SystemAlarm 비트(4.2) |
| D1 | Fault DataL | SystemFault 하위 16bit(4.3) |
| D2 | Fault DataH | SystemFault 상위 (CellIR_OV 등) |
| D3 | 누적 Ah | 0.1 Ah (×10), 부호 **S** |

#### ▶ 0x604 — 전력 한계 (100 ms)
| 워드 | 신호 | 단위/스케일 | 현재값 |
|------|------|-------------|--------|
| D0 | 연속 충전 전력 | 0.1 kW (×10) | 12.2 kW |
| D1 | 연속 방전 전력 | 0.1 kW (×10) | 8.6 kW |
| D2 | 피크 충전 전력 | 0.1 kW (×10) | 20.2 kW |
| D3 | 피크 방전 전력 | 0.1 kW (×10) | 12.2 kW |

> ※ R6 현재 고정값. 추후 실측/연산값으로 대체 예정.

#### ▶ 0x605 — 셀 전압 통계 (100 ms)
| 워드 | 신호 | 단위/스케일 |
|------|------|-------------|
| D0 | 셀 최대 전압 | mV (×1000) |
| D1 | 셀 최소 전압 | mV (×1000) |
| D2 | 셀 평균 전압 | mV (×1000) |
| D3 | 셀 전압 편차 | mV (×1000) |

#### ▶ 0x606 — 셀 온도 통계 (100 ms)
| 워드 | 신호 | 단위/스케일 | 부호 |
|------|------|-------------|------|
| D0 | 셀 최고 온도 | 0.1 ℃ (×10) | S |
| D1 | 셀 최저 온도 | 0.1 ℃ (×10) | S |
| D2 | 셀 평균 온도 | 0.1 ℃ (×10) | S |
| D3 | 셀 온도 편차 | 0.1 ℃ (×10) | S |

#### ▶ 0x607 — 셀 내부저항 통계 (100 ms)
| 워드 | 신호 | 현재값 | 비고 |
|------|------|--------|------|
| D0 | IR Max | 50 | 고정값 |
| D1 | IR Min | 40 | 고정값 |
| D2 | IR Avg | 45 | 고정값 |
| D3 | IR Div | 10 (Max−Min) | |

> ※ R6 현재 고정 더미값. 내부저항 측정 알고리즘 미적용.

#### ▶ 0x608 — 통신 진단 카운터 (100 ms)
각 워드는 두 8-bit 카운터를 결합(`ComBine`)한 값.
| 워드 | 상위 바이트 | 하위 바이트 |
|------|-------------|-------------|
| D0 | MailBox0 RxCount | 전체 MailBox RxCount |
| D1 | MailBox3 RxCount | MailBox2 RxCount |
| D2 | Slave1 ErrorCount | SysCanRxCount |
| D3 | 0 | Slave2 ErrorCount |

#### ▶ 0x609 — 셀 개별 데이터 (100 ms)
| 워드 | 신호 |
|------|------|
| D0 | 셀 인덱스 `CellIRTxNum` |
| D1 | 해당 인덱스 온도 |
| D2 | 해당 인덱스 전압 |
| D3 | 해당 인덱스 전압(중복) |

#### ▶ 0x60A — 셀 온도 3개 묶음 (100 ms)
| 워드 | 신호 |
|------|------|
| D0 | 시작 인덱스 `CellIRTxNum` |
| D1 | 온도[idx+0] |
| D2 | 온도[idx+1] |
| D3 | 0 (예약) |

> 인덱스는 매 송신마다 +3 씩 증가, 24셀 순환.

---

## 4. 상태/알람/폴트 비트 정의

### 4.1 `BAT80VStateReg` (0x602 D3) — SystemState
| bit | 신호 | bit | 신호 |
|-----|------|-----|------|
| 0–2 | SysSTATE (상태머신) | 9 | TCPIPTOMERR |
| 3 | BalanceMode | 10 | RS485COMERR |
| 4 | SysAalarm | 11 | RTCRD |
| 5 | SysFault | 12 | sysDisChaMode |
| 6 | SocMode | 13 | INITOK |
| 7 | INCANCOMERR | 14 | BalanceStatStop |
| 8 | EXCANCOMERR | 15 | CANCOMEnable |

> SysSTATE 값: 1=READY, 2=RUNNING, 3=ALARM, 4=FAULT/PROTECT

### 4.2 `BAT80VAlarmReg` (0x603 D0) — SystemAlarm
| bit | 신호 | bit | 신호 |
|-----|------|-----|------|
| 0 | PackOC | 8 | CellVolt_OV |
| 1 | PackVSOC_OV | 9 | CellVolt_UN |
| 2 | PackVSOC_UN | 10 | CellVolt_BL |
| 3 | PackVolt_OV | 11 | CellTemp_OV |
| 4 | PackVolt_UN | 12 | CellTemp_UN |
| 5 | PackTemp_OV | 13 | CellTemp_BL |
| 6 | PackTemp_UN | 14 | PackCAN_ERR |
| 7 | PackUnPWR_BL | 15 | (예약) |

### 4.3 `BAT80VFaultReg` (0x603 D1=하위, D2=상위) — SystemFault
| bit | 신호 | bit | 신호 |
|-----|------|-----|------|
| 0 | PackVCT_OV | 8 | CellVolt_OV |
| 1 | PackVSOC_OV | 9 | CellVolt_UN |
| 2 | PackVSOC_UN | 10 | CellVolt_BL |
| 3 | PackVolt_OV | 11 | CellTemp_OV |
| 4 | PackVolt_UN | 12 | CellTemp_UN |
| 5 | PackTemp_OV | 13 | CellTemp_BL |
| 6 | PackTemp_UN | 14 | PackRLY_ERR |
| 7 | PackUnPWR_BL | 15 | (예약) |
| 16 | CellIR_OV / PackOcTime_Err / PrtcOcEvent_Err | | (D2 워드) |

---

## 5. 수신 메시지 (VCU/PMS·센서 → BMS)

> 메일박스 MBOX0~MBOX3, Standard ID, 8 byte.

| ID | MBOX | 이름 | 데이터 해석 |
|----|------|------|-------------|
| 0x3C5 | MBOX0 | 80V 전류 센서 | CurrentH=(B0<<8\|B1), CurrentL=(B2<<8\|B3) |
| 0x3C3 | MBOX1 | 12V 전류 센서 | CurrentH=(B0<<8\|B1), CurrentL=(B2<<8\|B3) |
| 0x450 | MBOX2 | VCU/PMS 명령 | `PMSCMDRegs` = (B1<<8\|B0) |
| 0x400 | MBOX3 | NVR 설정 | `NVRSetRegs` = (B1<<8\|B0), SOC초기값 = (B3<<8\|B2) |

### 5.1 0x450 — VCU 명령 (`PMSCMDRegs`, VCUCOMMAND)
| bit | 신호 | 설명 |
|-----|------|------|
| 0–7 | PCCMD00–07 | 예약 명령 |
| 8 | **RUNStatus** | 1=기동 요청 (릴레이 Wake-up) |
| 9 | PrtctReset | 보호 리셋 |
| 10–15 | PCCMD10–15 | 예약 |

> RUNStatus 미수신 1초 경과 시(100ms×10) `PackCAN_ERR` 알람 발생 후 RUNStatus 강제 0.

### 5.2 0x400 — NVR 설정 (`NVRSetRegs`, NVRSET)
| bit | 신호 | 설명 |
|-----|------|------|
| 0 | NVRInit | NVRAM 초기화 |
| 1 | NVRforceWR | 강제 기록 |
| 2 | NVRSocInit | SOC 초기화 (값: D 워드 B3:B2) |
| 3 | NVRManual | 수동 모드 |
| 4 | NVRLogRead | 로그 읽기 |
| 5 | NVREventRead | 이벤트 읽기 |

---

## 6. 개정 이력

| 버전 | 일자 | 내용 |
|------|------|------|
| R6 | 2026-06-30 | 소스(main.c 등) 기준 CAN 통신 규약 정리 |

---

### 부록. 미확정/주의 항목
- 0x604 전력 한계, 0x607 내부저항: 현재 펌웨어 **고정값** 송신 (실측 연산 미적용).
- 0x601 D3(SOH): SOH 연산은 존재하나 전송은 1000(100.0%) 고정.
- 0x608 `ComBine` 바이트 상·하위 배치는 소스 사용 순서 기준 표기 — VCU 측 파싱 시 실측 확인 권장.
