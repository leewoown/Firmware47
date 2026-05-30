
#include "parameter.h"
#include "SysVariable.h"
#include "DSP28x_Project.h"
#include "stdio.h"
#include "math.h"
#include <string.h>

extern void SysTimerINIT(SystemReg *s);
extern void CANRegVarINIT(CANAReg *P);
extern void SysVarINIT(SystemReg *s);
extern void DigitalInput(SystemReg *sys);
extern void DigitalOutput(SystemReg *sys);
extern void CANATX(unsigned int ID, unsigned char Length, unsigned int Data0, unsigned int Data1,unsigned int Data2,unsigned int Data3);
extern void Cal80VSysVoltageHandle(SystemReg *s);
extern void Cal80VSysTemperatureHandle(SystemReg *s);
extern void Cal80VSysCurrentHandle(SystemReg *s);
extern void Cal80VSysFaultCheck(SystemReg *s);
extern void Cal80VSysAlarmtCheck(SystemReg *s);
extern int float32ToInt(float32 Vaule, Uint32 Num);
extern void TempTemps(SystemReg *s);
//extern SystemReg       SysRegs;

#define A 1664525
#define C 1013904223
#define M 4294967296 // 2^32


void TempTemps(SystemReg *s)
{

    s->NumA=(float32)(s->MainIsr1/3000);
}

