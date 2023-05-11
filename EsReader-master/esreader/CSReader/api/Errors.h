// Errors.h

#pragma once

//
// 错误码
//
#define ERR_SUCCEED					0x0000
#define ERR_UNDEFINED				0x0001		// 未定义错误
#define ERR_CARD_NONE				0x0002		// 无卡
#define ERR_CARD_MORE				0x0003		// 多卡
#define ERR_CARD_READ				0x0004		// 读卡失败
#define ERR_CARD_WRITE				0x0005		// 写卡失败
#define ERR_WITHOUT_SELL			0x0006		// 未发售（ES预赋值）的车票
#define ERR_CARD_STATUS				0x0007		// 票卡状态错误
#define ERR_CARD_REFUND				0x0008		// 票卡已退款（注销）
#define ERR_CARD_LOCKED				0x0009		// 黑名单卡
#define ERR_CARD_INVALID			0x000A		// 无效票
#define ERR_OVER_PERIOD_P			0x000B		// 票卡物理有效期过期
#define ERR_OVER_PERIOD_L			0x000C		// 票卡逻辑有效期过期
#define ERR_OVER_PERIOD_A			0x000D		// 票卡激活有效期过期
#define ERR_CARD_DISABLED			0x000E		// 票卡未启用
#define ERR_OVER_SYSTEM				0x000F		// 非本系统卡

#define ERR_CARD_TYPE				0x0020		// 票卡类型未定义
#define ERR_STATION_INVALID			0x0021		// 非法的车站代码
#define ERR_DEVICE_INVALID			ERR_STATION_INVALID
#define ERR_CARD_USE				0x0022		// 非本站使用的车票
#define ERR_LACK_WALLET				0x0023		// 票卡余额不足
#define ERR_OVER_WALLET				0x0024		// 票卡余额超出上限
#define ERR_ENTRY_STATION			0x0025		// 非付费区非本站进站
#define ERR_ENTRY_TIMEOUT			0x0026		// 非付费区进站超时
#define ERR_ENTRY_EVER				0x0027		// 非付费区有进站码
#define ERR_WITHOUT_ENTRY			0x0028		// 付费区无进站码
#define ERR_OVER_MILEAGE			0x0029		// 付费区超乘
#define ERR_EXIT_TIMEOUT			0x002A		// 付费区出站超时
#define ERR_UPDATE_STATION			0x002B		// 非本站更新的车票
#define ERR_UPDATE_DAY				0x002C		// 非本日更新的车票
#define ERR_PASSAGEWAY_1			0x002D		// 普通票通道，不支持优惠票
#define ERR_PASSAGEWAY_2			0x002E		// 优惠票通道，不支持普通票
#define ERR_LAST_EXIT_NEAR			0x002F		// 已于X分钟内在本站出站
#define ERR_OVER_TIME_MILEAGE		0x0030		// 超时又超乘


#define ERR_CALL_INVALID			0x0040		// 非法的接口调用
#define ERR_DEVICE_SUPPROT			0x0041		// 设备不支持的票卡类型
#define ERR_DIFFRENT_CARD			0x0042		// 非同一张票卡
#define ERR_CALLING_INVALID			0x0043		// 票卡当前调用非法
#define ERR_INPUT_PARAM				0x0044		// 非法的传入参数
#define ERR_DEVICE_UNINIT			0x0045		// 设备未初始化
#define ERR_APP_NOT_EXSIT			0x0046		// 不存在的应用接口


#define ERR_PARAM_NOT_EXIST			0x0100		// 参数文件不存在
#define ERR_PARAM_INVALID			0x0101		// 参数文件不合法
#define ERR_FILE_ACCESS				0x0102		// 文件错误
#define ERR_INPUT_INVALID			0x0103		// 无效的输入参数
#define ERR_HANDLER_OPEN			0x0104		// 通讯句柄打开错误
#define ERR_SEND_DATA				0x0105		// 通讯发送数据失败
#define ERR_RECIVE_DATA				0x0106		// 通讯接收数据失败
#define ERR_DATA_INVALID			0x0107		// 通讯数据非法
#define ERR_SAM_RESPOND				0x0108		// SAM卡响应错误
#define ERR_OVERLAPPED				0x0109		// 操作重叠，上位机串口在读写器一个完整流程未执行完毕的情况下发送新的指令
#define ERR_RF_NONE					0x010A		// Rf模块错误
//
// ES 专有错误码
//
#define ERR_CLEAR_FILE				0xF001		// 删除旧文件错误
#define ERR_CREATE_MF				0xF002		// 创建MF失败
#define ERR_CREATE_ADF				0xF003		// 创建ADF失败

//
// 关联提示码
//
#define NTC_NONE					0x00		// 无关联提示
#define NTC_TYPE_OVER_PERIOD		0x01		// 票种过期，启用普通票
#define NTC_TOKEN_RECLAIM			0x10		// 回收票
#define NTC_MUST_CONFIRM			0x11		// 需要进行交易确认
#define NTC_EXIT_TRAIN_TROUBLE		0x12		// 列车故障模式出站
