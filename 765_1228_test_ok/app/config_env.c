#include "common.h"
#include "ethernetif.h"
//#define DEBUG_CONFIG_ENV

NETIF_PAR netif_par;
NETIF_REMOTE_PAR netif_remote_par;


//ADDR_FLASH_SECTOR_1
#define ENV_START_ADDR   ADDR_FLASH_SECTOR_7

#define CFG_INFO_LEN   0x100

#define FATY_ADDR   (ENV_START_ADDR)   /* Start @ of user Flash area */

#define USER_ADDR   (FATY_ADDR+CFG_INFO_LEN+REG_BUF_LEN)   /* Start @ of user Flash area */

#define FPGA_ADDR   (USER_ADDR+CFG_INFO_LEN+REG_BUF_LEN)   /* Start @ of user Flash area */

#define ENV_END_ADDR     ((uint32_t)0x08100000-1)

#define SoftKey1_VAL        0xa5a5a5a5
#define SoftKey2_VAL        0x5a5a5a5a

REG_Info  FactoryReg;

REG_Info  UserReg;

static uint8_t current_tx_seq;

uint8_t FPGADatBuf[FPGA_REG_BUF_LEN];

extern void romflash_Factory_reg_get(void)
{
    CFG_Filestr  envfile;
    memset(&FactoryReg.Dat.ctr.m_U_Char, 0x00, sizeof(FactoryReg));
    if(romflash_word_read(FATY_ADDR) != SoftKey1_VAL) {
        romflash_clean_pages(ENV_START_ADDR);
        envfile.Key = SoftKey1_VAL;

        FactoryReg.Dat.ctr.m_U_Char = 0;
        FactoryReg.Dat.sRxSum = 0;
        FactoryReg.Dat.sRxCorrupt = 0;
        FactoryReg.Dat.ethRxSum = 0;
        FactoryReg.Dat.ethRxCorrupt = 0;
        FactoryReg.Dat.NoUsed_01 = 0;

        FactoryReg.Dat.mac[0] = FACTORY_MAC_B0;
        FactoryReg.Dat.mac[1] = FACTORY_MAC_B1;
        FactoryReg.Dat.mac[2] = FACTORY_MAC_B2;
        FactoryReg.Dat.mac[3] = FACTORY_MAC_B3;
        FactoryReg.Dat.mac[4] = FACTORY_MAC_B4;
        FactoryReg.Dat.mac[5] = FACTORY_MAC_B5;

        FactoryReg.Dat.srcIp[0] = FACTORY_IP_0;
        FactoryReg.Dat.srcIp[1] = FACTORY_IP_1;
        FactoryReg.Dat.srcIp[2] = FACTORY_IP_2;
        FactoryReg.Dat.srcIp[3] = FACTORY_IP_3;

        FactoryReg.Dat.netmask[0] = FACTORY_NETMASK_0;
        FactoryReg.Dat.netmask[1] = FACTORY_NETMASK_1;
        FactoryReg.Dat.netmask[2] = FACTORY_NETMASK_2;
        FactoryReg.Dat.netmask[3] = FACTORY_NETMASK_3;

        FactoryReg.Dat.gateway[0] = FACTORY_GATEWAY_0;
        FactoryReg.Dat.gateway[1] = FACTORY_GATEWAY_1;
        FactoryReg.Dat.gateway[2] = FACTORY_GATEWAY_2;
        FactoryReg.Dat.gateway[3] = FACTORY_GATEWAY_3;

        FactoryReg.Dat.txPort = FACTORY_UDP_OUT_PORT;//0x5003
        FactoryReg.Dat.rxPort = FACTORY_UDP_IN_PORT;//0x5004

        FactoryReg.Dat.remoteIp[0] = FACTORY_REMOTE_IP_0;
        FactoryReg.Dat.remoteIp[1] = FACTORY_REMOTE_IP_1;
        FactoryReg.Dat.remoteIp[2] = FACTORY_REMOTE_IP_2;
        FactoryReg.Dat.remoteIp[3] = FACTORY_REMOTE_IP_3;
        envfile.ckSum = crc_calculate((const uint8_t *)&FactoryReg.Dat.ctr.m_U_Char, sizeof(REG_Data));

        romflash_pages_write(FATY_ADDR, (uint8_t *)&envfile.Key, sizeof(envfile));
        romflash_pages_write(FATY_ADDR + CFG_INFO_LEN, (uint8_t *)&FactoryReg.Dat.ctr.m_U_Char, sizeof(REG_Data));
    } else {
        romflash_pages_read(FATY_ADDR + CFG_INFO_LEN, (uint8_t *)&FactoryReg.Dat.ctr.m_U_Char, sizeof(REG_Data));
    }
}

