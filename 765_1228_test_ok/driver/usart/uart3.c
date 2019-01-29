#include "common.h"

UART_HandleTypeDef huart3;
#ifdef SUP_DMA_USART3_RX
DMA_HandleTypeDef hdma_usart3_rx;
#endif

#ifdef SUP_DMA_USART3_TX
DMA_HandleTypeDef hdma_usart3_tx;
#endif



#define   RXTIMEOUT             10             // 接收超时时间,如10mSec内未接收任何数据,当作一个接收包

#define   usart3_tx_buf_Number  0x2000          // 发送缓冲区大小,必须为2的n次方
#define   usart3_rx_buf_Number  0x200          // 接接缓冲区大小,必须为2的n次方

#define _B9600      0
#define _B19200     1
#define _B38400     2
#define _B57600     3
#define _B115200    4

typedef struct {
    uint16_t                 Write;
    volatile uint16_t        Read;
    volatile Boolean        HaveData;
} UART_TX_T;

typedef struct {
    volatile uint16_t        rxcount;
    volatile uint8_t        timeout;
    Boolean                 busy;
} UART_RX_T;

typedef struct {
    UART_TX_T               tx;
    UART_RX_T               rx;
    uint8_t                 AutoChgBRageTimeOut;
    Boolean                 NotIn9600;
} UART_T;

static UART_T usart3;
static uint8_t usart3_tx_buf[usart3_tx_buf_Number];
static uint8_t usart3_rx_buf[usart3_rx_buf_Number];


/* USART3 init function */
extern void MX_USART3_UART_Init(void)
{

    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;

    //huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
// huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if(HAL_UART_Init(&huart3) != HAL_OK) {
        _Error_Handler(__FILE__, __LINE__);
    }

    memset(&usart3, 0, sizeof(usart3));
#ifdef SUP_DMA_USART3_RX
    HAL_UART_Receive_DMA(&huart3, (uint8_t*)&usart3_rx_buf[usart3.rx.rxcount], 1);
#else
    HAL_UART_Receive_IT(&huart3, (uint8_t*)&usart3_rx_buf[usart3.rx.rxcount], 1);
#endif
	//usart3.rx.rxcount = RBUF_NEXT_PT(usart3.rx.rxcount, 1, usart3_rx_buf_Number);

}


#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE {
    /* 把ch的内容通过uart2发送出去，长度是1，*/
    USART3_Putchar((uint8_t)ch);
	
    return 0;
}


extern void HAL_UART_RxCpltCallback(UART_HandleTypeDef *husart)
{
    if(husart->Instance == USART3) {
        if(usart3.rx.busy == 0) {
           
            usart3.rx.timeout = 0;
					 usart3.rx.rxcount = RBUF_NEXT_PT(usart3.rx.rxcount, 1, usart3_rx_buf_Number);
#ifdef SUP_DMA_USART3_RX
            HAL_UART_Receive_DMA(&huart3, (uint8_t*)&usart3_rx_buf[usart3.rx.rxcount], 1);
#else
            HAL_UART_Receive_IT(&huart3, (uint8_t *)&usart3_rx_buf[usart3.rx.rxcount], 1); //重新打开串口中断
#endif		
        }
    }
		  //接收到数据
		
		else if(husart->Instance == USART2){
					USART2_Rx_Deal();
		}
		
		else{}

  

}

extern void HAL_UART_TxCpltCallback(UART_HandleTypeDef *husart)
{
    if(husart->Instance == USART3) {
        if(usart3.tx.Write == usart3.tx.Read) {
            // 数据已经发送完
            usart3.tx.HaveData = 0;
        } else {
#ifdef SUP_DMA_USART3_TX
            HAL_UART_Transmit_DMA(&huart3, (uint8_t *)&usart3_tx_buf[usart3.tx.Read], 1);
#else
            HAL_UART_Transmit_IT(&huart3, (uint8_t *)&usart3_tx_buf[usart3.tx.Read], 1);
#endif
            usart3.tx.Read = RBUF_NEXT_PT(usart3.tx.Read, 1, sizeof(usart3_tx_buf));
        }
    }
		else if(husart->Instance == USART2){
					USART2_Tx_Deal();
		}
		else{}
			
}

extern void WaitTxBufEmpty(void)
{
    while(usart3.tx.HaveData) {       //等发送数据完毕
    }
}


extern void USART3_IRQ_Deal(void)
{

}

extern void USART3_RxReaddy(void)
{
    if(usart3.rx.busy == 0) {
        if(usart3.rx.rxcount) {
            usart3.rx.timeout++;

            if(usart3.rx.timeout > RXTIMEOUT) {
                usart3.rx.busy = 1;
            }
        }
    }
}


static uint16_t USART3_GetRxBufDat(uint8_t *buf)
{
    uint16_t len;

    len = usart3.rx.rxcount;
    memcpy(buf, usart3_rx_buf, usart3.rx.rxcount);
    usart3.rx.rxcount = 0;
    usart3.rx.timeout = 0;
    usart3.rx.busy = 0;
#ifdef SUP_DMA_USART3_RX
            HAL_UART_Receive_DMA(&huart3, (uint8_t*)&usart3_rx_buf[usart3.rx.rxcount], 1);
#else
            HAL_UART_Receive_IT(&huart3, (uint8_t *)&usart3_rx_buf[usart3.rx.rxcount], 1); //重新打开串口中断
#endif	
    return len;
}

