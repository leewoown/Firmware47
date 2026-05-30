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
void NVRAM_AZoneSaveHandler(NVRZoneAReg *p)
{
    static Uint8 buf[32];
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

    /* CRC 미사용 → 0 */
    p->MetaCRC = 0U;

    /* Write Enable */
    (void)NVR_SPIWrite(NVR_WREN_CMD, NVR_NO_ADDR, (Uint8*)0, 0U);
    /* A영역 저장 */
    (void)NVR_SPIWrite(NVR_WRITE_CMD, NVR_META_ADDR, (Uint8*)buf,(Uint16)32U);
}
void NVRAM_AZoneReadHandler(NVRZoneAReg *p)
{
    // Uint16 OK;
    static Uint8 buf[32];
    static Uint16 idx = 0U;
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

    /* 디버그용 확인 */
    if(idx != 32U)
    {
        /* 오류 처리 or assert */
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
    // Enable write
    NVR_SPIWrite(NVR_WREN_CMD, 0xffffffff, NULL, 0);
    // Set status register - Un-protect all range
    buf[0] = 0x02;
    NVR_SPIWrite(NVR_WRSR_CMD, 0Xffffffff, buf, 1);
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
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;        // Rising edge output
    SpiaRegs.SPIBRR                 = 50;       // Baud rate

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
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;        // Rising edge output
    SpiaRegs.SPIBRR                 = 50;       // Baud rate

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

    SPI_Sema = 0;
    return 1;
}

