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
//#include "build.h"
//#include "math.h"
//#include "IQmathLib.h"
#include "F2806x_Cla_typedefs.h"// F2806x CLA Type definitions
#include "F2806x_Device.h"      // F2806x Headerfile Include File
#include "F2806x_Examples.h"    // F2806x Examples Include File
#include "DSP28x_Project.h"


#ifndef PARAMETER_H
#define PARAMETER_H

/* Bit 위치 정의(주로 위에서 정의한 매크로 함수에서 사용하기 위해 정의함) */
#define BIT0_POS    	0
#define BIT1_POS    	1
#define BIT2_POS    	2
#define BIT3_POS    	3
#define BIT4_POS    	4
#define BIT5_POS    	5
#define BIT6_POS    	6
#define BIT7_POS    	7
#define BIT8_POS    	8
#define BIT9_POS    	9
#define BIT10_POS   	10
#define BIT11_POS   	11
#define BIT12_POS   	12
#define BIT13_POS   	13
#define BIT14_POS   	14
#define BIT15_POS   	15

/* Bit Mask Data 정의 */
#define	BIT0_MASK    	0x0001
#define	BIT1_MASK    	0x0002
#define	BIT2_MASK    	0x0004
#define	BIT3_MASK    	0x0008
#define	BIT4_MASK    	0x0010
#define	BIT5_MASK    	0x0020
#define	IT6_MASK    	0x0040
#define	BIT7_MASK    	0x0080
#define	BIT8_MASK    	0x0100
#define	BIT9_MASK    	0x0200
#define	BIT10_MASK   	0x0400
#define	BIT11_MASK   	0x0800
#define	BIT12_MASK   	0x1000
#define	BIT13_MASK   	0x2000
#define BIT14_MASK   	0x4000
#define BIT15_MASK   	0x8000

#define	SCIA_BUFRX		50			// Monstar와 맞추어야 함

#define UL_BYTE(x)		     (x >> 16)
#define HI_BYTE(x)		     (x >> 8)
#define LO_BYTE(x)           (x & 0xff)
#define MAKE_WORD(msb,lsb)	 ((msb<<8) | (lsb))
#define WordLShift(md,ml)    (md<<ml)
#define WordRShift(md,ml)    (md>>ml)



// Define the ISR frequency (kHz)
#define ISR_FREQUENCY 	       1
#define SYSTEM_FREQUENCY       80
#define CPUCLK			       80000000L					// CPU Main Clock
//#define CPLDCLK			   100000000L					// CPLD Clock

#define CPU_CLOCK_SPEED        12.5L                   // for a 80MHz CPU clock speed
//#define ADC_usDELAY 	    5000L



#define	Uint16Max		    65535U


/*
#define TxA_RDY_flag         SciaRegs.SCICTL2.bit.TXRDY
#define TxB_RDY_flag	     ScibRegs.SCICTL2.bit.TXRDY
#define TxC_RDY_flag         ScicRegs.SCICTL2.bit.TXRDY
#define TxA_Empty_flag       SciaRegs.SCICTL2.bit.TXEMPTY
#define TxB_Empty_flag       ScibRegs.SCICTL2.bit.TXEMPTY
#define TxC_Empty_flag	     ScicRegs.SCICTL2.bit.TXEMPTY
#define AD_START   	   	     AdcRegs.ADCTRL2.bit.SOC_SEQ1
#define IS_AD_BUSY 	         AdcRegs.ADCST.bit.SEQ1_BSY
*/

/*
 * LED00 indicates SYSTEM STATE status
 */
#define LEDSysState_H            GpioDataRegs.GPBSET.bit.GPIO58=1 //System Fault State  LED
#define LEDSysState_L            GpioDataRegs.GPBCLEAR.bit.GPIO58=1
#define LEDSysState_T            GpioDataRegs.GPBTOGGLE.bit.GPIO58=1

/*
 *  LED01 indicates SYSTEM Fault status
 */
#define LEDFault_H              GpioDataRegs.GPBSET.bit.GPIO44=1
#define LEDFault_L              GpioDataRegs.GPBCLEAR.bit.GPIO44=1
#define LEDFault_T              GpioDataRegs.GPBTOGGLE.bit.GPIO44=1

