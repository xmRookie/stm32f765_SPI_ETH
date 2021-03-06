//#include "libavutil/internal.h"

#include "avcodec.h"
#include "mpegaudio.h"

#include <stdio.h>               // stdio header
#include <string.h>              // string header
#include <stdlib.h>
#include <math.h>
//#include <cycle_count.h>         // for basic cycle counting



#define NUM_INPUT_PCM_CHANNELS	(2)
#define FIFO_BUFFER_SIZE        (MPA_FRAME_SIZE<<1)

#define NUM_OUT_BUFFERS        (4)


#define MP2BUF_NEXT_PT(dat, offset, size)     (((dat) + (offset)) & ((size) - 1))

extern void udp_Send_Pack(uint8_t *buf1, uint16_t len1,uint8_t *buf2, uint16_t len2);
void pollint_Audio_TS(void);

// codec error codes for the framework
enum _CODEC_ERRORS
{
    CODEC_SUCCESS,					// everything went okay
    CODEC_FILE_ERROR,				// error in file handling
    CODEC_ALGORITHM_ERROR,			// codec error
    CODEC_MEMORY_ALLOCATION_ERROR	// out of memory in heap
};

extern int MPA_encode_init(AVCodecContext *avctx);
extern int MPA_encode_frame(AVCodecContext *avctx,
                            unsigned char *frame, int buf_size, void *data);
extern int MPA_encode_close(AVCodecContext *avctx);

extern void PackAudioTs(uint8_t *buf,uint16_t len);

extern uint8_t GetTSmuxAudioState(void);

extern void  Start_Audio_Frame(void);

//short test_enc_input_buffer[MPA_FRAME_SIZE * NUM_INPUT_PCM_CHANNELS]; //1152*2

