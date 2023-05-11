// DlgInc.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgInc.h"
#include "CSTesterDlg.h"


// CDlgInc 对话框

IMPLEMENT_DYNAMIC(CDlgInc, CDialog)

CDlgInc::CDlgInc(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInc::IDD, pParent)
	, m_strToIncr(_T(""))
	, m_strAfter(_T(""))
{

}

CDlgInc::~CDlgInc()
{
}

void CDlgInc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TOINCR, m_strToIncr);
	DDX_Text(pDX, IDC_EDIT_AFTER, m_strAfter);
}

void CDlgInc::SetOldBalance(long lOldBalance)
{
	m_lOldBalance = lOldBalance;
	CString str;
	str.Format("%ld", m_lOldBalance);
	GetDlgItem(IDC_EDIT_BEFORE)->SetWindowText(str);
	GetDlgItem(IDC_EDIT_TOINCR)->SetWindowText("0");
	GetDlgItem(IDC_EDIT_AFTER)->SetWindowText(str);
}

BEGIN_MESSAGE_MAP(CDlgInc, CDialog)
	ON_BN_CLICKED(IDC_BN_INCREASE, &CDlgInc::OnBnClickedBnIncrease)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CDlgInc::OnBnClickedBnCancel)
	ON_EN_CHANGE(IDC_EDIT_TOINCR, &CDlgInc::OnEnChangeEditToincr)
	ON_EN_CHANGE(IDC_EDIT_AFTER, &CDlgInc::OnEnChangeEditAfter)
	ON_EN_SETFOCUS(IDC_EDIT_TOINCR, &CDlgInc::OnEnSetfocusEditToincr)
	ON_EN_SETFOCUS(IDC_EDIT_AFTER, &CDlgInc::OnEnSetfocusEditAfter)
END_MESSAGE_MAP()


// CDlgInc 消息处理程序