void CANATX(unsigned int ID, unsigned char Length, unsigned int Data0, unsigned int Data1,unsigned int Data2,unsigned int Data3)
{
    struct ECAN_REGS ECanaShadow;
    unsigned int CANWatchDog;
    unsigned int Data0Low, Data0High, Data1Low, Data1High;
    unsigned int Data2Low, Data2High, Data3Low, Data3High;

 //   CANWatchDog=0;

    Data0Low  = 0x00ff&Data0;
    Data0High = 0x00ff&(Data0>>8);
    Data1Low  = 0x00ff&Data1;
    Data1High = 0x00ff&(Data1>>8);
    Data2Low  = 0x00ff&Data2;
    Data2High = 0x00ff&(Data2>>8);
    Data3Low  = 0x00ff&Data3;
    Data3High = 0x00ff&(Data3>>8);



    EALLOW;
    ECanaShadow.CANME.all = ECanaRegs.CANME.all;
    ECanaShadow.CANME.bit.ME31=0;
    ECanaRegs.CANME.bit.ME31= ECanaShadow.CANME.bit.ME31;

 //   ECanaMboxes.MBOX31.MSGID.bit.STDMSGID=ID;
    ECanaMboxes.MBOX31.MSGID.all = 0UL;
    ECanaMboxes.MBOX31.MSGID.bit.IDE = 0U;                 // 표준 프레임
    ECanaMboxes.MBOX31.MSGID.bit.STDMSGID = (Uint16)(ID & 0x07FFU); // 11-bit

    ECanaMboxes.MBOX31.MSGCTRL.bit.RTR = 0U;
    ECanaMboxes.MBOX31.MSGCTRL.bit.DLC=Length;


    ECanaMboxes.MBOX31.MDL.byte.BYTE0=Data0Low;
    ECanaMboxes.MBOX31.MDL.byte.BYTE1=Data0High;
    ECanaMboxes.MBOX31.MDL.byte.BYTE2=Data1Low;
    ECanaMboxes.MBOX31.MDL.byte.BYTE3=Data1High;
    ECanaMboxes.MBOX31.MDH.byte.BYTE4=Data2Low;
    ECanaMboxes.MBOX31.MDH.byte.BYTE5=Data2High;
    ECanaMboxes.MBOX31.MDH.byte.BYTE6=Data3Low;
    ECanaMboxes.MBOX31.MDH.byte.BYTE7=Data3High;

    ECanaShadow.CANME.all = ECanaRegs.CANME.all;
    ECanaShadow.CANME.bit.ME31 = 1U;
    ECanaRegs.CANME.bit.ME31= ECanaShadow.CANME.bit.ME31;
    EDIS;

    ECanaRegs.CANTRS.bit.TRS31 = 1U;

    while (ECanaRegs.CANTA.bit.TA31 == 0U)
    {
        if (++CANWatchDog > 2000U)
        {
           break; // 타임아웃
       }
    }
    // TA31 플래그 클리어
    if (ECanaRegs.CANTA.bit.TA31 == 1U)
    {
       ECanaRegs.CANTA.bit.TA31 = 1U;
    }

}
void SysTimerINIT(SystemReg *s)
{
    s->SysMachine=System_STATE_INIT;
    s->Maincount=0;
    s->MainIsr1=0;
    s->CANRXCOUNT=0;
    s->CANRXMailBox00Count=0;
    s->CANRXMailBox01Count=0;
    s->CANRXMailBox02Count=0;
    s->CANRXMailBox03Count=0;
    s->CANRXMailBox04Count=0;
}
void SysVarINIT(SystemReg *s)
{

    s->PMSysCMDResg.all=0;
    s->BAT80VStateReg.all=0;
    s->BAT80VAlarmReg.all=0;
    s->BAT80VFaultReg.all=0;
    s->BAT80VFaulBuftReg.all=0;
    s->BAT80VProtectReg.all=0;
    s->BAT12VStateReg.all=0;
    s->BAT12VAlarmReg.all=0;
    s->BAT12VFaulBuftReg.all=0;
    s->BAT12VFaultReg.all=0;
    s->BAT12VProtectReg.all=0;
    s->BAT80VDigitalInputReg.all=0;
    s->BAT80VDigitalOutPutReg.all=0;
    s->Bat80VCurrentData.all=0x80000000;
    s->Bat12VCurrentData.all=0x80000000;
    s->PackCOMERR.all=0;

    s->Test=0;
    s->Maincount=0;
    s->InitValuleCnt=0;
    s->MainIsr1=0;
    s->CANRXCOUNT=0;
    s->CANRXMailBox00Count=0;
    s->CANRXMailBox01Count=0;
    s->CANRXMailBox02Count=0;
    s->CANRXMailBox03Count=0;
    s->CANRXMailBox04Count=0;
    s->SysRegTimer5msecCount=0;
    s->SysRegTimer10msecCount=0;
    s->SysRegTimer50msecCount=0;
    s->SysRegTimer100msecCount=0;
    s->SysRegTimer300msecCount=0;
    s->SysRegTimer500msecCount=0;
    s->SysRegTimer1000msecCount=0;
    s->CellVoltsampling=0;
    s->CellTempssampling=0;
    s->SysCanRxCount=0;
    s->AlarmStatecount=0;
    s->Bat80VFaultStatecount=0;
    s->Bat12VFaultStatecount=0;
    s->ProtectStatecount=0;
    s->RelayCheck=0;
    s->Bat80VoltageMaxNum=0;
    s->Bat80VoltageMinNum=0;

    s->Bat80VVoltageF=0;
    s->Bat80VCurrentF=0;
    s->Bat80VCurrentAsbF=0;
    s->Bat80VCellMaxVoltageF=0;
    s->Bat80VCellMinVoltageF=0;
    s->Bat80VCellDivVoltageF=0;
    s->Bat80VCellAgvVoltageF=0;
    s->Bat80VlMaxTemperatureF=0;
    s->Bat80VCellMaxTemperatureF=0;
    s->Bat80VCellMinTemperatureF=0;
    s->Bat80VCellDivTemperatureF=0;
    s->Bat80VCellAgvTemperatureF=0;
    s->Bat80VCHAPWRContintyF=5;//5
    s->Bat80VDisCHAPWRContintyF=5;
    s->Bat80VCHAPWRPeakF=5;
    s->Bat80VDisCHAPWRPeakF=5;
    s->Bat80VSOCF=0;
    s->Bat80VSOHF=0;
    s->Bat80VAhF=0;
    s->Bat80VISOResisF=0;



    s->BAPackOCCount=0;
    s->BAPackOVCount=0;
    s->BAPackUVCount=0;
    s->BACellOVCount=0;
    s->BACellUVCount=0;
    s->BACellUBVCount=0;
    s->BACellUBTCount=0;

    s->BalanceModeCount=0;    
    s->BalanceTimeCount=0;
    s->Bat80VFaultCurrentF=0.0;
    s->TempInitCount=0;

    memset(&s->Bat80VAlarmCont[0],0.0,32);
    memset(&s->Bat80VCellVoltageF[0],0.0,Sys80VCellVoltCount);
    memset(&s->Bat80VCellTemperatureF[0],0.0,Sys80VCellTempCount);
}
void CANRegVarINIT(CANAReg *P)
{
    P->SWTypeVer=0;
    /*
     *
     */
    P->NVRSocInit=0;
    P->NVRSetRegs.all=0;
    P->CellNumStart=0;;
    P-> NumberShift=0;;
    P->CellVotlageNumber=0;;
    P->CellVotlageMaxNumber=0;;
    P->CellVoltageNum=0;
    P->PMSCMDRegs.all=0;
    P->BAT80VDigitalOutPutReg.all=0;
    P->SwVerProducttype.all=0;
    P->BatConfParallelSerial.all=0;
    memset(&P->BAT80VoltageCell[0],0,24);
    P->CellNumTStart=0;
    P->NumberTShift=0;
    P->CellTemperatureNumber=0;
    P->CellTemperatureMaxNumber=0;
    P->CellTemperatureNum=0;
    memset(&P->BAT80VTempCell[0],0,24);
    /*
     *
     */
    P->BAT80VConfing=0;
    P->BAT80VStatus.all=0;
    P->BAT80VConFig=0;
    P->BAT80VSOC=0;
    P->BAT80VSOH=0;
    P->BAT80VAh=0;
    P->BAT80VCT=0;
    P->BAT80VPT=0;
    P->BAT80VCHAPWRContinty=0;
    P->BAT80VCHAPWRPeak=0;
    P->BAT80VDisCHAPWRContinty=0;
    P->BAT80VDisCHAPWRPeak=0;
    P->BAT80VoltageMax=0;
    P->BAT80VoltageMin=0;
    P->BAT80VoltageAgv=0;
    P->BAT80VoltageDiv=0;
    P->BAT80VoltageMaxNum=0;
    P->BAT80VoltageMinNum=0;
    P->BAT80VPackVotageBuf=0;

    P->BAT80VTemperaturelMAX=0;
    P->BAT80VTemperaturelMIN=0;
    P->BAT80VTemperatureAVG=0;
    P->BAT80VTemperatureDiv=0;
    P->BAT80VTemperatureMAXNUM=0;
    P->BAT80VTemperatureMINNUM=0;

    P->BAT12VStatus.all=0;
    P->BAT12VStatus_A.all=0;
    memset(&P->BAT12VoltageCell[0],0,4);
    memset(&P->BAT12TempCell[0],0,4);
    P->BAT12VPT=0;
    P->BAT12VCT=0;
    P->BAT12VSOC=0;
    P->BAT12VSOH=0;
    P->BAT12VAh=0;
    P->BAT12VCHAPWRContinty=0;
    P->BAT12VCHAPWRPeak=0;
    P->BAT12VDisCHAPWRContinty=0;
    P->BAT12VDisCHAPWRPeak=0;
    P->BAT12VoltageMax=0;
    P->BAT12VoltageMin=0;
    P->BAT12VoltageAgv=0;
    P->BAT12VoltageDiv=0;
    P->BAT12VoltageMaxNum=0;
    P->BAT12VoltageMinNum=0;
    P->BAT12VTemperatureMAX =0;
    P->BAT12VTemperaturelMIN=0;
    P->BAT12VTemperatureAVG=0;
    P->BAT12VTemperatureDiv=0;
    P->BAT12VTemperatureMAXNUM=0;
    P->BAT12VTemperatureMINNUM=0;
    P->MailBoxRxCount=0;
    P->MailBox0RxCount=0;
    P->MailBox1RxCount=0;
    P->MailBox2RxCount=0;
    P->MailBox3RxCount=0;
}
void Cal80VSysVoltageHandle(SystemReg *s)
{

    Uint16  CellCount=0;
    Uint16  CellSize=0;
    float32 SysCellMaxVoltageF=0;
    float32 SysCellMinVoltageF=0;
    float32 SysVoltageBufF=0;
    SysCellMaxVoltageF =s->Bat80VCellVoltageF[0];
    SysCellMinVoltageF =s->Bat80VCellVoltageF[0];
    CellSize = Sys80VCellVoltCount;//22
    for(CellCount=0;CellCount<CellSize;CellCount++)
    {
         if (SysCellMaxVoltageF <= s->Bat80VCellVoltageF[CellCount])
         {
             SysCellMaxVoltageF    =  s->Bat80VCellVoltageF[CellCount];
             s->Bat80VoltageMaxNum=CellCount;
         }
         if (SysCellMinVoltageF >= s->Bat80VCellVoltageF[CellCount])
         {
             SysCellMinVoltageF    =  s->Bat80VCellVoltageF[CellCount];
             s->Bat80VoltageMinNum=CellCount;
         }
    }
    s->Bat80VCellMaxVoltageF    = SysCellMaxVoltageF;
    s->Bat80VCellMinVoltageF    = SysCellMinVoltageF;
    s->Bat80VCellDivVoltageF    = (SysCellMaxVoltageF- SysCellMinVoltageF);
    CellSize = Sys80VCellVoltCount;
    for(CellCount=0;CellCount<CellSize;CellCount++)
    {
        SysVoltageBufF = SysVoltageBufF+ s->Bat80VCellVoltageF[CellCount];
    }
    s->Bat80VVoltageF          = SysVoltageBufF;
    s->Bat80VCellAgvVoltageF   =  s->Bat80VVoltageF/22.0;
}

