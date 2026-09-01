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


#define     Product_SysCellVauleS              22
#define     Product_SysCellVauleP              1
//#define     Product_Voltage                    768  // 3.664*22
//#define     Product_Capacity                   450  //
#define     Product_Voltage                    809  // TODO : [튜닝] 260809_Note1, 0.16 R11 Normal_Volt 80.96V(22S×3.68≈809)
#define     Product_Capacity                   564  // TODO : [튜닝] 260809_Note1, 0.16 R11 Capacity 56.4Ah(564)
#define     Product_Type                       0    // TODOS 26.07.02 TEST 버전0, 양상버전 1
//#define     Product_Version                    18   // 이전값(원복 전)
//#define     Product_Version                    16   // TODO : [변경] 260901_Note1, 0.16 Product_Version 18->16 (VER 0.16)
//#define     Product_Version                    17   // TODO : [변경] 260901_Note1, 0.17 Product_Version 16->17 (VER 0.17, R12 반영)
#define     Product_Version                      18   // TODO : [변경] 260901_Note1, 0.18 Product_Version 17->18 (BATIC SPI 설정 기입 추가)
/*--------------------------------------------------------------
 * 260831 : 디버깅보드 시험모드 해제 — 모사장치 CAN 대신 실기 isoSPI 사용.
 *          셀 전압/온도를 LTC6804 에서 직접 취득하며,
 *          BAT IC 통신 진단(PEC 에러 카운터)도 이때부터 동작한다.
 *--------------------------------------------------------------*/
//#define     DebugBoardMode                     1    // TODOS 260726_Note1, 0.14 디버깅보드 시험모드 (0:실기 isoSPI, 1이상:모사장치 CAN, 양산빌드 반드시 0)
#define     DebugBoardMode                       0    // TODO : [검증] 260831_Note1, 0.18 실기 isoSPI 모드

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



// Alarm Set Vaule  //
/*
// ※ 아래 Alarm #define은 현재 비활성(주석). 실제 Warning 임계값은 Cal80VSysAlarmtCheck()의 하드코딩값.
//    엑셀 설정값(WrnXX)과 일치하도록 갱신해 둠(향후 재사용 대비).
#define     C_Bat80VOVPackCurrentAlarm                     450.0  //26.05.30기준 WrnOC 450A (이전504)
#define     C_Bat80VOVPkACKSOCAlarm                        100.0  //26.05.30기준 WrnSocH 100%
#define     C_Bat80VUDPkACKSOCAlarm                        5.0    //26.05.30기준 WrnSocL 5%
#define     C_Bat80VOVPackVoltageAlarm                     90.2   //26.05.30기준 WrnOv 90.2V (이전108.8)
#define     C_Bat80VUDPackVoltageAlarm                     66.0   //26.05.30기준 WrnUv 66V (이전72.0)
#define     C_Bat80VOVPackTemperatureAlarm                 47.0   //26.05.30기준 WrnOt 47도 (이전55.0)
#define     C_Bat80VUNPackTemperatureAlarm                -25.0   //26.05.30기준 WrnUt -25도 (이전-15.0)
#define     C_Bat80VOVCellVoltageAlarm                     4.10   //26.05.30기준 WrnCellOv 4.1V (이전4.20)
#define     C_Bat80VUDCellVoltageAlarm                     3.00   //26.05.30기준 WrnCellUv 3.0V
#define     C_Bat80VDIVCellVoltageAlarm                    0.2    //26.05.30기준 WrnCellUnbalV 200mV
#define     C_Bat80VOVCellTemperatureAlarm                 55.0   //26.05.30기준 WrnCellOt 55도
#define     C_Bat80VUDCellTemperatureAlarm                -20.0   //26.05.30기준 WrnCellUt -20도 (이전-15.0)
#define     C_Bat80VDIVCellTemperatureAlarm                10.0   //26.05.30기준 WrnCellUnbalTmp 10도
*/

//Fault Set Vaule
#define     C_Bat80VFaultDelayCount                        8000
/*--------------------------------------------------------------
 * 260827 : 보호설정표 260827 R4 — ISOSPI 통신이상 카운트 50 → 210.
 *          설정표 기준(210 CONT)으로 정렬. 판정 로직은 아직 없음.
 *--------------------------------------------------------------*/
