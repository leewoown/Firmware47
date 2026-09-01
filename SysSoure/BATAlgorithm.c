#include "DSP28x_Project.h"
#include "BATAlgorithm.h"
#include "stdio.h"
#include "math.h"
#include "string.h"

extern void CalSocRegsInit (SocReg *P);
extern void CalSocHandle(SocReg *P);

#if FarasisP40Ah
void CalFarasis40AhSocInit(SocReg *P)
{
    
    P->AVGXF   =   P->CellAgvVoltageF;
    P->SocInitMode=0;
    if      (P->AVGXF < 3.049F)  { P->SOCbufF  = 0.0F; }
    else if (P->AVGXF < 3.183F)  { P->SOCbufF  = 0.0F  + (P->AVGXF  - 3.049F)  * 37.313F;}   /* 00~05% 보간 */
    else if (P->AVGXF < 3.221F)  { P->SOCbufF  = 5.0F  + (P->AVGXF  - 3.183F)  * 131.5789F;}  /* 05~10% 보간 */
    else if (P->AVGXF < 3.246F)  { P->SOCbufF  = 10.0F + (P->AVGXF  - 3.221F)  * 200.0F;}    /* 10~15% 보간 */
    else if (P->AVGXF < 3.262F)  { P->SOCbufF  = 15.0F + (P->AVGXF  - 3.246F)  * 312.5F;}    /* 15~20% 보간 */
    else if (P->AVGXF < 3.279F)  { P->SOCbufF  = 20.0F + (P->AVGXF  - 3.262F)  * 294.117F;}  /* 20~25% 보간 */
    else if (P->AVGXF < 3.293F)  { P->SOCbufF  = 25.0F + (P->AVGXF  - 3.279F)  * 357.142F;}  /* 25~30% 보간 */
    else if (P->AVGXF < 3.299F)  { P->SOCbufF  = 30.0F + (P->AVGXF  - 3.293F) * 1666.6667F;} /* 30~40% 보간 */
    else
    {
        P->SocInitMode=1;   /* 평탄구간(40~100%) */
    }
    if(P->SocInitMode==0)
    {
        P->SysSocInitF = P->SOCbufF;
    }
    else
    {
        P->SysSocInitF = P->NVRSocInitF;
    }
}
#endif
#if FarasisP52Ah
void CalFarasis52AhSocInit(SocReg *P)
{

    // 56Ah
     P->AVGXF         =   P->CellAgvVoltageF;
     P->SOCX2InF      =   P->AVGXF  * P->AVGXF;
     P->SOCX1InF      =   P->AVGXF;
     P->SOCX2OutF     =   C_Farasis52Ah_SOCX2 * P->SOCX2InF;
     P->SOCX1OutF     =   C_Farasis52Ah_SOCX1 * P->SOCX1InF;
     P->SOCbufF       =   P->SOCX2OutF + P->SOCX1OutF + C_Farasis52Ah_SOCX0;
     /*
      *  보관법 계산식 필요함
      */
     if((P->SOCbufF >= 0.0)&&(P->SOCbufF < 20.0))
     {
         P->SOCbufF  =   P->SOCbufF-2;
     }
     if((P->SOCbufF >= 20)&&(P->SOCbufF < 40.0))
     {
         P->SOCbufF  =   P->SOCbufF+3;
     }
     if((P->SOCbufF >= 40)&&(P->SOCbufF < 80.0))
     {
         P->SOCbufF  =   P->SOCbufF+3.0;
     }
     if(P->SOCbufF >= 80.0)
     {
         P->SOCbufF  =   P->SOCbufF-3;
     }
     /*
      *
      */
     if(P->SOCbufF <=0.0)
     {
         P->SOCbufF = 0.0;
     }
     else if(P->SOCbufF > 98.0)
     {
         P->SOCbufF = 100.0;
     }
     P->SysSocInitF = P->SOCbufF;
}
#endif
#if FarasisP56Ah
    extern void CellP56AhSocInit  (SocReg *P);
    const OCVtoSOC_Point P56AhOCVTable[P56AH_OCV_TABLE_SIZE]= 
    {
        /* OCV(V)   SysSOC(%)  | 셀SOC  | DOD */
        {3.4354F,    0.00F},   /*  10%  | 90% */
        {3.4744F,    6.25F},   /*  15%  | 85% */
        {3.5215F,   12.50F},   /*  20%  | 80% */
        {3.5679F,   18.75F},   /*  25%  | 75% */
        {3.5993F,   25.00F},   /*  30%  | 70% */
        {3.6243F,   31.25F},   /*  35%  | 65% */
        {3.6487F,   37.50F},   /*  40%  | 60% */
        {3.6761F,   43.75F},   /*  45%  | 55% */
        {3.7126F,   50.00F},   /*  50%  | 50% */
        {3.7686F,   56.25F},   /*  55%  | 45% */
        {3.8324F,   62.50F},   /*  60%  | 40% */
        {3.8802F,   68.75F},   /*  65%  | 35% */
        {3.9224F,   75.00F},   /*  70%  | 30% */
        {3.9624F,   81.25F},   /*  75%  | 25% */
        {4.0127F,   87.50F},   /*  80%  | 20% */
        {4.0664F,   93.75F},   /*  85%  | 15% */
        {4.0997F,  100.00F}    /*  90%  | 10% */
    };
    float32 OCVtoSOC_P56Ah(float32 ocv)
    {
        Uint16  i;
        float32 v0, v1, s0, s1;

        if(ocv <= P56AhOCVTable[0].OCV)
        {
            return 0.0F;
        }
        if(ocv >= P56AhOCVTable[P56AH_OCV_TABLE_SIZE - 1].OCV)
        {
            return 100.0F;
        }
        for(i = 1u; i < P56AH_OCV_TABLE_SIZE; i++)
        {
            if(ocv <= P56AhOCVTable[i].OCV)
            {
                v0 = P56AhOCVTable[i - 1u].OCV;
                v1 = P56AhOCVTable[i].OCV;
                s0 = P56AhOCVTable[i - 1u].SysSOC;
                s1 = P56AhOCVTable[i].SysSOC;
                return s0 + ((s1 - s0) * (ocv - v0)) / (v1 - v0);
            }
        }
        return 100.0F;
    }
    void CellP56AhSocInit(SocReg *P)
    {
        float32 OCVSocF;
        Uint16  NVRValid;

        /*========================================
        * 1. 평균 셀 전압 → OCV → SOC
        *    Lookup Table + 선형 보간
        *    셀 SOC 10~90% = DOD 80% = 시스템 SOC 0~100%
        *    cliff 영역 정밀도 ±0.5%
        *========================================*/
        P->AVGXF   = P->CellAgvVoltageF;
        OCVSocF    = OCVtoSOC_P56Ah(P->AVGXF);
        P->SOCbufF = OCVSocF;

        /*========================================
        * 2. OCV 신뢰 영역 판단
        *    선형 (20~80%)  : OCV 신뢰 가능
        *    비선형 (양 끝) : NVR 우선
        *========================================*/
        if((P->SOCbufF >= C_SocOCVLinearMinF) && (P->SOCbufF <= C_SocOCVLinearMaxF))
        {
            P->SocInitMode = 0u;   /* 선형 */
        }
        else
        {
            P->SocInitMode = 1u;   /* 비선형 */
        }

        /*========================================
        * 3. NVR 유효성 판정
        *    호출자가 NVRSocInitF에 음수(-1.0F)를 넣으면 "무효" 마커
        *    (NVRAM 미초기화 / 손상 / 첫 부팅 등)
        *========================================*/
        if(P->NVRSocInitF < 0.0F)
        {
            NVRValid = 0u;   /* 무효 — OCV 강제 사용 */
        }
        else
        {
            NVRValid = 1u;   /* 유효 */
        }

        /*========================================
        * 4. 초기 SOC 결정
        *    선형 영역  | NVR 무효       → OCV
        *               | delta > 20%    → OCV (NVR 신뢰 불가)
        *               | delta ≤ 20%    → NVR
        *    비선형 영역| NVR 무효       → OCV (clamp 0 또는 100)
        *               | NVR 유효       → NVR
        *========================================*/
        P->delta = fabs(P->SOCbufF - P->NVRSocInitF);

        if(P->SocInitMode == 0u)
        {
            /* 선형 */
            if((NVRValid == 0u) || (P->delta > 20.0F))
            {
                P->SysSocInitF = P->SOCbufF;
            }
            else
            {
                P->SysSocInitF = P->NVRSocInitF;
            }
        }
        else
        {
            /* 비선형 */
            // TODO(완료) :NVR이 OCV와 크게 다르면(stale NVR) OCV 폴백 — delta 체크 추가
            if((NVRValid == 0u) || (P->delta > 20.0F))
            {
                P->SysSocInitF = P->SOCbufF;
            }
            else
            {
                P->SysSocInitF = P->NVRSocInitF;
            }
        }

        /*========================================
        * 5. Clamp (안전)
        *========================================*/
        if(P->SysSocInitF > 100.0F)
        {
            P->SysSocInitF = 100.0F;
        }
        else if(P->SysSocInitF < 0.0F)
        {
            P->SysSocInitF = 0.0F;
        }

        /*========================================
        * 6. 초기 적산 변수
        *========================================*/
        P->SysAhF      = 0.0F;
        P->SysAhOldF   = 0.0F;
        P->SysAhNewF   = 0.0F;
        P->SysSOCBufF1 = 0.0F;
        P->SysSOCBufF2 = 0.0F;

        /*========================================
        * 7. 운용 SOC 설정 + 카운터 초기화
        *========================================*/
        P->SysSOCF = P->SysSocInitF;
        P->CTCount = 0u;
        P->SysTime = 0u;
    }
