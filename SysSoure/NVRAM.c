/*
 * Driver.c
 *
 *  Created on: 2025. 10. 26.
 *      Author: leewo
 */


#include "DSP28x_Project.h"
#include "parameter.h"
#include "NVRAM.h"
#include <stdio.h>
#include <math.h>
#include <string.h>



volatile Uint16 g_RTCommitReq  = 0U;
volatile Uint16 g_RTCommitDone = 0U;
volatile Uint32 g_SysTimeTick  = 0U;

extern void SPI_Write(unsigned int WRData);
extern unsigned int SPI_Read(void);
extern NVRAllReg NVRAllRegs;                 // TODO : [검증] 260831_Note1, 0.18 진단값(SRStatus/NvrOk) 기록용
extern void NVRAM_StateTest(void);
extern void NVRAM_ForceResetSysTimeTick(void);
extern void NVRAM_AZoneSaveHandler(NVRZoneAReg *p);
extern void NVRAM_AZoneReadHandler(NVRZoneAReg *p);
extern void NVRAM_RecognizeAndInit(NVRZoneAReg *p);

static void NVRAM_RTCommitTask(int16 Socinit);

#define TEST_ADDR   (0x000100UL)
#define TEST_LEN    (16U)

Uint8 txBuf[TEST_LEN];
Uint8 rxBuf[TEST_LEN];
Uint16 TestState = 1;     // 1: WRITE, 2: READ, 3: COMPARE
Uint16 TestError = 0;
Uint16 i;
//static NVRZoneAReg g_NvrMeta;
// Semaphore
Uint8 SPI_Sema = 0;
// For debugging
Uint16 dbg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
//struct  tm Time;

/******************************************************************************************
 *
 *  NVRAM MR25H40
 *
 *  When SPI clock is 1MHz, data transfer rate is 100bytes per 1 mSec
 *  If mass data should to be sent in interrupt routine, you should be careful of timing
 *
 ******************************************************************************************/

void NVRAM_StateTest(void)
{
    switch(TestState)
    {
        case 1: // WRITE
            // 패턴 생성
            for(i = 0; i < TEST_LEN; i++)
           //     txBuf[i] = (Uint8)(i + 1);

            // Write Enable
            NVR_SPIWrite(NVR_WREN_CMD, NVR_NO_ADDR, 0, 0);

            // Write Data
            NVR_SPIWrite(NVR_WRITE_CMD, TEST_ADDR, txBuf, TEST_LEN);

          //  TestState = 2;
            break;

        case 2: // READ
            memset(rxBuf, 0, TEST_LEN);

            NVR_SPIRead(NVR_READ_CMD, TEST_ADDR, rxBuf, TEST_LEN);

           TestState = 3;
            break;

        case 3: // COMPARE
            TestError = 0;
/*
            for(i = 0; i < TEST_LEN; i++)
            {
                if(txBuf[i] != rxBuf[i])
                {
                    TestError = 1;
                    break;
                }
            }
*/
          //  if(TestError)
           // {
                // 여기서 브레이크 걸고 확인
            //    asm(" ESTOP0");
            //}

            // 정상일 경우 다시 WRITE부터 반복
      //      TestState = 1;
            break;
        case 100:
                 NVRAM_RTCommitTask(250);
        break;

        default:
       //     TestState = 1;
            break;
    }
}
/*--------------------------------------------------------------
 * 260831 : NVRAM 자체 진단 신규(F-3).
 *          기존 NVRAM_StateTest() 는 패턴 생성 for 본문·상태 전이·비교 루프가
 *          모두 주석 처리되어 검증 기능이 없었고 호출처도 없었다.
 *          여기서는 부팅 1회 write→read 왕복을 그 자리에서 끝내고 결과만
 *          NVRAllRegs.NvrOk 에 남긴다(1=정상). 상태머신을 쓰지 않는다.
 *          진단 주소는 로그 영역을 침범하지 않는 NVR_SELFTEST_ADDR(0x0000E0).
 *          ※ 실행 전 NVR_Init() 으로 쓰기보호가 풀려 있어야 한다.
 *--------------------------------------------------------------*/