//#define     C_ISOSPICount                                  50
#define     C_ISOSPICount                                  210    // TODO : [튜닝] 260827_Note1, 0.16 BPA_FltISOSPI_Err 210 CONT (R4 반영, 판정 로직 미구현)
#define     C_CANCount                                     50
#define     C_RleyCount                                    1
// TODO(PrtotectSet2605030 반영, 검증 후 정리): Protect 임계값을 엑셀 설정값과 일치. 주석=이전값.
//#define     C_PackCTOV_Fault        500.0   //26.05.30기준 500A (이전506.0)
#define     C_PackCTOV_Fault        505.0   // TODO : [튜닝] 260809_Note1, 0.15 FltOc 505A(표반영)
#define     C_PackOCTimer_Fault     480.0   //26.05.30기준 480A (이전500.0) OC타이머 전류임계
/*--------------------------------------------------------------
 * 260827 : 보호설정표 R6 — 최대 전류 시간(Bsa_FltOcTimer) 1 → 10 Sec.
 *          480A 이상이 10초 이상 지속될 때 FAULT. 루프 1ms 기준 카운트.
 *--------------------------------------------------------------*/
//#define     C_PackOCTimerCount      1000    //26.05.30湲곗� 1sec (480A 1sec �쑀吏�) �삳（�봽1ms 湲곗�
#define     C_PackOCTimerCount      10000   // TODO : [튜닝] 260827_Note1, 0.16 최대 전류 시간 10sec 유지 (480A x 10sec, R6 반영)
/*--------------------------------------------------------------
 * 260827 : 최대 전류 반복(Bsa_FltOcTime_min) 신규 — 480A 진입 순간이
 *          1분 창 안에서 4회 발생하면 FAULT. 루프 1ms 기준.
 *--------------------------------------------------------------*/
/*--------------------------------------------------------------
 * 260827 : 보호설정표 R6 — 최대 전류 반복 4 → 5 Count/min.
 *--------------------------------------------------------------*/
//#define     C_PackOCEventCount      4       // TODO : [튜닝] 260827_Note1, 0.16 FltOcTime_min 1분당 허용 4회 (R5 반영)
#define     C_PackOCEventCount      5       // TODO : [튜닝] 260827_Note1, 0.16 최대 전류 반복 1분당 허용 5회 (R6 반영)
#define     C_PackOCEventWindow     60000   // TODO : [튜닝] 260827_Note1, 0.16 최대 전류 반복 집계 창 1분(60000ms)
//#define     C_PackSOCOV_Fault       101.0   //26.05.30기준 101%
//#define     C_PackSOCUN_Fault       -0.1    //26.05.30기준 -0.1%
#define     C_PackSOCOV_Fault       100.0   // TODO : [튜닝] 260809_Note1, 0.15 FltctSocH 100%(표반영)
#define     C_PackSOCUN_Fault       0.0     // TODO : [튜닝] 260809_Note1, 0.15 FltctSocL 0%(표반영)
#define     C_PackVoltOV_Fault      91.3    //26.05.30기준 91.3V (이전102.4)
//#define     C_PackVoltUN_Fault      61.6    //26.05.30기준 61.6V (이전67.2)
/*--------------------------------------------------------------
 * 260827 : 보호설정표 260827 R4 — 팩 저전압 FAULT 62.7 → 58.3 V.
 *          셀 저전압 폴트 2.65 V x 22S 연동값.
 *--------------------------------------------------------------*/
//#define     C_PackVoltUN_Fault      62.7    // TODO : [�뒠�떇] 260809_Note1, 0.15 FltUv 62.7V(�몴諛섏쁺)
#define     C_PackVoltUN_Fault      58.3    // TODO : [튜닝] 260827_Note1, 0.16 FltUv 58.3V (2.65V x 22S, R4 반영)
#define     C_PackTempOV_Fault      52.0    //26.05.30기준 52도 (이전60.0)
/*--------------------------------------------------------------
 * 260830 : 팩 저온 FAULT -35.0 → -25.0 도.
 *          셀 저온 폴트 -30.0 보다 높게 두어 팩이 먼저 검출되도록 함
 *          (경고 팩-20/셀-25 · 과온 팩52/셀60 과 방향 일치).
 *--------------------------------------------------------------*/
