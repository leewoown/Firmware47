
//###########################################################################
//
// FILE:   DSP28x_Project.h
//
// TITLE:  DSP28x Project Headerfile and Examples Include File
//
//###########################################################################
// $TI Release: $
// $Release Date: $
// $Copyright:
// Copyright (C) 2009-2024 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

#ifndef DSP28x_PROJECT_H
#define DSP28x_PROJECT_H

//
// Included Files
//
#include "F2806x_Cla_typedefs.h"// F2806x CLA Type definitions
#include "F2806x_Device.h"      // F2806x Headerfile Include File
#include "F2806x_Examples.h"   	// F2806x Examples Include File
#include "parameter.h"

//EDLAY 매크로 선언 --------------------------------------------------------------------//

// TI SDK 1.10의 소스 DSP2803x_usDelay.asm에서 제공하는 DELAY_US 함수를 사용

// TI SDK 1.10의 소스 DSP2803x_usDelay.asm에서 제공하는 DELAY_US 함수를 사용
#define delay_us(us)        DELAY_US(us)
// TI SDK 1.10의 소스 DSP2803x_usDelay.asm에서 제공하는 DELAY_US 함수를 사용
#define delay_ms(ms)        DELAY_US(ms*1000)
//#define  SPI_Read()       SPI_READ()

/*-------------------------------------------------------------------------------
Next, definitions used in main file.
-------------------------------------------------------------------------------*/
#define TRUE    1
#define FALSE   0
#define TRUE    1
#define ON      1
#define OFF     0
#define CAN_ID_11BIT   0   // 표준 ID (11-bit)
#define CAN_ID_29BIT   1    // 확장 ID (29-bit)


#define         Shift_RIGHT(val, bit)            ((val) >> (al))
#define         Shift_LEFT(val,  bit)            ((val) << (val))
#define         ComBine(Val_H, Val_L)            (((Val_H) << 8) | (Val_L))
#define         BetweenRange(val, Val_L, Val_H)  ((val) >= (Val_L) && (val) <= (Val_H))
#define         IS_OVER_AND_UNDER(A, MIN, MAX)   ((A) >= (MIN) && (A) <= (MAX))  // 이상 ~ 이하
#define         IS_ABOVE_AND_UNDER(A, MIN, MAX)  ((A) >  (MIN) && (A) <= (MAX))  // 초과 ~ 이하
#define         IS_OVER_AND_BELOW(A, MIN, MAX)   ((A) >= (MIN) && (A) <  (MAX))  // 이상 ~ 미만
#define         IS_ABOVE_AND_BELOW(A, MIN, MAX)  ((A) >  (MIN) && (A) <  (MAX))  // 초과 ~ 미만
#define         Hyst_On(Value, SetValue)   ((Value) > (SetValue))   // 켜짐 조건
#define         Hyst_Off(Value, RstValue)  ((Value) < (RstValue))   // 꺼짐 조건

#define BIT_MASK(bit)           (1 << (bit))
#define GetBit(val, bit)        (((val) & BIT_MASK(bit)) >> (bit))
#define SetBit(val, bit)        (val |= BIT_MASK(bit))
#define ClearBit(val, bit)      (val &= ~BIT_MASK(bit))
#define ToggleBit(val, bit)     (val ^= BIT_MASK(bit))
#define bit_is_set(val, bit)    (val & BIT_MASK(bit))
#define bit_is_clear(val, bit)  (~val & BIT_MASK(bit))

#define CAN_ALIGN_1BYTE(id)   ((Uint16)((id) & 0x07F0))  // 0xFF(X) : LSB4=0
#define CAN_ALIGN_2BYTE(id)   ((Uint16)((id) & 0x070F))  // 0xF(X)F : [7:4]=0

// ===== LAM 값(1=don't-care) : LAM_H에 그대로 넣으면 됨 =====
#define CAN_LAM_1BYTE         ((Uint16)0x003C)  // LSB4 don’t-care → 0x..0~0x..F
#define CAN_LAM_2BYTE         ((Uint16)0x03C0)  // [7:4] don’t-care → 0xF(X)F

