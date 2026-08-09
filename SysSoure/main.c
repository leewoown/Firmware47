

/**
 * main.c
 */

#include "DSP28x_Project.h"
#include "parameter.h"
#include "SysVariable.h"
#include "ProtectRelay.h"
#include "BAT_LTC6802.h"
#include "BATAlgorithm.h"
#include "NVRAM.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

/*
 *
 */
void InitGpio(void);

void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr);

/*
 *
 */
void InitECanaGpio(void);
void InitECana(void);
void CANATX(unsigned int ID, unsigned char Length, unsigned int Data0, unsigned int Data1,unsigned int Data2,unsigned int Data3);

/*
 *
 */
void SysTimerINIT(SystemReg *s);
void SysVarINIT(SystemReg *s);
void CANRegVarINIT(CANAReg *P);
void DigitalInput(SystemReg *sys);
void DigitalOutput(SystemReg *sys);
/*
 *
 */
void ProtectRelaySateCheck(PrtectRelayReg *p);
void ProtectRelayVarINIT(PrtectRelayReg *p);
void ProtectOffHandle(PrtectRelayReg *p);
void ProtectRelayWakeUpHandle(PrtectRelayReg *p);
void ProtectRelayHandle(PrtectRelayReg *p);
void Bat80V_FanControlHandle(PrtectRelayReg *p);
/*
 *
 */
void Cal80VSysVoltageHandle(SystemReg *s);
void Cal80VSysTemperatureHandle(SystemReg *s);
void Cal80VSysCurrentHandle(SystemReg *s);
/*
 * SOC 알고지름
 */

void CalSocRegsInit(SocReg *P);
void CellP56AhSocInit(SocReg *P);
void CalSocHandle(SocReg *P);
/*
 *
 */
void Cal80VSysFaultCheck(SystemReg *s);
void Cal80VSysAlarmtCheck(SystemReg *s);
void PWRHoldHandle(SystemReg *s);


void NVRAM_AZoneSaveHandler(NVRZoneAReg *p);
void NVRAM_AZoneReadHandler(NVRZoneAReg *p);
void NVRAM_RecognizeAndInit(NVRZoneAReg *p);
void NVRAM_StateTest(void);
/*
 *
 */
int float32ToInt(float32 Vaule, Uint32 Num);
/*
 *
 */

/*
 *
 */
void SPI_Write(unsigned int WRData);
unsigned int SPI_Read(void);
void BAT_InitSPI(void);
void SPI_BATWrite(unsigned int WRData);
/*
 *
 */
int LTC6804_read_cmd(char address, short command, char data[], int len);
int LTC6804_write_cmd(char address, short command, char data[], int len);
int LTC6804_DieTemperatureRead(int pack_id, float *temperature);
void init_PEC15_Table(void);
unsigned short pec15(char *data, int len);
int SlaveBMSIint(SlaveReg *s);
int SlaveBmsBalance(SlaveReg *s);
void SlaveVoltagHandler(SlaveReg *s);
void SlaveVoltagBalaHandler(SlaveReg *s);
//int  SlaveBMSDigiteldoutOHandler(SlaveReg *P);
void SlaveBMSDigiteldoutOHandler(SlaveReg *P);
void SalveTempsHandler(SlaveReg *s);


void SalveTempsVoltHandler(SlaveReg *s);
void SalveTempsVoltHandler_B(SlaveReg *s);
void TempTemps(SystemReg *s);
/*
 *  인터럽트 함수 선언
 */
interrupt void cpu_timer0_isr(void);
interrupt void ISR_CANRXINTA(void);
//interrupt void cpu_timer2_isr(void);



SystemReg       SysRegs;

PrtectRelayReg  PrtectRelayRegs;

SlaveReg        Slave1Regs;
SlaveReg        Slave2Regs;
//SlaveReg        Slave3Regs;

NVRAllReg       NVRAllRegs;
NVRZoneAReg     NVRZoneAWRRegs;
NVRZoneAReg     NVRZoneARDRegs;
NVRZoneAReg     NVRZoneAInitRegs;
CANAReg         CANARegs;
SocReg          Farasis56AhSocRegs;

#if DebugBoardMode != 0
DbgReg          DbgRegs;          // TODOS 260726_Note1, 0.14 모사장치 셀전압·온도 CAN 수신 버퍼
#endif



//SocReg          Farasis40AhSocRegs;
//SocReg          Frey60AhSocRegs;

float32         randomCT=0;
float32         randomA=0;
float32         randomC=0;
float32         testct=0;
float32         testctabs=0;
float32         NCMsocTestVoltAGV =3.210;
float32         NUMsocTestVCT =0.0;
float32         LFPsocTestVoltAGV =3.050;
float32         LFPsocTestVCT =0.0;
float32         Slave1Temps=0;
float32         Slave2Temps=0;
float32         Slave3Temps=0;
unsigned int    ProtectRelayCyle=0;
unsigned int    LFPINITFLAG=1;