/*
 * LED02 indicates SYSTEM CANSTAT status
 */

#define LEDCANState_H            GpioDataRegs.GPBSET.bit.GPIO51=1
#define LEDCANState_L            GpioDataRegs.GPBCLEAR.bit.GPIO51=1
#define LEDCANState_T            GpioDataRegs.GPBTOGGLE.bit.GPIO51=1


/*
 * DIP SW
 */
#define IDSW01           GpioDataRegs.GPADAT.bit.GPIO16
#define IDSW02           GpioDataRegs.GPADAT.bit.GPIO17

/*
 * SPI chip RTC CS, MFP DIO
 */

#define RTC_CS            GpioDataRegs.GPACLEAR.bit.GPIO9=1
#define RTC_DS            GpioDataRegs.GPASET.bit.GPIO9=1
#define RTC_MF            GpioDataRegs.GPCDAT.bit.GPIO8
/*--------------------------------------------------------------
 * 260910 : SPI 대기 루프 타임아웃 상한. 타임아웃이 없던 탓에
 *          부팅 중 NVRAM 접근에서 멈추면 main 의 while(1) 진입이
 *          안 되어 BATIC(LTC6804) 통신이 시작조차 못 했다.
 *          8bit @328kHz = 약 24us 인데 60000 회는 약 4~7ms 로
 *          정상 전송에는 걸리지 않는 여유값이다. (Uint16 상한 내)
 *--------------------------------------------------------------*/
#define C_SpiWaitTimeout                   60000U   // TODO : [검증] 260910_Note1, 0.21 SPI 대기 루프 타임아웃 상한

/*--------------------------------------------------------------
 * 260910 : 진단 카운터 상한. 규약 R14 에서 0x608 의 카운터가
 *          8bit(0~200) 로 재편되어, 송신값이 255 를 넘지 않도록
 *          200 에서 포화시킨다(리셋 아님 — 이상 이력을 유지).
 *--------------------------------------------------------------*/
#define C_SpiTimeoutMax                      200U   // TODO : [검증] 260910_Note1, 0.21 SpiTimeoutCount 포화 상한 (8bit CAN 송신)
#define C_IsoSpiErrLimit                     200    // TODO : [검증] 260910_Note1, 0.21 PackISO_ERR 판정 임계 (ltc_error_count 연속 실패)

/*
 * SPI chip NVRAM CS
 */
#define NvramCS            GpioDataRegs.GPBCLEAR.bit.GPIO11=1
#define NvramDS            GpioDataRegs.GPBSET.bit.GPIO11=1

/*
 *  SPI TCP IP CS, REST, RDY, INT
 */
#define TcpCS             GpioDataRegs.GPBCLEAR.bit.GPIO42=1
#define TcpDS             GpioDataRegs.GPBSET.bit.GPIO42=1

#define TcpResetOn        GpioDataRegs.GPACLEAR.bit.GPIO15=1
#define TcpResetOff       GpioDataRegs.GPASET.bit.GPIO15=1

#define TcpINT            GpioDataRegs.GPADAT.bit.GPIO13

/*
 *  SPI CAN FOR EN, CANINT, CANRX0INT, CANRX1INT,
 */
#define CANBCS             GpioDataRegs.GPBCLEAR.bit.GPIO43=1
#define CANBDS             GpioDataRegs.GPBSET.bit.GPIO43=1

#define CANINT             GpioDataRegs.GPADAT.bit.GPIO12=1
#define CANRX0INT          GpioDataRegs.GPADAT.bit.GPIO14
#define CANRX1INT          GpioDataRegs.GPBDAT.bit.GPIO52

/*
 * RS845 EN
 */
#define RS485EN            GpioDataRegs.GPBCLEAR.bit.GPIO50=1
#define RS485DS            GpioDataRegs.GPBSET.bit.GPIO50=1
/*
 *  BAT IC EN
 */
#define BATEN             GpioDataRegs.GPACLEAR.bit.GPIO10=1
#define BATDS             GpioDataRegs.GPASET.bit.GPIO10=1