struct Data_WORD
{
    unsigned int DataL;
    unsigned int DataH;
};
typedef enum
{
   System_STATE_INIT,
   System_STATE_STANDBY,
   System_STATE_READY,
   System_STATE_RUNING,
   System_STATE_PROTECTER,
   System_STATE_DATALOG,
   System_STATE_ProtectHistory,
   System_STATE_MANUALMode,
   System_STATE_CLEAR
} SysState;
struct ParentDeviceCMD_BIT
{
    // bits   description
   unsigned int     POWEREN                 :1;   // 0
   unsigned int     SW01                    :1;   // 2
   unsigned int     SW02                    :1;   // 3
   unsigned int     SW03                    :1;   // 4
   unsigned int     SW04                    :1;   // 5
   unsigned int     SW05                    :1;   // 6
   unsigned int     SW06                    :1;   // 6
   unsigned int     SW07                    :1;   // 7
   unsigned int     SW08                    :1;   // 8
   unsigned int     SW09                    :1;   // 9
   unsigned int     SW10                    :1;   // 11
   unsigned int     SW11                    :1;   // 12
   unsigned int     SW12                    :1;   // 13
   unsigned int     SW13                    :1;   // 14
   unsigned int     SW14                    :1;   // 15
   unsigned int     SW15                    :1;   // 16
};
union ParentDeviceCMD_REG
{
   unsigned int     all;
   struct ParentDeviceCMD_BIT bit;
};

struct DigitalInPut_BIT
{       // bits   description
   unsigned int     IDSW                    :2;   // 0
   unsigned int     PAUX                    :1;   // 2
   unsigned int     NAUX                    :1;   // 3
   unsigned int     CHAAUX                  :1;   // 4
   unsigned int     CANRX0                  :1;   // 5
   unsigned int     CANRX1                  :1;   // 6
   unsigned int     SW07                    :1;   // 7
   unsigned int     SW08                    :1;   // 8
   unsigned int     SW09                    :1;   // 9
   unsigned int     SW10                    :1;   // 11
   unsigned int     SW11                    :1;   // 12
   unsigned int     SW12                    :1;   // 13
   unsigned int     SW13                    :1;   // 14
   unsigned int     SW14                    :1;   // 15
   unsigned int     SW15                    :1;   // 16
};
union DigitalInput_REG
{
   unsigned int     all;
   struct DigitalInPut_BIT bit;
};
struct DigitalOutPut_BIT
{       // bits   description
    unsigned int     NRlyOUT                :1; // 0
    unsigned int     PRlyOUT                :1; // 1
    unsigned int     CHARlyOUT              :1; // 2
    unsigned int     StartBATOUT            :1; // 3
    unsigned int     IMDTOPOUT              :1; // 4
    unsigned int     IMDBOTOUT              :1; // 5
    unsigned int     LEDSysOUT              :1; // 6
    unsigned int     LEDCAnOUT              :1; // 7
    unsigned int     LEDAlarmOUT            :1; // 8
    unsigned int     LEDFaultOUT            :1; // 9
    unsigned int     LEDProtectOUT          :1; // 10
    unsigned int     FNAOUT                 :1; // 11
    unsigned int     PWRHoldOUT             :1; // 12
    unsigned int     DO013                  :1; // 13
    unsigned int     DO014                  :1; // 14
    unsigned int     DO015                  :1; // 15
};
union DigitalOutPut_REG
{
   unsigned int     all;
   struct DigitalOutPut_BIT bit;
};
struct SystemState_BIT
{       // bits   description