//#define     C_PackTempUN_Fault      -35.0   //26.05.30기준 -35도 (이전-30.0)
#define     C_PackTempUN_Fault      -25.0   // TODO : [튜닝] 260830_Note1, 0.16 FltUt -25.0 (셀 저온 폴트 -30.0 보다 먼저 검출)
//#define     C_CellVoltOV_Fault      4.15    //26.05.30기준 4.15V (이전4.27)
//#define     C_CellVoltUN_Fault      2.8     //26.05.30기준 2.8V (이전2.75)
//#define     C_CellVoltDIV_Fault     0.5     //26.05.30기준 500mV
#define     C_CellVoltOV_Fault      4.20    // TODO : [튜닝] 260809_Note1, 0.15 FltCellOv 4.20V(표반영)
/*--------------------------------------------------------------
 * 260827 : 보호설정표 260827 R4 — 셀 저전압 FAULT 2.85 → 2.65 V.
 *          셀 기준 보호 최대치 변경. 팩 FltUv 58.3 V 와 연동.
 *--------------------------------------------------------------*/
//#define     C_CellVoltUN_Fault      2.85    // TODO : [�뒠�떇] 260809_Note1, 0.15 FltCellUv 2.85V(�몴諛섏쁺)
#define     C_CellVoltUN_Fault      2.65    // TODO : [튜닝] 260827_Note1, 0.16 FltCellUv 2.65V (R4 반영)
#define     C_CellVoltDIV_Fault     0.35    // TODO : [튜닝] 260809_Note1, 0.15 FltCellUnbalVlt 350mV(표반영)
#define     C_CellTempOV_Fault      60.0    //26.05.30기준 60도
#define     C_CellTempUN_Fault      -30.0   //26.05.30기준 -30도 (이전-25.0)
/*--------------------------------------------------------------
 * 260810 : �� �삩�룄�렪李� �뤃�듃 �엫怨� �긽�뼢 �� 寃쎄퀬(10�룄)�� �뤃�듃(10�룄)媛�
 *          �룞�씪�빐 �렪李� 10.1�룄�뿉�꽌 寃쎄퀬 利됱떆 + 5珥� �썑 李⑤떒 諛쒖깮.
 *          �젅���삩�룄 蹂댄샇(��60�룄/�뙥52�룄)�뒗 �쑀吏��릺誘�濡� 留덉쭊 �솗蹂�.
 *--------------------------------------------------------------*/
//#define     C_CellTempDIV_Fault     10.0    //26.05.30湲곗� 10�룄 (�씠�쟾15.0)
/*--------------------------------------------------------------
 * 260827 : 보호설정표 260827 R4 는 10도 이나 펌웨어 20도 유지 확정.
 *          표대로 10도면 경고(10도)와 임계가 같아지고 폴트 지연이 0 이라
 *          경고 set(100ms) 전에 즉시 차단됨 → 단계 분리 위해 20도 존치.
 *          ※ 설정표를 20도로 갱신 요청할 것.
 *--------------------------------------------------------------*/
//#define     C_CellTempDIV_Fault     20.0    // TODO : [�뒠�떇] 260810_Note1, 0.15 ���삩�룄�렪李� �뤃�듃 20�룄 (寃쎄퀬 10�룄�� 遺꾨━)
#define     C_CellTempDIV_Fault     20.0    // TODO : [튜닝] 260827_Note1, 0.16 FltCellUnbalTmp 20도 유지 확정 (R4 표 10도 미반영, 경고 10도와 분리)
#define     C_IOSresistanceFault    45000
//Fault Delay Time

/*--------------------------------------------------------------
 * 260827 : 보호설정표 260830 R3 — Fault 유지시간 전 항목 「즉시 = 0」 통일.
 *          설정표는 Fault 23항목 모두 지연 0 이나 펌웨어에 1~5000ms 잔존.
 *          향후 재조정 대비로 기존 지연값은 주석 보존.
 *          ※ 값·미구현 로직·0x603 상위비트 보고는 본 변경 범위 아님(별도 반영).
 *--------------------------------------------------------------*/