/*
 * 80VBAT PROTECT Relay OUT, AUX
 */

#define PRlyOn              GpioDataRegs.GPASET.bit.GPIO22=1
#define PRlyOff             GpioDataRegs.GPACLEAR.bit.GPIO22=1
#define PRlyState           GpioDataRegs.GPADAT.bit.GPIO23

#define NRlyOn              GpioDataRegs.GPASET.bit.GPIO6=1
#define NRlyOff             GpioDataRegs.GPACLEAR.bit.GPIO6=1
#define NRlyState           GpioDataRegs.GPADAT.bit.GPIO7

#define CHARlyOn            GpioDataRegs.GPBSET.bit.GPIO32=1
#define CHARlyOff           GpioDataRegs.GPBCLEAR.bit.GPIO32=1
#define CHARlyState         GpioDataRegs.GPBDAT.bit.GPIO33


#define FAN_ON              GpioDataRegs.GPASET.bit.GPIO20=1
#define FAN_OFF             GpioDataRegs.GPACLEAR.bit.GPIO20=1

#define PWRHOLD_ON          GpioDataRegs.GPASET.bit.GPIO21=1
#define PWRHOLD_OFF         GpioDataRegs.GPACLEAR.bit.GPIO21=1

/* PWRHOLD(전원 유지) 제어 — VCU RUNStatus + VCU 통신 상태 기반 (PWRHoldHandle) */
#define C_VcuCommLostCount      10u     // SysCanRxCount 기준: 100ms×10 = 1초 미수신 → VCU 통신 끊김
#define C_PwrHoldOffDelayCount  300000UL  // 1ms×300000 = 5분 유지 후 PWRHOLD_OFF (C2000 int=16bit → UL 필수)

/*
 * Insulation Resistance Measurement, IMDTopON. IMDTopOFF,IMDBOTOn,IMDBOTOff
 */

#define IMDTOPOn           GpioDataRegs.GPASET.bit.GPIO26=1
#define IMDTopOff          GpioDataRegs.GPACLEAR.bit.GPIO26=1
#define IMDBOTOn           GpioDataRegs.GPASET.bit.GPIO27=1
#define IMDBOTOff          GpioDataRegs.GPACLEAR.bit.GPIO27=1



//------------------------------------------------------------------------------------------------
//#define A_PTR(y)			*(volatile unsigned int *)(y)
//#define FPGA_Addr(X)		*(volatile unsigned int *)(0x4000+X) 
//#define PARA_Addr(X)		*(volatile unsigned int *)(0x4200+X)

#define PBYTE(X)                *(volatile unsigned char      *)(X)
#define PWORD(X)                *(volatile unsigned int       *)(X)
#define PLONG(X)                *(volatile unsigned long      *)(X)
#define PLLONG(X)               *(volatile unsigned long long *)(X)

#define SysRegTimer5msec     4
#define SysRegTimer10msec    9
#define SysRegTimer50msec    49
#define SysRegTimer100msec   99
#define SysRegTimer300msec   299
#define SysRegTimer500msec   499
#define SysRegTimer1000msec  1000
#define CellVoltsampling100msec 100

/*-------------------------------------------------------------------------------
 TMS320F28069 CLK SET UP 
-------------------------------------------------------------------------------*/
#define	CPUCLK				    80000000L							// CPU Main Clock
/*-------------------------------------------------------------------------------
 TMS320F28069 CLK SET UP 
-------------------------------------------------------------------------------*/
#define	SCIA_LSPCLK				(CPUCLK/4)							// Peripheral Low Speed Clock for SCI-A
#define	SCIA_BAUDRATE			9600L								// SCI-A Baudrate
#define	SCIA_BRR_VAL			(SCIA_LSPCLK/(8*SCIA_BAUDRATE)-1)	// SCI-A BaudRate 설정 Register 값

#define	SCIB_LSPCLK				(CPUCLK/4)							// Peripheral Low Speed Clock for SCI-B
#define	SCIB_BAUDRATE			9600L								// SCI-B Baudrate
#define	SCIB_BRR_VAL			(SCIB_LSPCLK/(8*SCIB_BAUDRATE)-1)	// SCI-B BaudRate 설정 Register 값

