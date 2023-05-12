
#include <stdio.h>
#include <string.h>
#include <unistd.h> //usleep
#include <sys/io.h>
#include <fcntl.h>              /* open */
#include <linux/ioctl.h>
#include <stdarg.h>
#include "iso14443.h"
//#include <time.h>
#include <sys/ioctl.h>

#include "../api/Records.h"
#include "../link/myprintf.h"

#define uchar unsigned char
#define uint unsigned short

/*****************************************************************************
*MFIRE卡命令字
*****************************************************************************/
#define PICC_REQIDL           0x26                                     /*寻天线区内未进入休眠状态的卡*/
#define PICC_REQALL           0x52                                     /*寻天线区内全部卡*/
#define PICC_ANTICOLL1        0x93                                     /*防冲撞*/
#define PICC_ANTICOLL2        0x95                                     /*防冲撞*/
#define PICC_AUTHENT1A        0x60                                     /*验证A密钥*/
#define PICC_AUTHENT1B        0x61                                     /*验证B密钥*/
#define PICC_READ             0x30                                     /*读块*/
#define PICC_WRITE            0xA0                                     /*写块*/
#define PICC_WRITE_UL         0xA2                                     /*写块UL*/
#define PICC_DECREMENT        0xC0                                     /*扣款*/
#define PICC_INCREMENT        0xC1                                     /*充值*/
#define PICC_RESTORE          0xC2                                     /*调钱包到缓冲区*/
#define PICC_TRANSFER         0xB0                                     /*保存缓冲区中数据*/
#define PICC_HALT             0x50                                     /*休眠*/


/*****************************************************************************
*RC663 命令字Command Register Contents (0x00)
*****************************************************************************/
#define PHHAL_HW_RC663_BIT_STANDBY              0x80U   /**< Standby bit; If set, the IC transits to standby mode. */
#define PHHAL_HW_RC663_CMD_IDLE                 0x00U   /**< No action; cancels current command execution. */
#define PHHAL_HW_RC663_CMD_LPCD                 0x01U   /**< Low Power Card Detection. */
#define PHHAL_HW_RC663_CMD_LOADKEY              0x02U   /**< Reads a key from the FIFO buffer and puts it into the key buffer. */
#define PHHAL_HW_RC663_CMD_MFAUTHENT            0x03U   /**< Performs the Mifare standard authentication (in Mifare Reader/Writer mode only). */
#define PHHAL_HW_RC663_CMD_ACKREQ               0x04U   /**< Performs a Query, a Ack and a Req-Rn for EPC V2. */
#define PHHAL_HW_RC663_CMD_RECEIVE              0x05U   /**< Activates the receiver circuitry. */
#define PHHAL_HW_RC663_CMD_TRANSMIT             0x06U   /**< Transmits data from the FIFO buffer to Card. */
#define PHHAL_HW_RC663_CMD_TRANSCEIVE           0x07U   /**< Like #PHHAL_HW_RC663_CMD_TRANSMIT but automatically activates the receiver after transmission is finished. */
#define PHHAL_HW_RC663_CMD_WRITEE2              0x08U   /**< Gets one byte from FIFO buffer and writes it to the internal EEPROM. */
#define PHHAL_HW_RC663_CMD_WRITEE2PAGE          0x09U   /**< Gets up to 64 Bytes from FIFO buffer and writes it to the EEPROM. */
#define PHHAL_HW_RC663_CMD_READE2               0x0AU   /**< Reads data from EEPROM and puts it into the FIFO buffer. */
#define PHHAL_HW_RC663_CMD_LOADREG              0x0CU   /**< Reads data from EEPROM and initializes the registers. */
#define PHHAL_HW_RC663_CMD_LOADPROTOCOL         0x0DU   /**< Reads data from EEPROM and initializes the registers needed for a protocol change. */
#define PHHAL_HW_RC663_CMD_LOADKEYE2            0x0EU   /**< Copies a Mifare key from the EEPROM into the key buffer. */
#define PHHAL_HW_RC663_CMD_STOREKEYE2           0x0FU   /**< Stores a Mifare key into the EEPROM. */
#define PHHAL_HW_RC663_CMD_SOFTRESET            0x1FU   /**< Resets the IC. */
#define PHHAL_HW_RC663_MASK_COMMAND             0x1FU   /**< Mask for Command-bits. */


