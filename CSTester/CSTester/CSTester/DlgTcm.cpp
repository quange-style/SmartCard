// DlgTcm.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgTcm.h"
#include "CSTesterDlg.h"


// CDlgTcm 对话框

IMPLEMENT_DYNAMIC(CDlgTcm, CDialog)

CDlgTcm::CDlgTcm(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTcm::IDD, pParent)
{

}

CDlgTcm::~CDlgTcm()
{
}

void CDlgTcm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgTcm, CDialog)
	ON_BN_CLICKED(IDC_BN_INIT, &CDlgTcm::OnBnClickedBnInit)
	ON_BN_CLICKED(IDC_BN_TKINFO, &CDlgTcm::OnBnClickedBnTkinfo)
	ON_BN_CLICKED(IDC_BN_OPEN, &CDlgTcm::OnBnClickedBnOpen)
	ON_MESSAGE(WM_PAGECHANGED, &CDlgTcm::OnPanalChanged)

	ON_BN_CLICKED(IDC_BN_ANALYSE, &CDlgTcm::OnBnClickedBnAnalyse)
	//ON_BN_CLICKED(IDC_BN_ANALYSE2, &CDlgTcm::OnBnClickedBnAnalyse2)
	ON_BN_CLICKED(ES_BN_EVALUATE, &CDlgTcm::OnBnClickedBnEvaluate)
	ON_BN_CLICKED(ES_BN_DESTROY, &CDlgTcm::OnBnClickedBnDestroy)
	ON_BN_CLICKED(ES_BN_RECODE, &CDlgTcm::OnBnClickedBnRecode)
	ON_BN_CLICKED(ES_BN_PRIVATE, &CDlgTcm::OnBnClickedBnPrivate)
	ON_BN_CLICKED(ES_BN_ANALYSE, &CDlgTcm::OnBnClickedEsBnAnalyse)
	ON_BN_CLICKED(ES_BN_INIT, &CDlgTcm::OnBnClickedEsBnInit)
END_MESSAGE_MAP()


// CDlgTcm 消息处理程序

void CDlgTcm::OnBnClickedBnOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!((CCSTesterDlg *)AfxGetMainWnd())->GetSetting(g_nPortOpen, g_wStation, g_wDeviceID))
		return;

	g_retInfo.wErrCode = (uint16_t)g_Serial.Open(g_nPortOpen);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Null, NULL, 0, "打开串口", g_liStart, g_liEnd);

	if (g_retInfo.wErrCode == 0)
	{
		AfxGetMainWnd()->SendMessage(WM_INITREADER);

		GetDlgItem(IDC_BN_OPEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_INIT)->EnableWindow(TRUE);
	}
}

void CDlgTcm::OnBnClickedBnInit()
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
		GetDlgItem(IDC_BN_TKINFO)->EnableWindow(TRUE);
		GetDlgItem(IDC_BN_ANALYSE)->EnableWindow(TRUE);
		memcpy(&lenInf, bRecv + 7 + sizeof(RETINFO), 2);
	}

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_init_arr, bRecv + 9 + sizeof(RETINFO), lenInf, "设备环境初始化", g_liStart, g_liEnd);
}

void CDlgTcm::OnBnClickedBnTkinfo()
{
	// TODO: 在此添加控件通知处理程序代码
	TICKETINFO tkinf	= {0};
	WORD lenInf			= 0;
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 50, NULL, 0, bRecv, sizeof(bRecv));
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

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_TkInfo, &tkinf, lenInf, "TCM读票卡信息", g_liStart, g_liEnd);
}

LRESULT CDlgTcm::OnPanalChanged(WPARAM wparam, LPARAM lparam)
{
	g_Serial.Close();

	AfxGetMainWnd()->SendMessage(WM_UNINITREADER);
	GetDlgItem(IDC_BN_TKINFO)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_ANALYSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_INIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_OPEN)->EnableWindow(TRUE);
	g_bDeviceType = 7;

	return 0;
}

void CDlgTcm::OnBnClickedBnAnalyse()
{
	// TODO: 在此添加控件通知处理程序代码
	//TODO: 在此添加控件通知处理程序代码
	WORD lenAly			= 0;
	TICKETLOCK tlk		= {0};
	WORD lenTlk			= 0;
	BYTE bData[128]		= {0};
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		bData[0] = 0x00;
		bData[1] = 0x02;

		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 61, bData, 2, bRecv, sizeof(bRecv));
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

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Analyse, &g_Analyse, lenAly, "票卡分析", g_liStart, g_liEnd);
	if (lenTlk > 0)
		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Lock, &tlk, lenTlk, "黑名单锁卡", g_liStart, g_liEnd);
}


//ES预赋值
void CDlgTcm::OnBnClickedBnEvaluate()
{
	// TODO: 在此添加控件通知处理程序代码
}

//ES注销
void CDlgTcm::OnBnClickedBnDestroy()
{
	// TODO: 在此添加控件通知处理程序代码
}

//ES重编码
void CDlgTcm::OnBnClickedBnRecode()
{
	// TODO: 在此添加控件通知处理程序代码
}

//ES个性化
void CDlgTcm::OnBnClickedBnPrivate()
{
	// TODO: 在此添加控件通知处理程序代码
}

//ES分析
void CDlgTcm::OnBnClickedEsBnAnalyse()
{
	//TODO: 在此添加控件通知处理程序代码
	WORD lenAly			= 0;
	TICKETLOCK tlk		= {0};
	WORD lenTlk			= 0;
	BYTE bData[128]		= {0};
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		//bData[0] = 0x00;
		//bData[1] = 0x02;

		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 51, bData, 2, bRecv, sizeof(bRecv));
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

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_ES_Analyse, &g_Analyse, lenAly, "票卡分析", g_liStart, g_liEnd);
	if (lenTlk > 0)
		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Lock, &tlk, lenTlk, "黑名单锁卡", g_liStart, g_liEnd);
}

//ES初始化
void CDlgTcm::OnBnClickedEsBnInit()
{
	// TODO: 在此添加控件通知处理程序代码
	
}
