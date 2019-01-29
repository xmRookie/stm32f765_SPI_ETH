#include "common.h"

static u8_t LedCount;


extern void led_Par_Init(void)
{
    LedCount = 0;
}

extern void MX_LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /* GPIO Ports Clock Enable */
    //__HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14 | GPIO_PIN_7, GPIO_PIN_RESET);

    /*Configure GPIO pin : PB7 */
    /*Configure GPIO pins : PB14 PB7 */
    GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    led_Par_Init();
}

extern void Led1_Set(Boolean status)
{
    if(status) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    }
}


extern void Led2_Set(Boolean status)
{
    if(status) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    }
}


extern void Led_Test(void)
{
    LedCount++;

    if(LedCount == 1) {
   //     Led1_Set(OFF_ST);
        Led2_Set(ON_ST);
    } else if(LedCount == 2) {
   //     Led1_Set(ON_ST);
       Led2_Set(OFF_ST);
        LedCount = 0;
    }
}


extern void Status_busy(uint8_t st)
{
	if(st)
	{
		 Led1_Set(ON_ST);//red
	}
	else
	{
		Led1_Set(OFF_ST);// open
	}
}