#define MI_UNKNOWN_COMMAND   					 0xFF //
/*****************************************************************************
*RC663 FIFO长度定义
*****************************************************************************/
#define DEF_FIFO_LENGTH       512                                /*FIFO size=512byte*/
/*****************************************************************************
*RC663寄存器定义
*****************************************************************************/
#define PHHAL_HW_RC663_REG_COMMAND              0x00	//Command Register

#define PHHAL_HW_RC663_REG_HOSTCTRL             0x01	//Configure Host and SAM interfaces.

#define PHHAL_HW_RC663_REG_FIFOCONTROL          0x02	// FIFO-Control Register

#define PHHAL_HW_RC663_REG_WATERLEVEL           0x03	// FIFO WaterLevel configuration

#define PHHAL_HW_RC663_REG_FIFOLENGTH           0x04	//FIFO-Length Register

#define PHHAL_HW_RC663_REG_FIFODATA             0x05	//FIFO-Data Register

#define PHHAL_HW_RC663_REG_IRQ0                 0x06	//IRQ Registers

#define PHHAL_HW_RC663_REG_IRQ1                 0x07	//IRQ1 Register

#define PHHAL_HW_RC663_REG_IRQ0EN               0x08	//IRQ0EN Register

#define PHHAL_HW_RC663_REG_IRQ1EN               0x09	// IRQ1EN Register.

#define PHHAL_HW_RC663_REG_ERROR                0x0A	//Error Register.

#define PHHAL_HW_RC663_REG_STATUS               0x0B	//Status Register

#define PHHAL_HW_RC663_REG_RXBITCTRL            0x0C	//Rx-Bit-Control Register

#define PHHAL_HW_RC663_REG_RXCOLL               0x0D	// Rx-Coll Register

#define PHHAL_HW_RC663_REG_TCONTROL             0x0E	//Timer Registers

#define PHHAL_HW_RC663_REG_T0CONTROL            0x0F	//Timer0 Control Register

#define PHHAL_HW_RC663_REG_T0RELOADHI           0x10	//Timer0 Reload(High) Register

#define PHHAL_HW_RC663_REG_T0RELOADLO           0x11	//Timer0 Reload(Low) Register

#define PHHAL_HW_RC663_REG_T0COUNTERVALHI       0x12	//Timer0 Counter(High) Register

#define PHHAL_HW_RC663_REG_T0COUNTERVALLO       0x13	//Timer0 Counter(Low) Register

#define PHHAL_HW_RC663_REG_T1CONTROL            0x14	// Timer1 Control Register

#define PHHAL_HW_RC663_REG_T1RELOADHI           0x15	//Timer1 Reload(High) Register

#define PHHAL_HW_RC663_REG_T1RELOADLO           0x16	//Timer1 Reload(Low) Register

#define PHHAL_HW_RC663_REG_T1COUNTERVALHI       0x17	//Timer1 Counter(High) Register

#define PHHAL_HW_RC663_REG_T1COUNTERVALLO       0x18	//Timer1 Counter(Low) Register

#define PHHAL_HW_RC663_REG_T2CONTROL            0x19	//Timer2 Control Register

#define PHHAL_HW_RC663_REG_T2RELOADHI           0x1A	//Timer2 Reload(High) Register

#define PHHAL_HW_RC663_REG_T2RELOADLO           0x1B	//Timer2 Reload(Low) Register

#define PHHAL_HW_RC663_REG_T2COUNTERVALHI       0x1C	// Timer2 Counter(High) Register

#define PHHAL_HW_RC663_REG_T2COUNTERVALLO       0x1D	// Timer2 Counter(Low) Register

#define PHHAL_HW_RC663_REG_T3CONTROL            0x1E	//Timer3 Control Register

#define PHHAL_HW_RC663_REG_T3RELOADHI           0x1F	//Timer3 Reload(High) Register

#define PHHAL_HW_RC663_REG_T3RELOADLO           0x20	//Timer3 Reload(Low) Register

#define PHHAL_HW_RC663_REG_T3COUNTERVALHI       0x21	//Timer3 Counter(High) Register

#define PHHAL_HW_RC663_REG_T3COUNTERVALLO       0x22	//Timer3 Counter(Low) Register

#define PHHAL_HW_RC663_REG_T4CONTROL            0x23	//Timer4 Control Register

