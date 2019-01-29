#include "common.h"

SPI_HandleTypeDef hspi1;

//#define TS_LEN 0x2000

DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;
//__IO uint8_t  io_isq_flag = 0;
//__IO uint8_t  resend_key_flag = 0;
//__IO uint8_t enPollint;
//__IO uint8_t Tx_end_flag;
//__IO uint16_t index;
//static uint8_t spi1_tx_buf[TS_LEN];
//static uint8_t spi1_rx_buf[spi_rx_buf_Number];
#define Status_True 1
#define Status_False 0
static uint8_t Null_arr[5] = {0};
uint8_t Fpga_Write_Data[4]={0};
uint8_t Fpga_Read_Data[4]={0};


void flow_ctrl_set(Boolean status)
{
    if(status) {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);
    }
}


/* SPI1 init function */
extern void MX_SPI1_Init(void)
{
	uint32_t i;
    /* SPI1 parameter configuration*/
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;//SPI_NSS_HARD_OUTPUT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;

    if(HAL_SPI_Init(&hspi1) != HAL_OK) {
        _Error_Handler(__FILE__, __LINE__);
    }

	//memset((uint8_t *)&spi1_tx_buf[0],0x55, sizeof(spi1_tx_buf));
	setenPollint(0);

	//flow_ctrl_gpio();
	//flow_ctrl_set(1);
}
//--------- only ************************
extern void SPI1_Communicate(void)
{
	uint8_t get_error;
	Write_GPIOA_Pin4(Status_False);
	
		if( HAL_SPI_Transmit_IT(&hspi1, &Fpga_Write_Data[0], 3) != HAL_OK){
					//printf(" transmit data error\r\n");
				USART2_Putchar( 9 );
			}
	
			get_error = HAL_SPI_TransmitReceive_IT(&hspi1, Null_arr, &Fpga_Read_Data[0], 3);
		if( get_error != HAL_OK ){
		//	printf(" Recevie data error\r\n");
				USART2_Putchar( get_error );
		}
}
//--------- only ************************
extern void SPI1_RxCpltCallback(void)
{
	
	Write_GPIOA_Pin4(Status_True);

//	USART2_Putbuf(Fpga_Write_Data,3);     //stm32 wirte data
	
	USART2_Putbuf(Fpga_Read_Data,3);    //  fpga return data 

	
		//	Status_busy(0);
}
extern void SPI1_TxCpltCallback(void)
{
	
/*
	//printf("spi1 send end\n");
	
	if(!io_isq_flag)
	{
		Tx_end_flag = 1;
	//	printf("spi1 send end2\n");
		flow_ctrl_set(1);
	}
	else
	{
		resend_key_flag =1;
//		printf("\r\n spi1 resend_key_flag end2\r\n");
	}
*/
}

extern void Write_GPIOA_Pin4(uint8_t Status)
{
	if(Status)
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4,GPIO_PIN_SET) ;
	else
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4,GPIO_PIN_RESET) ;
}



extern void SPI1_Flow_deal(void)
{
	 
}


extern void En_flow_ctrl(void)
{
	__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_0);
	__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_2);

	    //中断线0
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	 //中断线2
    //HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);
    //HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	//index = 1;

}
void send_spi_dat(void)
{

	
}

void setenPollint(uint8_t en)
{

}

uint8_t getenPollint(void)
{

}