void NVRAM_SelfTest(void)
{
    Uint16 k;

    for(k = 0U; k < TEST_LEN; k++)
    {
        txBuf[k] = (Uint8)(k + 1U);                  // TODO : [검증] 260831_Note1, 0.18 패턴 생성(구 코드는 본문이 주석이라 누락)
    }

    NVR_SPIWrite(NVR_WREN_CMD,  NVR_NO_ADDR,       (Uint8*)0, 0U);
    NVR_SPIWrite(NVR_WRITE_CMD, NVR_SELFTEST_ADDR, txBuf,     TEST_LEN);

    memset(rxBuf, 0, TEST_LEN);
    NVR_SPIRead (NVR_READ_CMD,  NVR_SELFTEST_ADDR, rxBuf,     TEST_LEN);

    TestError = 0U;
    for(k = 0U; k < TEST_LEN; k++)
    {
        if(txBuf[k] != rxBuf[k])
        {
            TestError = 1U;                          // TODO : [검증] 260831_Note1, 0.18 비교 복구
            break;
        }
    }
    NVRAllRegs.NvrOk = (TestError == 0U) ? 1U : 0U;  // TODO : [검증] 260831_Note1, 0.18 1=왕복 일치
}
void NVRAM_AZoneSaveHandler(NVRZoneAReg *p)
{
    static Uint8 buf[NVR_META_BYTES];   // TODO : [검증] 260831_Note1, 0.18 32 → 36 byte (LastCellV/LastAh 추가)
    if(p == (NVRZoneAReg*)0)
    {
        return;
    }
    /* 최소 방어: SOC 범위 */
    if(p->LastSOC < (int16)-100)  p->LastSOC = (int16)-100;
    if(p->LastSOC > (int16)1100)  p->LastSOC = (int16)1100;

    /* 메타 버전 고정 */
    p->MetaVersion = Product_Version;

    buf[0] = (Uint8)(p->MetaVersion  & 0xFFU);
    buf[1] = (Uint8)((p->MetaVersion  >> 8) & 0xFFU);

    buf[2] = (Uint8)(p->MetaCRC & 0xFFU);
    buf[3] = (Uint8)((p->MetaCRC >> 8) & 0xFFU);

    buf[4] = (Uint8)((Uint16) p->LastSOC & 0xFFU);
    buf[5] = (Uint8)(((Uint16)p->LastSOC >> 8) & 0xFFU);


    buf[6] = (Uint8)(p->NVramtatusRegs.all & 0xFFU);
    buf[7] = (Uint8)((p->NVramtatusRegs.all >> 8) & 0xFFU);

    buf[8]  = (Uint8)( p->LastState         & 0xFFU);  // Byte0 (LSB)
    buf[9]  = (Uint8)((p->LastState >> 8)   & 0xFFU);  // Byte1
    buf[10] = (Uint8)((p->LastState >> 16)  & 0xFFU);  // Byte2
    buf[11] = (Uint8)((p->LastState >> 24)  & 0xFFU);  // Byte3 (MSB)


    buf[12] = (Uint8)( p->LogHeadIndex        & 0xFFU);  // Byte0 (LSB)
    buf[13] = (Uint8)((p->LogHeadIndex >> 8)  & 0xFFU);  // Byte1
    buf[14] = (Uint8)((p->LogHeadIndex >> 16) & 0xFFU);  // Byte2
    buf[15] = (Uint8)((p->LogHeadIndex >> 24) & 0xFFU);  // Byte3 (MSB)

    buf[16] = (Uint8)( p->EventHeadIndex        & 0xFFU);  // Byte0 (LSB)
    buf[17] = (Uint8)((p->EventHeadIndex >> 8)  & 0xFFU);  // Byte1
    buf[18] = (Uint8)((p->EventHeadIndex >> 16) & 0xFFU);  // Byte2
    buf[19] = (Uint8)((p->EventHeadIndex >> 24) & 0xFFU);  // Byte3 (MSB)

    buf[20] = (Uint8)( p->SysTimeTick        & 0xFFU);  // Byte0 (LSB)
    buf[21] = (Uint8)((p->SysTimeTick >> 8)  & 0xFFU);  // Byte1
    buf[22] = (Uint8)((p->SysTimeTick >> 16) & 0xFFU);  // Byte2
    buf[23] = (Uint8)((p->SysTimeTick >> 24) & 0xFFU);  // Byte3 (MSB)

    buf[24] = (Uint8)( p->LastLogTimestamp        & 0xFFU);  // Byte0 (LSB)
    buf[25] = (Uint8)((p->LastLogTimestamp >> 8)  & 0xFFU);  // Byte1
    buf[26] = (Uint8)((p->LastLogTimestamp >> 16) & 0xFFU);  // Byte2
    buf[27] = (Uint8)((p->LastLogTimestamp >> 24) & 0xFFU);  // Byte3 (MSB)

    buf[28] = (Uint8)( p->LastEventTimestamp        & 0xFFU);  // Byte0 (LSB)
    buf[29] = (Uint8)((p->LastEventTimestamp >> 8)  & 0xFFU);  // Byte1
    buf[30] = (Uint8)((p->LastEventTimestamp >> 16) & 0xFFU);  // Byte2
    buf[31] = (Uint8)((p->LastEventTimestamp >> 24) & 0xFFU);  // Byte3 (MSB)

    /*--------------------------------------------------------------
     * 260831 : 재기동 SOC 점프 대책 — 차단 직전 상태를 함께 남긴다.
     *          LastCellV [mV] : 휴지 중 전압 변화 판정용 (F-5)
     *          LastAh  [0.1Ah] : 적산 이력 확인용 기록 (F-7 검토 결과 복원엔 미사용)
     *--------------------------------------------------------------*/
    buf[32] = (Uint8)( p->LastCellV        & 0xFFU);           // TODO : [검증] 260831_Note1, 0.18 F-5
    buf[33] = (Uint8)((p->LastCellV >> 8)  & 0xFFU);
    buf[34] = (Uint8)((Uint16) p->LastAh        & 0xFFU);      // TODO : [검증] 260831_Note1, 0.18 F-7
    buf[35] = (Uint8)(((Uint16)p->LastAh >> 8)  & 0xFFU);

    /* CRC 미사용 → 0 */
    p->MetaCRC = 0U;

    /* Write Enable */
    (void)NVR_SPIWrite(NVR_WREN_CMD, NVR_NO_ADDR, (Uint8*)0, 0U);
    /* A영역 저장 */
    /*--------------------------------------------------------------
     * 260831 : 직렬화 길이 32 → NVR_META_BYTES(36).
     *--------------------------------------------------------------*/
    //(void)NVR_SPIWrite(NVR_WRITE_CMD, NVR_META_ADDR, (Uint8*)buf,(Uint16)32U);
    (void)NVR_SPIWrite(NVR_WRITE_CMD, NVR_META_ADDR, (Uint8*)buf,(Uint16)NVR_META_BYTES);   // TODO : [검증] 260831_Note1, 0.18
}
void NVRAM_AZoneReadHandler(NVRZoneAReg *p)
{
    // Uint16 OK;
    static Uint8 buf[NVR_META_BYTES];   // TODO : [검증] 260831_Note1, 0.18 32 → 36 byte
    static Uint16 idx = 0U;
    if (p == (NVRZoneAReg *)0)
    {
     //   return ;
    }
    //  1. 영역 A 메타데이터 읽기
    /*--------------------------------------------------------------
     * 260831 : 읽기 길이 32 → NVR_META_BYTES(36).
     *--------------------------------------------------------------*/
    //NVR_SPIRead(NVR_READ_CMD,NVR_META_ADDR,(Uint8*)buf,(Uint16)32U);
    NVR_SPIRead(NVR_READ_CMD,NVR_META_ADDR,(Uint8*)buf,(Uint16)NVR_META_BYTES);   // TODO : [검증] 260831_Note1, 0.18
    //  2. 최초 인지 판단
    /* MetaVersion */
    p->MetaVersion =(Uint16)buf[0] |((Uint16)buf[1] << 8);

    /* MetaCRC */
    p->MetaCRC = (Uint16)buf[2] |((Uint16)buf[3] << 8);

    /* LastSOC (int16) */
    p->LastSOC = (int16)((Uint16)buf[4] | ((Uint16)buf[5] << 8));

    /* rsvd0 */
    p->rsvd0 = (Uint16)buf[6] | ((Uint16)buf[7] << 8);

    /* LastState (Uint32) */
    p->LastState =
        ((Uint32)buf[8])         |
        ((Uint32)buf[9] << 8)    |
        ((Uint32)buf[10] << 16)  |
        ((Uint32)buf[11] << 24);

    /* LogHeadIndex */
    p->LogHeadIndex =
        ((Uint32)buf[12])        |
        ((Uint32)buf[13] << 8)   |
        ((Uint32)buf[14] << 16)  |
        ((Uint32)buf[15] << 24);

    /* EventHeadIndex */
    p->EventHeadIndex =
        ((Uint32)buf[16])        |
        ((Uint32)buf[17] << 8)   |
        ((Uint32)buf[18] << 16)  |
        ((Uint32)buf[19] << 24);

    /* SysTimeTick */
    p->SysTimeTick =
        ((Uint32)buf[20])        |
        ((Uint32)buf[21] << 8)   |
        ((Uint32)buf[22] << 16)  |
        ((Uint32)buf[23] << 24);

    /* LastLogTimestamp */
    p->LastLogTimestamp =
        ((Uint32)buf[24])        |
        ((Uint32)buf[25] << 8)   |
        ((Uint32)buf[26] << 16)  |
        ((Uint32)buf[27] << 24);

    /* LastEventTimestamp */
    p->LastEventTimestamp =
        ((Uint32)buf[28])        |
        ((Uint32)buf[29] << 8)   |
        ((Uint32)buf[30] << 16)  |
        ((Uint32)buf[31] << 24);

    /* LastCellV / LastAh */
    p->LastCellV = (Uint16)buf[32] | ((Uint16)buf[33] << 8);              // TODO : [검증] 260831_Note1, 0.18 F-5
    p->LastAh    = (int16)((Uint16)buf[34] | ((Uint16)buf[35] << 8));     // TODO : [검증] 260831_Note1, 0.18 F-7

    /* 디버그용 확인 */
    if(idx != 32U)
    {
        /* 오류 처리 or assert */
    }

    /*--------------------------------------------------------------
     * 260831 : 읽기 유효성 검사 복구(F-2). 기존 주석 블록은 복구값을
     *          LastSOC=300(30%) 으로 채워 '가짜 유효값' 이 되므로 쓰지 않고,
     *          무효 마커(-1) 로만 표시해 상위(main.c)가 OCV 로 폴백하게 한다.
     *--------------------------------------------------------------*/
    if (p->MetaVersion != (Uint16)Product_Version)
    {
        p->LastSOC   = (int16)-1;                                        // TODO : [검증] 260831_Note1, 0.18 F-2 무효 마커
        p->MetaCRC   = 0u;
        p->LastCellV = 0u;                                               // TODO : [검증] 260831_Note1, 0.18 전압 비교도 무효화
        p->LastAh    = (int16)0;
    }

/*
    if (p->MetaVersion != (Uint16)Product_Version)
    {
        // -------- 초기화 --------
        p->MetaVersion        = (Uint16)Product_Version;
        p->MetaCRC            = 0u;
        p->LastSOC            = 300;
        p->rsvd0              = 0u;
        p->LastState          = 0u;
        p->LogHeadIndex       = 0u;
        p->EventHeadIndex     = 0u;
        p->SysTimeTick        = 0u;
        p->LastLogTimestamp   = 0u;
        p->LastEventTimestamp = 0u;
        for (i = 0u; i < 8u; i++)
        {
            p->rsvd1[i] = 0u;
        }
    }
    else
    {

    }*/
}
void NVRAM_RecognizeAndInit(NVRZoneAReg *p)
{
    static Uint8 buf[32];
    if (p == (NVRZoneAReg *)0)
    {
     //   return ;
    }
    //  1. 영역 A 메타데이터 읽기
    NVR_SPIRead(NVR_READ_CMD,NVR_META_ADDR,(Uint8*)buf,(Uint16)32U);
    //  2. 최초 인지 판단
    /* MetaVersion */
    p->MetaVersion =(Uint16)buf[0] |((Uint16)buf[1] << 8);

    /* MetaCRC */
    p->MetaCRC = (Uint16)buf[2] |((Uint16)buf[3] << 8);

    /* LastSOC (int16) */
    p->LastSOC = (int16)((Uint16)buf[4] | ((Uint16)buf[5] << 8));

    /* rsvd0 */
    p->rsvd0 = (Uint16)buf[6] | ((Uint16)buf[7] << 8);

    /* LastState (Uint32) */
    p->LastState =
        ((Uint32)buf[8])        |
        ((Uint32)buf[9] << 8)   |
        ((Uint32)buf[10] << 16)  |
        ((Uint32)buf[11] << 24);

    /* LogHeadIndex */
    p->LogHeadIndex =
        ((Uint32)buf[12])        |
        ((Uint32)buf[13] << 8)   |
        ((Uint32)buf[14] << 16)  |
        ((Uint32)buf[15] << 24);

    /* EventHeadIndex */
    p->EventHeadIndex =
        ((Uint32)buf[16])        |
        ((Uint32)buf[17] << 8)   |
        ((Uint32)buf[18] << 16)  |
        ((Uint32)buf[19] << 24);

    /* SysTimeTick */
    p->SysTimeTick =
        ((Uint32)buf[20])        |
        ((Uint32)buf[21] << 8)   |
        ((Uint32)buf[22] << 16)  |
        ((Uint32)buf[23] << 24);

    /* LastLogTimestamp */
    p->LastLogTimestamp =
        ((Uint32)buf[24])        |
        ((Uint32)buf[25] << 8)   |
        ((Uint32)buf[26] << 16)  |
        ((Uint32)buf[27] << 24);

    /* LastEventTimestamp */
    p->LastEventTimestamp =
        ((Uint32)buf[28])        |
        ((Uint32)buf[29] << 8)   |
        ((Uint32)buf[30] << 16)  |
        ((Uint32)buf[31] << 24);
    //  2. 최초 인지 판단
    if (p->MetaVersion != (Uint16)Product_Version)
    {
        // -------- 초기화 --------
        p->MetaVersion        = (Uint16)Product_Version;
        p->MetaCRC            = 0u;
        p->LastSOC            = 300;
        p->rsvd0              = 0u;
        p->LastState          = 0u;
        p->LogHeadIndex       = 0u;
        p->EventHeadIndex     = 0u;
        p->SysTimeTick        = 0u;
        p->LastLogTimestamp   = 0u;
        p->LastEventTimestamp = 0u;
        for (i = 0u; i < 8u; i++)
        {
            p->rsvd1[i] = 0u;
        }
    }

}
static void NVRAM_RTCommitTask(int16 Socinit)
{
    int16 soc;

    if(g_RTCommitReq == 0U) return;

    // 재진입 방지
    g_RTCommitReq  = 0U;
    g_RTCommitDone = 0U;

    // CCS가 직접 바꿔놓은 "기존 SOC 변수" 사용
    soc = Socinit;

    // SOC 범위 클램프 (-100 ~ 1000)
    if(soc < (int16)-100) soc = (int16)-100;
    if(soc > (int16)1000) soc = (int16)1000;

    // TimeBase 리셋 + 인덱스 리셋 + SOC 저장
    g_SysTimeTick            = 0U;
    // (선택) State도 초기화하려면
    // NVRAM(영역 A) 저장
    NVR_SPIWrite(NVR_WREN_CMD, NVR_NO_ADDR, 0, 0);
  //  NVR_SPIWrite(NVR_WRITE_CMD, NVR_META_ADDR,
  //               (Uint8*)&g_NvrMeta, (Uint16)sizeof(NVR_Meta_t));

    g_RTCommitDone = 1U;
}

