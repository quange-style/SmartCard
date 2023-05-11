// DlgAvm.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgAvm.h"
#include "CSTesterDlg.h"


// DlgAvm 对话框

IMPLEMENT_DYNAMIC(CDlgAvm, CDialog)

CDlgAvm::CDlgAvm(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAvm::IDD, pParent)
{

}

CDlgAvm::~CDlgAvm()
{
}

BOOL CDlgAvm::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_cmbAnt.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgAvm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_ANT, m_cmbAnt);
}


BEGIN_MESSAGE_MAP(CDlgAvm, CDialog)
	//ON_BN_CLICKED(IDC_BTN_COM, &DlgAvm::OnBnClickedBtnCom)
	ON_BN_CLICKED(IDC_BN_OPEN, &CDlgAvm::OnBnClickedBnOpen)
	ON_BN_CLICKED(IDC_BN_INIT, &CDlgAvm::OnBnClickedBnInit)
	ON_BN_CLICKED(IDC_BN_ANALYSE, &CDlgAvm::OnBnClickedBnAnalyse)
	ON_BN_CLICKED(IDC_BN_ANALYSE, &CDlgAvm::OnBnClickedBnAnalyse)
	ON_MESSAGE(WM_PAGECHANGED, &CDlgAvm::OnPanalChanged)
	ON_BN_CLICKED(IDC_BN_TKINFO, &CDlgAvm::OnBnClickedBnTkinfo)
END_MESSAGE_MAP()


// DlgAvm 消息处理程序


void CDlgAvm::OnBnClickedBnOpen()
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



void CDlgAvm::OnBnClickedBnInit()
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
		//GetDlgItem(IDC_BN_SALE_SJT)->EnableWindow(TRUE);
		//GetDlgItem(IDC_BN_CLEAR_SJT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BN_TKINFO)->EnableWindow(TRUE);
		//GetDlgItem(IDC_BN_SVT_INC)->EnableWindow(TRUE);

		memcpy(&lenInf, bRecv + 7 + sizeof(RETINFO), 2);
	}

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_init_arr, bRecv + 9 + sizeof(RETINFO), lenInf, "设备环境初始化", g_liStart, g_liEnd);
}

void CDlgAvm::OnBnClickedBnAnalyse()
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
		//g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 71, NULL, 0, bRecv, sizeof(bRecv));
		g_retInfo = g_Serial.Communicate((BYTE)m_cmbAnt.GetCurSel(), g_nBeep, 1, 71, NULL, 0, bRecv, sizeof(bRecv));
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


	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Analyse, &g_Analyse, lenAly, "票卡分析", g_liStart, g_liEnd);
}



LRESULT CDlgAvm::OnPanalChanged(WPARAM wparam, LPARAM lparam)
{
	g_Serial.Close();

	AfxGetMainWnd()->SendMessage(WM_UNINITREADER);
	//GetDlgItem(IDC_BN_TKINFO)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_ANALYSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_INIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_OPEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BN_TKINFO)->EnableWindow(FALSE);
	g_bDeviceType = 16;

	return 0;
}

void CDlgAvm::OnBnClickedBnTkinfo()
{
	// TODO: 在此添加控件通知处理程序代码
	TICKETINFO tkinf	= {0};
	WORD lenInf			= 0;
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		//g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 70, NULL, 0, bRecv, sizeof(bRecv));
		g_retInfo = g_Serial.Communicate((BYTE)m_cmbAnt.GetCurSel(), g_nBeep, 1, 70, NULL, 0, bRecv, sizeof(bRecv));
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

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_TkInfo, &tkinf, lenInf, "AVM读票卡信息", g_liStart, g_liEnd);
}