//#define     C_PackCTOV_FaultDelay       1
#define     C_PackCTOV_FaultDelay       0      // TODO : [튜닝] 260827_Note1, 0.16 FltOc 유지시간 1→0ms (즉시)
#define     C_PackSOCOV_FaultDelay      0
#define     C_PackSOCUN_FaultDelay      0
//#define     C_PackVoltOV_FaultDelay     2000
//#define     C_PackVoltUN_FaultDelay     2000
#define     C_PackVoltOV_FaultDelay     0      // TODO : [튜닝] 260827_Note1, 0.16 FltOv 유지시간 2000→0ms (즉시)
#define     C_PackVoltUN_FaultDelay     0      // TODO : [튜닝] 260827_Note1, 0.16 FltUv 유지시간 2000→0ms (즉시)
#define     C_PackTempOV_FaultDelay     0
#define     C_PackTempUN_FaultDelay     0
//#define     C_CellVoltOV_FaultDelay     2000
//#define     C_CellVoltUN_FaultDelay     2000
//#define     C_CellVoltDIV_FaultDelay    5000
#define     C_CellVoltOV_FaultDelay     0      // TODO : [튜닝] 260827_Note1, 0.16 FltCellOv 유지시간 2000→0ms (즉시)
#define     C_CellVoltUN_FaultDelay     0      // TODO : [튜닝] 260827_Note1, 0.16 FltCellUv 유지시간 2000→0ms (즉시)
#define     C_CellVoltDIV_FaultDelay    0      // TODO : [튜닝] 260827_Note1, 0.16 FltCellUnbalVlt 유지시간 5000→0ms (즉시)
#define     C_CellTempOV_FaultDelay     0
#define     C_CellTempUN_FaultDelay     0
//#define     C_CellTempDIV_FaultDelay    5000
#define     C_CellTempDIV_FaultDelay    0      // TODO : [튜닝] 260827_Note1, 0.16 FltCellUnbalTmp 유지시간 5000→0ms (즉시, 임계 20도 유지)
/*--------------------------------------------------------------
 * 260827 : Bsa_FltUnbalPwr 신규 — 연속 전류 한계 초과 15초 이상 지속 시 FAULT.
 *          경고는 10초(C_PackUnbalPwr_WarnDelay). 루프 1ms 기준.
 *--------------------------------------------------------------*/
/*--------------------------------------------------------------
 * 260827 : 보호설정표 R6 — Bsa_FltUnbalPwr 유지시간 15 Sec 확정.
 *--------------------------------------------------------------*/
//#define     C_PackUnbalPwr_FaultDelay   10000  // TODO : [튜닝] 260827_Note1, 0.16 FltUnbalPwr 10sec 유지 (연속 전류 한계 초과, 단위 A)
#define     C_PackUnbalPwr_FaultDelay   15000  // TODO : [튜닝] 260827_Note1, 0.16 FltUnbalPwr 15sec 유지 (전류제한 초과, 단위 A, R6 반영)

//Warning(Alarm) Set Vaule
/*--------------------------------------------------------------
 * 260827 : 보호설정표 R6 정합 — 경고 해제값 8건을 설정표 정확값으로 복원.
 *          260809 에 소수 1~3자리로 반올림해 넣었던 값들이며,
 *          R6 5-3 절「반올림 차이」로 남아 있던 항목이다.
 *          float32 유효자리 안이라 표기 그대로 반영 가능.
 *--------------------------------------------------------------*/
// 26.05.30기준 PrtotectSet2605030 의 Wrn 항목. _Warn=Trigger, _WarnRst=Release(히스테리시스)
#define     C_PackCTOV_Warn                                450.0  //26.05.30기준 WrnOC 450A
#define     C_PackCTOV_WarnRst                             405.0  //26.05.30기준 release 405A
/*--------------------------------------------------------------
 * 260809 : WrnSocH 경고 임계 조정 — 100→95%, 해제 97→92.15%(3% 히스)
 *--------------------------------------------------------------*/
