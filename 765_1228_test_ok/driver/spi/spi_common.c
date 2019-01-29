#include "common.h"


extern void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI5) {
		SPI5_RxCpltCallback();
	}
	//if(hspi->Instance == SPI1)
	//	{
	//		SPI1_RxCpltCallback();
	//	}
}

extern void HAL_SPI_M1RxCpltCallback(SPI_HandleTypeDef *hspi)
{
		if(hspi->Instance == SPI5) {
		SPI5_M1RxCpltCallback();
	}
}

//extern void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi)
//{
//	if(hspi->Instance == SPI5) {
//		SPI5_RxHalfCpltCallback();
//	}
//}

extern void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
		
		if(hspi->Instance == SPI1)
		{
			SPI1_TxCpltCallback();
		}
		
}



extern  void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
		if(hspi->Instance == SPI1)
		{
			SPI1_RxCpltCallback();
		}
		
		
}

//i2s2 add
extern void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
		if(hi2s->Instance == SPI2)
		{
			I2S2_RxCpltCallback();
		}
}
extern void HAL_I2S_M1RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	if(hi2s->Instance == SPI2)
		{
			I2S2_M1RxCpltCallback();
		}
}


