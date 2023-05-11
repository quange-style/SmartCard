//extern unsigned char YCT_M1_Trade(unsigned char operation, unsigned char *inputData, unsigned char *prev);

extern unsigned char g_lstphysical[8];

#ifdef __cplusplus
extern "C"
{
#endif
//==========================================================================================
//函 数 名: octm1_saminit
//功    能: 对公交M1卡对应的SAM卡进行初始化 ，在读写器上电时执行一次即可
//入口参数:
//          idx_sam:当前应用SAM卡对应的卡槽序列号(从0开始)
//出口参数:
//          lpsamcode:6字节终端机编号/sam卡号
//返 回 值:
//          0=成功
//==========================================================================================
extern unsigned char octm1_saminit(unsigned char idx_sam, unsigned char *lpsamcode);

//==========================================================================================
//函 数 名: octm1_getdata
//功    能: 获取m1卡关键数据
//入口参数:
//          idx_sam:当前应用SAM卡对应的卡槽序列号(从0开始)
//     lpcardnumber:寻卡过程中获取的4字节的物理卡号
//出口参数:
//          lprev:取得的数据内容,内容如下
//					//==================================================
//					    unsigned char szbalance[4];
//					//==================================================
//					    unsigned char type_major;//卡片主类型 50消费用卡
//					    unsigned char type_minjor;//应用子类型
//					                              // 01：普通钱包卡
//					                              // A3：成人卡
//					                              // A4：学生卡
//					                              // A5：老年人票卡
//					                              // A6：残疾人票卡
//					                              // A7：公务员票卡
//					                              // A8：家属学生卡
//					    unsigned char appserial[4]; //应用卡编号 bcd
//					    unsigned char sale_dt[5];//发售时间 bcd, yymmddhhnn
//					    unsigned char sale_addr[2]; //发售地点编号 bcd,
//					    unsigned char deposit;//押金,元
//					    unsigned char appflag;//应用标识,AA：未启用；BB：启用；
//					//===================
//					    unsigned char verify_dt[3]; //审核情况 yymmdd
//					    unsigned char pursemode; //卡内储值情况
//					                             //0x00：卡内专用钱包、公共钱包均未启用；
//					                             //0x01：专用钱包已启用；
//					                             //0x02：公共钱包已启用；
//					                             //0x03：专用钱包、公共钱包均已启用
//					    unsigned char city[2];  //城市代码
//					    unsigned char industry[2]; //行业代码
//					//=================================================
//					    unsigned char common_count[2]; //公共钱包交易次数,高位在前
//					    unsigned char other_count[2];  //专用钱包交易次数,高位在前
//					    unsigned char dt_trdate[4]; //交易日期yyyy-mm-dd
//					    unsigned char lock_flag; //04锁卡，其它正常
//					//=================================================
//					    unsigned char tr_type;//交易类型
//					    unsigned char servicer_code[2];//服务商代码
//					    unsigned char terminal_no[4];   //POS编号 4BYTE;
//					    unsigned char dt_trdate2[3]; //过程历史交易日期yy-mm-dd
//					    unsigned char banlance[3];  //交易前余额
//					    unsigned char tr_amount[2];    //交易金额
//					//==================================================
//						unsigned char metro_inf[14]; //不需要CRC16;


//返 回 值:
//          0=成功
//==========================================================================================
//extern unsigned char octm1_getdata(unsigned char idx_sam, unsigned char *lpcardnumber, unsigned char *lprev);

//==========================================================================================
//函 数 名: octm1_getdata2
//功    能: 获取m1卡关键数据
//入口参数:
//          idx_sam:当前应用SAM卡对应的卡槽序列号(从0开始)
//     lpcardnumber:寻卡过程中获取的4字节的物理卡号
//出口参数:
//          lprev:取得的数据内容,内容如下
//					//==================================================
//					    unsigned char szbalance[4];   // 公共钱包
//					    unsigned char szbalance2[4]; // 专用钱包
//					//==================================================
//					    unsigned char type_major;//卡片主类型 50消费用卡
//					    unsigned char type_minjor;//应用子类型
//					                              // 01：普通钱包卡
//					                              // A3：成人卡
//					                              // A4：学生卡
//					                              // A5：老年人票卡
//					                              // A6：残疾人票卡
//					                              // A7：公务员票卡
//					                              // A8：家属学生卡
//					    unsigned char appserial[4]; //应用卡编号 bcd
//					    unsigned char sale_dt[5];//发售时间 bcd, yymmddhhnn
//					    unsigned char sale_addr[2]; //发售地点编号 bcd,
//					    unsigned char deposit;//押金,元
//					    unsigned char appflag;//应用标识,AA：未启用；BB：启用；=02走CPU卡流程
//					//===================
//					    unsigned char verify_dt[3]; //审核情况 yymmdd
//					    unsigned char pursemode; //卡内储值情况
//					                             //0x00：卡内专用钱包、公共钱包均未启用；
//					                             //0x01：专用钱包已启用；
//					                             //0x02：公共钱包已启用；
//					                             //0x03：专用钱包、公共钱包均已启用
//					    unsigned char city[2];  //城市代码
//					    unsigned char industry[2]; //行业代码
//					//=================================================
//					    unsigned char common_count[2]; //公共钱包交易次数,高位在前
//					    unsigned char other_count[2];  //专用钱包交易次数,高位在前
//					    unsigned char dt_trdate[4]; //交易日期yyyy-mm-dd
//					    unsigned char lock_flag; //04锁卡，其它正常
//					//=================================================
//					    unsigned char tr_type;//交易类型
//					    unsigned char servicer_code[2];//服务商代码
//					    unsigned char terminal_no[4];   //POS编号 4BYTE;
//					    unsigned char dt_trdate2[3]; //过程历史交易日期yy-mm-dd
//					    unsigned char banlance[3];  //交易前余额
//					    unsigned char tr_amount[2];    //交易金额
//					//==================================================
//						unsigned char metro_inf[14]; //不需要CRC16;


//返 回 值:
//          0=成功
//==========================================================================================
unsigned char octm1_getdata2(unsigned char idx_sam, unsigned char *lpcardnumber, unsigned char *lprev);


//==========================================================================================
//函 数 名: octm1_writedata
//功    能: 对M1卡进行写操作
//入口参数:
//            idx_sam:当前应用SAM卡对应的卡槽序列号(从0开始)
//          nopertion:操作方式, 0=黑名单解锁,1=黑名单锁卡(不写地铁信息区也不扣费，只写黑名单标记), 2=只更新地铁信息区，=3只扣费不操作地铁信息区, =4扣费的同时写地铁信息区,
//          tr_dttime:消费时间，只有在要扣值时才使用, bcd码, yyyymmddhhnnss
//             lvalue:要消费的值
//          metro_inf:地铁信息块 16byte
//
//出口参数:
//          无
//返 回 值:
//          0=成功
//       0xEE=写卡失败，需要时行交易确认
//       其它=写卡失败，无需处理
//==========================================================================================
//extern unsigned char octm1_writedata(unsigned char idx_sam, unsigned char nopertion, unsigned char *tr_dttime, unsigned long lvalue, unsigned char *metro_inf);

//==========================================================================================
//函 数 名: octm1_writedata2
//功    能: 对M1卡进行写操作
//入口参数:
//            idx_sam:当前应用SAM卡对应的卡槽序列号(从0开始)
//          nopertion:操作方式, 0=黑名单解锁,1=黑名单锁卡(不写地铁信息区也不扣费，只写黑名单标记), 2=只更新地铁信息区，=3只扣费不操作地铁信息区, =4扣费的同时写地铁信息区,
//          tr_dttime:消费时间，只有在要扣值时才使用, bcd码, yyyymmddhhnnss
//       npursetype:要处理的钱包类别, 0=公用钱包, 1或非0值=专用钱包
//             lvalue:要消费的值
//          metro_inf:地铁信息块 16byte
//
//出口参数:
//          无
//返 回 值:
//          0=成功
//       0xEE=写卡失败，需要时行交易确认
//       其它=写卡失败，无需处理
//==========================================================================================
unsigned char octm1_writedata2(unsigned char idx_sam, unsigned char nopertion, unsigned char *tr_dttime, unsigned char npursetype, unsigned long lvalue, unsigned char *metro_inf);


//==========================================================================================
//函 数 名: octm1_gettac
//功    能: 生成TAC码
//入口参数:
//          idx_sam:当前应用SAM卡对应的卡槽序列号(从0开始)
//      lpappnumber:当前卡片的应用卡编号, 在octm1_getdata函数中已获取
//           lpdata:要参与TAC计算的数据内容 16byte
//       
//出口参数:
//             lptac:要写入的地铁信息块 4byte
//返 回 值:
//          0=成功
//==========================================================================================
extern unsigned char octm1_gettac(unsigned char idx_sam, unsigned char *lpappnumber, unsigned char *lpdata, unsigned char *lptac);

//==========================================================================================
//函 数 名: octm1_judge
//功    能: 长沙公交卡判断,当m1卡的选卡select返回值sak为CPU卡时，要对此卡判断是否长沙的CPU仿M1卡,
//          如果能不读取M1数据块则认为是纯CPU卡，函数返回，并在函数调用后继续判断是什么行业的CPU卡;
//          如果能读取M1数据块则为公交cpu仿m1,再对m1卡的发行区启用标志进行判断,=0x01/0xBB，则以公交M1卡的
//          流程进行处理,如果=2则直接走公交CPU卡流程，如为其它标志则函数返回，并在函数调用后继续判断是什么行业的CPU卡;
//入口参数:
//      lpcardnumber:票卡的物理卡号, 在searchcard函数中已获取
//       
//出口参数:
//             无
//返 回 值:
//          0=无卡
//		    2=后续按公交m1流程处理
//          4=后续按公交cpu卡流程处理
//          3=后续需要继续判断是哪个行业的CPU卡
//===========================================================================
unsigned char octm1_judge(unsigned char *lpcardnumber);
unsigned short octm1_judge_ex(unsigned char *lpcardnumber, unsigned char& type);



#ifdef __cplusplus
};
#endif