void NVR_Init(void)
{
    NVR_UnLock();

}
void NVR_UnLock(void)
{
    Uint8 buf[10];
    Uint8 sr = 0U;
    // Enable write
    NVR_SPIWrite(NVR_WREN_CMD, 0xffffffff, NULL, 0);
    // Set status register - Un-protect all range
    buf[0] = 0x02;
    NVR_SPIWrite(NVR_WRSR_CMD, 0Xffffffff, buf, 1);

    /*--------------------------------------------------------------
     * 260831 : 해제 결과를 읽어 남긴다(F-1). 원인 판별용 진단값이며
     *          동작을 바꾸지는 않는다.
     *            0xFF           : 칩 무응답 (미실장·CS·전원)
     *            0x00           : MISO Low 고정 (배선)
     *            (sr & 0x0C)!=0 : BP 잔존 → 쓰기보호 해제 실패
     *--------------------------------------------------------------*/
    NVR_SPIRead(NVR_RDSR_CMD, NVR_NO_ADDR, &sr, 1);
    NVRAllRegs.SRStatus = (Uint16)sr;                    // TODO : [검증] 260831_Note1, 0.18 F-1 해제 확인
}

void NVR_Lock(void)
{
    Uint8 buf[10];
    // Disable write
    NVR_SPIWrite(NVR_WRDI_CMD, 0xffffffff, NULL, 0);
    // Set status register - Protect all range
    buf[0] = 0x80;
    NVR_SPIWrite(NVR_WRSR_CMD, 0Xffffffff, buf, 1);

}
volatile unsigned char testTXbuf[30];
volatile unsigned int  testTXlen;
volatile unsigned char testRXbuf[30];
volatile unsigned int  testRXlen;
Uint8 NVR_SPIWrite(Uint8 cmd, Uint32 addr, Uint8* buf, Uint16 len)
{
    Uint16 i = 0U;

    if(SPI_Sema) return 0U;
    SPI_Sema = 1U;

    // Optional safety
    if((len != 0U) && (buf == NULL))
    {
        SPI_Sema = 0U;
        return 0U;
    }
    // Set configuration
    /*--------------------------------------------------------------
     * 260901 : SPI-A 버스 공유 대응. NVRAM 설정을 CS Low 직전에
     *          기입하는 방식은 그대로 두고, BATIC 쪽에도 동일하게
     *          설정 기입을 추가하여 속도 오염을 막는다.
     *          (BAT_LTC6802.c : BATSPIEnable_low)
     *          주석 오기 정정 : CLKPOLARITY=1 은 Falling edge output
     *--------------------------------------------------------------*/
    //SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;        // Rising edge output
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;        // TODO : [검증] 260901_Note1, 0.18 NVRAM 극성 (Falling edge output)
    SpiaRegs.SPIBRR                 = 50;       // TODO : [검증] 260901_Note1, 0.18 NVRAM 속도 392kHz (기존값 유지)

    NVR_CE_L;
    delay_us(1);

    SPI_Write((Uint16)cmd);
    testTXlen =len;
   // addr = addr & 0x0007FFFF;
    if(addr != NVR_NO_ADDR) // No address field, address = 0xffffffff
    {
        addr &=NVR_ADDR_MASK;  // 0x0007FFFF
        SPI_Write((Uint16)((addr >> 16) & 0xFFU));
        SPI_Write((Uint16)((addr >>  8) & 0xFFU));
        SPI_Write((Uint16)((addr >>  0) & 0xFFU));
    }
    for(i = 0; i < len; i++)
    {
        testTXbuf[i]=buf[i];
        SPI_Write((Uint16)buf[i]);
    }

    NVR_CE_H;
    delay_us(1);

    /*--------------------------------------------------------------
     * 260831 : NVRAM 접근용 SPIBRR(50) 을 쓰고 나서 되돌리지 않아
     *          이후 LTC6804 통신이 잘못된 속도로 돌던 문제 수정.
     *          복원값은 InitSpi() 설정값 119 (BAT_LTC6802.c:72).
     *--------------------------------------------------------------*/
    //[원복] 260831 SPIBRR=119 복원 제거 (세션 되돌리기 E)
    //SpiaRegs.SPIBRR = 119;
    SPI_Sema = 0U;
    return 1U;
}