extern void romflash_reg_refresh(void)
{
    romflash_clean_pages(ENV_START_ADDR);
    romflash_Factory_reg_get();
    romflash_user_reg_get();
}



extern void romflash_user_reg_get(void)
{
    CFG_Filestr  envfile;
    memset(&UserReg.Dat.ctr.m_U_Char, 0x00, sizeof(UserReg));
    if(romflash_word_read(USER_ADDR) != SoftKey2_VAL) {
        envfile.Key = SoftKey2_VAL;
        memcpy((uint8_t *)&UserReg.Dat.ctr.m_U_Char, (uint8_t *)&FactoryReg.Dat.ctr.m_U_Char, sizeof(REG_Data));
        envfile.ckSum = crc_calculate((const uint8_t *)&UserReg.Dat.ctr.m_U_Char, sizeof(REG_Data));
        romflash_pages_write(USER_ADDR, (uint8_t *)&envfile.Key, sizeof(envfile));
        romflash_pages_write(USER_ADDR + CFG_INFO_LEN, (uint8_t *)&UserReg.Dat.ctr.m_U_Char, sizeof(REG_Data));
    } else {
        romflash_pages_read(USER_ADDR + CFG_INFO_LEN, (uint8_t *)&UserReg.Dat.ctr.m_U_Char, sizeof(REG_Data));
    }
}

static void config_data_init(void)
{

#ifdef  DEBUG_CONFIG_ENV
    uint16_t i;

    uint8_t RegDatBuf[REG_BUF_LEN];

    romflash_clean_pages(ENV_START_ADDR);
    printf("config_data_init\r\n");
    romflash_pages_read(FATY_ADDR, RegDatBuf, sizeof(CFG_Filestr));

    for(i = 0; i < sizeof(CFG_Filestr); i++) {
        printf("%x ", RegDatBuf[i]);
    }

#endif
    current_tx_seq = 0;

    romflash_Factory_reg_get();
    romflash_user_reg_get();


#ifdef DEBUG_CONFIG_ENV
    romflash_pages_read(FATY_ADDR + CFG_INFO_LEN, RegDatBuf, sizeof(RegDatBuf));

    for(i = 0; i < sizeof(FactoryReg); i++) {
        printf("%x ", RegDatBuf[i]);
    }

    printf(" romflash FATORY end %d  %d\r\n", FactoryReg.Dat.txPort, FactoryReg.Dat.rxPort);
    romflash_pages_read(USER_ADDR + CFG_INFO_LEN, RegDatBuf, sizeof(RegDatBuf));

    for(i = 0; i < sizeof(UserReg); i++) {
        printf("%x ", RegDatBuf[i]);
    }

    printf(" romflash dat end %d  %d\r\n", UserReg.Dat.txPort, UserReg.Dat.rxPort);
#endif
    romflash_pages_read(FPGA_ADDR, (uint8_t *)FPGADatBuf, sizeof(FPGADatBuf));

    memcpy(&netif_par.mac[0], &UserReg.Dat.mac[0], 6);
    memcpy(&netif_par.ip[0], &UserReg.Dat.srcIp[0], 4);
    memcpy(&netif_par.gateway[0], &UserReg.Dat.gateway[0], 4);
    memcpy(&netif_par.netmask[0],  &UserReg.Dat.netmask[0], 4);
    netif_par.port = UserReg.Dat.rxPort;
    memcpy(&netif_remote_par.ip[0], &UserReg.Dat.remoteIp[0], 4);
    netif_remote_par.port = UserReg.Dat.txPort;

}



