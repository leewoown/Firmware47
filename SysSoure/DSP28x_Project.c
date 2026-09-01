
#include "parameter.h"
#include "SysVariable.h"
#include "DSP28x_Project.h"
#include "stdio.h"
#include "math.h"
#include <string.h>

#define A 1664525
#define C 1013904223
#define M 4294967296 // 2^32


extern CANAReg         CANARegs;           // TODOS 260726_Note1, 0.14 PWRHoldHandle 이동에 따른 참조(main.c 정의)
//extern SystemReg       SysRegs;
#if DebugBoardMode != 0
extern DbgReg          DbgRegs;   // TODOS 260726_Note1, 0.14 모사장치 수신버퍼(main.c 정의) 참조
#endif



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
extern void PWRHoldHandle(SystemReg *s);   // TODOS 260726_Note1, 0.14 main.c에서 이동
/*--------------------------------------------------------------
 * 260902 : CalP56CurrentLimit(+PackLimitLookupF)는 BATAlgorithm.c 로 이동했고
 *          호출도 main.c 에서만 한다. DSP28x_Project.c 는 미호출 + BATAlgorithm.h
 *          (SocReg 정의) 미포함이라 여기 extern 은 죽은 선언 → 제거.
 *          (그대로 두면 SocReg 미정의로 컴파일 오류)
 *--------------------------------------------------------------*/