void Cal80VSysTemperatureHandle(SystemReg *s)
{

    Uint16  CellCount=0;
    Uint16  CellSize=0;
    float32 SysCellMaxTemperatureF=0;
    float32 SysCellMinTemperatureF=0;
    float32 SysTemperatureBufF=0;
    CellSize = 22;
    SysCellMaxTemperatureF =s->Bat80VCellTemperatureF [0];
    SysCellMinTemperatureF =s->Bat80VCellTemperatureF [0];
    for(CellCount=0;CellCount<CellSize;CellCount++)
    {
         if (SysCellMaxTemperatureF < s->Bat80VCellTemperatureF[CellCount])
         {
             SysCellMaxTemperatureF    =  s->Bat80VCellTemperatureF[CellCount];
             s->Bat80TemperatureMaxNum=CellCount;
         }
         if (SysCellMinTemperatureF > s->Bat80VCellTemperatureF[CellCount])
         {
             SysCellMinTemperatureF    =  s->Bat80VCellTemperatureF[CellCount];
             s->Bat80TemperatureMinNum=CellCount;
         }
    }
    s->Bat80VCellMaxTemperatureF    = SysCellMaxTemperatureF;
    s->Bat80VCellMinTemperatureF    = SysCellMinTemperatureF;
    s->Bat80VCellDivTemperatureF    = (SysCellMaxTemperatureF-SysCellMinTemperatureF);
    CellSize =22;
    SysTemperatureBufF=0.0;
    for(CellCount=0;CellCount<CellSize;CellCount++)
    {
        SysTemperatureBufF = SysTemperatureBufF+ s->Bat80VCellTemperatureF[CellCount];
    }
    s->Bat80VCellAgvTemperatureF   = (float32)SysTemperatureBufF/CellSize;
}

