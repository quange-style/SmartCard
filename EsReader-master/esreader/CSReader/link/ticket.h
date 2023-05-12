#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <stdint.h>

typedef struct
{
	unsigned char physicalnumber[8];
	unsigned char cardtype[2];
	unsigned char testflag;
	unsigned char balance[4];
	unsigned char logicnumber[8];
	unsigned char mtrinf[48];
}TMETROINF,*LPTMETROINF;

extern unsigned char g_ant_type;

#ifdef __cplusplus
extern "C"
{
#endif


	//========================================================
	//功能:对地铁SAM  进行初始化,  一般设备上电进入工作状态前必做一次
	//入口参数
	//	 nsamsck=sam卡对应卡槽
	//出口参数
	//       saminf=6字节的sam  卡号
	//返回值
	//       0=成功
	//========================================================
	extern int sam_metro_active(int nsamsck, unsigned char * p_len_sam_id, unsigned char * p_sam_id, unsigned char * p_len_terminal_id, unsigned char * p_terminal_id);

	//========================================================
	//功能:读单程票数据
	//入口参数
	//	 nsamsck=sam卡对应卡槽
	//出口参数
	//       lpbuffer=解码后的票卡数据，从第4页到第15页(48BYTE)
	//返回值
	//       0=读卡成功, 且两个处理区crc  都正确
	//       1=读卡成功, 第一个处理区crc  错误
	//       2=读卡成功, 第二个处理区crc  错误
	//       3=读卡成功, 两个处理区crc  都错误
	//      -1=无卡
	//      -2=读数据失败
	//      -5=操作sam  卡失败
	//========================================================
	extern int readul(int nsamsck, unsigned char *lpbuffer);

	//========================================================
	//功能:写单程票数据
	//入口参数
	//        nsamsck=sam卡对应卡槽
	//        nzone=数据区域，0-处理数据区0，1-处理数据区0
	//        lpbuffer=16字节处理区数据，
	//出口参数
	//       无
	//返回值
	//       0=成功
	//      -3=写卡失败
	//========================================================
	extern int writeul(int nsamsck, int nzone, unsigned char *lpbuffer);
	extern uint16_t writeul_new(int nsamsck, int nzone, unsigned char *lpbuffer);
	extern uint16_t add_ul_crc8(unsigned char data_point, unsigned char * p_area_data);

	//========================================================
	//功能:对地铁SAM  进行功能激活，只有激活过后的才能进行发售延期解锁
	//入口参数
	//	 nsamsck=sam卡对应卡槽
	//    direct=数据方向, 0=表示取激活信息，此信息需发送给服务器; 1=表示激活操作，将服务器返回信息发送到SAM  卡
	//    lpinf=当direct=1时为输入 参数
	//出口参数
	//       无
	//返回值
	//       0=成功
	//========================================================
	extern int sam_metro_function_active(int nsamsck, int direct, unsigned char *lpinf);

	//========================================================
	//功能:选择应用目录
	//入口参数
	//	 aid=应用目录标识, 0x3F00=主应用目录,  0x1001=ADF1目录, 0x1002=ADF2目录
	//出口参数
	//       无
	//返回值
	//       >=0成功
	//========================================================
	int svt_selectfile(unsigned short aid);


	//========================================================
	//功能: 读二进制文件,  要先进入相应的应用目录才能正确读取
	//入口参数
	//	 aid=短文件标识, 0x05=发行文件,  0x11=应用控制文件，0x15=公共信息文件,  0x16=持卡人个人信息文件
	//    start_offset:读取文件的起始偏移地址
	//    len:要读取的长度
	//出口参数
	//       lpdata: 读取的数据缓冲
	//返回值
	//       >=0成功
	//========================================================
	int svt_readbinary(unsigned char aid, unsigned char start_offset, unsigned char len, unsigned char *lpdata);

	//========================================================
	//功能: 读记录文件,  要先进入相应的应用目录才能正确读取
	//入口参数
	//	 aid=短文件标识, 0x17=复合文件，0x18=历史明细
	//    record_no:要读取文件记录号
	//    len:要读取的长度
	//出口参数
	//       lpdata: 读取的数据缓冲
	//返回值
	//       >=0成功
	//========================================================
	int svt_readrecord(unsigned char aid, unsigned char record_no, unsigned char len, unsigned char *lpdata);

	//========================================================
	//功能: 读历史明细,  要先进入相应的应用目录才能正确读取
	//入口参数
	//	 aid=短文件标识, 0x17=复合文件，0x18=历史明细
	//    record_start: 要读取起始记录号
	//      record_cnt:要读取的记录条数
	//出口参数
	//       lpdata: 读取的数据缓冲
	//返回值
	//       >=0成功
	//========================================================
	int svt_readhistory(unsigned char aid, unsigned char record_start, unsigned char record_cnt, unsigned char *lpdata);


	//========================================================
	//功能: 读钱包值,  要先进入相应的应用目录才能正确读取
	//入口参数
	//	 无
	//出口参数
	//       lpdata: 4字节的钱包值,  高字节在前
	//返回值
	//       >=0成功
	//========================================================
	int svt_getbalance(unsigned char *lpdata);

	//========================================================
	//功能: 取联机交易计数
	//入口参数
	//	 nsamsck: 地铁SAM  卡对应的卡座,  从0  开始
	//出口参数
	//       lpdata: 2字节的联机序列号,  高字节在前
	//返回值
	//       >=0成功
	//========================================================
	int svt_getonlineserial(int nsamsck,unsigned char *lpserial);

	//========================================================
	//功能: 取脱机交易计数
	//入口参数
	//	 nsamsck: 地铁SAM  卡对应的卡座,  从0  开始
	//出口参数
	//       lpdata: 2字节的胶机序列号,  高字节在前
	//返回值
	//       >=0成功
	//========================================================
	int svt_getofflineserial(int nsamsck,unsigned char *lpserial);



	//=================================================================================
	//函 数 名: svt_read
	//功    能: 读取地铁票的数据内容
	//入口参数:
	//          nmode:读卡方式,=0普通方式, =1全读方式，需要读取持卡人信息和所有历史
	//出口参数:
	//          lpcardinf:返回的数据缓冲区
	//          当nmode=0时
	//            发行基本信息:40Byte
	//            公共信息:30Byte
	//            交易辅助信息:48Byte
	//            地铁信息:48Byte
	//            应用控制信息:32Byte
	//            钱包值:4Byte
	//            最近一次历史记录:23Byte
	//          当nmode=1时
	//            发行基本信息:40Byte
	//            公共信息:30Byte
	//            交易辅助信息:48Byte
	//            地铁信息:48Byte
	//            应用控制信息:32Byte
	//            钱包值:4Byte
	//            持卡人个人信息:23Byte
	//            10条历史记录:230Byte
	//返 回 值:
	//        >0: 返回的数据长度
	//        <0: 操作出错
	//=================================================================================
	extern int svt_read(int opertion, unsigned char *lpcardinf);

	//========================================================
	//功能:对地铁svt 卡进行充值
	//入口参数
	//	 nsamsck=sam卡对应卡槽
	//    nstep=当=0时表示获取充值信息，=1时表示对票卡进行充值操作
	//    value=要充到票卡内的金额(  分)
	//    lpinf =当nstep=1时为输入数据
	//            输入数据结构:
	//                       交易时间7Byte
	//                        MAC2   4Byte
	//
	//出口参数
	//       pinf = 输出数据
	//             输出数据结构:
	//                               余额4Byte,
	//                       联机交易序号2Byte,
	//                           密钥版本1Byte,
	//                           算法标识1Byte,
	//                           伪随机数4Byte,
	//                             MAC1  4Byte

	//返回值
	//       0=成功
	//========================================================
	extern int svt_recharge(unsigned char trType, unsigned char * p_timenow, int nsamsck, int nstep, unsigned long value, unsigned char *lpinf);

	//========================================================
	//功能:对地铁票进行脱机消费
	//入口参数
	//    nsamsck=sam卡对应卡槽
	//    value = 消费金额
	//    trtype=交易类型, 06=普通消费,09=复合消费
	//    lpmetrofile=当trtype=09  时，作为复合文件更新的数据内容，普通消费时不使用
	//出口参数
	//       respone_data=SAM终端号(6BYTE) +  SAM终端流水( 4Byte) + TAC (4Byte)
	//返回值
	//       0=成功
	//========================================================
	extern int svt_consum(unsigned char * p_timenow, int nsamsck, unsigned char *lplogicnumber,unsigned long value,unsigned char trtype, unsigned char *lpmetrofile, unsigned char *respone_data);

	//===============================================================
	//功能:对SVT票的应用控制文件进行更新
	//入口参数
	//	 nsamsck=sam卡对应卡槽
	//       file_id=短文件标识，目前只支持0x11(应用控制文件)和0x16(个人信息文件)
	//        offset=要更新文件的起始地址
	//           nle=要更新的数据长度
	//         lpinf=要写入的数据

	//
	//出口参数
	//	无
	//返回值
	//       0=成功
	//===============================================================
	extern int svt_update_binfile(int nsamsck,unsigned char *lplogicnumber,int file_id, int offset, int nlen, unsigned char *lpinf);

	//========================================================
	//功能:搜寻天线感应区票卡
	//入口参数
	//	 无
	//出口参数
	//       lpcardno=卡号
	//返回值
	//       0=无卡
	//       1=UL
	//       2=M1卡
	//       3=CPU卡
	//      99=多卡
	//========================================================
	extern int searchcard(unsigned char *lpcardno);
	extern unsigned short search_card_ex(unsigned char *lpcardno, unsigned char& type);
	extern int searchcard_es(unsigned char *lpcardno);

	//=================================
	//功能: CPU 卡复位,  此前必须做过searchcard函数
	//入口参数:  无
	//出口参数:
	//                         lpinf: 复位信息
	//返回值:
	//                =0成功
	//=================================
	extern int svt_active(unsigned char *lpinf);

	//========================================================
	//功能:对地铁票指定的记录进行更新
	//入口参数
	//    nsamsck=sam卡对应卡槽
	//    lplogicnumber=要更新票卡的8字节逻辑卡号(应用序列号)
	//    record_id=文档中对应的记录号
	//    lock=锁定标识,=0不锁定
	//    lpfile=复合文件更新的数据内容
	//出口参数
	//       respone_data=SAM终端号(6BYTE) +  SAM终端流水( 4Byte) + TAC (4Byte)
	//返回值
	//       0=成功
	//========================================================
	extern int svt_updata_complex(unsigned char * p_timenow, int nsamsck, unsigned char *lplogicnumber, unsigned char record_id, unsigned char lock, unsigned char *lpfile, unsigned char *respone_data);


	//===============================================================
	//功能:对SVT票的复合文件指定记录进行更新
	//入口参数
	//	 nsamsck=sam卡对应卡槽
	//  lplogicnumber=票卡逻辑卡号
	//        record_id=记录ID
	//           lock=锁定标记
	//         lpinf=复合文件对应要写入的数据(要与读取时获得的相同)

	//
	//出口参数
	//	无
	//返回值
	//       0=成功
	//===============================================================
	extern int svt_unlock_recordfile(int nsamsck,unsigned char *lplogicnumber,int record_id, unsigned char lock, unsigned char *lpinf);

	//========================================================
	//功能:对地铁票指定的记录进行更新
	//入口参数
	//    nsamsck=sam卡对应卡槽
	//    lplogicnumber=要更新票卡的8字节逻辑卡号(应用序列号)
	//    recordcnt=要更新的记录总数
	//    dim_record_id=要更新的多条记录号数组
	//    dim_lock=对应多条记录的锁定标识数组,=0不锁定
	//    dim_lpfile=复合文件更新的多条记录的数据内容，赋值时需要注意不同的记录号对应的数据长度( 数据内容为不含记录前三个特定数据的内容)
	//出口参数
	//       respone_data=SAM终端号(6BYTE) +  SAM终端流水( 4Byte) + TAC (4Byte)
	//返回值
	//       0=成功
	//========================================================
	extern uint16_t svt_updata_more_complex(unsigned char tac_type, unsigned char * p_time_now, int nsamsck, 
		unsigned char *lplogicnumber, long lAmount, unsigned char recordcnt,
		unsigned char *dim_record_id, unsigned char *dim_lock, unsigned char *dim_lpfile, unsigned char *respone_data);

	typedef struct
	{
		unsigned char	id;
		unsigned char	lock_flag;
		unsigned char	len;
		unsigned char	data[255];
	}COMPLEX_FILE, * PCOMPLEX_FILE;

	extern uint16_t updata_more_complex(unsigned char * p_time_now, int nsamsck, unsigned char *lplogicnumber, long lAmount, unsigned char recordcnt,
		PCOMPLEX_FILE complex_arr, unsigned char *respone_data);

	//========================================================
	//功能:对一卡通票进行脱机消费
	//入口参数
	//	 nsamsck=sam卡对应卡槽
	//    value = 消费金额
	//    trtype=交易类型, 06=普通消费,09=复合消费
	//    lpmetrofile=当trtype=09  时，作为复合文件更新的数据内容，普通消费时不使用
	//出口参数
	//       respone_data=SAM终端号(6BYTE) +  SAM终端流水( 4Byte) + TAC (4Byte)
	//返回值
	//       0=成功
	//========================================================
	extern int oct_consum(unsigned char * p_timenow, int nsamsck, unsigned char *lplogicnumber, unsigned long value,unsigned char trtype, unsigned char *lpmetrofile, unsigned char *respone_data);

	//========================================================
	//功能：区分CPU卡是地铁CPU还是公交CPU
	//========================================================
	extern int distinguish_cpu(unsigned char * p_pid);
	extern unsigned short distinguish_cpu_ex(unsigned char * p_pid, unsigned char& type);

	//========================================================
	//功能:对OCT加解锁
	//入口参数
	//   nsamsck=sam卡对应卡槽
	//    lplogicnumber = 票卡应用序列号
	//    lpcitycode=城市代码
	//    optflag=操作类型(1-加锁,0-解锁)
	//出口参数
	//       无
	//返回值
	//       0=成功
	//注:黑卡判断
	//在3F00应用目录发送00B203D400，前两字节为0x53,0x4B则为已锁定黑卡
	//========================================================
	extern int oct_cpu_lock(int nsamsck, unsigned char *lplogicnumber, unsigned char *lpcitycode, unsigned char optflag);

	//=============================================================================================================
	//函 数 名: generate_tac
	//功    能: 计算单程票TAC码
	//入口参数:
	//          nsamsck: sam卡对应卡槽
	//   *lplogicnumber: 8byte逻辑卡号
	//            money: 4byte交易金额,long型
	//   trade_datetime: 7byte交易时间
	//出口参数:
	//        lpresult:计算结果(samid(6B)+dsn(4B)+tac(4B))，当返回值=0时有效
	//返回值:
	//       =0成功
	//=============================================================================================================
	extern int generate_tac(int nsamsck, unsigned char *lplogicnumber, unsigned long money, unsigned char *trade_datetime, unsigned char *lpresult);

	//========================================================
	//功能:通过aid 选择应用目录
	//入口参数
	//	 aid=应用目录标识, 
	//出口参数
	//       无
	//返回值
	//       >=0成功
	//========================================================
	extern int svt_selecfileaid(unsigned char ulen, unsigned char *lpaid);

	//========================================================
	//功能:获取CPU卡上次交易的TAC码
	//入口参数
	//	 aid=应用目录标识, 
	//出口参数
	//       无
	//返回值
	//       =0成功
	//========================================================
	extern int get_cpu_last_tac(uint16_t off_line_counter, uint8_t tac_type, unsigned char * p_recv);

	//========================================================
	//功能:CPU卡pin校验
	//出口参数
	//       无
	//返回值
	//       =0成功
	//========================================================
	extern int mtr_cpu_pin_check();

	//========================================================
	//功能:初始化SAM卡全局相关信息
	//出口参数
	//       无
	//返回值
	//========================================================
	extern void init_sam_inf();

#ifdef __cplusplus
}
#endif


#endif