    unsigned int     SysSTATE            :3; // 0,1,2
    unsigned int     BalanceMode         :1; // 3
    unsigned int     SysAalarm           :1; // 4
    unsigned int     SysFault            :1; // 5
    unsigned int     SocMode             :1; // 6
    unsigned int     INCANCOMERR         :1; // 7
    unsigned int     EXCANCOMERR         :1; // 8
    unsigned int     TCPIPTOMERR         :1; // 9
    unsigned int     RS485COMERR         :1; // 10
    unsigned int     RTCRD               :1; // 11
    unsigned int     sysDisChaMode       :1; // 12
    unsigned int     INITOK              :1; // 13
    unsigned int     BalanceStatStop     :1; // 14
    unsigned int     CANCOMEnable        :1; // 15
};
union SystemState_REG
{
   unsigned int     all;
   struct Data_WORD        Word;
   struct SystemState_BIT bit;
};
struct SystemAlarm_BIT
{       // bits   description
    unsigned int     PackOC              :1; // 0
    unsigned int     PackVSOC_OV         :1; // 1
    unsigned int     PackVSOC_UN         :1; // 2
    unsigned int     PackVolt_OV         :1; // 3
    unsigned int     PackVolt_UN         :1; // 4
    unsigned int     PackTemp_OV         :1; // 5
    unsigned int     PackTemp_UN         :1; // 6
    unsigned int     PackUnPWR_BL        :1; // 7
    unsigned int     CellVolt_OV         :1; // 8
    unsigned int     CellVolt_UN         :1; // 9
    unsigned int     CellVolt_BL         :1; // 10
    unsigned int     CellTemp_OV         :1; // 11
    unsigned int     CellTemp_UN         :1; // 12
    unsigned int     CellTemp_BL         :1; // 13
    unsigned int     PackCAN_ERR         :1; // 14
    unsigned int     Alarm15             :1; // 14
};
union SystemAlarm_REG
{
   unsigned int     all;
   struct SystemAlarm_BIT bit;
};
struct SystemFault_BIT
{       // bits   description
    unsigned int     PackVCT_OV          :1; // 0
    unsigned int     PackVSOC_OV         :1; // 1
    unsigned int     PackVSOC_UN         :1; // 2
    unsigned int     PackVolt_OV         :1; // 3
    unsigned int     PackVolt_UN         :1; // 4
    unsigned int     PackTemp_OV         :1; // 5
    unsigned int     PackTemp_UN         :1; // 6
    unsigned int     PackUnPWR_BL        :1; // 7
    unsigned int     CellVolt_OV         :1; // 8
    unsigned int     CellVolt_UN         :1; // 9
    unsigned int     CellVolt_BL         :1; // 10
    unsigned int     CellTemp_OV         :1; // 11
    unsigned int     CellTemp_UN         :1; // 12
    unsigned int     CellTemp_BL         :1; // 13
    unsigned int     PackRLY_ERR         :1; // 14
    unsigned int     Fault15             :1; // 15
    unsigned int     CellIR_OV           :1; // 16
    unsigned int     PackOcTime_Err      :1; // 16
    unsigned int     PrtcOcEvent_Err     :1; // 16
};
union SystemFault_REG
{
   unsigned int     all;
   struct Data_WORD        Word;
   struct SystemFault_BIT bit;
};
struct SystemProtect_BIT
{       // bits   description
    unsigned int     PackVCT_OV          :1; // 0
    unsigned int     PackVSOC_OV         :1; // 1
    unsigned int     PackVSOC_UN         :1; // 2
    unsigned int     PackVolt_OV         :1; // 3
    unsigned int     PackVolt_UN         :1; // 4
    unsigned int     PackTemp_OV         :1; // 5
    unsigned int     PackTemp_UN         :1; // 6
    unsigned int     PackUnPWR_BL        :1; // 7
    unsigned int     CellVolt_OV         :1; // 8
    unsigned int     CellVolt_UN         :1; // 9
    unsigned int     CellVolt_BL         :1; // 10
    unsigned int     CellTemp_OV         :1; // 11
    unsigned int     CellTemp_UN         :1; // 12
    unsigned int     CellTemp_BL         :1; // 13
    unsigned int     PackRLY_ERR         :1; // 14
    unsigned int     PackISO_ERR         :1; // 15
};
union SystemProtect_REG
{
   unsigned int     all;
   struct SystemProtect_BIT bit;
};
struct Current_byte
{
    unsigned int CurrentL;
    unsigned int CurrentH;
};
union Currnet_Reg
{
    long                all;
    struct Current_byte byte;
};
struct WORD2BYTE_byte
{
    unsigned int BYTEL;
    unsigned int BYTEH;
};
union WORD2BYTE_Reg
{
    unsigned int           all;
    struct WORD2BYTE_byte byte;
};
typedef struct System_Date
{
    /*
     *
     */
    Uint16  Test;
    Uint16  Maincount;
    Uint16  MainIsr1;
    Uint16  CANRXCOUNT;
    Uint16  TempInitCount;
    Uint16  InitValuleCnt;
    Uint16  CANRXMailBox00Count;
    Uint16  CANRXMailBox01Count;
    Uint16  CANRXMailBox02Count;
    Uint16  CANRXMailBox03Count;
    Uint16  CANRXMailBox04Count;
    Uint16  SysRegTimer5msecCount;
    Uint16  SysRegTimer10msecCount;
    Uint16  SysRegTimer50msecCount;
    Uint16  SysRegTimer100msecCount;
    Uint16  SysRegTimer300msecCount;
    Uint16  SysRegTimer500msecCount;
    Uint16  SysRegTimer1000msecCount;
    Uint16  CellVoltsampling;
    Uint16  CellTempssampling;
    Uint16  SysCanRxCount;
    Uint16  AlarmStatecount;
    Uint16  Bat80VAlarmCont[32];
    Uint16  Bat80VFaultStatecount;
    Uint16  Bat12VFaultStatecount;
    Uint16  ProtectStatecount;
    Uint16  BalanceModeCount;
    Uint16  BalanceTimeCount;
    Uint16  RelayCheck;
    Uint16  Bat80VoltageMaxNum;
    Uint16  Bat80VoltageMinNum;
    float32 Bat80VCellVoltageF[Sys80VCellVoltCount];
    float32 Bat80VCellTemperatureF[Sys80VCellTempCount];
    float32 Bat80VVoltageF;
    float32 Bat80VCurrentF;
    float32 Bat80VFaultCurrentF;
    float32 Bat80VCurrentAsbF;
  //  float32 Bat80VCurrentFaultAsbF;
    float32 Bat80VCellMaxVoltageF;
    float32 Bat80VCellMinVoltageF;
    float32 Bat80VCellDivVoltageF;
    float32 Bat80VCellAgvVoltageF;
    float32 Bat80VlMaxTemperatureF;
    float32 Bat80VCellMaxTemperatureF;
    float32 Bat80VCellMinTemperatureF;
    float32 Bat80VCellDivTemperatureF;
    float32 Bat80VCellAgvTemperatureF;
    Uint16  Bat80TemperatureMaxNum;
    Uint16  Bat80TemperatureMinNum;
    float32 Bat80VCHAPWRContintyF;
    float32 Bat80VDisCHAPWRContintyF;
    float32 Bat80VCHAPWRPeakF;
    float32 Bat80VDisCHAPWRPeakF;
    float32 Bat80VUnbalPwr;
    float32 Bat80VSOCF;
    float32 Bat80VSOHF;
    float32 Bat80VAhF;
    float32 Bat80VISOResisF;





    unsigned int NumA;
    float32 NumB;
    float32 NumC;
    float32 NumCT;
    float32 NumCTin;
//    float32 Bat12VCellVoltage[Sys12VCellVoltCount];
    //float32 Bat80VCellTe[Sys80VCellVoltCount];
    //float32 Bat12VCellVoltage[Sys12VCellVoltCount];
    /*
     *
    */
    unsigned int LEDSycCount;
    unsigned int LEDFaultCount;
    unsigned int LEDCanCount;
    unsigned int StartBATOUTOnCount;
    unsigned int StartBATOUTOffCount;


    //
    unsigned int  BAPackOCCount;
    unsigned int  BAPackOVCount;
    unsigned int  BAPackUVCount;
    unsigned int  BACellOVCount;
    unsigned int  BACellUVCount;
    unsigned int  BACellUBVCount;
    unsigned int  BACellUBTCount;

    SysState    SysMachine;
    union       ParentDeviceCMD_REG         PMSysCMDResg;
    union       SystemState_REG             BAT80VStateReg;
    union       SystemAlarm_REG             BAT80VAlarmReg;
    union       SystemAlarm_REG             BAT80VAlarmBufReg;
    union       SystemFault_REG             BAT80VFaultReg;
    union       SystemFault_REG             BAT80VFaulBuftReg;
    union       SystemProtect_REG           BAT80VProtectReg;
    union       SystemState_REG             BAT12VStateReg;
    union       SystemAlarm_REG             BAT12VAlarmReg;
    union       SystemFault_REG             BAT12VFaulBuftReg;
    union       SystemFault_REG             BAT12VFaultReg;
    union       SystemProtect_REG           BAT12VProtectReg;
    union       DigitalInput_REG            BAT80VDigitalInputReg;
    union       DigitalOutPut_REG           BAT80VDigitalOutPutReg;
    union       Currnet_Reg                 Bat80VCurrentData;
    union       Currnet_Reg                 Bat12VCurrentData;
    union       WORD2BYTE_Reg               PackCOMERR;
}SystemReg;

