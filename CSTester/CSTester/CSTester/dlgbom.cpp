// DlgBom.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgBom.h"
#include "DlgLogin.h"
#include "CSTesterDlg.h"

BOMANALYZE	g_Analyse = {0};
RETINFO		g_retInfo;
// CDlgBom 对话框

IMPLEMENT_DYNAMIC(CDlgBom, CDialog)

CDlgBom::CDlgBom(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBom::IDD, pParent)
{
	m_bLogin = false;
	m_nWorkZone = 2;
	memset(&m_opl, 0, sizeof(m_opl));
	m_hbpZoneP = (HBITMAP)::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BMP_PZONE));
	m_hbpZoneF = (HBITMAP)::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BMP_FZONE));
}

CDlgBom::~CDlgBom()
{
	::DeleteObject(m_hbpZoneP);
	::DeleteObject(m_hbpZoneF);
}

void CDlgBom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ZONE, m_StaticZone);
}

BEGIN_MESSAGE_MAP(CDlgBom, CDialog)
	ON_BN_CLICKED(IDC_BN_INIT, &CDlgBom::OnBnClickedBnInit)
	ON_BN_CLICKED(IDC_BN_LOGIN, &CDlgBom::OnBnClickedBnLogin)
	ON_BN_CLICKED(IDC_BN_ANALYSE, &CDlgBom::OnBnClickedBnAnalyse)
	ON_BN_CLICKED(IDC_BN_SALE_EXIT, &CDlgBom::OnBnClickedBnSaleExit)
	ON_BN_CLICKED(IDC_BN_SALE_SJT, &CDlgBom::OnBnClickedBnSaleSjt)
	ON_BN_CLICKED(IDC_BN_SVT_DEC, &CDlgBom::OnBnClickedBnSvtDec)
	ON_BN_CLICKED(IDC_BN_DESTORY, &CDlgBom::OnBnClickedBnDestory)
	ON_STN_CLICKED(IDC_STATIC_ZONE, &CDlgBom::OnStnClickedStaticZone)
	ON_STN_DBLCLK(IDC_STATIC_ZONE, &CDlgBom::OnStnDblclickStaticZone)
	ON_BN_CLICKED(IDC_BN_OPEN, &CDlgBom::OnBnClickedBnOpen)
	ON_BN_CLICKED(IDC_BN_SAM_ACTV, &CDlgBom::OnBnClickedBnSamActv)
	ON_MESSAGE(WM_PAGECHANGED, &CDlgBom::OnPanalChanged)
END_MESSAGE_MAP()


// CDlgBom 消息处理程序

void CDlgBom::OnBnClickedBnOpen()
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
		GetDlgItem(IDC_BN_LOGIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BN_INIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BN_SAM_ACTV)->EnableWindow(TRUE);
	}
}

void CDlgBom::OnBnClickedBnInit()
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
	bData[5] = 0x55;
	bData[6] = 0x10;
	memcpy(&bData[7],"\x02\x19\x55\x10\xFF\xFF\xFF\xFF",8);

	QueryPerformanceCounter(&g_liStart);
	g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 0, bData, 15, bRecv, sizeof(bRecv));
	QueryPerformanceCounter(&g_liEnd);

	if (g_retInfo.wErrCode == 0)
	{
		memcpy(&lenInf, bRecv + 7 + sizeof(RETINFO), 2);
	}

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_init_arr, bRecv + 9 + sizeof(RETINFO), lenInf, "设备环境初始化", g_liStart, g_liEnd);
}

void CDlgBom::OnBnClickedBnLogin()
{
	// TODO: 在此添加控件通知处理程序代码
	BYTE bData[32]		= {0};
	BYTE bRecv[1024]	= {0};

	CDlgLogin dlg;
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetOperatorInfo(m_opl.szOperID, m_opl.nBomShiftID);
		memcpy(bData, m_opl.szOperID, 6);

		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 30, bData, 7, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Null, NULL, 0, "操作员登录", g_liStart, g_liEnd);

		if (g_retInfo.wErrCode == 0)
		{
			m_opl.nBomShiftID++;
			m_bLogin = true;
			GetDlgItem(IDC_BN_LOGIN)->EnableWindow(FALSE);
			GetDlgItem(IDC_BN_ANALYSE)->EnableWindow(TRUE);
			GetDlgItem(IDC_BN_SALE_EXIT)->EnableWindow(TRUE);
			GetDlgItem(IDC_BN_SALE_SJT)->EnableWindow(TRUE);
			GetDlgItem(IDC_BN_DESTORY)->EnableWindow(TRUE);
		}
	}
}

void CDlgBom::OnBnClickedBnAnalyse()
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
		bData[0] = 0x01;
		bData[1] = (BYTE)(m_nWorkZone);

		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 31, bData, 2, bRecv, sizeof(bRecv));
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
			GetDlgItem(IDC_BN_SVT_DEC)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_BN_SVT_DEC)->EnableWindow(TRUE);

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Analyse, &g_Analyse, lenAly, "票卡分析", g_liStart, g_liEnd);
	if (lenTlk > 0)
		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Lock, &tlk, lenTlk, "黑名单锁卡", g_liStart, g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);
}

void CDlgBom::OnBnClickedBnSaleExit()
{
	// TODO: 在此添加控件通知处理程序代码

	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_SALEEX);
}

void CDlgBom::OnBnClickedBnSaleSjt()
{
	// TODO: 在此添加控件通知处理程序代码

	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_SJTSALE, 0x01);
}

void CDlgBom::OnBnClickedBnSvtDec()
{
	// TODO: 在此添加控件通知处理程序代码

	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_DECREASE);
}

void CDlgBom::OnBnClickedBnDestory()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bLogin = false;
	GetDlgItem(IDC_BN_LOGIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BN_ANALYSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_SALE_EXIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_SALE_SJT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_SVT_DEC)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_DESTORY)->EnableWindow(FALSE);
}

LRESULT CDlgBom::OnPanalChanged(WPARAM wparam, LPARAM lparam)
{
	g_Serial.Close();
	AfxGetMainWnd()->SendMessage(WM_UNINITREADER);

	GetDlgItem(IDC_BN_OPEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BN_INIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_SAM_ACTV)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_LOGIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_ANALYSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_SALE_EXIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_SALE_SJT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_SVT_DEC)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_DESTORY)->EnableWindow(FALSE);
	m_bLogin = false;

	g_bDeviceType = 3;

	return 0;
}

void CDlgBom::OnStnClickedStaticZone()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_nWorkZone == 2)
	{
		m_nWorkZone = 1;
		m_StaticZone.SetBitmap(m_hbpZoneP);
	}
	else if (m_nWorkZone == 1)
	{
		m_nWorkZone = 2;
		m_StaticZone.SetBitmap(m_hbpZoneF);
	}
}

void CDlgBom::OnStnDblclickStaticZone()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_nWorkZone == 2)
	{
		m_nWorkZone = 1;
		m_StaticZone.SetBitmap(m_hbpZoneP);
	}
	else if (m_nWorkZone == 1)
	{
		m_nWorkZone = 2;
		m_StaticZone.SetBitmap(m_hbpZoneF);
	}
}

void CDlgBom::OnBnClickedBnSamActv()
{
	//TODO: 在此添加控件通知处理程序代码
	AfxGetMainWnd()->SendMessage(WM_SAMACT);
}