//#define     C_PackSOCOV_Warn                               100.0  //26.05.30기준 WrnSocH 100%
//#define     C_PackSOCOV_WarnRst                            97.0   //26.05.30기준 release 97%
#define     C_PackSOCOV_Warn                               95.0   // TODO : [튜닝] 260809_Note1, 0.15 WrnSocH 95% 이상
//#define     C_PackSOCOV_WarnRst                            92.2   // TODO : [�뒠�떇] 260809_Note1, 0.15 �빐�젣 92.2%(�냼�닔1, �몴諛섏쁺)
#define     C_PackSOCOV_WarnRst                            92.15  // TODO : [튜닝] 260827_Note1, 0.16 WrnSocH 해제 92.15% (R6 정확값)
#define     C_PackSOCUN_Warn                               5.0    //26.05.30기준 WrnSocL 5%
#define     C_PackSOCUN_WarnRst                            5.25   //26.05.30기준 release 5.25%
//#define     C_PackVoltOV_Warn                              90.2   //26.05.30기준 WrnOv 90.2V
//#define     C_PackVoltOV_WarnRst                           87.494 //26.05.30기준 release 87.494V
//#define     C_PackVoltOV_Warn                              90.9   // TODO : [�뒠�떇] 260809_Note1, 0.15 WrnOv 90.9V(�냼�닔1, �몴諛섏쁺)
#define     C_PackVoltOV_Warn                              90.86  // TODO : [튜닝] 260827_Note1, 0.16 WrnOv 발생 90.86V (R6 정확값)
//#define     C_PackVoltOV_WarnRst                           88.1   // TODO : [�뒠�떇] 260809_Note1, 0.15 �빐�젣 88.1V(�냼�닔1)
#define     C_PackVoltOV_WarnRst                           88.1342 // TODO : [튜닝] 260827_Note1, 0.16 WrnOv 해제 88.1342V (R6 정확값)
#define     C_PackVoltUN_Warn                              66.0   //26.05.30기준 WrnUv 66V
#define     C_PackVoltUN_WarnRst                           69.3   //26.05.30기준 release 69.3V
#define     C_PackTempOV_Warn                              47.0   //26.05.30기준 WrnOt 47도
//#define     C_PackTempOV_WarnRst                           44.65  //26.05.30기준 release 44.65도
//#define     C_PackTempOV_WarnRst                           44.7   // TODO : [�뒠�떇] 260809_Note1, 0.15 �빐�젣 44.7�룄(�냼�닔1)
#define     C_PackTempOV_WarnRst                           44.65  // TODO : [튜닝] 260827_Note1, 0.16 WrnOt 해제 44.65도 (R6 정확값)
//#define     C_PackTempUN_Warn                             -25.0   //26.05.30湲곗� WrnUt -25�룄
#define     C_PackTempUN_Warn                        -25.0  // TODO : [원복] 260901_Note1, 0.18 저온경고 WrnUt -25도 (스왑 되돌림)
//#define     C_PackTempUN_WarnRst                           0.0    //26.05.30기준 release 0도
//#define     C_PackTempUN_WarnRst                          -23.8   //260809 WrnUt �빐�젣(�뒪�솑 �쟾)
#define     C_PackTempUN_WarnRst                     -23.8  // TODO : [원복] 260901_Note1, 0.18 저온경고 WrnUt 해제 -23.8도 (스왑 되돌림)
//#define     C_CellVoltOV_Warn                              4.10   //26.05.30기준 WrnCellOv 4.1V
//#define     C_CellVoltOV_WarnRst                           4.0795 //26.05.30기준 release 4.0795V
#define     C_CellVoltOV_Warn                              4.15   // TODO : [튜닝] 260809_Note1, 0.15 WrnCellOv 4.15V(표반영)
//#define     C_CellVoltOV_WarnRst                           4.129  // TODO : [�뒠�떇] 260809_Note1, 0.15 �빐�젣 4.129V(�냼�닔3)
#define     C_CellVoltOV_WarnRst                           4.12925 // TODO : [튜닝] 260827_Note1, 0.16 WrnCellOv 해제 4.12925V (R6 정확값)
#define     C_CellVoltUN_Warn                              3.00   //26.05.30기준 WrnCellUv 3.0V
#define     C_CellVoltUN_WarnRst                           3.015  //26.05.30기준 release 3.015V
#define     C_CellVoltDIV_Warn                             0.2    //26.05.30기준 WrnCellUnbalV 200mV
/*--------------------------------------------------------------
 * 260716 : 셀 편차 알람 해제 임계값 상향 (20mV로는 정상 편차에서
 *          해제가 안 돼 알람이 latch됨 → 히스테리시스 밴드 축소)
 *--------------------------------------------------------------*/
