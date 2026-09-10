---
name: 10KWFCUBAS_PrtotectSet(260909)R10
description: 현대자동차 10kWFC 2차년도 22S1P NCM 보조배터리 팩 BMS — 구현 코드 기준 경고·차단 판정 정밀 분석 (비교 조건·지연시간)
project: HDFC 10kW FC 2nd / F28069 Pack BMS
revision: R10
date: 2026-09-09
firmware: Product_Type 0 / Product_Version 21 (Ver 0.21)
sources:
  - SysInclude/parameter.h
  - SysSoure/DSP28x_Project.c  (Cal80VSysAlarmtCheck / Cal80VSysFaultCheck)
  - SysSoure/BATAlgorithm.c    (CalP56CurrentLimit)
  - C2806Xinclude/DSP28x_Project.h (Hyst_On / Hyst_Off)
  - 10KWFCUBAS_PrtotectSet(260901)R9.md
note: R9 까지는 설정표(엑셀)가 정본이었으나, 본 R10 은 **실제 구현 코드를 읽어 역으로 정리한 문서**다.
      설정표와 어긋나는 부분은 8 항에 모아 두었다.
---

# 보호 판정 정밀 분석 R10 (260909)

## 1. 문서 목적

설정표 R9 는 각 항목의 조건을 「이상 / 이하 / 초과 / 미만」으로 기재하고 지연을 「100 ms / 즉시 = 0」으로 적는다.
그러나 **판정은 매크로와 카운터 구조가 결정**하므로, 표기와 실제 동작이 한 눈금씩 어긋날 수 있다.

본 문서는 `Ver 0.21` 코드를 그대로 읽어

- 항목별 **실제 비교 연산자**(이상/이하/초과/미만)
- 항목별 **실제 지연시간**(틱 단위 환산)
- 발생·해제 사이의 **히스테리시스 밴드**

를 정리한다. 시험 판정 기준과 설정표 갱신의 근거로 쓴다.

---

## 2. 판정 기반

### 2-1. 시간 기준

