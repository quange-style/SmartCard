#ifndef __SAM_H__
#define __SAM_H__

//SAM  卡速率
#define SAM_BAUDRATE_9600 0
#define SAM_BAUDRATE_19200 1
#define SAM_BAUDRATE_38400 2
#define SAM_BAUDRATE_57600 3
#define SAM_BAUDRATE_115200 4

//SAM  卡插槽序号，从0  开始
#define SAM_IDX_METRO 0
#define SAM_IDX_OCT   1

#ifdef __cplusplus
extern "C"
{
#endif

//========================================================
//功能:对sam  设备进行初始化,  在应用启动时必须要做
//入口参数
//	                无
//出口参数
//                   无
//返回值
//       0=成功
//      -1=打开设备句柄出错
//========================================================
extern int samdev_init(void);

//========================================================
//功能:设备sam  卡工作速率
//入口参数
//	 nsamsck=sam卡对应卡槽
//    baudrate=sam卡速率序号0=9600,1=19200,2=38400,3=57600,4=115200
//出口参数
//       无
//返回值
//       无
//========================================================
extern void sam_setbaud(unsigned char socket,unsigned char baudrate);

//========================================================
//功能:对指定槽号的sam  卡进行复位
//入口参数
//	 nsamsck=sam卡对应卡槽
//出口参数
//       saminf=6字节的sam  卡号
//       lpdata=复位返回信息
//返回值
//       0=复位失败
//       >0复位成功后返回的数据
//========================================================
extern unsigned char sam_rst(unsigned char socket,unsigned char *data);

//========================================================
//功能:发送指令让sam  卡执行
//入口参数
//	 nsamsck=sam卡对应卡槽
//    nlen=要发送的指令长度(包括指令结构和数据)
//    indata=发送的指令数据流
//出口参数
//       outdata=sam卡执行完成后返回的数据
//       sam_sw=sam执行状态码,  一般0x9000  是成功
//返回值
//       0=执行无返回
//       >0执行有返回，需结合sam_sw  判断是否成功执行
//========================================================
extern unsigned char sam_cmd(unsigned char socket, unsigned char inlen,unsigned char *indata,unsigned char *outdata, unsigned short int *sam_sw);

extern char sam_version(unsigned char *version);

extern void samdev_destory(void);

#ifdef __cplusplus
}
#endif

#endif





