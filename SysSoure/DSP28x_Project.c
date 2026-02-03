
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
extern void Cal12VSysVoltageHandle(SystemReg *s);
extern void Cal12VSysTemperatureHandle(SystemReg *s);
extern void Cal12VSysCurrentHandle(SystemReg *s);
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
    /*
    //CAN Tx Request
    ECanaShadow.CANTRS.all=0;
    ECanaShadow.CANTRS.bit.TRS31= 1;
    ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;
    do
    {
       ECanaShadow.CANTA.all = ECanaRegs.CANTA.all;
     //  CANWatchDog++;
     //  if(CANWatchDog>20)
     //  {
     //      ECanaShadow.CANTA.bit.TA31=0;
     //  }
    }
    while(!ECanaShadow.CANTA.bit.TA31);

    //Tx Flag Clear

    ECanaShadow.CANTA.all = 0;
    ECanaShadow.CANTA.bit.TA31=1;                   // Clear TA5
    ECanaRegs.CANTA.all = ECanaShadow.CANTA.all;
   //
    */

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


    s->Bat12VoltageMaxNum=0;
    s->Bat12VoltageMinNum=0;
    s->Bat12VVoltageF=0;
    s->Bat12VCurrentF=0;
    s->Bat12VCurrentAsbF=0;
    s->Bat12VCellMaxVoltageF=0;
    s->Bat12VCellMinVoltageF=0;
    s->Bat12VCellAgvVoltageF=0;
    s->Bat12VCellDivVoltageF=0;
    s->Bat12VlMaxTemperatureF=0;
    s->Bat12VCellMaxTemperatureF=10;
    s->Bat12VCellMinTemperatureF=10;
    s->Bat12VCellDivTemperatureF=10;
    s->Bat12VCellAgvTemperatureF=0;
    s->Bat12VCHAPWRContintyF=3;
    s->Bat12VDisCHAPWRContintyF=3;
    s->Bat12VCHAPWRPeakF=3;
    s->Bat12VDisCHAPWRPeakF=3;
    s->Bat12VSOCF=0;
    s->Bat12VSOHF=0;
    s->Bat12VAhF=0;

    s->BAPackOCCount=0;
    s->BAPackOVCount=0;
    s->BAPackUVCount=0;
    s->BACellOVCount=0;
    s->BACellUVCount=0;
    s->BACellUBVCount=0;
    s->BACellUBTCount=0;


    memset(&s->Bat80VAlarmCont[0],0.0,32);
    memset(&s->Bat80VCellVoltageF[0],0.0,Sys80VCellVoltCount);
    memset(&s->Bat12VCellVoltageF[0],0.0,Sys12VCellVoltCount);
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
    CellSize = Sys80VCellVoltCount;//24
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
     if(Hyst_On(s->Bat80VCurrentAsbF,500.0f))
     {
          if(s->Bat80VAlarmCont[0]< 100){++s->Bat80VAlarmCont[0];}
          if(s->Bat80VAlarmCont[0]>=100)
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
          if(Hyst_Off(s->Bat80VCurrentAsbF,456.0f))
          {
              s->Bat80VAlarmCont[0]=0;
              s->BAT80VAlarmReg.bit.PackOC=0;
          }
      }
      // 팩 과충전 Alarm,유지시간카운터배열값:1,유지시간;100msec
      if(Hyst_On(s->Bat80VSOCF,100.1f))
      {
          if(s->Bat80VAlarmCont[1]< 100){++s->Bat80VAlarmCont[1];}
          if(s->Bat80VAlarmCont[1]>=100)
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
          if(Hyst_Off(s->Bat80VSOCF,97.0f))
          {
              s->Bat80VAlarmCont[1]=0;
              s->BAT80VAlarmReg.bit.PackVSOC_OV=0;
          }
      }
      // 팩 저충전,유지시간카운터배열값:2,유지시간;100msec
      if(Hyst_Off(s->Bat80VSOCF,5.0f))
      {
          if(s->Bat80VAlarmCont[2]< 100){++s->Bat80VAlarmCont[2];}
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
          if(Hyst_On(s->Bat80VSOCF,5.25f))
          {
              s->Bat80VAlarmCont[2]=0;
              s->BAT80VAlarmReg.bit.PackVSOC_UN=0;
          }
      }
      // 팩 과전압 Alarm,유지시간카운터배열값:3,유지시간;100msec
      if(Hyst_On(s->Bat80VVoltageF,102.0f))
      {
          if(s->Bat80VAlarmCont[3]< 100){++s->Bat80VAlarmCont[3];}
          if(s->Bat80VAlarmCont[3]>=100)
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
          if(Hyst_Off(s->Bat80VVoltageF,98.9f))
          {
              s->Bat80VAlarmCont[3]=0;
              s->BAT80VAlarmReg.bit.PackVolt_OV=0;
          }
      }
      // 팩 저전압 Alarm,유지시간카운터배열값:4,유지시간;100msec
      if(Hyst_Off(s->Bat80VVoltageF,68.4f))
      {
          if(s->Bat80VAlarmCont[4]< 100){++s->Bat80VAlarmCont[4];}
          if(s->Bat80VAlarmCont[4]>=100)
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
          if(Hyst_On(s->Bat80VVoltageF,71.4f))
          {
              s->Bat80VAlarmCont[4]=0;
              s->BAT80VAlarmReg.bit.PackVolt_UN=0;
          }
      }
      // 팩 저전압 Alarm,유지시간카운터배열값:5,유지시간;100msec
      if(Hyst_On(s->Bat80VCellAgvTemperatureF,55.0f))
      {
          if(s->Bat80VAlarmCont[5]< 100){++s->Bat80VAlarmCont[5];}
          if(s->Bat80VAlarmCont[5]>=100)
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
          if(Hyst_Off(s->Bat80VCellAgvTemperatureF,52.3f))
          {
              s->Bat80VAlarmCont[5]=0;
              s->BAT80VAlarmReg.bit.PackTemp_OV=0;
          }
      }
      // 팩 저온 Alarm,유지시간카운터배열값:6,유지시간;100msec
      if(s->BAT80VStateReg.bit.sysDisChaMode==1)
      {
          if(Hyst_Off(s->Bat80VCellAgvTemperatureF,-15.0f))
          {
              if(s->Bat80VAlarmCont[6]< 100){++s->Bat80VAlarmCont[6];}
              if(s->Bat80VAlarmCont[6]>=100)
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
              if(Hyst_On(s->Bat80VCellAgvTemperatureF,0.0f))
              {
                  s->Bat80VAlarmCont[6]=0;
                  s->BAT80VAlarmReg.bit.PackTemp_UN=0;
              }
          }
      }
      else
      {
          if(s->BAT80VStateReg.bit.sysDisChaMode==1)
          {
              if(Hyst_Off(s->Bat80VCellAgvTemperatureF,-15.0f))
              {
                  if(s->Bat80VAlarmCont[6]< 100){++s->Bat80VAlarmCont[6];}
                  if(s->Bat80VAlarmCont[6]>=100)
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
                  if(Hyst_On(s->Bat80VCellAgvTemperatureF,0.0f))
                  {
                      s->Bat80VAlarmCont[6]=0;
                      s->BAT80VAlarmReg.bit.PackTemp_UN=0;
                  }
              }
          }
      }
      if(s->BAT80VStateReg.bit.sysDisChaMode==1)
      {
       /* s->BBat80VDisCHAPWRContintyF
          if(Hyst_Off(s->Bat80VUnbalPwr,-15.0f))
          {
              if(s->Bat80VAlarmCont[7]< 100){++s->Bat80VAlarmCont[7];}
              if(s->Bat80VAlarmCont[7]>=100)
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
          /* s->BBat80VDisCHAPWRContintyF
          if(Hyst_Off(s->Bat80VUnbalPwr,-15.0f))
          {
              if(s->Bat80VAlarmCont[7]< 100){++s->Bat80VAlarmCont[7];}
              if(s->Bat80VAlarmCont[7]>=100)
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

      // 셀 과전압 Alarm,유지시간카운터배열값:8,유지시간:00msec
      if(Hyst_On(s->Bat80VCellMaxVoltageF,4.25f))
      {
          if(s->Bat80VAlarmCont[8]< 100){++s->Bat80VAlarmCont[8];}
          if(s->Bat80VAlarmCont[8]>=100)
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
          if(Hyst_Off(s->Bat80VCellMaxVoltageF,4.23f))
          {
              s->Bat80VAlarmCont[8]=0;
              s->BAT80VAlarmReg.bit.CellVolt_OV=0;
          }
      }
      // 셀 저전압 Alarm,유지시간카운터배열값:9,유지시간:100msec
      if(Hyst_Off(s->Bat80VCellMinVoltageF,2.80f))
      {
          if(s->Bat80VAlarmCont[9]< 100){++s->Bat80VAlarmCont[8];}
          if(s->Bat80VAlarmCont[9]>=100)
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
          if(Hyst_On(s->Bat80VCellMinVoltageF,2.94f))
          {
              s->Bat80VAlarmCont[9]=0;
              s->BAT80VAlarmReg.bit.CellVolt_UN=0;
          }
      }
      // 셀 전압 편차 Alarm,유지시간카운터배열값:10,유지시간:100msec
      if(Hyst_On(s->Bat80VCellDivVoltageF,0.45f))
      {
          if(s->Bat80VAlarmCont[10]< 100){++s->Bat80VAlarmCont[10];}
          if(s->Bat80VAlarmCont[10]>=100)
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
          if(Hyst_Off(s->Bat80VCellDivVoltageF,0.32f))
          {
              s->Bat80VAlarmCont[10]=0;
              s->BAT80VAlarmReg.bit.CellVolt_BL=0;
          }
      }
      // 셀 과온 Alarm,유지시간카운터배열값:11,유지시간:100msec
      if(Hyst_On(s->Bat80VCellMaxTemperatureF,55.0f))
      {
          if(s->Bat80VAlarmCont[11]< 100){++s->Bat80VAlarmCont[11];}
          if(s->Bat80VAlarmCont[11]>=100)
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
          if(Hyst_Off(s->Bat80VCellMaxTemperatureF,52.3f))
          {
              s->Bat80VAlarmCont[11]=0;
              s->BAT80VAlarmReg.bit.CellTemp_OV=0;
          }
      }
      // 셀 저온 Alarm,유지시간카운터배열값:12,유지시간:100msec
      if(Hyst_Off(s->Bat80VCellMinVoltageF,-15.0f))
      {
          if(s->Bat80VAlarmCont[12]< 100){++s->Bat80VAlarmCont[12];}
          if(s->Bat80VAlarmCont[12]>=100)
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
          if(Hyst_On(s->Bat80VCellMinVoltageF,0.0f))
          {
              s->Bat80VAlarmCont[12]=0;
              s->BAT80VAlarmReg.bit.CellTemp_UN=0;
          }
      }
      // 셀 온도 편차 Alarm,유지시간카운터배열값:13,유지시간:100msec
      if(Hyst_On(s->Bat80VCellDivTemperatureF,8.0f))
      {
          if(s->Bat80VAlarmCont[13]< 100){++s->Bat80VAlarmCont[13];}
          if(s->Bat80VAlarmCont[13]>=100)
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
          if(Hyst_Off(s->Bat80VCellDivTemperatureF,4.0f))
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
          if(s->BAPackOCCount >= C_Bat80VOVPackCurrentFaultDelay)
          {
              s-> BAT80VFaultReg.bit.PackVCT_OV=1;
              s->BAPackOCCount=C_Bat80VOVPackCurrentFaultDelay+10;
          }
      }
      else
      {
          s->BAPackOCCount=0;
         // s-> BAT80VFaultReg.bit.PackVCT_OV=0;
          s-> BAT80VFaulBuftReg.bit.PackVCT_OV=0;
      }
      */


      if(s->Bat80VCurrentAsbF >= C_Bat80VOVPackCurrentFault)
      {
          s-> BAT80VFaulBuftReg.bit.PackVCT_OV=1;
          s-> BAT80VFaultReg.bit.PackVCT_OV=1;
          s-> Bat80VFaultCurrentF=s->Bat80VCurrentF;
      }
      if(s->Bat80VCurrentAsbF >= C_Bat80VOVPackOCTimer)
      {
          s->BAPackOCCount++;
          if(s->BAPackOCCount>2000)
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
      if(s->Bat80VSOCF >=C_Bat80VOVPackSOCFault)
      {

          s->BAT80VFaulBuftReg.bit.PackVSOC_OV =1;
          s->BAT80VFaultReg.bit.PackVSOC_OV=1;

      }
      // 저충전 FAULT
      if(s->Bat80VSOCF <= C_Bat80VUDPackSOCFault)
      {
          s->BAT80VFaulBuftReg.bit.PackVSOC_UN =1;
          s->BAT80VFaultReg.bit.PackVSOC_UN =1;
      }
      // 팩 과전압 FAULT
      if(s->Bat80VVoltageF >= C_Bat80VOVPackVoltageFault)
      {
          s->BAPackOVCount++;
          s->BAT80VFaulBuftReg.bit.PackVolt_OV =1;
          if(s->BAPackOVCount>=C_Bat80VOVPackVoltageFaultDelay)
          {
              s->BAPackOVCount=C_Bat80VOVPackVoltageFaultDelay+10;
              s->BAT80VFaultReg.bit.PackVolt_OV =1;
          }
      }
      else
      {
          s->BAPackOVCount=0;
          s->BAT80VFaulBuftReg.bit.PackVolt_OV =0;
      }
      // 팩 저전압 FAULT
      if(s->Bat80VVoltageF <= C_Bat80VUDPackVoltageFault)
      {
          s->BAPackUVCount++;
          s->BAT80VFaulBuftReg.bit.PackVolt_UN =1;
          if(s->BAPackUVCount>=C_Bat80VUDPackVoltageFaultDelay)
          {
              s->BAPackUVCount = C_Bat80VUDPackVoltageFaultDelay+10;
              s->BAT80VFaultReg.bit.PackVolt_UN =1;
          }
      }
      else
      {
          s->BAPackUVCount=0;
          s->BAT80VFaulBuftReg.bit.PackVolt_UN =0;
      }
      // 팩 과온도 FAULT
      if(s->Bat80VCellAgvTemperatureF >= C_Bat80VOVPackTemperatureFault)
      {
          s->BAT80VFaulBuftReg.bit.PackTemp_OV=1;
          s->BAT80VFaultReg.bit.PackTemp_OV=1;
      }
      // 팩 저온도 FAULT
      if(s->Bat80VCellAgvTemperatureF <= C_Bat80VUNPackTemperatureFault)  //-30.0 deg C_Bat80VUNPackTemperatureFault
      {
          s->BAT80VFaulBuftReg.bit.PackTemp_UN=1;
          s->BAT80VFaultReg.bit.PackTemp_UN=1;
      }
      // 셀 과전압 FAULT
      if(s->Bat80VCellMaxVoltageF >= C_Bat80VOVCellVoltageFault)
      {
          s->BACellOVCount++;
          s->BAT80VFaulBuftReg.bit.CellVolt_OV =1;
          if(s->BACellOVCount>=C_Bat80VOVCellVoltageFaultDelay)
          {
              s->BACellOVCount=C_Bat80VOVCellVoltageFaultDelay+10;
              s->BAT80VFaultReg.bit.CellVolt_OV =1;
          }
      }
      else
      {
          s->BACellOVCount=0;
          s->BAT80VFaulBuftReg.bit.CellVolt_OV =0;
      }
      // 셀 저전압 FAULT
      if(s->Bat80VCellMinVoltageF <= C_Bat80VUDCellVoltageFault)
      {
          s->BACellUVCount++;
          //s->BAT80VFaulBuftReg.bit.CellVolt_UN =1;
          if(s->BACellUVCount>=C_Bat80VUDCellVoltageFaultDelay)
          {
              s->BACellUVCount = C_Bat80VUDCellVoltageFaultDelay+10;
            //  s->BAT80VFaultReg.bit.CellVolt_UN =1;
          }
      }
      else
      {
          s->BACellUVCount=0;
          s->BAT80VFaulBuftReg.bit.CellVolt_UN =0;

      }
      // 셀 편차 FAULT
      if(s->Bat80VCellDivVoltageF >= C_Bat80VDIVCellVoltageFault)
      {
          s->BACellUBVCount++;
          s->BAT80VFaulBuftReg.bit.CellVolt_BL =1;
          if(s->BACellUBVCount>=C_Bat80VDIVCellVoltageFaultDelay)
          {
              s->BACellUBVCount = C_Bat80VDIVCellVoltageFaultDelay+10;
              s->BAT80VFaultReg.bit.CellVolt_BL =1;
          }
      }
      else
      {
          s->BACellUBVCount=0;
          s->BAT80VFaulBuftReg.bit.CellVolt_BL =0;
      }
      // 셀 과온 FAULT
      if(s->Bat80VCellMaxTemperatureF >= C_Bat80VOVCellTemperatureFault)
      {
          s->BAT80VFaulBuftReg.bit.CellTemp_OV =1;
          s->BAT80VFaultReg.bit.CellTemp_OV =1;
      }
      // 셀 저온 FAULT
      if(s->Bat80VCellMinVoltageF <= C_Bat80VUDCellTemperatureFault)
      {
          s->BAT80VFaulBuftReg.bit.CellTemp_UN =1;
          s->BAT80VFaultReg.bit.CellTemp_UN =1;
      }
      // 셀 온도 편차 FAULT
      if(s->Bat80VCellDivVoltageF >= C_Bat80VDIVCellTemperatureFault)
      {
          s->BACellUBTCount++;
          s->BAT80VFaulBuftReg.bit.CellTemp_BL =1;
          if(s->BACellUBTCount>C_Bat80VDIVCellTemperatureFaultDelay)
          {
              s->BAT80VFaultReg.bit.CellTemp_BL =1;
              s->BACellUBTCount=C_Bat80VDIVCellTemperatureFaultDelay+10;
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
void Cal12VSysVoltageHandle(SystemReg *s)
{

    Uint16  CellCount=0;
    Uint16  CellSize=0;
    float32 SysCellMaxVoltageF=0;
    float32 SysCellMinVoltageF=0;
    float32 SysVoltageBufF=0;
    SysCellMaxVoltageF =s->Bat12VCellVoltageF[0];
    SysCellMinVoltageF =s->Bat12VCellVoltageF[0];
    CellSize = Sys12VCellVoltCount;//4
    for(CellCount=0;CellCount<CellSize;CellCount++)
    {
         if (SysCellMaxVoltageF <= s->Bat12VCellVoltageF[CellCount])
         {
             SysCellMaxVoltageF    =  s->Bat12VCellVoltageF[CellCount];
             s->Bat12VoltageMaxNum=CellCount;
         }
         if (SysCellMinVoltageF >= s->Bat12VCellVoltageF[CellCount])
         {
             SysCellMinVoltageF    =  s->Bat12VCellVoltageF[CellCount];
             s->Bat12VoltageMinNum=CellCount;
         }
    }
    s->Bat12VCellMaxVoltageF    = SysCellMaxVoltageF;
    s->Bat12VCellMinVoltageF    = SysCellMinVoltageF;
    s->Bat12VCellDivVoltageF    = s->Bat12VCellMaxVoltageF-s->Bat12VCellMinVoltageF;
    CellSize = Sys12VCellVoltCount;
    for(CellCount=0;CellCount<CellSize;CellCount++)
    {
        SysVoltageBufF = SysVoltageBufF+ s->Bat12VCellVoltageF[CellCount];
    }
    s->Bat12VVoltageF           = SysVoltageBufF;
    s->Bat12VCellAgvVoltageF   =  s->Bat12VVoltageF/4.0;
}
void Cal12VSysTemperatureHandle(SystemReg *s)
{

    Uint16  CellCount=0;
    Uint16  CellSize=0;
    float32 SysCellMaxTemperatureF=0;
    float32 SysCellMinTemperatureF=0;
    float32 SysTemperatureBufF=0;
    SysCellMaxTemperatureF =s->Bat12VCellTemperatureF [0];
    SysCellMinTemperatureF =s->Bat12VCellTemperatureF [0];
    CellSize = 4;//24

    for(CellCount=0;CellCount<CellSize;CellCount++)
    {
         if (SysCellMaxTemperatureF <= s->Bat12VCellTemperatureF[CellCount])
         {
             SysCellMaxTemperatureF    =  s->Bat12VCellTemperatureF[CellCount];
        //     s->Bat80TemperatureMaxNum=CellCount;
         }
         if (SysCellMinTemperatureF >= s->Bat12VCellTemperatureF[CellCount])
         {
             SysCellMinTemperatureF    =  s->Bat12VCellTemperatureF[CellCount];
   //          s->Bat80TemperatureMinNum=CellCount;
         }
    }
    s->Bat12VCellMaxTemperatureF    = SysCellMaxTemperatureF;
    s->Bat12VCellMinTemperatureF    = SysCellMinTemperatureF;
    s->Bat12VCellDivTemperatureF    = s->Bat12VCellMaxTemperatureF-s->Bat12VCellMinTemperatureF;
    CellSize = 4;
    for(CellCount=0;CellCount<CellSize;CellCount++)
    {
        SysTemperatureBufF = SysTemperatureBufF+ s->Bat12VCellTemperatureF[CellCount];
    }
    s->Bat12VCellAgvTemperatureF   = (float32)SysTemperatureBufF/CellSize;
}
void Cal12VSysCurrentHandle(SystemReg *s)
{
    long  CurrentCT  = 0;
    float32 Currentbuf = 0;
    CurrentCT  = s->Bat12VCurrentData.all;
    CurrentCT  =  CurrentCT - 0x80000000;

    Currentbuf         = ((float)CurrentCT)/1000;          // (mA to A) CAB500 resolution 1mA
    s->Bat12VCurrentF  = C_CTDirection * Currentbuf;    // Decide Current sensor's direction
    if(s->Bat12VCurrentF>=500.0)
    {
        s->Bat12VCurrentF=500.0;
    }
    if(s->Bat12VCurrentF<=-500.0)
    {
        s->Bat12VCurrentF=-500.0;
    }
    if(s->Bat12VCurrentF < 0)
    {
        s->Bat12VCurrentAsbF =-1.0 * s->Bat12VCurrentF;
    }
    else
    {
        s->Bat12VCurrentAsbF =s->Bat12VCurrentF;
    }

}
void Cal12VSysAlarmtCheck(SystemReg *s)
{

      // 과전류 FAULT
      if(s->Bat12VCurrentAsbF >= C_Bat12VOVPackCurrentAlarm)
      {
          s-> BAT12VAlarmReg.bit.PackOC=1;
      }
      else
      {
          s-> BAT12VAlarmReg.bit.PackOC=0;
      }
      // 팩 과충전 Alarm
      if(s->Bat12VSOCF >=C_Bat12VOVPkACKSOCAlarm)
      {
          s->BAT12VAlarmReg.bit.PackVSOC_OV =1;
      }
      else
      {
          s->BAT12VAlarmReg.bit.PackVSOC_OV =0;
      }
      // 팩 저충전 Alarm
      if(s->Bat12VSOCF <= C_Bat12VUDPkACKSOCAlarm)
      {
          s->BAT12VAlarmReg.bit.PackVSOC_UN =1;
      }
      else
      {
          s->BAT12VAlarmReg.bit.PackVSOC_UN =0;
      }
      // 팩 과전압 Alarm
      if(s->Bat12VVoltageF >= C_Bat12VOVPackVoltageAlarm)
      {
          s->BAT12VAlarmReg.bit.PackVolt_OV =1;
      }
      else
      {
          s->BAT12VAlarmReg.bit.PackVolt_OV =0;
      }
      // 팩 저전압 Alarm
      if(s->Bat12VVoltageF <= C_Bat12VUDPackVoltageAlarm)
      {
          s->BAT12VAlarmReg.bit.PackVolt_UN =1;
      }
      else
      {
          s->BAT12VAlarmReg.bit.PackVolt_UN =0;
      }
      // 팩 과온 Alarm
      if(s->Bat12VCellAgvTemperatureF >= C_Bat12VOVPackTemperatureAlarm)
      {
          s->BAT12VAlarmReg.bit.PackTemp_OV=1;
      }
      else
      {
          s->BAT12VAlarmReg.bit.PackTemp_OV=0;
      }
      // 팩 저온 Alarm
      if(s->Bat12VCellAgvTemperatureF <= C_Bat12VUNPackTemperatureAlarm)
      {
          s->BAT12VAlarmReg.bit.PackTemp_UN=1;
      }
      else
      {
          s->BAT12VAlarmReg.bit.PackTemp_UN=0;
      }
      // 셀 과전압 Alarm
      if(s->Bat12VCellMaxVoltageF >= C_Bat12VOVCellVoltageAlarm)
      {
          s->BAT12VAlarmReg.bit.CellVolt_OV =1;
      }
      else
      {
          s->BAT12VAlarmReg.bit.CellVolt_OV =0;
      }
      // 셀 저전압 Alarm
      if(s->Bat12VCellMinVoltageF <= C_Bat12VUDCellVoltageAlarm)
      {
          s->BAT12VAlarmReg.bit.CellVolt_UN =1;
      }
      else
      {
          s->BAT12VAlarmReg.bit.CellVolt_UN =0;
      }
      // 셀 전압 편차 Alarm
      if(s->Bat12VCellDivVoltageF >= C_Bat12VDIVCellVoltageAlarm)
      {
          s->BAT12VAlarmReg.bit.CellVolt_BL =1;
      }
      else
      {
          s->BAT12VAlarmReg.bit.CellVolt_BL =0;
      }
      // 셀 과온 Alarm
      if(s->Bat12VCellMaxTemperatureF >= C_Bat12VOVCellTemperatureAlarm)
      {
          s->BAT12VAlarmReg.bit.CellTemp_OV =1;
      }
      else
      {
          s->BAT12VAlarmReg.bit.CellTemp_OV =0;
      }
      // 셀 저온 Alarm
      if(s->Bat12VCellMinVoltageF <= C_Bat12VUDCellTemperatureAlarm)
      {
          s->BAT12VAlarmReg.bit.CellTemp_UN =1;
      }
      else
      {
          s->BAT12VAlarmReg.bit.CellTemp_UN =0;
      }
      // 셀 온도 편차 Alarm
      if(s->Bat12VCellDivVoltageF >= C_Bat12VDIVCellTemperatureAlarm)
      {
          s->BAT12VAlarmReg.bit.CellTemp_BL =1;
      }
      else
      {
          s->BAT12VAlarmReg.bit.CellTemp_BL =0;
      }
}
void Cal12VSysFaultCheck(SystemReg *s)
{
      // 과전류 FAULT
      if(s->Bat12VCurrentAsbF >= C_Bat12VOVPackCurrentFault)
      {
          s-> BAT12VFaulBuftReg.bit.PackVCT_OV=1;
      }
      // 과충전 FAULT
      if(s->Bat12VSOCF >=C_Bat12VOVPackSOCFault)
      {
          s->BAT12VFaulBuftReg.bit.PackVSOC_OV =1;
      }
      // 저충전 FAULT
      if(s->Bat12VSOCF <= C_Bat12VUDPackSOCFault)
      {
          s->BAT12VFaulBuftReg.bit.PackVSOC_UN =1;
      }
      // 팩 과전압 FAULT
      if(s->Bat12VVoltageF >= C_Bat12VOVPackVoltageFault)
      {
          s->BAT12VFaulBuftReg.bit.PackVolt_OV =1;
      }
      // 팩 저전압 FAULT
      if(s->Bat12VVoltageF <= C_Bat12VUDPackVoltageFault)
      {
          s->BAT12VFaulBuftReg.bit.PackVolt_UN =1;
      }
      // 팩 과온도 FAULT
      if(s->Bat12VCellAgvTemperatureF >= C_Bat12VOVPackTemperatureFault)
      {
          s->BAT12VFaulBuftReg.bit.PackTemp_OV=1;
      }
      // 팩 저온도 FAULT
      if(s->Bat12VCellAgvTemperatureF <= C_Bat12VUNPackTemperatureFault)
      {
          s->BAT12VFaulBuftReg.bit.PackTemp_UN=1;
      }
      // 셀 과전압 FAULT
      if(s->Bat12VCellMaxVoltageF >= C_Bat12VOVCellVoltageFault)
      {
          s->BAT12VFaulBuftReg.bit.CellVolt_OV =1;
      }
      if(s->Bat12VCellMinVoltageF <= C_Bat12VUDCellVoltageFault)
      {
          s->BAT12VFaulBuftReg.bit.CellVolt_UN =1;
      }
      // 셀 편차 FAULT
      if(s->Bat12VCellDivVoltageF >= C_Bat12VDIVCellVoltageFault)
      {
          s->BAT12VFaulBuftReg.bit.CellVolt_BL =1;
      }
      // 셀 과온 FAULT
      if(s->Bat12VCellMaxTemperatureF >= C_Bat12VOVCellTemperatureFault)
      {
          s->BAT12VFaulBuftReg.bit.CellTemp_OV =1;
      }
      // 셀 저온 FAULT
      if(s->Bat12VCellMinVoltageF <= C_Bat12VUDCellTemperatureFault)
      {
          s->BAT12VFaulBuftReg.bit.CellTemp_UN =1;
      }
      // 셀 온도 편차 FAULT
      if(s->Bat12VCellDivVoltageF >= C_Bat12VDIVCellTemperatureFault)
      {
          s->BAT12VFaulBuftReg.bit.CellTemp_BL =1;
      }
      if(s->BAT12VFaulBuftReg.all == 0)
       {
           s->Bat12VFaultStatecount =0;
           s->BAT12VStateReg.bit.SysFault=0;
           s->BAT12VFaulBuftReg.all=0;
           s->BAT12VFaultReg.all=0;
       }
       if(s->BAT12VFaulBuftReg.all != 0)
       {
           if( s-> BAT12VFaulBuftReg.bit.PackVCT_OV==1)
           {
               s->Bat12VFaultStatecount=C_Bat12VFaultDelayCount+1;
           }
           s->Bat12VFaultStatecount++;
       }
       if(s->Bat12VFaultStatecount >= C_Bat12VFaultDelayCount)   // 500msec 동안  Fault 검출시 Fault 상태 ON
       {
           s->BAT12VStateReg.bit.SysFault=1;
           s->Bat12VFaultStatecount=C_Bat12VFaultDelayCount+1;
       }
       else
       {
           s->BAT12VStateReg.bit.SysFault=0;
           s->BAT12VFaulBuftReg.all=0;
       }
       s->BAT12VFaultReg.all=s->BAT12VFaulBuftReg.all;
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
  /*  if(PRlyState==0)
    {
        sys->BAT80VDigitalInputReg.bit.PAUX=1;
    }
    else
    {
        sys->BAT80VDigitalInputReg.bit.PAUX=0;
    }
    if(NRlyState==0)
    {
        sys->BAT80VDigitalInputReg.bit.NAUX=1;
    }
    else
    {
        sys->BAT80VDigitalInputReg.bit.NAUX=0;
    }
    if(CHARlyState==0)
    {
        sys->BAT80VDigitalInputReg.bit.CHAAUX=1;
    }
    else
    {
        sys->BAT80VDigitalInputReg.bit.CHAAUX=0;
    }*/

}
void DigitalOutput(SystemReg *sys)
{
    /*
    if(sys->BAT80VDigitalOutPutReg.bit.RRlyOUT==1)
    {
        PRlyOn;
    }
    else
    {
        PRlyOff;
    }
    if(sys->BAT80VDigitalOutPutReg.bit.NRlyOUT==1)
    {
        NRlyOn;

    }
    else
    {
        NRlyOff;
    }
    if(sys->BAT80VDigitalOutPutReg.bit.CHARlyOUT==1)
    {
        CHARlyOn;
    }
    else
    {
        CHARlyOff;
    }
    */
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


//extern MODReg MODRegs;
/*
extern void InitECan(void);
extern void SciaTxchar(char Txchar);

extern void SPI_Write(unsigned int WRData);
extern unsigned int SPI_READ(void);
extern void CANATX(unsigned int ID, unsigned char Length, unsigned int Data0, unsigned int Data1,unsigned int Data2,unsigned int Data3 );
 
extern void ModuleCellMIN(SlaveReg *S);
extern void ModuleCellMAX(SlaveReg *S);
//extern void BatteryTeperaureCal(BatteryReg *bat,int Count);




extern void TEST_LTC6804(void);
extern int LTC6804_Init(void);

extern void SlaveBMSSPIEnable_low(void);
extern void SlaveBMSSPIEnable_high(void);
extern void SlaveBms_WakeUp(void);
extern int SlaveBMSCellReadCommand(SlaveReg *s);
extern void SlaveBmsBalance(SlaveReg *s);


extern float32 AbsVaule(float32 input);
extern void CellVoltagetoFloat(SlaveReg *s, ModuleReg *m);

extern void SystemAlarm(PackReg *bat,SystemReg *sys);
extern void SystemRegsInit(SystemReg *s);
extern void systemParmRead(SystemReg *sys,PackReg *bat);
extern void sysDigitalInput(SystemReg *sys);
extern void sysDigitalOutput(SystemReg *sys);
extern void SysCurrentOffset(SystemReg *sys);
extern void SystemforceBalance(ModuleReg *m);


extern void DigitalInput(SystemReg *sys);
extern void DigitalOutput(SystemReg *sys);
extern void SlaveRegsInit(SlaveReg *s);
extern void ModuleRegsInit(ModuleReg *m);
extern void PackRegsInit(PackReg *b);
extern void SystemInit(SystemReg *s);
extern void CellTempCalFloat(SlaveReg *s, ModuleReg *m);
extern void CellVOLTFloatTOInt(ModuleReg *s, CANTXReg *t);
extern void CellTempFloatToInt(ModuleReg *s, CANTXReg *t);

void CellTempFloatToInt(ModuleReg *s, CANTXReg *t)
{
	t->BatteryTempCell[t->CellNumer]  =(int)(s->CellTempF[0]*10.0);
	t->BatteryTempCell[t->CellNumer+1]=(int)(s->CellTempF[1]*10.0);
	t->BatteryTempCell[t->CellNumer+2]=(int)(s->CellTempF[2]*10.0);
	t->BatteryTempCell[t->CellNumer+3]=(int)(s->CellTempF[3]*10.0);
}
void CellVOLTFloatTOInt(ModuleReg *s, CANTXReg *t)
{
	t->BatteryVoltageCell[t->CellNumer]=(unsigned int)(s->CellVoltageF[0]*1000.0);
	t->BatteryVoltageCell[t->CellNumer+1]=(unsigned int)(s->CellVoltageF[1]*1000.0);
	t->BatteryVoltageCell[t->CellNumer+2]=(unsigned int)(s->CellVoltageF[2]*1000.0);
	t->BatteryVoltageCell[t->CellNumer+3]=(unsigned int)(s->CellVoltageF[3]*1000.0);
	t->BatteryVoltageCell[t->CellNumer+4]=(unsigned int)(s->CellVoltageF[4]*1000.0);
	t->BatteryVoltageCell[t->CellNumer+5]=(unsigned int)(s->CellVoltageF[5]*1000.0);
	t->BatteryVoltageCell[t->CellNumer+6]=(unsigned int)(s->CellVoltageF[6]*1000.0);
	t->BatteryVoltageCell[t->CellNumer+7]=(unsigned int)(s->CellVoltageF[7]*1000.0);
	t->BatteryVoltageCell[t->CellNumer+8]=(unsigned int)(s->CellVoltageF[8]*1000.0);
	t->BatteryVoltageCell[t->CellNumer+9]=(unsigned int)(s->CellVoltageF[9]*1000.0);
	t->BatteryVoltageCell[t->CellNumer+10]=(unsigned int)(s->CellVoltageF[10]*1000.0);
	t->BatteryVoltageCell[t->CellNumer+11]=(unsigned int)(s->CellVoltageF[11]*1000.0);


}
void TEST_LTC6804(void)
{
	LTC680x_CS;
	SPI_Write(0X55);
	SPI_Write(0X55);
	LTC680X_DS;
}
void Alarm_LED_handle(SystemReg *sys)
{

}
void Emergency_LED_handle(SystemReg *sys)
{
 
}
void SlaveRegsInit(SlaveReg *s)
{
	
}
void ModuleRegsInit(ModuleReg *m)
{

}
void PackRegsInit(PackReg *b)
{
//	b->PackVoltage=0.0;
//	b->PackTemp=0.0;
//	b->PackSOC=0.0; 

}
void SystemRegsInit(SystemReg *s)
{
	LTC680X_DS;
	s->initCount=0;
	s->DigitalOutPutReg.all=0;
	s->DigitalInputReg.all=0;
	s->SystemStateARegs.all=0x0000;
	s->Timer50msec=0;
	s->Timer250msec=0;
	s->Timer500msec=0;
	s->Timer1000msec=0;
	s->Timer1500msec=0;
	s->Timer2000msec=0;
	s->Timer2500msec=0;
 
}
void SystemInit(SystemReg *s)
{
	
	if(s->initCount<500)
	{
		s->SystemStateARegs.bit.INITOK=0;
	}
	if(s->initCount>=500)
	{
		s->SystemStateARegs.bit.INITOK=1;
		s->initCount=501;
	}
}

void SystemAlarm(PackReg *bat,SystemReg *sys)
{

}
void SystemFault(PackReg *bat, SystemReg *sys)
{

}
void SysCurrentOffset(SystemReg *sys)
{
}
void SocBalance(ModuleReg *m) 
{
	
}
void AutoBalance(ModuleReg *m) 
{
	if(m->CellVoltageF[0]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell00=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell00=1;
	}
	if(m->CellVoltageF[1]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell01=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell01=1;
	}
	if(m->CellVoltageF[2]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell02=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell02=1;
	}
	if(m->CellVoltageF[3]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell03=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell03=1;
	}
	if(m->CellVoltageF[4]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell04=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell04=1;
	}
	if(m->CellVoltageF[5]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell05=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell05=1;
	}
	if(m->CellVoltageF[6]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell06=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell06=1;
	}
	if(m->CellVoltageF[7]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell07=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell07=1;
	}
	if(m->CellVoltageF[8]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell08=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell08=1;
	}
	if(m->CellVoltageF[9]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell09=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell09=1;
	}
	if(m->CellVoltageF[10]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell10=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell10=1;
	}
	if(m->CellVoltageF[11]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell11=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell11=1;
	}
}
void SystemforceBalance(ModuleReg *m)
{
	if(m->CellVoltageF[0]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell00=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell00=1;
	}
	
	if(m->CellVoltageF[1]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell01=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell01=1;
	}
	
	if(m->CellVoltageF[2]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell02=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell02=1;
	}
	
	if(m->CellVoltageF[3]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell03=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell03=1;
	}
	
	if(m->CellVoltageF[4]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell04=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell04=1;
	}
	
	if(m->CellVoltageF[5]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell05=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell05=1;
	}
	
	if(m->CellVoltageF[6]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell06=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell06=1;
	}
	
	if(m->CellVoltageF[7]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell07=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell07=1;
	}
	
	if(m->CellVoltageF[8]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell08=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell08=1;
	}
	
	if(m->CellVoltageF[9]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell09=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell09=1;
	}
	
	if(m->CellVoltageF[10]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell10=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell10=1;
	}
	
	if(m->CellVoltageF[11]<= m->ForceBalanceVoltage)
	{
		m->forceBalance.bit.B_Cell11=0;
	}
	else 
	{
		m->forceBalance.bit.B_Cell11=1;
	}

}

float32 AbsVaule(float32 input)
{
	float32 AbsCurrent=0.0;
	if(input>=0)
	{
		AbsCurrent=input;
	}
	else if(input<0)
	{
		AbsCurrent=-1.0*input;
	}	
	return AbsCurrent;
}

void CellTempCalFloat(SlaveReg *s, ModuleReg *m)
{
	float32 tmpF0=0.0;
	float32 tmpF1=0.0;
	float32 tmpF2=0.0;
	float32 tmpF3=0.0;

	float32 x02=0.0;
	float32 x0=0.0;
	float32 x12=0.0;
	float32 x1=0.0;
	float32 x22=0.0;
	float32 x2=0.0;
	float32 x32=0.0;
	float32 x3=0.0;

	tmpF0=(float)(s->CellTempBuffer[0]*0.0001);
	tmpF1=(float)(s->CellTempBuffer[1]*0.0001);
	tmpF2=(float)(s->CellTempBuffer[2]*0.0001);
	tmpF3=(float)(s->CellTempBuffer[3]*0.0001);

	x02= tmpF0*tmpF0;
	x0=tmpF0;
	m->CellTempF[0]= 3.5081*x02-41.81*x0+107.59;

	x12= tmpF1*tmpF1;
	x1=tmpF1;
	m->CellTempF[1]= 3.5081*x12-41.81*x1+107.59;


	x22= tmpF2*tmpF2;
	x2=tmpF2;
	m->CellTempF[2]= 3.5081*x22-41.81*x2+107.59;

	x32= tmpF3*tmpF3;
	x3=tmpF3;
	m->CellTempF[3]= 3.5081*x32-41.81*x3+107.59;

}
void CellVoltagetoFloat(SlaveReg *s, ModuleReg *m)
{
	int i;
	float32 sum=0.0;
	
	for (i = 0; i < ModuleCellNum; i++)
	{	
	//	m->CellVoltageF[i]=(float32)s->CellVoltage[i]/10000;
		s->CellVoltagebuffer[i]=s->CellVoltage[i]/10;
		m->CellVoltageF[i]=(float32)(s->CellVoltagebuffer[i]*0.001);

	//	m->CellVoltageF[i]=m->CellVoltageF[i]+0.0005;
		sum=sum+m->CellVoltageF[i];
	}
	m->ModuleVoltageCellMAX=(float32)(s->CellVoltageMax*0.001);
	m->ModuleVoltageCellMIN=(float32)(s->CellVoltageMin*0.001);
	m->ModuleVoltageCellAVG=m->ModuleVoltageF/12.0;
	m->ModuleVoltageCellCap=m->ModuleVoltageCellMAX-m->ModuleVoltageCellMIN;
	m->ModuleVoltageF=sum;
	
}
// 입력 인자
// S->CellNumber : 
void ModuleCellMAX(SlaveReg *S)
{
	int i;
	S->CellVoltageMax=S->CellVoltage[0];
	S->CellVoltageMaxNum=0;
	for(i=0; i<S->CellNumber;i++)
	{
		if(S->CellVoltageMax< S->CellVoltage[i])
		{
			S->CellVoltageMax = S->CellVoltage[i];	
			S->CellVoltageMaxNum=i;
		}
	}
	
}
// 입력 인자
// S->CellNumber : 
void ModuleCellMIN(SlaveReg *S)
{
	int i;
	S->CellVoltageMin=S->CellVoltage[0];
	S->CellVoltageMinNum=0;
	for(i=0; i<S->CellNumber;i++)
	{
		if(S->CellVoltageMin> S->CellVoltage[i])
		{
			S->CellVoltageMin = S->CellVoltage[i];	
			S->CellVoltageMinNum=i;
		}
	}
}
void BatteryTeperaureCal(PackReg *bat,int Count)
{
	//bat->BatteryTempCellF[i]=(-4.67*num3+35.688*num2)-102.18*num1+91.477;
}
void SlaveBMSSPIEnable_low(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;
}

void SlaveBMSSPIEnable_high(void)
{
	GpioDataRegs.GPASET.bit.GPIO10 = 1;
}
void SlaveBms_WakeUp(void)
{
	// need to check
	SlaveBMSSPIEnable_low();
	SPI_Write(0);
	SPI_Write(0);
	SPI_Write(0);
	SPI_Write(0);
	SPI_Write(0);
	SlaveBMSSPIEnable_high();
}
int SlaveBMSCellReadCommand(SlaveReg *s)
{
	int i;
	SlaveBms_WakeUp();
	SlaveBMSSPIEnable_low();
//	s->WError= LTC6804_write(s->ID, s->Command, 0, 0);

	if((s->WError!=0)&&(s->len !=0))
	{
		for (i = 0; i < s->len; i++) 
		{
			s->ADCX[i]= SPI_READ();
		}
		s->pecr = (SPI_READ() << 8) & 0xff00;
		s->pecr |= (SPI_READ() & 0x00ff);
  //		s->pecg = pec15(s->ADCX[i], s->len);
		if (s->pecr ==s->pecg) 
		{
			s->RError=0;
			s->ErrorCount= 0;
		} 
		else 
		{
			s->RError=1;
			s->ErrorCount++;
		}
	}
	if(s->RError==1)
	{
		if(s->Command==LTC6804_CMD_RDCVA)
		{
			s->ErrorCode=LTC6804_ERROR_RDCVA;
		}
		if(s->Command==LTC6804_CMD_RDCVB)
		{
			s->ErrorCode=LTC6804_ERROR_RDCVB;
		}
		if(s->Command==LTC6804_CMD_RDCVC)
		{
			s->ErrorCode=LTC6804_ERROR_RDCVC;
		}
		if(s->Command==LTC6804_CMD_RDCVD)
		{
			s->ErrorCode=LTC6804_ERROR_RDCVD;
		}
	}
	SlaveBMSSPIEnable_high();
	return s->ErrorCode;	
}

int CellBalanceSet(int pack_id, CellBalance_t cell)
{
	int ret;
	char addr = LTC6804.address[pack_id];
	char *tab = (char *)LTC6804_init_table;
	tab[4] = (cell.all & 0xff);
	tab[5] = (tab[5] & 0xf0) | ((cell.all >> 8) & 0x0f);
	ret = LTC6804_write_cmd(addr, LTC6804_CMD_WRCFG, tab, sizeof(LTC6804_init_table));
	return ret;
}


//-----------------------------------------------------------------------

void SciaTxchar(char Txchar)
{
	while(!(SciaTxReadyFlag));
	SciaRegs.SCITXBUF=Txchar;
}
void CANATX(unsigned int ID, unsigned char Length, unsigned int Data0, unsigned int Data1,unsigned int Data2,unsigned int Data3)
{
	unsigned int Data0Low, Data0High, Data1Low, Data1High;
	unsigned int Data2Low, Data2High, Data3Low, Data3High;
	struct ECAN_REGS ECanaShadow;


	Data0Low  = 0x00ff&Data0;
	Data0High = 0x00ff&(Data0>>8);

	Data1Low  = 0x00ff&Data1;
	Data1High = 0x00ff&(Data1>>8);

	Data2Low  = 0x00ff&Data2;
	Data2High = 0x00ff&(Data2>>8);

	Data3Low  = 0x00ff&Data3;
	Data3High = 0x00ff&(Data3>>8);


	// 현재 CAN-A MBOX31가 전송 중이면 리턴함
	if(ECanaRegs.CANTRS.bit.TRS31== 1) return;
	//현재 CAN-A MBOX31가 전송 상태 체크함.
  	if(ECanaShadow.CANTA.bit.TA31== 0) // 0 : Send Fail,  1 : Send OK
	{
		ECanaShadow.CANTA.all = 0;
      	ECanaShadow.CANTA.bit.TA31= 1;     	         	// Clear TA5
       	ECanaRegs.CANTA.all = ECanaShadow.CANTA.all;
   	}


	// CAN-A 송수신 체크
//	if(COM_FAULT_MODE)	CanDataCnt++;			// reset at Rx Routine
//	if(CanDataCnt > COMER_set)	COMER_FS=1;
//	Can_TX_ERR_CNT = ECanaRegs.CANTEC.bit.TEC;
//	Can_RX_ERR_CNT = ECanaRegs.CANREC.bit.REC;


	// 전송하는 데이터 ID 설정함.
	ECanaRegs.CANME.bit.ME31= 0;
	ECanaMboxes.MBOX31.MSGID.bit.STDMSGID=ID;
	ECanaRegs.CANME.bit.ME31= 1;
	// 전송하는 데이터 길이?8 설정함.


	ECanaMboxes.MBOX31.MSGCTRL.bit.DLC=Length; 	// 전송하는 데이터 길이
	// DATA 전송
	ECanaMboxes.MBOX31.MDL.byte.BYTE0=Data0Low;
	ECanaMboxes.MBOX31.MDL.byte.BYTE1=Data0High;

	ECanaMboxes.MBOX31.MDL.byte.BYTE2=Data1Low;
	ECanaMboxes.MBOX31.MDL.byte.BYTE3=Data1High;

	ECanaMboxes.MBOX31.MDH.byte.BYTE4=Data2Low;
	ECanaMboxes.MBOX31.MDH.byte.BYTE5=Data2High;

	ECanaMboxes.MBOX31.MDH.byte.BYTE6=Data3Low;
	ECanaMboxes.MBOX31.MDH.byte.BYTE7=Data3High;

	// CAN-A MBOX31 전송 시작함.
	ECanaRegs.CANTRS.bit.TRS31= 1;
	// CAN-A MBOX31전송 완료 체크함.
	while(!ECanaShadow.CANTA.bit.TA31);
	ECanaShadow.CANTA.all = 0;
   	ECanaShadow.CANTA.bit.TA31=1;     	         	// Clear TA5
   	ECanaRegs.CANTA.all = ECanaShadow.CANTA.all;
	ECanaMboxes.MBOX31.MSGID.all	= 0;

	// From Here 해당 MailBox를 CAN Enable 시킴  이것을 왜 다시 해 주어야 하�?
	EALLOW;     // EALLOW enables access to protected bits

	ECanaShadow.CANME.all = ECanaRegs.CANME.all;

	ECanaShadow.CANME.bit.ME0= 1;
	ECanaShadow.CANME.bit.ME1= 1;
	ECanaShadow.CANME.bit.ME2= 1;
	ECanaShadow.CANME.bit.ME3= 1;
	ECanaShadow.CANME.bit.ME4= 1;
	ECanaShadow.CANME.bit.ME5= 1;
	ECanaShadow.CANME.bit.ME6= 1;
	ECanaShadow.CANME.bit.ME7= 1;
	ECanaShadow.CANME.bit.ME8= 1;
	ECanaShadow.CANME.bit.ME9= 1;
	ECanaShadow.CANME.bit.ME10= 1;
	ECanaShadow.CANME.bit.ME11= 1;
	ECanaShadow.CANME.bit.ME12= 1;
	ECanaShadow.CANME.bit.ME13= 1;
	ECanaShadow.CANME.bit.ME14= 1;
	ECanaShadow.CANME.bit.ME15= 1;
	ECanaShadow.CANME.bit.ME16= 1;
	ECanaShadow.CANME.bit.ME17= 1;
	ECanaShadow.CANME.bit.ME18= 1;
	ECanaShadow.CANME.bit.ME19= 1;
	ECanaShadow.CANME.bit.ME20= 1;
	ECanaShadow.CANME.bit.ME21= 1;
	ECanaShadow.CANME.bit.ME22= 1;
	ECanaShadow.CANME.bit.ME23= 1;
	ECanaShadow.CANME.bit.ME24= 1;
	ECanaShadow.CANME.bit.ME25= 1;
	ECanaShadow.CANME.bit.ME26= 1;
	ECanaShadow.CANME.bit.ME27= 1;
	ECanaShadow.CANME.bit.ME28= 1;
	ECanaShadow.CANME.bit.ME29= 1;
	ECanaShadow.CANME.bit.ME30= 1;
	ECanaShadow.CANME.bit.ME31= 1;
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;
	InitECan();

	EDIS;

	// To Here 해당 MailBox를 CAN Enable 시킴  이것을 왜 다시 해 주어야 하지?

}//EOF

//----------------------------------------------------------------------------------디지털 INPUT/OUTPUT 관련 함수


void DigitalOutput(SystemReg *sys)
{
	if(sys->DigitalOutPutReg.bit.Relay==1)
  	{
  		Relay_ON; 
  	}
	else 
	{
		Relay_OFF;
	}
}

void DataConversion(SystemReg *sys)
{

}
*/


