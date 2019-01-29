#include "common.h"

SPI_HandleTypeDef hspi5;
#ifdef SUP_DMA_SPI5_RX
DMA_HandleTypeDef hdma_spi5_rx;
#endif

#ifdef SUP_DMA_SPI5_TX
DMA_HandleTypeDef hdma_spi5_tx;
#endif

#define TEST_CNT

#ifdef TEST_CNT
static uint64_t subcnt=0,precnt=0; 
static uint64_t frame_cnt=0;
#endif


#define SPI_BUF_SIZE  2944//0x1000

typedef struct {
    volatile uint16_t       Write;
    volatile uint16_t       curWrite;
    volatile uint16_t       StartPos;
    volatile Boolean        StartFlg;
    volatile uint16_t       Read;
    volatile Boolean        HaveData;
    volatile uint16_t       len[SPI_BUFNB];
    uint8_t                 busy;
} SPI_STREAM;

static SPI_STREAM spiStream;


static char flow_lock = 0;

__ALIGN_BEGIN uint8_t g_es_stream[SPI_BUFNB][SPI_BUF_SIZE] __ALIGN_END;

__IO uint32_t  spi_len;
__IO uint8_t ISR_IO_FLAG = 0;
__IO uint8_t spi_lock;

static uint8_t index_M0;



#ifdef TEST_SPI
static uint8_t prehead;
#endif

void Client_flow_state(uint8_t state)
{
    if(state) {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
    }
}

/* SPI4 init function */
extern void MX_SPI5_Init(void)
{
    /* SPI5 parameter configuration*/
    hspi5.Instance = SPI5;
    hspi5.Init.Mode = SPI_MODE_SLAVE;
    hspi5.Init.Direction = SPI_DIRECTION_2LINES;
    //hspi5.Init.Direction = SPI_DIRECTION_2LINES_RXONLY;
    hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi5.Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi5.Init.NSS = SPI_NSS_SOFT;//SPI_NSS_SOFT;
    hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi5.Init.CRCPolynomial = 7;
    hspi5.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi5.Init.NSSPMode = SPI_NSS_PULSE_DISABLE; //SPI_NSS_PULSE_ENABLE;

    if(HAL_SPI_Init(&hspi5) != HAL_OK) {
        _Error_Handler(__FILE__, __LINE__);
    }

    ISR_IO_FLAG = 1;
    memset(&spiStream, 0x00, sizeof(spiStream));
    spiStream.StartPos = SPI_BUFNB;
#ifdef TEST_SPI
    prehead = 0;
#endif
    memset(&g_es_stream[0][0], 0x00, sizeof(g_es_stream));
    //HAL_SPI_Receive_DMA(&hspi5,g_es_stream[spiStream.Write], SPI_BUF_SIZE);
    __HAL_DMA_CLEAR_FLAG(&hdma_spi5_rx, DMA_FLAG_TCIF1_5 | DMA_FLAG_DMEIF1_5 | DMA_FLAG_FEIF1_5 | DMA_FLAG_HTIF1_5 | DMA_FLAG_TEIF1_5);
    HAL_SPI_Receive_2DMA_ex(&hspi5, g_es_stream[0], g_es_stream[1], SPI_BUF_SIZE);
    //__HAL_DMA_DISABLE(&hdma_spi5_rx);
    spiStream.StartFlg = 1;
	index_M0 = 0;
	spi_lock = 0;
    //uint16_t i=0;
    //g_es_stream[0][0] = 0xff;
    //g_es_stream[0][1] = 0xff;
    //g_es_stream[0][2] = 0x00;
    //g_es_stream[0][3] = 0x00;
    //for(i=4;i<SPI_BUF_SIZE;i++)
    //{
    //	g_es_stream[0][i] = i-4;
    //}
    //printf("isq init M0 %x M1 %x\r\n", hdma_spi5_rx.Instance->M0AR, hdma_spi5_rx.Instance->M1AR);
}


//M0 rx end  irq
extern void SPI5_RxCpltCallback(void)
{
    uint16_t head;
    uint16_t nextW;
	index_M0 = 0;
	//printf("M0\r\n");

    if(!ISR_IO_FLAG) {
        
        head = RBUF_NEXT_PT(spiStream.Write, 1, SPI_BUFNB);
        nextW = RBUF_NEXT_PT(head, 1, SPI_BUFNB);
 //       printf("spi5 M0 rx end	nextW %d write %d\r\n", nextW, spiStream.Write);
/*
        if(head == spiStream.Read) {
            spiStream.Read = RBUF_NEXT_PT(spiStream.Read, 1, SPI_BUFNB);
            spiStream.busy = 1;
        }
*/
	
		
			
        HAL_DMAEx_ChangeMemory(&hdma_spi5_rx, (uint32_t)g_es_stream[nextW], MEMORY0);
        spiStream.len[spiStream.Write] = SPI_BUF_SIZE;
        spiStream.Write = head;
		#ifdef TEST_CNT
		subcnt+=SPI_BUF_SIZE;
		#endif
        pollint_spi_slave();
    }

    else { //io isq int
//        printf("spi5 M0 io isq \r\n");
        //USART3_Putchar(0);
    }
	
}