//extern void P56PackCurrentLimit(SystemReg *s);   // 260901: 시그니처 변경으로 폐지
//extern void CalP56CurrentLimit(SocReg *p);       // 260902: DSP28x_Project.c 미사용(선언은 main.c)
//extern float32 PackLimitLookupF(const float32 *Table, Uint16 TempIdx, float32 TempRate, Uint16 SocIdx, float32 SocRate);   // 260902: DSP28x_Project.c 미사용


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
    s->Bat80VBATICTemperatureF = 0;   // TODO : [검증] 260901_Note1, 0.17 R12 BATIC 온도 초기화
    /*--------------------------------------------------------------
     * 260827 : 충/방전 허용전류 4종 — 단위 A. 초기값 5 → 0 으로 변경.
     *          5 는 단위가 kW 이던 시절의 잔재로 A 기준에서는 의미가 없다.
     *          P56PackCurrentLimit(100ms 주기)가 온도·SOC 표로 갱신하기 전까지
     *          0(충·방전 금지) 으로 두어 초기 오보고·오판정을 막는다.
     *          ※ 초과분 산출은 한계값을 갱신한 뒤에 하므로 이 값이 판정에
     *            쓰이는 경로는 없고, CAN 0x604 송신도 갱신 다음 슬롯이다.
     *--------------------------------------------------------------*/
    //s->Bat80VCHAContintyCurrF=5;
    //s->Bat80VDisCHAContintyCurrF=5;
    //s->Bat80VCHAPeakFCurrF=5;
    //s->Bat80VDisCHAPeakFCurrF=5;
    /*--------------------------------------------------------------
     * 260902 : 초기값 0 → 5 로 재변경. CAN 0x604 초기 송신값을
     *          충전 +5.0A / 방전 -5.0A 로 둔다(방전은 송신 시 -1 적용).
     *--------------------------------------------------------------*/
    //s->Bat80VCHAContintyCurrF=0;      // TODO : [검증] 260827_Note1, 0.16 연속 충전 허용전류[A] 산출 전 0
    //s->Bat80VDisCHAContintyCurrF=0;   // TODO : [검증] 260827_Note1, 0.16 연속 방전 허용전류[A] 산출 전 0(양수 크기)
    //s->Bat80VCHAPeakFCurrF=0;         // TODO : [검증] 260827_Note1, 0.16 5초 충전 허용전류[A] 산출 전 0
    //s->Bat80VDisCHAPeakFCurrF=0;      // TODO : [검증] 260827_Note1, 0.16 5초 방전 허용전류[A] 산출 전 0(양수 크기)
    s->Bat80VCHAContintyCurrF=5;      // TODO : [검증] 260902_Note1, 0.19 연속 충전 허용전류[A] 초기 +5.0A
    s->Bat80VDisCHAContintyCurrF=5;   // TODO : [검증] 260902_Note1, 0.19 연속 방전 허용전류[A] 초기 -5.0A(저장은 양수 크기)
    s->Bat80VCHAPeakFCurrF=5;         // TODO : [검증] 260902_Note1, 0.19 5초 충전 허용전류[A] 초기 +5.0A
    s->Bat80VDisCHAPeakFCurrF=5;      // TODO : [검증] 260902_Note1, 0.19 5초 방전 허용전류[A] 초기 -5.0A(저장은 양수 크기)
    s->Bat80VUnbalCurrentF=0;         // TODO : [검증] 260827_Note1, 0.16 첫 P56PackCurrentLimit 갱신(100ms) 전 오판정 방지
    s->Bat80VSOCF=0;
    s->Bat80VSOHF=0;
    s->Bat80VAhF=0;
    s->Bat80VISOResisF=0;



    s->BAPackOCCount=0;
    s->BAPackOCOld=0;          // TODO : [검증] 260827_Note1, 0.16 480A 진입 edge 이전상태 초기화
    s->BAPackOCEventTimer=0;   // TODO : [검증] 260827_Note1, 0.16 1분 집계 창 초기화
    /*--------------------------------------------------------------
     * 260827 : FAULT 유지시간 카운터 개별변수 → BatFalutCont[] 배열 전환.
     *          Alarm(Bat80VAlarmCont)과 동일하게 memset 으로 일괄 초기화.
     *--------------------------------------------------------------*/
    //s->BAPackOVCount=0;
    //s->BAPackUVCount=0;
    //s->BACellOVCount=0;
    //s->BACellUVCount=0;
    //s->BACellUBVCount=0;
    //s->BACellUBTCount=0;

    s->BalanceModeCount=0;
    s->BalanceTimeCount=0;
    s->Bat80VFaultCurrentF=0.0;
    s->TempInitCount=0;

    memset(&s->Bat80VAlarmCont[0],0.0,32);
    /*--------------------------------------------------------------
     * 260827 : FAULT 유지시간 카운터 배열은 원소 개수 대신 sizeof 로 클리어.
     *          원소 수가 바뀌어도 전체가 지워지도록 한다(260716 셀배열 수정과 동일).
     *          이 배열이 남아 있으면 리셋 직후 잔여 카운트로 즉시 FAULT 가
     *          설 수 있으므로 초기화 누락은 치명적이다.
     *--------------------------------------------------------------*/
    //memset(&s->BatFalutCont[0],0,32);
    memset(&s->BatFalutCont[0],0,sizeof(s->BatFalutCont));   // TODO : [검증] 260827_Note1, 0.16 FAULT 유지시간 카운터 배열 전체 클리어
    /*--------------------------------------------------------------
     * 260716 : float 배열 memset 크기 오류 수정. C28x는 sizeof(float32)==2 word라
     *          원소개수(22)를 크기로 주면 절반(11개)만 지워짐 → sizeof(배열)로 전체 클리어
     *--------------------------------------------------------------*/
    //memset(&s->Bat80VCellVoltageF[0],0.0,Sys80VCellVoltCount);
    //memset(&s->Bat80VCellTemperatureF[0],0.0,Sys80VCellTempCount);
    memset(&s->Bat80VCellVoltageF[0],0,sizeof(s->Bat80VCellVoltageF));           // TODO : [검증] 260716_Note1, 0.12 셀전압 배열 전체 클리어(44word)
    memset(&s->Bat80VCellTemperatureF[0],0,sizeof(s->Bat80VCellTemperatureF));   // TODO : [검증] 260716_Note1, 0.12 셀온도 배열 전체 클리어(44word)
