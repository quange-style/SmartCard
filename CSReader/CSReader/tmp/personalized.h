#ifndef _TOKEN_H_
#define _TOKEN_H_

#ifdef __cplusplus
extern "C"
{
#endif


//==============================================================
//函数: ticket_esanalyze
//功能: 对票卡创建文件结构
//入口参数:
//                      无
//出口参数:
//                     lpoutdata = 票卡分析数据
//返回值:
//               0=成功
//==============================================================
extern int ticket_esanalyze(void *lpoutdata);


//==============================================================
//函数: ticket_init
//功能: 对票卡创建文件结构
//入口参数:
//                       lpindata =  按照TINPUT 结构排序的数据
//出口参数:
//                     lpoutdata = 预赋值交易
//返回值:
//               0=成功
//==============================================================
extern int ticket_esinit(void *lpindata, void *lpoutdata);

#ifdef __cplusplus
}
#endif

#endif