extern void romflash_FPGA_reg_save(void)
{
    uint16_t  ck;
    CFG_Filestr  fenvfile;
    CFG_Filestr  uenvfile;
    REG_Info tmpFactoryReg;
    REG_Info tmpUserReg;
    //uint8_t  tmpFpgaReg[REG_BUF_LEN];

    if(romflash_word_read(FATY_ADDR) == SoftKey1_VAL) {

        romflash_pages_read(FATY_ADDR, (uint8_t *)&fenvfile.Key, sizeof(CFG_Filestr));

        //fenvfile.Key = SoftKey1_VAL;

        romflash_pages_read(FATY_ADDR + CFG_INFO_LEN, (uint8_t *)&tmpFactoryReg.Dat.ctr.m_U_Char, sizeof(tmpFactoryReg));

        ck = crc_calculate((const uint8_t *)&tmpFactoryReg.Dat.ctr.m_U_Char, sizeof(tmpFactoryReg));

        if(fenvfile.ckSum != ck  && fenvfile.Key != SoftKey1_VAL) {
            printf("romflash FATORY ck error\r\n");
        }

        romflash_pages_read(USER_ADDR, (uint8_t *)&uenvfile.Key, sizeof(CFG_Filestr));

        romflash_pages_read(USER_ADDR + CFG_INFO_LEN, (uint8_t *)&tmpUserReg.Dat.ctr.m_U_Char, sizeof(tmpUserReg));

        ck = crc_calculate((const uint8_t *)&tmpUserReg.Dat.ctr.m_U_Char, sizeof(tmpUserReg));

        if(uenvfile.ckSum != ck  && uenvfile.Key != SoftKey2_VAL) {
            printf("romflash USER ck error\r\n");
        }

        romflash_clean_pages(ENV_START_ADDR);

        romflash_pages_write(FATY_ADDR, (uint8_t *)&fenvfile.Key, sizeof(fenvfile));
        romflash_pages_write(FATY_ADDR + CFG_INFO_LEN, (uint8_t *)&tmpFactoryReg.Dat.ctr.m_U_Char, sizeof(tmpFactoryReg));

        romflash_pages_write(USER_ADDR, (uint8_t *)&uenvfile.Key, sizeof(uenvfile));
        romflash_pages_write(USER_ADDR + CFG_INFO_LEN, (uint8_t *)&tmpUserReg.Dat.ctr.m_U_Char, sizeof(tmpUserReg));

        romflash_pages_write(FPGA_ADDR, (uint8_t *)FPGADatBuf, sizeof(FPGADatBuf));
    }
}


extern void romflash_user_reg_save(void)
{
    //uint16_t  ck;
    CFG_Filestr  fenvfile;
    CFG_Filestr  uenvfile;
    REG_Info tmpFactoryReg;
    REG_Info tmpUserReg;
    uint8_t  tmpFpgaReg[FPGA_REG_BUF_LEN];

    if(romflash_word_read(FATY_ADDR) == SoftKey1_VAL) {

        fenvfile.Key = SoftKey1_VAL;

        romflash_pages_read(FATY_ADDR + CFG_INFO_LEN, (uint8_t *)&tmpFactoryReg.Dat.ctr.m_U_Char, sizeof(tmpFactoryReg));

        fenvfile.ckSum = crc_calculate((const uint8_t *)&tmpFactoryReg.Dat.ctr.m_U_Char, sizeof(tmpFactoryReg));

        //if(fenvfile.ckSum != ck) {
        //    printf("romflash FATORY ck error\r\n");
        //}

        uenvfile.Key = SoftKey2_VAL;
        memcpy((char *)&tmpUserReg.Dat.ctr.m_U_Char, (uint8_t *)&UserReg.Dat.ctr.m_U_Char, sizeof(UserReg));
        uenvfile.ckSum = crc_calculate((const uint8_t *)&tmpUserReg.Dat.ctr.m_U_Char, sizeof(tmpUserReg));

        romflash_pages_read(FPGA_ADDR, (uint8_t *)tmpFpgaReg, sizeof(tmpFpgaReg));


        romflash_clean_pages(ENV_START_ADDR);

        romflash_pages_write(FATY_ADDR, (uint8_t *)&fenvfile.Key, sizeof(fenvfile));
        romflash_pages_write(FATY_ADDR + CFG_INFO_LEN, (uint8_t *)&tmpFactoryReg.Dat.ctr.m_U_Char, sizeof(tmpFactoryReg));

        romflash_pages_write(USER_ADDR, (uint8_t *)&uenvfile.Key, sizeof(uenvfile));
        romflash_pages_write(USER_ADDR + CFG_INFO_LEN, (uint8_t *)&tmpUserReg.Dat.ctr.m_U_Char, sizeof(tmpUserReg));

        romflash_pages_write(FPGA_ADDR, (uint8_t *)tmpFpgaReg, sizeof(tmpFpgaReg));
    }
}




