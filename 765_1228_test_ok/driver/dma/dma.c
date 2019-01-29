#include "common.h"
 /** 
  * Enable DMA controller clock
  */
extern void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();
#ifdef SUP_DMA_USART3_RX
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
#endif

#ifdef SUP_DMA_USART3_TX
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
#endif
  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
 // HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  //HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
#ifdef SUP_DMA_SPI5_TX
   /* DMA2_Stream4_IRQn interrupt configuration */
 // HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 0, 0);
  //HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
#endif

#ifdef SUP_DMA_SPI5_RX
  /* DMA2_Stream5_IRQn interrupt configuration */
 HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);// HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
#endif  

#ifdef SUP_DMA_I2S2_RX
		HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
   HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

#endif
	

}
