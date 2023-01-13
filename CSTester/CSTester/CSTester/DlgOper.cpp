// DlgOper.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgOper.h"
#include "CSTesterDlg.h"


// CDlgOper 对话框

IMPLEMENT_DYNAMIC(CDlgOper, CDialog)

CDlgOper::CDlgOper(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOper::IDD, pParent)
	, m_bForbid(FALSE)
{

}

CDlgOper::~CDlgOper()
{
}

void CDlgOper::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_FORBBID, m_bForbid);
	DDX_Control(pDX, IDC_CHK_FORBBID, m_bnVislbe);
}

BEGIN_MESSAGE_MAP(CDlgOper, CDialog)
	ON_BN_CLICKED(IDC_BN_UPDATE, &CDlgOper::OnBnClickedBnUpdate)
	ON_BN_CLICKED(IDC_BN_SALE, &CDlgOper::OnBnClickedBnSale)
	ON_BN_CLICKED(IDC_BN_INC, &CDlgOper::OnBnClickedBnInc)
	ON_BN_CLICKED(IDC_BN_REFUND, &CDlgOper::OnBnClickedBnRefund)
	ON_BN_CLICKED(IDC_BN_UNLOCK, &CDlgOper::OnBnClickedBnUnlock)
	ON_BN_CLICKED(IDC_BN_DEFFER, &CDlgOper::OnBnClickedBnDeffer)
	ON_MESSAGE(WM_ANALYSE, &CDlgOper::OnAnalyse)
	ON_BN_CLICKED(IDC_BN_READ, &CDlgOper::OnBnClickedBnRead)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BN_DESCIND, &CDlgOper::OnBnClickedBnDescind)
	ON_BN_CLICKED(IDC_CHK_FORBBID, &CDlgOper::OnBnClickedChkForbbid)
END_MESSAGE_MAP()


// CDlgOper 消息处理程序

void CDlgOper::OnBnClickedBnUpdate()
{
	// TODO: 在此添加控件通知处理程序代码

	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_UPDATE);
}

void CDlgOper::OnBnClickedBnSale()
{
	// TODO: 在此添加控件通知处理程序代码

	if (g_Analyse.bTicketType[0] == 0x01)
	{
		((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_SJTSALE, 0x01);
	}
	else
	{
		OTHERSALE os		= {0};
		WORD lenos			= 0;
		WORD lenss			= 0;
		BYTE bData[128]		= {0};
		BYTE bRecv[1024]	= {0};
		int pos				= 7;
		WORD wValue			= 0;
		CString strValue;

		do 
		{
			GetDlgItemText(IDC_EDIT_VALUE, strValue);
			wValue = (WORD)atoi(strValue);

			QueryPerformanceCounter(&g_liStart);
			g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 33, bData, 4, bRecv, sizeof(bRecv));
			QueryPerformanceCounter(&g_liEnd);

			if (g_retInfo.wErrCode != 0)
				break;
			pos += sizeof(RETINFO);

			memcpy(&lenss, bRecv + pos, 2);
			pos += 2;

			if (lenss > 0)
			{
				pos += lenss;
			}

			memcpy(&lenos, bRecv + pos, 2);
			pos += 2;

			if (lenos > 0)
			{
				memcpy(&os, bRecv + pos, lenos);
			}

		} while (0);

		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_SvtSale, &os, lenos, "发售", g_liStart, g_liEnd);	

		((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);
	}
}

void CDlgOper::OnBnClickedBnInc()
{
	// TODO: 在此添加控件通知处理程序代码
	if (CCSTesterDlg::m_bEnableDebuger)
		((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_INCREASE);
	else
		AfxMessageBox("权限错误！");
}

void CDlgOper::OnBnClickedBnRefund()
{
	// TODO: 在此添加控件通知处理程序代码
	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_REFUND);
}

void CDlgOper::OnBnClickedBnUnlock()
{
	// TODO: 在此添加控件通知处理程序代码
	
	TICKETLOCK tu		= {0};
	WORD lentu			= 0;
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 41, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(&lentu, bRecv + pos, 2);
		pos += 2;

		if (lentu > 0)
		{
			memcpy(&tu, bRecv + pos, lentu);
			pos += lentu;
		}

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Lock, &tu, lentu, "车票解锁", g_liStart, g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);
}

