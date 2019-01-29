
#include "common.h"

typedef struct {
    uint32_t     Key;                // �����־
    uint32_t     FileWord;           // �ļ����� Word
    uint32_t     ckSum;             //  crc ����
    uint32_t     v_Hardver;          // Ӳ���汾��
    uint32_t     v_SWVer;            // ����汾��
    uint32_t     v_year;             // ��
    uint32_t     v_month;            // ��
    uint32_t     v_day;              // ��
} STR_Filestr;


#define F16KProgramAddr      0x08000000
#define F16KFileLen          0x4000   //16K
#define EnvLen               0x4000   //16K
#define VectorLen            0x400

#define ApplicationAddress  (F16KProgramAddr + F16KFileLen + EnvLen)

#define Soft_Key            0x55aa33cc

typedef void (*pFunction)(void);

typedef struct
{
    uint32_t     Flag;
    uint32_t     BaudRate;
    //uint8_t      ip[4];
    //uint8_t      mac[6];
} STR_F16kData;

typedef struct
{
    STR_F16kData     info;
    uint8_t          nouse[0x100-sizeof(STR_F16kData)];
} STR_F16kInfo;



static uint32_t UpDataFlag;      // �Ƿ�Ӵ��ڽ��յ�'a'
static uint32_t WaitATime;       // �ȴ�����'a'����ʱ
static STR_F16kInfo *f16kinfo;


const STR_Filestr variable1 __attribute__((at(F16KProgramAddr+VectorLen))) = {
    Soft_Key,
    0,
    0,
    HARD_VER,
    VER_VERSION,
    VER_YEAR,
    VER_MONTH,
    VER_DAY
};


static uint8_t WriteOnePackage(uint8_t *buf, uint32_t addr, uint16_t len)
{
    uint32_t operatoradr;  //Ҫ�����ĵ�ַ
 
    STR_Filestr *fileinf;

    if(addr == (VectorLen/0x100)) {
        fileinf = (STR_Filestr *)buf;

        if(fileinf->Key != Soft_Key) {
            // �Ƿ��ļ�
            return NET_ERR;
        }

        if(fileinf->v_Hardver != HARD_VER) {
            // Ӳ���汾��ƥ��
            return NET_ERR;
        }

    }

    if(romflash_clean_pages(addr) == TRUE) {
        operatoradr = (addr + ApplicationAddress);

        if(romflash_pages_write(operatoradr, buf, len) == TRUE) {
            return NET_ACK;
        }
    }

    return NET_ERR;
}



extern void SetUpdataFlag(void)
{
    f16kinfo = (STR_F16kInfo *)(0x20000000);

    if(f16kinfo->info.Flag != 0x12345678) {
        // ���Ǵ���������ת��
        UpDataFlag = 0;
        //f16kinfo->info.BaudRate = BaudRate_115200;
    } else {
        // �Ǵ���������ת��
        UpDataFlag = 1;
    }

    f16kinfo->info.Flag = 0;
    WaitATime = 0;
}


extern void SetSwitchFlag(void)
{
    f16kinfo->info.Flag = 0;
}


static void GoToUpdate(void)
{
    UpDataFlag = 1;
}

extern void ReceiveAGotoUpd(void)
{
    USART3_Putchar('a');
    GoToUpdate();
}


static uint8_t cfi_checkSw(uint32_t baseadr)
{
    STR_Filestr const *fileinfo;
    uint32_t address;
    uint32_t i;
    uint16_t ckSum;
    uint8_t ch;

    fileinfo = (STR_Filestr const *)(baseadr+VectorLen);

    if(fileinfo->Key != Soft_Key) {
        return ERROR;
    }

    address = baseadr;
    ch = *(__IO uint8_t *)address;
    ckSum = crc_calculate(&ch, 1);

    for(i = 1 ; i < fileinfo->FileWord ; i++) {
        ch = *(__IO uint8_t *)(address + i);
        crc_accumulate(ch, &ckSum);
    }

    if(ckSum != fileinfo->ckSum) {
        return ERROR;
    }

    return SUCCESS;
}

extern void TimeOutToChkSW(void)
{
    if(!UpDataFlag) {
        WaitATime++;

        if(WaitATime == 20) {
            if(cfi_checkSw(ApplicationAddress) == SUCCESS) {
                uint32_t JumpAddress;
                pFunction Jump_To_Application;

                __set_PRIMASK(1);                       // �ر������ж�
                JumpAddress = *(__IO uint32_t *)(ApplicationAddress+4);
                Jump_To_Application = (pFunction) JumpAddress;
                Jump_To_Application();                      //����Ӧ�ó���
            } else {
                GoToUpdate();
            }
        }
    }

}



extern uint8_t DownloadProgram(STR_PECRPORTOCOL *lan)
{

    uint32_t addr;
    uint8_t rst;

    addr = lan->addr;   //

    rst = WriteOnePackage(lan->buf, addr, lan->length);

    return(rst);

}