#define	SCIC_LSPCLK				(CPUCLK/4)							// Peripheral Low Speed Clock for SCI-C
#define	SCIC_BAUDRATE			9600L								// SCI-C Baudrate
#define	SCIC_BRR_VAL			(SCIC_LSPCLK/(8*SCIC_BAUDRATE)-1)	// SCI-C BaudRate 설정 Register 값

/*-------------------------------------------------------------------------------
Parameter
-------------------------------------------------------------------------------*/

// Define the Power Source Parameter
#define PI 							3.14159265358979
#define PIn							-3.14159265358979
#define PI2							6.283185307
#define WE							376.9911184
#define AdcNormalizerBipolar        0.00048828125           // 1 / 4096으로 나눗값
#define AdcNormalizerUnipolar       0.000244140625          // 1 / 2048으로 나눗값
#define AdcNormalizerpolar          0.000322997416          // 1 / 3096으로 나눗값
#define Inverse3					0.333333333			//1/3
#define InverseSQRT3				0.577350269			//1/root3
#define SQRT3						1.732050808			//root3
#define WL_Grid				 		0.150796447368		// 2pi * 60Hz * 400uH
#define TwoBySQRT3 					1.154700			// TwoBySQRT3   = 2/root3
#define Vdc_Minimum					50.0
#define Inverse_Vdc_Minimum			0.02
#define SectoHour                   0.00027778 // 1(h)/3600(sec)
#define Func_Hz                     20 // 1(h)/3600(sec)


/*
 * 162S1P, BATTERY PACK Protect Parameter setup
 */

//#define     Pack_ID                     1


/*
 *
    000 (0) : Battery system initial
    001 (1) : Battery System Ready
    010 (2) : Battery system charging
    011 (3) : Battery system discharging
    100 (4) : Battery system Balancing
 */


#define     Product_SysCellVauleS                22
#define     Product_SysCellVauleP                1
//#define     Product_Voltage                    768  // 3.664*22
//#define     Product_Capacity                   450  //
#define     Product_Voltage                      809   // TODO : [튜닝] 260809_Note1, 0.16 R11 Normal_Volt 80.96V(22S×3.68≈809)
#define     Product_Capacity                     564   // TODO : [튜닝] 260809_Note1, 0.16 R11 Capacity 56.4Ah(564)
#define     Product_Type                         0     // TODOS 26.07.02 TEST 버전0, 양상버전 1
//#define     Product_Version                    18   // 이전값(원복 전)
//#define     Product_Version                    16   // TODO : [변경] 260901_Note1, 0.16 Product_Version 18->16 (VER 0.16)
//#define     Product_Version                    17   // TODO : [변경] 260901_Note1, 0.17 Product_Version 16->17 (VER 0.17, R12 반영)
//#define     Product_Version                    18   // TODO : [변경] 260901_Note1, 0.18 Product_Version 17->18 (BATIC SPI 설정 기입 추가)
//#define     Product_Version                    19   // TODO : [변경] 260901_Note1, 0.19 Product_Version 18->19 (보호설정표 R9 반영)
//#define     Product_Version                    20   // (0.20 커밋 완료분: 7be1092)
//#define     Product_Version                    21   // (0.21 baseline — BATIC SPI 통신 불능 발생, V0.22 에서 복구)
//#define     Product_Version                    22   // (0.22 커밋 완료분: 64c69f3)
#define     Product_Version                      23   // TODO : [변경] 260910_Note1, 0.23 Product_Version 22->23 (NVRAM 지연초기화 + isoSPI 판정 보완 + 0x608 R14)
/*--------------------------------------------------------------
 * 260831 : 디버깅보드 시험모드 해제 — 모사장치 CAN 대신 실기 isoSPI 사용.
 *          셀 전압/온도를 LTC6804 에서 직접 취득하며,
 *          BAT IC 통신 진단(PEC 에러 카운터)도 이때부터 동작한다.
 *--------------------------------------------------------------*/