Boolean config_user_reg_set(uint16_t addr, uint8_t *buf, uint16_t len)
{
    uint8_t *tmp;
    tmp = &UserReg.Dat.ctr.m_U_Char;

    if((addr + len) <= REG_BUF_LEN) {
        memcpy((uint8_t *)&tmp[addr], buf, len);
        return (TRUE);
    }

    return FALSE;
}

void Pack_mavlink_and_Send(uint8_t *src, uint8_t len)
{
    uint16_t checksum, sumLen;
    uint8_t *buf;

    buf = src;

    buf[MAVLINK_SEQ_POS] = current_tx_seq++;
    checksum = crc_calculate((const uint8_t*)&src[MAVLINK_LEN_POS], len + MAVLINK_CORE_HEADER_LEN);
    buf[len + 1] = (uint8_t)(checksum & 0xFF);
    buf[len + 2] = (uint8_t)(checksum >> 8);
    sumLen = MAVLINK_NUM_HEADER_BYTES + MAVLINK_REG_ADDR_BYTES + len + MAVLINK_NUM_CHECKSUM_BYTES;

    if(buf[MAVLINK_COMP_ID_POS] == MAVLINK_TYPE_UART) {
        USART3_Putbuf(buf, sumLen);
    } else if(buf[MAVLINK_COMP_ID_POS] == MAVLINK_TYPE_ETH) {
        //udp_send_to_host(buf, sumLen);
    }

}

Boolean config_user_reg_get(uint16_t addr, const uint8_t *buf, uint16_t len)
{
    uint8_t ureg[MAVLINK_MAX_PACKET_LEN];
    uint8_t *tmp;
    uint16_t length;

    tmp = &UserReg.Dat.ctr.m_U_Char;

    //length = len + MAVLINK_NUM_HEADER_BYTES + MAVLINK_NUM_CHECKSUM_BYTES;

    length = MAVLINK_NUM_HEADER_BYTES + MAVLINK_REG_ADDR_BYTES;

    if(len <= (MAVLINK_MAX_PAYLOAD_LEN - MAVLINK_REG_ADDR_BYTES)) {

        memcpy(ureg, buf, length);
        memcpy(&ureg[length], (uint8_t *)&tmp[addr], len);
        Pack_mavlink_and_Send(ureg, len);
        return (TRUE);
    }

    return FALSE;
}


Boolean config_fpga_reg_set(uint16_t addr, uint8_t *buf, uint16_t len)
{
    uint8_t *tmp;
    tmp = FPGADatBuf;

    if((addr + len) < FPGA_REG_BUF_LEN) {
        memcpy((uint8_t *)&tmp[addr], buf, len);
        return (TRUE);
    }

    return FALSE;
}


