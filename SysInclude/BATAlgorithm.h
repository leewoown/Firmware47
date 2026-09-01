/* ==============================================================================
System Name:  현대자동차 수소 지게차 80V

File Name:		PARAMETER.H

Description:	현대
          	    Orientation Control for a Three Phase AC Induction Motor. 

Originator:		Digital control systems Group - Texas Instruments

Note: In this software, the default inverter is supposed to be DMC1500 board.
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
=================================================================================  */

//#include "F2806x_Cla_typedefs.h"// F2806x CLA Type definitions
//#include "F2806x_Device.h"      // F2806x Headerfile Include File
//#include "F2806x_Examples.h"    // F2806x Examples Include File
//#include "DSP28x_Project.h"

#ifndef BAT_Algorithm_H
#define BAT_Algorithm_H

#include "F2806x_Cla_typedefs.h"// F2806x CLA Type definitions
#include "F2806x_Device.h"      // F2806x Headerfile Include File
#include "F2806x_Examples.h"    // F2806x Examples Include File
#include "DSP28x_Project.h"

#define Kokam100Ah            0
#define Kokam60Ah             0
#define FarasisP56Ah          1
#define FarasisP52Ah          0
#define FarasisP40Ah          0
#define Frey60Ah              0


/*========================================
 * SOC 연산 공통 설정 (셀 변경 시 수정 없음)
 *========================================*/
// TODO(검증 후 삭제): OCV 재보정 제거로 미사용. 동작 검증 후 제거 예정
#define C_SocReInitCurrentAbsF    1.0F        // 저전류 판단 기준 (시스템별 튜닝 가능)
/*--------------------------------------------------------------------
 * 전류 적분 deadband
 *   |I| < 이 값 → 측정 노이즈로 간주, Coulomb Counting 적분 차단
 *
 *   현재 값 0.5A : Honeywell CSNV700 (Flux Gate, ±700A) 기준
 *     - 오프셋 ±0.05A (±3σ)  →  약 10σ 마진
 *     - 저전류 오차 ±0.1A (|I|≤20A, ±3σ)
 *
 *   ※ 다른 전류 센서로 교체 시 :
 *     해당 센서 오프셋 사양 × 약 10배로 재계산
 *     예) LEM HASS, Tamura 등 — 데이터시트의 offset error(±σ) 확인
 *-------------------------------------------------------------------*/
#define C_SocCurrentDeadbandF     0.5F        // [A] 적분 노이즈 차단 임계값
#define C_SocCumulativeTime       0.00027778F // 1/3600 (고정)
#define C_CTSampleTime            0.05F       // 50msec (고정)
#define C_SocSamPleCount          50u         // ISR 1ms 기준 (고정)
// TODO(검증 후 삭제): OCV 재보정 제거로 미사용. 동작 검증 후 제거 예정
#define C_SocReInitDelayCount     6000u       // 저전류 유지 시간 (시스템 튜닝 가능)



#if FarasisP56Ah
/*========================================
 * 🔥 셀 변경 시 반드시 확인/수정 필요
 *========================================*/
#define C_SocAvailableCapacityAh  45.0F       // ⚠️ [셀 변경 시 수정] 유효 용량 (DoD 기준)
#define C_SocNorm                 (1.0F / C_SocAvailableCapacityAh)

/*========================================
 * 🔥 셀 변경 시 반드시 수정 (OCV-SOC 곡선)
 *========================================*/
#define C_CellOCVToSOC_X2        -138.71F     // ⚠️ [셀 변경 시 수정] OCV→SOC 계수
#define C_CellOCVToSOC_X1        1195.70F     // ⚠️ [셀 변경 시 수정]
#define C_CellOCVToSOC_X0       -2472.30F     // ⚠️ [셀 변경 시 수정]

/*========================================
 * 🔥 셀 특성 따라 변경 가능 (권장)
 *========================================*/
#define C_SocOCVLinearMinF       20.0F        // ⚠️ [셀 특성 따라 조정] 선형구간 시작 SOC
#define C_SocOCVLinearMaxF       80.0F        // ⚠️ [셀 특성 따라 조정] 선형구간 끝 SOC

