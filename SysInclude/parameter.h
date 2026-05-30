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

#define UL_BYTE(x)		    (x >> 16)
#define HI_BYTE(x)		    (x >> 8)
#define LO_BYTE(x)          (x & 0xff)
#define MAKE_WORD(msb,lsb)	((msb<<8) | (lsb))
#define WordLShift(md,ml)   (md<<ml)
#define WordRShift(md,ml)   (md>>ml)



// Define the ISR frequency (kHz)
#define ISR_FREQUENCY 	    1
#define SYSTEM_FREQUENCY    80
#define CPUCLK			       80000000L					// CPU Main Clock
//#define CPLDCLK			    100000000L					// CPLD Clock

#define CPU_CLOCK_SPEED     12.5L                   // for a 80MHz CPU clock speed
//#define ADC_usDELAY 	    5000L



#define	Uint16Max		    65535U


/*
#define TxA_RDY_flag        SciaRegs.SCICTL2.bit.TXRDY
#define TxB_RDY_flag	       ScibRegs.SCICTL2.bit.TXRDY
#define TxC_RDY_flag        ScicRegs.SCICTL2.bit.TXRDY
#define TxA_Empty_flag      SciaRegs.SCICTL2.bit.TXEMPTY
#define TxB_Empty_flag      ScibRegs.SCICTL2.bit.TXEMPTY
#define TxC_Empty_flag	    ScicRegs.SCICTL2.bit.TXEMPTY
#define AD_START   	   	    AdcRegs.ADCTRL2.bit.SOC_SEQ1
#define IS_AD_BUSY 	        AdcRegs.ADCST.bit.SEQ1_BSY
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
 TMS320F28335 CLK SET UP 
-------------------------------------------------------------------------------*/
#define	CPUCLK				    80000000L							// CPU Main Clock
/*-------------------------------------------------------------------------------
 TMS320F28335 CLK SET UP 
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
#define     Product_Voltage                    768  // 3.664*22
#define     Product_Capacity                   400  //
#define     Product_Type                       3    // 24.09.21
#define     Product_Version                    10   // 24.09.21

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
#define     C_ISOSPICount                                  50
#define     C_CANCount                                     50
#define     C_RleyCount                                    1
// TODO(PrtotectSet2605030 반영, 검증 후 정리): Protect 임계값을 엑셀 설정값과 일치. 주석=이전값.
#define     C_PackCTOV_Fault        500.0   //26.05.30기준 500A (이전506.0)
#define     C_PackOCTimer_Fault     480.0   //26.05.30기준 480A (이전500.0) OC타이머 전류임계
#define     C_PackOCTimerCount      1000    //26.05.30기준 1sec (480A 1sec 유지) ※루프1ms 기준
#define     C_PackSOCOV_Fault       101.0   //26.05.30기준 101%
#define     C_PackSOCUN_Fault       -0.1    //26.05.30기준 -0.1%
#define     C_PackVoltOV_Fault      91.3    //26.05.30기준 91.3V (이전102.4)
#define     C_PackVoltUN_Fault      61.6    //26.05.30기준 61.6V (이전67.2)
#define     C_PackTempOV_Fault      52.0    //26.05.30기준 52도 (이전60.0)
#define     C_PackTempUN_Fault      -35.0   //26.05.30기준 -35도 (이전-30.0)
#define     C_CellVoltOV_Fault      4.15    //26.05.30기준 4.15V (이전4.27)
#define     C_CellVoltUN_Fault      2.8     //26.05.30기준 2.8V (이전2.75)
#define     C_CellVoltDIV_Fault     0.5     //26.05.30기준 500mV
#define     C_CellTempOV_Fault      60.0    //26.05.30기준 60도
#define     C_CellTempUN_Fault      -30.0   //26.05.30기준 -30도 (이전-25.0)
#define     C_CellTempDIV_Fault     10.0    //26.05.30기준 10도 (이전15.0)
#define     C_IOSresistanceFault    45000
//Fault Delay Time

#define     C_PackCTOV_FaultDelay       1
#define     C_PackSOCOV_FaultDelay      0
#define     C_PackSOCUN_FaultDelay      0
#define     C_PackVoltOV_FaultDelay     2000
#define     C_PackVoltUN_FaultDelay     2000
#define     C_PackTempOV_FaultDelay     0
#define     C_PackTempUN_FaultDelay     0
#define     C_CellVoltOV_FaultDelay     2000
#define     C_CellVoltUN_FaultDelay     2000
#define     C_CellVoltDIV_FaultDelay    5000
#define     C_CellTempOV_FaultDelay     0
#define     C_CellTempUN_FaultDelay     0
#define     C_CellTempDIV_FaultDelay    5000

//Warning(Alarm) Set Vaule
// 26.05.30기준 PrtotectSet2605030 의 Wrn 항목. _Warn=Trigger, _WarnRst=Release(히스테리시스)
#define     C_PackCTOV_Warn                                450.0  //26.05.30기준 WrnOC 450A
#define     C_PackCTOV_WarnRst                             405.0  //26.05.30기준 release 405A
#define     C_PackSOCOV_Warn                               100.0  //26.05.30기준 WrnSocH 100%
#define     C_PackSOCOV_WarnRst                            97.0   //26.05.30기준 release 97%
#define     C_PackSOCUN_Warn                               5.0    //26.05.30기준 WrnSocL 5%
#define     C_PackSOCUN_WarnRst                            5.25   //26.05.30기준 release 5.25%
#define     C_PackVoltOV_Warn                              90.2   //26.05.30기준 WrnOv 90.2V
#define     C_PackVoltOV_WarnRst                           87.494 //26.05.30기준 release 87.494V
#define     C_PackVoltUN_Warn                              66.0   //26.05.30기준 WrnUv 66V
#define     C_PackVoltUN_WarnRst                           69.3   //26.05.30기준 release 69.3V
#define     C_PackTempOV_Warn                              47.0   //26.05.30기준 WrnOt 47도
#define     C_PackTempOV_WarnRst                           44.65  //26.05.30기준 release 44.65도
#define     C_PackTempUN_Warn                             -25.0   //26.05.30기준 WrnUt -25도
#define     C_PackTempUN_WarnRst                           0.0    //26.05.30기준 release 0도
#define     C_CellVoltOV_Warn                              4.10   //26.05.30기준 WrnCellOv 4.1V
#define     C_CellVoltOV_WarnRst                           4.0795 //26.05.30기준 release 4.0795V
#define     C_CellVoltUN_Warn                              3.00   //26.05.30기준 WrnCellUv 3.0V
#define     C_CellVoltUN_WarnRst                           3.015  //26.05.30기준 release 3.015V
#define     C_CellVoltDIV_Warn                             0.2    //26.05.30기준 WrnCellUnbalV 200mV
#define     C_CellVoltDIV_WarnRst                          0.02   //26.05.30기준 release 20mV
#define     C_CellTempOV_Warn                              55.0   //26.05.30기준 WrnCellOt 55도
#define     C_CellTempOV_WarnRst                           52.25  //26.05.30기준 release 52.25도
#define     C_CellTempUN_Warn                             -20.0   //26.05.30기준 WrnCellUt -20도
#define     C_CellTempUN_WarnRst                           0.0    //26.05.30기준 release 0도
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
#define     C_PackUnbalPwr_WarnDelay    100    //idx7 불평형파워(현재 미사용)
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