#define PHHAL_HW_RC663_REG_T4RELOADHI           0x24	//Timer4 Reload(High) Register

#define PHHAL_HW_RC663_REG_T4RELOADLO           0x25	//Timer4 Reload(Low) Register

#define PHHAL_HW_RC663_REG_T4COUNTERVALHI       0x26	//Timer4 Counter(High) Register.

#define PHHAL_HW_RC663_REG_T4COUNTERVALLO       0x27	// Timer4 Counter(Low) Register

#define PHHAL_HW_RC663_REG_DRVMODE              0x28	//Driver Mode Register

#define PHHAL_HW_RC663_REG_TXAMP                0x29	// Tx Amplifier Register

#define PHHAL_HW_RC663_REG_DRVCON               0x2A	// Driver Control Register.

#define PHHAL_HW_RC663_REG_TXI                  0x2B	//TxI Register

#define PHHAL_HW_RC663_REG_TXCRCCON             0x2C	// Tx-CRC Control Register

#define PHHAL_HW_RC663_REG_RXCRCCON             0x2D	//Rx-CRC Control Register

#define PHHAL_HW_RC663_REG_TXDATANUM            0x2E	// Tx-DataNum Register

#define PHHAL_HW_RC663_REG_TXMODWIDTH           0x2F	//Tx-Modwidth Register

#define PHHAL_HW_RC663_REG_TXSYM10BURSTLEN      0x30	//Symbol 0 and 1 Register

#define PHHAL_HW_RC663_REG_TXWAITCTRL           0x31	//Tx-Wait Control Register

#define PHHAL_HW_RC663_REG_TXWAITLO             0x32	//TxWaitLo Register

#define PHHAL_HW_RC663_REG_FRAMECON             0x33	// Frame control register

#define PHHAL_HW_RC663_REG_RXSOFD               0x34	//RxSOFD Register

#define PHHAL_HW_RC663_REG_RXCTRL               0x35	//Rx Control Register

#define PHHAL_HW_RC663_REG_RXWAIT               0x36	//Rx-Wait Register

#define PHHAL_HW_RC663_REG_RXTHRESHOLD          0x37	//Rx-Threshold Register

#define PHHAL_HW_RC663_REG_RCV                  0x38	// Receiver Register

#define PHHAL_HW_RC663_REG_RXANA                0x39	//Rx-Analog Register

#define PHHAL_HW_RC663_REG_SERIALSPEED          0x3B	//Serial Speed Register.

#define PHHAL_HW_RC663_REG_LPO_TRIMM            0x3C	//LPO_TRIMM Register

#define PHHAL_HW_RC663_REG_PLL_CTRL             0x3D	//PLL Control Register.

#define PHHAL_HW_RC663_REG_PLL_DIV              0x3E	//PLL DivO Register

#define PHHAL_HW_RC663_REG_LPCD_QMIN            0x3F	// LPCD QMin Register.

#define PHHAL_HW_RC663_REG_LPCD_QMAX            0x40	//LPCD QMax Register

#define PHHAL_HW_RC663_REG_LPCD_IMIN            0x41	//LPCD IMin Register

#define PHHAL_HW_RC663_REG_LPCD_RESULT_I        0x42	//LPCD Result(I) Register

#define PHHAL_HW_RC663_REG_LPCD_RESULT_Q        0x43	//LPCD Result(Q) Register

#define PHHAL_HW_RC663_REG_VERSION              0x7F	//Version Register


/*****************************************************************************
*和RC633通讯时返回的错误代码
*****************************************************************************/
#define MI_OK                 0
#define MI_CHK_OK             0