/*--------------------------------------------------------------
 * 260831 : 재기동 SOC 점프 대책(F-5) — 차단 직전 셀전압과 부팅 시 셀전압이
 *          거의 같으면 휴지 중 상태 변화가 없었다고 보고 NVR 값을 이어받는다.
 *          휴지시간을 재지 않으므로 RTC 등 추가 HW 가 필요 없다.
 *--------------------------------------------------------------*/
#define C_SocRestCellVoltDiffmV   5.0F        /* [mV] 이 이하면 NVR 우선 채택 */         // TODO : [검증] 260831_Note1, 0.18 F-5

/*--------------------------------------------------------------
 * 260831 : 운전 중 OCV 완만 보정(F-6) 진입 조건과 보정 속도.
 *          Relaxation 미수렴 전압으로 초기화된 오차를 무부하 구간에서
 *          천천히 되돌린다. 아래 조건을 모두 만족할 때만 동작한다.
 *--------------------------------------------------------------*/
#define C_SocOcvAdjRestCount      36000UL     /* 무부하 1800s = 50ms x 36000 */          // TODO : [검증] 260831_Note1, 0.18 F-6
#define C_SocOcvAdjVoltWinCount   2400UL      /* 전압 수렴 판정창 120s = 50ms x 2400 */  // TODO : [검증] 260831_Note1, 0.18 F-6
#define C_SocOcvAdjVoltSettlemV   2.0F        /* [mV] 120s 변화량 임계 */                // TODO : [검증] 260831_Note1, 0.18 F-6
#define C_SocOcvAdjCellDivF       0.05F       /* [V] 셀 편차 상한 50mV */                // TODO : [검증] 260831_Note1, 0.18 F-6
#define C_SocOcvAdjTempMinF       10.0F       /* [degC] OCV 표 신뢰 하한 */              // TODO : [검증] 260831_Note1, 0.18 F-6
#define C_SocOcvAdjTempMaxF       40.0F       /* [degC] OCV 표 신뢰 상한 */              // TODO : [검증] 260831_Note1, 0.18 F-6
#define C_SocOcvAdjDeadBandF      1.0F        /* [%p] 이 이내면 보정 안 함 */            // TODO : [검증] 260831_Note1, 0.18 F-6
#define C_SocOcvAdjStopBandF      0.5F        /* [%p] 히스테리시스 정지 */               // TODO : [검증] 260831_Note1, 0.18 F-6
#define C_SocOcvAdjRateF          0.0005F     /* [%p/50ms] = 0.01 %p/s */                // TODO : [검증] 260831_Note1, 0.18 F-6
#define C_SocOcvAdjMaxPerRestF    5.0F        /* [%p] 1회 무부하 인터벌 상한 */          // TODO : [검증] 260831_Note1, 0.18 F-6

/*========================================
 * OCV-SOC Lookup Table (정밀도 ±0.5%)
 *========================================*/
typedef struct
{
    float32 OCV;       /* 셀 평균 전압 (V) */
    float32 SysSOC;    /* 시스템 SOC (%) */
} OCVtoSOC_Point;

#define P56AH_OCV_TABLE_SIZE   17
//extern void   CellP56AhSocInit(SocReg *P);
extern const OCVtoSOC_Point P56AhOCVTable[P56AH_OCV_TABLE_SIZE];
extern float32 OCVtoSOC_P56Ah(float32 ocv);

#endif

#if FarasisP52Ah
extern void CalFarasis52AhRegsInit(SocReg *P);
extern void CalFarasis52AhSocInit(SocReg *P);
extern void CalFarasis52AhSocHandle(SocReg *P);
#define C_Farasis52Ah_SOCX2     -138.71
#define C_Farasis52Ah_SOCX1     1195.70
#define C_Farasis52Ah_SOCX0    -2472.30
#define C_FarasisP52AhNorm      0.0238//1/42Ah
#endif

#if FarasisP40Ah
#define C_FarasisP40AhNorm      0.0312//1/32Ah
extern void CalFarasis40AhRegsInit(SocReg *P);
extern void CalFarasis40AhSocInit(SocReg *P);
extern void CalFarasis40AhSocHandle(SocReg *P);
#endif


