
#ifndef __CONFIG_ENV_H__
#define __CONFIG_ENV_H__

#define REG_BUF_LEN  0x200
#define FPGA_REG_BUF_LEN  0x100


#pragma pack (1) /*指定???1字节对齐*/

typedef struct {
    uint32_t     Key;                // 特殊标志
    uint32_t     ckSum;             //  crc 检验

} CFG_Filestr;

typedef union reg_CTRL {

    uint8_t  m_U_Char;	/* Unsigned char Access */

    struct {				/* Bit Access */
        uint8_t				: 5;	/*Reserved */
        uint8_t  bFS		: 1;	/*load default factory settings into all registers */
        uint8_t  bRD	: 1;	/*read flash values and load into reg */
        uint8_t  bWR		: 1;	/*write into flash */
    } mBit;

} T_REG_CTRL;

typedef struct  {
	T_REG_CTRL ctr;
	uint8_t sRxSum;  /*counts total number of packets received (for serial port)*/
	uint8_t sRxCorrupt;
	uint8_t ethRxSum;
	uint8_t ethRxCorrupt;
	uint8_t NoUsed_01;
	uint8_t mac[6];
	uint8_t srcIp[4];
	uint8_t gateway[4];
	uint8_t netmask[4];
	uint16_t txPort;
	uint16_t rxPort;
	uint8_t remoteIp[4];
}REG_Data;


typedef struct{
    REG_Data     Dat;
    uint8_t      nouse[REG_BUF_LEN-sizeof(REG_Data)];
} REG_Info;
#pragma pack () /*取消指定对齐，恢复缺省对齐*/


#define FACTORY_IP_0  192
#define FACTORY_IP_1  168
#define FACTORY_IP_2  0
#define FACTORY_IP_3  188

#define FACTORY_NETMASK_0  255
#define FACTORY_NETMASK_1  255
#define FACTORY_NETMASK_2  255
#define FACTORY_NETMASK_3  0

#define FACTORY_GATEWAY_0  192
#define FACTORY_GATEWAY_1  168
#define FACTORY_GATEWAY_2  0
#define FACTORY_GATEWAY_3  1

#define FACTORY_MAC_B0  0x00
#define FACTORY_MAC_B1  0x80
#define FACTORY_MAC_B2  0xE1
#define FACTORY_MAC_B3  0x00
#define FACTORY_MAC_B4  0x00
#define FACTORY_MAC_B5  0x00

#define FACTORY_UDP_IN_PORT  0x5004

#define FACTORY_REMOTE_IP_0  192
#define FACTORY_REMOTE_IP_1  168
#define FACTORY_REMOTE_IP_2  0
#define FACTORY_REMOTE_IP_3  190

#define FACTORY_UDP_OUT_PORT  0x5003



#ifndef MAVLINK_STX
#define MAVLINK_STX 254
#endif


#ifndef MAVLINK_CRC_EXTRA
#define MAVLINK_CRC_EXTRA 0
#endif


#ifndef MAVLINK_MAX_PAYLOAD_LEN
// it is possible to override this, but be careful!
#define MAVLINK_MAX_PAYLOAD_LEN 255 ///< Maximum payload length
#endif

#define MAVLINK_CORE_HEADER_LEN 5 ///< Length of core header (of the comm. layer): message length (1 byte) + message sequence (1 byte) + message system id (1 byte) + message component id (1 byte) + message type id (1 byte)
#define MAVLINK_REG_ADDR_BYTES   2
#define MAVLINK_NUM_HEADER_BYTES (MAVLINK_CORE_HEADER_LEN + 1) ///< Length of all header bytes, including core and checksum
#define MAVLINK_NUM_CHECKSUM_BYTES 2

#define MAVLINK_NUM_NON_PAYLOAD_BYTES (MAVLINK_NUM_HEADER_BYTES + MAVLINK_NUM_CHECKSUM_BYTES)

#define MAVLINK_MAX_PACKET_LEN (MAVLINK_MAX_PAYLOAD_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES) ///< Maximum packet length
#define MAVLINK_MIN_PACKET_LEN (MAVLINK_NUM_HEADER_BYTES + MAVLINK_NUM_CHECKSUM_BYTES)

#define MAVLINK_STX_POS  0
#define MAVLINK_LEN_POS  1
#define MAVLINK_SEQ_POS   2
#define MAVLINK_SYS_ID_POS  3
#define MAVLINK_COMP_ID_POS  4
#define MAVLINK_MSG_ID_POS  5
#define MAVLINK_REG_ADDR_H_POS  6
#define MAVLINK_REG_ADDR_L_POS  7



#define MAVLINK_SYSID   0x88
#define MAVLINK_TYPE_UART 0x23
#define MAVLINK_TYPE_ETH  0x24
#define MAVLINK_CONFIG_EXTRA_CRC 0x20

#define REG_IGNORE    0x00
#define REG_WSTM    0x01
#define REG_RSTM   0x02
#define REG_WFPGA    0x03
#define REG_RFPGA    0x04

//	0x00 – do nothing
//	0x01 – write to stm32 registers
//	0x02 – read from stm32 registers
//	0x03 – write to FPGA registers
//	0x04 – read from FPGA registers



extern void config_env_init(void);

extern uint8_t mavlink_parse_config(uint8_t *src,uint16_t length);

extern void AddParseCorruptCnt(uint8_t typePort);

extern void AddParseRightCnt(uint8_t typePort);

extern uint8_t DownloadConfig(STR_PECRPORTOCOL *lan);


extern void config_CheckStatus(void);

extern void romflash_user_reg_save(void);

extern void romflash_FPGA_reg_save(void);

extern void romflash_user_reg_get(void);

extern void romflash_reg_refresh(void);

extern void romflash_Factory_reg_get(void);




#endif //__CONFIG_ENV_H__
