// DlgTvm.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgTvm.h"
#include "CSTesterDlg.h"


// CDlgTvm 对话框

IMPLEMENT_DYNAMIC(CDlgTvm, CDialog)

CDlgTvm::CDlgTvm(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTvm::IDD, pParent)
{
}

CDlgTvm::~CDlgTvm()
{
}

void CDlgTvm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_ANT, m_cmbAnt);
}

BEGIN_MESSAGE_MAP(CDlgTvm, CDialog)
	ON_BN_CLICKED(IDC_BN_INIT, &CDlgTvm::OnBnClickedBnInit)
	ON_BN_CLICKED(IDC_BN_ANALYSE, &CDlgTvm::OnBnClickedBnAnalyse)
	ON_BN_CLICKED(IDC_BN_SALE_SJT, &CDlgTvm::OnBnClickedBnSaleSjt)
	ON_BN_CLICKED(IDC_BN_CLEAR_SJT, &CDlgTvm::OnBnClickedBnClearSjt)
	ON_BN_CLICKED(IDC_BN_SVT_INC, &CDlgTvm::OnBnClickedBnSvtInc)
	ON_BN_CLICKED(IDC_BN_SVT_DEC, &CDlgTvm::OnBnClickedBnSvtDec)
	ON_BN_CLICKED(IDC_BN_SALE_ON, &CDlgTvm::OnBnClickedBnSaleOn)

	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BN_OPEN, &CDlgTvm::OnBnClickedBnOpen)
	ON_MESSAGE(WM_PAGECHANGED, &CDlgTvm::OnPanalChanged)

//	ON_BN_CLICKED(IDC_BN_SALE_ON2, &CDlgTvm::OnBnClickedBnSaleOn2)
ON_BN_CLICKED(IDC_BN_SALE_ON2, &CDlgTvm::OnBnClickedBnSaleOn2)
//ON_BN_CLICKED(IDC_BN_SALE_SJT2, &CDlgTvm::OnBnClickedBnSaleSjt2)
//ON_BN_CLICKED(IDC_BN_SALE_ON3, &CDlgTvm::OnBnClickedBnSaleOn3)
ON_BN_CLICKED(IDC_BN_SALE_ON3, &CDlgTvm::OnBnClickedBnSaleOn3)
ON_BN_CLICKED(IDC_BN_SALE_ON4, &CDlgTvm::OnBnClickedBnSaleOn4)
ON_BN_CLICKED(IDC_BN_SALE_ON5, &CDlgTvm::OnBnClickedBnSaleOn5)
ON_BN_CLICKED(IDC_BN_SALE_ON6, &CDlgTvm::OnBnClickedBnSaleOn6)

END_MESSAGE_MAP()


// CDlgTvm 消息处理程序

void CDlgTvm::OnBnClickedBnOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!((CCSTesterDlg *)AfxGetMainWnd())->GetSetting(g_nPortOpen, g_wStation, g_wDeviceID))
		return;

	g_retInfo.wErrCode = (uint16_t)g_Serial.Open(g_nPortOpen);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Null, NULL, 0, "打开串口", g_liStart, g_liEnd);

}

void CDlgTvm::OnBnClickedBnInit()
{
	// TODO: 在此添加控件通知处理程序代码
	BYTE bData[32]		= {0};
	BYTE bRecv[1024]	= {0};
	WORD lenInf			= 0;

	if (!((CCSTesterDlg *)AfxGetMainWnd())->GetSetting(g_nPortOpen, g_wStation, g_wDeviceID))
		return;

	bData[0] = Hex_Ex_Bcd((BYTE)(g_wStation / 100), true);
	bData[1] = Hex_Ex_Bcd((BYTE)(g_wStation % 100), true);
	bData[2] = g_bDeviceType;
	bData[3] = Hex_Ex_Bcd((BYTE)(g_wDeviceID / 100), true);
	bData[4] = Hex_Ex_Bcd((BYTE)(g_wDeviceID % 100), true);

	QueryPerformanceCounter(&g_liStart);
	g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 0, bData, 5, bRecv, sizeof(bRecv));
	QueryPerformanceCounter(&g_liEnd);

	if (g_retInfo.wErrCode == 0)
	{
		GetDlgItem(IDC_BN_ANALYSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BN_SALE_SJT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BN_CLEAR_SJT)->EnableWindow(TRUE);
		//GetDlgItem(IDC_BN_SVT_DEC)->EnableWindow(TRUE);
		//GetDlgItem(IDC_BN_SVT_INC)->EnableWindow(TRUE);

		memcpy(&lenInf, bRecv + 7 + sizeof(RETINFO), 2);
	}

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_init_arr, bRecv + 9 + sizeof(RETINFO), lenInf, "设备环境初始化", g_liStart, g_liEnd);
}