| 항목 | 값 | 근거 |
|---|---|---|
| 판정 주기 | **1 ms** | `ConfigCpuTimer(&CpuTimer0, 80, 1000)` — 80 MHz, 1000 µs ([main.c:230](SysSoure/main.c#L230)) |
| 카운터 1 tick | 1 ms | 경고·차단 카운터 모두 이 주기로 증가 |
| 판정 호출 | 매 주기 1회 | `INITOK == 1` 일 때만 ([main.c:1013](SysSoure/main.c#L1013)) |
| 호출 순서 | 경고 → 차단 | `Cal80VSysAlarmtCheck` → `Cal80VSysFaultCheck` |
| 전류한계 갱신 | **100 ms** | `CalP56CurrentLimit` 는 100 ms 슬롯 ([main.c:1289](SysSoure/main.c#L1289)) |

### 2-2. 비교 매크로 실제 정의

[DSP28x_Project.h:85-86](C2806Xinclude/DSP28x_Project.h#L85-L86)

```c
#define Hyst_On(Value, SetValue)   ((Value) >= (SetValue))   // 켜짐 조건
#define Hyst_Off(Value, RstValue)  ((Value) <= (RstValue))   // 꺼짐 조건
```

| 매크로 | 실제 연산 | 우리말 | 설정표 R9 §2 기재 |
|---|---|---|---|
| `Hyst_On` | `>=` | **이상** | `>` 초과 |
| `Hyst_Off` | `<=` | **이하** | `<` 미만 |

> **설정표와 어긋난다.** 설정표는 경계값에서 판정이 서지 않는다고 적혀 있으나,
> 구현은 **경계값을 포함**한다. 상세는 8-1 항.

### 2-3. 차단 판정의 비교

차단은 매크로를 쓰지 않고 직접 비교한다. 방향은 매크로와 같다.

```c
if(s->Bat80VCellMaxVoltageF >= C_CellVoltOV_Fault)   // 상한 : 이상
if(s->Bat80VCellMinVoltageF <= C_CellVoltUN_Fault)   // 하한 : 이하
```

---

## 3. 경고 판정 구조와 타이밍

### 3-1. 공통 골격

[DSP28x_Project.c:455~](SysSoure/DSP28x_Project.c#L455) 이하 14 항목이 동일 형태다.

```c
if(Hyst_On(측정값, 발생임계))                       // ① 발생 조건 (이상/이하)
{
    if(cnt <  Delay){ ++cnt; }                      // ② Delay 에서 포화
    if(cnt >= Delay){ 경고비트 = 1; }               // ③ 이상 비교로 SET
}
else
{
    if(경고비트 == 0){ cnt = 0; }                   // ④ 미발생 상태에서만 리셋
    if(Hyst_Off(측정값, 해제임계))                  // ⑤ 해제 조건
    {
        cnt = 0;  경고비트 = 0;                     // ⑥ 지연 없이 즉시 해제
    }
}
```

### 3-2. 타이밍 계산

| 단계 | 동작 | 소요 |
|---|---|---|
| 발생 | 조건 성립 후 카운터가 `Delay` 에 도달하는 순간 SET | **정확히 `Delay` tick = `Delay` ms** |
| 해제 | 해제 조건 성립 즉시 CLEAR | **0 ms** |
| 중간 구간 | 발생임계와 해제임계 사이 | 직전 상태 유지 (경고 서 있으면 유지) |

②에서 `cnt < Delay` 일 때만 증가시키므로 카운터는 `Delay` 에서 멈춘다. 오버플로가 없다.

**계산 예 — No.9 셀 과전압 경고 (4.15 V / 4.12925 V / 100 ms)**

| 시각 | 셀 최고전압 | cnt | 경고비트 | 설명 |
|---:|---:|---:|:--:|---|
| 0 ms | 4.1499 | 0 | 0 | 발생 조건 미성립 |
| 1 ms | 4.1500 | 1 | 0 | **경계값 포함**되어 조건 성립, 계수 시작 |
| 99 ms | 4.1500 | 99 | 0 | 아직 미달 |
| **100 ms** | 4.1500 | **100** | **1** | `100 >= 100` → SET |
| 150 ms | 4.1400 | 100 | 1 | 발생임계 아래지만 해제임계 위 → **유지** |
| 200 ms | 4.12925 | 0 | **0** | 해제 조건(이하) 성립 → **즉시 해제** |

### 3-3. 예외 — No.8 전류한계 초과 경고

매크로를 쓰지 않고 초과분 부호만 본다. 해제 임계가 없다.

```c
if(s->Bat80VUnbalCurrentF > 0)                     // 초과분 > 0 (초과)
{
    if(cnt <  C_PackUnbalPwr_WarnDelay){ ++cnt; }
    if(cnt >= C_PackUnbalPwr_WarnDelay){ SET; }
}
else
{
    cnt = 0;  CLEAR;                                // 초과 해소 즉시 해제
}
```

- 초과분 `Bat80VUnbalCurrentF` 는 `CalP56CurrentLimit` 이 **100 ms 주기**로 갱신
- 카운터는 1 ms 로 증가하므로, 판정값은 100 ms 계단이지만 **지속시간 계측은 1 ms 정밀도** 유지
- 100 ms 미만의 짧은 초과는 샘플 사이에 묻혀 계수되지 않을 수 있다

---

## 4. 경고 15 항목 정밀표

측정값·조건·밴드는 코드에서 직접 추출했다. 지연은 전 항목 **설정값 그대로**다.

| No | 항목 | 측정 변수 | 발생 조건 | 해제 조건 | 밴드 | Delay | 0x603 |
|--:|---|---|---|---|--:|--:|:--:|
| 1 | `Bsa_WrnOC` | `Bat80VCurrentAsbF` | **450.0 A 이상** | **405.0 A 이하** | 45.0 A | 100 ms | bit0 |
| 2 | `Bsa_WrnSocH` | `Bat80VSOCF` | **95.0 % 이상** | **92.15 % 이하** | 2.85 % | 100 ms | bit1 |
| 3 | `Bsa_WrnSocL` | `Bat80VSOCF` | **5.0 % 이하** | **5.25 % 이상** | 0.25 % | 100 ms | bit2 |
| 4 | `Bsa_WrnOv` | `Bat80VVoltageF` | **90.86 V 이상** | **88.1342 V 이하** | 2.7258 V | 100 ms | bit3 |
| 5 | `Bsa_WrnUv` | `Bat80VVoltageF` | **65.45 V 이하** | **68.7 V 이상** | 3.25 V | 100 ms | bit4 |
| 6 | `Bsa_WrnOt` | `Bat80VCellAgvTemperatureF` | **47.0 ℃ 이상** | **44.65 ℃ 이하** | 2.35 ℃ | 100 ms | bit5 |
| 7 | `Bsa_WrnUt` | `Bat80VCellAgvTemperatureF` | **−20.0 ℃ 이하** | **−19.0 ℃ 이상** | 1.0 ℃ | 100 ms | bit6 |
| 8 | `Bsa_WrnUnbalPwr` | `Bat80VUnbalCurrentF` | **0 A 초과** | **0 A 이하** | 없음 | **10 000 ms** | bit7 |
| 9 | `Bsa_WrnCellOv` | `Bat80VCellMaxVoltageF` | **4.15 V 이상** | **4.12925 V 이하** | 0.02075 V | 100 ms | bit8 |
| 10 | `Bsa_WrnCellUv` | `Bat80VCellMinVoltageF` | **3.00 V 이하** | **3.015 V 이상** | 0.015 V | 100 ms | bit9 |
| 11 | `Bsa_WrnCellUnbalV` | `Bat80VCellDivVoltageF` | **0.2 V 이상** | **0.0666667 V 이하** | 0.1333 V | 100 ms | bit10 |
| 12 | `Bsa_WrnCellOt` | `Bat80VCellMaxTemperatureF` | **55.0 ℃ 이상** | **52.25 ℃ 이하** | 2.75 ℃ | 100 ms | bit11 |
| 13 | `Bsa_WrnCellUt` | `Bat80VCellMinTemperatureF` | **−25.0 ℃ 이하** | **−23.5 ℃ 이상** | 1.5 ℃ | 100 ms | bit12 |
| 14 | `Bsa_WrnCellUnbalTmp` | `Bat80VCellDivTemperatureF` | **10.0 ℃ 이상** | **5.0 ℃ 이하** | 5.0 ℃ | 100 ms | bit13 |
| 15 | `BPA_WrnVcuCan_Err` | `SysCanRxCount` | **10 회 이상** | **10 회 미만** | — | 즉시 | bit14 |

- No.15 는 100 ms 슬롯에서 카운트하므로 **10 회 = 1 초 미수신**이며, 수신 재개 시 카운터가 0 이 되어 즉시 해제된다 ([main.c:1227-1244](SysSoure/main.c#L1227-L1244)).
- 밴드는 「발생임계 − 해제임계」의 절대값이다. 이 구간 안에서는 직전 상태가 유지된다.

---

## 5. 차단 판정 구조와 타이밍

### 5-1. 공통 골격

```c
if(측정값 >= 차단임계)                    // ① 이상 (하한 항목은 <= 이하)
{
    cnt++;                                // ② 무조건 선증가
    순시비트 = 1;
    if(cnt > Delay)                       // ③ 초과 비교
    {
        cnt = Delay + 10;                 // ④ 포화
        차단비트 = 1;                     // ⑤ 걸림 유지 (자동 해제 없음)
    }
}
else
{
    cnt = 0;
    (일부 항목) 순시비트 = 0;
}
```

### 5-2. 경고와 다른 점

| 구분 | 경고 | 차단 |
|---|---|---|
| 카운터 증가 | `cnt < Delay` 일 때만 | **무조건** 선증가 |
| SET 비교 | `cnt >= Delay` (이상) | **`cnt > Delay` (초과)** |
| 실제 지연 | `Delay` tick | **`Delay + 1` tick** |
| 해제 | 해제임계 도달 시 자동 | **없음** — 전원 OFF 또는 `Ifcu_BSAReset` |

③이 `>` 인 것은 부호 없는 카운터와 0 을 `>=` 로 비교할 때 나던 컴파일 경고(`#188-D`)를 피하기 위한 것이다(Ver 0.19).
카운터를 먼저 증가시키므로 `Delay = 0` 이면 첫 tick 에 `1 > 0` 이 성립한다.

### 5-3. 「즉시 = 0」의 실제 값

**설정표의 「즉시 = 0」은 실제로 1 ms 이다.**

| 설정 `Delay` | 첫 SET 시각 | 비고 |
|--:|--:|---|
| 0 | **1 ms** | 조건 성립 다음 tick |
| 1 | 2 ms | |
| N | N + 1 ms | |

판정 주기가 1 ms 이므로 조건 성립과 동시에 SET 하는 것은 구조상 불가능하다.
1 ms 는 릴레이 응답(수 ms~수십 ms)에 비해 무시할 수준이므로 실용상 「즉시」로 본다.

---

## 6. 차단 21 항목 정밀표

| No | 항목 | 측정 변수 | 조건 | 설정 Delay | **실제 지연** | 0x603 |
|--:|---|---|---|--:|--:|:--:|
| 16 | `Bsa_FltOc` | `Bat80VCurrentAsbF` | **505.0 A 이상** | 0 | **1 ms** | bit16 |
| 17 | `Bsa_FltctSocH` | `Bat80VSOCF` | **100.0 % 이상** | 0 | **1 ms** | bit17 |
| 18 | `Bsa_FlttctSocL` | `Bat80VSOCF` | **0.0 % 이하** | 0 | **1 ms** | bit18 |
| 19 | `Bsa_FltOv` | `Bat80VVoltageF` | **91.3 V 이상** | 0 | **1 ms** | bit19 |
| 20 | `Bsa_FltUv` | `Bat80VVoltageF` | **58.3 V 이하** | 0 | **1 ms** | bit20 |
| 21 | `Bsa_FltOt` | `Bat80VCellAgvTemperatureF` | **52.0 ℃ 이상** | 0 | **1 ms** | bit21 |
| 22 | `Bsa_FltUt` | `Bat80VCellAgvTemperatureF` | **−27.0 ℃ 이하** | 0 | **1 ms** | bit22 |
| 23 | `Bsa_FltUnbalPwr` | `Bat80VUnbalCurrentF` | (0 A 초과) | 15 000 | **차단 중** | bit23 |
| 24 | `Bsa_FltCellOv` | `Bat80VCellMaxVoltageF` | **4.20 V 이상** | 0 | **1 ms** | bit24 |
| 25 | `Bsa_FltCellUv` | `Bat80VCellMinVoltageF` | **2.65 V 이하** | 0 | **1 ms** | bit25 |
| 26 | `Bsa_FltCellUnbalVlt` | `Bat80VCellDivVoltageF` | **0.40 V 이상** | 0 | **1 ms** | bit26 |
| 27 | `Bsa_FltCellOt` | `Bat80VCellMaxTemperatureF` | **60.0 ℃ 이상** | 0 | **1 ms** | bit27 |
| 28 | `Bsa_FltCellUt` | `Bat80VCellMinTemperatureF` | **−30.0 ℃ 이하** | 0 | **1 ms** | bit28 |
| 29 | `Bsa_FltCellUnbalTmp` | `Bat80VCellDivTemperatureF` | **20.0 ℃ 이상** | 0 | **1 ms** | bit29 |
| 30 | `Bsa_FltRly_Err` | — | (판정 없음) | — | **차단 중** | bit30 |
| 31 | `BSA_Prtct_FAULT15` | — | 예약 | — | — | bit31 |
| 32 | `Bsa_FltCellIR_OV` | — | (판정 없음) | — | **차단 중** | bit32 |
| 33 | `Bsa_FltOcTimer` | `Bat80VCurrentAsbF` | **480.0 A 이상 10 000 tick** | — | **10.000 s** | bit33 |
| 34 | `Bsa_FltOcTime_min` | 진입 edge 횟수 | **1 분 창 내 5 회 이상** | — | 5 회째 즉시 | bit34 |
| 35 | `BPA_FltISOSPI_Err` | — | (판정 없음) | — | 미구현 | bit35 |
| 36 | `Bsa_FltIMD_Err` | — | (판정 없음) | — | **차단 중** | bit36 |

「차단 중」은 R9 「261220 이후 반영」에 따라 **판정부를 주석 처리하고 비트를 매 주기 0 으로 강제**한 상태다.

---

## 7. 특수 판정 3 건 상세

### 7-1. No.33 최대 전류 시간

```c
if(s->Bat80VCurrentAsbF >= C_PackOCTimer_Fault)   // 480.0 A 이상
{
    s->BAPackOCCount++;
    if(s->BAPackOCCount >= C_PackOCTimerCount)     // 10000 : 이상 비교
    {
        s->BAPackOCCount = C_PackOCTimerCount + 10;
        s->BAT80VFaultReg.bit.PackOcTime_Err = 1;
    }
}
else { s->BAPackOCCount = 0;  s->BAPackOCOld = 0; }
```

- 여기만 `>=` **이상** 비교다. 선증가 후 비교이므로 **10 000 tick = 10.000 초** 정확
- 480 A 아래로 한 번이라도 내려가면 카운터가 0 이 되어 **누적되지 않는다**

### 7-2. No.34 최대 전류 반복

```c
if(s->BAPackOCOld == 0)                            // 480 A 진입 순간(edge)만
{
    if(s->BAPackOCEventTimer == 0)
    {
        s->BAPackOCEventTimer = C_PackOCEventWindow;   // 60000 tick = 60 s
        s->BatFalutCont[18] = 0;
    }
    if(s->BatFalutCont[18] <  C_PackOCEventCount){ ++s->BatFalutCont[18]; }
    if(s->BatFalutCont[18] >= C_PackOCEventCount){ SET; }   // 5 회 이상
}
s->BAPackOCOld = 1;
```

| 요소 | 동작 |
|---|---|
| 이벤트 정의 | 전류가 480 A **미만 → 이상**으로 올라서는 순간 1 회 |
| 창 시작 | **첫 이벤트** 시점부터 60.000 초 고정 창 |
| 판정 | 창 안에서 **5 회째** 이벤트가 발생하는 즉시 |
| 창 만료 | 60 초 경과 시 횟수 0, 다음 진입에서 새 창 시작 |
| 창 진행 | 전류 수준과 무관하게 매 tick 감소 |

**고정 창**이므로 「59 초에 4 회 + 61 초에 1 회」는 걸리지 않는다. 슬라이딩 창이 필요하면 별도 설계가 필요하다.

### 7-3. No.8 · 23 전류한계 초과

| 구분 | 값 | 상태 |
|---|---|---|
| 초과분 산출 | `Bat80VUnbalCurrentF = 측정전류 − 연속한계` | `CalP56CurrentLimit`, 100 ms |
| 충전(전류 +) | `Bat80VCurrentF − Bat80VCHAContintyCurrF` | [BATAlgorithm.c:774](SysSoure/BATAlgorithm.c#L774) |
| 방전(전류 −) | `Bat80VCurrentAsbF − Bat80VDisCHAContintyCurrF` | [BATAlgorithm.c:778](SysSoure/BATAlgorithm.c#L778) |
| No.8 경고 | 초과분 > 0 이 **10.000 초** 지속 | **활성** |
| No.23 차단 | 초과분 > 0 이 **15.001 초** 지속 | **차단 중** (261220 이후) |

연속한계는 온도·SOC 표를 2 차원 보간해 얻으므로 **운전 중 계속 변한다**. 초과분이 음수로 바뀌면 경고는 즉시 풀린다.

---

## 8. 설정표(R9) 대비 차이 — 반영 요청

### 8-1. 조건 표기 : 「초과/미만」 → 「이상/이하」

R9 §2 는 `Hyst_On = (V) > (S)` 초과, `Hyst_Off = (V) < (R)` 미만으로 적혀 있으나
구현은 `>=` **이상**, `<=` **이하**다. **경계값에서 판정이 갈린다.**

| 예시 | 설정표 해석 | 실제 동작 |
|---|---|---|
| 셀 과전압 경고 4.15 V | 4.150 에서 **미발생** | 4.150 에서 **발생** |
| 셀 저전압 경고 3.00 V | 3.000 에서 미발생 | 3.000 에서 **발생** |
| 팩 저온 차단 −27.0 ℃ | −27.0 에서 미발생 | −27.0 에서 **발생** |

시험 시 경계값을 정확히 인가하면 결과가 달라지므로, **설정표 §2 를 「이상 / 이하」로 정정**하거나
구현을 `>` / `<` 로 바꿔야 한다. 현재 30 개 항목이 모두 경계 포함 방식이므로 **설정표 정정을 권한다**.

### 8-2. 차단 지연 「즉시 = 0」 → 실제 1 ms

5-3 항. 구조상 최소 1 tick 이 필요하다. 설정표에 「즉시(1 ms 이내)」로 표기하면 오해가 없다.

### 8-3. No.8 해제 조건

R9 §3 은 해제를 「0 Sec 미만」으로 적었으나, 구현은 **초과분이 0 이하가 되면 지연 없이 해제**한다.
지속시간을 세는 개념이 아니라 초과 여부만 보므로, 표기를 「초과 해소 시 즉시 해제」로 바꾸는 편이 정확하다.

### 8-4. 경고·차단이 같은 지점인 항목

| 물리량 | 경고 | 차단 | 상태 |
|---|---|---|---|
| 팩 과전압 | 셀 경고 4.150 (= 91.3 V) | **팩 차단 91.3 V** | 260901 결정으로 유지. `bit8` · `bit19` 동시 SET |
| 저전압 | 셀 차단 2.650 | 팩 차단 58.3 V (= 2.650 × 22S) | 백업 계층, 의도됨 |

두 경우 모두 **두 비트가 같은 시점에 SET** 된다. 시험 판정 시 이상으로 보지 않는다.

---

## 9. 계층 검증 (Ver 0.21 실측 기준)

셀 기준으로 환산해 배열한 결과다. 팩 값은 22 로 나눈 값을 병기했다.

| 물리량 | 계층 | 판정 |
|---|---|:--:|
| 과전압 | 팩 경고 4.1300 → **셀 경고 4.1500 = 팩 차단 4.1500** → 셀 차단 4.2000 | 겹침 1 |
| 저전압 | 셀 경고 3.0000 → 팩 경고 2.9750 → **셀 차단 2.6500 = 팩 차단 2.6500** | 겹침 1 (의도) |
| 고온 | 팩 경고 47 → 팩 차단 52 → 셀 경고 55 → 셀 차단 60 | 정상 |
| 저온 | 팩 경고 −20 → 셀 경고 −25 → 팩 차단 −27 → 셀 차단 −30 | 정상 |
| SOC | 경고 5 / 95 % → 차단 0 / 100 % | 정상 |
| 전류 | 경고 450 → 시간 480(10 s) → 반복 480(5 회/min) → 차단 505 A | 정상 |
| 전압 편차 | 경고 200 → 차단 400 mV | 정상 |
| 온도 편차 | 경고 10 → 차단 20 ℃ | 정상 |

---

## 10. 시험 시 유의 사항

| # | 항목 | 내용 |
|--:|---|---|
| 1 | 경계값 인가 | 임계값을 **정확히** 인가하면 발생한다(이상/이하). 미발생을 확인하려면 1 LSB 아래로 인가 |
| 2 | 경고 지연 | 조건 성립 후 **정확히 100 ms** 에 SET. 계측 분해능이 100 ms 보다 크면 관측 불가 |
| 3 | 차단 지연 | 1 ms. 사실상 조건 성립과 동시로 관측된다 |
| 4 | 경고 해제 | 지연 없이 즉시. 해제임계를 넘나들면 채터링 가능 |
| 5 | 중간 구간 | 발생임계와 해제임계 사이에서는 **직전 상태 유지**. 이 구간 값으로는 상태가 바뀌지 않는다 |
| 6 | 온도 항목 | 신호 갱신 0.30 s → 하강 속도 0.5 ℃/s 시 계단 0.15 ℃. R9 §1.2 조치 유지 |
| 7 | No.34 | 고정 창이므로 창 경계를 걸친 이벤트는 합산되지 않음 |
| 8 | 차단 4 건 | No.23 · 30 · 32 · 36 은 비트가 강제 0. **시험 대상 제외** |
| 9 | 차단 복귀 | 자동 해제 없음. `Ifcu_BSAReset`(0x450) 또는 전원 재투입 필요 |

---

## 11. 근거 코드 위치

| 대상 | 위치 |
|---|---|
| 비교 매크로 | [DSP28x_Project.h:85-86](C2806Xinclude/DSP28x_Project.h#L85-L86) |
| 경고 판정 | [DSP28x_Project.c:455~](SysSoure/DSP28x_Project.c#L455) `Cal80VSysAlarmtCheck` |
| 차단 판정 | [DSP28x_Project.c:760~](SysSoure/DSP28x_Project.c#L760) `Cal80VSysFaultCheck` |
| 전류한계·초과분 | [BATAlgorithm.c:774-778](SysSoure/BATAlgorithm.c#L774-L778) `CalP56CurrentLimit` |
| 통신 끊김 | [main.c:1227-1244](SysSoure/main.c#L1227-L1244) |
| 판정 호출 | [main.c:1013-1029](SysSoure/main.c#L1013-L1029) |
| 설정값 | [parameter.h](SysInclude/parameter.h) — `//Fault Set Vaule` · `//Warning(Alarm) Set Vaule` 절 |