void Cal80VSysCurrentHandle(SystemReg *s)
{
    long  CurrentCT  = 0;
    float32 Currentbuf = 0;
    CurrentCT  = s->Bat80VCurrentData.all;
    CurrentCT  =  CurrentCT - 0x80000000;

    Currentbuf        =  ((float)CurrentCT)/1000;          // (mA to A) CAB500 resolution 1mA
    s->Bat80VCurrentF  = C_CTDirection * Currentbuf;    // Decide Current sensor's direction

    if(s->Bat80VCurrentF>=700.0)
    {
        s->Bat80VCurrentF=700.0;
    }
    if(s->Bat80VCurrentF<=-700.0)
    {
        s->Bat80VCurrentF=-700.0;
    }
    if(s->Bat80VCurrentF <= 0)
    {
        s->Bat80VCurrentAsbF =-1.0 * s->Bat80VCurrentF;
    }
    else
    {
        s->Bat80VCurrentAsbF =s->Bat80VCurrentF;
    }

}
void Cal80VSysAlarmtCheck(SystemReg *s)
{

     // 과전류 Alarm,유지시간카운터배열값:0,유지시간;100msec
     // TODO(26.05.30기준반영, 검증 후 정리): WrnOC 450A/405A (이전 500/456)
     if(Hyst_On(s->Bat80VCurrentAsbF,C_PackCTOV_Warn))
     {
          if(s->Bat80VAlarmCont[0]< C_PackCTOV_WarnDelay){++s->Bat80VAlarmCont[0];}
          if(s->Bat80VAlarmCont[0]>=C_PackCTOV_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.PackOC=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.PackOC==0)
          {
              s->Bat80VAlarmCont[0]=0;
          }
          if(Hyst_Off(s->Bat80VCurrentAsbF,C_PackCTOV_WarnRst))
          {
              s->Bat80VAlarmCont[0]=0;
              s->BAT80VAlarmReg.bit.PackOC=0;
          }
      }
      // 팩 과충전 Alarm,유지시간카운터배열값:1,유지시간;100msec
      if(Hyst_On(s->Bat80VSOCF,C_PackSOCOV_Warn))
      {
          if(s->Bat80VAlarmCont[1]< C_PackSOCOV_WarnDelay){++s->Bat80VAlarmCont[1];}
          if(s->Bat80VAlarmCont[1]>=C_PackSOCOV_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.PackVSOC_OV=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.PackVSOC_OV==0)
          {
              s->Bat80VAlarmCont[1]=0;
          }
          if(Hyst_Off(s->Bat80VSOCF,C_PackSOCOV_WarnRst))
          {
              s->Bat80VAlarmCont[1]=0;
              s->BAT80VAlarmReg.bit.PackVSOC_OV=0;
          }
      }
      // 팩 저충전,유지시간카운터배열값:2,유지시간;100msec
      if(Hyst_Off(s->Bat80VSOCF,C_PackSOCUN_Warn))
      {
          if(s->Bat80VAlarmCont[2]< C_PackSOCUN_WarnDelay){++s->Bat80VAlarmCont[2];}
          if(s->Bat80VAlarmCont[2]>=C_PackSOCUN_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.PackVSOC_UN=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.PackVSOC_UN==0)
          {
              s->Bat80VAlarmCont[2]=0;
          }
          if(Hyst_On(s->Bat80VSOCF,C_PackSOCUN_WarnRst))
          {
              s->Bat80VAlarmCont[2]=0;
              s->BAT80VAlarmReg.bit.PackVSOC_UN=0;
          }
      }
      // 팩 과전압 Alarm,유지시간카운터배열값:3,유지시간;100msec
      if(Hyst_On(s->Bat80VVoltageF,C_PackVoltOV_Warn))
      {
          if(s->Bat80VAlarmCont[3]< C_PackVoltOV_WarnDelay){++s->Bat80VAlarmCont[3];}
          if(s->Bat80VAlarmCont[3]>=C_PackVoltOV_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.PackVolt_OV=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.PackVolt_OV==0)
          {
              s->Bat80VAlarmCont[3]=0;
          }
          if(Hyst_Off(s->Bat80VVoltageF,C_PackVoltOV_WarnRst))
          {
              s->Bat80VAlarmCont[3]=0;
              s->BAT80VAlarmReg.bit.PackVolt_OV=0;
          }
      }
      // 팩 저전압 Alarm,유지시간카운터배열값:4,유지시간;100msec
      if(Hyst_Off(s->Bat80VVoltageF,C_PackVoltUN_Warn))
      {
          if(s->Bat80VAlarmCont[4]< C_PackVoltUN_WarnDelay){++s->Bat80VAlarmCont[4];}
          if(s->Bat80VAlarmCont[4]>=C_PackVoltUN_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.PackVolt_UN=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.PackVolt_UN==0)
          {
              s->Bat80VAlarmCont[4]=0;
          }
          if(Hyst_On(s->Bat80VVoltageF,C_PackVoltUN_WarnRst))
          {
              s->Bat80VAlarmCont[4]=0;
              s->BAT80VAlarmReg.bit.PackVolt_UN=0;
          }
      }
      // 팩 과온 Alarm,유지시간카운터배열값:5,유지시간;100msec
      if(Hyst_On(s->Bat80VCellAgvTemperatureF,C_PackTempOV_Warn))
      {
          if(s->Bat80VAlarmCont[5]< C_PackTempOV_WarnDelay){++s->Bat80VAlarmCont[5];}
          if(s->Bat80VAlarmCont[5]>=C_PackTempOV_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.PackTemp_OV=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.PackTemp_OV==0)
          {
              s->Bat80VAlarmCont[5]=0;
          }
          if(Hyst_Off(s->Bat80VCellAgvTemperatureF,C_PackTempOV_WarnRst))
          {
              s->Bat80VAlarmCont[5]=0;
              s->BAT80VAlarmReg.bit.PackTemp_OV=0;
          }
      }
      // 팩 저온 Alarm,유지시간카운터배열값:6,유지시간;100msec
      // TODO(버그 F, 검증 후 정리): 기존 else 분기가 sysDisChaMode==1을 재검사하는 dead code였고,
      //   else(충전모드)에 방전분기와 동일 로직이 복붙되어 있었음 -> 충/방전 무관하게 평가하도록 모드 게이팅 제거.
      if(Hyst_Off(s->Bat80VCellAgvTemperatureF,C_PackTempUN_Warn))
      {
          if(s->Bat80VAlarmCont[6]< C_PackTempUN_WarnDelay){++s->Bat80VAlarmCont[6];}
          if(s->Bat80VAlarmCont[6]>=C_PackTempUN_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.PackTemp_UN=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.PackTemp_UN==0)
          {
              s->Bat80VAlarmCont[6]=0;
          }
          if(Hyst_On(s->Bat80VCellAgvTemperatureF,C_PackTempUN_WarnRst))
          {
              s->Bat80VAlarmCont[6]=0;
              s->BAT80VAlarmReg.bit.PackTemp_UN=0;
          }
      }
      if(s->BAT80VStateReg.bit.sysDisChaMode==1)
      {
       /* s->BBat80VDisCHAPWRContintyF
          if(Hyst_Off(s->Bat80VUnbalPwr,-15.0f))
          {
              if(s->Bat80VAlarmCont[7]< C_PackUnbalPwr_WarnDelay){++s->Bat80VAlarmCont[7];}
              if(s->Bat80VAlarmCont[7]>=C_PackUnbalPwr_WarnDelay)
              {
                  s->BAT80VAlarmReg.bit.PackUnPWR_BL=1;
              }
          }
          else
          {
              if(s->BAT80VAlarmReg.bit.PackUnPWR_BL==0)
              {
                  s->Bat80VAlarmCont[7]=0;
              }
              if(Hyst_On(s->Bat80VUnbalPwr,0.0f))
              {
                  s->Bat80VAlarmCont[7]=0;
                  s->BAT80VAlarmReg.bit.PackUnPWR_BL=0;
              }
          }
        */
      }
      else if(s->BAT80VStateReg.bit.sysDisChaMode==0)
      {

      }

      // 셀 과전압 Alarm,유지시간카운터배열값:8,유지시간:00msec
      if(Hyst_On(s->Bat80VCellMaxVoltageF,C_CellVoltOV_Warn))
      {
          if(s->Bat80VAlarmCont[8]< C_CellVoltOV_WarnDelay){++s->Bat80VAlarmCont[8];}
          if(s->Bat80VAlarmCont[8]>=C_CellVoltOV_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.CellVolt_OV=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.CellVolt_OV==0)
          {
              s->Bat80VAlarmCont[8]=0;
          }
          if(Hyst_Off(s->Bat80VCellMaxVoltageF,C_CellVoltOV_WarnRst))
          {
              s->Bat80VAlarmCont[8]=0;
              s->BAT80VAlarmReg.bit.CellVolt_OV=0;
          }
      }
      // TODO:셀 저전압 Alarm,유지시간카운터배열값:9,유지시간:100msec
      if(Hyst_Off(s->Bat80VCellMinVoltageF,C_CellVoltUN_Warn))
      {
          if(s->Bat80VAlarmCont[9]< C_CellVoltUN_WarnDelay){++s->Bat80VAlarmCont[9];}
          if(s->Bat80VAlarmCont[9]>=C_CellVoltUN_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.CellVolt_UN=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.CellVolt_UN==0)
          {
              s->Bat80VAlarmCont[9]=0;
          }
          if(Hyst_On(s->Bat80VCellMinVoltageF,C_CellVoltUN_WarnRst))
          {
              s->Bat80VAlarmCont[9]=0;
              s->BAT80VAlarmReg.bit.CellVolt_UN=0;
          }
      }
      // 셀 전압 편차 Alarm,유지시간카운터배열값:10,유지시간:100msec
      if(Hyst_On(s->Bat80VCellDivVoltageF,C_CellVoltDIV_Warn))
      {
          if(s->Bat80VAlarmCont[10]< C_CellVoltDIV_WarnDelay){++s->Bat80VAlarmCont[10];}
          if(s->Bat80VAlarmCont[10]>=C_CellVoltDIV_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.CellVolt_BL=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.CellVolt_BL==0)
          {
              s->Bat80VAlarmCont[10]=0;
          }
          if(Hyst_Off(s->Bat80VCellDivVoltageF,C_CellVoltDIV_WarnRst))
          {
              s->Bat80VAlarmCont[10]=0;
              s->BAT80VAlarmReg.bit.CellVolt_BL=0;
          }
      }
      // 셀 과온 Alarm,유지시간카운터배열값:11,유지시간:100msec
      if(Hyst_On(s->Bat80VCellMaxTemperatureF,C_CellTempOV_Warn))
      {
          if(s->Bat80VAlarmCont[11]< C_CellTempOV_WarnDelay){++s->Bat80VAlarmCont[11];}
          if(s->Bat80VAlarmCont[11]>=C_CellTempOV_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.CellTemp_OV=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.CellTemp_OV==0)
          {
              s->Bat80VAlarmCont[11]=0;
          }
          if(Hyst_Off(s->Bat80VCellMaxTemperatureF,C_CellTempOV_WarnRst))
          {
              s->Bat80VAlarmCont[11]=0;
              s->BAT80VAlarmReg.bit.CellTemp_OV=0;
          }
      }
      // 셀 저온 Alarm,유지시간카운터배열값:12,유지시간:100msec
      if(Hyst_Off(s->Bat80VCellMinTemperatureF,C_CellTempUN_Warn))
      {
          if(s->Bat80VAlarmCont[12]< C_CellTempUN_WarnDelay){++s->Bat80VAlarmCont[12];}
          if(s->Bat80VAlarmCont[12]>=C_CellTempUN_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.CellTemp_UN=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.CellTemp_UN==0)
          {
              s->Bat80VAlarmCont[12]=0;
          }
          if(Hyst_On(s->Bat80VCellMinTemperatureF,C_CellTempUN_WarnRst))
          {
              s->Bat80VAlarmCont[12]=0;
              s->BAT80VAlarmReg.bit.CellTemp_UN=0;
          }
      }
      // 셀 온도 편차 Alarm,유지시간카운터배열값:13,유지시간:100msec
      if(Hyst_On(s->Bat80VCellDivTemperatureF,C_CellTempDIV_Warn))
      {
          if(s->Bat80VAlarmCont[13]< C_CellTempDIV_WarnDelay){++s->Bat80VAlarmCont[13];}
          if(s->Bat80VAlarmCont[13]>=C_CellTempDIV_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.CellTemp_BL=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.CellTemp_BL==0)
          {
              s->Bat80VAlarmCont[13]=0;
          }
          if(Hyst_Off(s->Bat80VCellDivTemperatureF,C_CellTempDIV_WarnRst))
          {
              s->Bat80VAlarmCont[13]=0;
              s->BAT80VAlarmReg.bit.CellTemp_BL=0;
          }
      }

}
unsigned int    CellVoltUnBalaneFaulCount=0;
void Cal80VSysFaultCheck(SystemReg *s)
{
      // 과전류 FAULT
      /*
      if(s->Bat80VCurrentAsbF >= C_Bat80VOVPackCurrentFault)
      {
          s->BAPackOCCount++;
          s-> BAT80VFaulBuftReg.bit.PackVCT_OV=1;
          if(s->BAPackOCCount >= C_PackCTOV_FaultDelay)
          {
              s-> BAT80VFaultReg.bit.PackVCT_OV=1;
              s->BAPackOCCount=C_PackCTOV_FaultDelay+10;
          }
      }
      else
      {
          s->BAPackOCCount=0;
         // s-> BAT80VFaultReg.bit.PackVCT_OV=0;
          s-> BAT80VFaulBuftReg.bit.PackVCT_OV=0;
      }
      */


      if(s->Bat80VCurrentAsbF >= C_PackCTOV_Fault)
      {
          s-> BAT80VFaulBuftReg.bit.PackVCT_OV=1;
          s-> BAT80VFaultReg.bit.PackVCT_OV=1;
          s-> Bat80VFaultCurrentF=s->Bat80VCurrentF;
      }
      if(s->Bat80VCurrentAsbF >= C_PackOCTimer_Fault)
      {
          s->BAPackOCCount++;
          // TODO(26.05.30기준반영, 검증 후 정리): 1sec 유지카운트를 #define(26.05.30기준1sec)으로 대체. 이전값 2000.
          if(s->BAPackOCCount>=C_PackOCTimerCount)
          {
              s->BAPackOCCount=30000;
              s-> BAT80VFaultReg.bit.PackOcTime_Err =1;
            //  s-> BAT80VFaultReg.bit.PrtcOcEvent_Err =0;
          }
      }
      else
      {
          s->BAPackOCCount=0;
          s-> BAT80VFaultReg.bit.PackOcTime_Err =0;
      }
      // 과충전 FAULT
      if(s->Bat80VSOCF >=C_PackSOCOV_Fault)
      {

          s->BAT80VFaulBuftReg.bit.PackVSOC_OV =1;
          s->BAT80VFaultReg.bit.PackVSOC_OV=1;

      }
      // 저충전 FAULT
      if(s->Bat80VSOCF <= C_PackSOCUN_Fault)
      {
          s->BAT80VFaulBuftReg.bit.PackVSOC_UN =1;
          s->BAT80VFaultReg.bit.PackVSOC_UN =1;
      }
      // 팩 과전압 FAULT
      if(s->Bat80VVoltageF >= C_PackVoltOV_Fault)
      {
          s->BAPackOVCount++;
          s->BAT80VFaulBuftReg.bit.PackVolt_OV =1;
          if(s->BAPackOVCount>=C_PackVoltOV_FaultDelay)
          {
              s->BAPackOVCount=C_PackVoltOV_FaultDelay+10;
              s->BAT80VFaultReg.bit.PackVolt_OV =1;
          }
      }
      else
      {
          s->BAPackOVCount=0;
          s->BAT80VFaulBuftReg.bit.PackVolt_OV =0;
      }
      // 팩 저전압 FAULT
      if(s->Bat80VVoltageF <= C_PackVoltUN_Fault)
      {
          s->BAPackUVCount++;
          s->BAT80VFaulBuftReg.bit.PackVolt_UN =1;
          if(s->BAPackUVCount>=C_PackVoltUN_FaultDelay)
          {
              s->BAPackUVCount = C_PackVoltUN_FaultDelay+10;
              s->BAT80VFaultReg.bit.PackVolt_UN =1;
          }
      }
      else
      {
          s->BAPackUVCount=0;
          s->BAT80VFaulBuftReg.bit.PackVolt_UN =0;
      }
      // 팩 과온도 FAULT
      if(s->Bat80VCellAgvTemperatureF >= C_PackTempOV_Fault)
      {
          s->BAT80VFaulBuftReg.bit.PackTemp_OV=1;
          s->BAT80VFaultReg.bit.PackTemp_OV=1;
      }
      // 팩 저온도 FAULT
      if(s->Bat80VCellAgvTemperatureF <= C_PackTempUN_Fault)  //26.05.30기준 -35도
      {
          s->BAT80VFaulBuftReg.bit.PackTemp_UN=1;
          s->BAT80VFaultReg.bit.PackTemp_UN=1;
      }
      // 셀 과전압 FAULT
      if(s->Bat80VCellMaxVoltageF >= C_CellVoltOV_Fault)
      {
          s->BACellOVCount++;
          s->BAT80VFaulBuftReg.bit.CellVolt_OV =1;
          if(s->BACellOVCount>=C_CellVoltOV_FaultDelay)
          {
              s->BACellOVCount=C_CellVoltOV_FaultDelay+10;
              s->BAT80VFaultReg.bit.CellVolt_OV =1;
          }
      }
      else
      {
          s->BACellOVCount=0;
          s->BAT80VFaulBuftReg.bit.CellVolt_OV =0;
      }
      // 셀 저전압 FAULT
      // TODO(버그 G, 검증 후 정리): Fault/Buf 비트 설정이 주석 처리되어 셀 과방전 보호 미동작이었음 -> 복구.
      if(s->Bat80VCellMinVoltageF <= C_CellVoltUN_Fault)
      {
          s->BACellUVCount++;
          s->BAT80VFaulBuftReg.bit.CellVolt_UN =1;
          if(s->BACellUVCount>=C_CellVoltUN_FaultDelay)
          {
              s->BACellUVCount = C_CellVoltUN_FaultDelay+10;
              s->BAT80VFaultReg.bit.CellVolt_UN =1;
          }
      }
      else
      {
          s->BACellUVCount=0;
          s->BAT80VFaulBuftReg.bit.CellVolt_UN =0;

      }
      // 셀 편차 FAULT
      if(s->Bat80VCellDivVoltageF >= C_CellVoltDIV_Fault)
      {
          s->BACellUBVCount++;
          s->BAT80VFaulBuftReg.bit.CellVolt_BL =1;
          if(s->BACellUBVCount>=C_CellVoltDIV_FaultDelay)
          {
              s->BACellUBVCount = C_CellVoltDIV_FaultDelay+10;
              s->BAT80VFaultReg.bit.CellVolt_BL =1;
          }
      }
      else
      {
          s->BACellUBVCount=0;
          s->BAT80VFaulBuftReg.bit.CellVolt_BL =0;
      }
      // 셀 과온 FAULT
      if(s->Bat80VCellMaxTemperatureF >= C_CellTempOV_Fault)
      {
          s->BAT80VFaulBuftReg.bit.CellTemp_OV =1;
          s->BAT80VFaultReg.bit.CellTemp_OV =1;
      }
      // 셀 저온 FAULT
      if(s->Bat80VCellMinTemperatureF <= C_CellTempUN_Fault)
      {
          s->BAT80VFaulBuftReg.bit.CellTemp_UN =1;
          s->BAT80VFaultReg.bit.CellTemp_UN =1;
      }
      // 셀 온도 편차 FAULT
      if(s->Bat80VCellDivTemperatureF >= C_CellTempDIV_Fault)
      {
          s->BACellUBTCount++;
          s->BAT80VFaulBuftReg.bit.CellTemp_BL =1;
          if(s->BACellUBTCount>C_CellTempDIV_FaultDelay)
          {
              s->BAT80VFaultReg.bit.CellTemp_BL =1;
              s->BACellUBTCount=C_CellTempDIV_FaultDelay+10;
          }

      }
      else
      {
          s->BACellUBTCount=0;
          s->BAT80VFaulBuftReg.bit.CellTemp_BL = 0;
      }
      if(s->RelayCheck >=C_RleyCount)
      {
          s->BAT80VFaulBuftReg.bit.PackRLY_ERR =1;
      }
      if(s->Bat80VISOResisF>C_IOSresistanceFault)
      {
          s->BAT80VFaulBuftReg.bit.PackRLY_ERR =1;
      }
}
int float32ToInt(float32 Vaule, Uint32 Num)
{
    Uint32 intVaule=0;
    intVaule = roundf(Vaule*10)/10;

    return (Uint32)intVaule;
}
void DigitalInput(SystemReg *sys)
{
    if((IDSW02==0)&&(IDSW01==0))
    {
        sys->BAT80VDigitalInputReg.bit.IDSW=0;
    }
    if((IDSW02==0)&&(IDSW01==1))
    {
        sys->BAT80VDigitalInputReg.bit.IDSW=1;
    }
    if((IDSW02==1)&&(IDSW01==0))
    {
        sys->BAT80VDigitalInputReg.bit.IDSW=2;
    }
    if((IDSW01==1)&&(IDSW01==1))
    {
        sys->BAT80VDigitalInputReg.bit.IDSW=3;
    }
    if(CANRX0INT==0)
    {
        sys->BAT80VDigitalInputReg.bit.CANRX0=1;
    }
    else
    {
        sys->BAT80VDigitalInputReg.bit.CANRX0=0;
    }
    if(CANRX1INT==0)
    {
        sys->BAT80VDigitalInputReg.bit.CANRX1=1;
    }
    else
    {
        sys->BAT80VDigitalInputReg.bit.CANRX1=0;
    }


}
void DigitalOutput(SystemReg *sys)
{
    if(sys->BAT80VDigitalOutPutReg.bit.LEDAlarmOUT==1)
    {
        sys->LEDFaultCount++;
        if(sys->LEDFaultCount>1200)
        {
            LEDFault_T;
            sys->LEDFaultCount=0;
        }
    }

    if(sys->BAT80VDigitalOutPutReg.bit.LEDFaultOUT==1)
    {
        sys->LEDFaultCount++;
        if(sys->LEDFaultCount>500)
        {
            LEDFault_T;
            sys->LEDFaultCount=0;
        }
    }

    if(sys->BAT80VDigitalOutPutReg.bit.LEDProtectOUT==1)
    {
        sys->LEDFaultCount++;
        if(sys->LEDFaultCount>200)
        {
            LEDFault_T;
            sys->LEDFaultCount=0;
        }
    }
    if((sys->BAT80VDigitalOutPutReg.bit.LEDAlarmOUT==0)&&(sys->BAT80VDigitalOutPutReg.bit.LEDFaultOUT==0)&&(sys->BAT80VDigitalOutPutReg.bit.LEDProtectOUT==0))
    {
        LEDFault_H;
    }
    if(sys->BAT80VDigitalOutPutReg.bit.LEDSysOUT==1)
    {
        sys->LEDSycCount++;
        if(sys->LEDSycCount>500)
        {
      //      LEDSysState_T;
            sys->LEDSycCount=0;
        }
    }
    else
    {
       // LEDSysState_H;
    }
    if(sys->BAT80VDigitalOutPutReg.bit.LEDCAnOUT==1)
    {
  -
        sys->LEDCanCount++;
        if(sys->LEDCanCount>500)
        {
            LEDCANState_T;
            sys->LEDCanCount=0;
        }
    }
    else
    {
        LEDCANState_H;
    }
}