void CDlgTvm::OnBnClickedBnAnalyse()
{
	//TODO: 在此添加控件通知处理程序代码
	WORD lenAly			= 0;
	TICKETLOCK tlk		= {0};
	WORD lenTlk			= 0;
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		//g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 22, NULL, 0, bRecv, sizeof(bRecv));
		g_retInfo = g_Serial.Communicate((BYTE)m_cmbAnt.GetCurSel(), g_nBeep, 1, 22, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(&lenAly, bRecv + pos, 2);
		pos += 2;

		if (lenAly > 0)
		{
			memcpy(&g_Analyse, bRecv + pos, lenAly);
			pos += lenAly;
		}

		memcpy(&lenTlk, bRecv + pos, 2);
		pos += 2;

		if (lenTlk > 0)
		{
			memcpy(&tlk, bRecv + pos, lenTlk);
			pos += lenTlk;
		}

		AfxGetMainWnd()->SendMessage(WM_ANALYSE);

		if (g_Analyse.bTicketType[0] == 0x01)
		{
			GetDlgItem(IDC_BN_SVT_INC)->EnableWindow(FALSE);
			GetDlgItem(IDC_BN_SVT_DEC)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_BN_SVT_INC)->EnableWindow(TRUE);
			GetDlgItem(IDC_BN_SVT_DEC)->EnableWindow(TRUE);
		}

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Analyse, &g_Analyse, lenAly, "票卡分析", g_liStart, g_liEnd);
}

void CDlgTvm::OnBnClickedBnSaleSjt()
{
	// TODO: 在此添加控件通知处理程序代码
	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_SJTSALE);
}

void CDlgTvm::OnBnClickedBnClearSjt()
{
	// TODO: 在此添加控件通知处理程序代码
	int pos				= 7;
	BYTE bRecv[256]		= {0};

	QueryPerformanceCounter(&g_liStart);
	g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 21, NULL, 0, bRecv, sizeof(bRecv));
	QueryPerformanceCounter(&g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Null, NULL, 0, "单程票减值", g_liStart, g_liEnd);
}

void CDlgTvm::OnBnClickedBnSvtInc()
{
	// TODO: 在此添加控件通知处理程序代码
	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_INCREASE);
}

void CDlgTvm::OnBnClickedBnSvtDec()
{
	// TODO: 在此添加控件通知处理程序代码
	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_DECREASE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void CDlgTvm::SjtSale(WORD wValue)
{
	// TODO: 在此添加控件通知处理程序代码
	SJTSALE ss			= {0};
	WORD lenss			= 0;
	BYTE bData[128]		= {0};
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		memcpy(bData, &wValue, 2);
		bData[2] = 0x01;
		bData[3] = 0x02;

		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 20, bData, 4, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		if (g_retInfo.wErrCode != 0)
			break;
		pos += sizeof(RETINFO);

		memcpy(&lenss, bRecv + pos, 2);
		pos += 2;

		if (lenss > 0)
		{
			memcpy(&ss, bRecv + pos, lenss);
		}

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_SjtSale, &ss, lenss, "发售", g_liStart, g_liEnd);
}

void CDlgTvm::SjtAnalyse()
{
	//TODO: 在此添加控件通知处理程序代码
	WORD lenAly			= 0;
	TICKETLOCK tlk		= {0};
	WORD lenTlk			= 0;
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 22, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(&lenAly, bRecv + pos, 2);
		pos += 2;

		if (lenAly > 0)
		{
			memcpy(&g_Analyse, bRecv + pos, lenAly);
			pos += lenAly;
		}

		memcpy(&lenTlk, bRecv + pos, 2);
		pos += 2;

		if (lenTlk > 0)
		{
			memcpy(&tlk, bRecv + pos, lenTlk);
			pos += lenTlk;
		}

		AfxGetMainWnd()->SendMessage(WM_ANALYSE);

		if (g_Analyse.bTicketType[0] == 0x01)
		{
			GetDlgItem(IDC_BN_SVT_INC)->EnableWindow(FALSE);
			GetDlgItem(IDC_BN_SVT_DEC)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_BN_SVT_INC)->EnableWindow(TRUE);
			GetDlgItem(IDC_BN_SVT_DEC)->EnableWindow(TRUE);
		}

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Analyse, &g_Analyse, lenAly, "票卡分析", g_liStart, g_liEnd);
}