/*
 *  Frey60Ah
 */
#if Frey60Ah
#define LFP_VOLT_A_BOT   3.050
#define LFP_VOLT_A_TOP   3.211
#define LFP_VOLT_B_BOT   3.211
#define LFP_VOLT_B_TOP   3.294
#define LFP_VOLT_C_BOT   3.294
#define LFP_VOLT_C_TOP   3.317
#define LFP_VOLT_D_BOT   3.317
#define LFP_VOLT_D_TOP   3.337
#endif


typedef enum
{
  SOC_STATE_IDLE,
  SOC_STATE_RUNNING,
  SOC_STATE_Save,
  SOC_STATE_CLEAR
} SoCState;
struct SoCSate_BIT
{       // bits   description
   unsigned int     CalMeth         :1; // 0
   unsigned int     State01         :1; // 1
   unsigned int     State02         :1; // 2
   unsigned int     State03         :1; // 3
   unsigned int     State04         :1; // 4
   unsigned int     State05         :1; // 5
   unsigned int     State06         :1; // 6
   unsigned int     State07         :1; // 7
   unsigned int     State08         :1; // 8
   unsigned int     State09         :1; // 9
   unsigned int     State10         :1; // 10
   unsigned int     State11         :1; // 11
   unsigned int     State12         :1; // 12
   unsigned int     State13         :1; // 13
   unsigned int     State14         :1; // 14
   unsigned int     State15         :1; // 15
};
union SoCState_REG
{
   unsigned int     all;
   struct SoCSate_BIT bit;
};
typedef struct
{
  /*
   *
   */
  unsigned int CTCount;
  unsigned int SysTime;
  unsigned int SocInitMode;
  SoCState state;
  float32 SysSOCdtF;
  float32 SysSoCCTF;
  float32 SysSoCCTAbsF;
  float32 SysAhNewF;
  float32 SysAhOldF;
  float32 SysAhF;
  float32 SysSOCBufF1;
  float32 SysSOCBufF2;
  float32 SysSOCF;
  float32 NVRSocInitF;
  float32 delta;
  /*
   *
   */
  float32  AVGXF;
  float32  SOCX4InF;
  float32  SOCX3InF;
  float32  SOCX2InF;
  float32  SOCX1InF;

  float32  SOCX4OutF;
  float32  SOCX3OutF;
  float32  SOCX2OutF;
  float32  SOCX1OutF;
/*
 *
 */
  float32  SOCX4InFAZore;
  float32  SOCX3InFAZore;
  float32  SOCX2InFAZore;
  float32  SOCX1InFAZore;
  float32  SOCX4OutFAZore;
  float32  SOCX3OutFAZore;
  float32  SOCX2OutFAZore;
  float32  SOCX1OutFAZore;
  Uint16   AZoreCalCout;



  float32  SOCX4InFBZore;
  float32  SOCX3InFBZore;
  float32  SOCX2InFBZore;
  float32  SOCX1InFBZore;
  float32  SOCX4OutFBZore;
  float32  SOCX3OutFBZore;
  float32  SOCX2OutFBZore;
  float32  SOCX1OutFBZore;
  Uint16   BZoreCalCout;


  float32  SOCX4InFCZore;
  float32  SOCX3InFCZore;
  float32  SOCX2InFCZore;
  float32  SOCX1InFCZore;
  float32  SOCX4OutFCZore;
  float32  SOCX3OutFCZore;
  float32  SOCX2OutFCZore;
  float32  SOCX1OutFCZore;
  Uint16   CZoreCalCout;

  float32  SOCX4InFDZore;
  float32  SOCX3InFDZore;
  float32  SOCX2InFDZore;
  float32  SOCX1InFDZore;
  float32  SOCX4OutFDZore;
  float32  SOCX3OutFDZore;
  float32  SOCX2OutFDZore;
  float32  SOCX1OutFDZore;
  Uint16   DZoreCalCout;


  float32  SOCbufF;
  float32  SysSocInitF;
  float32  CellAgvVoltageF;

  /*--------------------------------------------------------------
   * 260831 : 재기동 SOC 점프 대책(F-5) 및 운전 중 완만 보정(F-6) 입력·상태.
   *          온도·셀편차·시스템 상태는 main 루프에서 매 주기 채워 넣는다.
   *--------------------------------------------------------------*/
  float32  RestVoltDiffF;      /* [mV] 부팅 시 셀 평균전압 - NVR LastCellV */   // TODO : [검증] 260831_Note1, 0.18 F-5
  Uint16   NvrAdopted;         /* 1 = 부팅 SOC 로 NVR 값을 채택 */              // TODO : [검증] 260831_Note1, 0.18 F-5

  float32  CellTempF;          /* [degC] 셀 평균온도 */                          // TODO : [검증] 260831_Note1, 0.18 F-6 입력
  float32  CellDivVoltF;       /* [V] 셀 전압 편차 */                            // TODO : [검증] 260831_Note1, 0.18 F-6 입력
  Uint16   SysStateNo;         /* System_STATE_* 값 */                           // TODO : [검증] 260831_Note1, 0.18 F-6 입력

  Uint32   RestTimeCount;      /* 무부하 지속 50ms 카운트 */                     // TODO : [검증] 260831_Note1, 0.18 F-6
  Uint32   VoltChkCount;       /* 전압 수렴 판정 창(120s) 카운트 */              // TODO : [검증] 260831_Note1, 0.18 F-6
  float32  VoltRefF;           /* [V] 120s 전 셀 평균전압 */                     // TODO : [검증] 260831_Note1, 0.18 F-6
  float32  VoltSettleF;        /* [mV] 120s 간 전압 변화량 */                    // TODO : [검증] 260831_Note1, 0.18 F-6
  float32  OcvAdjErrF;         /* [%p] OCV 대비 SOC 오차 */                      // TODO : [검증] 260831_Note1, 0.18 F-6
  float32  OcvAdjUsedF;        /* [%p] 이번 무부하 인터벌 누적 보정량 */          // TODO : [검증] 260831_Note1, 0.18 F-6
  Uint16   OcvAdjActive;       /* 1 = 완만 보정 진행 중 */                       // TODO : [검증] 260831_Note1, 0.18 F-6
  Uint16   OcvAdjLimitFlag;    /* 1 = 1회 보정 한계(5%p) 도달로 중단 */          // TODO : [검증] 260831_Note1, 0.18 F-6




  /*--------------------------------------------------------------
   * 260901 : P56 안전 전류제한 I/O 를 SocReg 로 이관 (CalP56CurrentLimit).
   *          호출부(main.c)에서 SysRegs 와 이 멤버들 사이 입출력 복사.
   *--------------------------------------------------------------*/
  float32  Bat80VCellAgvTemperatureF;   // TODO : [검증] 260901_Note1, 0.19 P56 입력: 셀 평균 온도[C]
  float32  Bat80VSOCF;                  // TODO : [검증] 260901_Note1, 0.19 P56 입력: 팩 SOC[%]
  float32  Bat80VCurrentF;              // TODO : [검증] 260901_Note1, 0.19 P56 입력: 팩 전류[A]
  float32  Bat80VCurrentAsbF;           // TODO : [검증] 260901_Note1, 0.19 P56 입력: 팩 전류 절대값[A]
  float32  Bat80VDisCHAPeakFCurrF;      // TODO : [검증] 260901_Note1, 0.19 P56 출력: 5s 방전 한계[A]
  float32  Bat80VCHAPeakFCurrF;         // TODO : [검증] 260901_Note1, 0.19 P56 출력: 5s 충전 한계[A]
  float32  Bat80VDisCHAContintyCurrF;   // TODO : [검증] 260901_Note1, 0.19 P56 출력: 연속 방전 한계[A]
  float32  Bat80VCHAContintyCurrF;      // TODO : [검증] 260901_Note1, 0.19 P56 출력: 연속 충전 한계[A]
  float32  Bat80VUnbalCurrentF;         // TODO : [검증] 260901_Note1, 0.19 P56 출력: 연속한계 초과분[A]

  union SoCState_REG SoCStateRegs;
} SocReg;





#endif  // end of PARAMETER.H definition


//===========================================================================
// No more.
//===========================================================================