//extern unsigned int    CellVoltUnBalaneFaulCount=0;
void main(void)
{
//    struct ECAN_REGS ECanaShadow;
    InitSysCtrl();
    /*
     * To check the clock status of the C2000 in operation
     */
  //  GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 3; //enable XCLOCKOUT through GPIO mux
  //  SysCtrlRegs.XCLK.bit.XCLKOUTDIV = 0; //XCLOCKOUT = 1/2* SYSCLK

// Step 2. Initalize GPIO:
// This example function is found in the DSP2803x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// For this example use the following configuration:
// Step 3. Clear all interrupts and initialize PIE vector table:
    DINT;
// Initialize PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the DSP2803x_PieCtrl.c file.
    InitPieCtrl();
// Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    EALLOW;  // This is needed to write to EALLOW protected registers

    /*
     *  인터럽트 함수 선언
     */
    PieVectTable.TINT0 = &cpu_timer0_isr;
    PieVectTable.ECAN0INTA  = &ISR_CANRXINTA;
//    PieVectTable.TINT2 = &cpu_timer2_isr;
    EDIS;    // This is needed to disable write to EALLOW protected registers
    InitGpio();
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 3; //enable XCLOCKOUT through GPIO mux
    SysCtrlRegs.XCLK.bit.XCLKOUTDIV = 2; //XCLOCKOUT = SYSCLK
    InitSpiGpio();
    InitSpi();
    InitECanGpio();
    InitECan();

    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    InitFlash();

    ConfigCpuTimer(&CpuTimer0, 80, 1000);
    //CpuTimer0Regs.PRD.all = 80000;// 90000 is 1msec
    CpuTimer0Regs.PRD.all = 80400;// 90000 is 1msec
    //   ConfigCpuTimer(&CpuTimer1, 80, 1000000);
    //   ConfigCpuTimer(&CpuTimer2, 80, 1000000);
    CpuTimer0Regs.TCR.all = 0x4001; // Use write-only instruction to set TSS bit = 0
    //  CpuTimer1Regs.TCR.all = 0x4001; // Use write-only instruction to set TSS bit = 0
    //  CpuTimer2Regs.TCR.all = 0x4001; // Use write-only instruction to set TSS bit = 0
//    InitAdc();
//    AdcOffsetSelfCal();
    EALLOW;
    EDIS;    // This is needed to disable write to EALLOW protected registers
    IER |= M_INT1;
    IER |= M_INT13;
    IER |= M_INT14;
    IER |= M_INT9;//test
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;      // Enable TINT0 in the PIE: Group 1 interrupt 7
    PieCtrlRegs.PIEIER9.bit.INTx5 = 1;      // Enable ECAN-A interrupt of PIE group 9
//  PieCtrlRegs.PIEIER9.bit.INTx1 = 1;      // SCIA RX interrupt of PIE group
    EINT;   // Enable Global interrupt INTM
    ERTM;   // Enable Global realtime interrupt DBGM
    SysRegs.SysMachine =System_STATE_INIT;
    Slave1Regs.StateMachine =STATE_BATIDLE;
    Slave2Regs.StateMachine =STATE_BATIDLE;
//    Slave3Regs.StateMachine =STATE_BATIDLE;
    while(1)
    {

        SysRegs.Maincount++;
        /* 부팅 초기화 완료(INITOK=1) 후에는 VCU 통신 끊김(PackFcu_CANErr) 여부와
         * 무관하게 CAN 송신을 항상 활성 유지 */
        if(SysRegs.BAT80VStateReg.bit.INITOK==1)
        {
            SysRegs.BAT80VStateReg.bit.CANCOMEnable=1;
        }
        switch(SysRegs.SysMachine)
        { 
            case System_STATE_INIT:
                 //SysRegs.BAT80VStateReg.bit.SysSTATE = 0;
                 CANARegs.BAT80VStatus.bit.BATStatus=0;

                 SysTimerINIT(&SysRegs);
                 SysVarINIT(&SysRegs);
                 CANRegVarINIT(&CANARegs);
                 CalSocRegsInit(&Farasis56AhSocRegs);
                 ProtectRelayVarINIT(&PrtectRelayRegs);
                 SysRegs.BAT80VDigitalOutPutReg.bit.LEDAlarmOUT=0;
                 SysRegs.BAT80VDigitalOutPutReg.bit.LEDFaultOUT=0;
                 SysRegs.BAT80VDigitalOutPutReg.bit.LEDProtectOUT=0;
                 SysRegs.BAT80VStateReg.bit.CANCOMEnable=0;
                 SysRegs.BAT80VStateReg.bit.BalanceMode=0;
                 SysRegs.BAT80VStateReg.bit.INITOK=0;
                 NVRAllRegs.SEQTimeTick=0;
                 CANARegs.BAT80VFaultCT = 0;
                 SysRegs.Bat80VFaultCurrentF=0.0;
                 SysRegs.Bat80VFaultCurrentF=0.0;

                 /*
                  *
                  */
                 SysRegs.SysRegTimer5msecCount=0;
                 SysRegs.SysRegTimer10msecCount=0;
                 SysRegs.SysRegTimer50msecCount=0;
                 SysRegs.SysRegTimer100msecCount=0;
                 SysRegs.SysRegTimer300msecCount=0;
                 SysRegs.SysRegTimer500msecCount=0;
                 SysRegs.BalanceModeCount=0;
                 SysRegs.BalanceTimeCount=0;
                 SysRegs.Bat80VFaultStatecount=0;
                 CANARegs.MailBoxRxCount =0;
                 CANARegs.MailBox0RxCount =0;
                 CANARegs.MailBox1RxCount =0;
                 CANARegs.MailBox2RxCount =0;
                 /*
                  *
                  */
                 if(Slave1Regs.StateMachine==STATE_BATIDLE)
                 {
                     Slave1Regs.ID=BMS_ID_1;
                     Slave1Regs.ErrorCount=0;
                     SlaveBMSIint(&Slave1Regs);
                     Slave1Regs.Balance.all=0x0000;
                     memset(&Slave1Regs.CellVoltage[0],3250,12);
                     memset(&Slave1Regs.CellTemperature[0],200,12);
                     Slave1Regs.StateMachine = STATE_BATSTANDBY;
                 }
                 if(Slave2Regs.StateMachine==STATE_BATIDLE)
                  {
                      Slave2Regs.ID=BMS_ID_2;
                      Slave2Regs.ErrorCount=0;
                      SlaveBMSIint(&Slave2Regs);
                      Slave2Regs.Balance.all=0x0000;
                      memset(&Slave2Regs.CellVoltage[0],3250,12);
                      memset(&Slave2Regs.CellTemperature[0],200,12);
                      Slave2Regs.StateMachine = STATE_BATSTANDBY;
                 }
                 SysRegs.BAT80VStateReg.bit.CANCOMEnable=0;
                 SysRegs.BAT80VStateReg.bit.BalanceMode=0;
                 SysRegs.BalanceModeCount=0;
                 SysRegs.BalanceTimeCount=0;
                 Farasis56AhSocRegs.state=SOC_STATE_IDLE;
                 SysRegs.BAT80VFaulBuftReg.all=0;
                 
                 /*
                  * CELL VOLTAGE measurement
                  */
#if DebugBoardMode == 0
                 /* TODOS 260726_Note1, 0.14 디버깅모드는 isoSPI 취득 생략 — CAN(0x401~0x406) 수신값 사용 */
                 for(SysRegs.InitValuleCnt=0;SysRegs.InitValuleCnt<5;SysRegs.InitValuleCnt++)
                 {
                     Slave1Regs.StateMachine = STATE_BATREAD;
                     Slave2Regs.StateMachine = STATE_BATREAD;
                     Slave1Regs.ID=BMS_ID_1;

                     SlaveVoltagHandler(&Slave1Regs);
                     Slave2Regs.ID=BMS_ID_2;
                     SlaveVoltagHandler(&Slave2Regs);
                     /*
                      * BAT80V CELL VOLTATGE, SOC
                      * Initialize SOC calculation
                      */
                 }
                 memcpy(&SysRegs.Bat80VCellVoltageF[0],     &Slave1Regs.CellVoltageF[0],sizeof(float32)*11);
                 memcpy(&SysRegs.Bat80VCellVoltageF[11],    &Slave2Regs.CellVoltageF[0],sizeof(float32)*11);
#endif
#if DebugBoardMode != 0
                while(DbgRegs.VoltFlag.all != 0x003F)
                {
                    /* wait for all cell voltage frames (0x401~0x406) */
                }
                DbgRegs.CellVoltF[0]  = (float32)(DbgRegs.CellVolt[0]*0.001);    // TODOS 260726_Note1, 0.14 셀 01 mV to V
                DbgRegs.CellVoltF[1]  = (float32)(DbgRegs.CellVolt[1]*0.001);    // TODOS 260726_Note1, 0.14 셀 02 mV to V
                DbgRegs.CellVoltF[2]  = (float32)(DbgRegs.CellVolt[2]*0.001);    // TODOS 260726_Note1, 0.14 셀 03 mV to V
                DbgRegs.CellVoltF[3]  = (float32)(DbgRegs.CellVolt[3]*0.001);    // TODOS 260726_Note1, 0.14 셀 04 mV to V
                DbgRegs.CellVoltF[4]  = (float32)(DbgRegs.CellVolt[4]*0.001);    // TODOS 260726_Note1, 0.14 셀 05 mV to V
                DbgRegs.CellVoltF[5]  = (float32)(DbgRegs.CellVolt[5]*0.001);    // TODOS 260726_Note1, 0.14 셀 06 mV to V
                DbgRegs.CellVoltF[6]  = (float32)(DbgRegs.CellVolt[6]*0.001);    // TODOS 260726_Note1, 0.14 셀 07 mV to V
                DbgRegs.CellVoltF[7]  = (float32)(DbgRegs.CellVolt[7]*0.001);    // TODOS 260726_Note1, 0.14 셀 08 mV to V
                DbgRegs.CellVoltF[8]  = (float32)(DbgRegs.CellVolt[8]*0.001);    // TODOS 260726_Note1, 0.14 셀 09 mV to V
                DbgRegs.CellVoltF[9]  = (float32)(DbgRegs.CellVolt[9]*0.001);    // TODOS 260726_Note1, 0.14 셀 10 mV to V
                DbgRegs.CellVoltF[10] = (float32)(DbgRegs.CellVolt[10]*0.001);   // TODOS 260726_Note1, 0.14 셀 11 mV to V
                DbgRegs.CellVoltF[11] = (float32)(DbgRegs.CellVolt[11]*0.001);   // TODOS 260726_Note1, 0.14 셀 12 mV to V
                DbgRegs.CellVoltF[12] = (float32)(DbgRegs.CellVolt[12]*0.001);   // TODOS 260726_Note1, 0.14 셀 13 mV to V
                DbgRegs.CellVoltF[13] = (float32)(DbgRegs.CellVolt[13]*0.001);   // TODOS 260726_Note1, 0.14 셀 14 mV to V
                DbgRegs.CellVoltF[14] = (float32)(DbgRegs.CellVolt[14]*0.001);   // TODOS 260726_Note1, 0.14 셀 15 mV to V
                DbgRegs.CellVoltF[15] = (float32)(DbgRegs.CellVolt[15]*0.001);   // TODOS 260726_Note1, 0.14 셀 16 mV to V
                DbgRegs.CellVoltF[16] = (float32)(DbgRegs.CellVolt[16]*0.001);   // TODOS 260726_Note1, 0.14 셀 17 mV to V
                DbgRegs.CellVoltF[17] = (float32)(DbgRegs.CellVolt[17]*0.001);   // TODOS 260726_Note1, 0.14 셀 18 mV to V
                DbgRegs.CellVoltF[18] = (float32)(DbgRegs.CellVolt[18]*0.001);   // TODOS 260726_Note1, 0.14 셀 19 mV to V
                DbgRegs.CellVoltF[19] = (float32)(DbgRegs.CellVolt[19]*0.001);   // TODOS 260726_Note1, 0.14 셀 20 mV to V
                DbgRegs.CellVoltF[20] = (float32)(DbgRegs.CellVolt[20]*0.001);   // TODOS 260726_Note1, 0.14 셀 21 mV to V
                DbgRegs.CellVoltF[21] = (float32)(DbgRegs.CellVolt[21]*0.001);   // TODOS 260726_Note1, 0.14 셀 22 mV to V
                memcpy(&SysRegs.Bat80VCellVoltageF[0],    &DbgRegs.CellVoltF[0],sizeof(float32)*22);
#endif
                 Cal80VSysVoltageHandle(&SysRegs);
                 /*
                  * CELL temperature measurement
                  */
#if DebugBoardMode == 0
                 for(SysRegs.TempInitCount=0;SysRegs.TempInitCount<50;SysRegs.TempInitCount++)
                 {
                     Slave1Regs.ID=BMS_ID_1;
                     Slave1Regs.BATICDO.bit.GPIO1=1;
                     SlaveBMSDigiteldoutOHandler(&Slave1Regs);
                     SalveTempsHandler(&Slave1Regs);                       // TODOS 260726_Note1, 0.14 디버깅모드는 CAN(0x407~0x40C) 수신으로 대체
                    // delay_ms(1);
                     Slave2Regs.ID=BMS_ID_2;
                     Slave2Regs.BATICDO.bit.GPIO1=1;
                     SlaveBMSDigiteldoutOHandler(&Slave2Regs);

                     SalveTempsHandler(&Slave2Regs);                       // TODOS 260726_Note1, 0.14 디버깅모드는 CAN(0x407~0x40C) 수신으로 대체
                   //  delay_ms(1);
                 }
                 memcpy(&SysRegs.Bat80VCellTemperatureF[0],     &Slave1Regs.CellTemperatureF[0],sizeof(float32)*11);
                 memcpy(&SysRegs.Bat80VCellTemperatureF[11],    &Slave2Regs.CellTemperatureF[0],sizeof(float32)*11);
#endif
#if DebugBoardMode != 0
                while(DbgRegs.TempFlag.all != 0x003F)
                {
                    /* wait for all cell temperature frames (0x407~0x40C) */
                }
                DbgRegs.CellTempF[0]=(float32)(DbgRegs.CellTemp[0]*0.1);   // TODOS 260726_Note1, 0.14 셀 01 0.1℃ to ℃
                DbgRegs.CellTempF[1]=(float32)(DbgRegs.CellTemp[1]*0.1);   // TODOS 260726_Note1, 0.14 셀 02 0.1℃ to ℃
                DbgRegs.CellTempF[2]=(float32)(DbgRegs.CellTemp[2]*0.1);   // TODOS 260726_Note1, 0.14 셀 03 0.1℃ to ℃
                DbgRegs.CellTempF[3]=(float32)(DbgRegs.CellTemp[3]*0.1);   // TODOS 260726_Note1, 0.14 셀 04 0.1℃ to ℃
                DbgRegs.CellTempF[4]=(float32)(DbgRegs.CellTemp[4]*0.1);   // TODOS 260726_Note1, 0.14 셀 05 0.1℃ to ℃
                DbgRegs.CellTempF[5]=(float32)(DbgRegs.CellTemp[5]*0.1);   // TODOS 260726_Note1, 0.14 셀 06 0.1℃ to ℃
                DbgRegs.CellTempF[6]=(float32)(DbgRegs.CellTemp[6]*0.1);   // TODOS 260726_Note1, 0.14 셀 07 0.1℃ to ℃
                DbgRegs.CellTempF[7]=(float32)(DbgRegs.CellTemp[7]*0.1);   // TODOS 260726_Note1, 0.14 셀 08 0.1℃ to ℃
                DbgRegs.CellTempF[8]=(float32)(DbgRegs.CellTemp[8]*0.1);   // TODOS 260726_Note1, 0.14 셀 09 0.1℃ to ℃
                DbgRegs.CellTempF[9]=(float32)(DbgRegs.CellTemp[9]*0.1);   // TODOS 260726_Note1, 0.14 셀 10 0.1℃ to ℃
                DbgRegs.CellTempF[10]=(float32)(DbgRegs.CellTemp[10]*0.1); // TODOS 260726_Note1, 0.14 셀 11 0.1℃ to ℃
                DbgRegs.CellTempF[11]=(float32)(DbgRegs.CellTemp[11]*0.1); // TODOS 260726_Note1, 0.14 셀 12 0.1℃ to ℃
                DbgRegs.CellTempF[12]=(float32)(DbgRegs.CellTemp[12]*0.1); // TODOS 260726_Note1, 0.14 셀 13 0.1℃ to ℃
                DbgRegs.CellTempF[13]=(float32)(DbgRegs.CellTemp[13]*0.1); // TODOS 260726_Note1, 0.14 셀 14 0.1℃ to ℃
                DbgRegs.CellTempF[14]=(float32)(DbgRegs.CellTemp[14]*0.1); // TODOS 260726_Note1, 0.14 셀 15 0.1℃ to ℃
                DbgRegs.CellTempF[15]=(float32)(DbgRegs.CellTemp[15]*0.1); // TODOS 260726_Note1, 0.14 셀 16 0.1℃ to ℃
                DbgRegs.CellTempF[16]=(float32)(DbgRegs.CellTemp[16]*0.1); // TODOS 260726_Note1, 0.14 셀 17 0.1℃ to ℃
                DbgRegs.CellTempF[17]=(float32)(DbgRegs.CellTemp[17]*0.1); // TODOS 260726_Note1, 0.14 셀 18 0.1℃ to ℃
                DbgRegs.CellTempF[18]=(float32)(DbgRegs.CellTemp[18]*0.1); // TODOS 260726_Note1, 0.14 셀 19 0.1℃ to ℃
                DbgRegs.CellTempF[19]=(float32)(DbgRegs.CellTemp[19]*0.1); // TODOS 260726_Note1, 0.14 셀 20 0.1℃ to ℃
                DbgRegs.CellTempF[20]=(float32)(DbgRegs.CellTemp[20]*0.1); // TODOS 260726_Note1, 0.14 셀 21 0.1℃ to ℃
                DbgRegs.CellTempF[21]=(float32)(DbgRegs.CellTemp[21]*0.1); // TODOS 260726_Note1, 0.14 셀 22 0.1℃ to ℃
                memcpy(&SysRegs.Bat80VCellTemperatureF[0],     &DbgRegs.CellTempF[0],sizeof(float32)*22);
#endif
                 Cal80VSysTemperatureHandle(&SysRegs);

                 /*
                  *
                  */
                // NVRAM_AZoneReadHandler(&NVRZoneARDRegs);
                 //Farasis56AhSocRegs.NVRSocInitF = (float32)NVRZoneARDRegs.LastSOC/10.0F;

                 /*----------------------------------------
                  * 1. 평균 셀 전압 입력
                  *----------------------------------------*/
                 Farasis56AhSocRegs.CellAgvVoltageF = SysRegs.Bat80VCellAgvVoltageF;
                 /*----------------------------------------
                  * 2. NVR SOC 입력
                  *----------------------------------------*/
                 NVRAM_AZoneReadHandler(&NVRZoneARDRegs);
                 Farasis56AhSocRegs.NVRSocInitF = (float32)NVRZoneARDRegs.LastSOC / 10.0F;
                 /* NVR TODO: 한글 주석 복구 필요(원본 인코딩 손상) */
                //  if(Farasis56AhSocRegs.NVRSocInitF > 100.0F)
                //  {
                //      Farasis56AhSocRegs.NVRSocInitF = 100.0F;
                //  }
                //  else if(Farasis56AhSocRegs.NVRSocInitF < 0.0F)
                //  {
                //      Farasis56AhSocRegs.NVRSocInitF = 0.0F;
                //  }
                /* NVR 유효성 검증 — 비정상이면 무효 마커(-1)로 표시 */
                if((NVRZoneARDRegs.LastSOC < 0) ||                         /* 음수 (0xFFFF=-1 포함) */
                (NVRZoneARDRegs.LastSOC > 1000) ||                         /* 100% 초과 */
                (NVRZoneARDRegs.MetaVersion == 0xFFFF) ||                  /* 첫 부팅 (Flash 미초기화) */
                (NVRZoneARDRegs.MetaVersion == 0))                         /* MetaVersion 손상 */
                {
                    /* 무효: OCV로 강제 */
                    Farasis56AhSocRegs.NVRSocInitF = -1.0F;                   /* 무효 마커 */
                    
                }
                else
                {
                    Farasis56AhSocRegs.NVRSocInitF = (float32)NVRZoneARDRegs.LastSOC / 10.0F;
                    /* 정상 범위는 NVR 사용 */
                    if(Farasis56AhSocRegs.NVRSocInitF > 100.0F)
                    {
                        Farasis56AhSocRegs.NVRSocInitF = 100.0F;
                    }
                    else if(Farasis56AhSocRegs.NVRSocInitF < 0.0F)
                    {
                        Farasis56AhSocRegs.NVRSocInitF = 0.0F;
                    }
                }
                 /*----------------------------------------
                  * 3. SOC 초기화 (OCV + NVR 판단)
                  *----------------------------------------*/

                 CellP56AhSocInit(&Farasis56AhSocRegs);

                 /*----------------------------------------
                  * 4. 시스템 SOC 반영
                  *----------------------------------------*/
                 Farasis56AhSocRegs.delta = fabs(Farasis56AhSocRegs.SOCbufF - Farasis56AhSocRegs.NVRSocInitF);
                 /* OCV 값은 SOCbufF 사용 (이미 계산된 값) */
                 if((Farasis56AhSocRegs.SOCbufF >= C_SocOCVLinearMinF) && (Farasis56AhSocRegs.SOCbufF <= C_SocOCVLinearMaxF))
                 {
                    //  /* TODO: 한글 주석 복구 필요(원본 인코딩 손상) */
                    //  if(Farasis56AhSocRegs.delta > 20.0F)
                    //  {
                    //      /* NVR TODO: 한글 주석 복구 필요(원본 인코딩 손상) OCV TODO: 한글 주석 복구 필요(원본 인코딩 손상) */
                    //      Farasis56AhSocRegs.SysSocInitF = Farasis56AhSocRegs.SOCbufF;
                    //  }
                    //  else
                    //  {
                    //      /* NVR TODO: 한글 주석 복구 필요(원본 인코딩 손상) NVR TODO: 한글 주석 복구 필요(원본 인코딩 손상) */
                    //      Farasis56AhSocRegs.SysSocInitF = Farasis56AhSocRegs.NVRSocInitF;
                    //  }
                        /* 선형 영역 */
                    if((Farasis56AhSocRegs.NVRSocInitF < 0.0F) ||              /* NVR 무효 */
                       (Farasis56AhSocRegs.delta > 20.0F))                     /* NVR과 차이 큼 */
                    {
                        Farasis56AhSocRegs.SysSocInitF = Farasis56AhSocRegs.SOCbufF;
                    }
                    else
                    {
                        Farasis56AhSocRegs.SysSocInitF = Farasis56AhSocRegs.NVRSocInitF;
                    }
                 }
                 else
                 {
                    /* 비선형 영역 → NVR 사용, 단 NVR 무효 또는 OCV와 큰 괴리면 OCV 강제 */
                    // TODO(검증): stale NVR(예: 3.56V인데 NVR=100%) 방지 — delta>20% 시 OCV 폴백 추가
                    if((Farasis56AhSocRegs.NVRSocInitF < 0.0F) ||              /* NVR 무효 */
                       (Farasis56AhSocRegs.delta > 20.0F))                     /* NVR과 차이 큼(stale) */
                    {
                        /* NVR 무효 or OCV와 큰 괴리: OCV 결과 사용 (clamp된 0 또는 100) */
                        Farasis56AhSocRegs.SysSocInitF = Farasis56AhSocRegs.SOCbufF;
                    }
                    else
                    {
                        Farasis56AhSocRegs.SysSocInitF = Farasis56AhSocRegs.NVRSocInitF;
                    }
                 }
                //  else
                //  {
                //      /* TODO: 한글 주석 복구 필요(원본 인코딩 손상) NVR TODO: 한글 주석 복구 필요(원본 인코딩 손상) */
                //      Farasis56AhSocRegs.SysSocInitF = Farasis56AhSocRegs.NVRSocInitF;
                //  }

                 /*----------------------------------------
                  * 초기 상태 정렬 (중요)
                  *----------------------------------------*/
                 Farasis56AhSocRegs.SysAhF    = 0.0F;
                 Farasis56AhSocRegs.SysAhOldF = 0.0F;
                 Farasis56AhSocRegs.SysAhNewF = 0.0F;
                 /* 최종 SOC */
                 Farasis56AhSocRegs.SysSOCF = Farasis56AhSocRegs.SysSocInitF;
                 /*----------------------------------------
                  * 5. 상태 전이
                  *----------------------------------------*/
                 CANARegs.BAT80VStatus.bit.BATStatus=0;
                 if(SysRegs.BAT80VStateReg.bit.SysAalarm==1)
                 {
                    CANARegs.BAT80VStatus.bit.BATStatus=3;
                 }
                 if(SysRegs.BAT80VStateReg.bit.SysFault==1)
                 {
                    CANARegs.BAT80VStatus.bit.BATStatus=4;
                 }
                 SysRegs.SysMachine=System_STATE_STANDBY;
            break;
            case System_STATE_STANDBY:
                  /*--------------------------------------------------------------
                   * 260715 : STANDBY는 부팅 초기화 완료 후 대기 상태 → Init(0) 오보고 수정
                   *--------------------------------------------------------------*/
                  //CANARegs.BAT80VStatus.bit.BATStatus=0;
                  CANARegs.BAT80VStatus.bit.BATStatus=1;   // TODO : [검증] 260715_Note1, 0.12 STANDBY 구간 Ready(1) 보고
                  if(SysRegs.BAT80VStateReg.bit.SysAalarm==1)
                  {
                    CANARegs.BAT80VStatus.bit.BATStatus=3;
                  }
                  if(SysRegs.BAT80VStateReg.bit.SysFault==1)
                  {
                    CANARegs.BAT80VStatus.bit.BATStatus=4;
                  }
                  SysRegs.BAT80VStateReg.bit.SysSTATE = 1;
                  SysRegs.BAT80VStateReg.bit.CANCOMEnable=0;
                  SysRegs.BAT80VDigitalOutPutReg.bit.LEDAlarmOUT=0;
                  SysRegs.BAT80VDigitalOutPutReg.bit.LEDFaultOUT=0;
                
                  /*
                   * SChange the state machine to System_STATE_READY
                   */
                  SysRegs.BAT80VFaulBuftReg.all=0;
                  SysRegs.BAT12VFaulBuftReg.all=0;
                  SysRegs.Bat80VFaultStatecount=0;
                  SysRegs.BAT80VFaultReg.bit.CellIR_OV=0;
                  SysRegs.BAT80VFaultReg.bit.PackOcTime_Err =0;
                  SysRegs.BAT80VFaultReg.bit.PrtcOcEvent_Err =0;
                  SysRegs.BAT80VStateReg.bit.CANCOMEnable=1;
                  SysRegs.BAT80VStateReg.bit.INITOK=1;
                  NVRAllRegs.SEQ=NVRAM_AZoneSave;
                  SysRegs.SysMachine=System_STATE_READY;
                  Farasis56AhSocRegs.state= SOC_STATE_RUNNING;
            break;
            case System_STATE_READY:
                 SysRegs.BAT80VStateReg.bit.CANCOMEnable=1;
                 SysRegs.BAT80VStateReg.bit.INITOK=1;
                 SysRegs.BAT80VStateReg.bit.SysSTATE = 1;
                 SysRegs.BAT80VDigitalOutPutReg.bit.LEDAlarmOUT=0;
                 CANARegs.BAT80VStatus.bit.BATStatus=1;
                 if(SysRegs.BAT80VStateReg.bit.SysAalarm==1)
                 {
                    CANARegs.BAT80VStatus.bit.BATStatus=3;
                    SysRegs.BAT80VDigitalOutPutReg.bit.LEDAlarmOUT=1;
                 }
                 else 
                 {
                    SysRegs.BAT80VDigitalOutPutReg.bit.LEDAlarmOUT=0;   
                 }
                 if(SysRegs.BAT80VStateReg.bit.SysFault==1)
                 {
                    CANARegs.BAT80VStatus.bit.BATStatus=4;
                    SysRegs.SysMachine=System_STATE_PROTECTER;
                 }
                 if((CANARegs.PMSCMDRegs.bit.RUNStatus==1)&&(CANARegs.PMSCMDRegs.bit.PrtctReset==1))
                 {
                     PrtectRelayRegs.State.bit.WakeUpEN=0;
                 }
                 if((CANARegs.PMSCMDRegs.bit.RUNStatus==1)&&(CANARegs.PMSCMDRegs.bit.PrtctReset==0))
                 {
                     PrtectRelayRegs.State.bit.WakeUpEN=1;
                     if(SysRegs.BAT80VStateReg.bit.SysFault==1)
                     {
                         SysRegs.SysMachine=System_STATE_PROTECTER;
                         PrtectRelayRegs.State.bit.WakeUpEN=0;
                     }
                     ProtectRelayWakeUpHandle(&PrtectRelayRegs);
                     SysRegs.SysMachine=System_STATE_RUNING;
                 }
            break;
            case System_STATE_RUNING:
                  SysRegs.BAT80VStateReg.bit.CANCOMEnable=1;
                  CANARegs.BAT80VStatus.bit.BATStatus=2;
                  if(SysRegs.BAT80VStateReg.bit.SysAalarm==1)
                  {
                    CANARegs.BAT80VStatus.bit.BATStatus=3;
                    SysRegs.BAT80VDigitalOutPutReg.bit.LEDAlarmOUT=1;
                  }
                  else 
                  {
                    SysRegs.BAT80VDigitalOutPutReg.bit.LEDAlarmOUT=0;   
                  }
                  if(SysRegs.BAT80VStateReg.bit.SysFault==1)
                  {
                    CANARegs.BAT80VStatus.bit.BATStatus=4;
                    SysRegs.SysMachine=System_STATE_PROTECTER;
                  }
                 if(CANARegs.PMSCMDRegs.bit.RUNStatus==0)
                 {
                     PrtectRelayRegs.State.bit.WakeUpEN=0;
                     ProtectRelayWakeUpHandle(&PrtectRelayRegs);
                     SysRegs.SysMachine=System_STATE_READY;
                 }
            break;
            case System_STATE_PROTECTER:
                 CANARegs.BAT80VStatus.bit.BATStatus=4;
                 if(SysRegs.BAT80VStateReg.bit.SysFault==1)
                 {
                     ProtectRelayHandle(&PrtectRelayRegs);
                   // SysRegs.SysMachine=System_STATE_STANDBY;
                 }
                 if(CANARegs.PMSCMDRegs.bit.PrtctReset==1) //TODO : PrtctReset은 Res
                 {
                   //  CANARegs.PMSCMDRegs.bit.PrtctReset=0;
                     SysRegs.BAT80VFaultReg.all=0;
                     SysRegs.BAT80VFaulBuftReg.all=0;
                     SysRegs.BAT80VFaultReg.all=0;
                     SysRegs.BAT80VStateReg.bit.SysFault=0;
                     PrtectRelayRegs.State.all=0;
                     ProtectRelayVarINIT(&PrtectRelayRegs); // TODOS : 26.06.30 Fault 이후에 FCU(VCU)에서 리셋 이후에 릴레이 시퀀스 개선
                     delay_ms(200);
                     SysRegs.SysMachine=System_STATE_INIT;
                 }
                 SysRegs.BAT80VStateReg.bit.CANCOMEnable=1;
                 SysRegs.BAT80VDigitalOutPutReg.bit.LEDAlarmOUT=0;
                 SysRegs.BAT80VDigitalOutPutReg.bit.LEDFaultOUT=1;
            break;
            case System_STATE_NvrInit:
                 SysRegs.BAT80VStateReg.bit.CANCOMEnable=1;
            break;
            case System_STATE_History:

            break;
            case System_STATE_Manual:
            break;

            default :
            break;
        }
        if(SysRegs.CellVoltsampling>=CellVoltsampling100msec)
        {
            /*
             *
             */
            if(SysRegs.Bat80VCurrentAsbF<=6.0)
            {
                SysRegs.BalanceModeCount++;
                if(SysRegs.BalanceModeCount>=100)
                {
                    SysRegs.BalanceModeCount=101;
                    SysRegs.BAT80VStateReg.bit.BalanceMode=1;
                }
            }
            if(SysRegs.Bat80VCurrentAsbF>6.0)
            {
                SysRegs.BAT80VStateReg.bit.BalanceMode=0;
                SysRegs.BAT80VStateReg.bit.BalanceStatStop=0;
                SysRegs.BalanceModeCount=0;
                SysRegs.BalanceTimeCount=0;
            }
            if(SysRegs.Bat80VCellMinVoltageF<3.0)
            {
                SysRegs.BAT80VStateReg.bit.BalanceMode=0;
                SysRegs.BAT80VStateReg.bit.BalanceStatStop=0;
                SysRegs.BalanceModeCount=0;
                SysRegs.BalanceTimeCount=0;
            }
            //SysRegs.BAT80VStateReg.bit.BalanceMode=0;
            if(SysRegs.BAT80VStateReg.bit.BalanceMode==1)
            {
                SysRegs.BalanceTimeCount++;
                if(SysRegs.BalanceTimeCount>10)
                {
                   SysRegs.BAT80VStateReg.bit.BalanceStatStop = !  SysRegs.BAT80VStateReg.bit.BalanceStatStop;
                   SysRegs.BalanceTimeCount=0;
                }
            }
            if(SysRegs.BAT80VStateReg.bit.BalanceStatStop==0)
            {
                /*
                 *
                 */
                Slave1Regs.Balance.all = 0x0000;
                Slave2Regs.Balance.all = 0x0000;
              //Slave3Regs.Balance.all = 0x0000;
                SlaveBmsBalance(&Slave1Regs);
                SlaveBmsBalance(&Slave2Regs);
              //  SlaveBmsBalance(&Slave3Regs);

                Slave1Regs.ID=BMS_ID_1;
#if DebugBoardMode == 0
                SlaveVoltagHandler(&Slave1Regs);                           // TODOS 260726_Note1, 0.14 디버깅모드는 CAN(0x401~0x406) 수신으로 대체
                delay_ms(1);
                Slave2Regs.ID=BMS_ID_2;
                SlaveVoltagHandler(&Slave2Regs);                           // TODOS 260726_Note1, 0.14 디버깅모드는 CAN(0x401~0x406) 수신으로 대체
#endif
                delay_ms(1);
             //   Slave1Regs.BatICTempsF = Slave1Temps;
            //    SalveTempsVoltHandler_B(&Slave1Regs);
                delay_ms(1);
            }
            if(SysRegs.BAT80VStateReg.bit.BalanceStatStop==1)
            {
#if DebugBoardMode == 0
                Slave1Regs.ID=BMS_ID_1;
                Slave1Regs.SysCellMinVoltage = SysRegs.Bat80VCellMinVoltageF;
                SlaveVoltagBalaHandler(&Slave1Regs);
                SlaveBmsBalance(&Slave1Regs);

                Slave2Regs.ID=BMS_ID_2;
                Slave2Regs.SysCellMinVoltage = SysRegs.Bat80VCellMinVoltageF;
                SlaveVoltagBalaHandler(&Slave2Regs);
                SlaveBmsBalance(&Slave2Regs);
#endif  
            }

            SysRegs.CellVoltsampling=0;

        }
        if(SysRegs.CellTempssampling>50)
        {
#if DebugBoardMode == 0
            Slave1Regs.ID=BMS_ID_1;
            Slave1Regs.BATICDO.bit.GPIO1=1;
            SlaveBMSDigiteldoutOHandler(&Slave1Regs);
            delay_ms(5);
            SalveTempsHandler(&Slave1Regs);                                // TODOS 260726_Note1, 0.14 디버깅모드는 CAN(0x407~0x40C) 수신으로 대체
            delay_ms(1);
            Slave2Regs.ID=BMS_ID_2;
            Slave2Regs.BATICDO.bit.GPIO1=1;
            SlaveBMSDigiteldoutOHandler(&Slave2Regs);
            delay_ms(5);
            SalveTempsHandler(&Slave2Regs);                                // TODOS 260726_Note1, 0.14 디버깅모드는 CAN(0x407~0x40C) 수신으로 대체

            delay_ms(1);
            SysRegs.CellTempssampling=0;
           Slave1Regs.BatICTempsF = Slave1Temps;
           LTC6804_DieTemperatureRead(BMS_ID_1, &Slave1Temps);
#endif
        }
        if((NVRAllRegs.SEQTimeTick>100)&&(SysRegs.BAT80VStateReg.bit.INITOK==1))
        {
            NVRZoneAWRRegs.NVramtatusRegs.all=CANARegs.NVRSetRegs.all;
            if(NVRZoneAWRRegs.NVramtatusRegs.bit.NVRManual==1)
            {
                NVRZoneAWRRegs.NVramtatusRegs.bit.NVRSocInit=0;
                NVRZoneAWRRegs.NVramtatusRegs.bit.NVRforceWR=0;
                NVRZoneAWRRegs.NVramtatusRegs.bit.NVRInit=0;
                NVRAllRegs.SEQ=NVRAM_MANUALMode;

            }
            if(NVRZoneAWRRegs.NVramtatusRegs.bit.NVRSocInit==1)
            {
                NVRZoneAWRRegs.NVramtatusRegs.bit.NVRInit=0;
                NVRZoneAWRRegs.NVramtatusRegs.bit.NVRforceWR=0;
                NVRAllRegs.SEQ=NVRAM_NVRSocInit;

            }
            if(NVRZoneAWRRegs.NVramtatusRegs.bit.NVRforceWR==1)
            {
                NVRZoneAWRRegs.NVramtatusRegs.bit.NVRInit=0;
                NVRZoneAWRRegs.NVramtatusRegs.bit.NVRSocInit=0;
                NVRAllRegs.SEQ=NVRAM_NVRforceWR;
            }
            if(NVRZoneAWRRegs.NVramtatusRegs.bit.NVRInit==1)
            {
                NVRZoneAWRRegs.NVramtatusRegs.bit.NVRforceWR=0;
                NVRZoneAWRRegs.NVramtatusRegs.bit.NVRSocInit=0;
                NVRAllRegs.SEQ=NVRAM_NVRInit;
            }
            switch(NVRAllRegs.SEQ)
            {
                case NVRAM_AZoneSave :
                      NVRZoneAWRRegs.MetaVersion=Product_Version;
                      NVRZoneAWRRegs.SysTimeTick++;
                      NVRZoneAWRRegs.LastState = SysRegs.BAT80VStateReg.all;
                      NVRZoneAWRRegs.LastSOC   = (int16)(SysRegs.Bat80VSOCF * 10.0F);
                      NVRAM_AZoneSaveHandler(&NVRZoneAWRRegs);
                      NVRAllRegs.SEQ=NVRAM_AZoneRead;
                      NVRAllRegs.DebugCount++;
                break;
                case NVRAM_AZoneRead :
                     NVRAM_AZoneReadHandler(&NVRZoneARDRegs);
                     if(NVRAllRegs.DebugCount>200)
                     {
                         NVRAllRegs.DebugCount=0;
                     }
                     NVRAllRegs.SEQ=NVRAM_AZoneSave;
                break;
                case NVRAM_BZoneSave :

                break;
                case NVRAM_CZoneSave :

                break;
                case NVRAM_DZoneSave :

                break;
                case NVRAM_NVRInit :
                     NVRZoneAWRRegs.LastSOC=0;
                     NVRZoneAWRRegs.MetaVersion=0;
                     NVRZoneAWRRegs.LastState=0;
                     NVRZoneAWRRegs.LogHeadIndex=0;
                     NVRZoneAWRRegs.EventHeadIndex=0;
                     NVRZoneAWRRegs.SysTimeTick=0;
                     NVRZoneAWRRegs.LastLogTimestamp=0;
                     NVRZoneAWRRegs.LastEventTimestamp=0;
                   //  NVRAM_AZoneSaveHandler(&NVRZoneAWRRegs);
                    // SysRegs.SysMachine=System_STATE_INIT;
                     NVRAllRegs.SEQ=NVRAM_AZoneRead;
                break;
                case NVRAM_NVRforceWR :

                break;
                case NVRAM_NVRSocInit :
                    NVRZoneAWRRegs.LastSOC          = CANARegs.NVRSocInit;
                    Farasis56AhSocRegs.SysSocInitF  = CANARegs.NVRSocInit;
                    Farasis56AhSocRegs.CTCount=0;
                  //  NVRAM_AZoneSaveHandler(&NVRZoneAWRRegs);
                  //  SysRegs.SysMachine=System_STATE_READY;
                  //  NVRAllRegs.SEQ=NVRAM_AZoneRead;
                break;
                case NVRAM_MANUALMode :

                break;
                default :
                break;
            }
            NVRAllRegs.SEQTimeTick=0;
        }
        if(SysRegs.Maincount>3000){SysRegs.Maincount=0;}


        //LTC6804_DieTemperatureRead(BMS_ID_2, &Slave2Temps);
        //LTC6804_DieTemperatureRead(BMS_ID_3, &Slave3Temps);
        //randValuem =rand();// (float32) rand();
    }

}