//#define     C_CellVoltDIV_WarnRst                          0.02   //26.05.30기준 release 20mV
//#define     C_CellVoltDIV_WarnRst                          0.1    // TODO : [튜닝] 260716_Note1, 0.12 셀전압편차 알람 해제 100mV (기존 20mV→100mV)
//#define     C_CellVoltDIV_WarnRst                          0.067  // TODO : [�뒠�떇] 260809_Note1, 0.15 �빐�젣 67mV(�몴諛섏쁺)
#define     C_CellVoltDIV_WarnRst                          0.0666667 // TODO : [튜닝] 260827_Note1, 0.16 WrnCellUnbalV 해제 66.6667mV (R6 정확값)
#define     C_CellTempOV_Warn                              55.0   //26.05.30기준 WrnCellOt 55도
//#define     C_CellTempOV_WarnRst                           52.25  //26.05.30기준 release 52.25도
//#define     C_CellTempOV_WarnRst                           52.3   // TODO : [�뒠�떇] 260809_Note1, 0.15 �빐�젣 52.3�룄(�냼�닔1)
#define     C_CellTempOV_WarnRst                           52.25  // TODO : [튜닝] 260827_Note1, 0.16 WrnCellOt 해제 52.25도 (R6 정확값)
//#define     C_CellTempUN_Warn                             -20.0   //26.05.30湲곗� WrnCellUt -20�룄
#define     C_CellTempUN_Warn                        -20.0  // TODO : [원복] 260901_Note1, 0.18 저온경고 WrnCellUt -20도 (스왑 되돌림)
//#define     C_CellTempUN_WarnRst                           0.0    //26.05.30기준 release 0도
//#define     C_CellTempUN_WarnRst                          -19.0   //260809 WrnCellUt �빐�젣(�뒪�솑 �쟾)
//#define     C_CellTempUN_WarnRst                          -23.8   // TODO : [�뒠�떇] 260809_Note1, 0.15 WrnCellUt �빐�젣 -23.8�룄 (�뒪�솑, �엳�뒪�뀒由ъ떆�뒪 �쑀吏�)
#define     C_CellTempUN_WarnRst                          -23.75  // TODO : [튜닝] 260827_Note1, 0.16 WrnCellUt 해제 -23.75도 (R6 정확값)
#define     C_CellTempDIV_Warn                             10.0   //26.05.30기준 WrnCellUnbalTmp 10도
#define     C_CellTempDIV_WarnRst                          5.0    //26.05.30기준 release 5도

//Warning(Alarm) Hold Time (유지시간 카운트, 루프1ms 기준). 항목별 개별 변경 가능
#define     C_PackCTOV_WarnDelay        100    //idx0 과전류
#define     C_PackSOCOV_WarnDelay       100    //idx1 과충전
#define     C_PackSOCUN_WarnDelay       100    //idx2 저충전
#define     C_PackVoltOV_WarnDelay      100    //idx3 팩과전압
#define     C_PackVoltUN_WarnDelay      100    //idx4 팩저전압
#define     C_PackTempOV_WarnDelay      100    //idx5 팩과온
#define     C_PackTempUN_WarnDelay      100    //idx6 팩저온
/*--------------------------------------------------------------
 * 260827 : 불평형(피크초과) 파워 경고 유지시간 100 → 1000 ms.
 *          「1초 이상 초과 시 알람」 요구 반영.
 *--------------------------------------------------------------*/
//#define     C_PackUnbalPwr_WarnDelay    100    //idx7 遺덊룊�삎�뙆�썙(�쁽�옱 誘몄궗�슜)
/*--------------------------------------------------------------
 * 260827 : 보호설정표 R6 — Bsa_WrnUnbalPwr 유지시간 10 Sec 확정.
 *          설정표 Delay 100ms 는 표 양식상 값이며 유지시간 10초에 포함.
 *--------------------------------------------------------------*/
//#define     C_PackUnbalPwr_WarnDelay    1000   // TODO : [튜닝] 260827_Note1, 0.16 WrnUnbalPwr 1sec 유지 (연속 전류 한계 초과, 단위 A)
#define     C_PackUnbalPwr_WarnDelay    10000  // TODO : [튜닝] 260827_Note1, 0.16 WrnUnbalPwr 10sec 유지 (전류제한 초과, 단위 A, R6 반영)
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