/*--------------------------------------------------------------
 * 260910 : LTC6804 실기 통신 불능 — DebugBoardMode 가 1(모사장치 CAN)로
 *          남아 있어 SlaveVoltagHandler() 가 컴파일에서 제외되고
 *          CAN 메일박스도 모사장치 ID(0x401~) 로 잡혀 있었다.
 *          값(1)과 우측 주석("실기 isoSPI")이 불일치 상태였음.
 *--------------------------------------------------------------*/
//#define     DebugBoardMode                     1    // TODOS 260726_Note1, 0.14 디버깅보드 시험모드 (0:실기 isoSPI, 1이상:모사장치 CAN, 양산빌드 반드시 0)
//#define     DebugBoardMode                     1    // TODO : [검증] 260831_Note1, 0.18 (값 오기 — 주석은 isoSPI 인데 값이 1)
#define     DebugBoardMode                       0    // TODO : [검증] 260910_Note1, 0.21 실기 isoSPI 모드 복귀 (LTC6804 직접 취득)

#if (DebugBoardMode != 0) && (Product_Type == 1)
#error "DebugBoardMode must be 0 when Product_Type=1 (production build)"
#endif

#define     Bat80VSysVoltMax                 924 //4.2*22
#define     Bat80VSysVoltMin                 616  //2.8*22


#define     C_Bat80VCellShutdownFault            2.8
#define     C_Bat80VBalacneLimtVoltage           3.0
#define     C_Bat80VBalanceDivVoltage            0.01 //10mV
#define     C_Bat80VBalanceCurrent               5.0

#define     C_CTDirection               1.0
#define     CurrentDirection            1.0
#define     Cell_Capacity               52

#define     Sys80VCellVoltCount         22
#define     Sys80VCellTempCount         22
#define     SysNoramlState              0
#define     SysAlarmlState              1
#define     SysAlertState               2
#define     SysFaultState               3

#define     SysMondeInitial             0
#define     SysMondeReady               1
#define     SysMondeBalancing           4

#define     NoModePreRelayOnCntVaule    40 //2sec
#define     NoModePRelayOnCntVaule      80 //2sec
#define     NoModePreRelayOffCntVaule   140//3sec

#define     OffModePRelayOnCntVaule     40 //2sec
#define     OffModeNRelayOnCntVaule     80 //2sec
#define     OffModeProRelayOnCntVaule   140//3sec

#define     C_BalanceDivVoltage         0.03



//Fault Set Vaule
#define     C_Bat80VFaultDelayCount                        8000
#define     C_ISOSPICount                                  210    // ISOSPI 통신오류 : 50(초기)->210(V0.16), 판정 로직 미구현
#define     C_CANCount                                     50
#define     C_RleyCount                                    1      // 릴레이 이상 : 임시값, 설정 미확정
#define     C_PackCTOV_Fault        505.0   // 과전류 차단 : 506.0->500.0(초기)->505.0(V0.15)
#define     C_PackOCTimer_Fault     480.0   // 최대전류 시간 전류임계 : 500.0->480.0(초기)
#define     C_PackOCTimerCount      10000   // 최대전류 시간 유지 : 1000(1s,초기)->10000(10s,V0.16)
#define     C_PackOCEventCount      5       // 최대전류 반복 횟수/분 : 4->5(V0.16 신규)
#define     C_PackOCEventWindow     60000   // 최대전류 반복 집계창 1분 : 신규(V0.16)
#define     C_PackSOCOV_Fault       100.0   // 과충전 차단 : 101.0(초기)->100.0(V0.15)
#define     C_PackSOCUN_Fault       0.0     // 저충전 차단 : -0.1(초기)->0.0(V0.15)
#define     C_PackVoltOV_Fault      91.3    // 팩 과전압 차단 : 102.4->91.3(초기), R9 92.4 미반영하고 91.3 유지(V0.19)
#define     C_PackVoltUN_Fault      58.3    // 팩 저전압 차단 : 67.2->61.6(초기)->62.7(V0.15)->58.3(V0.16, 셀2.65x22S)
#define     C_PackTempOV_Fault      52.0    // 팩 고온 차단 : 60.0->52.0(초기)
#define     C_PackTempUN_Fault      -27.0   // 팩 저온 차단 : -30.0->-35.0(초기)->-25.0(V0.16)->-27.0(V0.19)
#define     C_CellVoltOV_Fault      4.20    // 셀 과전압 차단 : 4.27->4.15(초기)->4.20(V0.15)
#define     C_CellVoltUN_Fault      2.65    // 셀 저전압 차단 : 2.75->2.8(초기)->2.85(V0.15)->2.65(V0.16)
#define     C_CellVoltDIV_Fault     0.40    // 셀 전압편차 차단 : 0.5(초기)->0.35(V0.15)->0.40(V0.19)
#define     C_CellTempOV_Fault      60.0    // 셀 고온 차단 : 초기값
#define     C_CellTempUN_Fault      -30.0   // 셀 저온 차단 : -25.0->-30.0(초기)
#define     C_CellTempDIV_Fault     20.0    // 셀 온도편차 차단 : 15.0->10.0(초기)->20.0(V0.15, 경고10도와 분리)
#define     C_IOSresistanceFault    45000   // 절연저항 이상 : 임시값, 설정 미확정