#define MI_NOTAGERR        (uchar)   (-1)
#define MI_CHK_FAILED      (uchar)   (-1)
#define MI_CRCERR           (uchar)  (-2)
#define MI_CHK_COMPERR      (uchar)  (-2)
#define MI_EMPTY            (uchar)  (-3)
#define MI_AUTHERR          (uchar)  (-4)
#define MI_PARITYERR       (uchar)   (-5)
#define MI_CODEERR          (uchar)  (-6)
#define MI_SERNRERR        (uchar)   (-8)
#define MI_KEYERR          (uchar)   (-9)
#define MI_NOTAUTHERR      (uchar)   (-10)
#define MI_BITCOUNTERR     (uchar)   (-11)
#define MI_BYTECOUNTERR    (uchar)   (-12)
#define MI_IDLE             (uchar)  (-13)
#define MI_TRANSERR         (uchar)  (-14)
#define MI_WRITEERR         (uchar)  (-15)
#define MI_INCRERR          (uchar)  (-16)
#define MI_DECRERR          (uchar)  (-17)
#define MI_READERR          (uchar)  (-18)
#define MI_OVFLERR          (uchar)  (-19)
#define MI_POLLING          (uchar)  (-20)
#define MI_FRAMINGERR       (uchar)  (-21)
#define MI_ACCESSERR        (uchar)  (-22)
//#define MI_UNKNOWN_COMMAND    (-23)
#define MI_COLLERR          (uchar)  (-24)
#define MI_RESETERR        (uchar)   (-25)
#define MI_INITERR          (uchar)  (-25)
#define MI_INTERFACEERR     (uchar)  (-26)
#define MI_ACCESSTIMEOUT    (uchar)  (-27)
#define MI_NOBITWISEANTICOLL (uchar) (-28)
#define MI_QUIT             (uchar)  (-30)
#define MI_RECBUF_OVERFLOW  (uchar)  (-50)
#define MI_SENDBYTENR        (uchar) (-51)
#define MI_SENDBUF_OVERFLOW  (uchar) (-53)
#define MI_BAUDRATE_NOT_SUPPORTED   (uchar)    (-54)
#define MI_SAME_BAUDRATE_REQUIRED  (uchar)     (-55)
#define MI_WRONG_PARAMETER_VALUE   (uchar)     (-60)
#define MI_BREAK                   (uchar)     (-99)
#define MI_NY_IMPLEMENTED            (uchar)   (-100)
#define MI_NO_MFRC                  (uchar)    (-101)
#define MI_MFRC_NOTAUTH             (uchar)    (-102)
#define MI_WRONG_DES_MODE            (uchar)   (-103)
#define MI_HOST_AUTH_FAILED         (uchar)    (-104)
#define MI_WRONG_LOAD_MODE           (uchar)   (-106)
#define MI_WRONG_DESKEY             (uchar)    (-107)
#define MI_MKLOAD_FAILED            (uchar)    (-108)
#define MI_FIFOERR                  (uchar)    (-109)
#define MI_WRONG_ADDR               (uchar)    (-110)
#define MI_DESKEYLOAD_FAILED        (uchar)    (-111)
#define MI_WRONG_SEL_CNT             (uchar)   (-114)
#define MI_WRONG_TEST_MODE           (uchar)   (-117)
#define MI_TEST_FAILED               (uchar)   (-118)
#define MI_TOC_ERROR                 (uchar)   (-119)
#define MI_COMM_ABORT                (uchar)   (-120)
#define MI_INVALID_BASE              (uchar)   (-121)
#define MI_MFRC_RESET                (uchar)   (-122)
#define MI_WRONG_VALUE               (uchar)   (-123)
#define MI_VALERR                    (uchar)   (-124)
#define MI_COM_ERR                   (uchar)   (-125)

#define Timecount	100



/** \name Error Register Contents (0x0A)
*/
/*@{*/
#define PHHAL_HW_RC663_BIT_EE_ERR               0x80U
#define PHHAL_HW_RC663_BIT_FIFOWRERR            0x40U
#define PHHAL_HW_RC663_BIT_FIFOOVL              0x20U
#define PHHAL_HW_RC663_BIT_MINFRAMEERR          0x10U
#define PHHAL_HW_RC663_BIT_NODATAERR            0x08U
#define PHHAL_HW_RC663_BIT_COLLDET              0x04U
#define PHHAL_HW_RC663_BIT_PROTERR              0x02U
#define PHHAL_HW_RC663_BIT_INTEGERR             0x01U

