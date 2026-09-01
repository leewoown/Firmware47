/*
 * NVRAM.h
 *
 *  Created on: 2025. 10. 26.
 *      Author: leewo
 */
//#include "F2806x_Cla_typedefs.h"// F2806x CLA Type definitions
//#include "F2806x_Device.h"      // F2806x Headerfile Include File
//#include "F2806x_Examples.h"    // F2806x Examples Include File
#include "DSP28x_Project.h"
#include <string.h>   // memset if needed

#ifndef SYSINCLUDE_NVRAM_H_
#define SYSINCLUDE_NVRAM_H_



// For SPI interrupt control
void SPI_DI(void);
void SPI_EI(void);


#define NVR_CE_L     (GpioDataRegs.GPACLEAR.bit.GPIO11 = 1)
#define NVR_CE_H     (GpioDataRegs.GPASET.bit.GPIO11 = 1)


// For debugging
//extern Uint16 dbg[];
typedef enum
{
   NVRAM_AZoneSave, //1
   NVRAM_BZoneSave, //1
   NVRAM_CZoneSave, //2
   NVRAM_DZoneSave, //2
   NVRAM_AZoneRead, //3
   NVRAM_BZoneRead, //4
   NVRAM_CZoneRead, //5
   NVRAM_DZoneRead, //6
   NVRAM_NVRInit,   //7
   NVRAM_NVRforceWR,//8
   NVRAM_NVRSocInit,//9
   NVRAM_MANUALMode //10
}NVRState;
struct NVramtatus_BIT
{       // bits   description
    unsigned int     NVRInit         :1; // 0
    unsigned int     NVRforceWR      :1; // 1
    unsigned int     NVRSocInit      :1; // 2
    unsigned int     NVRManual       :1; // 3
    unsigned int     NVRLogRead      :1; // 4
    unsigned int     NVREventRead    :1; // 5
    unsigned int     STATE06         :1; // 6
    unsigned int     STATE07         :1; // 7
    unsigned int     STATE08         :1; // 8
    unsigned int     STATE09         :1; // 9
    unsigned int     STATE10         :1; // 10
    unsigned int     STATE11         :1; // 11
    unsigned int     STATE12         :1; // 12
    unsigned int     STATE13         :1; // 13
    unsigned int     STATE14         :1; // 14
    unsigned int     STATE15         :1; // 15
};
union NVramtatus_REG
{
   unsigned int     all;
   struct NVramtatus_BIT bit;
};
typedef struct NVRAll_Data
{
    NVRState SEQ;
    unsigned int SEQTimeTick;
    unsigned int DebugCount;
    Uint32 SysTimeTick;
    /*--------------------------------------------------------------
     * 260831 : NVR 미동작 원인(쓰기보호 잔존 / 칩 무응답) 을 실측으로
     *          가리기 위한 진단값 추가. 부팅 1회 갱신 후 유지된다.
     *--------------------------------------------------------------*/
    Uint16 SRStatus;          /* 상태 레지스터 원본 : 0xFF=무응답, 0x00=MISO Low, bit3:2=BP */   // TODO : [검증] 260831_Note1, 0.18 F-1 진단
    Uint16 NvrOk;             /* 부팅 자체 진단 결과 : 1=write→read 왕복 일치 */                 // TODO : [검증] 260831_Note1, 0.18 F-3 진단

}NVRAllReg;

// NVRAM MR25H40
//
// A 영역: Metadata (Fixed area)

typedef struct NVRZoneA_Data
{

    Uint16 MetaVersion;
    Uint16 MetaCRC;

    int16  LastSOC;           // -100 ~ 1000
    Uint16 rsvd0;

    Uint32 LastState;         // 32-bit state flags

    Uint32 LogHeadIndex;      // B 영역 write index
    Uint32 EventHeadIndex;    // C 영역 write index

    Uint32 SysTimeTick;       // 100ms tick (uint32 -> about 13.6 years)
    Uint32 LastLogTimestamp;  // optional
    Uint32 LastEventTimestamp;// optional
    union NVramtatus_REG        NVramtatusRegs;
    /*--------------------------------------------------------------
     * 260831 : 재기동 SOC 점프 대책 — 차단 직전 셀 평균전압과 적산 Ah 를
     *          함께 남긴다. 부팅 시 전압이 그대로면(휴지 중 변화 없음)
     *          OCV 재추정 대신 NVR 값을 그대로 이어받기 위한 것.
     *          메타 영역 직렬화 길이는 32 → 36 byte 로 늘어난다.
     *--------------------------------------------------------------*/
    Uint16 LastCellV;         /* [mV] 마지막 저장 시점 셀 평균전압 */                    // TODO : [검증] 260831_Note1, 0.18 F-5 대책
    int16  LastAh;            /* [0.1Ah] 마지막 저장 시점 적산 Ah */                     // TODO : [검증] 260831_Note1, 0.18 F-7 대책
    Uint16 rsvd1[8];          // expansion/padding
}NVRZoneAReg;