const short test_enc_input_buffer[2304]={
0xE101,0xE201,0x5902,0x5A02,0x4702,0x4702,0x5F01,0x5E01,
0x0300,0x0100,0xE2FE,0xE1FE,0x60FE,0x5FFE,0x16FE,0x15FE,
0x11FD,0x10FD,0x7FFA,0x7CFA,0x19F7,0x18F7,0xF1F4,0xEFF4,
0x23F6,0x24F6,0xE6FA,0xEAFA,0xC001,0xC601,0x3308,0x3708,
0x7A0B,0x7C0B,0x4D09,0x4A09,0x6702,0x6002,0x74F9,0x6CF9,
0xA7F2,0xA2F2,0x1EF1,0x1EF1,0x66F5,0x6AF5,0x68FD,0x6EFD,
0xCF05,0xD405,0x200B,0x230B,0x430B,0x420B,0xD505,0xD005,
0x20FD,0x18FD,0x30F5,0x2BF5,0xC7F0,0xC4F0,0x2DF1,0x30F1,
0xDFF5,0xE2F5,0x0FFD,0x15FD,0x1B05,0x2105,0x1F0C,0x230C,
0x2610,0x2810,0xC20F,0xC10F,0x920A,0x8C0A,0xF601,0xEF01,
0xAEF8,0xA7F8,0x8DF1,0x88F1,0xA2EE,0xA0EE,0x6EF1,0x70F1,
0x2CF9,0x32F9,0x1902,0x1F02,0x8907,0x8C07,0x9C06,0x9A06,
0x3701,0x3101,0xD6FB,0xD3FB,0xAAFA,0xABFA,0x45FE,0x48FE,
0x7903,0x7D03,0xBF06,0xC006,0x8805,0x8705,0x0100,0xFCFF,
0x5EF9,0x5AF9,0x60F5,0x60F5,0x98F6,0x9BF6,0x94FC,0x9BFC,
0xDB04,0xE204,0xC90B,0xCD0B,0x490E,0x480E,0xCA0B,0xC60B,
0x2306,0x1D06,0xD7FF,0xD2FF,0x01FB,0xFEFA,0x21F8,0x1FF8,
0x6AF6,0x68F6,0x0DF5,0x0CF5,0x41F4,0x42F4,0x13F5,0x14F5,
0x52F8,0x54F8,0x9DFD,0xA2FD,0x2A04,0x2F04,0x6C0A,0x710A,
0x1F0E,0x200E,0xAF0D,0xAF0D,0x6E09,0x6A09,0xF502,0xEF02,
0x80FC,0x7BFC,0x98F8,0x94F8,0x6EF8,0x6FF8,0x0AFB,0x0CFB,
0x64FE,0x66FE,0x6E00,0x6F00,0xEFFF,0xEDFF,0x79FD,0x76FD,
0x9FFB,0x9CFB,0x7DFC,0x7FFC,0x1700,0x1900,0xEF04,0xF204,
0xA008,0xA308,0x5D09,0x5D09,0x1307,0x1107,0x9303,0x9203,
0x6900,0x6600,0x79FE,0x78FE,0xDCFD,0xDBFD,0x05FE,0x04FE,
0x75FE,0x75FE,0xBAFE,0xBBFE,0x3BFF,0x3DFF,0x9700,0x9800,
0xC102,0xC202,0xA504,0xA604,0xC904,0xC804,0x7B02,0x7802,
0x0FFE,0x0BFE,0xE2F8,0xDFF8,0x3BF5,0x39F5,0x03F5,0x04F5,
0xF3F8,0xF8F8,0x1700,0x1C00,0x2F08,0x3608,0x170E,0x190E,
0x6D0F,0x6B0F,0xF70B,0xF30B,0x5F05,0x5B05,0x66FE,0x63FE,
0xD5F9,0xD3F9,0xD4F8,0xD4F8,0x97FA,0x98FA,0x5AFD,0x5DFD,
0x8DFF,0x8EFF,0x2900,0x2A00,0x57FF,0x56FF,0x5BFD,0x59FD,
0xF2FB,0xF2FB,0x3AFC,0x3BFC,0xEDFD,0xEFFD,0xE3FF,0xE5FF,
0xD500,0xD700,0xC100,0xC200,0x3F00,0x3F00,0x1800,0x1900,
0x8500,0x8600,0x7201,0x7301,0x1E02,0x1E02,0xE601,0xE501,
0x3201,0x3201,0x4900,0x4800,0xA7FF,0xA7FF,0x22FF,0x22FF,
0x33FE,0x31FE,0x7EFC,0x7CFC,0x3FFA,0x3DFA,0x48F8,0x47F8,
0xC7F7,0xC8F7,0x33FA,0x36FA,0x8BFF,0x90FF,0x4206,0x4806,
0x9A0B,0x9D0B,0x6D0D,0x6C0D,0xBD0A,0xB90A,0x7604,0x7004,
0xAEFC,0xA8FC,0x01F6,0xFDF5,0x08F2,0x05F2,0xD0F1,0xCFF1,
0xC9F4,0xCBF4,0x6FF9,0x72F9,0xBAFE,0xBDFE,0x4F03,0x5103,
0xEC05,0xEE05,0x4406,0x4306,0xC804,0xC504,0x5802,0x5602,
0x27FF,0x24FF,0xADFB,0xABFB,0x06F9,0x05F9,0x68F8,0x69F8,
0x3FFA,0x42FA,0x93FD,0x95FD,0x6700,0x6900,0x6101,0x6201,
0x0201,0x0201,0x3900,0x3800,0xDAFF,0xD9FF,0x7FFF,0x7FFF,
0x99FE,0x99FE,0xD7FD,0xD7FD,0x64FD,0x64FD,0x8BFC,0x8BFC,
0x0FFB,0x0EFB,0xFFF8,0xFEF8,0xC9F7,0xC8F7,0xCCF8,0xCEF8,
0x56FB,0x5AFB,0x0AFE,0x0BFE,0x6E00,0x6F00,0xA802,0xAB02,
0xC104,0xC304,0x0306,0x0206,0x8105,0x8105,0xC302,0xC002,
0x9CFE,0x97FE,0xA3FA,0xA0FA,0x2AF8,0x29F8,0xBDF7,0xBDF7,
0xDDF8,0xDEF8,0xD6FA,0xD8FA,0xF6FC,0xF8FC,0x72FF,0x74FF,
0x3F02,0x4202,0x0A05,0x0C05,0x1707,0x1707,0xE707,0xE707,
0xB806,0xB806,0x6803,0x6703,0xE2FE,0xDFFE,0xA4FA,0xA0FA,
0xA3F8,0xA2F8,0x01FA,0x03FA,0xA1FD,0xA5FD,0x2701,0x2901,
0x2D02,0x2D02,0x4900,0x4700,0x80FD,0x7DFD,0xA6FB,0xA4FB,
0xF2FB,0xF2FB,0x9AFD,0x9CFD,0xD6FE,0xD6FE,0x3AFE,0x39FE,
0xB4FB,0xB2FB,0xDAF8,0xD8F8,0x6EF7,0x6EF7,0x21F9,0x23F9,
0x5EFE,0x63FE,0xBE05,0xC405,0x8B0C,0x8E0C,0xBA0F,0xBA0F,
0x980D,0x950D,0xFB06,0xF606,0x5EFF,0x58FF,0xDAF9,0xD7F9,
0xB9F7,0xB8F7,0x28F8,0x27F8,0xEFF9,0xF0F9,0x77FC,0x79FC,
0x1FFF,0x1FFF,0x3101,0x3201,0x6702,0x6802,0x7502,0x7502,
0x5A01,0x5901,0x7DFF,0x7CFF,0xEEFC,0xECFC,0x64FA,0x62FA,
0x84F9,0x84F9,0xB0FB,0xB3FB,0xE600,0xEC00,0x0C07,0x1007,
0xB30A,0xB50A,0x1A0A,0x190A,0xA705,0xA405,0x83FF,0x7EFF,
0x04FA,0x00FA,0x21F7,0x20F7,0x1BF8,0x1DF8,0x17FD,0x1BFD,
0x3E04,0x4304,0x570A,0x5C0A,0xFE0B,0xFD0B,0x2808,0x2608,
0x5901,0x5301,0x21FB,0x1BFB,0xDCF8,0xDBF8,0xA1FB,0xA5FB,
0x5701,0x5B01,0xFB06,0xFF06,0xB209,0xB409,0x6E08,0x6D08,
0x4C03,0x4603,0x90FC,0x8BFC,0xD0F6,0xCDF6,0x2DF4,0x2BF4,
0x3CF5,0x3DF5,0x0DF9,0x10F9,0xA5FE,0xA9FE,0x5B05,0x6005,
0xC20B,0xC60B,0x000F,0x010F,0x280D,0x250D,0xBA06,0xB406,
0x8AFE,0x85FE,0x0FF8,0x0AF8,0xC8F5,0xC7F5,0xB2F8,0xB4F8,
0xA3FF,0xAAFF,0xF007,0xF707,0x410E,0x450E,0xC00F,0xC00F,
0x7E0C,0x7A0C,0x5206,0x4D06,0x3100,0x2D00,0x1AFC,0x18FC,
0x70FA,0x6FFA,0x22FA,0x22FA,0x2FFA,0x2EFA,0x5FFA,0x5EFA,
0xEAFA,0xEDFA,0xA2FC,0xA5FC,0x8DFF,0x8FFF,0xF102,0xF102,
0x0D06,0x0F06,0x5508,0x5808,0x7C09,0x7C09,0x7109,0x7209,
0xEF07,0xEE07,0x9604,0x9404,0x2F00,0x2A00,0xDEFB,0xDBFB,
0x0DF9,0x0BF9,0xC7F8,0xC8F8,0x25FB,0x28FB,0x7EFF,0x81FF,
0x0904,0x0C04,0xF507,0xF707,0x2D0B,0x2F0B,0x4E0D,0x500D,
0x360D,0x340D,0xC309,0xBF09,0xE102,0xDC02,0x6BFA,0x65FA,
0xB3F3,0xADF3,0x1DF1,0x1CF1,0x76F3,0x78F3,0x22FA,0x29FA,
0x7C03,0x8403,0x2C0C,0x330C,0xD510,0xD810,0xAF0F,0xAD0F,
0x8909,0x8409,0xEE01,0xEB01,0x1DFC,0x1AFC,0x71F9,0x70F9,
0x27F9,0x29F9,0xD5F9,0xD6F9,0xE3FA,0xE3FA,0xB8FB,0xB9FB,
0xB8FC,0xBAFC,0xBFFE,0xC1FE,0x5702,0x5A02,0x8807,0x8B07,
0xE10C,0xE50C,0x3A10,0x3D10,0xF60F,0xF40F,0x310C,0x2E0C,
0x4B06,0x4606,0x10FF,0x0CFF,0xEBF7,0xE6F7,0xC2F2,0xC0F2,
0x4AF1,0x4AF1,0x65F3,0x68F3,0x27F8,0x2BF8,0x2CFE,0x31FE,
0xC003,0xC403,0x7407,0x7507,0x7108,0x7208,0x4F07,0x4D07,
0xD104,0xCD04,0x8202,0x8102,0xD300,0xD200,0xDFFF,0xDEFF,
0x2CFF,0x2CFF,0x7AFE,0x7AFE,0x57FD,0x55FD,0xB8FB,0xB6FB,
0xFDF9,0xFCF9,0x55F9,0x55F9,0xBCFA,0xBDFA,0xB6FE,0xB9FE,
0x2B04,0x2F04,0xAF08,0xB208,0x740A,0x760A,0xC808,0xC708,
0xA804,0xA404,0xDEFF,0xDBFF,0x3EFC,0x3CFC,0xA1FA,0xA0FA,
0xB6FA,0xB5FA,0x4EFC,0x4FFC,0x19FF,0x1CFF,0xEF01,0xF101,
0x1304,0x1404,0x3D05,0x3E05,0x0105,0xFF04,0xB203,0xB103,
0x5401,0x5301,0xF7FD,0xF4FD,0x33FA,0x31FA,0xF5F7,0xF3F7,
0x35F8,0x35F8,0xE7FA,0xEAFA,0x26FF,0x2BFF,0xC903,0xCC03,
0x9607,0x9807,0x8208,0x8208,0xE405,0xE205,0x9001,0x8E01,
0x00FE,0xFDFD,0xF8FC,0xF8FC,0xB1FE,0xB3FE,0x5B02,0x5E02,
0xFC05,0xFE05,0xDA07,0xDB07,0x4007,0x4007,0x5B04,0x5904,
0x7F00,0x7C00,0xF5FC,0xF2FC,0x7CFA,0x7AFA,0x0BFA,0x0BFA,
0xCCFB,0xCDFB,0xF2FE,0xF4FE,0x0602,0x0802,0xA403,0xA503,
0xF602,0xF602,0xC400,0xC400,0xFBFE,0xF9FE,0xFAFE,0xFBFE,
0xB300,0xB600,0x6903,0x6A03,0xBD05,0xBE05,0xD605,0xD605,
0xCE02,0xCB02,0xF9FD,0xF5FD,0x44FA,0x43FA,0xB1F9,0xB1F9,
0x41FC,0x44FC,0xFFFF,0x0200,0x7503,0x7703,0xFE05,0x0006,
0x3808,0x3B08,0x1D0A,0x1D0A,0xA50A,0xA60A,0x2409,0x2309,
0x7505,0x7305,0xA000,0x9C00,0x14FC,0x11FC,0x02F9,0x02F9,
0x31F8,0x30F8,0x22FA,0x23FA,0x9BFE,0xA0FE,0x5C04,0x6104,
0xAE08,0xB008,0x2709,0x2609,0x1B06,0x1806,0xB001,0xAD01,
0x5CFE,0x59FE,0x27FD,0x26FD,0xDFFD,0xDFFD,0x4BFF,0x4DFF,
0xE9FF,0xE8FF,0xB8FE,0xB8FE,0x41FC,0x41FC,0xC7FA,0xC6FA,
0xCEFC,0xD0FC,0xE002,0xE502,0x1E0A,0x250A,0x850F,0x880F,
0xFF10,0xFF10,0x540E,0x530E,0x1809,0x1309,0x9903,0x9403,
0x3FFF,0x3EFF,0x1EFC,0x1CFC,0x9FF9,0x9EF9,0xDFF7,0xDEF7,
0x84F7,0x84F7,0x4AF9,0x4AF9,0x6DFD,0x70FD,0x7402,0x7802,
0x3306,0x3706,0x2907,0x2A07,0x6005,0x5F05,0x4202,0x4002,
0xE4FF,0xE3FF,0xD4FF,0xD3FF,0xA201,0xA301,0x0304,0x0504,
0x0005,0x0205,0x6303,0x6203,0x46FF,0x42FF,0x2EFB,0x2CFB,
0x04FA,0x03FA,0x9FFC,0xA1FC,0xC501,0xC801,0x9206,0x9406,
0xDF08,0xE008,0x8408,0x8308,0xC106,0xBD06,0xEF03,0xEC03,
0xF4FF,0xF1FF,0xA0FB,0x9EFB,0xA6F8,0xA4F8,0xDBF8,0xDCF8,
0x9FFC,0xA3FC,0x7202,0x7702,0x3A07,0x3D07,0xC808,0xC708,
0x0A07,0x0607,0x2703,0x2403,0x6AFF,0x69FF,0x63FD,0x63FD,
0x48FD,0x49FD,0x20FE,0x21FE,0xA6FE,0xA6FE,0x45FE,0x44FE,
0xFCFD,0xFBFD,0x09FF,0x0AFF,0xFD01,0x0002,0xF405,0xF605,
0x4A09,0x4D09,0x800A,0x810A,0x0209,0x0109,0xEA04,0xE604,
0x93FF,0x8FFF,0xB8FB,0xB5FB,0x08FB,0x09FB,0x5FFD,0x61FD,
0xC200,0xC600,0xA303,0xA503,0x8004,0x8204,0xC102,0xBF02,
0xA1FF,0x9EFF,0x94FC,0x92FC,0xFEFA,0xFDFA,0xDCFB,0xDCFB,
0xA5FE,0xA8FE,0xD901,0xDB01,0x5A03,0x5A03,0x6302,0x6102,
0x30FF,0x2DFF,0xD5FB,0xD2FB,0x52FA,0x53FA,0xF2FB,0xF4FB,
0x4200,0x4500,0x5205,0x5505,0x4408,0x4408,0xC207,0xC007,
0x2104,0x1E04,0xB0FF,0xACFF,0x2AFC,0x29FC,0xFCFA,0xFCFA,
0xF0FB,0xF0FB,0x26FE,0x27FE,0xC900,0xCC00,0x2103,0x2303,
0x6B04,0x6D04,0x5D04,0x5D04,0xC003,0xC103,0x8602,0x8502,
0x2C00,0x2800,0xF3FC,0xF0FC,0xF6F9,0xF3F9,0xD2F8,0xD2F8,
0x44FA,0x46FA,0x3EFD,0x41FD,0x7D00,0x7F00,0x0503,0x0703,
0x8804,0x8804,0xC204,0xC204,0x5303,0x5103,0xD700,0xD400,
0x86FE,0x85FE,0x5CFD,0x5BFD,0x53FD,0x54FD,0x85FD,0x86FD,
0xC5FC,0xC5FC,0x05FB,0x04FB,0x9DF9,0x9DF9,0x67FA,0x68FA,
0x18FE,0x1CFE,0x7703,0x7A03,0x1408,0x1608,0x0F0A,0x0E0A,
0xFA00,0xFA00,0xFF00,0xFF00,0x4200,0x4100,0xA0FE,0x9EFE,
0xCCFB,0xC9FB,0x99F9,0x98F9,0x6BFA,0x6DFA,0x72FF,0x76FF,
0x0407,0x0B07,0x800D,0x860D,0x4610,0x4910,0xE90E,0xE70E,
0x2E0A,0x290A,0xEF02,0xE802,0xE2FA,0xDCFA,0x1FF5,0x1BF5,
0xCCF3,0xCBF3,0x8DF6,0x90F6,0x0CFB,0x11FB,0x24FF,0x27FF,
0x2901,0x2B01,0x6101,0x6201,0x0701,0x0701,0xBD00,0xBE00,
0x1C01,0x1C01,0x5702,0x5802,0x2C04,0x2E04,0x1D05,0x1E05,
0x1F04,0x1D04,0x0601,0x0201,0x64FC,0x62FC,0x76F8,0x75F8,
0xF5F7,0xF6F7,0x77FB,0x7AFB,0x0501,0x0801,0x6206,0x6606,
0x2009,0x2209,0x6808,0x6508,0x3B05,0x3705,0x1A01,0x1701,
0xCAFD,0xC8FD,0x2DFC,0x2CFC,0xC9FB,0xC9FB,0x7AFB,0x7AFB,
0xA3FB,0xA4FB,0x2CFD,0x2EFD,0x5D00,0x5F00,0x8304,0x8604,
0x6407,0x0002,0xD309,0xD809,0x6C0D,0x6D0D,0xD80B,0xD50B,
0x3506,0x3006,0x8EFF,0x8AFF,0x0BFB,0x07FB,0x3BFA,0x3BFA,
0x73FC,0x76FC,0x88FF,0x8AFF,0xA301,0xA401,0x1102,0x1002,
0x1301,0x1201,0xD4FF,0xD3FF,0xADFE,0xADFE,0xC4FD,0xC4FD,
0x01FD,0x00FD,0xFAFC,0xFBFC,0x90FE,0x92FE,0x4402,0x4902,
0x5C06,0x6006,0xC507,0xC607,0xC004,0xBC04,0xD8FD,0xD4FD,
0x0BF7,0x05F7,0xF8F3,0xF7F3,0x98F5,0x99F5,0x8CFB,0x91FB,
0x8003,0x8603,0x630A,0x670A,0x1E0D,0x1F0D,0xC70A,0xC30A,
0x8504,0x8004,0xCBFC,0xC5FC,0x56F6,0x51F6,0x3EF3,0x3DF3,
0xA9F4,0xAAF4,0x85F9,0x88F9,0x6BFF,0x6EFF,0xA803,0xAA03,
0x5B05,0x5B05,0x9A04,0x9804,0x0102,0xFE01,0x07FF,0x04FF,
0x3DFD,0x3CFD,0x90FD,0x91FD,0x7CFF,0x7EFF,0x0C02,0x0E02,
0x6C04,0x6E04,0x7305,0x7305,0x3D04,0x3A04,0x9900,0x9600,
0x09FC,0x05FC,0x18F9,0x15F9,0x46F9,0x47F9,0xF5FB,0xF6FB,
0x4AFF,0x4DFF,0x9B01,0x9D01,0x1B02,0x1C02,0x9100,0x8F00,
0x7FFD,0x7CFD,0x4CFB,0x4AFB,0x3BFC,0x3DFC,0xCD00,0xD100,
0xC306,0xC806,0x7C0A,0x7F0A,0x9109,0x8E09,0x2A04,0x2504,
0xD7FC,0xD1FC,0x13F7,0x10F7,0x80F4,0x7FF4,0xDFF4,0xE0F4,
0x12F7,0x14F7,0xCEF9,0xD0F9,0xACFC,0xAEFC,0xA1FF,0xA4FF,
0x6D02,0x6F02,0xDD04,0xDD04,0xAA06,0xAB06,0xBB06,0xBA06,
0x8204,0x8004,0xB700,0xB300,0xDFFC,0xDCFC,0x17FB,0x17FB,
0xC3FB,0xC4FB,0x77FD,0x79FD,0x85FE,0x86FE,0x80FE,0x80FE,
0xDEFD,0xDDFD,0x79FD,0x79FD,0x5CFD,0x5CFD,0x5BFD,0x5CFD,
0xD5FD,0xD4FD,0x8EFF,0x90FF,0x7702,0x7902,0x7105,0x7305,
0xA806,0xA606,0x5305,0x5205,0x9501,0x9201,0xDDFC,0xD9FC,
0xF5F8,0xF0F8,0xADF7,0xADF7,0xE9F9,0xEBF9,0xE4FE,0xE8FE,
0x5F04,0x6304,0xDE07,0xE107,0xE107,0xE107,0x9E04,0x9A04,
0x8EFF,0x89FF,0x6EFA,0x6BFA,0xCCF6,0xCBF6,0xDCF5,0xDCF5,
0x41F8,0x42F8,0xC3FC,0xC7FC,0x3201,0x3401,0xB203,0xB403,
0x2C04,0x2C04,0x5B03,0x5903,0xBB01,0xB801,0x49FF,0x48FF,
0x96FC,0x93FC,0x44FB,0x43FB,0x51FC,0x53FC,0x71FF,0x73FF,
0x5E02,0x6002,0xEA02,0xEA02,0xE900,0xE600,0xDBFD,0xDAFD,
0x65FB,0x62FB,0x3AFA,0x39FA,0xD4FA,0xD4FA,0x1FFD,0x21FD,
0xB400,0xB700,0xEA03,0xEB03,0x0A05,0x0B05,0x2303,0x2103,
0xCFFE,0xCBFE,0x9AF9,0x95F9,0x92F5,0x90F5,0x8BF4,0x8CF4,
0xBFF7,0xC1F7,0x55FE,0x5AFE,0x9605,0x9B05,0x580A,0x5B0A,
0xE70A,0xE60A,0x0308,0x0108,0x9403,0x9203,0x84FF,0x80FF,
0xAAFC,0xA8FC,0xF0FA,0xEEFA,0x0DFA,0x0CFA,0xE0F9,0xE0F9,
0xE3FA,0xE5FA,0x2EFD,0x30FD,0x8400,0x8600,0xEA03,0xEB03,
0xD405,0xD505,0xD305,0xD205,0xE903,0xE703,0xFF00,0xFC00,
0x66FE,0x64FE,0x1CFD,0x1CFD,0x29FD,0x29FD,0x70FE,0x71FE,
0x4600,0x4700,0x6101,0x6101,0x7501,0x7401,0x6900,0x6700,
0x0DFF,0x0CFF,0x57FE,0x56FE,0x69FE,0x68FE,0x87FE,0x88FE,
0x27FE,0x25FE,0xC6FC,0xC3FC,0x38FB,0x35FB,0x52FA,0x51FA,
0x93FA,0x94FA,0xAEFB,0xAEFB,0x4CFD,0x4CFD,0x2C00,0x2F00,
0x1404,0x1504,0x3308,0x3608,0xB40A,0xB50A,0x010A,0xFF09,
0x7206,0x6E06,0x2801,0x2301,0x7CFB,0x78FB,0x91F6,0x8FF6,
0xA0F3,0x9DF3,0xCBF3,0xCCF3,0x91F7,0x94F7,0x73FD,0x77FD,
0x9E02,0xA202,0xF704,0xF804,0x6F04,0x6E04,0x8302,0x8102,
0xCE00,0xCC00,0x4900,0x4900,0x6F00,0x6F00,0x6501,0x6601,
0xCE02,0xCF02,0x5D03,0x5D03,0xF201,0xF001,0x10FF,0x0EFF,
0x35FC,0x33FC,0xF8FA,0xF7FA,0x0AFB,0x0BFB,0xEDFB,0xEDFB,
0x9CFC,0x9DFC,0x95FD,0x95FD,0x0FFF,0x10FF,0xB800,0xBA00,
0xD801,0xD601,0xD301,0xD301,0x6A01,0x6A01,0x4001,0x4001,
0x8B01,0x8B01,0x0902,0x0A02,0x1102,0x1102,0x6B01,0x6B01,
0x5100,0x5100,0xFAFE,0xF8FE,0x83FD,0x81FD,0xB0FC,0xAEFC,
0xAAFC,0xA9FC,0xA8FC,0xA8FC,0x4CFC,0x4BFC,0xD7FB,0xD6FB,
0x4DFC,0x4CFC,0x4DFE,0x4DFE,0xC701,0xC901,0x5805,0x5A05,
0x5807,0x5907,0xF106,0xEE06,0xA304,0xA104,0x8901,0x8801,
0x6FFE,0x6BFE,0xA7FC,0xA6FC,0x1FFD,0x20FD,0x8FFF,0x91FF,
0x3A02,0x3C02,0x0F03,0x0F03,0xB001,0xAE01,0x75FF,0x73FF,
0x87FD,0x86FD,0x0AFD,0x0AFD,0x97FD,0x97FD,0xD0FE,0xD1FE,
0x3800,0x3900,0xAD01,0xAD01,0xB302,0xB302,0xBD02,0xBB02,
0x5001,0x4F01,0x36FF,0x33FF,0x59FD,0x57FD,0x08FC,0x08FC,
0x0AFC,0x0AFC,0x1AFE,0x1DFE,0x0A02,0x0E02,0x8006,0x8306,
0xBF09,0xBF09,0xC409,0xC409,0xBD06,0xBA06,0xF801,0xF401,
0x1EFD,0x19FD,0x70F9,0x6EF9,0x9FF7,0x9DF7,0x37F8,0x37F8,
0xCAFB,0xCEFB,0x6801,0x6C01,0xA506,0xA906,0xFE08,0xFE08,
0x8E07,0x8B07,0x4803,0x4603,0x63FE,0x5FFE,0x2EFB,0x2CFB,
0x10FB,0x10FB,0x28FE,0x2BFE,0xE302,0xE702,0x6406,0x6506,
0xA106,0xA006,0x6903,0x6603,0x95FE,0x92FE,0x23FB,0x20FB,
0x14FB,0x14FB,0xCBFD,0xCDFD,0x3201,0x3301,0x4603,0x4603,
0x0403,0x0403,0xFD00,0xFD00,0x5BFE,0x57FE,0x9EFC,0x9DFC,
0x86FC,0x87FC,0xA1FD,0xA3FD,0xECFE,0xEDFE,0xE6FF,0xE7FF,
0xEF00,0xF000,0x6702,0x6802,0x5404,0x5504,0xBA05,0xBA05,
0x8405,0x8205,0x5203,0x4F03,0xB8FF,0xB4FF,0xBDFB,0xBAFB,
0x22F9,0x22F9,0x49F9,0x4AF9,0x86FC,0x88FC,0xD401,0xD801,
0x1907,0x1C07,0x060A,0x060A,0x3E09,0x3C09,0xD904,0xD504,
0x09FF,0x05FF,0x99FA,0x97FA,0x5EF9,0x5EF9,0xC8FB,0xCBFB,
0x7D00,0x8000,0xC104,0xC304,0x7906,0x7B06,0x6B05,0x6905,
0xA002,0x9E02,0x95FF,0x93FF,0x79FD,0x79FD,0x7DFC,0x7DFC,
0x58FC,0x58FC,0x5FFD,0x61FD,0x22FF,0x23FF,0xC200,0xC300,
0xEE01,0xEF01,0x4102,0x4002,0x9901,0x9801,0x3600,0x3400,
0xD0FE,0xCFFE,0xA5FD,0xA4FD,0x10FD,0x0FFD,0x57FD,0x59FD,
0xFBFE,0xFCFE,0xFB00,0xFC00,0x0002,0xFF01,0x3401,0x8D0B,
0x0B07,0x0907,0xED00,0xE800,0x30FB,0x2DFB,0x90F7,0x8EF7,
0x74F6,0x74F6,0x49F7,0x4BF7,0xF5F8,0xF6F8,0x25FA,0xAF00,
0xC700,0xC700,0xEAFF,0xE8FF,0x0CFE,0x0BFE,0xC5FB,0xC4FB,
0x42FA,0x40FA,0x93FA,0x92FA,0xBDFC,0xBEFC,0xBCFF,0xBDFF,
0xE801,0xEB01,0x1002,0x1102,0xD3FF,0xD0FF,0x41FC,0x3EFC,
0xC5F8,0xC2F8,0x38F7,0x37F7,0x4FF9,0x53F9,0x81FE,0x85FE,
0x9A04,0x9F04,0x5308,0x5608,0xA107,0xA007,0x7402,0x6E02,
0xE2FA,0xDCFA,0x1BF4,0x18F4,0x97F1,0x96F1,0x51F4,0x54F4,
0x57FB,0x5DFB,0x8F03,0x9603,0x6909,0x6C09,0xB20A,0xB40A,
0x7A07,0x7707,0x1002,0x0B02,0x87FC,0x82FC,0xDBF8,0xD9F8,
0x6FF7,0x6EF7,0xA0F7,0xA0F7,0xC5F8,0xC5F8,0x2DFB,0x2DFB,
0x89FE,0x8BFE,0x4702,0x4A02,0x0B05,0x0C05,0xE004,0xDF04,
0x3301,0x2F01,0x52FB,0x4EFB,0x96F6,0x93F6,0xD8F5,0xD8F5,
0x1FFA,0x22FA,0x5101,0x5601,0xD507,0xD807,0x000A,0x010A,
0x8606,0x8206,0x40FF,0x39FF,0x04F8,0xFFF7,0xF6F4,0xF4F4,
0x50F7,0x52F7,0x72FC,0x76FC,0x1E00,0x2000,0xC200,0xC000,
0x78FF,0x77FF,0x7EFE,0x7EFE,0x18FF,0x1AFF,0x1900,0x1A00,
0x2D00,0x2C00,0x1AFF,0x18FF,0xD5FD,0xD4FD,0x5FFD,0x5EFD,
0x8CFE,0x8EFE,0x4901,0x4B01,0x5E04,0x6004,0x3306,0x3406,
0x3D05,0x3C05,0x3001,0x2C01,0xA5FB,0x9FFB,0x2CF7,0x28F7,
0xCBF5,0xCAF5,0x6FF8,0x73F8,0xB0FE,0xB5FE,0x6906,0x6E06,
0xF70B,0xF90B,0xF40B,0xF30B,0x3C06,0x3606,0x88FE,0x84FE,
0xCBF8,0xC8F8,0xB8F7,0xB7F7,0x3FFB,0x42FB,0x1E00,0x2300,
0xBE03,0xC003,0xCB04,0xCB04,0x8603,0x8503,0xBA00,0xB600,
0x0BFE,0x08FE,0x79FC,0x78FC,0x65FC,0x66FC,0x0BFD,0x0BFD,
0x03FE,0x04FE,0x9CFF,0x9EFF,0xB601,0xB701,0x9E03,0xA003,
0xD604,0xD504,0xA904,0xA704,0xDE02,0xDC02,0x8FFF,0x8BFF,
0xFCFB,0xFAFB,0xB0F9,0xAFF9,0xEBF9,0xEBF9,0xC9FC,0xCBFC,
0x7F00,0x8100,0xF602,0xF602,0xA502,0xA302,0x1500,0x1300,
0x4FFD,0x4DFD,0x42FC,0x42FC,0x8CFD,0x8CFD,0x8500,0x8700,
0xF403,0xF803,0x1906,0x1B06,0x5005,0x4F05,0x9501,0x9101,
0xFBFC,0xF6FC,0x4EFA,0x4CFA,0xEDFA,0xEFFA,0xA5FE,0xA7FE,
0x9103,0x9503,0x5707,0x5B07,0xF807,0xF907,0xFB04,0xF704,
0x82FF,0x7CFF,0x66FA,0x63FA,0x1EF8,0x1EF8,0x46F9,0x49F9,
0xD4FC,0xD5FC,0xFA00,0xFC00,0xE203,0xE403,0x6D04,0x6D04,
0x9C02,0x9C02,0x69FF,0x66FF,0x0DFC,0x09FC,0xA4F9,0xA4F9,
0x7AF9,0x7BF9,0xD2FB,0xD5FB,0x86FF,0x88FF,0x3703,0x3903,
0xB705,0xB605,0xB306,0xB306,0xBF05,0xBE05,0x0503,0x0303,
0xBAFE,0xB5FE,0xF7F9,0xF2F9,0x6EF6,0x6CF6,0x50F5,0x4FF5,
0x26F7,0x29F7,0x79FB,0x7DFB,0xC800,0xCC00,0xCF04,0xD204,
0x0606,0x0606,0x7204,0x7104,0xA400,0xA000,0x26FC,0x23FC,
0xECF8,0xEAF8,0xF2F7,0xF1F7,0xB2F9,0xB4F9,0x0BFE,0x0EFE,
0xDC02,0xDF02,0x6F05,0x7305,0x7604,0x7504,0x4F00,0x4A00,
0x20FB,0x1BFB,0x7CF7,0x7BF7,0x96F7,0x95F7,0x11FB,0x13FB,
0xDEFF,0xE0FF,0x8C03,0x8F03,0xF304,0xF404,0xF403,0xF403,
0xB801,0xB801,0x4FFF,0x4DFF,0xC3FC,0xC2FC,0xACFA,0xABFA,
0xB1F9,0xB0F9,0xFAF9,0xFBF9,0x48FB,0x48FB,0x82FD,0x85FD,

};