/** \name Communication Errors
*/
/*@{*/
#define PH_ERR_IO_TIMEOUT               0x0001U /**< No reply received, e.g. PICC removal. */
#define PH_ERR_INTEGRITY_ERROR          0x0002U /**< Wrong CRC or parity detected. */
#define PH_ERR_COLLISION_ERROR          0x0003U /**< A collision occured. */
#define PH_ERR_BUFFER_OVERFLOW          0x0004U /**< Attempt to write beyond buffer size. */
#define PH_ERR_FRAMING_ERROR            0x0005U /**< Invalid frame format. */
#define PH_ERR_PROTOCOL_ERROR           0x0006U /**< Received response violates protocol. */
#define PH_ERR_AUTH_ERROR               0x0007U /**< Authentication error. */
#define PH_ERR_READ_WRITE_ERROR         0x0008U /**< A Read or Write error occured in RAM/ROM or Flash. */
#define PH_ERR_TEMPERATURE_ERROR        0x0009U /**< The RC sensors signal overheating. */
#define PH_ERR_RF_ERROR                 0x000AU /**< Error on RF-Interface. */
#define PH_ERR_INTERFACE_ERROR          0x000BU /**< An error occured in RC communication. */
#define PH_ERR_LENGTH_ERROR             0x000CU /**< A length error occured. */
#define PH_ERR_INTERNAL_ERROR           0x007FU /**< An internal error occured. */

/*@{*/
#define PH_ERR_SUCCESS                  0x0000U /**< Returned in case of no error. */
#define PH_ERR_SUCCESS_CHAINING         0x0071U /**< Rx chaining is not complete, further action needed. */
#define PH_ERR_SUCCESS_INCOMPLETE_BYTE  0x0072U /**< An incomplete byte was received. */

#define PICC_RESET 0xE0
#define PPSS       0xD0


#define RF_IOC_MAGIC 'R'

#define ISO14443A_INIT				_IO(RF_IOC_MAGIC, 0)
#define ISO14443A_REQ				_IO(RF_IOC_MAGIC, 1)
#define ISO14443A_ANT				_IO(RF_IOC_MAGIC, 2)
#define ISO14443A_SEL				_IO(RF_IOC_MAGIC, 3)
#define ISO14443A_AUT				_IO(RF_IOC_MAGIC, 4)
#define ISO14443A_RED				_IO(RF_IOC_MAGIC, 5)
#define ISO14443A_WRE				_IO(RF_IOC_MAGIC, 6)
#define ISO14443A_RES				_IO(RF_IOC_MAGIC, 7)
#define ISO14443A_TRA 				_IO(RF_IOC_MAGIC, 8)
#define ISO14443A_VAL 				_IO(RF_IOC_MAGIC, 9)
#define ISO14443A_HAL 				_IO(RF_IOC_MAGIC, 10)


#define ISO14443A_REQA_UL			_IO(RF_IOC_MAGIC, 11)
#define ISO14443A_ANT_UL			_IO(RF_IOC_MAGIC, 12)
#define ISO14443A_SEL_UL			_IO(RF_IOC_MAGIC, 13)
#define ISO14443A_RED_UL			_IO(RF_IOC_MAGIC, 14)
#define ISO14443A_WRE_UL			_IO(RF_IOC_MAGIC, 15)
#define ISO14443A_CWRE_UL			_IO(RF_IOC_MAGIC, 16)
#define ISO14443A_HAT_UL			_IO(RF_IOC_MAGIC, 17)

#define MIFAREPRO_RST				_IO(RF_IOC_MAGIC, 18)
#define MIFAREPRO_COM				_IO(RF_IOC_MAGIC, 19)

#define READ_BASE_REG				_IO(RF_IOC_MAGIC, 20)
#define WRITE_BASE_REG				_IO(RF_IOC_MAGIC, 21)
#define RF_SET						_IO(RF_IOC_MAGIC, 22)
#define RF_VERSION					_IO(RF_IOC_MAGIC, 23)
#define RF_RST						_IO(RF_IOC_MAGIC, 24)

#define TYPEA 0
#define TYPEB080 1
#define TYPEB020 2


//static unsigned char gpKey080[8]={0x68,0xC1,0x23,0x97,0xAE,0x94,0x1C,0x1C};


struct TranSciveBuffer
{
    uchar  MfCommand;
    uint MfLength;
    uchar  MfData[200];
} MfComData;

unsigned char	g_cFSD = 48;
unsigned char	g_cFWI;
unsigned char	g_cCID_NAD;
unsigned char	g_cPCB;

static int g_currentreader=0;
static int g_hostreader=-1;
//static int g_slavereader=-1;


char ISO14443A_Init_default(void)
{
	return 0;
}

int antenna_switch(int idx);



unsigned char ReadRC(unsigned char addr)
{
    unsigned char tmp=addr;
    unsigned char i;

    ioctl(g_hostreader, READ_BASE_REG, &tmp);
    for (i=0;i<50;i++);

    return(tmp);

}


