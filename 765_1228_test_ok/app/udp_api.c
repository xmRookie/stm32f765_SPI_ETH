
#include "common.h"
#include "ethernetif.h"
//extern struct netif gnetif;
/* Global Ethernet handle */
extern ETH_HandleTypeDef heth;


#define P_ARP       0x0806
#define P_RARP      0x8035
#define P_IP		0x0800




#define UDP_HLEN 8
#define LINK_HLEN 14
/* Size of the IPv4 header. Same as 'sizeof(struct ip_hdr)'. */
#define IP_HLEN 20



extern NETIF_PAR netif_par;
extern NETIF_REMOTE_PAR netif_remote_par;



static uint8_t link_state = 0;
/** The IP header ID of the next outgoing IP packet */
static u16_t ip_id;




typedef struct {
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
    uint8_t type[2];
} EtherHdr;

typedef struct
{
    uint8_t                  hardware[2];       // Hardware
    uint8_t                  protocol[2];       // Protocol
    uint8_t                  HL_PL[2];          // Hardware Address Length(8 bit) : Protocol Address Length(8 bit)
    uint8_t                  op[2];             // Operation
    uint8_t                  sedmac[6];      // Sender Hardware Address
    uint8_t                  sedip[4];          // Sender Internet Address
    uint8_t                  trgmac[6];      // Target Hardware Address
    uint8_t                  trgip[4];          // Target Internet Address
} ARPPACKA_TYPE;


typedef struct {
    uint8_t                  socpot[2];                     // 璇锋眰绔鍙?
    uint8_t                  dstpot[2];                     // 鏈嶅姟鍣ㄧ鍙?
    uint8_t                  len[2];                     // UDP鏁版嵁闀垮害
    uint8_t                  chksum[2];                     // UDP鍖呮牎楠屾牳

} UDPPACKA_TYPE;


typedef struct {
    uint8_t                  V_H_T[2];          // Version(4 bit) : Header Length(4 bit) : Type of Service(8 bit)
    uint8_t                  totlen[2];         // Total Length
    uint8_t                  idtf[2];           // Identifier
    uint8_t                  FLA_OFS[2];        // Eragmentation Flags(3 bit) : Eragment Offset(13 bit)
    uint8_t                  LIVE_PTC[2];       // Time To Live(8 bit) : Protocol(8 bit)
    uint8_t                  chksum[2];       // Header Checksum
    uint8_t                  socip[4];          // Sourct IP Address
    uint8_t                  dstip[4];          // Dest. IP Address
} IPPACKA_TYPE;



typedef struct {
    EtherHdr               eth;           // 14
    IPPACKA_TYPE           ipH;            // 20
    UDPPACKA_TYPE          udpH;            // 8
} NETPACKA_TYPE;


typedef struct {
    EtherHdr               eth;           // 14
    ARPPACKA_TYPE           arp;            // 28

} NETPACKA_ARP_TYPE;

NETPACKA_TYPE   netUdpPack;
//NETPACKA_ARP_TYPE netArpPack;



static void U16_To_HexBuf(uint8_t *dest, uint16_t bin)
{
    *dest++ = bin >> 8;
    *dest = bin;
}


static uint16_t HexBuf_To_U16(uint8_t *src)
{
    return ((src[0] * 0x100) + src[1]);
}


u16_t get_ip_id(void)
{
	return ip_id;
}

void Inc_ip_id(void)
{
	++ip_id;
}




//鍔熻兘  ARP鏁版嵁鍖呯殑鍙戦?佸嚱鏁? //ARP鐨勬暟鎹暱搴︽槸42涓瓧鑺?
//杈撳叆  op  ARP鐨勬搷浣滅爜 
//杈撳嚭
//杩斿洖
static void sendArp(uint8_t op)
{
    NETPACKA_ARP_TYPE *txpt;
    uint8_t TxBuf[0x100];

    txpt = (NETPACKA_ARP_TYPE *)TxBuf;
    U16_To_HexBuf(txpt->arp.hardware, 0x0001);
    U16_To_HexBuf(txpt->arp.protocol, 0x0800);
    txpt->arp.HL_PL[0] = 0x06;
    txpt->arp.HL_PL[1] = 0x04;
    U16_To_HexBuf(txpt->arp.op, op);
    memcpy(txpt->arp.sedmac, netif_par.mac, 6);
    memcpy(txpt->arp.sedip, netif_par.ip, 4);
    if (op == 1)
    {   // ARP 璇锋眰
        memset(txpt->arp.trgmac, 0x00, 6);
		memset( netif_remote_par.mac,0xff, 6);
    }
    else
    {   // ARP 搴旂瓟
        memcpy(txpt->arp.trgmac, netif_remote_par.mac, 6);
    }
    memcpy(txpt->arp.trgip, netif_remote_par.ip, 4);

	memcpy(txpt->eth.dest_mac, netif_remote_par.mac, 6);
    memcpy(txpt->eth.src_mac, netif_par.mac, 6);
    U16_To_HexBuf(txpt->eth.type, P_ARP);
	udp_ts_output(TxBuf,42);
}


