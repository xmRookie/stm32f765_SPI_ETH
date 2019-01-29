
#ifndef __COMMOM_H__
#define __COMMOM_H__

#include "stm32f7xx_hal.h"

#include "string.h"

//#define SUP_DMA_USART3_TX
//#define SUP_DMA_USART3_RX

#define SUP_DMA_SPI5_TX
#define SUP_DMA_SPI5_RX

//I2S2 Add
#define SUP_DMA_I2S2_RX


typedef int_fast8_t         Boolean;        // ����Ϊ1bit


#ifndef FALSE
    #define FALSE       0
#endif

#ifndef TRUE
    #define TRUE        1
#endif



#define NET_LINKED       1

#define NET_UNLINK       0

typedef uint8_t   u8_t;
typedef int8_t    s8_t;
typedef uint16_t  u16_t;
typedef int16_t   s16_t;
typedef uint32_t  u32_t;
typedef int32_t   s32_t;
typedef uintptr_t mem_ptr_t;



//mavlink ����״̬

#ifndef PAR_FALSE
    #define PAR_FALSE      0
#endif

#ifndef PAR_OK
    #define PAR_OK         1
#endif

#ifndef PAR_IGNORE
    #define PAR_IGNORE      2
#endif


#define RBUF_NEXT_PT(dat, offset, size)     (((dat) + (offset)) & ((size) - 1))

#define AVBUF_NEXT_PT(dat, size)     ((dat + 1) % (size))



#define VERSION  "0.5"

#define ON_ST   1
#define OFF_ST  0

//for 25MHZ config
#define EXT_OSC_HSE

#define EN_TIM3
 /* maximum transfer unit */
#define MAX_NUM_WINDOW  1500

#define MODE_ON             0
#define MODE_OFF            1
#define MODE_ERR            2
#define MODE_RESET          3
#define MODE_REG            4

#define HARD_VER                    0x00030001              //; Ӳ���汾

#define VER_VERSION                 0x1005                  //; ����汾��
#define VER_YEAR                    0x2018                  //; ��
#define VER_MONTH                   0x07                    //; ��
#define VER_DAY                     0x08                    //; ��


#define NET_ERR                 0       // ʧ��,�����ݷ���
#define NET_ACK                 1       // ȷ������
#define NET_DownPrg             2       // ���س���
#define NET_DownConfigPrg       3       // �������ó���


#define  PRO_STX      0xfe 
#define  PRO_MSGID    0x1e 

#define PRO_EXTRA_CRC 39

#define PROG_FILE_INFO          1
#define PROG_FILE_DAT           2
#define PROG_FILE_END           3   

#define PROG_FILE_CNFG_DAT      4
#define PROG_FILE_CNFG_END      5


#define PROG_MAX_LEN           264

#define PROG_CORE_HEADER_LEN   5
#define PROG_NUM_HEADER_BYTES (PROG_CORE_HEADER_LEN + 1) ///< Length of all header bytes, including core and checksum
#define PROG_NUM_CHECKSUM_BYTES 2
#define PROG_MAX_PAYLOAD_LEN 256 ///< Maximum payload length




typedef struct
{
    uint8_t                 type;                       //
    uint8_t                 command;
    uint16_t                length;
    uint32_t                addr;
//---------------------------
    uint8_t                 magic;   ///< protocol magic marker
	uint8_t                 len;     //len = 0 = 256
    uint8_t                 cmd;                        // ����  PROG_FILE_INFO PROG_FILE_DAT PROG_FILE_END
    uint8_t                 para1;                      // ��ַ����������1
    uint8_t                 para2;                      // ��ַ����������2
	uint8_t 				msgid;   ///< ID of message in payload
    uint8_t                 buf[PROG_MAX_PAYLOAD_LEN];                 // ���ݻ�����
} __attribute__( ( packed ) ) STR_PECRPORTOCOL;         // ͨѶ����ṹ



//-----------------------------------------------------------------------------


#define evRst              0x20            // ��������
#define evURegSave          0x21            // save user registers to flash
#define evGUserReg          0x22            //read flash values and load into reg
#define evFctyRegSave       0x23			
#define evGFctyReg          0x24           //load default factory settings into all registers	

#define evRst              0x20            // ��������




#define ev1ST               0xa0            // 1���¼�
#define evHST               0xa1            // �����¼�
#define ev100MS             0xa2
#define evNON               0xff



#define   SPI_RXTIMEOUT             1             // ���ճ�ʱʱ��,��1mSec��δ�����κ�����,����һ�����հ�

#define   spi_tx_buf_Number  20000 //1128          // ���ͻ�������С,188X3
#define   spi_rx_buf_Number  2048          // �ӽӻ�������С,


#define   TS_BUF_Number  1128          // ���ͻ�������С,188X6

#define  TS_PACKET_SIZE  188

#define TS_BUF_NEXT_PT(dat, offset, size)     (((dat) + (offset)) & ((size) - TS_PACKET_SIZE))



#define LAN_ACK                 0x02
#define LAN_ERR                 0x03
#define LAN_ETX                 0x04


typedef struct {
    volatile uint16_t        Write;
    volatile uint16_t        Read;
    volatile Boolean         HaveData;
	uint16_t                 maxlen;
	uint16_t                 cnt;

} AV_TS_STREAM;




#define TS_MUX_BUSY 1
#define TS_MUX_IDLE 0

#define UDP_TS_NUM    14

#define SPI_BUFNB   8



#define SPI_BUF_DISCARD (SPI_BUF_SIZE+1)

#define SPI_Half_BUF_SIZE  0x1000

#define TS_BUF_SIZE   0x2000

#define UDP_BUF_SIZE  0x4000

//#define TEST_UDP_FFFF
//#define TEST_UDP
//#define TEST_SPI
//#define SEFT_TEST


#include "main.h"
#include "romflash.h"
#include "checksum.h"
#include "pollint.h"
#include "uart2.h"
#include "uart3.h"
#include "uart4.h"
#include "uart6.h"
//#include "uart7.h"

#include "i2c1.h"
#include "i2c2.h"
#include "i2c3.h"
#include "i2c4.h"

#include "spi1.h"
#include "spi4.h"
#include "spi5.h"

#include "i2s2.h"
#include "i2s3.h"

#include "dma.h"

#include "udp_api.h"
#include "led.h"
#include "key.h"
#include "eventManage.h"
#include "power.h"
#include "config_env.h"
#include "mboot.h"
#include "tsmux.h"
#include "mp2.h"
#endif  /*__COMMOM_H__ */