void WriteRC(unsigned char addr, unsigned char value)
{
    char status = MI_OK;
    unsigned char tmp[2];
    unsigned char i;


    tmp[0] = addr;
    tmp[1] = value;
    status = ioctl(g_hostreader, WRITE_BASE_REG, tmp);
    for (i=0;i<50;i++);

}

//========================================================
//功能:对射频 设备进行初始化,  在应用启动时必须要做
//入口参数
//	                无
//出口参数
//                   无
//返回值
//       0=成功
//      -1=打开设备句柄出错
//========================================================
int rfdev_init(void)
{
	int nresult=-1;

	g_hostreader = open("/dev/rf531dev0", O_RDWR);
	if (g_hostreader > 0)
	{
		nresult = 0;
	}

	return nresult;

}

//===================================
//函数 rfdev_get_rfstatus
//功能 对天线进行初始化
//入口参数: 无
//出口参数: 无
//返回值:
//      0=没有天线
//      1=RF1天线有效
//      2=RF2天线有效
//      3=RF1和RF2天线有效
//===================================
int rfdev_get_rfstatus(unsigned char ant_to_rst)
{

	unsigned char sztmp[255];
	int result=MI_OK;

	sztmp[0] = ant_to_rst;
	result = ioctl(g_hostreader, RF_RST, sztmp);

	//switch(result)
	//{
	//case 1:
	//	antenna_switch(0);
	//	ISO14443A_Init(u_ant_type);
	//	break;
	//case 2:
	//	antenna_switch(1);:
	//	ISO14443A_Init(u_ant_type);
	//	break;
	//case 3:
	//	antenna_switch(0);
	//	ISO14443A_Init(u_ant_type);
	//	antenna_switch(1);
	//	ISO14443A_Init(u_ant_type);
	//	break;
	//}

	return result;

}
//*****************************************************************************
//* 功能：置RC500寄存器位
//*****************************************************************************
void SetBitMask(unsigned char reg, unsigned char mask)
{
    char tmp = ReadRC(reg);
    WriteRC(reg, tmp | mask);  			// set bit mask
}

//*****************************************************************************
//* 功能：清RC500寄存器位
//*****************************************************************************
void ClearBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = ReadRC(reg);
    WriteRC(reg, tmp & ~mask);  			// clear bit mask
}

void FlushFIFO(void)
{
    SetBitMask(PHHAL_HW_RC663_REG_FIFOCONTROL,0x10);
}

void PcdSetTmo(uint tmoLength)
{
    uint cClock0,cClock1;

    switch (tmoLength)
    {
    case 0:
        cClock0 = 0x00; //
        cClock1 = 0x00; //
        break;
    case 1:
        cClock0 = 0x00; //
        cClock1 = 0x00; //
        break;
    case 2:
        cClock0 = 0x0894;	  //	 161.94us
        cClock1 = 0x00; //
        break;
    case 3:
        cClock0 = 0x35D8; //	  166us
        cClock1 = 0x00; //
        break;
    case 4:
        cClock0 = 0x18D8; //	  166us
        cClock1 = 0x00; //
        break;
    case 5:
        cClock0 = 0xFFFF; //	  4.83ms
        cClock1 = 0x0000; //
        break;
    case 6:
        cClock0 = 0x7FFF; //
        cClock1 = 0x0000; //
        break;
    default:
        cClock0 = 0x00;
        cClock1 = 0x00;

        break;
    }

    WriteRC(PHHAL_HW_RC663_REG_T0RELOADHI,(uchar)(cClock0>>8));
    WriteRC(PHHAL_HW_RC663_REG_T0RELOADLO,(uchar)cClock0);
    WriteRC(PHHAL_HW_RC663_REG_T1RELOADHI,(uchar)(cClock1>>8));
    WriteRC(PHHAL_HW_RC663_REG_T1RELOADLO,(uchar)cClock1);
    WriteRC(PHHAL_HW_RC663_REG_TCONTROL,0x33);	 //


}

//======================================================================
//Mifare One 操作指令集
//
//======================================================================
#ifndef _MIFAREONE_OPT_
#define _MIFAREONE_OPT_
#endif

char ISO14443A_Authentication( unsigned char auth_mode, unsigned char *snr, unsigned char *keys, unsigned char block )
{
    unsigned char data[16];
    char status=0x01;

    data[0] =  auth_mode;
    data[1] = block;
    memcpy(&data[1+1],keys,6);
    memcpy(&data[1+1+6],snr,4);
    status = ioctl(g_hostreader, ISO14443A_AUT, data);
    return status;
}


