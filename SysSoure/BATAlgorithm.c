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
    void CellP56AhSocInit(SocReg *P)
    {

        float32 OCVSocF;

        /*========================================
         * 1. 평균 셀 전압 기반 OCV 계산
         *========================================*/
        P->AVGXF      = P->CellAgvVoltageF;
        P->SOCX2InF   = P->AVGXF * P->AVGXF;
        P->SOCX1InF   = P->AVGXF;

        P->SOCX2OutF  = C_CellOCVToSOC_X2 * P->SOCX2InF;
        P->SOCX1OutF  = C_CellOCVToSOC_X1 * P->SOCX1InF;

        OCVSocF       = P->SOCX2OutF + P->SOCX1OutF + C_CellOCVToSOC_X0;

        /*========================================
         * 2. 기존 검증 보정값 반영
         *========================================*/
        if((OCVSocF >= 0.0F) && (OCVSocF < 20.0F))
        {
            OCVSocF = OCVSocF - 2.0F;
        }
        else if((OCVSocF >= 20.0F) && (OCVSocF < 40.0F))
        {
            OCVSocF = OCVSocF + 3.0F;
        }
        else if((OCVSocF >= 40.0F) && (OCVSocF < 80.0F))
        {
            OCVSocF = OCVSocF + 3.0F;
        }
        else if(OCVSocF >= 80.0F)
        {
            OCVSocF = OCVSocF - 3.0F;
        }

        /*========================================
         * 3. Clamp
         *========================================*/
        if(OCVSocF > 100.0F)
        {
            OCVSocF = 100.0F;
        }
        else if(OCVSocF < 0.0F)
        {
            OCVSocF = 0.0F;
        }

        P->SOCbufF = OCVSocF;

        /*========================================
         * 4. OCV 구간 판단
         *========================================*/
        if((P->SOCbufF >= C_SocOCVLinearMinF) && (P->SOCbufF <= C_SocOCVLinearMaxF))
        {
            P->SocInitMode = 0u;   /* 선형 구간 */
        }
        else
        {
            P->SocInitMode = 1u;   /* 평탄 구간 */
        }

        /*========================================
         * 5. 초기 SOC 결정
         *    - 선형 구간 : OCV/NVR 비교 후 선택
         *    - 평탄 구간 : NVR 사용
         *========================================*/
        P->delta = fabs(P->SOCbufF - P->NVRSocInitF);

        if(P->SocInitMode == 0u)
        {
            if(P->delta > 20.0F)
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
            P->SysSocInitF = P->NVRSocInitF;
        }

        /* NVR / 최종 초기 SOC 보호 */
        if(P->SysSocInitF > 100.0F)
        {
            P->SysSocInitF = 100.0F;
        }
        else if(P->SysSocInitF < 0.0F)
        {
            P->SysSocInitF = 0.0F;
        }

        /*========================================
         * 6. 초기 상태 정렬
         *========================================*/
        P->SysAhF      = 0.0F;
        P->SysAhOldF   = 0.0F;
        P->SysAhNewF   = 0.0F;

        P->SysSOCBufF1 = 0.0F;
        P->SysSOCBufF2 = 0.0F;
        P->SysSOCF     = P->SysSocInitF;

        P->CTCount     = 0u;
        P->SysTime     = 0u;
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

}
void CalSocHandle(SocReg *P)
{
    float32 OCVSocF;
    float32 DeltaSocF;
    float32 TargetAhF;

    P->SysTime++;
    if(P->SysTime < C_SocSamPleCount)
    {
        return;
    }
    P->SysTime = 0u;

    /*========================================
     * 1. Coulomb Counting 항상 수행
     *========================================*/
    P->SysSOCdtF   = C_CTSampleTime * C_SocCumulativeTime;
    P->SysAhNewF   = P->SysSoCCTF * P->SysSOCdtF;
    P->SysAhOldF   = P->SysAhF;
    P->SysAhF      = P->SysAhOldF + P->SysAhNewF;

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

    /*========================================
     * 2. 저전류 지속 시간 체크
     *========================================*/
    if(P->SysSoCCTAbsF <= C_SocReInitCurrentAbsF)
    {
        if(P->CTCount < C_SocReInitDelayCount)
        {
            P->CTCount++;
        }
    }
    else
    {
        P->CTCount = 0u;
    }

    /*========================================
     * 3. 모드 상태 표시
     *    1 : Coulomb Counting 운전
     *    0 : 저전류 장시간 → OCV 재판단 가능 상태
     *========================================*/
    if(P->CTCount >= C_SocReInitDelayCount)
    {
        P->SoCStateRegs.bit.CalMeth = 0u;
    }
    else
    {
        P->SoCStateRegs.bit.CalMeth = 1u;
    }

    /*========================================
     * 4. 저전류 장시간 유지 시 OCV 재판단
     *    - 선형구간 : OCV SOC 사용
     *    - 평탄구간 : 기존 연산 SOC 유지
     *========================================*/
    if(P->CTCount >= C_SocReInitDelayCount)
    {
        P->AVGXF      = P->CellAgvVoltageF;
        P->SOCX2InF   = P->AVGXF * P->AVGXF;
        P->SOCX1InF   = P->AVGXF;

        P->SOCX2OutF  = C_CellOCVToSOC_X2 * P->SOCX2InF;
        P->SOCX1OutF  = C_CellOCVToSOC_X1 * P->SOCX1InF;

        OCVSocF       = P->SOCX2OutF + P->SOCX1OutF + C_CellOCVToSOC_X0;

        if(OCVSocF > 100.0F)
        {
            OCVSocF = 100.0F;
        }
        else if(OCVSocF < 0.0F)
        {
            OCVSocF = 0.0F;
        }

        P->SOCbufF = OCVSocF;

        /* 선형/평탄 구간 판단 */
        if((OCVSocF >= C_SocOCVLinearMinF) && (OCVSocF <= C_SocOCVLinearMaxF))
        {
            P->SocInitMode = 0u;   /* 선형 구간 */
        }
        else
        {
            P->SocInitMode = 1u;   /* 평탄 구간 */
        }

        /*====================================
         * 5. 선형 구간만 OCV 재초기화
         *    평탄 구간은 기존 SOC 유지
         *====================================*/
        if(P->SocInitMode == 0u)
        {
            /* 현재 SysSOCF를 유지하면서 기준점만 OCV로 재정렬 */
            DeltaSocF = OCVSocF - P->SysSOCF;

            /* 기준점 변경 */
            P->SysSocInitF = OCVSocF;

            /* Ah 누적값을 재정렬해서 최종 SOC 점프 방지 */
            TargetAhF = (-DeltaSocF / 100.0F) / C_SocNorm;
            P->SysAhF = P->SysAhF + TargetAhF;

            if(P->SysAhF > C_SocAvailableCapacityAh)
            {
                P->SysAhF = C_SocAvailableCapacityAh;
            }
            else if(P->SysAhF < (-C_SocAvailableCapacityAh))
            {
                P->SysAhF = -C_SocAvailableCapacityAh;
            }

            P->SysAhOldF = P->SysAhF;

            /* 재계산 */
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
        }
        else
        {
            /* 평탄 구간 : 기존 연산 SOC 유지 */
        }
    }
}