void CDlgTvm::OnBnClickedBnSaleOn()
{
	// TODO: 在此添加控件通知处理程序代码
	int nPriceArry[4] = {200, 300, 400, 500};
	int pos;
	for (int Count=0;;Count++)
	{
		pos = Count % 4;
		SjtSale(nPriceArry[pos]);
		if (g_retInfo.wErrCode == 0)
		{
			SjtAnalyse();
			if (g_retInfo.wErrCode != 0 || g_Analyse.lBalance != nPriceArry[pos])
			{
				AfxMessageBox("无效票");
				break;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CDlgTvm::OnPanalChanged(WPARAM wparam, LPARAM lparam)
{
	g_Serial.Close();

	AfxGetMainWnd()->SendMessage(WM_UNINITREADER);
	GetDlgItem(IDC_BN_ANALYSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_SALE_SJT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_CLEAR_SJT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_SVT_INC)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_SVT_DEC)->EnableWindow(FALSE);

	g_bDeviceType = 2;

	return 0;
}

BOOL CDlgTvm::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_cmbAnt.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//void CDlgTvm::OnBnClickedBnSaleOn2()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}

void CDlgTvm::OnBnClickedBnSaleOn2()
{
	// TODO: 在此添加控件通知处理程序代码

	ETICKETDEALINFO etg = {0};
	GETETICKETRESPINFO getri = {0};
	WORD leneng			= 0;
	WORD lenetg			= 0;
	WORD lengetri       = 0;
	TICKETLOCK tl		= {0};
	WORD lentl			= 0;
	BYTE bRecv[512]		= {0};
	int pos				= 7;
	BYTE bRfu[2] = {0};

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate((BYTE)m_cmbAnt.GetCurSel(), g_nBeep, 1, 27, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(bRfu,bRecv + 7 + 3, 2);

		if(bRfu[0] == 0x11)
			memcpy(&lengetri, bRecv + pos, 2);
		else
			memcpy(&leneng, bRecv + pos, 2);
		pos += 2;

		if(lengetri > 0)
		{
			printf("lengetri");
			memcpy(&getri, bRecv + pos, lengetri);
			pos += lengetri;
		}

		memcpy(&lentl, bRecv + pos, 2);
		pos += 2;

		if (lentl > 0)
		{
			memcpy(&tl, bRecv + pos, lentl);
		}

	} while (0);

	if (lentl > 0)
		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Lock, &tl, lentl, "黑名单加锁", g_liStart, g_liEnd);
	if(bRfu[0] == 0x11)
	{
		printf("二维码进闸");
		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_GetETicket, &getri, lengetri, "互联网取票", g_liStart, g_liEnd);
	}
}

/*
//取票发售
void CDlgTvm::OnBnClickedBnSaleSjt3()
{
	// TODO: 在此添加控件通知处理程序代码
	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_SJTSALE);
}*/

//取票发售
void CDlgTvm::OnBnClickedBnSaleOn3()
{
	// TODO: 在此添加控件通知处理程序代码
	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_SJTSALE);
}


void CDlgTvm::OnBnClickedBnSaleOn4()
{
	// TODO: 在此添加控件通知处理程序代码

	ETICKETDEALINFO etg = {0};
	GETETICKETRESPINFO getri = {0};
	WORD leneng			= 0;
	WORD lenetg			= 0;
	WORD lengetri       = 0;
	TICKETLOCK tl		= {0};
	WORD lentl			= 0;
	BYTE bRecv[512]		= {0};
	int pos				= 7;
	BYTE bRfu[2] = {0};
	BYTE bData[32]		= {0};

	do 
	{
		QueryPerformanceCounter(&g_liStart);

		memcpy(bData, "02", 2);
		memcpy(&bData[2], "01", 2);

		g_retInfo = g_Serial.Communicate((BYTE)m_cmbAnt.GetCurSel(), g_nBeep, 1, 69, bData, 4, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(bRfu,bRecv + 7 + 3, 2);

		if(bRfu[0] == 0x11)
			memcpy(&lengetri, bRecv + pos, 2);
		else
			memcpy(&leneng, bRecv + pos, 2);
		pos += 2;


	} while (0);

}


void CDlgTvm::OnBnClickedBnSaleOn5()
{
	// TODO: 在此添加控件通知处理程序代码

	ETICKETDEALINFO etg = {0};
	GETETICKETRESPINFO getri = {0};
	WORD leneng			= 0;
	WORD lenetg			= 0;
	WORD lengetri       = 0;
	TICKETLOCK tl		= {0};
	WORD lentl			= 0;
	BYTE bRecv[512]		= {0};
	int pos				= 7;
	BYTE bRfu[2] = {0};
	BYTE bData[32]		= {0};

	do 
	{
		QueryPerformanceCounter(&g_liStart);

		g_retInfo = g_Serial.Communicate((BYTE)m_cmbAnt.GetCurSel(), g_nBeep, 1, 68, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(bRfu,bRecv + 7 + 3, 2);

		if(bRfu[0] == 0x11)
			memcpy(&lengetri, bRecv + pos, 2);
		else
			memcpy(&leneng, bRecv + pos, 2);
		pos += 2;


	} while (0);

}


void CDlgTvm::OnBnClickedBnSaleOn6()
{
	// TODO: 在此添加控件通知处理程序代码
	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_SJTSALE);

}