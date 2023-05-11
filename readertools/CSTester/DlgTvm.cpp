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

	//ON_BN_CLICKED(IDC_BN_SALE_ON2, &CDlgTvm::OnBnClickedBnSaleOn2)
	ON_BN_CLICKED(IDC_BN_SALE_ON2, &CDlgTvm::OnBnClickedBnSaleOn2)
	//ON_BN_CLICKED(IDC_BN_SALE_SJT2, &CDlgTvm::OnBnClickedBnSaleSjt2)
	//ON_BN_CLICKED(IDC_BN_SALE_ON3, &CDlgTvm::OnBnClickedBnSaleOn3)
	ON_BN_CLICKED(IDC_BN_SALE_ON3, &CDlgTvm::OnBnClickedBnSaleOn3)
	ON_BN_CLICKED(IDC_BN_SALE_ON4, &CDlgTvm::OnBnClickedBnSaleOn4)
	ON_BN_CLICKED(IDC_BN_SALE_ON5, &CDlgTvm::OnBnClickedBnSaleOn5)
	ON_BN_CLICKED(IDC_BN_SALE_ON6, &CDlgTvm::OnBnClickedBnSaleOn6)
	ON_BN_CLICKED(IDC_BN_RETURNQR, &CDlgTvm::OnBnClickedBnReturnQR)

	ON_BN_CLICKED(IDC_BN_GET_PAY_CODE, &CDlgTvm::OnClickedGetPayCode)
	ON_BN_CLICKED(IDC_BN_SAM_ACTV, &CDlgTvm::OnBnClickedBnSamActv)
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
		setBtnEnable(TRUE);
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
	g_retInfo = g_Serial.Communicate((BYTE)m_cmbAnt.GetCurSel(), g_nBeep, 1, 21, NULL, 0, bRecv, sizeof(bRecv));
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
	SJTSALE data			= {0};
	WORD len_ss			= 0;
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

		CString cs = "good";
		if (g_retInfo.wErrCode != 0) {
			//失败计数  
			cs.Format("wErrCode=%2d, CurrentFailureCount:%ld  TotleCount:		%ld\n ",
				g_retInfo.wErrCode, testCounterRecord.getCurrentFailureCount(), testCounterRecord.getCurrentTotleCounts());
			testCounterRecord.setFailureCount();
			break;
		}else {
			//成功计数
			testCounterRecord.setSuccessCount();
			cs.Format("SuccessCount:  %ld  TotleCount:	%ld\n ", 
				testCounterRecord.getCurrentSuccessCount(),testCounterRecord.getCurrentTotleCounts());
		}
		((CCSTesterDlg*)AfxGetMainWnd())->DispResult(cs, "发售", g_retInfo, g_liStart, g_liEnd);
		pos += sizeof(RETINFO);

		memcpy(&len_ss, bRecv + pos, 2);
		pos += 2;

		if (len_ss > 0)
		{
			memcpy(&data, bRecv + pos, len_ss);
		}

	} while (0);

	

	//((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_SjtSale, &data, len_ss, "发售", g_liStart, g_liEnd);

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
	AfxGetMainWnd()->SendMessage(WM_SALE_TEST);
	return;

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CDlgTvm::OnPanalChanged(WPARAM wparam, LPARAM lparam)
{
	g_Serial.Close();

	AfxGetMainWnd()->SendMessage(WM_UNINITREADER);
	setBtnEnable(FALSE);
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

void CDlgTvm::OnBnClickedBnSaleOn2()//互联网取票
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
void CDlgTvm::OnBnClickedBnSaleOn3()//单程票取票
{
	// TODO: 在此添加控件通知处理程序代码
	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_SJTSALE);
}


void CDlgTvm::OnBnClickedBnSaleOn4()//取票结果回写
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


void CDlgTvm::OnBnClickedBnSaleOn5()//第三方取票
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


void CDlgTvm::OnBnClickedBnSaleOn6()//第三方取票售卡
{
	// TODO: 在此添加控件通知处理程序代码
	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_SJTSALE);

}