Uint8 NVR_SPIRead(Uint8 cmd, Uint32 addr, Uint8* buf, Uint16 len)
{
    Uint16 i = 0;

    if(SPI_Sema) return 0;
    SPI_Sema = 1;

    if((len != 0U) && (buf == NULL))
    {
        SPI_Sema = 0U;
        return 0U;
    }
    // Set configuration
    /*--------------------------------------------------------------
     * 260901 : SPI-A 버스 공유 대응. NVR_SPIWrite() 와 동일 사유.
     *--------------------------------------------------------------*/
    //SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;        // Rising edge output
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;        // TODO : [검증] 260901_Note1, 0.18 NVRAM 극성 (Falling edge output)
    SpiaRegs.SPIBRR                 = 50;       // TODO : [검증] 260901_Note1, 0.18 NVRAM 속도 392kHz (기존값 유지)

    NVR_CE_L;
    delay_us(1);

    SPI_Write(cmd);


    if(addr != NVR_NO_ADDR)
    {
        addr &= NVR_ADDR_MASK;               // 0x0007FFFF
        SPI_Write((Uint16)((addr >> 16) & 0xFFU));
        SPI_Write((Uint16)((addr >>  8) & 0xFFU));
        SPI_Write((Uint16)((addr >>  0) & 0xFFU));
    }
    testRXlen=len;
    for (i = 0; i < len; i++)
    {
        buf[i] = SPI_Read();
        testRXbuf[i]=buf[i];
    }
    NVR_CE_H;
    delay_us(1);

    //[원복] 260831 SPIBRR=119 복원 제거 (세션 되돌리기 E)
    //SpiaRegs.SPIBRR = 119;
    SPI_Sema = 0;
    return 1;
}

