#include "common.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define FLASH_SECTOR_NULL           ((uint32_t)128U) /*!< Sector Number NULL   */


extern void romflash_dual_mode(void)
{
    FLASH_OBProgramInitTypeDef    OBInit;

    HAL_FLASH_OB_Unlock();
    /* Get the Dual bank configuration status */
    HAL_FLASHEx_OBGetConfig(&OBInit);

    /* if FLASH is configured in Dual Bank mode */
    if((OBInit.USERConfig & OB_NDBANK_SINGLE_BANK) == OB_NDBANK_SINGLE_BANK) {
        //printf("in Single Bank mode \r\n");
		} else {
        //printf("dest in Dual Bank mode \r\n");
    }
#if defined(DUAL_BANK)
    OBInit.USERConfig = OB_NDBANK_DUAL_BANK;
    HAL_FLASHEx_OBProgram(&OBInit);
    HAL_FLASHEx_OBGetConfig(&OBInit);

    /* Turn on LED3 if FLASH is configured in Dual Bank mode */
    if((OBInit.USERConfig & OB_NDBANK_SINGLE_BANK) == OB_NDBANK_SINGLE_BANK) {
        printf("dest in Single Bank mode \r\n");
    } else {
        printf("dest in Dual Bank mode \r\n");
    }
#endif
    //HAL_FLASH_OB_Lock();

}


