// AutomaticAgmDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutomaticAgm.h"
#include "AutomaticAgmDlg.h"
#include "PubFuncs.h"
#include "Structs.h"
#include "Serial.h"


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


// CAutomaticAgmDlg 对话框




CAutomaticAgmDlg::CAutomaticAgmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutomaticAgmDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bRunning = FALSE;
	m_nPort = 0;
	m_devStatus = Service_Off;
	memset(&m_tdInf, 0, sizeof(m_tdInf));
	memset(&m_Result, 0, sizeof(m_Result));

	m_hEventFlush = NULL;
	m_hEventRun = NULL;
}

void CAutomaticAgmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cmbType);
	DDX_Control(pDX, IDC_COMBO2, m_cmbPort);
	DDX_Control(pDX, IDC_EDIT_DEVICE, m_edtDevice);
	DDX_Control(pDX, IDC_SET, m_bnSet);
}

BEGIN_MESSAGE_MAP(CAutomaticAgmDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SET, &CAutomaticAgmDlg::OnBnClickedSet)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CAutomaticAgmDlg::OnCbnSelchangeCombo2)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAutomaticAgmDlg 消息处理程序

BOOL CAutomaticAgmDlg::OnInitDialog()
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
	m_edtDevice.SetLimitText(9);
	m_edtDevice.SetWindowText("020106004");

	m_cmbType.SetCurSel(0);

	EnumSerials(m_cmbPort);
	m_nPort = GetReaderPort();

	m_rcFlush.left = 5;
	m_rcFlush.right = 470;
	//m_rcFlush.top = 62;
	m_rcFlush.top = 200;
	m_rcFlush.bottom = 462;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAutomaticAgmDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAutomaticAgmDlg::OnPaint()
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
		CPaintDC dc(this); // 用于绘制的设备上下文
		
		DisplayStatus();

		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CAutomaticAgmDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAutomaticAgmDlg::OnBnClickedSet()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bRunning)
	{
		m_bRunning = StartDevice(m_cmbType.GetCurSel());
		if (m_bRunning)
		{
			m_cmbType.EnableWindow(FALSE);
			m_cmbPort.EnableWindow(FALSE);
			m_edtDevice.EnableWindow(FALSE);
			m_bnSet.SetWindowText("停止设备");
		}
	}
	else
	{
		StopDevice();
		m_bRunning = FALSE;

		m_cmbType.EnableWindow(TRUE);
		m_cmbPort.EnableWindow(TRUE);
		m_edtDevice.EnableWindow(TRUE);
		m_bnSet.SetWindowText("启动设备");
	}
}

BOOL CAutomaticAgmDlg::InitDevice()
{
	BYTE bData[32]		= {0};
	BYTE bRecv[1024]	= {0};
	BOOL ret			= FALSE;
	CString strDev;

	do 
	{
		m_edtDevice.GetWindowText(strDev);
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

BOOL CAutomaticAgmDlg::StartDevice(int nType)
{
	BOOL ret = FALSE;
	int nPortCursel = -1;

	do 
	{
		m_devStatus = Service_Starting;
		InvalidateRect(&m_rcFlush);

		if (m_nPort <= 0)
			break;

		if (g_Serial.Open(m_nPort) != 0)
			break;

		if (!InitDevice())
			break;

		m_hEventRun = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_hEventRun == INVALID_HANDLE_VALUE)
			break;

		if (nType == 0)		// 进闸机
		{
			m_hEventFlush = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (m_hEventFlush == INVALID_HANDLE_VALUE)
				break;

			if (CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)(EntryProc), this, 0, NULL) == INVALID_HANDLE_VALUE)
				break;
		} 
		else				// 出闸机
		{
			m_hEventFlush = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (m_hEventFlush == INVALID_HANDLE_VALUE)
				break;

			if (CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)(ExitProc), this, 0, NULL) == INVALID_HANDLE_VALUE)
				break;
		}

		if (CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)(FlushUiProc), this, 0, NULL) == INVALID_HANDLE_VALUE)
			break;

		SetEvent(m_hEventRun);

		ret = TRUE;

	} while (0);

	if (ret)
		m_devStatus = Service_On;
	else
		m_devStatus = Service_Off;

	InvalidateRect(&m_rcFlush);

	return ret;
}