//Fault Delay Time (유지시간 카운트, 루프1ms 기준). 설정표는 전 항목 즉시(0)
#define     C_PackCTOV_FaultDelay       0      // 과전류 : 1(초기)->0(V0.16)
#define     C_PackSOCOV_FaultDelay      0      // 과충전
#define     C_PackSOCUN_FaultDelay      0      // 저충전
#define     C_PackVoltOV_FaultDelay     0      // 팩 과전압 : 2000(초기)->0(V0.16)
#define     C_PackVoltUN_FaultDelay     0      // 팩 저전압 : 2000(초기)->0(V0.16)
#define     C_PackTempOV_FaultDelay     0      // 팩 고온
#define     C_PackTempUN_FaultDelay     0      // 팩 저온
#define     C_CellVoltOV_FaultDelay     0      // 셀 과전압 : 2000(초기)->0(V0.16)
#define     C_CellVoltUN_FaultDelay     0      // 셀 저전압 : 2000(초기)->0(V0.16)
#define     C_CellVoltDIV_FaultDelay    0      // 셀 전압편차 : 5000(초기)->0(V0.16)
#define     C_CellTempOV_FaultDelay     0      // 셀 고온
#define     C_CellTempUN_FaultDelay     0      // 셀 저온
#define     C_CellTempDIV_FaultDelay    0      // 셀 온도편차 : 5000(초기)->0(V0.16)
#define     C_PackUnbalPwr_FaultDelay   15000  // 전류한계 초과 차단 : 10000->15000(V0.16 신규, 15초)

