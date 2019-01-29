#include "common.h"

#define NULL_KEY     0xffff
#define NOR_KEY      0x8000
#define RESET_KEY    0x4000

#define KEY_RESET_GPIO                     GPIOC


#define KeyFilterLen        3
static u8_t SCodeBuf[KeyFilterLen];
static u8_t PreKeyCode;            // KeyCode1�洢�ϴ�ɨ����,KeyCode2�洢�ϴ��ύ�İ���ֵ
static u8_t PressedKey;          // �洢�ϴ��ύ�İ���ֵ
static u16_t KeyPressCount;

u8_t KeyScanCode;



static void key_Par_Init(void)
{
    memset(SCodeBuf, 0xff, sizeof(SCodeBuf));
    PreKeyCode = 0xff;
    PressedKey = 0xff;
    KeyPressCount = 0;
    KeyScanCode = 0;
}


//������ʼ������
extern void MX_KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_KEY_InitStruct;
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /*Configure GPIO pin : PC13 */
    GPIO_KEY_InitStruct.Pin = GPIO_PIN_13;
    GPIO_KEY_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_KEY_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(KEY_RESET_GPIO, &GPIO_KEY_InitStruct);
    key_Par_Init();
}



//����  ��ȡ��ǰ����ɨ��ֵ
//����  ��
//���  ��
//����  ����ɨ����,�����ް���(0xff)
static u8_t GetScanCode(void)
{

    u8_t kvalue;

    if(HAL_GPIO_ReadPin(KEY_RESET_GPIO, GPIO_PIN_13) == GPIO_PIN_SET) {
        kvalue = 0x04;
    } else {
        kvalue = 0xff;
    }

    return (kvalue);
}


static u16_t GetKey(void)
{
    u8_t ch;

    for(ch = KeyFilterLen - 1 ; ch ; ch--) {
        SCodeBuf[ch] = SCodeBuf[ch - 1];
    }

    SCodeBuf[0] = GetScanCode();                         // ��ȡ������ǰɨ����

    for(ch = 1 ; ch < KeyFilterLen ; ch++) {
        if(SCodeBuf[ch] != SCodeBuf[0]) {
            return NULL_KEY;
        }
    }

    ch = SCodeBuf[0];

//------------------ ����Ĵ���Ϊ�������˲�����
    if(ch == 0xff) {
        // ��ǰû�а�������
        if(PressedKey == 0xff) {
            // ���𰴼��Ѿ��ύ
            PreKeyCode = 0xff;
            KeyPressCount = 0;
            return(NULL_KEY);
        }

        if(PreKeyCode != 0xff) {
            // ���𰴼���û���ύ,�ҵ�һ����⵽��������
            PreKeyCode = 0xff;
            return(NULL_KEY);
        }

        ch = PressedKey;                             // ���ζ�����ɨ��һ���ŷ���
        KeyScanCode = 0xff;
        KeyPressCount = 0;
        PressedKey = 0xff;
        return(NOR_KEY + ch);
    }

    if(PressedKey == 0xff) {
        // ������û���ύ
        if(ch == PreKeyCode) {                       // ���ζ�����ɨ��һ���ŷ���
            // ˵���ǰ�������
            PressedKey = ch;
            KeyPressCount = 1;
            KeyScanCode = ch;
            return(ch);
        } else {
            PreKeyCode = ch;
            return(NULL_KEY);
        }
    }

    if(ch == PreKeyCode) {                            // ���ζ�����ɨ��һ���ŷ���
        if(PressedKey == ch) {
            // ������������¼�
            KeyPressCount++;

            if(KeyPressCount == 300) {              // �ύ3���¼�
                return(RESET_KEY + ch);
            }

            return(NULL_KEY);
        } else {
            // �����µİ���,����һ�����ĵ��𰴼���û���ύ,���ύ��һ�����ĵ��𰴼�
            ch = PressedKey;
            KeyScanCode = 0xff;
            KeyPressCount = 0;
            PreKeyCode = 0xff;
            PressedKey = 0xff;
            return(NOR_KEY + ch);
        }
    } else {
        PreKeyCode = ch;
        return (NULL_KEY);
    }
}

// ����    ����ɨ�����
extern void key_Scan(void)
{
    u16_t tmp;
	  //STR_EVENT dcode;
    tmp = GetKey();

    if(tmp == NULL_KEY) {
        //δ���յ���������
        //printf("null key\n");
        return;
    }

    if(tmp & NOR_KEY) {
        // ���𰴼�
        #ifdef SEFT_TEST
         En_flow_ctrl();
		#endif
         if(getenPollint())
         {
			setenPollint(0);
		 }
		 else
		 {
			setenPollint(1);
		 }
        //printf("NOR_KEY %d \n", tmp);
        return;
    } else if(tmp & RESET_KEY) {
        // 3�밴��
			  PushEvt_OnlyEvt(evRst);
        return;
    } else {

    }
}

