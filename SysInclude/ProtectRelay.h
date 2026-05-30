/* ==============================================================================
System Name:  �����ڵ��� ���� ������ 80V

File Name:		PARAMETER.H

Description:	����
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

#ifndef PROTECTRELAY_H
#define PROTECTRELAY_H

#include "F2806x_Cla_typedefs.h"// F2806x CLA Type definitions
#include "F2806x_Device.h"      // F2806x Headerfile Include File
#include "F2806x_Examples.h"    // F2806x Examples Include File
#include "DSP28x_Project.h"


/*
 *
 */
#define ProRelayOnTime          50  // 50msec
#define PRelayOnTime            50  // 50msec
#define ProRelayOFFTime         250 // 250msec
#define WakeUpOnTimeOut         350*4 // 250msec
/*
 *
 */
#define PRelayOFFTime           50  //
#define WakeUpOFFTimeOutCount   50*2



typedef enum
{
  STATE_IDLE,
  STATE_WakeUpReady,
  STATE_WakeUpON,
  STATE_WakeUpOFF,
  STATE_CLEAR
}ProtectRelayState;
typedef enum
{
    FAN_STATE_OFF = 0u,
    FAN_STATE_ON  = 1u
} FanStateState;

struct ProtectRelayState_BIT
{       // bits   description
    unsigned int     WakeUpEN                    :1; // 0
    unsigned int     PRelayDI                    :1; // 1
    unsigned int     NRelayDI                    :1; // 2
    unsigned int     PreRelayDI                  :1; // 3
    unsigned int     PRelayDO                    :1; // 4
    unsigned int     NRelayDO                    :1; // 5
    unsigned int     PreRelayDO                  :1; // 6
    unsigned int     LatchRelayOn                :1; // 7
    unsigned int     LatchRelayOFF               :1; // 8
    unsigned int     ProtectRelayCyle            :1; // 9
    unsigned int     STATE09                     :1; // 10
    unsigned int     STATE10                     :1; // 11
    unsigned int     STATE11                     :1; // 12
    unsigned int     WakeUpState                 :1; // 13
    unsigned int     WakeUpSEQERR                :1; // 14
    unsigned int     RlyFaulttSate               :1; // 15
};
union ProtectRelaySate_REG
{
   unsigned int     all;
   struct ProtectRelayState_BIT bit;
};

typedef struct PrtectRelay_Date
{
    /*
     *
     */
    float32 SysFanMaxTempF;
    ProtectRelayState StateMachine;
    FanStateState FanMachine;
    Uint16 ProRleayOnTimerCount;
    Uint16 ProRleayOffTimerCount;
    Uint16 PRleayOnTimerCount;
    Uint16 WakeUpOnTimeOutCount;
    Uint16 WakeUpOffTimeOutCount;
    union  ProtectRelaySate_REG    State;


}PrtectRelayReg;
extern void ProtectRelaySateCheck(PrtectRelayReg *p);
extern void ProtectRelayVarINIT(PrtectRelayReg *p);
extern void ProtectOffHandle(PrtectRelayReg *p);
extern void ProtectRelayWakeUpHandle(PrtectRelayReg *p);
//extern void ProtecLatchRelayHandle(PrtectRelayReg *p);   // TODO(미사용 - 검증 후 제거 예정)
#endif  // end of PARAMETER.H definition


//===========================================================================
// No more.
//===========================================================================
