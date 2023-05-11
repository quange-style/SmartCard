// DlgNfs.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgNfs.h"
#include "CSTesterDlg.h"


// CDlgNfs 对话框

IMPLEMENT_DYNAMIC(CDlgNfs, CDialog)

CDlgNfs::CDlgNfs(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNfs::IDD, pParent)
{

}

CDlgNfs::~CDlgNfs()
{
}

void CDlgNfs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SHARE_NAME, m_edtShareName);
	DDX_Control(pDX, IDC_EDIT_TIME, m_edtTime);
	DDX_Control(pDX, IDC_COMB_PORT, m_cbPort);
	DDX_Control(pDX, IDC_EDIT_SAVE_FOLDER, m_edtSaveFolder);
}


BEGIN_MESSAGE_MAP(CDlgNfs, CDialog)
	ON_BN_CLICKED(ID_GET, &CDlgNfs::OnBnClickedGet)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgNfs 消息处理程序

BOOL CDlgNfs::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CCSTesterDlg::EnumSerials(m_cbPort);

	m_edtTime.SetWindowText("60");
	m_edtTime.SetLimitText(3);

	m_edtShareName.SetWindowText("nfs");
	m_edtShareName.SetLimitText(16);

	m_edtSaveFolder.SetWindowText("reader_data");
	m_edtSaveFolder.SetLimitText(16);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgNfs::OnBnClickedGet()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTemp;
	BYTE bData[256]		= {0};
	BYTE bRecv[512]		= {0};
	DWORD dwTimeOut		= 0;
	RETINFO ret			= {0};

	do 
	{

		m_cbPort.GetWindowText(strTemp);
		g_nPortOpen = atoi(strTemp.Right(strTemp.GetLength() - 3));
		g_retInfo.wErrCode = g_Serial.Open(g_nPortOpen);
		if (g_retInfo.wErrCode != 0)
		{
			AfxMessageBox("串口打开失败");
			break;
		}

		GetDlgItem(ID_GET)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

		m_edtShareName.GetWindowText(strTemp);
		memcpy(bData, strTemp.GetBuffer(), strTemp.GetLength());

		m_edtSaveFolder.GetWindowText(strTemp);
		memcpy(bData + 16, strTemp.GetBuffer(), strTemp.GetLength());

		ret = g_Serial.Communicate(0, 0, 0x10, 0xff, bData, 32, bRecv, sizeof(bRecv));
		if (ret.wErrCode != 0)
		{
			AfxMessageBox("文件传输失败");
			break;
		}


		AfxMessageBox("文件传输完成");

	} while (0);

	GetDlgItem(ID_GET)->EnableWindow(TRUE);
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);

	g_Serial.Close();
}

void CDlgNfs::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	g_Serial.Close();
	CDialog::OnClose();
}

// 未公开的API
//BOOL DhcpNotifyConfigChange(LPWSTR lpwszServerName, // 本地机器为NULL
//							LPWSTR lpwszAdapterName, // 适配器名称
//							BOOL bNewIpAddress, // TRUE表示更改IP
//							DWORD dwIpIndex, // 指明第几个IP地址，如果只有该接口只有一个IP地址则为0
//							DWORD dwIpAddress, // IP地址
//							DWORD dwSubNetMask, // 子网掩码
//							int nDhcpAction )

typedef BOOL (*DHCPNOTIFYPROC)(LPWSTR,LPWSTR,BOOL,DWORD,DWORD,DWORD,int);


BOOL SetIP(LPCTSTR lpszAdapterName, int nIndex, LPCTSTR pIPAddress, LPCTSTR pNetMask, LPCTSTR pNetGate)
{
	HKEY hKey;
	CString strKeyName = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
	strKeyName += lpszAdapterName;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,	strKeyName,	0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
		return FALSE;

	char mszIPAddress[100];
	char mszNetMask[100];
	char mszNetGate[100];

	strncpy(mszIPAddress, pIPAddress, 98);
	strncpy(mszNetMask, pNetMask, 98);
	strncpy(mszNetGate, pNetGate, 98);

	int nIP, nMask, nGate;

	nIP = (int)strlen(mszIPAddress);
	nMask = (int)strlen(mszNetMask);
	nGate = (int)strlen(mszNetGate);

	*(mszIPAddress + nIP + 1) = 0x00;
	nIP += 2;

	*(mszNetMask + nMask + 1) = 0x00;
	nMask += 2;

	*(mszNetGate + nGate + 1) = 0x00;
	nGate += 2;

	RegSetValueEx(hKey, "IPAddress", 0, REG_MULTI_SZ, (unsigned char*)mszIPAddress, nIP);
	RegSetValueEx(hKey, "SubnetMask", 0, REG_MULTI_SZ, (unsigned char*)mszNetMask, nMask);
	RegSetValueEx(hKey, "DefaultGateway", 0, REG_MULTI_SZ, (unsigned char*)mszNetGate, nGate);

	RegCloseKey(hKey);

	//通知IP地址改变
	BOOL	 bResult = FALSE;
	HINSTANCE	 hDhcpDll;
	DHCPNOTIFYPROC	pDhcpNotifyProc;
	WCHAR wcAdapterName[256];

	MultiByteToWideChar(CP_ACP, 0, lpszAdapterName, -1, wcAdapterName,256);

	if((hDhcpDll = LoadLibrary("dhcpcsvc")) == NULL)
		return FALSE;

	if((pDhcpNotifyProc = (DHCPNOTIFYPROC)GetProcAddress(hDhcpDll, "DhcpNotifyConfigChange")) != NULL)
		if((pDhcpNotifyProc)(NULL, wcAdapterName, TRUE, nIndex, inet_addr(pIPAddress), inet_addr(pNetMask), 0) == ERROR_SUCCESS)
			bResult = TRUE;

	FreeLibrary(hDhcpDll);

	return TRUE;
}