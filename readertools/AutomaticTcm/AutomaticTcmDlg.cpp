// AutomaticTcmDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutomaticTcm.h"
#include "AutomaticTcmDlg.h"
#include "Serial.h"
#include "PubFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Serial g_Serial;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAutomaticTcmDlg 对话框




CAutomaticTcmDlg::CAutomaticTcmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutomaticTcmDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bRunning = FALSE;
	m_hEventRun = NULL;
}

void CAutomaticTcmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_HIS, m_ListHis);
	DDX_Control(pDX, IDC_EDIT_DEVICE, m_EdtDev);
	DDX_Control(pDX, IDC_CMB_PORT, m_CmbPort);
	DDX_Control(pDX, IDC_SET, m_BnSet);
}

BEGIN_MESSAGE_MAP(CAutomaticTcmDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SET, &CAutomaticTcmDlg::OnBnClickedSet)
	ON_CBN_SELCHANGE(IDC_CMB_PORT, &CAutomaticTcmDlg::OnCbnSelchangeCmbPort)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAutomaticTcmDlg 消息处理程序

BOOL CAutomaticTcmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_EdtDev.SetLimitText(9);
	m_EdtDev.SetWindowText("020107001");

	EnumSerials(m_CmbPort);
	m_nPort = GetReaderPort();

	m_ListHis.SetExtendedStyle(m_ListHis.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListHis.InsertColumn(0, "交易时间", LVCFMT_LEFT, 200);
	m_ListHis.InsertColumn(1, "交易设备", LVCFMT_LEFT, 150);
	m_ListHis.InsertColumn(2, "交易金额", LVCFMT_LEFT, 100);
	m_ListHis.InsertColumn(3, "状态", LVCFMT_LEFT, 300);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAutomaticTcmDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutomaticTcmDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CAutomaticTcmDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


int CAutomaticTcmDlg::EnumSerials(CComboBox& comb)
{
	HKEY			hKey;
	int				nIndex		= 0;
	TCHAR			szName[64]	= {0};
	TCHAR			szValue[64]	= {0};
	DWORD			dwSize		= 64;
	DWORD			dwType		= REG_SZ;
	if(ERROR_SUCCESS == ::RegOpenKey(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", &hKey))
	{
		comb.ResetContent();
		while(ERROR_NO_MORE_ITEMS != RegEnumValue(hKey, nIndex++, szName, &dwSize, NULL, &dwType, (LPBYTE)szValue, &dwSize))
		{
			comb.AddString(szValue);
			ZeroMemory(szName, 64);
			ZeroMemory(szValue, 64);
			dwSize		= 64;
			dwType		= REG_SZ;
		}
	}

	int ret = comb.GetCount();
	if (ret > 0)
		comb.SetCurSel(0);

	return ret;
}

void CAutomaticTcmDlg::OnBnClickedSet()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bRunning)
	{
		m_bRunning = StartDevice();
		if (m_bRunning)
		{
			m_CmbPort.EnableWindow(FALSE);
			m_EdtDev.EnableWindow(FALSE);
			m_BnSet.SetWindowText("停止设备");
		}
	}
	else
	{
		StopDevice();
		m_bRunning = FALSE;

		m_CmbPort.EnableWindow(FALSE);
		m_EdtDev.EnableWindow(FALSE);
		m_BnSet.SetWindowText("启动设备");
	}
}

void CAutomaticTcmDlg::OnCbnSelchangeCmbPort()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nPort = GetReaderPort();
}

int CAutomaticTcmDlg::GetReaderPort()
{
	CString strPort;
	int nPort = -1;
	m_CmbPort.GetWindowText(strPort);
	if (strPort != "")
	{
		strPort = strPort.Right(strPort.GetLength() - 3);
		nPort = atoi(strPort);
	}

	return nPort;
}

void CAutomaticTcmDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	StopDevice();

	CDialog::OnClose();
}

BOOL CAutomaticTcmDlg::InitDevice()
{
	BYTE bData[32]		= {0};
	BYTE bRecv[1024]	= {0};
	BOOL ret			= FALSE;
	CString strDev;

	do 
	{
		m_EdtDev.GetWindowText(strDev);
		if (strDev.GetLength() != 9)
			break;

		strDev = strDev.Left(6) + "0" + strDev.Right(3);

		String2HexSeq(strDev.GetBuffer(), strDev.GetLength(), bData, sizeof(bData));

		RETINFO retInf = g_Serial.Communicate(0, 0, 1, 0, bData, 5, bRecv, sizeof(bRecv));
		if (retInf.wErrCode != 0)
			break;

		ret = TRUE;

	} while (0);

	return ret;
}