int antenna_switch(int idx)
{
    unsigned char data;
    char status=MI_OK;
    if (idx!=g_currentreader)
    {
        g_currentreader=idx;
    }
    data = g_currentreader;
    status = ioctl(g_hostreader, RF_SET, &data);
    return status;
}


//char ISO14443A_Init(void)
char ISO14443A_Init(unsigned char val)
{
    char status=MI_OK;
    uchar data;

		data = val;
    status = ioctl(g_hostreader, ISO14443A_INIT, &data);

    return status;

}

char ISO14443A_Request(unsigned char req_code, unsigned char *atq)
{
    char status;
    unsigned char data[5];
	for(int i = 0;i<3;i++)
	{
		data[0] =  req_code;
		status = ioctl(g_hostreader, ISO14443A_REQ, data);
		//g_Record.log_out_debug(0, level_error,"ISO14443A_Request:%d,%02X",i,status);
		if (status == 0x00)//寻卡成功，直接退出
		{
			memcpy(atq,data,2);
			break;
		}
		//usleep(50 * 1000);
	}

    return status;

}

char  ISO14443A_Anticoll(unsigned char *cPsnr )
{
    char status = MI_OK;
    unsigned char data[5];
	for(int i = 0;i<3;i++)
	{
		status = ioctl(g_hostreader, ISO14443A_ANT, data);
		//g_Record.log_out_debug(0, level_error,"ISO14443A_Anticoll:%d,%02X",i,status);
		if ((status == 0x00))
		{
			memcpy(cPsnr,data,4);
			break;
		}else if(status == MI_COLLERR){
			break;
		}
	}
    return status;
}

char ISO14443A_Select( unsigned char *cPsnr, unsigned char *_size )
{
    char status = MI_OK;
    unsigned char data[5];

    memcpy(data,cPsnr,4);

	for(int i = 0;i<3;i++)
	{
		status = ioctl(g_hostreader, ISO14443A_SEL, data);
		//g_Record.log_out_debug(0, level_error,"ISO14443A_Select:%d,%02X",i,status);
		if (status == 0x00)
		{
			*_size = data[0];
			break;
		}
	}

    return status;
}

char ISO14443A_ReadBlock(unsigned char addr, unsigned char *readdata)
{
    char status = MI_OK;
    unsigned char data[20];

    data[0] = addr;
    status = ioctl(g_hostreader, ISO14443A_RED, data);
    if (status==0x00)
        memcpy(readdata,data,16);

    return status;
}

char ISO14443A_WriteBlock(unsigned char addr, unsigned char *writedata)
{
    char status = MI_OK;
    unsigned char data[20];

    data[0] = addr;
    memcpy(&data[1],writedata,16);
    status = ioctl(g_hostreader, ISO14443A_WRE, data);

    return status;

}


char ISO14443A_Restore(unsigned char addr)
{
    char status = MI_OK;
    unsigned char data[10];

    data[0] = addr;
    status = ioctl(g_hostreader, ISO14443A_RES, data);

    return status;
}

char ISO14443A_Value( unsigned char dd_mode,unsigned char addr, unsigned char* value )
{
    char status = MI_OK;
    unsigned char data[10];

    data[0] = dd_mode;
    data[1] = addr;
    memcpy(&data[2],value,4);
    status = ioctl(g_hostreader, ISO14443A_VAL, data);

    return status;
}


char ISO14443A_Transfer(unsigned char addr)
{
    char status = MI_OK;
    unsigned char data[5];

    data[0] = addr;
    status = ioctl(g_hostreader, ISO14443A_TRA, data);

    return status;
}

char ISO14443A_Halt(void)
{
    char status = MI_OK;
    unsigned char data = 0;

    status = ioctl(g_hostreader, ISO14443A_HAL, data);

    return status;
}

//==========UL==========
char ISO14443A_Request_UL(unsigned char req_code, unsigned char *atq)
{
    char status;
    unsigned char data[32];

    data[0] =  req_code;
	
	for(int i = 0;i<3;i++)
	{
		status = ioctl(g_hostreader, ISO14443A_REQA_UL, data);
		//g_Record.log_out_debug(0, level_error,"ISO14443A_Request_UL:%d,%02X",i,status);
		if (status == 0x00)
		{
			memcpy(atq,data,2);
			break;
		}
	}

    return status;

}