static uint32_t GetSector(uint32_t addr)
{
    uint32_t sector = 0;

    switch(addr) {
    case ADDR_FLASH_SECTOR_0:
        sector = FLASH_SECTOR_0;
        break;

    case ADDR_FLASH_SECTOR_1:
        sector = FLASH_SECTOR_1;
        break;

    case ADDR_FLASH_SECTOR_2:
        sector = FLASH_SECTOR_2;
        break;

    case ADDR_FLASH_SECTOR_3:
        sector = FLASH_SECTOR_3;
        break;

    case ADDR_FLASH_SECTOR_4:
        sector = FLASH_SECTOR_4;
        break;

    case ADDR_FLASH_SECTOR_5:
        sector = FLASH_SECTOR_5;
        break;

    case ADDR_FLASH_SECTOR_6:
        sector = FLASH_SECTOR_6;
        break;

    case ADDR_FLASH_SECTOR_7:
        sector = FLASH_SECTOR_7;
        break;

    case ADDR_FLASH_SECTOR_8:
        sector = FLASH_SECTOR_8;
        break;

    case ADDR_FLASH_SECTOR_9:
        sector = FLASH_SECTOR_9;
        break;

    case ADDR_FLASH_SECTOR_10:
        sector = FLASH_SECTOR_10;
        break;

    case ADDR_FLASH_SECTOR_11:
        sector = FLASH_SECTOR_11;
        break;
#if defined(DUAL_BANK)

    case ADDR_FLASH_SECTOR_12:
        sector = FLASH_SECTOR_0;
        break;

    case ADDR_FLASH_SECTOR_13:
        sector = FLASH_SECTOR_13;
        break;

    case ADDR_FLASH_SECTOR_14:
        sector = FLASH_SECTOR_14;
        break;

    case ADDR_FLASH_SECTOR_15:
        sector = FLASH_SECTOR_15;
        break;

    case ADDR_FLASH_SECTOR_16:
        sector = FLASH_SECTOR_16;
        break;

    case ADDR_FLASH_SECTOR_17:
        sector = FLASH_SECTOR_17;
        break;

    case ADDR_FLASH_SECTOR_18:
        sector = FLASH_SECTOR_18;
        break;

    case ADDR_FLASH_SECTOR_19:
        sector = FLASH_SECTOR_19;
        break;

    case ADDR_FLASH_SECTOR_20:
        sector = FLASH_SECTOR_20;
        break;

    case ADDR_FLASH_SECTOR_21:
        sector = FLASH_SECTOR_21;
        break;

    case ADDR_FLASH_SECTOR_22:
        sector = FLASH_SECTOR_22;
        break;

    case ADDR_FLASH_SECTOR_23:
        sector = FLASH_SECTOR_23;
        break;
#endif

    default:
        sector = FLASH_SECTOR_NULL;
        printf("Null sector\r\n");
        break;
    }

    return sector;
}
//FLASH_TYPEPROGRAM_BYTE        ((uint32_t)0x00U)  /*!< Program byte (8-bit) at a specified address           */
//FLASH_TYPEPROGRAM_HALFWORD    ((uint32_t)0x01U)  /*!< Program a half-word (16-bit) at a specified address   */
//FLASH_TYPEPROGRAM_WORD        ((uint32_t)0x02U)  /*!< Program a word (32-bit) at a specified address        */
//FLASH_TYPEPROGRAM_DOUBLEWORD  ((uint32_t)0x03U)  /*!< Program a double word (64-bit) at a specified address */
extern Boolean romflash_clean_pages(uint32_t addr)
{

    /*Variable used for Erase procedure*/
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t useSector = 0;
    uint32_t SECTORError = 0;

    HAL_FLASH_Unlock();
    /* Allow Access to option bytes sector */

    /* Erase the user Flash area
      (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

    /* Get the 1st sector to erase */
    useSector = GetSector(addr);
		printf("clean pages %x\r\n",useSector);
    if(useSector == FLASH_SECTOR_NULL) {
        HAL_FLASH_Lock();
				printf("clean pages do nothing\r\n");
        return (TRUE);//do nothing
    }

    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector        = useSector;
    EraseInitStruct.NbSectors     = 1;

    /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
       you have to make sure that these data are rewritten before they are accessed during code
       execution. If this cannot be done safely, it is recommended to flush the caches by setting the
       DCRST and ICRST bits in the FLASH_CR register. */
    if(HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK) {
        /*
          Error occurred while sector erase.
          User can add here some code to deal with this error.
          SECTORError will contain the faulty sector and then to know the code error on this sector,
          user can call function 'HAL_FLASH_GetError()'
        */
        printf("Error occurred while sector erase");
        return (FALSE);
    }

    /* Lock the Flash to disable the flash control register access (recommended
       to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
    return (TRUE);
}



extern Boolean romflash_pages_write(const uint32_t suaddr, uint8_t *buf, int len)
{
    uint32_t addr;
    uint16_t  i = 0;
    addr  = suaddr;
    printf("romflash wirte len %d \r\n", len);
    HAL_FLASH_Unlock();

    for(i = 0; i < len; i++) {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, buf[i]) == HAL_OK) {
            addr = addr + 1;
        } else {
            /* Error occurred while writing data in Flash memory.
               User can add here some code to deal with this error */
            printf("Error occurred while writing data in Flash memory \r\n");
            return (FALSE);
        }
    }

    /* Lock the Flash to disable the flash control register access (recommended
       to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
    return (TRUE);
}


extern Boolean romflash_byte_write(uint32_t addr, uint8_t dat)
{

    HAL_FLASH_Unlock();

    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, dat) != HAL_OK) {
        /* Error occurred while writing data in Flash memory.
        	 User can add here some code to deal with this error */
        printf("Error occurred while writing data in Flash memory \r\n");
        return (FALSE);
    }

    /* Lock the Flash to disable the flash control register access (recommended
       to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
    return (TRUE);
}


extern void romflash_pages_read(const uint32_t suaddr, uint8_t *buf, int len)
{
    uint32_t address;
    uint16_t i;
    address = suaddr;

    for(i = 0; i < len; i++) {
        buf[i] = *(__IO uint8_t *)address;
        address = address + 1;
    }
}

extern uint32_t romflash_word_read(const uint32_t suaddr)
{
    uint32_t address;
    uint32_t dat;
    address = suaddr;
    dat = *(__IO uint32_t *)address;
    return dat;
}
