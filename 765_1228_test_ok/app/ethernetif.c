/**
  ******************************************************************************
  * File Name          : ethernetif.c
  * Description        : This file provides code for the configuration
  *                      of the ethernetif.c MiddleWare.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "ethernetif.h"
#include <string.h>
#include "common.h"

#define ETH_HWADDR_LEN 6

uint8_t MACAddr[ETH_HWADDR_LEN] = {0};

/* Private define ------------------------------------------------------------*/

/* Network interface name */
#define IFNAME0 's'
#define IFNAME1 't'

/* Private variables ---------------------------------------------------------*/
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;/* Ethernet Rx MA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;/* Ethernet Tx DMA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_END; /* Ethernet Receive Buffer */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
__ALIGN_BEGIN uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_END; /* Ethernet Transmit Buffer */


/* Global Ethernet handle */
ETH_HandleTypeDef heth;


/* Private functions ---------------------------------------------------------*/

void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct;

  if(ethHandle->Instance==ETH)
  {
        /* USER CODE BEGIN ETH_MspInit 0 */

        /* USER CODE END ETH_MspInit 0 */
        /* Enable Peripheral clock */
        __HAL_RCC_ETH_CLK_ENABLE();
  
    /**ETH GPIO Configuration    
    PG14     ------> ETH_TXD1
    PG13     ------> ETH_TXD0
    PG11     ------> ETH_TX_EN
    PC1     ------> ETH_MDC
    PA1     ------> ETH_REF_CLK
    PC4     ------> ETH_RXD0
    PA2     ------> ETH_MDIO
    PC5     ------> ETH_RXD1
    PA7     ------> ETH_CRS_DV 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_13|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
        GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral interrupt init */
    //HAL_NVIC_SetPriority(ETH_IRQn, 0, 0);
    //HAL_NVIC_EnableIRQ(ETH_IRQn);
  /* USER CODE BEGIN ETH_MspInit 1 */

  /* USER CODE END ETH_MspInit 1 */
  }
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
{
  if(ethHandle->Instance==ETH)
  {
        /* USER CODE BEGIN ETH_MspDeInit 0 */

        /* USER CODE END ETH_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_ETH_CLK_DISABLE();
  
    /**ETH GPIO Configuration    
    PG14     ------> ETH_TXD1
    PG13     ------> ETH_TXD0
    PG11     ------> ETH_TX_EN
    PC1     ------> ETH_MDC
    PA1     ------> ETH_REF_CLK
    PC4     ------> ETH_RXD0
    PA2     ------> ETH_MDIO
    PC5     ------> ETH_RXD1
    PA7     ------> ETH_CRS_DV 
    */
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_14|GPIO_PIN_13|GPIO_PIN_11);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(ETH_IRQn);

        /* USER CODE BEGIN ETH_MspDeInit 1 */

        /* USER CODE END ETH_MspDeInit 1 */
    }
}

extern void ethernetif_mac_conf(uint8_t *macadr)
{
    memcpy(MACAddr, macadr, ETH_HWADDR_LEN);
}


/*******************************************************************************
                       LL Driver Interface ( LwIP stack --> ETH)
*******************************************************************************/
/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
extern void ethernetif_init(void)
{ 
  HAL_StatusTypeDef hal_eth_init_status;
  
/* Init ETH */

    heth.Instance = ETH;
   heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_DISABLE;
  heth.Init.Speed = ETH_SPEED_100M;
  heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
  heth.Init.PhyAddress = KSZ8041NL_PHY_ADDRESS;
    heth.Init.MACAddr = &MACAddr[0];
    heth.Init.RxMode = ETH_RXPOLLING_MODE;
    heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
    heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;


    hal_eth_init_status = HAL_ETH_Init(&heth);


    /* Initialize Tx Descriptors list: Chain Mode */
    HAL_ETH_DMATxDescListInit(&heth, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);

    /* Initialize Rx Descriptors list: Chain Mode  */
    HAL_ETH_DMARxDescListInit(&heth, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);


    /* Enable MAC and DMA transmission and reception */
    HAL_ETH_Start(&heth);

	
   HAL_ETH_WritePHYRegister(&heth, PHY_ITSCR, LINK_DOWN_IRQ_ENABLE | LINK_UP_IRQ_ENABLE);  //  enter PB0 interrupt server
		

}


 HAL_ETH_StateTypeDef udp_GetState(void)
 {
	return HAL_ETH_GetState(&heth);
 }

