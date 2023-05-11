// DlgConfirm.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgConfirm.h"


// CDlgConfirm 对话框

IMPLEMENT_DYNAMIC(CDlgConfirm, CDialog)

CDlgConfirm::CDlgConfirm(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConfirm::IDD, pParent)
{

}

CDlgConfirm::~CDlgConfirm()
{
}

void CDlgConfirm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgConfirm, CDialog)
	ON_BN_CLICKED(IDB_CONFIRM, &CDlgConfirm::OnBnClickedConfirm)
	ON_BN_CLICKED(IDB_UNLOCK_SCREEN, &CDlgConfirm::OnBnClickedUnlockScreen)
END_MESSAGE_MAP()


// CDlgConfirm 消息处理程序

RETINFO CDlgConfirm::Confirm(BYTE operType)
{
	TRANSACTCONFIRM tc	= {0};
	WORD lentc			= 0;
	BYTE bData[128]		= {0};
	BYTE bRecv[1024]	= {0};
	int pos				= 7;
	WORD lenTrade		= 0;
	_Struct_Type type	= struct_Null;
	BYTE nTempBeep		= g_nBeep;
	CString strOper;

	g_nBeep = 1;

	do 
	{
		bData[0] = operType;

		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 35, bData, 1, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		if (g_retInfo.wErrCode != 0)
			break;
		pos += sizeof(RETINFO);

		memcpy(&lentc, bRecv + pos, 2);
		pos += 2;

		if (lentc > 0)
		{
			memcpy(&tc, bRecv + pos, lentc);
			GetConfirmTradeInf(tc.bTradeType, type, lenTrade);
		}

	} while (0);

	g_nBeep = nTempBeep;

	if (operType == 0)
		strOper = "交易确认";
	else
		strOper = "解除锁定";

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(type, tc.bData, lenTrade, strOper, g_liStart, g_liEnd);

	return g_retInfo;
}

void CDlgConfirm::GetConfirmTradeInf(BYTE btradeType, _Struct_Type& type, WORD& len_type)
{
	switch (btradeType)
	{
	case 0x50:			// 单程票发售
		type = struct_SjtSale;
		len_type = (WORD)sizeof(SJTSALE);
		break;
	case 0x51:			// 储值票发售
		type = struct_SvtSale;
		len_type = (WORD)sizeof(OTHERSALE);
		break;
	case 0x53:			// 进闸
		type = struct_Entry;
		len_type = (WORD)sizeof(ENTRYGATE);
		break;
	case 0x54:			// 钱包交易
		type = struct_Purse;
		len_type = (WORD)sizeof(PEPURSETRADE);
		break;
	case 0x55:			// 延期
		type = struct_Deffer;
		len_type = (WORD)sizeof(TICKETDEFER);
		break;
	case 0x56:			// 更新
		type = struct_Update;
		len_type = (WORD)sizeof(TICKETUPDATE);
		break;
	case 0x57:			// 退款
		type = struct_Refund;
		len_type = (WORD)sizeof(DIRECTREFUND);
		break;
	case 0x59:			// 加解锁
		type = struct_Lock;
		len_type = (WORD)sizeof(TICKETLOCK);
		break;
	}
}

void CDlgConfirm::OnBnClickedConfirm()
{
	// TODO: 在此添加控件通知处理程序代码
	RETINFO ret = Confirm(0);

	if (ret.wErrCode == 0)
		EndDialog(0);
}

void CDlgConfirm::OnBnClickedUnlockScreen()
{
	// TODO: 在此添加控件通知处理程序代码
	RETINFO ret = Confirm(1);

	if (ret.wErrCode == 0)
		EndDialog(0);
}