void CAutomaticAgmDlg::StopDevice()
{
	m_devStatus = Service_Off;
	InvalidateRect(&m_rcFlush);

	m_bRunning = FALSE;
	SetEvent(m_hEventFlush);
	Sleep(50);

	if (m_hEventFlush != NULL)
	{
		CloseHandle(m_hEventFlush);
		m_hEventFlush = NULL;
	}

	if (m_hEventRun != NULL)
	{
		CloseHandle(m_hEventRun);
		m_hEventRun = NULL;
	}

	g_Serial.Close();
}

void CAutomaticAgmDlg::FlushUiProc(LPVOID lparam)
{
	DWORD dwWaitEvent;
	CAutomaticAgmDlg * pDlg = (CAutomaticAgmDlg *)lparam;

	while (pDlg->m_bRunning)
	{
		dwWaitEvent = WaitForSingleObject(pDlg->m_hEventFlush, 3000);

		if (dwWaitEvent == WAIT_TIMEOUT)
		{
			pDlg->m_devStatus = Service_On;
			pDlg->InvalidateRect(&pDlg->m_rcFlush);

		}
		else if (dwWaitEvent == WAIT_OBJECT_0)
		{
			if (pDlg->m_Result.wErrCode != 0)
				pDlg->m_devStatus = Service_Forbid_Pass;
			else if (pDlg->m_cmbType.GetCurSel() == 0)
				pDlg->m_devStatus = Service_Pass_Entry;
			else
				pDlg->m_devStatus = Service_Pass_Exit;

			pDlg->InvalidateRect(&pDlg->m_rcFlush);
		}
	}
}

RETINFO CAutomaticAgmDlg::Entry(PENTRYGATE pEntry)
{
	RETINFO result		= {0};
	WORD leneng			= 0;
	TICKETLOCK tl		= {0};
	WORD lentl			= 0;
	BYTE bRecv[128]		= {0};
	int pos				= 7;

	do 
	{
		result = g_Serial.Communicate(0, 0, 1, 14, NULL, 0, bRecv, sizeof(bRecv));

		pos += sizeof(RETINFO);

		memcpy(&leneng, bRecv + pos, 2);
		pos += 2;

		if (leneng > 0)
		{
			memcpy(pEntry, bRecv + pos, leneng);
			pos += leneng;
		}

		memcpy(&lentl, bRecv + pos, 2);
		pos += 2;

		if (lentl > 0)
		{
			memcpy(&tl, bRecv + pos, lentl);
		}

	} while (0);

	return result;
}

RETINFO CAutomaticAgmDlg::Exit(PPURSETRADE pExit, BYTE bAnti)
{
	RETINFO result		= {0};
	WORD lenps			= 0;
	TICKETLOCK tl		= {0};
	WORD lentl			= 0;
	BYTE bRecv[512]		= {0};
	int pos				= 7;

	do 
	{
		result = g_Serial.Communicate(bAnti, 0, 1, 15, NULL, 0, bRecv, sizeof(bRecv));

		pos += sizeof(RETINFO);

		memcpy(&lenps, bRecv + pos, 2);
		pos += 2;

		if (lenps > 0)
		{
			memcpy(pExit, bRecv + pos, lenps);
			pos += lenps;
		}

		memcpy(&lentl, bRecv + pos, 2);
		pos += 2;

		if (lentl > 0)
		{
			memcpy(&tl, bRecv + pos, lentl);
		}

	} while (0);

	return result;
}

void CAutomaticAgmDlg::EntryProc(LPVOID lparam)
{
	ENTRYGATE eng	= {0};
	CAutomaticAgmDlg * pDlg = (CAutomaticAgmDlg *)lparam;

	while (pDlg->m_bRunning)
	{
		if (WaitForSingleObject(pDlg->m_hEventRun, INFINITE) == WAIT_OBJECT_0)
		{
			pDlg->m_Result = pDlg->Entry(&eng);

			if (pDlg->m_Result.wErrCode != 0x0002)
				SetEvent(pDlg->m_hEventFlush);

			SetEvent(pDlg->m_hEventRun);
		}
	}
}