BOOL CAutomaticTcmDlg::StartDevice()
{
	BOOL ret = FALSE;
	int nPortCursel = -1;

	do 
	{
		if (m_nPort <= 0)
		{
			MessageBox("请选择可用的端口", "错误");
			break;
		}

		if (g_Serial.Open(m_nPort) != 0)
		{
			MessageBox("端口打开失败", "错误");
			break;
		}

		if (!InitDevice())
		{
			MessageBox("设备环境初始化失败", "错误");
			break;
		}

		m_hEventRun = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_hEventRun == NULL)
			break;

		if (CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)(ReadProc), this, 0, NULL) == INVALID_HANDLE_VALUE)
			break;

		SetEvent(m_hEventRun);

		ret = TRUE;

	} while (0);


	return ret;
}

void CAutomaticTcmDlg::StopDevice()
{
	m_bRunning = FALSE;
	if (m_hEventRun != NULL)
	{
		SetEvent(m_hEventRun);
		CloseHandle(m_hEventRun);
		m_hEventRun = NULL;
	}

	g_Serial.Close();
}

void CAutomaticTcmDlg::BCDTimetoString(LPBYTE pbSrc, BYTE bLenSrc, char * pszTarg)
{
	char szTemp[16] = {0};

	sprintf_s(szTemp, sizeof(szTemp), "%02X%02X-%02X-%02X", pbSrc[0], pbSrc[1], pbSrc[2], pbSrc[3]);
	strcpy(pszTarg, szTemp);

	if (bLenSrc > 4)
	{
		sprintf_s(szTemp, sizeof(szTemp), " %02X:%02X:%02X", pbSrc[4], pbSrc[5], pbSrc[6]);
		strcat_s(pszTarg, bLenSrc * 2 + 6, szTemp);
	}
}

void CAutomaticTcmDlg::DispTicketInf(BOOL bClear, TICKETINFO& tkInf)
{
	if (bClear)
	{
		GetDlgItem(IDC_EDT_PID)->SetWindowText("");
		GetDlgItem(IDC_EDT_LID)->SetWindowText("");
		GetDlgItem(IDC_EDT_BALANCE)->SetWindowText("");
		GetDlgItem(IDC_EDT_MAX)->SetWindowText("");
		GetDlgItem(IDC_EDT_ISSUE)->SetWindowText("");
		GetDlgItem(IDC_EDT_PPERIOD)->SetWindowText("");
		GetDlgItem(IDC_EDT_LPERIOD)->SetWindowText("");
		GetDlgItem(IDC_EDT_STATUS)->SetWindowText("");
		GetDlgItem(IDC_EDT_LAST_DEV)->SetWindowText("");
		GetDlgItem(IDC_EDT_ENTRY_STATION)->SetWindowText("");
		GetDlgItem(IDC_EDT_ENTRY_TM)->SetWindowText("");

		m_ListHis.DeleteAllItems();
	}
	else
	{
		CString strTemp;
		char szTemp1[64] = {0};
		char szTemp2[64] = {0};

		memcpy(szTemp1, tkInf.cPhysicalID, sizeof(tkInf.cPhysicalID));
		GetDlgItem(IDC_EDT_PID)->SetWindowText(szTemp1);

		memcpy(szTemp1, tkInf.cLogicalID, sizeof(tkInf.cLogicalID));
		GetDlgItem(IDC_EDT_LID)->SetWindowText(szTemp1);

		strTemp.Format("%d.%02d", tkInf.lBalance / 100, tkInf.lBalance % 100);
		GetDlgItem(IDC_EDT_BALANCE)->SetWindowText(strTemp);

		strTemp.Format("%d.%02d", tkInf.lLimitedBalance / 100, tkInf.lLimitedBalance % 100);
		GetDlgItem(IDC_EDT_MAX)->SetWindowText(strTemp);

		BCDTimetoString(tkInf.bIssueData, 4, szTemp1);
		GetDlgItem(IDC_EDT_ISSUE)->SetWindowText(szTemp1);

		BCDTimetoString(tkInf.bExpiry, 4, szTemp1);
		GetDlgItem(IDC_EDT_PPERIOD)->SetWindowText(szTemp1);

		BCDTimetoString(tkInf.bStartDate, 7, szTemp1);
		BCDTimetoString(tkInf.bEndDate, 7, szTemp2);
		strTemp.Format("%s~%s", szTemp1, szTemp2);
		GetDlgItem(IDC_EDT_LPERIOD)->SetWindowText(strTemp);

		strTemp.Format("%d", tkInf.bStatus);
		GetDlgItem(IDC_EDT_STATUS)->SetWindowText(strTemp);

		strTemp.Format("%02x%02x/%02x/%x%02x", tkInf.bLastStationID[0], tkInf.bLastStationID[1], 
			tkInf.bLastDeviceType, tkInf.bLastDeviceID[0], tkInf.bLastDeviceID[1]);
		GetDlgItem(IDC_EDT_LAST_DEV)->SetWindowText(strTemp);

		strTemp.Format("%02x%02x", tkInf.bEntrySationID[0], tkInf.bEntrySationID[1]);
		GetDlgItem(IDC_EDT_ENTRY_STATION)->SetWindowText(strTemp);

		BCDTimetoString(tkInf.bEndDate, 7, szTemp1);
		GetDlgItem(IDC_EDT_ENTRY_TM)->SetWindowText(szTemp1);

		for (int nHis=0;nHis<tkInf.bUsefulCount;nHis++)
		{
			SetListCtrlIine(nHis, tkInf.bhs[nHis]);
		}
	}
}