/*
 *
 */

void TimerinitHandle(TimerReg *timer)
{
  timer->state = TIMER_STATE_IDLE;
  timer->TimeCount = 0;
  timer->Start=0;
  timer->Stop=0;
  timer->OutState=0;
  timer->Reset=0;
  timer->TimerVaule=0;
}
void ProtectRelayTimerHandle(TimerReg *timer)
{
  switch (timer->state)
  {
    case TIMER_STATE_IDLE:
      // 타이머 시작
      if (timer->Start==1)
      {
        timer->state = TIMER_STATE_RUNNING;
      }
      if (timer->Reset==1)
      {
         timer->state = TIMER_STATE_CLEAR;
      }
      break;
    case TIMER_STATE_RUNNING:
      // 타이머 만료 확인
      if (timer->TimeCount >= timer->TimerVaule)
      {
        timer->state = TIMER_STATE_EXPIRED;
      }
      // 타이머 증가
      timer->TimeCount++;
      break;
    case TIMER_STATE_EXPIRED:
         // 타이머 만료 처리
         timer->OutState = 1;
         // 타이머 재시작
         if (timer->Reset==1)
         {
            timer->state = TIMER_STATE_CLEAR;
         }
    break;
    case TIMER_STATE_CLEAR:
         timer->state = TIMER_STATE_IDLE;
         timer->TimeCount = 0;
         timer->Start=0;
         timer->Stop=0;
         timer->OutState=0;
         timer->TimerVaule=0;
         timer->Reset=0;
    break;

  }
}
