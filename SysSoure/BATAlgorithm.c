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
    //extern void  P56PackCurrentLimit(SystemReg *s);                                                                   // 260901: 시그니처 변경으로 폐지
    extern void    CalP56CurrentLimit(SocReg *p);                                                                       // TODO : [검증] 260901_Note1, 0.19 P56 Safety Current Limit 산출(하단 정의)
    extern float32 PackLimitLookupF(const float32 *Table, Uint16 TempIdx, float32 TempRate, Uint16 SocIdx, float32 SocRate); // TODO : [검증] 260901_Note1, 0.19 전류한계 표 bilinear 보간
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

        /*--------------------------------------------------------------
         * 260902 : F-5(휴지 중 전압 변화 판정)를 main.c 에서 이 함수로 이관.
         *          기존에는 이 함수가 SysSocInitF 를 정한 뒤 main.c 가 같은
         *          판정을 다시 해서 덮어썼고, F-5 가지는 main.c 에만 있어
         *          두 판정이 이미 어긋나 있었다. 판정을 여기 한 곳으로 모은다.
         *          차단 직전 셀전압(NvrCellVoltF)과 부팅 시 셀전압이 5mV
         *          이내면 그 사이 상태 변화가 없었다고 보고 NVR 을 이어받는다.
         *          Relaxation 미수렴 전압으로 OCV 재추정하는 것을 막는 것이
         *          목적이며, 휴지시간 계측(RTC) 없이 전압만으로 판단한다.
         *          NvrCellVoltF = 0 은 값이 없는 경우(구버전·무효)라 제외.
         *--------------------------------------------------------------*/
        P->NvrAdopted    = 0u;                                                        // TODO : [검증] 260902_Note1, 0.21 F-5 이관
        P->RestVoltDiffF = fabs((P->CellAgvVoltageF * 1000.0F) - P->NvrCellVoltF);    // TODO : [검증] 260902_Note1, 0.21 F-5 이관

        if((NVRValid == 1u) &&                                     /* NVR 유효 */
           (P->NvrCellVoltF != 0.0F) &&                            /* 저장된 전압 있음 */
           (P->RestVoltDiffF <= C_SocRestCellVoltDiffmV))          /* 전압 변화 없음 */
        {
            P->SysSocInitF = P->NVRSocInitF;                                          // TODO : [검증] 260902_Note1, 0.21 F-5 NVR 우선
            P->NvrAdopted  = 1u;                                                      // TODO : [검증] 260902_Note1, 0.21
        }
        else if(P->SocInitMode == 0u)
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
    P->NvrCellVoltF   = 0.0F;                                  // TODO : [검증] 260902_Note1, 0.21 F-5 입력 초기화
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

    /*--------------------------------------------------------------
     * 260901 : P56 안전 전류제한 I/O 초기화 (CalP56CurrentLimit 입출력).
     *          첫 100ms 갱신 전 오판정 방지로 0 으로 둔다.
     *--------------------------------------------------------------*/
    P->Bat80VCellAgvTemperatureF = 0.0F;                      // TODO : [검증] 260901_Note1, 0.19 P56 입력: 셀 평균 온도
    P->Bat80VSOCF                = 0.0F;                      // TODO : [검증] 260901_Note1, 0.19 P56 입력: 팩 SOC
    P->Bat80VCurrentF            = 0.0F;                      // TODO : [검증] 260901_Note1, 0.19 P56 입력: 팩 전류
    P->Bat80VCurrentAsbF         = 0.0F;                      // TODO : [검증] 260901_Note1, 0.19 P56 입력: 팩 전류 절대값
    /*--------------------------------------------------------------
     * 260902 : 한계 4종 초기값 0 → 5 로 변경. SysVarINIT 와 동일 기준
     *          (CAN 0x604 초기값 충전 +5.0A / 방전 -5.0A).
     *--------------------------------------------------------------*/
    //P->Bat80VDisCHAPeakFCurrF    = 0.0F;                    // TODO : [검증] 260901_Note1, 0.19 P56 출력: 5s 방전 한계
    //P->Bat80VCHAPeakFCurrF       = 0.0F;                    // TODO : [검증] 260901_Note1, 0.19 P56 출력: 5s 충전 한계
    //P->Bat80VDisCHAContintyCurrF = 0.0F;                    // TODO : [검증] 260901_Note1, 0.19 P56 출력: 연속 방전 한계
    //P->Bat80VCHAContintyCurrF    = 0.0F;                    // TODO : [검증] 260901_Note1, 0.19 P56 출력: 연속 충전 한계
    P->Bat80VDisCHAPeakFCurrF    = 5.0F;                      // TODO : [검증] 260902_Note1, 0.19 P56 출력: 5s 방전 한계 초기 -5.0A(양수 크기)
    P->Bat80VCHAPeakFCurrF       = 5.0F;                      // TODO : [검증] 260902_Note1, 0.19 P56 출력: 5s 충전 한계 초기 +5.0A
    P->Bat80VDisCHAContintyCurrF = 5.0F;                      // TODO : [검증] 260902_Note1, 0.19 P56 출력: 연속 방전 한계 초기 -5.0A(양수 크기)
    P->Bat80VCHAContintyCurrF    = 5.0F;                      // TODO : [검증] 260902_Note1, 0.19 P56 출력: 연속 충전 한계 초기 +5.0A
    P->Bat80VUnbalCurrentF       = 0.0F;                      // TODO : [검증] 260901_Note1, 0.19 P56 출력: 연속한계 초과분
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

    /*--------------------------------------------------------------
     * 260902 : 적산 windup 차단.
     *          기존에는 표시값(SysSOCF)만 0~100 으로 잘랐고 적산분
     *          (SysAhF)은 +-45Ah 까지 계속 쌓였다. 그래서 SOC 가 한계에
     *          닿은 뒤로는 반대 방향으로 전류가 흘러도, 넘치게 쌓인 양을
     *          다 되돌릴 때까지 SOC 가 움직이지 않았다.
     *          예) SysSocInitF 60% 에서 완충 후 50A 방전 시 약 32분간
     *              SOC 가 100.0% 에 고정.
     *          한계에 닿으면 그 한계에 대응하는 값으로 SysAhF 를 되돌려
     *          방향이 바뀌는 즉시 SOC 가 반응하게 한다.
     *          ※ SysAhF 는 CAN BAT80VAh 로 송신되나 검증용이라 무방.
     *--------------------------------------------------------------*/
    //if(P->SysSOCF > 100.0F)
    //{
    //    P->SysSOCF = 100.0F;
    //}
    //else if(P->SysSOCF < 0.0F)
    //{
    //    P->SysSOCF = 0.0F;
    //}

    if(P->SysSOCF > 100.0F)
    {
        P->SysSOCF   = 100.0F;
        P->SysAhF    = (100.0F - P->SysSocInitF) * 0.01F * C_SocAvailableCapacityAh;   // TODO : [검증] 260902_Note1, 0.21 상한 역산
        P->SysAhOldF = P->SysAhF;                                                      // TODO : [검증] 260902_Note1, 0.21
    }
    else if(P->SysSOCF < 0.0F)
    {
        P->SysSOCF   = 0.0F;
        P->SysAhF    = (0.0F - P->SysSocInitF) * 0.01F * C_SocAvailableCapacityAh;     // TODO : [검증] 260902_Note1, 0.21 하한 역산
        P->SysAhOldF = P->SysAhF;                                                      // TODO : [검증] 260902_Note1, 0.21
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

            /*--------------------------------------------------------------
             * 260902 : 램프 결과에 범위 제한 추가.
             *          기존에는 SysSocInitF 를 0.0005 %p 씩 올리고 내리기만
             *          하고 잘라 주지 않아 0~100 밖으로 흘러갈 수 있었다.
             *          윗단 windup 차단식이 (100 - SysSocInitF) 로 상한을
             *          역산하므로, 범위를 벗어나면 음수 상한 같은 잘못된
             *          값이 나온다.
             *--------------------------------------------------------------*/
            if(P->SysSocInitF > 100.0F)    { P->SysSocInitF = 100.0F; }          // TODO : [검증] 260902_Note1, 0.21 기준 SOC 상한
            else if(P->SysSocInitF < 0.0F) { P->SysSocInitF = 0.0F;   }          // TODO : [검증] 260902_Note1, 0.21 기준 SOC 하한

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

/*--------------------------------------------------------------
 * 260901 : P56PackCurrentLimit(+PackLimitLookupF, 한계표)를 DSP28x_Project.c 에서 이동.
 *          P56 셀 전용 → #if FarasisP56Ah 로 감쌈. // TODO : [검증] 260901_Note1, 0.19
 *--------------------------------------------------------------*/
#if FarasisP56Ah
/*--------------------------------------------------------------
 * 260827 : P56PackCurrentLimit 신규 — P56 Safety Current Limit 표를
 *          [셀온도 x 셀SOC] 2차원 선형보간(bilinear)하여 팩 허용전류 산출.
 *          출력 4종 : 5sec(Peak) 충/방전, 연속(Continuity) 충/방전. 단위 A.
 *          근거 : 20260827_P56_Safety_Current_Limit_AI.md (R1)
 *          260831 : 근거 문서 R3 로 갱신 — 20260831_P56_Safety_Current_Limit_R3.md.
 *                   연속 충전 한계 표(10~50 degC)만 변경, 나머지 3개 표는 R1 과 동일.
 *          - 22S1P 구성이라 셀 전류 = 팩 전류(병렬 1P).
 *          - (260827 폐지) 표 SOC 축은 '셀SOC'. BMS SysSOC(0~100%)는
 *            셀 10~90% 구간에 대응하므로 셀SOC = 10 + SysSOC*0.8 로 변환 후 조회.
 *          - 표 SOC 축은 SysSOC(Bat80VSOCF) 를 그대로 사용한다.
 *            Bat80VSOCF 는 이미 DOD 80%(C_SocAvailableCapacityAh 45Ah)가
 *            반영된 값이므로 별도 셀SOC 환산을 하지 않는다.
 *          - 표의 방전값은 음수지만 여기서는 크기(양수)로 저장.
 *            (Cal80VSysAlarmtCheck 가 |I| 와 비교하기 때문)
 *          - -30/-20/60도 행은 문서상 참고값(취소선)이나 실차 계측 편의를
 *            위해 표에 그대로 포함. 표 범위 밖 온도는 양끝 행으로 클램프하며,
 *            실제 차단은 팩온도 Fault(52도 / -35도)가 담당한다.
 *          ※ 미검증 : 5sec 충전 480A(8.6C) 및 저온 충전값은 셀 제조사
 *            확정 전까지 실차 적용 보류 대상(문서 '미검증 항목' 1,2).
 *          ※ 단위 : 본 함수는 A 로 채운다. CAN 0x604 규약도 kW→A 로 변경.
 *--------------------------------------------------------------*/
#define C_PackLimitTempPoint     9        /* -30, -20, -10, 0, 10, 25, 40, 50, 60 degC */
#define C_PackLimitSocPoint      11       /* 셀 SOC 0 ~ 100 %, 10 % 간격              */
//260902 : 0A 정책 보류로 미사용(가드 주석과 함께 보류). 되살릴 때 주석 해제.
//#define C_PackLimitUsableTempMinF  (-10.0F)  /* R3 Note2 이용온도 하한[degC] */
//#define C_PackLimitUsableTempMaxF  ( 55.0F)  /* R3 Note2 이용온도 상한[degC] */

const float32 PackLimitTempAxisF[C_PackLimitTempPoint] =
{
    -30.0F, -20.0F, -10.0F,   0.0F,  10.0F,  25.0F,  40.0F,  50.0F,  60.0F
};
const float32 PackLimitSocAxisF[C_PackLimitSocPoint] =
{
      0.0F,  10.0F,  20.0F,  30.0F,  40.0F,  50.0F,  60.0F,  70.0F,  80.0F,  90.0F, 100.0F
};

/* 5초 방전 한계 [A](크기) : 행 = 온도, 열 = 셀 SOC */
const float32 PackDisChaPeakLimitF[C_PackLimitTempPoint][C_PackLimitSocPoint] =
{
    {  0.0F,   0.0F,   0.0F,  16.6F,  35.5F,  35.0F,  31.1F,  44.2F,  57.4F,  50.9F,  50.9F },   /* -30 degC */
    {  0.0F,  15.3F,  58.7F,  83.0F, 106.4F, 105.0F, 108.9F, 103.2F, 114.9F, 101.8F, 101.8F },   /* -20 degC */
    {  0.0F,  30.5F,  88.0F, 165.9F, 212.8F, 227.5F, 217.8F, 221.1F, 215.4F, 203.6F, 216.4F },   /* -10 degC */
    {  0.0F,  61.1F, 161.3F, 232.3F, 283.7F, 297.5F, 295.6F, 294.7F, 301.5F, 292.7F, 292.7F },   /*   0 degC */
    {  0.0F, 106.9F, 220.0F, 315.3F, 354.7F, 385.0F, 373.3F, 383.2F, 387.7F, 394.5F, 394.5F },   /*  10 degC */
    {  0.0F, 168.0F, 308.0F, 448.0F, 500.0F, 500.0F, 500.0F, 500.0F, 500.0F, 500.0F, 500.0F },   /*  25 degC */
    {  0.0F, 168.0F, 308.0F, 448.0F, 500.0F, 500.0F, 500.0F, 500.0F, 500.0F, 500.0F, 500.0F },   /*  40 degC */
    {  0.0F, 168.0F, 308.0F, 448.0F, 500.0F, 500.0F, 500.0F, 500.0F, 500.0F, 500.0F, 500.0F },   /*  50 degC */
    {  0.0F, 114.5F, 205.3F, 298.7F, 354.7F, 350.0F, 388.9F, 368.4F, 359.0F, 381.8F, 381.8F }    /*  60 degC */
};
/* 5초 충전 한계 [A] */
const float32 PackChaPeakLimitF[C_PackLimitTempPoint][C_PackLimitSocPoint] =
{
    {   5.7F,   5.7F,   5.7F,   5.7F,   6.1F,   5.6F,   5.8F,   5.6F,   3.2F,   2.7F, 0.0F },    /* -30 degC */
    {  22.9F,  22.9F,  22.9F,  22.9F,  18.3F,  16.9F,  17.5F,  11.1F,   4.8F,   5.4F, 0.0F },    /* -20 degC */
    { 125.7F, 125.7F, 125.7F, 114.3F,  97.8F,  67.5F,  46.7F,  22.2F,  16.0F,  10.8F, 0.0F },    /* -10 degC */
    { 228.6F, 228.6F, 228.6F, 205.7F, 195.6F, 135.0F, 116.7F,  66.7F,  32.0F,  18.0F, 0.0F },    /*   0 degC */
    { 320.0F, 320.0F, 320.0F, 297.1F, 293.3F, 225.0F, 163.3F, 111.1F,  48.0F,  36.0F, 0.0F },    /*  10 degC */
    { 480.0F, 480.0F, 480.0F, 480.0F, 440.0F, 360.0F, 280.0F, 200.0F,  96.0F,  72.0F, 0.0F },    /*  25 degC */
    { 480.0F, 480.0F, 480.0F, 480.0F, 440.0F, 360.0F, 280.0F, 200.0F,  96.0F,  72.0F, 0.0F },    /*  40 degC */
    { 480.0F, 480.0F, 480.0F, 480.0F, 440.0F, 360.0F, 280.0F, 200.0F,  96.0F,  72.0F, 0.0F },    /*  50 degC */
    { 480.0F, 480.0F, 480.0F, 480.0F, 440.0F, 360.0F, 280.0F, 200.0F,  96.0F,  72.0F, 0.0F }     /*  60 degC */
};
/* 연속(120초 초과) 방전 한계 [A](크기) */
const float32 PackDisChaContLimitF[C_PackLimitTempPoint][C_PackLimitSocPoint] =
{
    { 0.0F,   0.0F,   0.0F,   5.6F,   9.0F,   9.0F,  16.8F,  16.8F,  16.8F,  16.8F,  16.8F },    /* -30 degC */
    { 0.0F,   9.0F,  22.4F,  33.6F,  33.6F,  33.6F,  33.6F,  33.6F,  33.6F,  33.6F,  33.6F },    /* -20 degC */
    { 0.0F,  11.2F,  33.6F,  33.6F,  50.4F,  50.4F,  50.4F,  50.4F,  50.4F,  50.4F,  50.4F },    /* -10 degC */
    { 0.0F,  67.2F,  84.0F,  84.0F,  84.0F,  84.0F,  84.0F,  84.0F,  84.0F,  84.0F,  84.0F },    /*   0 degC */
    { 0.0F,  89.6F, 123.2F, 134.4F, 134.4F, 134.4F, 134.4F, 134.4F, 134.4F, 134.4F, 134.4F },    /*  10 degC */
    { 0.0F,  67.2F, 123.2F, 168.0F, 168.0F, 168.0F, 201.6F, 201.6F, 201.6F, 201.6F, 201.6F },    /*  25 degC */
    { 0.0F,  67.2F, 123.2F, 168.0F, 168.0F, 168.0F, 201.6F, 201.6F, 201.6F, 201.6F, 201.6F },    /*  40 degC */
    { 0.0F,  67.2F,  84.0F,  84.0F,  84.0F,  84.0F,  84.0F,  84.0F,  84.0F,  84.0F,  84.0F },    /*  50 degC */
    { 0.0F,  56.0F,  56.0F,  56.0F,  56.0F,  56.0F,  56.0F,  56.0F,  56.0F,  56.0F,  56.0F }     /*  60 degC */
};
/* 연속 충전 한계 [A] */
const float32 PackChaContLimitF[C_PackLimitTempPoint][C_PackLimitSocPoint] =
{
    {   1.5F,   1.5F,   1.5F,   1.5F,   1.5F,   1.5F,   1.5F,   0.6F,   0.3F,   0.1F, 0.0F },    /* -30 degC */
    {   2.8F,   2.8F,   2.8F,   2.8F,   2.8F,   2.8F,   2.8F,   0.9F,   0.6F,   0.3F, 0.0F },    /* -20 degC */
    {  33.6F,  16.8F,  16.8F,   5.6F,   5.6F,   3.4F,   3.4F,   1.1F,   1.1F,   0.6F, 0.0F },    /* -10 degC */
    {  56.0F,  33.6F,  33.6F,  11.2F,  11.2F,   5.6F,   5.6F,   2.8F,   2.8F,   1.1F, 0.0F },    /*   0 degC */
    /*--------------------------------------------------------------
     * 260831 : 문서 R3 반영 — 10~50 degC 구간 연속 충전 한계 상향.
     *          SOC 50~70% = 1.3C(72.8A), SOC 0~40% 는 1.3C 이상을 유지해
     *          SOC 증가에 따라 충전 한계가 단조감소하도록 정리.
     *          근거 : 20260831_P56_Safety_Current_Limit_R3.md (Note 6)
     *--------------------------------------------------------------*/
    //{  84.0F,  84.0F,  84.0F,  39.2F,  39.2F,  28.0F,  28.0F,  28.0F,  28.0F,  28.0F, 0.0F },    /*  10 degC */
    //{ 168.0F, 168.0F, 168.0F,  78.4F,  78.4F,  56.0F,  33.6F,  33.6F,  33.6F,  33.6F, 0.0F },    /*  25 degC */
    //{ 201.6F, 201.6F, 201.6F, 201.6F, 179.2F, 134.4F,  89.6F,  67.2F,  67.2F,  67.2F, 0.0F },    /*  40 degC */
    //{  56.0F,  56.0F,  56.0F,  56.0F,  56.0F,  56.0F,  33.6F,  33.6F,  33.6F,  33.6F, 0.0F },    /*  50 degC */
    {  84.0F,  84.0F,  84.0F,  72.8F,  72.8F,  72.8F,  72.8F,  72.8F,  28.0F,  28.0F, 0.0F },    /*  10 degC */   // TODO : [검증] 260831_Note1, 0.18 R3 반영
    { 168.0F, 168.0F, 168.0F,  78.4F,  78.4F,  72.8F,  72.8F,  72.8F,  33.6F,  33.6F, 0.0F },    /*  25 degC */   // TODO : [검증] 260831_Note1, 0.18 R3 반영
    { 201.6F, 201.6F, 201.6F, 201.6F, 179.2F,  72.8F,  72.8F,  72.8F,  67.2F,  67.2F, 0.0F },    /*  40 degC */   // TODO : [검증] 260831_Note1, 0.18 R3 반영
    {  72.8F,  72.8F,  72.8F,  72.8F,  72.8F,  72.8F,  72.8F,  72.8F,  33.6F,  33.6F, 0.0F },    /*  50 degC */   // TODO : [검증] 260831_Note1, 0.18 R3 반영
    {   0.0F,   0.0F,   0.0F,   0.0F,   0.0F,   0.0F,   0.0F,   0.0F,   0.0F,   0.0F, 0.0F }     /*  60 degC */
};

/* 표 격자의 네 모서리를 SOC 먼저, 그다음 온도 순으로 보간 */
float32 PackLimitLookupF(const float32 *Table, Uint16 TempIdx, float32 TempRate, Uint16 SocIdx, float32 SocRate)
{
    float32 V00, V01, V10, V11;
    float32 LowRowF, HighRowF;

    V00 = Table[( TempIdx        * C_PackLimitSocPoint) + SocIdx      ];
    V01 = Table[( TempIdx        * C_PackLimitSocPoint) + SocIdx + 1u ];
    V10 = Table[((TempIdx + 1u)  * C_PackLimitSocPoint) + SocIdx      ];
    V11 = Table[((TempIdx + 1u)  * C_PackLimitSocPoint) + SocIdx + 1u ];

    /*--------------------------------------------------------------
     * 260902 : a+(b-a)*t 형태는 t=1 에서 b 를 정확히 돌려주지 못해
     *          격자점에서 1e-7 잔차가 남았고, CAN 정수변환(버림)과 겹쳐
     *          연속충전 -10도/SOC70%(1.1A->1.0A), -30도/SOC90%(0.1A->0.0A)
     *          두 지점에서 0.1A 오차로 드러났다.
     *          (1-t)*a + t*b 형태는 t=0/1 에서 a/b 를 정확히 반환한다.
     *--------------------------------------------------------------*/
    //LowRowF  = V00 + ((V01 - V00) * SocRate);        /* 낮은 온도 행에서 SOC 방향 보간 */
    //HighRowF = V10 + ((V11 - V10) * SocRate);        /* 높은 온도 행에서 SOC 방향 보간 */
    //return LowRowF + ((HighRowF - LowRowF) * TempRate);
    LowRowF  = (V00 * (1.0F - SocRate)) + (V01 * SocRate);          // TODO : [검증] 260902_Note1, 0.19 낮은 온도 행에서 SOC 방향 보간(격자점 정확)
    HighRowF = (V10 * (1.0F - SocRate)) + (V11 * SocRate);          // TODO : [검증] 260902_Note1, 0.19 높은 온도 행에서 SOC 방향 보간(격자점 정확)

    return (LowRowF * (1.0F - TempRate)) + (HighRowF * TempRate);   // TODO : [검증] 260902_Note1, 0.19 온도 방향 보간(격자점 정확)
}

/*--------------------------------------------------------------
 * 260901 : SystemReg *s -> SocReg *p, 함수명 P56PackCurrentLimit -> CalP56CurrentLimit.
 *          P56 전류제한 입출력을 SocReg(Farasis56AhSocRegs)로 이관.
 *          호출부(main.c)에서 SysRegs 와 SocRegs 사이 입출력 복사.
 *          본체 s-> 를 p-> 로 일괄 변경(로직 동일).
 *--------------------------------------------------------------*/
//void P56PackCurrentLimit(SystemReg *s)
void CalP56CurrentLimit(SocReg *p)
{
    Uint16  i;
    Uint16  TempIdx;
    Uint16  SocIdx;
    float32 TempRate;
    float32 SocRate;
    float32 CellTempF;
    float32 CellSocF;

    CellTempF = p->Bat80VCellAgvTemperatureF;                       // TODO : [검증] 260827_Note1, 0.16 표 온도축 입력 = 팩 평균 셀온도
    /*--------------------------------------------------------------
     * 260827 : Bat80VSOCF 에 DOD 80% 가 이미 반영되어 있어 셀SOC 환산을 폐지.
     *          SysSOC 를 표의 SOC 축으로 그대로 사용한다.
     *--------------------------------------------------------------*/
    //CellSocF  = 10.0F + (p->Bat80VSOCF * 0.8F);
    CellSocF  = p->Bat80VSOCF;                                      // TODO : [검증] 260827_Note1, 0.16 SysSOC 직접 사용(DOD80% 기반영)

    /* 표 범위로 제한, 양끝은 가장자리 행 값을 유지 */
    if(CellTempF < PackLimitTempAxisF[0])                         { CellTempF = PackLimitTempAxisF[0]; }
    if(CellTempF > PackLimitTempAxisF[C_PackLimitTempPoint - 1u]) { CellTempF = PackLimitTempAxisF[C_PackLimitTempPoint - 1u]; }
    if(CellSocF  < PackLimitSocAxisF[0])                          { CellSocF  = PackLimitSocAxisF[0]; }
    if(CellSocF  > PackLimitSocAxisF[C_PackLimitSocPoint - 1u])   { CellSocF  = PackLimitSocAxisF[C_PackLimitSocPoint - 1u]; }

    /* 온도 구간과 그 구간 안에서의 위치 비율을 구함 */
    TempIdx  = C_PackLimitTempPoint - 2u;
    TempRate = 1.0F;
    for(i = 1u; i < C_PackLimitTempPoint; i++)
    {
        if(CellTempF <= PackLimitTempAxisF[i])
        {
            TempIdx  = i - 1u;
            TempRate = (CellTempF - PackLimitTempAxisF[i - 1u]) / (PackLimitTempAxisF[i] - PackLimitTempAxisF[i - 1u]);
            break;
        }
    }
    /* SOC 구간도 같은 방식으로 구함 */
    SocIdx  = C_PackLimitSocPoint - 2u;
    SocRate = 1.0F;
    for(i = 1u; i < C_PackLimitSocPoint; i++)
    {
        if(CellSocF <= PackLimitSocAxisF[i])
        {
            SocIdx  = i - 1u;
            SocRate = (CellSocF - PackLimitSocAxisF[i - 1u]) / (PackLimitSocAxisF[i] - PackLimitSocAxisF[i - 1u]);
            break;
        }
    }

    p->Bat80VDisCHAPeakFCurrF     = PackLimitLookupF(&PackDisChaPeakLimitF[0][0], TempIdx, TempRate, SocIdx, SocRate);   // TODO : [검증] 260827_Note1, 0.16 5sec 방전 한계[A](양수 크기)
    p->Bat80VCHAPeakFCurrF        = PackLimitLookupF(&PackChaPeakLimitF[0][0],    TempIdx, TempRate, SocIdx, SocRate);   // TODO : [검증] 260827_Note1, 0.16 5sec 충전 한계[A]
    p->Bat80VDisCHAContintyCurrF = PackLimitLookupF(&PackDisChaContLimitF[0][0], TempIdx, TempRate, SocIdx, SocRate);   // TODO : [검증] 260827_Note1, 0.16 연속 방전 한계[A](양수 크기)
    p->Bat80VCHAContintyCurrF    = PackLimitLookupF(&PackChaContLimitF[0][0],    TempIdx, TempRate, SocIdx, SocRate);   // TODO : [검증] 260827_Note1, 0.16 연속 충전 한계[A]

    /*--------------------------------------------------------------
     * 260827 : 연속 전류 한계 대비 초과분[A] 산출 — 한계값을 갱신한 자리에서
     *          함께 구해 Bat80VUnbalCurrentF 에 담는다. 값이 0 보다 크면 초과.
     *          충전(전류 +)은 연속 충전 한계, 방전(전류 -)은 전류 크기와
     *          연속 방전 한계를 비교한다(표·측정 모두 양수 크기 기준).
     *          경고(No.8)·보호(No.23) 판정이 이 값을 사용한다.
     *--------------------------------------------------------------*/
    if(p->Bat80VCurrentF >= 0)
    {
        p->Bat80VUnbalCurrentF = p->Bat80VCurrentF    - p->Bat80VCHAContintyCurrF;      // TODO : [검증] 260827_Note1, 0.16 연속 충전 한계 대비 초과분[A](+ = 초과)
    }
    else
    {
        p->Bat80VUnbalCurrentF = p->Bat80VCurrentAsbF - p->Bat80VDisCHAContintyCurrF;   // TODO : [검증] 260827_Note1, 0.16 연속 방전 한계 대비 초과분[A](+ = 초과)
    }

    /*--------------------------------------------------------------
     * 260902 : (사용자 결정) R3 서문 "이용온도 밖 0A(사용 금지)" 정책 미적용.
     *          → 전류한계 표대로만 동작. 취소선 -30/-20/60℃ 행도 참고값으로
     *            보간에 그대로 사용(추가 0A 강제 없음).
     *          아래 0A 강제 가드는 보류 — 되살릴 때 주석 해제 + 상단 매크로 복구.
     *--------------------------------------------------------------*/
    //if((p->Bat80VCellAgvTemperatureF < C_PackLimitUsableTempMinF) ||
    //   (p->Bat80VCellAgvTemperatureF > C_PackLimitUsableTempMaxF))
    //{
    //    p->Bat80VDisCHAPeakFCurrF    = 0.0F;   // 이용온도 밖 방전피크 0A(사용 금지)
    //    p->Bat80VCHAPeakFCurrF       = 0.0F;   // 이용온도 밖 충전피크 0A(사용 금지)
    //    p->Bat80VDisCHAContintyCurrF = 0.0F;   // 이용온도 밖 방전연속 0A(사용 금지)
    //    p->Bat80VCHAContintyCurrF    = 0.0F;   // 이용온도 밖 충전연속 0A(사용 금지)
    //}
}
#endif