#if DebugBoardMode != 0
    memset(&DbgRegs.CellVolt[0],0,sizeof(DbgRegs.CellVolt));                     // TODOS 260726_Note1, 0.14 모사장치 셀전압 raw 클리어(22word)
    memset(&DbgRegs.CellTemp[0],0,sizeof(DbgRegs.CellTemp));                     // TODOS 260726_Note1, 0.14 모사장치 셀온도 raw 클리어(22word)
    memset(&DbgRegs.CellVoltF[0],0,sizeof(DbgRegs.CellVoltF));                   // TODOS 260726_Note1, 0.14 모사장치 셀전압 버퍼 클리어(44word)
    memset(&DbgRegs.CellTempF[0],0,sizeof(DbgRegs.CellTempF));                   // TODOS 260726_Note1, 0.14 모사장치 셀온도 버퍼 클리어(44word)
    DbgRegs.VoltFlag.all=0;                                                      // TODOS 260726_Note1, 0.14 전압 프레임 수신표시 클리어
    DbgRegs.TempFlag.all=0;                                                      // TODOS 260726_Note1, 0.14 온도 프레임 수신표시 클리어
    DbgRegs.VoltCnt=0;                                                           // TODOS 260726_Note1, 0.14 전압 수신 카운터 클리어
    DbgRegs.TempCnt=0;                                                           // TODOS 260726_Note1, 0.14 온도 수신 카운터 클리어