static void ProgramSwitch(void)
{
    USART3_Putchar(LAN_ETX);
    WaitTxBufEmpty();
    delayms_Lock(2);
    SetSwitchFlag();
    __set_PRIMASK(1);                       // 关闭所有中断
    NVIC_SystemReset();
}

extern void USART3_Putchar(uint8_t ch)
{
    uint16_t head;

    head = RBUF_NEXT_PT(usart3.tx.Write, 1, sizeof(usart3_tx_buf));

    if(head == usart3.tx.Read) { //满
         return;
    }

    usart3_tx_buf[usart3.tx.Write] = ch;
    usart3.tx.Write = head;

    if(usart3.tx.HaveData == 0) {
        // 发送还没有启动
        usart3.tx.HaveData = 1;
#ifdef SUP_DMA_USART3_TX
        HAL_UART_Transmit_DMA(&huart3, (uint8_t *)&usart3_tx_buf[usart3.tx.Read], 1);
#else
        HAL_UART_Transmit_IT(&huart3, (uint8_t *)&usart3_tx_buf[usart3.tx.Read], 1);
#endif
        usart3.tx.Read = RBUF_NEXT_PT(usart3.tx.Read, 1, sizeof(usart3_tx_buf));
    }

}

extern void USART3_Putbuf(uint8_t  *buf, uint16_t len)
{
    for(; len ; len--) {
        USART3_Putchar(*buf++);
    }
}


static Boolean  Decode_OK(void const *src, STR_PECRPORTOCOL *lan, uint16_t len)
{
    uint16_t checksum, i, length;
    uint8_t const *buf;
    uint8_t *destpt;

    buf = src;

    if(buf[1] == 0) { //length
        length = PROG_MAX_PAYLOAD_LEN;

    } else {
        length = buf[1];
    }

    if(len != (length + PROG_NUM_HEADER_BYTES + PROG_NUM_CHECKSUM_BYTES)) {
        return (FALSE);
    }

    checksum = crc_calculate((const uint8_t*)&buf[1], PROG_CORE_HEADER_LEN);

    for(i = 0; i < length; i++) {
        lan->buf[i] = buf[i + PROG_NUM_HEADER_BYTES];
    }

    crc_accumulate_buffer(&checksum, (const char*)&buf[PROG_NUM_HEADER_BYTES], length);
#if MAVLINK_CRC_EXTRA
    crc_accumulate(PRO_EXTRA_CRC, &checksum);//EXTRA
#endif

    if(buf[PROG_NUM_HEADER_BYTES + length] == (uint8_t)(checksum & 0xFF)
       && buf[PROG_NUM_HEADER_BYTES + length + 1] == (uint8_t)(checksum >> 8)) {
        destpt = &lan->magic;
        length += PROG_NUM_HEADER_BYTES;

        for(i = 0; i < length; i++) {
            destpt[i] = buf[i];
        }

        lan->type = 0;
        lan->command = lan->cmd;
        lan->length = length;
        lan->addr = (lan->para2 * 0x100 + lan->para1) * 0x100;

        return (TRUE);
    } else {
        return (FALSE);
    }

}

static uint8_t LAN_FUN_DoPackage(STR_PECRPORTOCOL *lan)
{
    switch(lan->command) {
    case PROG_FILE_DAT:
        return DownloadProgram(lan);

    case PROG_FILE_END:
        return NET_DownPrg;

    case PROG_FILE_CNFG_DAT:
        return DownloadConfig(lan);

    case PROG_FILE_CNFG_END:
        return NET_DownConfigPrg;

    default:
        return NET_ERR;
    }
}



extern void USART3_CheckSerial(void)
{
    uint16_t len;
    //uint8_t status;
    uint8_t rxbuf[usart3_rx_buf_Number];
    STR_PECRPORTOCOL lan;

    if(usart3.rx.busy != 1) {
        return;
    }

    len = USART3_GetRxBufDat(rxbuf);

    //for downloader
    if((rxbuf[0] == PRO_STX) && (rxbuf[5] == PRO_MSGID) && (len > 9)) {
        if(Decode_OK(&rxbuf[0], &lan, len) == TRUE) {
            switch(LAN_FUN_DoPackage(&lan)) {
            case NET_ACK:
                USART3_Putchar(LAN_ACK);
                break;

            case NET_DownPrg://end
                ProgramSwitch();
                break;

            case NET_DownConfigPrg:
                USART3_Putchar(LAN_ETX);
                break;

            case NET_ERR:
            default:
                USART3_Putchar(LAN_ERR);
                break;
            }
        }

        return;
    }

    //else if ((len == 1) && (rxbuf[0] == 'a'))
    //{
    //    ReceiveAGotoUpd();
    //}
#if 0
    status = mavlink_parse_config(rxbuf, len);
    if(status == PAR_FALSE) {
        AddParseCorruptCnt(rxbuf[MAVLINK_COMP_ID_POS]);
    } else if(status == PAR_OK) {
        AddParseRightCnt(rxbuf[MAVLINK_COMP_ID_POS]);
		
    } else {
        //do nothing
        
    }
#endif	
}