typedef enum
{
  TIMER_STATE_IDLE,
  TIMER_STATE_RUNNING,
  TIMER_STATE_EXPIRED,
  TIMER_STATE_CLEAR
}TimerState;
typedef struct
{
  TimerState state;
  int TimeCount;
  int Start,Stop,Reset,OutState;
  unsigned int TimerVaule;
}TimerReg;
struct BATStatus_BIT
{       // bits   description
    unsigned int     BATStatus       :3; // 0
    unsigned int     BalanceEN       :1; // 1
    unsigned int     STATE02         :1; // 2
    unsigned int     STATE03         :1; // 3
    unsigned int     STATE04         :1; // 4
    unsigned int     STATE05         :1; // 5
    unsigned int     STATE06         :1; // 6
    unsigned int     STATE07         :1; // 7
    unsigned int     STATE08         :1; // 8
    unsigned int     STATE09         :1; // 9
    unsigned int     STATE10         :1; // 10
    unsigned int     STATE11         :1; // 11
    unsigned int     STATE12         :1; // 12
    unsigned int     STATE13         :1; // 13
    unsigned int     STATE14         :1; // 14
    unsigned int     STATE15         :1; // 15
};
union BATStatus_REG
{
   unsigned int     all;
   struct BATStatus_BIT bit;
};
struct BAT12VStatus_BIT
{       // bits   description
    unsigned int     BalanceEN       :1; // 0
    unsigned int     PRelayEN       :1; // 1
    unsigned int     STATE02         :1; // 2
    unsigned int     STATE03         :1; // 3
    unsigned int     STATE04         :1; // 4
    unsigned int     STATE05         :1; // 5
    unsigned int     STATE06         :1; // 6
    unsigned int     STATE07         :1; // 7
    unsigned int     STATE08         :1; // 8
    unsigned int     STATE09         :1; // 9
    unsigned int     STATE10         :1; // 10
    unsigned int     STATE11         :1; // 11
    unsigned int     STATE12         :1; // 12
    unsigned int     STATE13         :1; // 13
    unsigned int     STATE14         :1; // 14
    unsigned int     STATE15         :1; // 15
};
union BAT12VStatus_REG
{
   unsigned int     all;
   struct BAT12VStatus_BIT bit;
};
struct VCUCOMMAND_BIT
{       // bits   description
   unsigned int     PCCMD00         :1; // 0
   unsigned int     PCCMD01         :1; // 1
   unsigned int     PCCMD02         :1; // 2
   unsigned int     PCCMD03         :1; // 3
   unsigned int     PCCMD04         :1; // 4
   unsigned int     PCCMD05         :1; // 5
   unsigned int     PCCMD06         :1; // 6
   unsigned int     PCCMD07         :1; // 7
   unsigned int     RUNStatus       :1; // 8
   unsigned int     PrtctReset      :1; // 9
   unsigned int     PCCMD10         :1; // 10
   unsigned int     PCCMD11         :1; // 11
   unsigned int     PCCMD12         :1; // 12
   unsigned int     PCCMD13         :1; // 13
   unsigned int     PCCMD14         :1; // 14
   unsigned int     PCCMD15         :1; // 15
};
union VCUCOMMAND_REG
{
   unsigned int     all;
   struct VCUCOMMAND_BIT bit;
};
struct NVRSET_BIT
{       // bits   description
    unsigned int     NVRInit         :1; // 0
    unsigned int     NVRforceWR      :1; // 1
    unsigned int     NVRSocInit      :1; // 2
    unsigned int     NVRManual       :1; // 3
    unsigned int     NVRLogRead      :1; // 4
    unsigned int     NVREventRead    :1; // 5
    unsigned int     STATE06         :1; // 6
    unsigned int     STATE07         :1; // 7
    unsigned int     STATE08         :1; // 8
    unsigned int     STATE09         :1; // 9
    unsigned int     STATE10         :1; // 10
    unsigned int     STATE11         :1; // 11
    unsigned int     STATE12         :1; // 12
    unsigned int     STATE13         :1; // 13
    unsigned int     STATE14         :1; // 14
    unsigned int     STATE15         :1; // 15
};
union NVRSET_REG
{
   unsigned int     all;
   struct NVRSET_BIT bit;
};
typedef struct CANA_DATA
{

    Uint16 SWTypeVer;
    /*
     *
     */
    Uint16 CellNumStart;
    Uint16 NumberShift;
    Uint16 CellVotlageNumber;
    Uint16 CellVotlageMaxNumber;
    Uint16 CellVoltageNum;
    Uint16 CellVoltTxNum;
    Uint16 CellVoltTxA;
    Uint16 CellVoltTxB;
    Uint16 CellVoltTxC;

    Uint16 CellTempsTxNum;
    int16 CellTempsTxA;
    int16 CellTempsTxB;
    int16 CellTempsTxC;


    Uint16 CellIRTxNum;
    Uint16 CellIRTxA;
    Uint16 CellIRTxB;
    Uint16 CellIRTxC;

    Uint16 CANTxA;
    Uint16 CANTxB;
    Uint16 CANTxC;
    Uint16 CANTxD;
    Uint16 CANTxE;
    Uint16 AlarmNum;



    Uint16 BAT80VoltageCell[24];
    int16  BAT80VTemperatureCell[24];
    Uint16 BAT80VInternalResCell[24];

    Uint16 CellVoltagteInf;
    Uint16 CellVoltagteTotalNum;
    Uint16 CellVoltagteTotalNumShift;
    Uint16 CellVoltagteStartNum;
    Uint16 CellNumTStart;
    Uint16 NumberTShift;
    Uint16 CellTemperatureNumber;
    Uint16 CellTemperatureMaxNumber;
    Uint16 CellTemperatureNum;
    Uint16 BAT80VTempCell[24];
    Uint16 BAT80VConfing;
    /*
     *
     */

    union BATStatus_REG               BAT80VStatus;
    union DigitalOutPut_REG           BAT80VDigitalOutPutReg;
    union VCUCOMMAND_REG              PMSCMDRegs;
//    union VCUCOMMAND_REG              PMSCMDRegs;
    Uint16 BAT80VConFig;
    int16  BAT80VSOC;
    Uint16 BAT80VSOH;
    Uint16 BAT80VAh;
    int16  BAT80VCT;
    int16  BAT80VFaultCT;
   // int16  BAT80VCTFaultVaule;
    Uint16 BAT80VPT;
    Uint16 BAT80VCHAPWRContinty;
    Uint16 BAT80VCHAPWRPeak;
    Uint16 BAT80VDisCHAPWRContinty;
    Uint16 BAT80VDisCHAPWRPeak;
    Uint16 BAT80VoltageMax;
    Uint16 BAT80VoltageMin;
    Uint16 BAT80VoltageAgv;
    Uint16 BAT80VoltageDiv;
    Uint16 BAT80VoltageMaxNum;
    Uint16 BAT80VoltageMinNum;
    Uint16 BAT80VPackVotageBuf;
    int16  BAT80VTemperaturelMAX;
    int16  BAT80VTemperaturelMIN;
    int16  BAT80VTemperatureAVG;
    Uint16 BAT80VTemperatureDiv;

    int16  BAT80VBatIRlMAX;
    int16  BAT80VBatIRlMIN;
    int16  BAT80VBatIRAVG;
    Uint16 BAT80VBatIRDiv;



    Uint16 BAT80VTemperatureMAXNUM;
    Uint16 BAT80VTemperatureMINNUM;
    union  WORD2BYTE_Reg    SwVerProducttype;
    union  WORD2BYTE_Reg    BatConfParallelSerial;
    union  NVRSET_REG       NVRSetRegs;
    /*
     *
     */
    union  BATStatus_REG         BAT12VStatus;
    union  BAT12VStatus_REG       BAT12VStatus_A;
    Uint16 BAT12VoltageCell[4];
    int16  BAT12TempCell[4];
    Uint16 BAT12VPT ;
    Uint16 BAT12VCT ;
    int16  BAT12VSOC;
    Uint16 BAT12VSOH;
    Uint16 BAT12VAh;
    Uint16 BAT12VCHAPWRContinty;
    Uint16 BAT12VCHAPWRPeak;
    Uint16 BAT12VDisCHAPWRContinty;
    Uint16 BAT12VDisCHAPWRPeak;
    Uint16 BAT12VoltageMax;
    Uint16 BAT12VoltageMin;
    Uint16 BAT12VoltageAgv;
    Uint16 BAT12VoltageDiv;
    Uint16 BAT12VoltageMaxNum;
    Uint16 BAT12VoltageMinNum;
    int16  BAT12VTemperatureMAX ;
    int16  BAT12VTemperaturelMIN;
    int16  BAT12VTemperatureAVG;
    int16  BAT12VTemperatureDiv;
    Uint16 BAT12VTemperatureMAXNUM;
    Uint16 BAT12VTemperatureMINNUM;
    int16 NVRSocInit;
    /*
     *
     */
    Uint16 MailBoxRxCount;
    Uint16 MailBox0RxCount;
    Uint16 MailBox1RxCount;
    Uint16 MailBox2RxCount;
    Uint16 MailBox3RxCount;
//  Uint16 CANID;



}CANAReg;
#endif  // end of DSP28x_PROJECT_H definition