void CDlgOper::OnBnClickedBnDeffer()
{
	// TODO: 在此添加控件通知处理程序代码


	TICKETDEFER td		= {0};
	WORD lentd			= 0;
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 40, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(&lentd, bRecv + pos, 2);
		pos += 2;

		if (lentd > 0)
		{
			memcpy(&td, bRecv + pos, lentd);
			pos += lentd;
		}

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Deffer, &td, lentd, "车票延期", g_liStart, g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);
}

// 票卡分析可操作状态
#define OSTA_CHARGE					(DWORD)(0x01)							// 是否可充(值/乘次)
#define OSTA_UPDATE					(DWORD)(0x01<<1)						// 是否可更新
#define OSTA_SALE					(DWORD)(0x01<<2)						// 是否可发售
#define OSTA_ACTIVE					(DWORD)(0x01<<3)						// 是否可激活
#define OSTA_DIFFER					(DWORD)(0x01<<4)						// 是否可延期
#define OSTA_REFUND					(DWORD)(0x01<<5)						// 是否可退款
#define OSTA_UNLOCK					(DWORD)(0x01<<6)						// 是否可解锁
#define OSTA_REFUND_DE				(DWORD)(0x01<<7)						// 是否可退押金

void CDlgOper::EnableButtons(DWORD dwOperStatus)
{
	m_dwOperStatus = dwOperStatus;

	DWORD dwMap[][2] = 
	{
		IDC_BN_INC,		OSTA_CHARGE,
		IDC_BN_SALE,	OSTA_SALE,
		IDC_BN_DEFFER,	OSTA_DIFFER,
		IDC_BN_REFUND,	OSTA_REFUND,
		IDC_BN_UNLOCK,	OSTA_UNLOCK,
		IDC_BN_UPDATE,	OSTA_UPDATE,
		IDC_BN_REFUND,	OSTA_REFUND_DE
	};

	for (int i=0;i<sizeof(dwMap)/sizeof(dwMap[0]);i++)
	{
		if (dwOperStatus & dwMap[i][1])
			GetDlgItem(dwMap[i][0])->EnableWindow(TRUE);
		else
			GetDlgItem(dwMap[i][0])->EnableWindow(FALSE);
	}
}

void CDlgOper::DispAnalyseErr(CString str)
{
	CRect rc(0, 0, 300, 30);

	m_strAnalyseErr = str;
	InvalidateRect(&rc);
}

LRESULT CDlgOper::OnAnalyse(WPARAM wparam, LPARAM lparam)
{
	m_dwOperStatus = g_Analyse.dwOperationStauts;
	UpdateData();
	if (!m_bForbid)
		EnableButtons(g_Analyse.dwOperationStauts);

	m_bnVislbe.ShowWindow(CCSTesterDlg::m_bEnableDebuger);
	
	return 0;
}

void CDlgOper::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255, 20, 10));
	dc.TextOut(10, 1, m_strAnalyseErr, m_strAnalyseErr.GetLength());
}

void CDlgOper::OnBnClickedBnRead()
{
	// TODO: 在此添加控件通知处理程序代码
	TICKETINFO tkinf	= {0};
	WORD lenInf			= 0;
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 32, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(&lenInf, bRecv + pos, 2);
		pos += 2;

		if (lenInf > 0)
		{
			memcpy(&tkinf, bRecv + pos, lenInf);
			pos += lenInf;
		}

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_TkInfo, &tkinf, lenInf, "BOM读票卡信息", g_liStart, g_liEnd);
}

void CDlgOper::OnBnClickedBnDescind()
{
	// TODO: 在此添加控件通知处理程序代码
	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_DESCIND);
}

void CDlgOper::OnBnClickedChkForbbid()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	if (!m_bForbid)
		EnableButtons(g_Analyse.dwOperationStauts);
	else
		EnableButtons(0xFFFFFFFF);

}