// B 영역: 운전 이력 레코드 (32B)
typedef struct NVRZoneB_Data
{
    Uint32 TimeTick;          // 100ms tick
    int16  Soc;               // 2B
    Uint16 PackVolt;          // 2B
    int16  PackCurr;          // 2B

    Uint16 CellMaxVolt;       // 2B
    Uint16 CellMinVolt;       // 2B

    int16  CellMaxTemp;       // 2B
    int16  CellMinTemp;       // 2B

    Uint16 AlarmBits;         // 2B
    Uint16 FaultBits;         // 2B

    Uint32 ProtectBits;       // 4B
    Uint32 StateBits;         // 4B

    Uint16 rsvd;              // 2B (padding)
} NVRZoneBReg;

// C 영역: 이벤트 레코드 (32B)
typedef struct NVRZoneC_Data
{
    Uint32 TimeTick;          // 100ms tick
    Uint32 ProtectBits;       // 4B
    Uint32 StateBits;         // 4B

    Uint32 LogIndex;          // reference to B 영역 index

    int16  Soc;
    Uint16 PackVolt;
    int16  PackCurr;

    Uint16 CellMaxVolt;
    Uint16 CellMinVolt;

    int16  CellMaxTemp;
    int16  CellMinTemp;

    Uint16 rsvd;              // padding
} NVRZoneCReg;

// ============================================================================
// Globals (defined in NVRAM.c)
// ============================================================================
extern volatile Uint32 g_SysTimeTick;   // 100ms tick
extern volatile Uint16 g_Flag100ms;
extern volatile Uint16 g_RTCommitReq;
extern volatile Uint16 g_RTCommitDone;


extern Uint32 g_LogIndex;
extern Uint32 g_EventIndex;


#define NVR_WREN_CMD          (0x06)
#define NVR_WRDI_CMD          (0x04)
#define NVR_RDSR_CMD          (0x05)
#define NVR_WRSR_CMD          (0x01)
#define NVR_READ_CMD          (0x03)
#define NVR_WRITE_CMD         (0x02)
#define NVR_SLEEP_CMD         (0xB9)
#define NVR_WAKE_CMD          (0xAB)
#define NVR_NO_ADDR           (0xFFFFFFFFUL)
#define NVR_TOTAL_SIZE_BYTES  (524288UL)     // 512kB
#define NVR_ADDR_MASK         (0x0007FFFFUL) // 19-bit


#define NVR_META_BASE_ADDR   (0x000000UL)
#define NVR_META_SIZE_BYTES  (256UL)
#define NVR_META_ADDR        (NVR_META_BASE_ADDR)
/*--------------------------------------------------------------
 * 260831 : 메타 직렬화 길이를 매크로화 — LastCellV/LastAh 추가로 32 → 36 byte.
 *          자체 진단 주소는 메타 36 byte 와 로그 영역(0x000100) 사이의
 *          빈 자리(0x0000E0) 를 쓴다. 기존 TEST_ADDR(0x000100) 은
 *          로그 첫 레코드를 덮어써서 부적합했다.
 *--------------------------------------------------------------*/
#define NVR_META_BYTES       (36U)           /* A 영역 직렬화 길이 */                    // TODO : [검증] 260831_Note1, 0.18
#define NVR_SELFTEST_ADDR    (0x0000E0UL)    /* 부팅 자체 진단 전용 주소 */              // TODO : [검증] 260831_Note1, 0.18 F-3

#define NVR_LOG_BASE_ADDR    (0x000100UL)
#define NVR_LOG_REC_SIZE     (32UL)
#define NVR_LOG_AREA_SIZE    (384UL * 1024UL)
#define NVR_LOG_MAX_REC      (NVR_LOG_AREA_SIZE / NVR_LOG_REC_SIZE)
#define NVR_LOG_ADDR(idx)    (NVR_LOG_BASE_ADDR + ((Uint32)(idx) * NVR_LOG_REC_SIZE))

#define NVR_EVT_BASE_ADDR    (NVR_LOG_BASE_ADDR + NVR_LOG_AREA_SIZE) // 0x060100
#define NVR_EVT_REC_SIZE     (32UL)
#define NVR_EVT_AREA_SIZE    (NVR_TOTAL_SIZE_BYTES - NVR_EVT_BASE_ADDR)
#define NVR_EVT_MAX_REC      (NVR_EVT_AREA_SIZE / NVR_EVT_REC_SIZE)
#define NVR_EVT_ADDR(idx)    (NVR_EVT_BASE_ADDR + ((Uint32)(idx) * NVR_EVT_REC_SIZE))


void NVR_Init(void);
void NVR_Lock(void);
void NVR_UnLock(void);
void NVRAM_SelfTest(void);   // TODO : [검증] 260831_Note1, 0.18 F-3 부팅 1회 write→read 왕복 진단
Uint8 NVR_SPIWrite(Uint8 cmd, Uint32 addr, Uint8* buf, Uint16 len);
Uint8 NVR_SPIRead (Uint8 cmd, Uint32 addr, Uint8* buf, Uint16 len);
Uint8 NVR_WriteBytes(Uint32 addr, Uint8* buf, Uint16 len);
Uint8 NVR_ReadBytes (Uint32 addr, Uint8* buf, Uint16 len);
void   NVR_LoadMeta(void);
void  NVR_SaveMeta(void);
void  NVR_LoggingTask100ms(void);
void  NVR_PushEvent(Uint32 protectBits, Uint32 stateBits);


#endif /* SSYSINCLUDE_NVRAM_H_*/