uint8_t test_enc_output_buffer[NUM_INPUT_PCM_CHANNELS][MPA_FRAME_SIZE]; //1152*2


static unsigned int bitstream_buffer[NUM_OUT_BUFFERS][MPA_MAX_CODED_FRAME_SIZE / (sizeof(int))];
//static int bit_stream_pattern[FIFO_BUFFER_SIZE*NUM_FIFO_BUFFERS];
const int bitstream_buffer_size = MPA_MAX_CODED_FRAME_SIZE;

/******************************************************************************
**          Typedefs/Enumerations
******************************************************************************/
typedef unsigned char   Byte;

AVCodecContext  avctx;
MpegAudioContext  audioctx;


typedef struct {
    volatile uint16_t       Write;
    volatile uint16_t       Read;
    uint8_t                 busy;
} MP2_STREAM;

MP2_STREAM mp2_Stream;


extern void MP2_init(void)
{

    //if (!(avctx.priv_data= (void*)calloc(1,sizeof(MpegAudioContext))))
    //{
    /// printf("fail to allocate MpegAudioContext struct...\n");
    //return CODEC_MEMORY_ALLOCATION_ERROR;
    //}

    memset(&mp2_Stream, 0, sizeof(mp2_Stream));
    avctx.sample_rate = 48000;
    avctx.channels = 2;
    avctx.bit_rate = 384000;//1152 byte data
    avctx.priv_data = (void*)&audioctx;

    if (0 > MPA_encode_init(&avctx))
    {
        printf("fail to call MPA_encode_init...\n");

    }
}