//M1 rx end isq  or  io  flow  end spi
extern void SPI5_M1RxCpltCallback(void)
{
    uint16_t head;
    uint16_t nextW;
	index_M0 = 1;
	//printf("M1\r\n");
	
    if(!ISR_IO_FLAG) {
        
        head = RBUF_NEXT_PT(spiStream.Write, 1, SPI_BUFNB);
        nextW = RBUF_NEXT_PT(head, 1, SPI_BUFNB);

//        printf("spi5 M1 rx end	nextW %d write %d\r\n", nextW, spiStream.Write);
/*			
        if(head == spiStream.Read) {
            spiStream.Read = RBUF_NEXT_PT(spiStream.Read, 1, SPI_BUFNB);
            spiStream.busy = 1;
        }
*/
        HAL_DMAEx_ChangeMemory(&hdma_spi5_rx, (uint32_t)g_es_stream[nextW], MEMORY1);

        spiStream.len[spiStream.Write] = SPI_BUF_SIZE;
        spiStream.Write = head;
        pollint_spi_slave();
		#ifdef TEST_CNT
		subcnt+=SPI_BUF_SIZE;
		#endif
    }
		else { //io isq int
//        printf("spi5 M1 io isq \r\n");
        //USART3_Putchar(1);
    }
	
}

extern void  SPI5_RxHalfCpltCallback(void)
{

}



extern void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == SPI5) {
        //printf("spi5 error\r\n");
        USART3_Putchar(2);
    }
}





void Salve_deal_es(void)
{
    uint16_t dma_num = 0;
    uint16_t nextW;
    uint8_t i;


    if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9) == GPIO_PIN_RESET) { //下降沿  开启spi
			
			ISR_IO_FLAG = 0;
			// io? start
	//		if(flow_lock == 1)
	//			return;
	//		flow_lock = 1;
			
        
        //__HAL_SPI_ENABLE(&hspi5);
        __HAL_DMA_ENABLE(&hdma_spi5_rx);
        //Client_flow_state(0);
       // nextW = RBUF_NEXT_PT(spiStream.Write, 1, SPI_BUFNB);
//		printf("spi5 rx start Read %d \r\n",spiStream.Read);
        //if(nextW == spiStream.Read) {
        //    spiStream.Read = RBUF_NEXT_PT(spiStream.Read, 1, SPI_BUFNB);
        //    spiStream.busy = 1;
        //}

//        printf("spi5 rx start  %d \r\n",spiStream.Write);
        spiStream.StartPos  = spiStream.Write;
        spiStream.len[spiStream.Write] = 0;

		#ifdef TEST_CNT
		frame_cnt++;
		if(precnt != subcnt)
		{
		     precnt = subcnt;
			 printf("poll index i %lld frame_cnt %lld\r\n",precnt,frame_cnt);
		}
		subcnt = 0;
		#endif
    }

    else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9) == GPIO_PIN_SET) { //上升沿  关闭spi
			
			 ISR_IO_FLAG = 1;
			
	//		if(flow_lock == 0)
	//			 return;
	//		flow_lock = 0;
			
			
				
        while(HAL_SPI_GetState(&hspi5) != HAL_SPI_STATE_BUSY_RX);
				while(HAL_DMA_GetState(&hdma_spi5_rx) == HAL_DMA_STATE_READY);
        //__HAL_SPI_DISABLE(&hspi5);

        __HAL_DMA_DISABLE(&hdma_spi5_rx);

        while(__HAL_DMA_GET_FLAG(&hdma_spi5_rx, DMA_FLAG_TCIF1_5) == RESET);

        dma_num = __HAL_DMA_GET_COUNTER(&hdma_spi5_rx);

        __HAL_DMA_SET_COUNTER(&hdma_spi5_rx, SPI_BUF_SIZE);
//        printf("dma_num %d index %d r=%d w=%d \r\n",dma_num,index_M0,spiStream.Read,spiStream.Write);

			//	if(dma_num == SPI_BUF_SIZE){
			//		dma_num = 0;
			//	}
			
				if(SPI_BUF_SIZE != dma_num) { //buf have data
            spiStream.len[spiStream.Write] = SPI_BUF_SIZE - dma_num;
        }
				else
				{
					spiStream.len[spiStream.Write] = SPI_BUF_SIZE;
				}
				spiStream.Write = RBUF_NEXT_PT(spiStream.Write, 1, SPI_BUFNB);
						nextW = RBUF_NEXT_PT(spiStream.Write, 1, SPI_BUFNB);
				 if(index_M0) {
					hdma_spi5_rx.Instance->CR  |= DMA_SxCR_CT;
					HAL_DMAEx_ChangeMemory(&hdma_spi5_rx, (uint32_t)g_es_stream[nextW], MEMORY0);
				} else {
					hdma_spi5_rx.Instance->CR  &=  ~ DMA_SxCR_CT;
					HAL_DMAEx_ChangeMemory(&hdma_spi5_rx, (uint32_t)g_es_stream[nextW], MEMORY1);
				}


			
        pollint_spi_slave();

		////MX_LWIP_Process();
        //for(i = 0; i < SPI_BUFNB; i++) {
        //    printf("index i %d  %x  %x\r\n", i, g_es_stream[i][0], g_es_stream[i][1]);
        //}

        //printf("isq end  dma_num %d write %d  M0 %x M1 %x\r\n", dma_num, spiStream.Write,hdma_spi5_rx.Instance->M0AR,hdma_spi5_rx.Instance->M1AR);
        //HAL_SPI_Receive_2DMA_ex(&hspi5, g_es_stream[spiStream.Write], g_es_stream[nextW], SPI_BUF_SIZE);
        //HAL_DMAEx_ChangeMemory(&hdma_spi5_rx,(uint32_t)g_es_stream[spiStream.Write],MEMORY0);
        //HAL_DMAEx_ChangeMemory(&hdma_spi5_rx, (uint32_t)g_es_stream[nextW], MEMORY1);
    }
}