interrupt void cpu_timer0_isr(void)
{
   static Uint16 PrtctResetOld = 0;   // TODO : [검증] 260722_Note1, 0.13 PrtctReset edge 검출용 이전값
   SysRegs.MainIsr1++;
   NVRAllRegs.SEQTimeTick++;
   SysRegs.SysRegTimer5msecCount++;
   SysRegs.SysRegTimer10msecCount++;
   SysRegs.SysRegTimer50msecCount++;
   SysRegs.SysRegTimer100msecCount++;
   SysRegs.SysRegTimer300msecCount++;
   SysRegs.SysRegTimer500msecCount++;
   SysRegs.SysRegTimer1000msecCount++;
   SysRegs.CellVoltsampling++;
   SysRegs.CellTempssampling++;
   if(SysRegs.SysRegTimer5msecCount   >SysRegTimer5msec)    {SysRegs.SysRegTimer5msecCount=0;}
   if(SysRegs.SysRegTimer10msecCount  >SysRegTimer10msec)   {SysRegs.SysRegTimer10msecCount=0;}
   if(SysRegs.SysRegTimer50msecCount  >SysRegTimer50msec)   {SysRegs.SysRegTimer50msecCount=0;}
   if(SysRegs.SysRegTimer100msecCount >SysRegTimer100msec)  {SysRegs.SysRegTimer100msecCount=0;}
   if(SysRegs.SysRegTimer300msecCount >SysRegTimer300msec)   {SysRegs.SysRegTimer300msecCount=0;}
   if(SysRegs.SysRegTimer1000msecCount>SysRegTimer1000msec)  {SysRegs.SysRegTimer1000msecCount=0;}

   /*
    * DigitalInput detection
    */
   DigitalInput(&SysRegs);
  /*
   * current sensing detection
   */
   //TODO : 전류적산 알고리즘
   if(SysRegs.BAT80VStateReg.bit.INITOK==1)
   {
       Cal80VSysCurrentHandle(&SysRegs);
       Farasis56AhSocRegs.CellAgvVoltageF = SysRegs.Bat80VCellAgvVoltageF;
       Farasis56AhSocRegs.SysSoCCTF       = SysRegs.Bat80VCurrentF;
       Farasis56AhSocRegs.SysSoCCTAbsF    = SysRegs.Bat80VCurrentAsbF;
       CalSocHandle(&Farasis56AhSocRegs);
       SysRegs.Bat80VSOCF = Farasis56AhSocRegs.SysSOCF;
   }

   /*
    * 80V Battery Alarm & Fault Check
    */
   if(SysRegs.BAT80VStateReg.bit.INITOK==1)
   {
       Cal80VSysAlarmtCheck(&SysRegs);
       if(SysRegs.BAT80VAlarmReg.all != 0)
       {
           SysRegs.BAT80VStateReg.bit.SysAalarm=1;
       }
       else
       {
           SysRegs.BAT80VStateReg.bit.SysAalarm=0;
       }
       Cal80VSysFaultCheck(&SysRegs);
       if(SysRegs.BAT80VFaultReg.all != 0)
       {
           SysRegs.BAT80VStateReg.bit.SysFault=1;
       }
   }
   else
   {
       SysRegs.BAT80VStateReg.bit.SysAalarm=0;
       SysRegs.BAT80VStateReg.bit.SysFault=0;
   }
   /*--------------------------------------------------------------
    * 260722 : PrtctReset self-clear(=0)가 READY 가드·WakeUp 가드를 무력화 →
    *          VCU가 PrtctReset을 홀드해도 INIT→RUNNING 반복되어 릴레이 토글.
    *          rising edge(0→1)에서만 리셋 1회 실행, self-clear 제거로 플래그 유지.
    *--------------------------------------------------------------*/
   //if(CANARegs.PMSCMDRegs.bit.PrtctReset==1)
   //{
   //    CANARegs.PMSCMDRegs.bit.PrtctReset=0;
   //    SysRegs.BAT80VFaultReg.all=0;
   //    SysRegs.BAT80VFaulBuftReg.all=0;
   //    SysRegs.BAT80VFaultReg.all=0;
   //    SysRegs.BAT80VStateReg.bit.SysFault=0;
   //    PrtectRelayRegs.State.all=0;          // WakeUpState/WakeUpEN/ProtectRelayCyle 리셋
   //    ProtectRelayVarINIT(&PrtectRelayRegs);
   //    delay_ms(50);
   //    SysRegs.SysMachine=System_STATE_INIT;
   //}
   if((CANARegs.PMSCMDRegs.bit.PrtctReset==1)&&(PrtctResetOld==0))   // TODO : [검증] 260722_Note1, 0.13 PrtctReset 0→1 edge에서만 리셋(self-clear 제거)
   {
       PrtectRelayRegs.State.bit.WakeUpEN=0;                 // TODO : [검증] 260722_Note1, 0.13 릴레이 ON 중이면 OFF 시퀀스 태우려 WakeUpEN=0
       ProtectRelayWakeUpHandle(&PrtectRelayRegs);           // TODO : [검증] 260722_Note1, 0.13 WakeUpState==1이면 컨택터 물리 OFF(NRlyOff→250ms→PRlyOff)
       SysRegs.BAT80VFaultReg.all=0;
       SysRegs.BAT80VFaulBuftReg.all=0;
       SysRegs.BAT80VFaultReg.all=0;
       SysRegs.BAT80VStateReg.bit.SysFault=0;
       PrtectRelayRegs.State.all=0;          // WakeUpState/WakeUpEN/ProtectRelayCyle 리셋
       ProtectRelayVarINIT(&PrtectRelayRegs);
       delay_ms(50);
       SysRegs.SysMachine=System_STATE_INIT;
   }
   PrtctResetOld = CANARegs.PMSCMDRegs.bit.PrtctReset;   // TODO : [검증] 260722_Note1, 0.13 edge 검출용 이전값 갱신(매 틱)
   switch(SysRegs.SysRegTimer5msecCount)
   {
       case 1:

       break;
       default :
       break;
   }
   switch(SysRegs.SysRegTimer10msecCount)
   {
       case 1:

       break;
       case 2:
               if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
               {
                   CANARegs.BAT80VPT = (unsigned int)(SysRegs.Bat80VVoltageF*10);
                   CANARegs.BAT80VCT = (int)(SysRegs.Bat80VCurrentF*10);
                   CANARegs.BAT80VSOC =(unsigned int)(SysRegs.Bat80VSOCF*10);
                   CANARegs.BAT80VSOH =(unsigned int)(SysRegs.Bat80VSOHF*10);
                   //At 80MHZ, operation time is 0.151msec
                   CANATX(0x601,8,CANARegs.BAT80VPT,CANARegs.BAT80VCT,CANARegs.BAT80VSOC,1000);
               }
       break;
       case 3:
               if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
               {

               }
       break;
       case 5:
               /*  
               if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
               {

                   if(SysRegs.BAT80VStateReg.bit.SysAalarm==1)
                   {
                      SysRegs.BAT80VStateReg.bit.SysSTATE=3;
                   }
                   if(SysRegs.BAT80VStateReg.bit.SysFault==1)
                   {
                      SysRegs.BAT80VStateReg.bit.SysSTATE=4;
                   }
                   CANARegs.BAT80VStatus.bit.BATStatus           = SysRegs.BAT80VStateReg.bit.SysSTATE;
                   CANARegs.BAT80VDigitalOutPutReg.bit.NRlyOUT   = PrtectRelayRegs.State.bit.NRelayDO;
                   CANARegs.BAT80VDigitalOutPutReg.bit.CHARlyOUT = PrtectRelayRegs.State.bit.PreRelayDO;
                   CANARegs.BAT80VDigitalOutPutReg.bit.PRlyOUT   = PrtectRelayRegs.State.bit.PRelayDO;
                   CANARegs.BAT80VStatus.bit.BalanceEN           = SysRegs.BAT80VStateReg.bit.BalanceMode;
                   SysRegs.BAT80VStateReg.bit.SocMode            = Farasis40AhSocRegs.SoCStateRegs.bit.CalMeth;
                   CANARegs.BAT80VAh                             = (int)(Farasis40AhSocRegs.SysAhF*10);
                   CANATX(0x602,8,CANARegs.BAT80VStatus.all,CANARegs.BAT80VDigitalOutPutReg.all,CANARegs.BAT80VAh,SysRegs.BAT80VStateReg.all);
               }
               */
                
                if(SysRegs.BAT80VStateReg.bit.SysAalarm==1)
                {
                    CANARegs.BAT80VStatus.bit.BATStatus=3;
                }
                if(SysRegs.BAT80VStateReg.bit.SysFault==1)
                {
                    CANARegs.BAT80VStatus.bit.BATStatus=4;
                }
               //SysRegs.BAT80VStateReg.bit.SysSTATE           = Slave1Regs.StateMachine;
               CANARegs.BAT80VStatus.bit.BalanceEN           = SysRegs.BAT80VStateReg.bit.BalanceMode;
               CANARegs.BAT80VDigitalOutPutReg.bit.NRlyOUT   = PrtectRelayRegs.State.bit.NRelayDO;
               CANARegs.BAT80VDigitalOutPutReg.bit.CHARlyOUT = PrtectRelayRegs.State.bit.PreRelayDO;
               CANARegs.BAT80VDigitalOutPutReg.bit.PRlyOUT   = PrtectRelayRegs.State.bit.PRelayDO;
               CANARegs.BAT80VAh                             = (int)(Farasis56AhSocRegs.SysAhF*10);
               SysRegs.BAT80VStateReg.bit.SocMode            = Farasis56AhSocRegs.SoCStateRegs.bit.CalMeth;
               SysRegs.BAT80VStateReg.bit.SysSTATE           = SysRegs.SysMachine;   // TODO : [검증] 2606.088_Note1, 0.15 상태머신 보고(Init/Ready/Running/Protecter)
               if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
               {
                 CANATX(0x602,8,CANARegs.BAT80VStatus.all,CANARegs.BAT80VDigitalOutPutReg.all,CANARegs.BAT80VAh,SysRegs.BAT80VStateReg.all);
               } 
       default :
       break;
   }
   switch(SysRegs.SysRegTimer50msecCount)
   {
       case 1:
              //LEDSysState_H;
              //At 80MHZ, operation time is 1.29usec
              #if DebugBoardMode == 0
              memcpy(&SysRegs.Bat80VCellVoltageF[0],     &Slave1Regs.CellVoltageF[0],sizeof(float32)*11);
              memcpy(&SysRegs.Bat80VCellVoltageF[11],    &Slave2Regs.CellVoltageF[0],sizeof(float32)*11);
              #endif 
              #if DebugBoardMode != 0

              DbgRegs.CellVoltF[0]  = (float32)(DbgRegs.CellVolt[0]*0.001);    // TODOS 260726_Note1, 0.14 셀 01 mV to V
              DbgRegs.CellVoltF[1]  = (float32)(DbgRegs.CellVolt[1]*0.001);    // TODOS 260726_Note1, 0.14 셀 02 mV to V
              DbgRegs.CellVoltF[2]  = (float32)(DbgRegs.CellVolt[2]*0.001);    // TODOS 260726_Note1, 0.14 셀 03 mV to V
              DbgRegs.CellVoltF[3]  = (float32)(DbgRegs.CellVolt[3]*0.001);    // TODOS 260726_Note1, 0.14 셀 04 mV to V
              DbgRegs.CellVoltF[4]  = (float32)(DbgRegs.CellVolt[4]*0.001);    // TODOS 260726_Note1, 0.14 셀 05 mV to V
              DbgRegs.CellVoltF[5]  = (float32)(DbgRegs.CellVolt[5]*0.001);    // TODOS 260726_Note1, 0.14 셀 06 mV to V
              DbgRegs.CellVoltF[6]  = (float32)(DbgRegs.CellVolt[6]*0.001);    // TODOS 260726_Note1, 0.14 셀 07 mV to V
              DbgRegs.CellVoltF[7]  = (float32)(DbgRegs.CellVolt[7]*0.001);    // TODOS 260726_Note1, 0.14 셀 08 mV to V
              DbgRegs.CellVoltF[8]  = (float32)(DbgRegs.CellVolt[8]*0.001);    // TODOS 260726_Note1, 0.14 셀 09 mV to V
              DbgRegs.CellVoltF[9]  = (float32)(DbgRegs.CellVolt[9]*0.001);    // TODOS 260726_Note1, 0.14 셀 10 mV to V
              DbgRegs.CellVoltF[10] = (float32)(DbgRegs.CellVolt[10]*0.001);   // TODOS 260726_Note1, 0.14 셀 11 mV to V
              DbgRegs.CellVoltF[11] = (float32)(DbgRegs.CellVolt[11]*0.001);   // TODOS 260726_Note1, 0.14 셀 12 mV to V
              DbgRegs.CellVoltF[12] = (float32)(DbgRegs.CellVolt[12]*0.001);   // TODOS 260726_Note1, 0.14 셀 13 mV to V
              DbgRegs.CellVoltF[13] = (float32)(DbgRegs.CellVolt[13]*0.001);   // TODOS 260726_Note1, 0.14 셀 14 mV to V
              DbgRegs.CellVoltF[14] = (float32)(DbgRegs.CellVolt[14]*0.001);   // TODOS 260726_Note1, 0.14 셀 15 mV to V
              DbgRegs.CellVoltF[15] = (float32)(DbgRegs.CellVolt[15]*0.001);   // TODOS 260726_Note1, 0.14 셀 16 mV to V
              DbgRegs.CellVoltF[16] = (float32)(DbgRegs.CellVolt[16]*0.001);   // TODOS 260726_Note1, 0.14 셀 17 mV to V
              DbgRegs.CellVoltF[17] = (float32)(DbgRegs.CellVolt[17]*0.001);   // TODOS 260726_Note1, 0.14 셀 18 mV to V
              DbgRegs.CellVoltF[18] = (float32)(DbgRegs.CellVolt[18]*0.001);   // TODOS 260726_Note1, 0.14 셀 19 mV to V
              DbgRegs.CellVoltF[19] = (float32)(DbgRegs.CellVolt[19]*0.001);   // TODOS 260726_Note1, 0.14 셀 20 mV to V
              DbgRegs.CellVoltF[20] = (float32)(DbgRegs.CellVolt[20]*0.001);   // TODOS 260726_Note1, 0.14 셀 21 mV to V
              DbgRegs.CellVoltF[21] = (float32)(DbgRegs.CellVolt[21]*0.001);   // TODOS 260726_Note1, 0.14 셀 22 mV to V
              memcpy(&SysRegs.Bat80VCellVoltageF[0],    &DbgRegs.CellVoltF[0],sizeof(float32)*22);
              #endif 

              //LEDSysState_L;
       break;
       case 5:

       break;
       case 10:
               //LEDSysState_H;
               //At 80MHZ, operation time is 33usec
               Cal80VSysVoltageHandle(&SysRegs);
               //LEDSysState_L;
       break;
       case 20:
               //LEDSysState_H;
               //At 80MHZ, operation time is 33usec

               //LEDSysState_L;
       break;
       case 30 :
               #if DebugBoardMode == 0
               memcpy(&CANARegs.BAT80VoltageCell[0],     &Slave1Regs.CellVoltage[0],sizeof(Uint16)*11);
               memcpy(&CANARegs.BAT80VoltageCell[11],    &Slave2Regs.CellVoltage[0],sizeof(Uint16)*11);
               #endif 
               #if DebugBoardMode != 0
                memcpy(&CANARegs.BAT80VoltageCell[0],     &DbgRegs.CellVolt[0],sizeof(Uint16)*22);
               #endif

       break;
       case 40 :

       break;
       default :
       break;
   }

   switch(SysRegs.SysRegTimer100msecCount)
   {
       case 5:
                //At 80MHZ, operation time is 0.151msec
                /*--------------------------------------------------------------
                 * 260715 : 통신 복구 시 통신알람 자동 해제(auto-clear) 추가.
                 *          기존은 PackFcu_CANErr이 latch되어 VCU 통신 복구돼도
                 *          재부팅(SysVarINIT) 전까지 알람이 유지되던 문제
                 *--------------------------------------------------------------*/
                SysRegs.SysCanRxCount++;
                if(SysRegs.SysCanRxCount>=10)
                {
                    //CANARegs.PMSCMDRegs.bit.RUNStatus=0;       // TODO : [검증] 260715_Note1, 0.12 PackFcu_CANErr 발생해도 RUN 유지(파워릴레이 유지)
                    /*--------------------------------------------------------------
                     * 260808 : PackFcu_CANErr 검출을 버전별 분기
                     *          VER<16(0.15) → Fault(차단), VER>=16 → Alarm(경고)
                     *--------------------------------------------------------------*/
                #if (Product_Version < 16)
                    SysRegs.BAT80VFaultReg.bit.PackFcu_CANErr=1;   // TODO : [검증] 260808_Note1, 0.15 VER15 통신끊김 Fault(차단)
                #else
                    SysRegs.BAT80VAlarmReg.bit.PackFcu_CANErr=1;   // VER16+ 통신끊김 Alarm(경고)
                #endif
                    SysRegs.SysCanRxCount=11000;
                }
                else
                {
                #if (Product_Version < 16)
                    SysRegs.BAT80VFaultReg.bit.PackFcu_CANErr=0;
                #else
                    SysRegs.BAT80VAlarmReg.bit.PackFcu_CANErr=0;   // TODO : [검증] 260715_Note1, 0.12 수신워치독<10(통신정상)이면 통신알람 해제
                #endif
                }
       break;
       case 8:
                //At 80MHZ, operation time is 0.151msec
               if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
               {
                   CANATX(0x603,8,SysRegs.BAT80VAlarmReg.all,SysRegs.BAT80VFaultReg.Word.DataL,SysRegs.BAT80VFaultReg.Word.DataH,0X0000);
               }
       break;
       case 11:
                //At 80MHZ, operation time is 0.151msec
               if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
               {
                 SysRegs.Bat80VCHAPWRContintyF    =  12.2;
                 SysRegs.Bat80VDisCHAPWRContintyF =  8.6;
                 SysRegs.Bat80VCHAPWRPeakF = 20.2;
                 SysRegs.Bat80VDisCHAPWRPeakF = 12.2;
                 CANARegs.BAT80VCHAPWRContinty    = (unsigned int)(SysRegs.Bat80VCHAPWRContintyF*10);
                 CANARegs.BAT80VDisCHAPWRContinty = (unsigned int)(SysRegs.Bat80VDisCHAPWRContintyF*10);
                 CANARegs.BAT80VCHAPWRPeak        = (unsigned int)(SysRegs.Bat80VCHAPWRPeakF*10);
                 CANARegs.BAT80VDisCHAPWRPeak     = (unsigned int)(SysRegs.Bat80VDisCHAPWRPeakF*10);
                 CANATX(0x604,8,CANARegs.BAT80VCHAPWRContinty,CANARegs.BAT80VDisCHAPWRContinty,CANARegs.BAT80VCHAPWRPeak,CANARegs.BAT80VDisCHAPWRPeak);
               }
       break;
       case 14:
           //At 80MHZ, operation time is 0.151msec
                if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
                {
                   CANARegs.BAT80VoltageMax = (unsigned int)(SysRegs.Bat80VCellMaxVoltageF*1000);
                   CANARegs.BAT80VoltageMin = (unsigned int)(SysRegs.Bat80VCellMinVoltageF*1000);
                   CANARegs.BAT80VoltageAgv = (unsigned int)(SysRegs.Bat80VCellAgvVoltageF*1000);
                   CANARegs.BAT80VoltageDiv = (unsigned int)(SysRegs.Bat80VCellDivVoltageF*1000);
                   CANATX(0x605,8,CANARegs.BAT80VoltageMax,CANARegs.BAT80VoltageMin,CANARegs.BAT80VoltageAgv,CANARegs.BAT80VoltageDiv);
                }
       break;
       case 17:
                if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
                {
                    CANARegs.BAT80VTemperaturelMAX    = (int)(SysRegs.Bat80VCellMaxTemperatureF*10);
                    CANARegs.BAT80VTemperaturelMIN    = (int)(SysRegs.Bat80VCellMinTemperatureF*10);
                    CANARegs.BAT80VTemperatureAVG     = (int)(SysRegs.Bat80VCellAgvTemperatureF*10);
                    CANARegs.BAT80VTemperatureDiv     = (int)(SysRegs.Bat80VCellDivTemperatureF*10);
                    CANATX(0x606,8,CANARegs.BAT80VTemperaturelMAX,CANARegs.BAT80VTemperaturelMIN,CANARegs.BAT80VTemperatureAVG,CANARegs.BAT80VTemperatureDiv);
                }
       break;
       case 20:
                if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
                {
                   CANARegs.BAT80VBatIRlMAX=50;
                   CANARegs.BAT80VBatIRlMIN=40;
                   CANARegs.BAT80VBatIRAVG=45;
                   CANARegs.BAT80VBatIRDiv=CANARegs.BAT80VBatIRlMAX-CANARegs.BAT80VBatIRlMIN;
                   CANATX(0x607,8,CANARegs.BAT80VBatIRlMAX,CANARegs.BAT80VBatIRlMIN,CANARegs.BAT80VBatIRAVG,CANARegs.BAT80VBatIRDiv);
                }
       break;
       case 23:
                if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
                {
                    if(Slave1Regs.ErrorCount>200)
                    {
                        Slave1Regs.ErrorCount=0;
                    }
                    if(Slave2Regs.ErrorCount>200)
                    {
                        Slave2Regs.ErrorCount=0;
                    }
                    CANARegs.CANTxA = ComBine(CANARegs.MailBox0RxCount,CANARegs.MailBoxRxCount);    // TODO : [검증] 260808_Note1, 0.15 CAN RX 전체, 전류센서 카운터
                    CANARegs.CANTxB = ComBine(SysRegs.SysCanRxCount,CANARegs.MailBox2RxCount);      // TODO : [검증] 260808_Note1, 0.15 CAN FCU 카운터,SysRegs.SysCanRxCount(리셋 카운터값)
                    CANARegs.CANTxC = ComBine(Slave2Regs.ErrorCount,Slave1Regs.ErrorCount);         // TODO : [검증] 260808_Note1, 0.15 Slave2,Slave1 에러 카운터
                    CANARegs.CANTxD = 0;
                    CANATX(0x608,8,CANARegs.CANTxA,CANARegs.CANTxB,CANARegs.CANTxC, CANARegs.CANTxD);
                    /*--------------------------------------------------------------
                     * 260808 : AlarmNum 미사용(write-only) 제거 예정
                     *--------------------------------------------------------------*/
                    //CANARegs.AlarmNum +=1;              // TODO : [삭제] 260808_Note1, 0.15 미참조 dead
                    //if(CANARegs.AlarmNum>=13)
                    //{
                    //    CANARegs.AlarmNum =0;
                    //}
                }
       break;
       case 26:

       break;
       case 30:
               if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
               {
                   /*--------------------------------------------------------------
                    * 260715 : 0x609 셀전압 송신 버그 수정 (22셀 순환 전송)
                    *   - 읽기 인덱스가 온도용 CellIRTxNum 공유 → 전용 CellVoltTxNum 사용
                    *   - 인덱스 증가(+=3) 누락 보완, 리셋 변수 CellIRTxNum → CellVoltTxNum
                    *--------------------------------------------------------------*/
                   //CANARegs.CellVoltTxA = CANARegs.BAT80VoltageCell[CANARegs.CellIRTxNum+0];
                   //CANARegs.CellVoltTxB = CANARegs.BAT80VoltageCell[CANARegs.CellIRTxNum+1];
                   //CANARegs.CellVoltTxC = CANARegs.BAT80VoltageCell[CANARegs.CellIRTxNum+2];
                   CANARegs.CellVoltTxA = (CANARegs.CellVoltTxNum+0<22) ? CANARegs.BAT80VoltageCell[CANARegs.CellVoltTxNum+0] : 0;   // TODO : [검증] 260715_Note1, 0.11 22셀 전압 0x609 송신
                   CANARegs.CellVoltTxB = (CANARegs.CellVoltTxNum+1<22) ? CANARegs.BAT80VoltageCell[CANARegs.CellVoltTxNum+1] : 0;
                   CANARegs.CellVoltTxC = (CANARegs.CellVoltTxNum+2<22) ? CANARegs.BAT80VoltageCell[CANARegs.CellVoltTxNum+2] : 0;
                   CANATX(0x609,8,CANARegs.CellVoltTxNum,CANARegs.CellVoltTxA ,CANARegs.CellVoltTxB, CANARegs.CellVoltTxC );
                   CANARegs.CellVoltTxNum +=3;
                   if(CANARegs.CellVoltTxNum>=22)
                   {
                       CANARegs.CellVoltTxNum =0;
                   }
               }
       break;
       case 35:
                 /*--------------------------------------------------------------
                  * 260715 : case 35 미사용 코드 정리 (NumB write-only, 빈 if 블록)
                  *   검증 후 제거 예정
                  *--------------------------------------------------------------*/
                 //SysRegs.NumA=SysRegs.MainIsr1/300;                // TODO : [삭제] 260715_Note1, 0.11 NumB 미참조 dead
                 //SysRegs.NumB=(float32)(SysRegs.NumA*0.1);
                 //if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
                 //{
                 //}
       break;
       case 38:
               if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
               {
               }

       break;
       case 45:
               if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
               {

               }

       break;
       case 50 :
                if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
                {

                    CANARegs.CellTempsTxA = (CANARegs.CellTempsTxNum+0<22) ? CANARegs.BAT80VTemperatureCell[CANARegs.CellTempsTxNum+0] : 0;
                    CANARegs.CellTempsTxB = (CANARegs.CellTempsTxNum+1<22) ? CANARegs.BAT80VTemperatureCell[CANARegs.CellTempsTxNum+1] : 0;
                    CANARegs.CellTempsTxC = (CANARegs.CellTempsTxNum+2<22) ? CANARegs.BAT80VTemperatureCell[CANARegs.CellTempsTxNum+2] : 0;
                    CANATX(0x60A,8,CANARegs.CellTempsTxNum,CANARegs.CellTempsTxA ,CANARegs.CellTempsTxB, CANARegs.CellTempsTxC);
                    CANARegs.CellTempsTxNum +=3;
                    if(CANARegs.CellTempsTxNum>=22)
                    {
                        CANARegs.CellTempsTxNum =0;
                    }
                }
       break;
       case 55 :
                if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
                {

                }
       break;
       default:

       break;
   }

   switch(SysRegs.SysRegTimer300msecCount)
   {

       case 50:

       break;

       case 100:
                #if DebugBoardMode == 0
                memcpy(&SysRegs.Bat80VCellTemperatureF[0],     &Slave1Regs.CellTemperatureF[0],sizeof(float32)*11);
                memcpy(&SysRegs.Bat80VCellTemperatureF[11],    &Slave2Regs.CellTemperatureF[0],sizeof(float32)*11);
                #endif
                #if DebugBoardMode != 0
                DbgRegs.CellTempF[0]=(float32)(DbgRegs.CellTemp[0]*0.1);   // TODOS 260726_Note1, 0.14 셀 01 0.1℃ to ℃
                DbgRegs.CellTempF[1]=(float32)(DbgRegs.CellTemp[1]*0.1);   // TODOS 260726_Note1, 0.14 셀 02 0.1℃ to ℃
                DbgRegs.CellTempF[2]=(float32)(DbgRegs.CellTemp[2]*0.1);   // TODOS 260726_Note1, 0.14 셀 03 0.1℃ to ℃
                DbgRegs.CellTempF[3]=(float32)(DbgRegs.CellTemp[3]*0.1);   // TODOS 260726_Note1, 0.14 셀 04 0.1℃ to ℃
                DbgRegs.CellTempF[4]=(float32)(DbgRegs.CellTemp[4]*0.1);   // TODOS 260726_Note1, 0.14 셀 05 0.1℃ to ℃
                DbgRegs.CellTempF[5]=(float32)(DbgRegs.CellTemp[5]*0.1);   // TODOS 260726_Note1, 0.14 셀 06 0.1℃ to ℃
                DbgRegs.CellTempF[6]=(float32)(DbgRegs.CellTemp[6]*0.1);   // TODOS 260726_Note1, 0.14 셀 07 0.1℃ to ℃
                DbgRegs.CellTempF[7]=(float32)(DbgRegs.CellTemp[7]*0.1);   // TODOS 260726_Note1, 0.14 셀 08 0.1℃ to ℃
                DbgRegs.CellTempF[8]=(float32)(DbgRegs.CellTemp[8]*0.1);   // TODOS 260726_Note1, 0.14 셀 09 0.1℃ to ℃
                DbgRegs.CellTempF[9]=(float32)(DbgRegs.CellTemp[9]*0.1);   // TODOS 260726_Note1, 0.14 셀 10 0.1℃ to ℃
                DbgRegs.CellTempF[10]=(float32)(DbgRegs.CellTemp[10]*0.1); // TODOS 260726_Note1, 0.14 셀 11 0.1℃ to ℃
                DbgRegs.CellTempF[11]=(float32)(DbgRegs.CellTemp[11]*0.1); // TODOS 260726_Note1, 0.14 셀 12 0.1℃ to ℃
                DbgRegs.CellTempF[12]=(float32)(DbgRegs.CellTemp[12]*0.1); // TODOS 260726_Note1, 0.14 셀 13 0.1℃ to ℃
                DbgRegs.CellTempF[13]=(float32)(DbgRegs.CellTemp[13]*0.1); // TODOS 260726_Note1, 0.14 셀 14 0.1℃ to ℃
                DbgRegs.CellTempF[14]=(float32)(DbgRegs.CellTemp[14]*0.1); // TODOS 260726_Note1, 0.14 셀 15 0.1℃ to ℃
                DbgRegs.CellTempF[15]=(float32)(DbgRegs.CellTemp[15]*0.1); // TODOS 260726_Note1, 0.14 셀 16 0.1℃ to ℃
                DbgRegs.CellTempF[16]=(float32)(DbgRegs.CellTemp[16]*0.1); // TODOS 260726_Note1, 0.14 셀 17 0.1℃ to ℃
                DbgRegs.CellTempF[17]=(float32)(DbgRegs.CellTemp[17]*0.1); // TODOS 260726_Note1, 0.14 셀 18 0.1℃ to ℃
                DbgRegs.CellTempF[18]=(float32)(DbgRegs.CellTemp[18]*0.1); // TODOS 260726_Note1, 0.14 셀 19 0.1℃ to ℃
                DbgRegs.CellTempF[19]=(float32)(DbgRegs.CellTemp[19]*0.1); // TODOS 260726_Note1, 0.14 셀 20 0.1℃ to ℃
                DbgRegs.CellTempF[20]=(float32)(DbgRegs.CellTemp[20]*0.1); // TODOS 260726_Note1, 0.14 셀 21 0.1℃ to ℃
                DbgRegs.CellTempF[21]=(float32)(DbgRegs.CellTemp[21]*0.1); // TODOS 260726_Note1, 0.14 셀 22 0.1℃ to ℃
                memcpy(&SysRegs.Bat80VCellTemperatureF[0],     &DbgRegs.CellTempF[0],sizeof(float32)*22);
                #endif
       break;

       case 120:

       break;

       case 150:
                #if DebugBoardMode == 0
                memcpy(&CANARegs.BAT80VTemperatureCell[0],     &Slave1Regs.CellTemperature[0],sizeof(int)*11);
                memcpy(&CANARegs.BAT80VTemperatureCell[11],    &Slave2Regs.CellTemperature[0],sizeof(int)*11);
                #endif
                #if DebugBoardMode != 0
                memcpy(&CANARegs.BAT80VTemperatureCell[0],     &DbgRegs.CellTemp[0],sizeof(int)*22);
                #endif

       break;   

       case 200:
                Cal80VSysTemperatureHandle(&SysRegs);
       break;

       case 250:
       break;
       default :
       break;
   }
   if(SysRegs.SysRegTimer500msecCount >SysRegTimer500msec)
   {
       SysRegs.SysRegTimer500msecCount=0;
   }
   switch(SysRegs.SysRegTimer1000msecCount)
   {
       case 1:

               if(SysRegs.BAT80VStateReg.bit.CANCOMEnable==1)
               {
                 //  CANARegs.SwVerProducttype.byte.BYTEL = Product_Type;
                 //  CANARegs.SwVerProducttype.byte.BYTEH = Product_Version;
                   CANARegs.SWTypeVer        = ComBine(Product_Version,Product_Type); 
                   CANARegs.BAT80VConfing    = ComBine(Product_SysCellVauleP,Product_SysCellVauleS);   
                   CANATX(0x600,8,CANARegs.SWTypeVer,Product_Voltage,Product_Capacity,CANARegs.BAT80VConfing);
               }

       break;
       default :
       break;
   }
   
   PrtectRelayRegs.SysFanMaxTempF=SysRegs.Bat80VCellMaxTemperatureF;
   Bat80V_FanControlHandle(&PrtectRelayRegs);
   PWRHoldHandle(&SysRegs);
   DigitalOutput(&SysRegs);

   //
    //InitECan();
   // Acknowledge this interrupt to receive more interrupts from group 1

   if(SysRegs.MainIsr1>3000) {SysRegs.MainIsr1=0;}
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
interrupt void ISR_CANRXINTA(void)
{
   // struct ECAN_REGS ECanaShadow;
    if(ECanaRegs.CANGIF0.bit.GMIF0 == 1)
    {
        
        CANARegs.MailBoxRxCount++;
        if(CANARegs.MailBoxRxCount>200){CANARegs.MailBoxRxCount=0;}
        if(ECanaRegs.CANRMP.bit.RMP0==1)
        {
            if(ECanaMboxes.MBOX0.MSGID.bit.STDMSGID==0x3C5)
            {
                CANARegs.MailBox0RxCount++;
                if(CANARegs.MailBox0RxCount>200){CANARegs.MailBox0RxCount=0;}
                SysRegs.Bat80VCurrentData.byte.CurrentH   = (ECanaMboxes.MBOX0.MDL.byte.BYTE0<<8)|(ECanaMboxes.MBOX0.MDL.byte.BYTE1);
                SysRegs.Bat80VCurrentData.byte.CurrentL   = (ECanaMboxes.MBOX0.MDL.byte.BYTE2<<8)|(ECanaMboxes.MBOX0.MDL.byte.BYTE3);
            }
            ECanaRegs.CANRMP.bit.RMP0 = 1;  //interrupt pending clear by writing 1
        }
        if(ECanaRegs.CANRMP.bit.RMP1==1)
        {
            if(ECanaMboxes.MBOX1.MSGID.bit.STDMSGID==0x3C3)
            {
               CANARegs.MailBox1RxCount++;
               if(CANARegs.MailBox1RxCount>200){CANARegs.MailBox1RxCount=0;}
               SysRegs.Bat12VCurrentData.byte.CurrentH   = (ECanaMboxes.MBOX1.MDL.byte.BYTE0<<8)|(ECanaMboxes.MBOX1.MDL.byte.BYTE1);
               SysRegs.Bat12VCurrentData.byte.CurrentL   = (ECanaMboxes.MBOX1.MDL.byte.BYTE2<<8)|(ECanaMboxes.MBOX1.MDL.byte.BYTE3);
      //       if(CANRXRegs.MailBox0RxCount>3000)
            }
            ECanaRegs.CANRMP.bit.RMP1 = 1;
        }
        if(ECanaRegs.CANRMP.bit.RMP2==1)
        {
            if(ECanaMboxes.MBOX2.MSGID.bit.STDMSGID==0x450)
            {
                CANARegs.MailBox2RxCount++;
                if(CANARegs.MailBox2RxCount>200){CANARegs.MailBox2RxCount=0;}
                 CANARegs.PMSCMDRegs.all      =  (ECanaMboxes.MBOX2.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX2.MDL.byte.BYTE0);
                //CANRXRegs.WORD700_1          =  (ECanaMboxes.MBOX2.MDL.byte.BYTE2<<8)|(ECanaMboxes.MBOX2.MDL.byte.BYTE3);
                //CANRXRegs.WORD700_2          =  (ECanaMboxes.MBOX2.MDH.byte.BYTE5<<8)|(ECanaMboxes.MBOX2.MDH.byte.BYTE4);
                //CANRXRegs.WORD700_3          =  (ECanaMboxes.MBOX2.MDH.byte.BYTE7<<8)|(ECanaMboxes.MBOX2.MDH.byte.BYTE6);
                SysRegs.SysCanRxCount=0;
            }
            ECanaRegs.CANRMP.bit.RMP2 = 1;
        }
        if(ECanaRegs.CANRMP.bit.RMP3==1)
        {
            if(ECanaMboxes.MBOX3.MSGID.bit.STDMSGID==0x400)
            {
                CANARegs.MailBox3RxCount++;
                if(CANARegs.MailBox3RxCount>3000){CANARegs.MailBox3RxCount=0;}
                  CANARegs.NVRSetRegs.all      =  (ECanaMboxes.MBOX3.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX3.MDL.byte.BYTE0);
                  CANARegs.NVRSocInit          =  (ECanaMboxes.MBOX3.MDL.byte.BYTE3<<8)|(ECanaMboxes.MBOX3.MDL.byte.BYTE2);
                //CANRXRegs.WORD700_2          =  (ECanaMboxes.MBOX3.MDH.byte.BYTE5<<8)|(ECanaMboxes.MBOX3.MDH.byte.BYTE4);
                //CANRXRegs.WORD700_3          =  (ECanaMboxes.MBOX3.MDH.byte.BYTE7<<8)|(ECanaMboxes.MBOX3.MDH.byte.BYTE6);
            }
            ECanaRegs.CANRMP.bit.RMP3 = 1;
        }
#if DebugBoardMode != 0
        /* 0x401~0x406 : cell voltage mV, 0x407~0x40C : cell temperature 0.1degC */
        if(ECanaRegs.CANRMP.bit.RMP4==1)
        {
            if(ECanaMboxes.MBOX4.MSGID.bit.STDMSGID==0x401)
            {
                DbgRegs.CellVolt[0]  = (ECanaMboxes.MBOX4.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX4.MDL.byte.BYTE0);   // TODOS 260726_Note1, 0.14 셀 01
                DbgRegs.CellVolt[1]  = (ECanaMboxes.MBOX4.MDL.byte.BYTE3<<8)|(ECanaMboxes.MBOX4.MDL.byte.BYTE2);   // TODOS 260726_Note1, 0.14 셀 02
                DbgRegs.CellVolt[2]  = (ECanaMboxes.MBOX4.MDH.byte.BYTE5<<8)|(ECanaMboxes.MBOX4.MDH.byte.BYTE4);   // TODOS 260726_Note1, 0.14 셀 03
                DbgRegs.CellVolt[3]  = (ECanaMboxes.MBOX4.MDH.byte.BYTE7<<8)|(ECanaMboxes.MBOX4.MDH.byte.BYTE6);   // TODOS 260726_Note1, 0.14 셀 04
                DbgRegs.VoltFlag.bit.Frame01 = 1;
                DbgRegs.VoltCnt++;
                if(DbgRegs.VoltCnt>200){DbgRegs.VoltCnt=0;}
            }
            ECanaRegs.CANRMP.bit.RMP4 = 1;
        }
        if(ECanaRegs.CANRMP.bit.RMP5==1)
        {
            if(ECanaMboxes.MBOX5.MSGID.bit.STDMSGID==0x402)
            {
                DbgRegs.CellVolt[4]  = (ECanaMboxes.MBOX5.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX5.MDL.byte.BYTE0);   // TODOS 260726_Note1, 0.14 셀 05
                DbgRegs.CellVolt[5]  = (ECanaMboxes.MBOX5.MDL.byte.BYTE3<<8)|(ECanaMboxes.MBOX5.MDL.byte.BYTE2);   // TODOS 260726_Note1, 0.14 셀 06
                DbgRegs.CellVolt[6]  = (ECanaMboxes.MBOX5.MDH.byte.BYTE5<<8)|(ECanaMboxes.MBOX5.MDH.byte.BYTE4);   // TODOS 260726_Note1, 0.14 셀 07
                DbgRegs.CellVolt[7]  = (ECanaMboxes.MBOX5.MDH.byte.BYTE7<<8)|(ECanaMboxes.MBOX5.MDH.byte.BYTE6);   // TODOS 260726_Note1, 0.14 셀 08
                DbgRegs.VoltFlag.bit.Frame02 = 1;
            }
            ECanaRegs.CANRMP.bit.RMP5 = 1;
        }
        if(ECanaRegs.CANRMP.bit.RMP6==1)
        {
            if(ECanaMboxes.MBOX6.MSGID.bit.STDMSGID==0x403)
            {
                DbgRegs.CellVolt[8]  = (ECanaMboxes.MBOX6.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX6.MDL.byte.BYTE0);   // TODOS 260726_Note1, 0.14 셀 09
                DbgRegs.CellVolt[9]  = (ECanaMboxes.MBOX6.MDL.byte.BYTE3<<8)|(ECanaMboxes.MBOX6.MDL.byte.BYTE2);   // TODOS 260726_Note1, 0.14 셀 10
                DbgRegs.CellVolt[10] = (ECanaMboxes.MBOX6.MDH.byte.BYTE5<<8)|(ECanaMboxes.MBOX6.MDH.byte.BYTE4);   // TODOS 260726_Note1, 0.14 셀 11
                DbgRegs.CellVolt[11] = (ECanaMboxes.MBOX6.MDH.byte.BYTE7<<8)|(ECanaMboxes.MBOX6.MDH.byte.BYTE6);   // TODOS 260726_Note1, 0.14 셀 12
                DbgRegs.VoltFlag.bit.Frame03 = 1;
            }
            ECanaRegs.CANRMP.bit.RMP6 = 1;
        }
        if(ECanaRegs.CANRMP.bit.RMP7==1)
        {
            if(ECanaMboxes.MBOX7.MSGID.bit.STDMSGID==0x404)
            {
                DbgRegs.CellVolt[12] = (ECanaMboxes.MBOX7.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX7.MDL.byte.BYTE0);   // TODOS 260726_Note1, 0.14 셀 13
                DbgRegs.CellVolt[13] = (ECanaMboxes.MBOX7.MDL.byte.BYTE3<<8)|(ECanaMboxes.MBOX7.MDL.byte.BYTE2);   // TODOS 260726_Note1, 0.14 셀 14
                DbgRegs.CellVolt[14] = (ECanaMboxes.MBOX7.MDH.byte.BYTE5<<8)|(ECanaMboxes.MBOX7.MDH.byte.BYTE4);   // TODOS 260726_Note1, 0.14 셀 15
                DbgRegs.CellVolt[15] = (ECanaMboxes.MBOX7.MDH.byte.BYTE7<<8)|(ECanaMboxes.MBOX7.MDH.byte.BYTE6);   // TODOS 260726_Note1, 0.14 셀 16
                DbgRegs.VoltFlag.bit.Frame04 = 1;
            }
            ECanaRegs.CANRMP.bit.RMP7 = 1;
        }
        if(ECanaRegs.CANRMP.bit.RMP8==1)
        {
            if(ECanaMboxes.MBOX8.MSGID.bit.STDMSGID==0x405)
            {
                DbgRegs.CellVolt[16] = (ECanaMboxes.MBOX8.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX8.MDL.byte.BYTE0);   // TODOS 260726_Note1, 0.14 셀 17
                DbgRegs.CellVolt[17] = (ECanaMboxes.MBOX8.MDL.byte.BYTE3<<8)|(ECanaMboxes.MBOX8.MDL.byte.BYTE2);   // TODOS 260726_Note1, 0.14 셀 18
                DbgRegs.CellVolt[18] = (ECanaMboxes.MBOX8.MDH.byte.BYTE5<<8)|(ECanaMboxes.MBOX8.MDH.byte.BYTE4);   // TODOS 260726_Note1, 0.14 셀 19
                DbgRegs.CellVolt[19] = (ECanaMboxes.MBOX8.MDH.byte.BYTE7<<8)|(ECanaMboxes.MBOX8.MDH.byte.BYTE6);   // TODOS 260726_Note1, 0.14 셀 20
                DbgRegs.VoltFlag.bit.Frame05 = 1;
            }
            ECanaRegs.CANRMP.bit.RMP8 = 1;
        }
        if(ECanaRegs.CANRMP.bit.RMP9==1)
        {
            if(ECanaMboxes.MBOX9.MSGID.bit.STDMSGID==0x406)
            {
                DbgRegs.CellVolt[20] = (ECanaMboxes.MBOX9.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX9.MDL.byte.BYTE0);   // TODOS 260726_Note1, 0.14 셀 21
                DbgRegs.CellVolt[21] = (ECanaMboxes.MBOX9.MDL.byte.BYTE3<<8)|(ECanaMboxes.MBOX9.MDL.byte.BYTE2);   // TODOS 260726_Note1, 0.14 셀 22 (BYTE4~7 Rsvd)
                DbgRegs.VoltFlag.bit.Frame06 = 1;
            }
            ECanaRegs.CANRMP.bit.RMP9 = 1;
        }
        if(ECanaRegs.CANRMP.bit.RMP10==1)
        {
            if(ECanaMboxes.MBOX10.MSGID.bit.STDMSGID==0x407)
            {
                DbgRegs.CellTemp[0]  = (int16)((ECanaMboxes.MBOX10.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX10.MDL.byte.BYTE0));   // TODOS 260726_Note1, 0.14 온도 01
                DbgRegs.CellTemp[1]  = (int16)((ECanaMboxes.MBOX10.MDL.byte.BYTE3<<8)|(ECanaMboxes.MBOX10.MDL.byte.BYTE2));   // TODOS 260726_Note1, 0.14 온도 02
                DbgRegs.CellTemp[2]  = (int16)((ECanaMboxes.MBOX10.MDH.byte.BYTE5<<8)|(ECanaMboxes.MBOX10.MDH.byte.BYTE4));   // TODOS 260726_Note1, 0.14 온도 03
                DbgRegs.CellTemp[3]  = (int16)((ECanaMboxes.MBOX10.MDH.byte.BYTE7<<8)|(ECanaMboxes.MBOX10.MDH.byte.BYTE6));   // TODOS 260726_Note1, 0.14 온도 04
                DbgRegs.TempFlag.bit.Frame01 = 1;
                DbgRegs.TempCnt++;
                if(DbgRegs.TempCnt>200){DbgRegs.TempCnt=0;}
            }
            ECanaRegs.CANRMP.bit.RMP10 = 1;
        }
        if(ECanaRegs.CANRMP.bit.RMP11==1)
        {
            if(ECanaMboxes.MBOX11.MSGID.bit.STDMSGID==0x408)
            {
                DbgRegs.CellTemp[4]  = (int16)((ECanaMboxes.MBOX11.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX11.MDL.byte.BYTE0));   // TODOS 260726_Note1, 0.14 온도 05
                DbgRegs.CellTemp[5]  = (int16)((ECanaMboxes.MBOX11.MDL.byte.BYTE3<<8)|(ECanaMboxes.MBOX11.MDL.byte.BYTE2));   // TODOS 260726_Note1, 0.14 온도 06
                DbgRegs.CellTemp[6]  = (int16)((ECanaMboxes.MBOX11.MDH.byte.BYTE5<<8)|(ECanaMboxes.MBOX11.MDH.byte.BYTE4));   // TODOS 260726_Note1, 0.14 온도 07
                DbgRegs.CellTemp[7]  = (int16)((ECanaMboxes.MBOX11.MDH.byte.BYTE7<<8)|(ECanaMboxes.MBOX11.MDH.byte.BYTE6));   // TODOS 260726_Note1, 0.14 온도 08
                DbgRegs.TempFlag.bit.Frame02 = 1;
            }
            ECanaRegs.CANRMP.bit.RMP11 = 1;
        }
        if(ECanaRegs.CANRMP.bit.RMP12==1)
        {
            if(ECanaMboxes.MBOX12.MSGID.bit.STDMSGID==0x409)
            {
                DbgRegs.CellTemp[8]  = (int16)((ECanaMboxes.MBOX12.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX12.MDL.byte.BYTE0));   // TODOS 260726_Note1, 0.14 온도 09
                DbgRegs.CellTemp[9]  = (int16)((ECanaMboxes.MBOX12.MDL.byte.BYTE3<<8)|(ECanaMboxes.MBOX12.MDL.byte.BYTE2));   // TODOS 260726_Note1, 0.14 온도 10
                DbgRegs.CellTemp[10] = (int16)((ECanaMboxes.MBOX12.MDH.byte.BYTE5<<8)|(ECanaMboxes.MBOX12.MDH.byte.BYTE4));   // TODOS 260726_Note1, 0.14 온도 11
                DbgRegs.CellTemp[11] = (int16)((ECanaMboxes.MBOX12.MDH.byte.BYTE7<<8)|(ECanaMboxes.MBOX12.MDH.byte.BYTE6));   // TODOS 260726_Note1, 0.14 온도 12
                DbgRegs.TempFlag.bit.Frame03 = 1;
            }
            ECanaRegs.CANRMP.bit.RMP12 = 1;
        }
        if(ECanaRegs.CANRMP.bit.RMP13==1)
        {
            if(ECanaMboxes.MBOX13.MSGID.bit.STDMSGID==0x40A)
            {
                DbgRegs.CellTemp[12] = (int16)((ECanaMboxes.MBOX13.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX13.MDL.byte.BYTE0));   // TODOS 260726_Note1, 0.14 온도 13
                DbgRegs.CellTemp[13] = (int16)((ECanaMboxes.MBOX13.MDL.byte.BYTE3<<8)|(ECanaMboxes.MBOX13.MDL.byte.BYTE2));   // TODOS 260726_Note1, 0.14 온도 14
                DbgRegs.CellTemp[14] = (int16)((ECanaMboxes.MBOX13.MDH.byte.BYTE5<<8)|(ECanaMboxes.MBOX13.MDH.byte.BYTE4));   // TODOS 260726_Note1, 0.14 온도 15
                DbgRegs.CellTemp[15] = (int16)((ECanaMboxes.MBOX13.MDH.byte.BYTE7<<8)|(ECanaMboxes.MBOX13.MDH.byte.BYTE6));   // TODOS 260726_Note1, 0.14 온도 16
                DbgRegs.TempFlag.bit.Frame04 = 1;
            }
            ECanaRegs.CANRMP.bit.RMP13 = 1;
        }
        if(ECanaRegs.CANRMP.bit.RMP14==1)
        {
            if(ECanaMboxes.MBOX14.MSGID.bit.STDMSGID==0x40B)
            {
                DbgRegs.CellTemp[16] = (int16)((ECanaMboxes.MBOX14.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX14.MDL.byte.BYTE0));   // TODOS 260726_Note1, 0.14 온도 17
                DbgRegs.CellTemp[17] = (int16)((ECanaMboxes.MBOX14.MDL.byte.BYTE3<<8)|(ECanaMboxes.MBOX14.MDL.byte.BYTE2));   // TODOS 260726_Note1, 0.14 온도 18
                DbgRegs.CellTemp[18] = (int16)((ECanaMboxes.MBOX14.MDH.byte.BYTE5<<8)|(ECanaMboxes.MBOX14.MDH.byte.BYTE4));   // TODOS 260726_Note1, 0.14 온도 19
                DbgRegs.CellTemp[19] = (int16)((ECanaMboxes.MBOX14.MDH.byte.BYTE7<<8)|(ECanaMboxes.MBOX14.MDH.byte.BYTE6));   // TODOS 260726_Note1, 0.14 온도 20
                DbgRegs.TempFlag.bit.Frame05 = 1;
            }
            ECanaRegs.CANRMP.bit.RMP14 = 1;
        }
        if(ECanaRegs.CANRMP.bit.RMP15==1)
        {
            if(ECanaMboxes.MBOX15.MSGID.bit.STDMSGID==0x40C)
            {
                DbgRegs.CellTemp[20] = (int16)((ECanaMboxes.MBOX15.MDL.byte.BYTE1<<8)|(ECanaMboxes.MBOX15.MDL.byte.BYTE0));   // TODOS 260726_Note1, 0.14 온도 21
                DbgRegs.CellTemp[21] = (int16)((ECanaMboxes.MBOX15.MDL.byte.BYTE3<<8)|(ECanaMboxes.MBOX15.MDL.byte.BYTE2));   // TODOS 260726_Note1, 0.14 온도 22 (BYTE4~7 Rsvd)
                DbgRegs.TempFlag.bit.Frame06 = 1;
            }
            ECanaRegs.CANRMP.bit.RMP15 = 1;
        }
#endif
    }
   // ECanaShadow.CANRMP.all = 0;
   // ECanaRegs.CANRMP.all = ECanaShadow.CANRMP.all ;
    ECanaRegs.CANGIF0.all = ECanaRegs.CANGIF0.all;
    ECanaRegs.CANGIF1.all = ECanaRegs.CANGIF1.all;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;

}//EOF


