#include "common.h"

static uint8_t count_24=0;

typedef struct {
    uint16_t             msCountDown;        // 毫秒延时计数器
    uint8_t             _msCount;
    uint8_t             _10msCount;
    uint8_t             _100msCount;
    Boolean             Pollintack;
} STR_POLLINT;

static STR_POLLINT r_pollint;


#ifdef EN_TIM3

static STR_POLLINT tim3_pollint;


TIM_HandleTypeDef htim3;

/* TIM3 init function */
extern void MX_TIM3_Init(void)
{
//TIM_ClockConfigTypeDef sClockSourceConfig;
//  TIM_MasterConfigTypeDef sMasterConfig;
    /* Set TIMx instance */
    htim3.Instance = TIM3;

    /* Initialize TIMx peripheral as follows:
         + Period = 10 - 1
         + Prescaler = ((SystemCoreClock / 2)/10000) - 1
         + ClockDivision = 0
         + Counter direction = Up
    */
    htim3.Init.Period            = 40-1;//263us irq
    htim3.Init.Prescaler         = 1080 - 1;////((1+TIM_Prescaler)/108M)*(1+TIM_Period)=((1+108))/108M)*(1+0)=0.000001s=1us
    htim3.Init.ClockDivision     = 0;
    htim3.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim3.Init.RepetitionCounter = 0;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if(HAL_TIM_Base_Init(&htim3) != HAL_OK) {
        /* Initialization Error */
        Error_Handler();
    }

    /*##-2- Start the TIM Base generation in interrupt mode ####################*/
    /* Start Channel1 */
    if(HAL_TIM_Base_Start_IT(&htim3) != HAL_OK) {
        /* Starting Error */
        Error_Handler();
    }
	#if 0
	 sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
	  {
	    _Error_Handler(__FILE__, __LINE__);
	  }

	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	  {
	    _Error_Handler(__FILE__, __LINE__);
	  }
	#endif

}

// 每毫秒更改软件定时器
static void ChangeTimeUs(void)
{
    tim3_pollint.Pollintack = 0;

    if(tim3_pollint.msCountDown > 0) {
        tim3_pollint.msCountDown--;
    }

    tim3_pollint._msCount = (tim3_pollint._msCount + 1) % 10;

    if(tim3_pollint._msCount == 0) {
        tim3_pollint._10msCount = (tim3_pollint._10msCount + 1) % 10;

        if(tim3_pollint._10msCount == 0) {
            tim3_pollint._100msCount = (tim3_pollint._100msCount + 1) % 10;
        }
    }
}

// 每毫秒更改软件定时器,成功更改返回1
static Boolean ChangeTimeUsCount_OK(void)
{
    __WFI();

    if(tim3_pollint.Pollintack) {
        ChangeTimeUs();
        return(TRUE);
    } else {
        return(FALSE);
    }
}


extern void Timer3_irq_fun(void)
{
	tim3_pollint.Pollintack = 1;
	//udp_TimerTick();
	
}


extern void pollintTim3(void)
{
    if(ChangeTimeUsCount_OK()) {
		pollint_spi_slave();
		//pollint_i2s_slave();
			
			
			//count_24++;
			
			if(count_24 > 24){
				if(Get_Audio_enflag())
				{
					count_24=0;
					MP2_Encode(NULL,0);
				}
			}
		}				
}


/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
}
#endif
/*******************************************************************************
* Function Name  : TimerTickNproc
* Description    : 系统1ms定时处理函数.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void TimerTickNproc(void)
{
    r_pollint.Pollintack = 1;
    //udp_TimerTick();
    //Audio_TimerTick();
    
}

// 以下1-9每1秒执行一次
static void Pollint1Sec(void)
{
    switch(r_pollint._100msCount) {
    case 1:
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    case 6:
		if(get_remoteLwip_udp_link())
		{
        	Led_Test();
			En_RX_Flow_Ctrl();
		}
		else
		{
		    udp_TimerTick();
		}
//audio_main();
        break;

    case 7:
        break;

    case 8:
        break;

    case 9:
        break;

    default:
        break;
    }
}

// 以下1-9每100毫秒执行一次
static void Pollint100mSec(void)
{
    switch(r_pollint._10msCount) {
    case 1:
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    case 6:
        break;

    case 7:
        break;

    case 8:
        break;

    case 9:
        break;

    default:
        Pollint1Sec();
        break;
    }
}

// 以下1-9每10毫秒执行一次
static void Pollint10mSec(void)
{
    switch(r_pollint._msCount) {
    case 1:
		
        break;

    case 2:
        key_Scan();
        break;

    case 3:
		  
        break;

    case 4:
        break;

    case 5:
        break;

    case 6:
        break;

    case 7:
        break;

    case 8:
        break;

    case 9:
        break;

    default:
        Pollint100mSec();
        break;
    }
}

// 每毫秒更改软件定时器
static void ChangeTime1mSec(void)
{
    r_pollint.Pollintack = 0;

    if(r_pollint.msCountDown > 0) {
        r_pollint.msCountDown--;
    }

    r_pollint._msCount = (r_pollint._msCount + 1) % 10;

    if(r_pollint._msCount == 0) {
        r_pollint._10msCount = (r_pollint._10msCount + 1) % 10;

        if(r_pollint._10msCount == 0) {
            r_pollint._100msCount = (r_pollint._100msCount + 1) % 10;
        }
    }
}

// 每毫秒更改软件定时器,成功更改返回1
static Boolean ChangeTimeCount_OK(void)
{
    __WFI();

    if(r_pollint.Pollintack) {
        ChangeTime1mSec();
        return(TRUE);
    } else {
        return(FALSE);
    }
}

// 毫秒延时,进入此延时,不处理任何与时间相关的事件,教准确,适用于短时间(毫秒级别)的延时
extern void delayms_Lock(uint16_t value)
{
    r_pollint.msCountDown = value + 1;

    while(r_pollint.msCountDown) {
        ChangeTimeCount_OK();
    }
}

// 函数    定时查询程序
extern void pollint(void)
{
    if(ChangeTimeCount_OK()) {
        // 每毫秒执行一次的事件
		PollintEth();
        USART3_RxReaddy();
        USART3_CheckSerial();
		config_CheckStatus();
        Pollint10mSec();
				
		if( !Get_Spi_Lock() ){
			pollint_spi_transmit();
		}
				
		//send_spi_dat();
		//pollint_ethernetif_link_status();
    }
}

// 毫秒延时,进入此延时,定时处理与时间相关的事件,因此不会太准确,适用于长时间(秒级别)的延时
extern void delay_NoLock(uint16_t value)
{
    r_pollint.msCountDown = value + 1;

    while(r_pollint.msCountDown) {
        pollint();
    }
}
