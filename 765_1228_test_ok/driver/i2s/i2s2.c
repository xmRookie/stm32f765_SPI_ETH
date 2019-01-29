#include "common.h"

#define i2s2_rx_buf_size 1152//1152//4608 
#define i2s_buf_nb 8

#define rx_size 1152 // 1152//4608

volatile unsigned char audio_Write;
volatile unsigned char audio_Read;


__IO uint8_t i2s_lock;

//#define Debug


I2S_HandleTypeDef hi2s2;
DMA_HandleTypeDef hdma_i2s2_rx;

//short i2s2_rx_buf[i2s_buf_nb][i2s2_rx_buf_size];
uint32_t  i2s2_rx_buf[i2s_buf_nb][i2s2_rx_buf_size];
short pcm_buf[2304];




/* I2S2 init function */
extern void MX_I2S2_Init(void)
{

  hi2s2.Instance = SPI2;
  hi2s2.Init.Mode = I2S_MODE_SLAVE_RX;
  hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B_EXTENDED;//I2S_DATAFORMAT_16B;//I2S_DATAFORMAT_32B;//
  hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_48K;
  hi2s2.Init.CPOL = I2S_CPOL_LOW;
  hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
  if (HAL_I2S_Init(&hi2s2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
		i2s_lock=0;
	
	audio_Write = 0;
	audio_Read = 0;
	
	HAL_I2S_Receive_2DMA_EX(&hi2s2, (uint16_t*)&i2s2_rx_buf[0][0],(uint16_t*)&i2s2_rx_buf[1][0],rx_size);		
	//printf(" HAL_I2S_Receive_2DMA_EX\r\n ");
}



extern void I2S2_RxCpltCallback(void)
{
	uint8_t nextW;
	uint8_t head;
	
	
	head = RBUF_NEXT_PT(audio_Write, 1, i2s_buf_nb);
	nextW = RBUF_NEXT_PT(head, 1, i2s_buf_nb);

	if(head == audio_Read){
		//audio_Read = RBUF_NEXT_PT(audio_Read, 1, i2s_buf_nb);
		//i2s2.busy = 1;
		return;
	}
	audio_Write = head;
	HAL_DMAEx_ChangeMemory(&hdma_i2s2_rx, (uint32_t)i2s2_rx_buf[nextW], MEMORY0);

	pollint_i2s_slave();
	
}

extern void I2S2_M1RxCpltCallback(void)
{
	//can change M0 addr
	uint8_t nextW;
	uint8_t head;
	
	head = RBUF_NEXT_PT(audio_Write, 1, i2s_buf_nb);
	nextW = RBUF_NEXT_PT(head, 1, i2s_buf_nb);
	
	

	if(head == audio_Read){
		//audio_Read = RBUF_NEXT_PT(audio_Read, 1, i2s_buf_nb);
		return;
	}
	audio_Write = head;
	HAL_DMAEx_ChangeMemory(&hdma_i2s2_rx, (uint32_t)i2s2_rx_buf[nextW], MEMORY1);

	pollint_i2s_slave();	
}

extern void pollint_i2s_slave(void)
{
	//short pcm_buf[2304];
	static uint16_t cnt_i2s=0;
	uint16_t size = 0;
	uint16_t i = 0;
	uint16_t j = 0;
	
	if(i2s_lock)
	{
		USART3_Putchar(43);
		return;
	}
	i2s_lock = 1;


   if(audio_Write != audio_Read)
   {
		 size = i2s2_rx_buf_size + cnt_i2s;
#if 0		 
		for(i=cnt_i2s; i<size; )
		{
			pcm_buf[i>>1] = i2s2_rx_buf[audio_Read][i];
			i += 2;
		}
#else   //16b  extend
		j=0;
		for(i=cnt_i2s; i<size; i++){
				pcm_buf[i] =  (short)( i2s2_rx_buf[audio_Read][j] );    //  i2s2_rx_buf[audio_Read][i] >> 16 ¡§?y2?|¨¬?
			j++;
			}		 
#endif		
			
		cnt_i2s += i2s2_rx_buf_size;
		audio_Read = RBUF_NEXT_PT(audio_Read, 1, i2s_buf_nb);
		
		if(cnt_i2s == 2304){
			cnt_i2s = 0;
			if(udp_GetState() == HAL_ETH_STATE_READY && get_remoteLwip_udp_link())
				MP2_Encode((short *)pcm_buf,2304);
		}
    	
   }
    i2s_lock = 0;
}