void CAutomaticAgmDlg::ExitProc(LPVOID lparam)
{
	PURSETRADE exg	= {0};
	BYTE bAnti		= 0;
	CAutomaticAgmDlg * pDlg = (CAutomaticAgmDlg *)lparam;

	while (pDlg->m_bRunning)
	{
		if (WaitForSingleObject(pDlg->m_hEventRun, INFINITE) == WAIT_OBJECT_0)
		{
			pDlg->m_Result = pDlg->Exit(&exg, bAnti);
			bAnti = 1 - bAnti;

			if (pDlg->m_Result.wErrCode != 0x0002)
				SetEvent(pDlg->m_hEventFlush);
	
			SetEvent(pDlg->m_hEventRun);
		}
	}
}

void CAutomaticAgmDlg::OnCbnSelchangeCombo2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nPort = GetReaderPort();
}

int CAutomaticAgmDlg::GetReaderPort()
{
	CString strPort;
	int nPort = -1;
	m_cmbPort.GetWindowText(strPort);
	if (strPort != "")
	{
		strPort = strPort.Right(strPort.GetLength() - 3);
		nPort = atoi(strPort);
	}

	return nPort;
}

int CAutomaticAgmDlg::EnumSerials(CComboBox& comb)
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

void CAutomaticAgmDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	StopDevice();

	CDialog::OnClose();
}

void CAutomaticAgmDlg::DisplayStatus()
{
	static COLORREF corText[3] = {RGB(200, 0, 0), RGB(0, 200, 0), RGB(0, 0, 200),};
	static int nDisCount;

	nDisCount = (nDisCount + 1) % 3;
	COLORREF clr = corText[nDisCount];

	LOGFONT lft;
	CFont fnt;
	CString strText;
	CDC * pCD = GetDC();
	CDC MmDc;

	switch(m_devStatus)
	{
	case Service_Off:
		strText = "服务未启动";
		break;
	case Service_Starting:
		strText = "服务启动中……";
		break;
	case Service_On:
		strText = "请出示车票";
		break;
	case Service_Pass_Entry:
		strText = "请进闸";
		break;
	case Service_Pass_Exit:
		strText = "请出闸";
		break;
	case Service_Forbid_Pass:
		strText.Format("禁止通行：%04X,%02X", m_Result.wErrCode, m_Result.bNoticeCode);
		break;
	}

	strcpy_s(lft.lfFaceName, "微软雅黑"); 
	lft.lfWeight		= 700; 
	lft.lfWidth			= 20; 
	lft.lfHeight		= 50; 
	lft.lfEscapement	= 0; 
	lft.lfUnderline		= FALSE; 
	lft.lfItalic		= FALSE; 
	lft.lfStrikeOut		= FALSE; 
	lft.lfCharSet		= GB2312_CHARSET;
	fnt.CreateFontIndirect(&lft);

	pCD->SetTextColor(clr);									//设置文字的颜色
	pCD->SetBkMode(TRANSPARENT);

	//MmDc.CreateCompatibleDC(pCD);
	//CFont * pfntOld = MmDc.SelectObject(&fnt);
	//MmDc.DrawText(strText, &m_rcFlush, DT_CENTER);			//绘制文字
	//pCD->BitBlt(m_rcFlush.left, m_rcFlush.top, m_rcFlush.right - m_rcFlush.left, m_rcFlush.bottom - m_rcFlush.top, &MmDc, 0, 0, SRCCOPY);
	//MmDc.SelectObject(pfntOld);
	//fnt.DeleteObject();
	//MmDc.DeleteDC();

	CFont * pfntOld = pCD->SelectObject(&fnt);
	pCD->DrawText(strText, &m_rcFlush, DT_EDITCONTROL | DT_CENTER);			//绘制文字
	pCD->SelectObject(pfntOld);
	fnt.DeleteObject();

	ReleaseDC(pCD);
}