Boolean config_fpga_reg_get(uint16_t addr, const uint8_t *buf, uint16_t len)
{
    uint8_t freg[MAVLINK_MAX_PACKET_LEN];
    uint8_t *tmp;
    uint16_t length;

    tmp = FPGADatBuf;

    length = MAVLINK_NUM_HEADER_BYTES + MAVLINK_REG_ADDR_BYTES;

    if(len <= (MAVLINK_MAX_PAYLOAD_LEN - MAVLINK_REG_ADDR_BYTES)) {

        memcpy(freg, buf, length);
        memcpy(&freg[length], (uint8_t *)&tmp[addr], len);
        Pack_mavlink_and_Send(freg, len);
        return (TRUE);
    }

    return FALSE;
}




extern uint8_t DownloadConfig(STR_PECRPORTOCOL *lan)
{
    uint32_t addr;
    uint32_t operatoradr;  //要操作的地址
    addr = lan->addr;   //

    if(romflash_clean_pages(addr) == TRUE) {
        operatoradr = (addr + FATY_ADDR);

        if(romflash_pages_write(operatoradr, lan->buf, lan->length) == TRUE) {
            return NET_ACK;
        }
    }

    return NET_ERR;

}



extern uint8_t mavlink_parse_config(uint8_t *src, uint16_t length)
{
    Boolean rst = FALSE;
    uint8_t *regBuf;
    uint16_t checksum, len;
    uint8_t *buf;

    uint16_t addr;
    uint8_t regType;
    uint8_t i;
    buf = src;

    if((buf[MAVLINK_STX_POS] == MAVLINK_STX)
            && (buf[MAVLINK_SYS_ID_POS] == MAVLINK_SYSID)
            && (length >= MAVLINK_MIN_PACKET_LEN)) {

        len = buf[MAVLINK_LEN_POS];

        regType = buf[MAVLINK_MSG_ID_POS];//0x00 – do nothing
        //0x01 – write to stm32 registers
        //0x02 – read from stm32 registers
        //0x03 – write to FPGA registers
        //0x04 – read from FPGA registers

        if(length != (len + MAVLINK_NUM_HEADER_BYTES + MAVLINK_NUM_CHECKSUM_BYTES)) {
            return (PAR_FALSE);
        }

        checksum = crc_calculate((const uint8_t*)&buf[MAVLINK_LEN_POS], MAVLINK_CORE_HEADER_LEN);

        crc_accumulate_buffer(&checksum, (const char*)&buf[MAVLINK_REG_ADDR_H_POS], len);
        USART3_Putchar((uint8_t)(checksum & 0xFF));
        USART3_Putchar((uint8_t)(checksum >> 8));

#if MAVLINK_CRC_EXTRA
        crc_accumulate(MAVLINK_CONFIG_EXTRA_CRC, &checksum);//EXTRA
#endif
        for(i = 0; i < length; i++)
        {
            USART3_Putchar(buf[i]);
        }
        USART3_Putchar((uint8_t)(checksum & 0xFF));
        USART3_Putchar((uint8_t)(checksum >> 8));
        if(buf[MAVLINK_NUM_HEADER_BYTES + len] == (uint8_t)(checksum & 0xFF)
                && buf[MAVLINK_NUM_HEADER_BYTES + len + 1] == (uint8_t)(checksum >> 8)) {

            addr = buf[MAVLINK_REG_ADDR_H_POS] * 0x100 + buf[MAVLINK_REG_ADDR_L_POS];
            regBuf = &buf[MAVLINK_NUM_HEADER_BYTES + MAVLINK_REG_ADDR_BYTES];
            len -= MAVLINK_REG_ADDR_BYTES;

            //typePort = buf[MAVLINK_COMP_ID_POS];
            switch(regType) {
            case REG_IGNORE:
                return (PAR_IGNORE);

            case  REG_WSTM:
                rst = config_user_reg_set(addr, regBuf, len);
                USART3_Putchar(len);
                //Led_Test();
                break;

            case REG_RSTM:
                rst = config_user_reg_get(addr, buf, len);
                break;

            case REG_WFPGA:
                rst = config_fpga_reg_set(addr, regBuf, len);
                break;

            case REG_RFPGA:
                rst = config_fpga_reg_get(addr, buf, len);
                break;

            default:
                return (PAR_FALSE);

            }

            if(rst == TRUE)
                return (PAR_OK);
        }
    }

    return (PAR_FALSE);
}