char ISO14443A_Anticoll_UL( unsigned char ucode, unsigned char *cPsnr )
{

    char status = MI_OK;
    unsigned char data[32];

    data[0]= ucode;

	for(int i = 0;i<3;i++)
	{
		status = ioctl(g_hostreader, ISO14443A_ANT_UL, data);
		//g_Record.log_out_debug(0, level_error,"ISO14443A_Anticoll_UL:%d,%02X",i,status);
		if (status == 0x00)
		{
			memcpy(cPsnr,data,5);
			break;
		}else if(status == MI_COLLERR){
			break;
		}
	}

	return status;
}

char ISO14443A_Select_UL(unsigned char ucode, unsigned char *cPsnr, unsigned char *_size)
{
    char status = MI_OK;
    unsigned char data[32];


    data[0] = ucode;
    memcpy(&data[1],cPsnr,5);

	for(int i = 0;i<3;i++)
	{
		status = ioctl(g_hostreader, ISO14443A_SEL_UL, data);
		//g_Record.log_out_debug(0, level_error,"ISO14443A_Select_UL:%d,%02X",i,status);
		if (status==0x00)
		{
			*_size = data[0];
			break;
		}
	}
    return status;
}

char ISO14443A_Read_UL(unsigned char addr,unsigned char *readdata)
{
    char status = MI_OK;
    unsigned char data[32];

    data[0] = addr;

    status = ioctl(g_hostreader, ISO14443A_RED_UL, data);
    if (status == 0x00)
        memcpy(readdata,data,16);

    return status;
}

char ISO14443A_Write_UL(unsigned char addr, unsigned char *writedata)
{
    char status = MI_OK;
    unsigned char data[10];

	memset(data,0x00,10);

    data[0] = addr;
    memcpy(&data[1],writedata,4);
    status = ioctl(g_hostreader, ISO14443A_WRE_UL, data);

    return status;

}


char ISO14443A_CWrite_UL(unsigned char addr, unsigned char *writedata)
{
    char status = MI_OK;
    unsigned char data[20];

    data[0] = addr;
    memcpy(&data[1],writedata,16);
    status = ioctl(g_hostreader, ISO14443A_CWRE_UL, data);

    return status;

}


//=====mifarepro=====
unsigned char MifareProRst(unsigned char cParameter,unsigned char cpLength,unsigned char *cpOutMsg)
{
    char status = MI_OK;
    unsigned char data[100];
    unsigned char len=0;

    data[0] = cParameter;
    status = ioctl(g_hostreader, MIFAREPRO_RST, data);
    if (status==0x00)
    {
        len = data[0];
        //*cpLength = data[0];
        memcpy(cpOutMsg,&data[1],len);
    }

    return len;
}


unsigned char MifareProCom(unsigned char cpLength,unsigned char *lpdata, unsigned short int *respone_sw)
{
    char status = MI_OK;
    unsigned char buf[512];
    unsigned char len;
	unsigned short int sw=0xFFFF;
	//int i;
	//char sztmp[500];

	//dbg_dumpmemory("lpdata=",lpdata,cpLength);

	*respone_sw=sw;

	memset(buf,0x39,sizeof(buf));
    buf[0] = cpLength;
    len = cpLength;
    memcpy(&buf[1],lpdata,len);

	status = ioctl(g_hostreader, MIFAREPRO_COM, buf);
    if (status==0x00)
    {
        len = buf[0];
        memcpy(lpdata,&buf[1],len);
        sw = lpdata[len-2];
		sw <<= 8;
		sw += lpdata[len-1];
		*respone_sw = sw;
		status = len;
    }
    else
    {
		//dbg_formatvar("status = %02x",status);
    	sw=0xFFFF;
    	*respone_sw = sw;
		status = 0;
    }

	//dbg_formatvar("status2 = %02x",status);

    return status;
}


char rf_version(unsigned char *version)
{
	char status=MI_OK;
	//rc531_switch(0);
	status = ioctl(g_hostreader, RF_VERSION, version);

	return status;

}

void rfdev_destory(void)
{
	if (g_hostreader > 0)
	{
		close(g_hostreader);
		g_hostreader = -1;
	}
}