int MP2_Encode(short *buf, int len)
{
    uint16_t head;
    int num_output_bytes_produced;

    head = MP2BUF_NEXT_PT(mp2_Stream.Write, 1, NUM_OUT_BUFFERS);

    if(head == mp2_Stream.Read) {
		printf("audio busy\r\n");
        return 0;
    }
	
    num_output_bytes_produced = MPA_encode_frame(&avctx,
                                (unsigned char*)bitstream_buffer[mp2_Stream.Write],
                                bitstream_buffer_size,  //1792
                                (void*)&test_enc_input_buffer[0]);
    mp2_Stream.Write = head;
    return CODEC_SUCCESS;
}


void reset_MP2_encode(void)
{
    // clean up the mess after encoding finishes
    MPA_encode_close(&avctx);
    //av_free(avctx.priv_data);
}



void pollint_Audio_TS(void)
{
    if(mp2_Stream.Write == mp2_Stream.Read) {

    } else if (GetTSmuxAudioState()==0){
		
		
		Start_Audio_Frame();
		//memcpy(test_enc_output_buffer,(unsigned char*)bitstream_buffer[mp2_Stream.Read], 1152);
        PackAudioTs((unsigned char*)bitstream_buffer[mp2_Stream.Read], 1152);
	//		udp_Send_Pack((unsigned char*)&bitstream_buffer[mp2_Stream.Read][0], 384,NULL, 0);
	//		udp_Send_Pack((unsigned char*)&bitstream_buffer[mp2_Stream.Read][96], 384,NULL, 0);
	//		udp_Send_Pack((unsigned char*)&bitstream_buffer[mp2_Stream.Read][192], 384,NULL, 0);
			
        mp2_Stream.Read = MP2BUF_NEXT_PT(mp2_Stream.Read, 1, NUM_OUT_BUFFERS);
    }
}