#endif
#if Kokam100Ah
void CalKokam100AhSocInit(void)
{

}

#endif
void CalSocRegsInit(SocReg *P)
{
    P->SysSOCdtF      = 0.0F;
    P->SysSoCCTF      = 0.0F;
    P->SysSoCCTAbsF   = 0.0F;

    P->SysAhNewF      = 0.0F;
    P->SysAhOldF      = 0.0F;
    P->SysAhF         = 0.0F;

    P->SysSOCBufF1    = 0.0F;
    P->SysSOCBufF2    = 0.0F;
    P->SysSOCF        = 0.0F;

    P->NVRSocInitF    = 0.0F;
    P->SysSocInitF    = 0.0F;
    P->SOCbufF        = 0.0F;

    P->AVGXF          = 0.0F;
    P->CellAgvVoltageF= 0.0F;

    P->SOCX4InF       = 0.0F;
    P->SOCX3InF       = 0.0F;
    P->SOCX2InF       = 0.0F;
    P->SOCX1InF       = 0.0F;

    P->SOCX4OutF      = 0.0F;
    P->SOCX3OutF      = 0.0F;
    P->SOCX2OutF      = 0.0F;
    P->SOCX1OutF      = 0.0F;

    P->SOCX4InFAZore  = 0.0F;
    P->SOCX3InFAZore  = 0.0F;
    P->SOCX2InFAZore  = 0.0F;
    P->SOCX1InFAZore  = 0.0F;
    P->SOCX4OutFAZore = 0.0F;
    P->SOCX3OutFAZore = 0.0F;
    P->SOCX2OutFAZore = 0.0F;
    P->SOCX1OutFAZore = 0.0F;
    P->AZoreCalCout   = 0u;

    P->SOCX4InFBZore  = 0.0F;
    P->SOCX3InFBZore  = 0.0F;
    P->SOCX2InFBZore  = 0.0F;
    P->SOCX1InFBZore  = 0.0F;
    P->SOCX4OutFBZore = 0.0F;
    P->SOCX3OutFBZore = 0.0F;
    P->SOCX2OutFBZore = 0.0F;
    P->SOCX1OutFBZore = 0.0F;
    P->BZoreCalCout   = 0u;

    P->SOCX4InFCZore  = 0.0F;
    P->SOCX3InFCZore  = 0.0F;
    P->SOCX2InFCZore  = 0.0F;
    P->SOCX1InFCZore  = 0.0F;
    P->SOCX4OutFCZore = 0.0F;
    P->SOCX3OutFCZore = 0.0F;
    P->SOCX2OutFCZore = 0.0F;
    P->SOCX1OutFCZore = 0.0F;
    P->CZoreCalCout   = 0u;

    P->SOCX4InFDZore  = 0.0F;
    P->SOCX3InFDZore  = 0.0F;
    P->SOCX2InFDZore  = 0.0F;
    P->SOCX1InFDZore  = 0.0F;
    P->SOCX4OutFDZore = 0.0F;
    P->SOCX3OutFDZore = 0.0F;
    P->SOCX2OutFDZore = 0.0F;
    P->SOCX1OutFDZore = 0.0F;
    P->DZoreCalCout   = 0u;

    P->CTCount        = 0u;
    P->SysTime        = 0u;
    P->SocInitMode    = 0u;
    P->delta          = 0.0F;
    P->SoCStateRegs.all = 0u;
    P->state            = SOC_STATE_IDLE;

    /*--------------------------------------------------------------
     * 260831 : F-5/F-6 용 상태 초기화. VoltSettleF 는 '아직 수렴 판정 전'
     *          을 뜻하는 큰 값으로 두어, 판정창 120s 가 한 번 지나기 전에는
     *          보정이 시작되지 않도록 한다.
     *--------------------------------------------------------------*/
    P->RestVoltDiffF  = 0.0F;                                  // TODO : [검증] 260831_Note1, 0.18 F-5
    P->NvrAdopted     = 0u;

    P->CellTempF      = 25.0F;                                 // TODO : [검증] 260831_Note1, 0.18 F-6
    P->CellDivVoltF   = 0.0F;
    P->SysStateNo     = 0u;

    P->RestTimeCount  = 0u;                                    // TODO : [검증] 260831_Note1, 0.18 F-6
    P->VoltChkCount   = 0u;
    P->VoltRefF       = 0.0F;
    P->VoltSettleF    = 1000.0F;                               /* 수렴 미판정 */
    P->OcvAdjErrF     = 0.0F;
    P->OcvAdjUsedF    = 0.0F;
    P->OcvAdjActive   = 0u;
    P->OcvAdjLimitFlag= 0u;
}
void CalSocHandle(SocReg *P)
{
    /*========================================
     * 50ms 게이트 (1ms ISR × 50회)
     *========================================*/
    P->SysTime++;
    if(P->SysTime < C_SocSamPleCount)
    {
        return;
    }
    P->SysTime = 0u;

    /*========================================
     * Coulomb Counting (50ms마다 갱신)
     *   부팅 시 1회 초기화(CellP56AhSocInit) 이후, SOC는
     *   NVRAM 기준값(SysSocInitF) + 전류 적산으로만 갱신한다.
     *   운용 중 OCV 재보정 없음 — OFF→ON 시에만 재초기화.
     *   dt = 50ms × (1/3600) = 1.388e-5 hr,  ΔAh = I[A] × dt
     *========================================*/
    P->SysSOCdtF = C_CTSampleTime * C_SocCumulativeTime;

    /*----------------------------------------
     * 전류 deadband (CSNV700 노이즈 차단)
     *   |I| < 0.5A → 센서 오프셋/노이즈 영역
     *   적분 안 함 → 정지 상태 SOC 떠내려감 방지
     *   (SysSoCCTAbsF는 이미 |I| — Cal80VSysCurrentHandle에서 계산됨)
     *----------------------------------------*/
    if(P->SysSoCCTAbsF < C_SocCurrentDeadbandF)
    {
        P->SysAhNewF = 0.0F;                              /* deadband 내부 — 적분 차단 */
    }
    else
    {
        P->SysAhNewF = P->SysSoCCTF * P->SysSOCdtF;       /* 정상 적분 */
    }

    P->SysAhOldF = P->SysAhF;
    P->SysAhF    = P->SysAhOldF + P->SysAhNewF;

    if(P->SysAhF > C_SocAvailableCapacityAh)
    {
        P->SysAhF = C_SocAvailableCapacityAh;
    }
    else if(P->SysAhF < (-C_SocAvailableCapacityAh))
    {
        P->SysAhF = -C_SocAvailableCapacityAh;
    }
    P->SysSOCBufF1 = P->SysAhF * C_SocNorm;
    P->SysSOCBufF2 = P->SysSOCBufF1 * 100.0F;
    P->SysSOCF     = P->SysSocInitF + P->SysSOCBufF2;

    if(P->SysSOCF > 100.0F)
    {
        P->SysSOCF = 100.0F;
    }
    else if(P->SysSOCF < 0.0F)
    {
        P->SysSOCF = 0.0F;
    }
    /*--------------------------------------------------------------
     * 260831 : 운전 중 OCV 완만 보정 추가(F-6).
     *          기존에는 부팅 시 한 번 정한 SysSocInitF 를 끝까지 쓰기 때문에
     *          초기 오차가 영구히 남았다. 충분히 오래 쉬고 전압이 수렴한
     *          구간에서만 OCV 로 기준값을 아주 천천히 끌어당긴다.
     *          - 진입 조건(전부 AND) : 무부하 1800s, 120s 전압변화 2mV 이내,
     *            셀편차 50mV 미만, 온도 10~40도, READY 상태
     *            (READY = 상위 RUNStatus 0 → 컨택터 개방된 대기 상태.
     *             STANDBY 는 부팅 중 한 번 지나갈 뿐이라 조건에서 제외)
     *          - 보정 속도 0.01 %p/s, 데드밴드 1.0 %p, 정지밴드 0.5 %p
     *          - 1회 무부하 인터벌당 최대 5.0 %p 까지만 (OCV 오판 피해 한정)
     *          SysSocInitF 만 움직이므로 적산분(SysAhF)은 건드리지 않는다.
     *--------------------------------------------------------------*/
    //P->SoCStateRegs.bit.CalMeth = 1u;
#if FarasisP56Ah   /* OCV 표(OCVtoSOC_P56Ah)가 있는 셀에서만 동작 */
    if((P->SysSoCCTAbsF < C_SocCurrentDeadbandF) &&
       (P->SysStateNo == (Uint16)System_STATE_READY))   // TODO : [검증] 260831_Note1, 0.18 STANDBY 제외(부팅 중 1회 통과)
    {
        if(P->RestTimeCount < C_SocOcvAdjRestCount) { P->RestTimeCount++; }      // TODO : [검증] 260831_Note1, 0.18 무부하 지속시간
        P->VoltChkCount++;
        if(P->VoltChkCount >= C_SocOcvAdjVoltWinCount)
        {
            P->VoltSettleF = fabs((P->CellAgvVoltageF - P->VoltRefF) * 1000.0F); // TODO : [검증] 260831_Note1, 0.18 120s 전압 변화량[mV]
            P->VoltRefF    = P->CellAgvVoltageF;
            P->VoltChkCount = 0u;
        }
    }
    else
    {
        /* 부하가 걸리면 무부하 판정과 1회 보정량을 모두 초기화 */
        P->RestTimeCount   = 0u;                                                // TODO : [검증] 260831_Note1, 0.18 무부하 해제
        P->VoltChkCount    = 0u;
        P->VoltRefF        = P->CellAgvVoltageF;
        P->VoltSettleF     = 1000.0F;                                           /* 수렴 미판정 상태 */
        P->OcvAdjUsedF     = 0.0F;
        P->OcvAdjActive    = 0u;
        P->OcvAdjLimitFlag = 0u;
    }

    if((P->RestTimeCount  >= C_SocOcvAdjRestCount) &&
       (P->VoltSettleF    <= C_SocOcvAdjVoltSettlemV) &&
       (P->CellDivVoltF   <  C_SocOcvAdjCellDivF) &&
       (P->CellTempF      >= C_SocOcvAdjTempMinF) &&
       (P->CellTempF      <= C_SocOcvAdjTempMaxF) &&
       (P->OcvAdjLimitFlag == 0u))
    {
        P->OcvAdjErrF = OCVtoSOC_P56Ah(P->CellAgvVoltageF) - P->SysSOCF;         // TODO : [검증] 260831_Note1, 0.18 OCV 대비 오차[%p]

        if(P->OcvAdjActive == 0u)
        {
            if(fabs(P->OcvAdjErrF) > C_SocOcvAdjDeadBandF) { P->OcvAdjActive = 1u; }   /* 데드밴드 밖 → 보정 시작 */
        }
        else
        {
            if(fabs(P->OcvAdjErrF) <= C_SocOcvAdjStopBandF) { P->OcvAdjActive = 0u; }  /* 정지밴드 안 → 보정 종료 */
        }

        if(P->OcvAdjActive == 1u)
        {
            if(P->OcvAdjErrF > 0.0F) { P->SysSocInitF += C_SocOcvAdjRateF; }     // TODO : [검증] 260831_Note1, 0.18 0.01%p/s 램프
            else                     { P->SysSocInitF -= C_SocOcvAdjRateF; }

            P->OcvAdjUsedF += C_SocOcvAdjRateF;
            if(P->OcvAdjUsedF >= C_SocOcvAdjMaxPerRestF)
            {
                P->OcvAdjLimitFlag = 1u;                                         // TODO : [검증] 260831_Note1, 0.18 1회 5%p 한계 도달
                P->OcvAdjActive    = 0u;
            }
            P->SoCStateRegs.bit.CalMeth = 0u;                                    /* 보정 중에는 OCV 보정 표시 */
        }
        else
        {
            P->SoCStateRegs.bit.CalMeth = 1u;
        }
    }
    else
    {
        P->OcvAdjActive = 0u;
        P->SoCStateRegs.bit.CalMeth = 1u;                                        /* 그 외에는 Coulomb Counting */
    }
#else
    P->SoCStateRegs.bit.CalMeth = 1u;                                            /* OCV 표 없는 셀 : 기존 동작 유지 */
#endif
}