uint32_t udp_ts_output(uint8_t *buf,uint16_t len)
{
    uint32_t errval;
    uint8_t *buffer = (uint8_t *)(heth.TxDesc->Buffer1Addr);
    __IO ETH_DMADescTypeDef *DmaTxDesc;
    uint32_t framelength = 0;

	uint8_t *dat;

    DmaTxDesc = heth.TxDesc;

	dat = buf;

    framelength = len;


	if(framelength>ETH_RX_BUF_SIZE)
	{
	    errval = 0;
		goto error;
	}
    /* Is this buffer available? If not, goto error */
    if((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET) {
        errval = 0;
        goto error;
    }

    /* Copy the remaining bytes */
    memcpy((uint8_t*)((uint8_t*)buffer), (uint8_t*)((uint8_t*)dat), framelength);

	if(HAL_OK != HAL_ETH_TransmitFrame(&heth, framelength)) {
	    errval = framelength;
	}

error:

    /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
    if((heth.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET) {
		printf("DMASR error ETH_DMASR_TUS %d\r\n",heth.Instance->DMASR);
        /* Clear TUS ETHERNET DMA flag */
        heth.Instance->DMASR = ETH_DMASR_TUS;

        /* Resume DMA transmission*/
        heth.Instance->DMATPDR = 0;
    }
	//printf("DMASR %d\n",heth.Instance->DMASR);
    return errval;
}


extern uint32_t udp_ts_output2(PBUF *p,uint8_t *buf1,uint16_t len1,uint8_t *buf2,uint16_t len2)
{
    uint32_t errval;
    PBUF *q;
    uint8_t *buffer = (uint8_t *)(heth.TxDesc->Buffer1Addr);
    __IO ETH_DMADescTypeDef *DmaTxDesc;
    uint32_t framelength = 0;
	uint32_t hearlength = 0;
	uint32_t bufferoffset = 0;

	uint8_t ret;
	uint16_t datlen;

	
	heth.State = HAL_ETH_STATE_BUSY_TX;
    DmaTxDesc = heth.TxDesc;
    bufferoffset = 0;
	q = p;


    framelength = q->tot_len;
	datlen =  q->len;
	hearlength= framelength - datlen;

	if(framelength>ETH_RX_BUF_SIZE)
	{
	    errval = 0;
		goto error;
	}
    /* Is this buffer available? If not, goto error */
    if((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET) {
        errval = 0;
        goto error;
    }

    /* Copy the remaining bytes */
    memcpy((uint8_t*)((uint8_t*)buffer), (uint8_t*)((uint8_t*)q->payload), hearlength);
	
	bufferoffset += hearlength;
	if(len1)
		memcpy((uint8_t*)((uint8_t*)buffer + bufferoffset), (uint8_t*)((uint8_t*)buf1), len1);

	if(len2)
	{
		bufferoffset += len1;
		memcpy((uint8_t*)((uint8_t*)buffer + bufferoffset), (uint8_t*)((uint8_t*)buf2), len2);
    
	}

	ret = HAL_ETH_TransmitFrame(&heth, framelength);
  	if(HAL_OK != ret) {
			errval = 0;
	}
	else
	{
			errval = len1+len2;
	}
error:

    /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
    if((heth.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET) {
		USART3_Putchar(4);
        /* Clear TUS ETHERNET DMA flag */
        heth.Instance->DMASR = ETH_DMASR_TUS;

        /* Resume DMA transmission*/
        heth.Instance->DMATPDR = 0;
    }
	 heth.State = HAL_ETH_STATE_READY;
    return errval;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
   */
extern uint16_t   eth_irq_input(uint8_t *payload)
{
   
    uint16_t len = 0;
    uint8_t *buffer;
    __IO ETH_DMADescTypeDef *dmarxdesc;

    uint32_t i = 0;


    /* get received frame */
    if(HAL_ETH_GetReceivedFrame(&heth) != HAL_OK)
        return 0 ;

    /* Obtain the size of the packet and put it into the "len" variable. */
    len = heth.RxFrameInfos.length;
    buffer = (uint8_t *)heth.RxFrameInfos.buffer;

     /* Copy remaining data in pbuf */
     //memcpy((uint8_t*)((uint8_t*)payload ), (uint8_t*)((uint8_t*)buffer), len);
	nicIP(buffer,len);
    /* Release descriptors to DMA */
    /* Point to first descriptor */
    dmarxdesc = heth.RxFrameInfos.FSRxDesc;

    /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
    for(i = 0; i < heth.RxFrameInfos.SegCount; i++) {
        dmarxdesc->Status |= ETH_DMARXDESC_OWN;
        dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
    }

    /* Clear Segment_Count */
    heth.RxFrameInfos.SegCount = 0;

    /* When Rx Buffer unavailable flag is set: clear it and resume reception */
    if((heth.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET) {
        /* Clear RBUS ETHERNET DMA flag */
        heth.Instance->DMASR = ETH_DMASR_RBUS;
        /* Resume DMA reception */
        heth.Instance->DMARPDR = 0;
    }
	
	return len;
}


extern void PollintEth(void)
{
	if(get_remoteLwip_udp_link() == NET_UNLINK)
	{
		eth_irq_input(NULL);
	}
}
extern void Link_Callback(void)
{
	uint32_t regvalue=0;
	HAL_ETH_ReadPHYRegister(&heth, PHY_ITSCR, &regvalue); //501 link up   504 link down
	if(	regvalue & LINK_DOWN_OCCURRED ){
		Set_remoteLwip_udp_link(NET_UNLINK);
	}
}

extern void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef * heth)
{
    printf("HAL_ETH_TxCpltCallback\r\n");
}


extern void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef * heth)
{
	
	//eth_irq_input(NULL);
    printf("HAL_ETH_RxCpltCallback\r\n");
}


extern void HAL_ETH_ErrorCallback(ETH_HandleTypeDef * heth)
{
    printf("HAL_ETH_ErrorCallback\n");
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