void CDlgInc::OnBnClickedBnIncrease()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strToInc;
	long lToInc;
	GetDlgItemText(IDC_EDIT_TOINCR, strToInc);
	lToInc = atol(strToInc);

	BYTE bData[128]			= {0};
	BYTE bRecv[1024]		= {0};
	CHARGE_INIT chint		= {0};
	CHARGE_APP_ACK chapp	= {0};
	CHARGE_CONFIRM chcfm	= {0};
	CHARGE_CONFIRM_ACK chcfmack = {0};
	WORD lenchint			= 0;
	EPURSETRADE ps			= {0};
	WORD lenps				= 0;
	ETICKETDEALINFO DealData = { 0 };
	WORD lenData = 0;
	int pos = 0;
	CString ipAddr;
	uint16_t uPort;
	MpSocket sock;
	pos = 7 + sizeof(RETINFO);
	do 
	{
		GetDlgItemText(IDC_IPADDR_SERVER, ipAddr);
		uPort = (uint16_t)GetDlgItemInt(IDC_EDIT_PORT);

		g_Serial.SetTimeOut(15 * 1000);

		QueryPerformanceCounter(&g_liStart);

		// 充值初始化
		bData[0] = 0x01;
		memcpy(bData + 1,&lToInc, 4);
		memset(bData + 5, '0', 16);
		if (g_bDeviceType == 2)//tvm
			g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 24, bData, 21, bRecv, sizeof(bRecv));
		else//bom
			g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 42, bData, 21, bRecv, sizeof(bRecv));

		if (g_retInfo.wErrCode != 0)
			break;

		memcpy(&lenchint, bRecv + pos, 2);
		pos += 2;
		if (lenchint == 0)
		{
			g_retInfo.wErrCode = -3;
			break;
		}

		// 发送充值申请，接收申请数据
		memcpy(&chint, bRecv + pos, lenchint);

		g_retInfo.wErrCode = (uint16_t)sock.MpConnect(ipAddr, uPort);
		if (g_retInfo.wErrCode != 0)	break;
		g_retInfo.wErrCode = (uint16_t)sock.SendSrcWaitTarget(0x03, (LPBYTE)(&chint), lenchint, 60, 0x03, bRecv);
		sock.MpDisconnect();
		if (g_retInfo.wErrCode != 0)	break;

		memcpy(&chapp, bRecv + 5, sizeof(chapp));
		if (memcmp(chapp.cAck, "00", 2) != 0)
		{
			g_retInfo.wErrCode = 0xFFFF;
			break;
		}

		// 充值写卡，发送给读写器的数据为前置机发回的数据不要最后的四字节
		memset(bRecv, 0, sizeof(bRecv));
		memcpy(bData, &chapp, sizeof(CHARGE_APP_ACK) - 4);
		bData[77] = 0x55;

		if (g_bDeviceType == 2)
			g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 25, bData, 78, bRecv, sizeof(bRecv));
		else
			g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 43, bData, 78, bRecv, sizeof(bRecv));

		if (g_retInfo.wErrCode != 0)	break;
		pos = 7 + sizeof(RETINFO);
		memcpy(&lenps, bRecv + pos, 2);
		pos += 2;
		
		if (lenps > 0)
			memcpy(&ps, bRecv + pos, lenps);
		pos += lenps;
		
		// 充值确认
		memcpy(&chcfm, &chint, 120);
		memcpy(chcfm.cMsgType, "52", 2);
		chcfm.lBalcance = ps.lBalance;
		memcpy(chcfm.cTac, ps.cMACorTAC, sizeof(chcfm.cTac));
		chcfm.cResult = '0';
		memcpy(chcfm.cOperatorId, ps.cOperatorID, sizeof(chcfm.cOperatorId));
		chcfm.lServerSeq = chapp.lServerSeq;
		memcpy(chcfm.dtTimeOper, chapp.dtServer, sizeof(chcfm.dtTimeOper));
		g_retInfo.wErrCode = (uint16_t)sock.MpConnect(ipAddr, uPort);
		if (g_retInfo.wErrCode != 0)	break;

		g_retInfo.wErrCode = (uint16_t)sock.SendSrcWaitTarget(0x03, (LPBYTE)(&chcfm), sizeof(chcfm), 60, 0x03, bRecv);
		sock.MpDisconnect();
		if (g_retInfo.wErrCode != 0)
		{
			break;
		}

		memcpy(&chcfmack, bRecv + 5, sizeof(chcfmack));

	} while (0);

	QueryPerformanceCounter(&g_liEnd);

	g_Serial.SetTimeOut(5 * 1000);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Purse, &ps, lenps, "充值数据一", g_liStart, g_liEnd);
	
	((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);

	ShowWindow(SW_HIDE);
}

void CDlgInc::OnBnClickedBnCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
}

void CDlgInc::OnEnChangeEditToincr()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if (m_uCurEdit == IDC_EDIT_TOINCR)
	{
		UpdateData();
		int nLen = m_strToIncr.GetLength();
		m_lIncrease = atol(m_strToIncr);
		m_lNewBalance = m_lOldBalance + m_lIncrease;
		m_strAfter.Format("%ld", m_lNewBalance);

		SetDlgItemText(IDC_EDIT_AFTER, m_strAfter);
	}
}

void CDlgInc::OnEnChangeEditAfter()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if (m_uCurEdit == IDC_EDIT_AFTER)
	{
		UpdateData();
		int nLen = m_strAfter.GetLength();
		m_lNewBalance = atol(m_strAfter);
		m_lIncrease = m_lNewBalance - m_lOldBalance;
		m_strToIncr.Format("%ld", m_lIncrease);

		SetDlgItemText(IDC_EDIT_TOINCR, m_strToIncr);
	}
}

void CDlgInc::OnEnSetfocusEditToincr()
{
	// TODO: 在此添加控件通知处理程序代码
	m_uCurEdit = IDC_EDIT_TOINCR;
}

void CDlgInc::OnEnSetfocusEditAfter()
{
	// TODO: 在此添加控件通知处理程序代码
	m_uCurEdit = IDC_EDIT_AFTER;
}

BOOL CDlgInc::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//SetDlgItemText(IDC_IPADDR_SERVER, "219.136.207.188");
	//SetDlgItemText(IDC_EDIT_PORT, "8090");
	SetDlgItemText(IDC_IPADDR_SERVER, "10.99.1.40");
	SetDlgItemText(IDC_EDIT_PORT, "6001");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