void CAutomaticTcmDlg::SetListCtrlIine(int nItem, HSSVT& his)
{
	CString strTemp;
	char szTemp[64] = {0};

	BCDTimetoString(his.dtDate, 7, szTemp);
	m_ListHis.InsertItem(0, szTemp);

	strTemp.Format("%02x%02x/%02x/%x%02x", 
		his.bStationID[0], his.bStationID[1], his.bDeviceType, his.bDeviceID[0], his.bDeviceID[1]);
	m_ListHis.SetItem(nItem, 1, LVIF_TEXT, strTemp, 0, 0, 0, 0);

	strTemp.Format("%d.%02d", his.lTradeAmount / 100, his.lTradeAmount % 100);
	m_ListHis.SetItem(nItem, 2, LVIF_TEXT, strTemp, 0, 0, 0, 0);

	strTemp.Format("%d", his.bStatus);
	m_ListHis.SetItem(nItem, 3, LVIF_TEXT, strTemp, 0, 0, 0, 0);
}

int CAutomaticTcmDlg::GetTicketInfo(TICKETINFO& tkinf)
{
	// TODO: 在此添加控件通知处理程序代码
	WORD lenInf			= 0;
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		memset(&tkinf, 0, sizeof(TICKETINFO));

		pos += sizeof(RETINFO);

		memcpy(&lenInf, bRecv + pos, 2);
		pos += 2;

		if (lenInf > 0)
		{
			memcpy(&tkinf, bRecv + pos, lenInf);
			pos += lenInf;

			return 0;
		}

	} while (0);

	return -1;
}

void CAutomaticTcmDlg::ReadProc(LPVOID lparam)
{

	TICKETINFO tkInf	= {0};
	char szLastID[20]	= {0};
	BYTE bAnti			= 0;
	CAutomaticTcmDlg * pDlg = (CAutomaticTcmDlg *)lparam;

	while (pDlg->m_bRunning)
	{
		if (WaitForSingleObject(pDlg->m_hEventRun, INFINITE) == WAIT_OBJECT_0)
		{
			if (pDlg->GetTicketInfo(tkInf) == 0)
			{
				if (memcmp(szLastID, tkInf.cLogicalID, 20) != 0)
				{
					pDlg->DispTicketInf(FALSE, tkInf);
					memcpy(szLastID, tkInf.cLogicalID, 20);
				}
			}
			else
			{
				pDlg->DispTicketInf(TRUE, tkInf);
				memset(szLastID, 0, 20);
			}

			SetEvent(pDlg->m_hEventRun);
		}
	}
}