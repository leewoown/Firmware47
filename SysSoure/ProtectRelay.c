


#include "DSP28x_Project.h"
#include "parameter.h"
#include "ProtectRelay.h"
#include <stdio.h>
#include <math.h>
#include <string.h>


extern void ProtectRelaySateCheck(PrtectRelayReg *p);
extern void ProtectRelayVarINIT(PrtectRelayReg *p);
extern void ProtectOffHandle(PrtectRelayReg *p);
extern void ProtectRelayWakeUpHandle(PrtectRelayReg *p);
//extern void ProtecLatchRelayHandle(PrtectRelayReg *p);   // TODO(미사용 - 검증 후 제거 예정)
extern void Bat80V_FanControlHandle(PrtectRelayReg *p);
void ProtectRelayVarINIT(PrtectRelayReg *p)
{
    p->State.all=0;
    p->ProRleayOnTimerCount=0;
    p->ProRleayOffTimerCount=0;
    p->PRleayOnTimerCount=0;
    p->WakeUpOnTimeOutCount=0;
    p->WakeUpOffTimeOutCount=0;
    p->StateMachine=STATE_WakeUpReady;
    p->FanMachine=FAN_STATE_OFF;     // ← 추가
    p->SysFanMaxTempF=0.0F;          // ← 추가
}
void Bat80V_FanControlHandle(PrtectRelayReg *p)
{
    /* 현재 최대 온도 */
    float32 MaxTempF = p->SysFanMaxTempF;

    switch (p->FanMachine)
    {
        case FAN_STATE_OFF:
        {
            /* OFF → ON 조건 */
            if (Hyst_On(MaxTempF, 30.0))
            {
                p->FanMachine = FAN_STATE_ON;
                FAN_ON;
            }
            break;
        }

        case FAN_STATE_ON:
        {
            /* ON → OFF 조건 */
            if (Hyst_Off(MaxTempF, 28.0))
            {
                p->FanMachine = FAN_STATE_OFF;
                FAN_OFF;
            }
            break;
        }

        default:
        {
            /* 예외 방어 */
            p->FanMachine = FAN_STATE_OFF;
            FAN_OFF;
            break;
        }
    }
}

void ProtectRelaySateCheck(PrtectRelayReg *p)
{
    /*
    if(p->StateMachine== STATE_IDLE)
    {
        if((p->State.bit.PRelayDI==0)&&(p->State.bit.PRelayDI==0))
        {
            p->State.bit.RlyFaulttSate =0;
        }
        else
        {
            p->State.bit.RlyFaulttSate =1;
        }
    }
    if(p->StateMachine== STATE_WakeUpOFFHold)
    {
        if((p->State.bit.PRelayDI==0)&&(p->State.bit.PRelayDI==0))
        {
            p->State.bit.RlyFaulttSate =0;
        }
        else
        {
            p->State.bit.RlyFaulttSate =1;
        }
    }
    if(p->State.bit.RlyFaulttSate ==0)
    {
        p->StateMachine=STATE_WakeUpReady;
    }*/
}

void ProtectOffHandle(PrtectRelayReg *p)
{
    p->State.bit.PreRelayDO=1;
    delay_ms(20);
    p->State.bit.NRelayDO=0;
    delay_ms(10);
    p->State.bit.PRelayDO=0;
    delay_ms(10);
    p->State.bit.PreRelayDO=0;
    if((p->State.bit.PRelayDI==0)&&(p->State.bit.PRelayDI==0))
    {
        p->State.bit.RlyFaulttSate =0;
    }
    else
    {
        p->State.bit.RlyFaulttSate =1;
    }
}

void ProtectRelayWakeUpHandle(PrtectRelayReg *p)
{
    if(p->State.bit.WakeUpEN==1)
    {
        if(p->State.bit.WakeUpState==0)
        {

            NRlyOn;
            p->State.bit.NRelayDO=1;
            delay_ms(30);
            CHARlyOn;
            p->State.bit.PreRelayDO=1;
            delay_ms(200);
            PRlyOn;
            p->State.bit.PRelayDO =1;
            delay_ms(10);
            CHARlyOff;
            p->State.bit.PreRelayDO=0;
            p->State.bit.WakeUpState=1;
        }
    }
    if(p->State.bit.WakeUpEN==0)
    {
        if(p->State.bit.WakeUpState==1)
        {
            NRlyOff;
            p->State.bit.NRelayDO=1;
            delay_ms(250);
            PRlyOff;
            p->State.bit.PRelayDO=0;
            p->State.bit.WakeUpState=0;
        }
    }


}

void ProtectRelayHandle(PrtectRelayReg *p)
{
    if(p->State.bit.ProtectRelayCyle==0)
    {
        CHARlyOn;
        p->State.bit.PreRelayDO=1;
        delay_ms(5);
        PRlyOff;
        p->State.bit.PRelayDO =0;
        delay_ms(10);
        NRlyOff;
        CHARlyOff;
        p->State.bit.PreRelayDO=0;
        p->State.bit.NRelayDO=0;
        p->State.bit.ProtectRelayCyle=1;
    }
}
/* TODO(미사용 - 검증 후 제거 예정): 본 프로젝트에서 호출하지 않음
 * (유일 호출처 main.c 주석처리됨). GPIO20은 FAN_ON/FAN_OFF로 용도 변경됨. */
#if 0
void ProtecLatchRelayHandle(PrtectRelayReg *p)
{
    if(p->State.bit.LatchRelayOn==1)
    {
        LatchSetRlyON;
        delay_ms(50);
        LatchSetRlyOFF;
        p->State.bit.LatchRelayOn=0;
    }
    if(p->State.bit.LatchRelayOFF==1)
    {
        LatchResetRlyON;
        delay_ms(50);
        LatchResetRlyOFF;
        p->State.bit.LatchRelayOFF=0;
    }

}
#endif