//Warning(Alarm) Set Vaule   _Warn=Trigger, _WarnRst=Release(히스테리시스)
#define     C_PackCTOV_Warn                                450.0  // 과전류 경고 : 초기값
#define     C_PackCTOV_WarnRst                             405.0  // 과전류 해제 : 초기값
#define     C_PackSOCOV_Warn                               95.0   // 과충전 경고 : 100.0(초기)->95.0(V0.15)
#define     C_PackSOCOV_WarnRst                            92.15  // 과충전 해제 : 97.0(초기)->92.2(V0.15)->92.15(V0.16)
#define     C_PackSOCUN_Warn                               5.0    // 저충전 경고 : 초기값
#define     C_PackSOCUN_WarnRst                            5.25   // 저충전 해제 : 초기값
#define     C_PackVoltOV_Warn                              90.86  // 팩 과전압 경고 : 90.2(초기)->90.9(V0.15)->90.86(V0.16)
#define     C_PackVoltOV_WarnRst                           88.1342 // 팩 과전압 해제 : 87.494(초기)->88.1(V0.15)->88.1342(V0.16)
#define     C_PackVoltUN_Warn                              65.45  // 팩 저전압 경고 : 66.0(초기)->65.45(V0.19, 셀2.975x22S)
#define     C_PackVoltUN_WarnRst                           68.7   // 팩 저전압 해제 : 69.3(초기)->68.7(V0.19, 히스5%)
#define     C_PackTempOV_Warn                              47.0   // 팩 고온 경고 : 초기값
#define     C_PackTempOV_WarnRst                           44.65  // 팩 고온 해제 : 44.65(초기)->44.7(V0.15)->44.65(V0.16)
#define     C_PackTempUN_Warn                              -20.0  // 팩 저온 경고 : -25.0(초기)->-20.0(V0.15)->-25.0(V0.18)->-20.0(V0.19)
#define     C_PackTempUN_WarnRst                           -19.0  // 팩 저온 해제 : 0.0(초기)->-19.0(V0.15)->-23.8(V0.18)->-19.0(V0.19)
#define     C_CellVoltOV_Warn                              4.15   // 셀 과전압 경고 : 4.10(초기)->4.15(V0.15)
#define     C_CellVoltOV_WarnRst                           4.12925 // 셀 과전압 해제 : 4.0795(초기)->4.129(V0.15)->4.12925(V0.16)
#define     C_CellVoltUN_Warn                              3.00   // 셀 저전압 경고 : 초기값
#define     C_CellVoltUN_WarnRst                           3.015  // 셀 저전압 해제 : 초기값
#define     C_CellVoltDIV_Warn                             0.2    // 셀 전압편차 경고 : 초기값(200mV)
#define     C_CellVoltDIV_WarnRst                          0.0666667 // 셀 전압편차 해제 : 0.02(초기)->0.1(V0.12)->0.067(V0.15)->0.0666667(V0.16)
#define     C_CellTempOV_Warn                              55.0   // 셀 고온 경고 : 초기값
#define     C_CellTempOV_WarnRst                           52.25  // 셀 고온 해제 : 52.25(초기)->52.3(V0.15)->52.25(V0.16)
#define     C_CellTempUN_Warn                              -25.0  // 셀 저온 경고 : -20.0(초기)->-25.0(V0.15)->-20.0(V0.18)->-25.0(V0.19)
#define     C_CellTempUN_WarnRst                           -23.5  // 셀 저온 해제 : 0.0(초기)->-23.8(V0.15)->-23.75(V0.16)->-23.5(V0.19, 표준율6%)
#define     C_CellTempDIV_Warn                             10.0   // 셀 온도편차 경고 : 초기값
#define     C_CellTempDIV_WarnRst                          5.0    // 셀 온도편차 해제 : 초기값

//Warning(Alarm) Hold Time (유지시간 카운트, 루프1ms 기준). 항목별 개별 변경 가능
#define     C_PackCTOV_WarnDelay        100    //idx0 과전류
#define     C_PackSOCOV_WarnDelay       100    //idx1 과충전
#define     C_PackSOCUN_WarnDelay       100    //idx2 저충전
#define     C_PackVoltOV_WarnDelay      100    //idx3 팩과전압
#define     C_PackVoltUN_WarnDelay      100    //idx4 팩저전압
#define     C_PackTempOV_WarnDelay      100    //idx5 팩과온
#define     C_PackTempUN_WarnDelay      100    //idx6 팩저온
#define     C_PackUnbalPwr_WarnDelay    10000  //idx7 전류한계 초과 : 100(초기)->1000->10000(V0.16, 10초)
#define     C_CellVoltOV_WarnDelay      100    //idx8 셀과전압
#define     C_CellVoltUN_WarnDelay      100    //idx9 셀저전압
#define     C_CellVoltDIV_WarnDelay     100    //idx10 셀전압편차
#define     C_CellTempOV_WarnDelay      100    //idx11 셀과온
#define     C_CellTempUN_WarnDelay      100    //idx12 셀저온
#define     C_CellTempDIV_WarnDelay     100    //idx13 셀온도편차



#define SectoHour                   0.00027778 // 1(h)/3600(sec)
#define Func_Hz                     20 // 1(h)/3600(sec)
#define SocCumulativeTime           0.00027778 //1/3600
#define SocCurrentSampleTime        0.05


#endif  // end of PARAMETER.H definition



//===========================================================================
// No more.
//===========================================================================