#endif
}
void CANRegVarINIT(CANAReg *P)
{
    P->SWTypeVer=0;
    P->BATIC_Temp = 0;   // TODO : [검증] 260901_Note1, 0.17 R12 BATIC_Temp 초기화
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
  //  P->SwVerProducttype.all=0;
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
    P->BAT80VCHAContintyCurr=0;
    P->BAT80VCHAPeakCurr=0;
    P->BAT80VDisCHAContintyCurr=0;
    P->BAT80VDisCHAPeakCurr=0;
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
    P->BAT12VCHAContintyCurr=0;
    P->BAT12VCHAPeakCurr=0;
    P->BAT12VDisCHAContintyCurr=0;
    P->BAT12VDisCHAPeakCurr=0;
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
              s->BAT80VAlarmReg.bit.PackSOC_OV=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.PackSOC_OV==0)
          {
              s->Bat80VAlarmCont[1]=0;
          }
          if(Hyst_Off(s->Bat80VSOCF,C_PackSOCOV_WarnRst))
          {
              s->Bat80VAlarmCont[1]=0;
              s->BAT80VAlarmReg.bit.PackSOC_OV=0;
          }
      }
      // 팩 저충전,유지시간카운터배열값:2,유지시간;100msec
      if(Hyst_Off(s->Bat80VSOCF,C_PackSOCUN_Warn))
      {
          if(s->Bat80VAlarmCont[2]< C_PackSOCUN_WarnDelay){++s->Bat80VAlarmCont[2];}
          if(s->Bat80VAlarmCont[2]>=C_PackSOCUN_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.PackSOC_UN=1;
          }
      }
      else
      {
          if(s->BAT80VAlarmReg.bit.PackSOC_UN==0)
          {
              s->Bat80VAlarmCont[2]=0;
          }
          if(Hyst_On(s->Bat80VSOCF,C_PackSOCUN_WarnRst))
          {
              s->Bat80VAlarmCont[2]=0;
              s->BAT80VAlarmReg.bit.PackSOC_UN=0;
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
      /*--------------------------------------------------------------
       * 260901 : No.8 연속 전류 한계 초과 경고 (Bsa_WrnUnbalPwr, Alarm bit7) 활성화.
       *          설정표 R9 「261220 이후 반영」 대상은 차단(No.23)뿐이며
       *          경고는 그대로 사용하므로 260830 임시 차단을 해제한다.
       *          초과분[A] Bat80VUnbalCurrentF 가 0 보다 큰 상태가 10초 지속되면 Alarm.
       *          초과분 산출은 P56PackCurrentLimit(100ms 주기)이 담당한다.
       *          ※ 해제 임계(히스테리시스) 설정값이 없어 초과 해소 즉시 해제.
       *--------------------------------------------------------------*/
      // 연속 전류 한계 초과 Alarm,유지시간카운터배열값:7,유지시간:10000msec
      if(s->Bat80VUnbalCurrentF > 0)
      {
          if(s->Bat80VAlarmCont[7]< C_PackUnbalPwr_WarnDelay){++s->Bat80VAlarmCont[7];}   // TODO : [검증] 260901_Note1, 0.19 초과 지속시간 카운터
          if(s->Bat80VAlarmCont[7]>=C_PackUnbalPwr_WarnDelay)
          {
              s->BAT80VAlarmReg.bit.PackUnPWR_BL=1;                                       // TODO : [검증] 260901_Note1, 0.19 10초 이상 초과 → 경고
          }
      }
      else
      {
          s->Bat80VAlarmCont[7]=0;                                                        // TODO : [검증] 260901_Note1, 0.19 초과 해소 → 카운터 리셋
          s->BAT80VAlarmReg.bit.PackUnPWR_BL=0;                                           // TODO : [검증] 260901_Note1, 0.19 초과 해소 즉시 해제(해제 임계 미설정)
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
      /*--------------------------------------------------------------
       * 260827 : 보호설정표 260827 R6 기준 정리 (0.16)
       *  - 판정 순서를 설정표 No.16~36 = 0x603 Protection bit16~36 순으로 배열
       *  - 유지시간 카운터를 BatFalutCont[] 배열로 통일. 첨자 = FaultReg 비트번호
       *  - 판정 형식 통일 : 조건 성립 중 카운터 증가 → _FaultDelay 경과 시 FAULT
       *    비교는 증가 후 '>' 사용. 설정 0(즉시)이면 첫 tick 에 1>0 으로 성립하고,
       *    부호 없는 카운터와 0 을 '>=' 로 비교할 때 나던 경고(#188-D)도 없앤다.
       *  - FAULT 는 전원 OFF 또는 FCU_RESET 까지 걸림 유지(자동 해제 없음)
       *  - FaulBuftReg 는 현재 조건 성립 여부만 나타내는 순시 플래그
       *    (SOC/온도 6항목은 기존 동작 유지를 위해 해제 처리를 넣지 않음)
       *--------------------------------------------------------------*/

      // No.16 과전류 (Bsa_FltOc, bit16) : 505A 이상
      if(s->Bat80VCurrentAsbF >= C_PackCTOV_Fault)
      {
          s->BatFalutCont[0]++;
          s->BAT80VFaulBuftReg.bit.PackOC=1;
          s->Bat80VFaultCurrentF=s->Bat80VCurrentF;
          if(s->BatFalutCont[0]>C_PackCTOV_FaultDelay)
          {
              s->BatFalutCont[0]=C_PackCTOV_FaultDelay+10;
              s->BAT80VFaultReg.bit.PackOC=1;
          }
      }
      else
      {
          s->BatFalutCont[0]=0;
          s->BAT80VFaulBuftReg.bit.PackOC=0;
      }

      // No.17 과충전 (Bsa_FltctSocH, bit17) : SOC 100% 이상
      if(s->Bat80VSOCF >= C_PackSOCOV_Fault)
      {
          s->BatFalutCont[1]++;
          s->BAT80VFaulBuftReg.bit.PackSOC_OV=1;
          if(s->BatFalutCont[1]>C_PackSOCOV_FaultDelay)
          {
              s->BatFalutCont[1]=C_PackSOCOV_FaultDelay+10;
              s->BAT80VFaultReg.bit.PackSOC_OV=1;
          }
      }
      else
      {
          s->BatFalutCont[1]=0;
      }

      // No.18 저충전 (Bsa_FlttctSocL, bit18) : SOC 0% 이하
      if(s->Bat80VSOCF <= C_PackSOCUN_Fault)
      {
          s->BatFalutCont[2]++;
          s->BAT80VFaulBuftReg.bit.PackSOC_UN=1;
          if(s->BatFalutCont[2]>C_PackSOCUN_FaultDelay)
          {
              s->BatFalutCont[2]=C_PackSOCUN_FaultDelay+10;
              s->BAT80VFaultReg.bit.PackSOC_UN=1;
          }
      }
      else
      {
          s->BatFalutCont[2]=0;
      }

      // No.19 팩 과전압 (Bsa_FltOv, bit19) : 91.3V 이상
      if(s->Bat80VVoltageF >= C_PackVoltOV_Fault)
      {
          s->BatFalutCont[3]++;
          s->BAT80VFaulBuftReg.bit.PackVolt_OV=1;
          if(s->BatFalutCont[3]>C_PackVoltOV_FaultDelay)
          {
              s->BatFalutCont[3]=C_PackVoltOV_FaultDelay+10;
              s->BAT80VFaultReg.bit.PackVolt_OV=1;
          }
      }
      else
      {
          s->BatFalutCont[3]=0;
          s->BAT80VFaulBuftReg.bit.PackVolt_OV=0;
      }

      // No.20 팩 저전압 (Bsa_FltUv, bit20) : 58.3V 이하
      if(s->Bat80VVoltageF <= C_PackVoltUN_Fault)
      {
          s->BatFalutCont[4]++;
          s->BAT80VFaulBuftReg.bit.PackVolt_UN=1;
          if(s->BatFalutCont[4]>C_PackVoltUN_FaultDelay)
          {
              s->BatFalutCont[4]=C_PackVoltUN_FaultDelay+10;
              s->BAT80VFaultReg.bit.PackVolt_UN=1;
          }
      }
      else
      {
          s->BatFalutCont[4]=0;
          s->BAT80VFaulBuftReg.bit.PackVolt_UN=0;
      }

      // No.21 팩 고온 (Bsa_FltOt, bit21) : 평균 52도 이상
      if(s->Bat80VCellAgvTemperatureF >= C_PackTempOV_Fault)
      {
          s->BatFalutCont[5]++;
          s->BAT80VFaulBuftReg.bit.PackTemp_OV=1;
          if(s->BatFalutCont[5]>C_PackTempOV_FaultDelay)
          {
              s->BatFalutCont[5]=C_PackTempOV_FaultDelay+10;
              s->BAT80VFaultReg.bit.PackTemp_OV=1;
          }
      }
      else
      {
          s->BatFalutCont[5]=0;
      }

      // No.22 팩 저온 (Bsa_FltUt, bit22) : 평균 -35도 이하
      if(s->Bat80VCellAgvTemperatureF <= C_PackTempUN_Fault)
      {
          s->BatFalutCont[6]++;
          s->BAT80VFaulBuftReg.bit.PackTemp_UN=1;
          if(s->BatFalutCont[6]>C_PackTempUN_FaultDelay)
          {
              s->BatFalutCont[6]=C_PackTempUN_FaultDelay+10;
              s->BAT80VFaultReg.bit.PackTemp_UN=1;
          }
      }
      else
      {
          s->BatFalutCont[6]=0;
      }

      /*==============================================================
       * No.23 전류한계 초과 차단 (Bsa_FltUnbalPwr, bit23)
       * TODOS : [2nd] 260901_Note1, 0.19 261220 이후 반영 — 현재 기능 차단
       *   - 설정표 R9 「261220 이후 반영」 → 판정부 비활성, 폴트 비트 항상 0
       *   - 판정 로직은 아래 주석으로 보존. 되살릴 때 강제 해제 3줄을 지울 것
       *   - 조건 : 초과분[A] Bat80VUnbalCurrentF > 0 이 15초 지속 (P56PackCurrentLimit 산출)
       *==============================================================*/
      //if(s->Bat80VUnbalCurrentF > 0)
      //{
      //    s->BatFalutCont[7]++;
      //    s->BAT80VFaulBuftReg.bit.PackUnPWR_BL=1;
      //    if(s->BatFalutCont[7]>C_PackUnbalPwr_FaultDelay)
      //    {
      //        s->BatFalutCont[7]=C_PackUnbalPwr_FaultDelay+10;
      //        s->BAT80VFaultReg.bit.PackUnPWR_BL=1;
      //    }
      //}
      //else
      //{
      //    s->BatFalutCont[7]=0;
      //    s->BAT80VFaulBuftReg.bit.PackUnPWR_BL=0;
      //}
      s->BatFalutCont[7]=0;                                                               // TODOS : [2nd] 260901_Note1, 0.19 261220 이후 반영 - 지속시간 카운터 고정
      s->BAT80VFaulBuftReg.bit.PackUnPWR_BL=0;                                            // TODOS : [2nd] 260901_Note1, 0.19 261220 이후 반영 - 폴트 버퍼 비트 강제 해제
      s->BAT80VFaultReg.bit.PackUnPWR_BL=0;                                               // TODOS : [2nd] 260901_Note1, 0.19 261220 이후 반영 - 폴트 비트 강제 해제

      // No.24 셀 과전압 (Bsa_FltCellOv, bit24) : 최고셀 4.20V 이상
      if(s->Bat80VCellMaxVoltageF >= C_CellVoltOV_Fault)
      {
          s->BatFalutCont[8]++;
          s->BAT80VFaulBuftReg.bit.CellVolt_OV=1;
          if(s->BatFalutCont[8]>C_CellVoltOV_FaultDelay)
          {
              s->BatFalutCont[8]=C_CellVoltOV_FaultDelay+10;
              s->BAT80VFaultReg.bit.CellVolt_OV=1;
          }
      }
      else
      {
          s->BatFalutCont[8]=0;
          s->BAT80VFaulBuftReg.bit.CellVolt_OV=0;
      }

      // No.25 셀 저전압 (Bsa_FltCellUv, bit25) : 최저셀 2.65V 이하
      if(s->Bat80VCellMinVoltageF <= C_CellVoltUN_Fault)
      {
          s->BatFalutCont[9]++;
          s->BAT80VFaulBuftReg.bit.CellVolt_UN=1;
          if(s->BatFalutCont[9]>C_CellVoltUN_FaultDelay)
          {
              s->BatFalutCont[9]=C_CellVoltUN_FaultDelay+10;
              s->BAT80VFaultReg.bit.CellVolt_UN=1;
          }
      }
      else
      {
          s->BatFalutCont[9]=0;
          s->BAT80VFaulBuftReg.bit.CellVolt_UN=0;
      }

      // No.26 셀 전압 편차 (Bsa_FltCellUnbalVlt, bit26) : 350mV 이상
      if(s->Bat80VCellDivVoltageF >= C_CellVoltDIV_Fault)
      {
          s->BatFalutCont[10]++;
          s->BAT80VFaulBuftReg.bit.CellVolt_BL=1;
          if(s->BatFalutCont[10]>C_CellVoltDIV_FaultDelay)
          {
              s->BatFalutCont[10]=C_CellVoltDIV_FaultDelay+10;
              s->BAT80VFaultReg.bit.CellVolt_BL=1;
          }
      }
      else
      {
          s->BatFalutCont[10]=0;
          s->BAT80VFaulBuftReg.bit.CellVolt_BL=0;
      }

      // No.27 셀 고온 (Bsa_FltCellOt, bit27) : 최고셀 60도 이상
      if(s->Bat80VCellMaxTemperatureF >= C_CellTempOV_Fault)
      {
          s->BatFalutCont[11]++;
          s->BAT80VFaulBuftReg.bit.CellTemp_OV=1;
          if(s->BatFalutCont[11]>C_CellTempOV_FaultDelay)
          {
              s->BatFalutCont[11]=C_CellTempOV_FaultDelay+10;
              s->BAT80VFaultReg.bit.CellTemp_OV=1;
          }
      }
      else
      {
          s->BatFalutCont[11]=0;
      }

      // No.28 셀 저온 (Bsa_FltCellUt, bit28) : 최저셀 -30도 이하
      if(s->Bat80VCellMinTemperatureF <= C_CellTempUN_Fault)
      {
          s->BatFalutCont[12]++;
          s->BAT80VFaulBuftReg.bit.CellTemp_UN=1;
          if(s->BatFalutCont[12]>C_CellTempUN_FaultDelay)
          {
              s->BatFalutCont[12]=C_CellTempUN_FaultDelay+10;
              s->BAT80VFaultReg.bit.CellTemp_UN=1;
          }
      }
      else
      {
          s->BatFalutCont[12]=0;
      }

      // No.29 셀 온도 편차 (Bsa_FltCellUnbalTmp, bit29) : 20도 이상
      if(s->Bat80VCellDivTemperatureF >= C_CellTempDIV_Fault)
      {
          s->BatFalutCont[13]++;
          s->BAT80VFaulBuftReg.bit.CellTemp_BL=1;
          if(s->BatFalutCont[13]>C_CellTempDIV_FaultDelay)
          {
              s->BatFalutCont[13]=C_CellTempDIV_FaultDelay+10;
              s->BAT80VFaultReg.bit.CellTemp_BL=1;
          }
      }
      else
      {
          s->BatFalutCont[13]=0;
          s->BAT80VFaulBuftReg.bit.CellTemp_BL=0;
      }

      /*==============================================================
       * No.30 릴레이 이상 (Bsa_FltRly_Err, bit30)
       * TODOS : [2nd] 260901_Note1, 0.19 261220 이후 반영 — 판정 로직 구현 구역
       *   - 설정값 미정 (C_RleyCount 1 은 임시값, 설정표 확정 후 반영)
       *   - 판정 입력 RelayCheck 산출 로직 없음 (초기화만 존재)
       *   - 구현 시 FaulBuftReg + FaultReg 둘 다 set, 걸림 유지 적용
       *   - 되살릴 때 아래 강제 해제 2줄을 지울 것
       *==============================================================*/
      //if(s->RelayCheck >= C_RleyCount)
      //{
      //    s->BAT80VFaulBuftReg.bit.PackRLY_ERR=1;
      //}
      s->BAT80VFaulBuftReg.bit.PackRLY_ERR=0;                  // TODOS : [2nd] 260901_Note1, 0.19 261220 이후 반영 - 순시 비트 강제 해제
      s->BAT80VFaultReg.bit.PackRLY_ERR=0;                     // TODOS : [2nd] 260901_Note1, 0.19 261220 이후 반영 - 폴트 비트 강제 해제

      /*==============================================================
       * No.32 셀 내부저항 (Bsa_FltCellIR_OV, bit32) : 15mOhm 이상
       * TODOS : [2nd] 260901_Note1, 0.19 261220 이후 반영 — 판정 로직 구현 구역
       *   - 셀 내부저항 산출 로직 없음 (BAT80VInternalResCell 는 송신 배열)
       *   - 구현 시 BatFalutCont[16] 을 유지시간 카운터로 사용
       *   - 되살릴 때 아래 강제 해제 2줄을 지울 것
       *==============================================================*/
      s->BAT80VFaulBuftReg.bit.CellIR_OV=0;                    // TODOS : [2nd] 260901_Note1, 0.19 261220 이후 반영 - 순시 비트 강제 해제
      s->BAT80VFaultReg.bit.CellIR_OV=0;                       // TODOS : [2nd] 260901_Note1, 0.19 261220 이후 반영 - 폴트 비트 강제 해제

      // No.33 최대 전류 시간 (Bsa_FltOcTimer, bit33) : 480A 이상 10초 지속
      if(s->Bat80VCurrentAsbF >= C_PackOCTimer_Fault)
      {
          s->BAPackOCCount++;
          if(s->BAPackOCCount>=C_PackOCTimerCount)
          {
              s->BAPackOCCount=C_PackOCTimerCount+10;
              s->BAT80VFaultReg.bit.PackOcTime_Err=1;
          }
          /* No.34 최대 전류 반복 : 임계 진입 순간을 1회로 집계 */
          if(s->BAPackOCOld==0)
          {
              if(s->BAPackOCEventTimer==0)
              {
                  s->BAPackOCEventTimer=C_PackOCEventWindow;   /* 첫 진입에서 1분 창 시작 */
                  s->BatFalutCont[18]=0;
              }
              if(s->BatFalutCont[18]<C_PackOCEventCount){++s->BatFalutCont[18];}
              if(s->BatFalutCont[18]>=C_PackOCEventCount)
              {
                  s->BAT80VFaultReg.bit.PrtcOcEvent_Err=1;
              }
          }
          s->BAPackOCOld=1;
      }
      else
      {
          s->BAPackOCCount=0;
          s->BAPackOCOld=0;
      }
      /* No.34 집계 창은 전류 수준과 무관하게 진행, 1분 경과 시 횟수 초기화 */
      if(s->BAPackOCEventTimer>0)
      {
          s->BAPackOCEventTimer--;
          if(s->BAPackOCEventTimer==0)
          {
              s->BatFalutCont[18]=0;
          }
      }

      /* No.35 통신칩 이상 (BPA_FltISOSPI_Err, bit19/CAN35) : 210회 연속, 판정 로직 미구현
       *        C_ISOSPICount 는 설정값만 정렬된 상태 */

      /*==============================================================
       * No.36 절연저항 이상 (Bsa_FltIMD_Err, bit20 / CAN36)
       * TODOS : [2nd] 260901_Note1, 0.19 261220 이후 반영 — 판정 로직 구현 구역
       *   - 설정값 미정 (C_IOSresistanceFault 45000 은 임시값)
       *   - 판정 입력 Bat80VISOResisF 산출 로직 없음 (초기화만 존재)
       *   - 기존 코드가 PackRLY_ERR(No.30) 에 오매핑돼 있던 것을
       *     PackIMD_ERR 로 분리해 둠. 구현 시 이 비트를 사용할 것
       *   - 되살릴 때 아래 강제 해제 2줄을 지울 것
       *==============================================================*/
      //if(s->Bat80VISOResisF > C_IOSresistanceFault)
      //{
      //    s->BAT80VFaulBuftReg.bit.PackIMD_ERR=1;
      //}
      s->BAT80VFaulBuftReg.bit.PackIMD_ERR=0;                  // TODOS : [2nd] 260901_Note1, 0.19 261220 이후 반영 - 순시 비트 강제 해제
      s->BAT80VFaultReg.bit.PackIMD_ERR=0;                     // TODOS : [2nd] 260901_Note1, 0.19 261220 이후 반영 - 폴트 비트 강제 해제
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
/*========================================
 * PWRHoldHandle : BMS 전원 유지(PWRHOLD, GPIO21) 제어
 *   ※ 1ms ISR 주기 호출 전제 (C_PwrHoldOffDelayCount = 1ms×300000 = 5분)
 *
 *   - RUNStatus == 1 : VCU RUN 명령 → 전원 유지 (PWRHOLD_ON)
 *   - RUNStatus == 0 : RUN 종료 → VCU 통신 상태로 분기
 *       · 통신 끊김  : 원 OFF 판단 → 5분전 유지 후 PWRHOLD_OFF (대기 중 CAN 통신은 유지)
 *       · 통신 정상  : BMS 전원 ON 판단 → PWRHOLD_ON (SOC는 전류적산 + NVRAM으로만 계속)
 *
 *   VCU 통신 상태는 수신 워치독(s->SysCanRxCount)으로 판정:
 *     VCU 수신 시 ISR에서 0 리셋, 100ms마다 증가. C_VcuCommLostCount 이상이면 끊김.
 *
 *   TODO(검증 필요): 실기 검증 전 신규 로직. 검증 항목 —
 *     (1) ISR 주기 1ms 여부(CpuTimer0 PRD=80400), (2) GPIO21 극성(High=전원유지),
 *     (3) VCU 미연결 시 ~5분 후 자동 PWRHOLD_OFF 동작, (4) 통신 복구 시 카운트 리셋.
 *     검증 완료 후 본 TODO 제거.
 *========================================*/
void PWRHoldHandle(SystemReg *s)           // TODOS 260726_Note1, 0.14 main.c에서 DSP28x_Project.c로 이동
{
    static Uint32 PwrOffDelayCount = 0u;
    Uint16 VcuCommLost;

    VcuCommLost = (s->SysCanRxCount >= C_VcuCommLostCount) ? 1u : 0u;

    if(CANARegs.PMSCMDRegs.bit.RUNStatus == 1u)
    {
        /* VCU RUN 명령 → 전원 유지 */
        PWRHOLD_ON;
        CANARegs.BAT80VDigitalOutPutReg.bit.PWRHoldOUT = 1u;
        PwrOffDelayCount = 0u;
    }
    else
    {
        /* RUNStatus == 0 : RUN 종료 */
        if(VcuCommLost == 1u)
        {
            /* VCU 통신 끊김 → 전원 OFF 판단. 5분 유지 후 PWRHOLD_OFF */
            if(PwrOffDelayCount < C_PwrHoldOffDelayCount)
            {
                PwrOffDelayCount++;
            }
            else
            {
                PWRHOLD_OFF;
                CANARegs.BAT80VDigitalOutPutReg.bit.PWRHoldOUT = 0;
            }
        }
        else
        {
            /* VCU 통신 정상 → BMS 전원 ON 판단, 전원 유지 (SOC는 전류적산 + NVRAM) */
            PWRHOLD_ON;
            CANARegs.BAT80VDigitalOutPutReg.bit.PWRHoldOUT = 1u;
            PwrOffDelayCount = 0u;
        }
    }
}

