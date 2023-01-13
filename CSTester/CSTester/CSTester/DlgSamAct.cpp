// DlgSamAct.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgSamAct.h"
#include "CSTesterDlg.h"



// CDlgSamAct 对话框

IMPLEMENT_DYNAMIC(CDlgSamAct, CDialog)

CDlgSamAct::CDlgSamAct(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSamAct::IDD, pParent)
{

}

CDlgSamAct::~CDlgSamAct()
{
}

void CDlgSamAct::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSamAct, CDialog)
	ON_BN_CLICKED(IDC_BN_INCREASE, &CDlgSamAct::OnBnClickedBnSamAct)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CDlgSamAct::OnBnClickedBnCancel)
END_MESSAGE_MAP()


// CDlgSamAct 消息处理程序

void CDlgSamAct::OnBnClickedBnSamAct()
{
	// TODO: 在此添加控件通知处理程序代码
	WORD lenact			= 0;
	SAM_ACTIVE sact		= {0};
	BYTE bData[128]		= {0};
	BYTE bRecv[1024]	= {0};
	SAM_ACTIVE_ACK sack	= {0};
	CString ipAddr;
	uint16_t uPort;
	MpSocket sock;

	do 
	{
		if (!((CCSTesterDlg *)AfxGetMainWnd())->GetSetting(g_nPortOpen, g_wStation, g_wDeviceID))
		{
			g_retInfo.wErrCode = -2;
			break;
		}

		GetDlgItemText(IDC_IPADDR_SERVER, ipAddr);
		uPort = (uint16_t)GetDlgItemInt(IDC_EDIT_PORT);

		QueryPerformanceCounter(&g_liStart);

		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 44, bData, 0, bRecv, sizeof(bRecv));
		if (g_retInfo.wErrCode != 0)
			break;

		memcpy(&lenact, bRecv + 7 + sizeof(RETINFO), 2);
		if (lenact == 0)
		{
			g_retInfo.wErrCode = -3;
			break;
		}

		memcpy(&sact, bRecv + 9 + sizeof(RETINFO), lenact);

		g_retInfo.wErrCode = (uint16_t)sock.MpConnect(ipAddr, uPort);
		if (g_retInfo.wErrCode != 0)
			break;

		g_retInfo.wErrCode = (uint16_t)sock.SendSrcWaitTarget(0x03, (LPBYTE)(&sact), sizeof(sact), 60, 0x03, bRecv);
		sock.MpDisconnect();
		if (g_retInfo.wErrCode != 0)
		{
			break;
		}

		memcpy(&sack, bRecv + 5, sizeof(sack));
		if (memcmp(sack.cAck, "00", 2) != 0)
		{
			g_retInfo.wErrCode = -4;
			break;
		}
		// 发送给读写器的数据为前置机发回的数据不要最后的四字节
		memset(bRecv, 0, sizeof(bRecv));
		memcpy(bData, &sack, sizeof(sack) - 4);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 45, bData, sizeof(sack) - 4, bRecv, sizeof(bRecv));

	} while (0);

	QueryPerformanceCounter(&g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Null, NULL, 0, "SAM卡激活", g_liStart, g_liEnd);

	if (g_retInfo.wErrCode == 0)
		ShowWindow(SW_HIDE);

}

void CDlgSamAct::OnBnClickedBnCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
}

BOOL CDlgSamAct::OnInitDialog()
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