//鍔熻兘  ARP鍔熻兘鍑芥暟锛屼富瑕佺敤浜庢煡璇㈠拰鍥炵瓟鐩稿簲鐨凪AC鍦板潃
//      纭欢绫诲瀷(16)            鍗忚绫诲瀷(16)
//      纭欢闀垮害(8) 鍗忚闀垮害(8) 鎿嶄綔(16)
//              鍙戦?佺珯纭欢鍦板潃
//              鍙戦?佺珯鍗忚鍦板潃
//              鐩爣绔欑‖浠跺湴鍧?
//              鐩爣绔欏崗璁湴鍧?
//杈撳叆
//杈撳嚭
//杩斿洖
static void arpFun(uint8_t *netBuffer)
{
    NETPACKA_ARP_TYPE *rxpt;

    rxpt = (NETPACKA_ARP_TYPE *)netBuffer;
    if (rxpt->arp.op[0] != 0)
    {   //ARP鐨勯珮浣嶆搷浣滅爜闈炴硶
        return;
    }
    //memcpy(remoteIP->ip, rxpt->arp.sedip, 4);                    //璁板綍杩滅鐨処P
    switch (rxpt->arp.op[1])
    {
        case 1:     //ARP璇锋眰鎿嶄綔,杩欓噷鍙戦?佽姹傚簲绛?
           sendArp(2);
            break;
        case 2:     //ARP搴旂瓟鎿嶄綔
        	if (memcmp(netif_par.ip, rxpt->arp.trgip, 4) == 0)//杩滅鐨処P涓庢湰鏈虹殑IP鐩稿悓,IP鍦板潃鍐茬獊
			{
				memcpy(netif_remote_par.mac,rxpt->arp.sedmac, 6);
				memcpy(netUdpPack.eth.dest_mac, netif_remote_par.mac, 6);
				//USART3_Putbuf(netif_remote_par.mac, 6);
				link_state = NET_LINKED;
			}
            break;
        default:
            return;
    }
    
}


static void udpFun(uint8_t *netBuffer,uint16_t len)
{
    NETPACKA_TYPE *rxpt;

    rxpt = (NETPACKA_TYPE *)netBuffer;

    printf("udp len %d\r\n",HexBuf_To_U16(rxpt->udpH.len));

	
}

extern void nicIP(uint8_t *netBuffer,uint16_t len)
{

    EtherHdr *rxpt;

    rxpt = (EtherHdr *)netBuffer;
	
	 if (memcmp(netif_par.mac, rxpt->dest_mac, 6) != 0)//鍒ゆ柇鏄槸鍚﹀彂缁欐垜浠殑
	 {
		 return;
	 }

    switch(HexBuf_To_U16(rxpt->type))          
    {
        case P_ARP:                                 
            arpFun(netBuffer);
            break;
        case P_RARP:                               
            break;
        case P_IP:                                  
            udpFun(netBuffer,len);//376 or 188 or 42
            break;
        default:
            break;
    }
}





extern uint8_t get_remoteLwip_udp_link(void)
{
    return link_state;
}


extern void Set_remoteLwip_udp_link(uint8_t link)
{
	if(link)
		link_state = NET_LINKED;
	else
		link_state = NET_UNLINK;
}



extern void udp_TimerTick(void)
{
	if(link_state == NET_UNLINK)
	{
		sendArp(1);
	}

}



extern void udp_Send_Pack(uint8_t *buf1, uint16_t len1,uint8_t *buf2, uint16_t len2)
{
	uint16_t id;
	uint32_t ret;
	volatile uint16_t i;
	PBUF mPortBuf;
	
	i = len1+len2;
	mPortBuf.len = i;

	Inc_ip_id();
	id = get_ip_id();
	U16_To_HexBuf(netUdpPack.ipH.idtf, id);

	i += UDP_HLEN;
	U16_To_HexBuf(netUdpPack.udpH.len, i);
	i += IP_HLEN;
	U16_To_HexBuf(netUdpPack.ipH.totlen, i);


	mPortBuf.tot_len = i + LINK_HLEN;

	mPortBuf.payload = (void *)&netUdpPack;
	ret = len1+len2;
	if(udp_ts_output2( &mPortBuf, buf1,len1,buf2,len2) != ret){
		USART3_Putchar(6);
	}

}



//UDP
extern void udp_echoclient_connect(void)
{
    uint16_t id;
    uint16_t len;

	link_state = NET_UNLINK;

	/** The IP header ID of the next outgoing IP packet */
	ip_id = 0;

    len = 0;
    len += UDP_HLEN;

    U16_To_HexBuf(netUdpPack.udpH.socpot, netif_par.port);
    U16_To_HexBuf(netUdpPack.udpH.dstpot, netif_remote_par.port);

    U16_To_HexBuf(netUdpPack.udpH.len, len);
    netUdpPack.udpH.chksum[1] =  netUdpPack.udpH.chksum[0] = 0;


    len += IP_HLEN;
    netUdpPack.ipH.V_H_T[0] = 0x45;
    netUdpPack.ipH.V_H_T[1] = 0x00;
    U16_To_HexBuf(netUdpPack.ipH.totlen, len);
    //Inc_ip_id();
    id = get_ip_id();
    U16_To_HexBuf(netUdpPack.ipH.idtf, id);
    U16_To_HexBuf(netUdpPack.ipH.FLA_OFS, 0x0000);
    netUdpPack.ipH.LIVE_PTC[0] = 0xFF;//ttl
    netUdpPack.ipH.LIVE_PTC[1] = 0x11;//proto
    memcpy(netUdpPack.ipH.socip, netif_par.ip, 4);
    memcpy(netUdpPack.ipH.dstip, netif_remote_par.ip, 4);
    netUdpPack.ipH.chksum[1] =  netUdpPack.ipH.chksum[0] = 0;

    memcpy(netUdpPack.eth.dest_mac, netif_remote_par.mac, 6);
    memcpy(netUdpPack.eth.src_mac, netif_par.mac, 6);
    U16_To_HexBuf(netUdpPack.eth.type, 0x0800);

}