void CDlgTvm::OnBnClickedBnReturnQR()
{
	// TODO: 在此添加控件通知处理程序代码
	WORD lenAly = 0;
	BYTE bData[128] = { 0 };
	BYTE bRecv[1024] = { 0 };
	int pos = 7;
	BYTE bRfu[2] = { 0 };
	char TicketSubType[2] = { 0 };
	BOMESJTRETURN sjtData = { 0 };
	BOMESVTRETURN svtData = { 0 };

	do
	{
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 72, bData, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);
		memcpy(bRfu, bRecv + 7 + 3, 2);
		memcpy(&lenAly, bRecv + pos, 2);
		pos += 2;
		memcpy(&TicketSubType, bRecv + (pos + 28), 2);

		if (lenAly > 0)
		{
			if (TicketSubType[1] == '0')
				memcpy(&sjtData, bRecv + pos, lenAly);
			else
				memcpy(&svtData, bRecv + pos, lenAly);

			pos += lenAly;
		}

	} while (0);

	if (TicketSubType[1] == '0')
	{
		((CCSTesterDlg*)AfxGetMainWnd())->DispResult(struct_ESJT, &sjtData, lenAly, "电子单程票返回", g_liStart, g_liEnd);
	}
	else
		((CCSTesterDlg*)AfxGetMainWnd())->DispResult(struct_ESVT, &svtData, lenAly, "电子储值票返回", g_liStart, g_liEnd);

	((CCSTesterDlg*)AfxGetMainWnd())->TransConfirm(g_retInfo);
}


void CDlgTvm::OnBnClickedBnSamActv()
{
	// TODO: 在此添加控件通知处理程序代码
	AfxGetMainWnd()->SendMessage(WM_SAMACT);
}

void CDlgTvm::OnClickedGetPayCode()
{
	WORD lengetri = 0;
	BYTE bRecv[512] = { 0 };
	int pos = 7;
	BYTE bData[32] = { 0 };
	CString pay_code_cs;
	CString tip_info;
	RETINFO ref;
	char pay_code[64] = { 0 };
	do
	{
		tip_info = "获取第三方支付授权码";
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 73, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);
		memcpy(&ref.wErrCode, bRecv + pos - 5, 2);
		memcpy(&ref.bNoticeCode, bRecv + pos - 3, 2);
		memcpy(&ref.bRfu, bRecv + pos - 2, 2);

		if (ref.wErrCode) {
			break;
		}

		memcpy(&lengetri, bRecv + pos, 2);
		pos += 2;

		memcpy(&pay_code, bRecv + pos, 64);
		pay_code_cs.Format(pay_code);
		pay_code_cs.AppendChar('\n');


	} while (0);
	((CCSTesterDlg*)AfxGetMainWnd())->DispResult(pay_code_cs, tip_info, ref, g_liStart, g_liEnd);
}

void CDlgTvm::setBtnEnable(BOOL beEnable) {

	GetDlgItem(IDC_BN_ANALYSE)->EnableWindow(beEnable);
	GetDlgItem(IDC_BN_SALE_SJT)->EnableWindow(beEnable);
	GetDlgItem(IDC_BN_CLEAR_SJT)->EnableWindow(beEnable);
	GetDlgItem(IDC_BN_RETURNQR)->EnableWindow(beEnable);
	GetDlgItem(IDC_BN_GET_PAY_CODE)->EnableWindow(beEnable);
	GetDlgItem(IDC_BN_SALE_ON)->EnableWindow(beEnable);


	GetDlgItem(IDC_BN_SALE_ON2)->EnableWindow(beEnable);
	GetDlgItem(IDC_BN_SALE_ON3)->EnableWindow(beEnable);
	GetDlgItem(IDC_BN_SALE_ON4)->EnableWindow(beEnable);
	GetDlgItem(IDC_BN_SALE_ON5)->EnableWindow(beEnable);
	GetDlgItem(IDC_BN_SALE_ON6)->EnableWindow(beEnable);


}