extern void En_RX_Flow_Ctrl(void)
{
    if(spiStream.StartFlg) {
        spiStream.StartFlg  = 0;
        /* EXTI interrupt init*/
        HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 1); //HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 1);   //！
        HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
        Client_flow_state(0);
    }
}


extern void pollint_spi_slave(void)
{
    //uint16_t i;

	 if(spi_lock)
	{
		USART3_Putchar(33);
		return;
	}
	spi_lock = 1;
    if(spiStream.Write == spiStream.Read) {
        // 数据已经发送完
        //if(get_remoteLwip_udp_link())
        //{
        //	spiStream.Write = 1;
        //	spiStream.Read = 0;
        //	spiStream.StartPos =0;
        //	spiStream.len[spiStream.Read]=SPI_BUF_SIZE;
        //}
    } else if(udp_GetState() == HAL_ETH_STATE_READY && get_remoteLwip_udp_link()){

        if(spiStream.StartPos == spiStream.Read) {
#ifdef TEST_SPI
            prehead++;

            if(prehead != g_es_stream[spiStream.Read][3]) {
                USART3_Putchar(g_es_stream[spiStream.Read][3]);
                prehead  = g_es_stream[spiStream.Read][3];

//               for(i = 0; i < SPI_BUFNB; i++) {
//                   printf("index i %d  %x  %x\r\n", i, g_es_stream[i][2], g_es_stream[i][3]);
//               }
            }

#endif
            spiStream.StartPos = SPI_BUFNB;
            Start_TS_Frame();
			
        }

        if(spiStream.len[spiStream.Read] && GetTSmuxState() == TS_MUX_IDLE) 
		{
			
            PackVideoTs(&g_es_stream[spiStream.Read][0], spiStream.len[spiStream.Read]);
			 #ifdef TEST_CNT
	//		subcnt += spiStream.len[spiStream.Read];
			 #endif
            spiStream.len[spiStream.Read] = 0;
           
            spiStream.Read = RBUF_NEXT_PT(spiStream.Read, 1, SPI_BUFNB);
        }

        //if(spiStream.busy) {
        //    spiStream.busy = 0;
        //    USART3_Putchar(3);
            //printf("busy spi read\r\n");
        //}
    }
	spi_lock = 0;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint32_t regvalue=0;

    if(GPIO_Pin  == GPIO_PIN_9) {

        Salve_deal_es();
    }
		
	if(GPIO_Pin  == GPIO_PIN_0){

		Link_Callback();
	}

}

extern uint8_t Get_Spi_Lock(void)
{
	return spi_lock;
}

