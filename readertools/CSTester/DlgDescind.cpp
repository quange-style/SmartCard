// DlgDescind.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgDescind.h"
#include "CSTesterDlg.h"


// CDlgDescind 对话框

IMPLEMENT_DYNAMIC(CDlgDescind, CDialog)

CDlgDescind::CDlgDescind(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDescind::IDD, pParent)
{

}

CDlgDescind::~CDlgDescind()
{
}

void CDlgDescind::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDescind, CDialog)
	ON_BN_CLICKED(IDC_BN_DESCIND, &CDlgDescind::OnBnClickedBnDescind)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CDlgDescind::OnBnClickedBnCancel)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDlgDescind 消息处理程序

void CDlgDescind::OnBnClickedBnDescind()
{
	// TODO: 在此添加控件通知处理程序代码
	char szTemp[32];
	long lToDescind, lBalance;

	GetDlgItemText(IDC_EDIT_AMOUNT, szTemp, 31);
	lToDescind = atol(szTemp);

	GetDlgItemText(IDC_EDIT_BALANCE, szTemp, 31);
	lBalance = atol(szTemp);

	BYTE bData[128]		= {0};
	BYTE bRecv[1024]	= {0};
	DESCIND_INIT dcint	= {0};
	DESCIND_APP_ACK dsapp= {0};
	WORD lendcint		= 0;
	EPURSETRADE ps		= {0};
	WORD lenps			= 0;
	CString ipAddr;
	uint16_t uPort = 0;
	MpSocket sock;
	int pos = 7 + sizeof(RETINFO);

	do 
	{
		GetDlgItemText(IDC_IPADDR_SERVER, ipAddr);
		uPort = (uint16_t)GetDlgItemInt(IDC_EDIT_PORT);

		QueryPerformanceCounter(&g_liStart);
		bData[0] = 0x01;
		memcpy(bData + 1,&lToDescind, 4);
		memcpy(bData + 5,&lBalance, 4);
		memset(bData + 9, '0', 16);

		GetDlgItemText(IDC_EDIT_LGCID, szTemp, 31);
		memcpy(bData + 25, szTemp, 20);

		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 46, bData, 45, bRecv, sizeof(bRecv));
		if (g_retInfo.wErrCode != 0)
			break;

		memcpy(&lendcint, bRecv + pos, 2);
		pos += 2;
		if (lendcint == 0)
		{
			g_retInfo.wErrCode = -3;
			break;
		}
		
		memcpy(&dcint, bRecv + pos, lendcint);

		g_retInfo.wErrCode = (uint16_t)sock.MpConnect(ipAddr, uPort);
		if (g_retInfo.wErrCode != 0)	break;
		g_retInfo.wErrCode = (uint16_t)sock.SendSrcWaitTarget(0x03, (LPBYTE)(&dcint), lendcint, 60, 0x03, bRecv);
		sock.MpDisconnect();
		if (g_retInfo.wErrCode != 0)	break;

		memcpy(&dsapp, bRecv + 5, sizeof(dsapp));
		if (memcmp(dsapp.cAck, "00", 2) != 0)
		{
			g_retInfo.wErrCode = 0xFFFF;
			break;
		}

		// 发送给读写器的数据为前置机发回的数据不要最后的四字节
		memset(bRecv, 0, sizeof(bRecv));
		bData[0] = 0x55;
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 47, bData, 1, bRecv, sizeof(bRecv));
		
		if (g_retInfo.wErrCode != 0)		break;
		pos = 7 + sizeof(RETINFO);
		memcpy(&lenps, bRecv + pos, 2);
		pos += 2;

		if (lenps > 0)	memcpy(&ps, bRecv + pos, lenps);
		pos += lenps;
	} while (0);

	QueryPerformanceCounter(&g_liEnd);


	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Purse, &ps, lenps, "充值撤销", g_liStart, g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);

	ShowWindow(SW_HIDE);
}

void CDlgDescind::OnBnClickedBnCancel()
{
	// TODO: 在此添加控件通知处理程序代码

	ShowWindow(SW_HIDE);
}

void CDlgDescind::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	if (bShow)
	{
		//SetDlgItemText(IDC_IPADDR_SERVER, "219.136.207.188");
		//SetDlgItemText(IDC_EDIT_PORT, "8090");
		SetDlgItemText(IDC_IPADDR_SERVER, "10.99.1.40");
		SetDlgItemText(IDC_EDIT_PORT, "6001");

		char szTemp[64] = {0};
		memcpy(szTemp, g_Analyse.cLogicalID, sizeof(g_Analyse.cLogicalID));
		SetDlgItemText(IDC_EDIT_LGCID, szTemp);

		sprintf(szTemp, "%d", g_Analyse.lBalance);
		SetDlgItemText(IDC_EDIT_BALANCE, szTemp);

	}
}