extern void AddParseCorruptCnt(uint8_t typePort)
{
    if(typePort == MAVLINK_TYPE_UART) {
        UserReg.Dat.sRxCorrupt++;
    } else if(typePort == MAVLINK_TYPE_ETH) {
        UserReg.Dat.ethRxCorrupt++;
    } else {
        //do nothing
    }
}


extern void AddParseRightCnt(uint8_t typePort)
{
    if(typePort == MAVLINK_TYPE_UART) {
        UserReg.Dat.sRxSum++;
    } else if(typePort == MAVLINK_TYPE_ETH) {
        UserReg.Dat.ethRxSum++;
    } else {
        //do nothing
    }
}


extern void config_CheckStatus(void)
{

    uint8_t wRegFlg = 0;
    uint8_t rRegFlg = 0;
    uint8_t refresh = 0;

    if(UserReg.Dat.ctr.mBit.bWR) {
        UserReg.Dat.ctr.mBit.bWR = 0;
        wRegFlg = 1;

    }

    if(UserReg.Dat.ctr.mBit.bRD) {
        UserReg.Dat.ctr.mBit.bRD = 0;
        rRegFlg = 1;
    }

    if(UserReg.Dat.ctr.mBit.bFS) {
        UserReg.Dat.ctr.mBit.bFS = 0;
        refresh = 1;
    }

    if(wRegFlg)
        PushEvt_OnlyEvt(evURegSave);

    if(rRegFlg)
        PushEvt_OnlyEvt(evGUserReg);

    if(refresh)
        PushEvt_OnlyEvt(evGFctyReg);
}

#if 1

static void load_Factory_ipPar(void)
{
    netif_par.ip[0] = FACTORY_IP_0;
    netif_par.ip[1] = FACTORY_IP_1;
    netif_par.ip[2] = FACTORY_IP_2;
    netif_par.ip[3] = FACTORY_IP_3;

    netif_par.netmask[0] = FACTORY_NETMASK_0;
    netif_par.netmask[1] = FACTORY_NETMASK_1;
    netif_par.netmask[2] = FACTORY_NETMASK_2;
    netif_par.netmask[3] = FACTORY_NETMASK_3;

    netif_par.gateway[0] = FACTORY_GATEWAY_0;
    netif_par.gateway[1] = FACTORY_GATEWAY_1;
    netif_par.gateway[2] = FACTORY_GATEWAY_2;
    netif_par.gateway[3] = FACTORY_GATEWAY_3;

    netif_par.mac[0] = FACTORY_MAC_B0;
    netif_par.mac[1] = FACTORY_MAC_B1;
    netif_par.mac[2] = FACTORY_MAC_B2;
    netif_par.mac[3] = FACTORY_MAC_B3;
    netif_par.mac[4] = FACTORY_MAC_B4;
    netif_par.mac[5] = FACTORY_MAC_B5;

    netif_par.port = FACTORY_UDP_IN_PORT;
	
}


static void load_Factory_remote_ipPar(void)
{
    netif_remote_par.ip[0] = FACTORY_REMOTE_IP_0;
    netif_remote_par.ip[1] = FACTORY_REMOTE_IP_1;
    netif_remote_par.ip[2] = FACTORY_REMOTE_IP_2;
    netif_remote_par.ip[3] = FACTORY_REMOTE_IP_3;
    netif_remote_par.port = FACTORY_UDP_OUT_PORT;
}

#endif

extern void config_env_init(void)
{
#if 0
    config_data_init();
#else
    load_Factory_ipPar();
    load_Factory_remote_ipPar();
#endif
	ethernetif_mac_conf(netif_par.mac);
	//Lwip_Par_conf(&flash_ip_par);
    //remoteLwip_Par_conf(&flash_remoteip_par);
}

