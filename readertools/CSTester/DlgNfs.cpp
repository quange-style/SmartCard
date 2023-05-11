// DlgNfs.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgNfs.h"
#include "CSTesterDlg.h"


// CDlgNfs �Ի���

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


// CDlgNfs ��Ϣ�������

BOOL CDlgNfs::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CCSTesterDlg::EnumSerials(m_cbPort);

	m_edtTime.SetWindowText("60");
	m_edtTime.SetLimitText(3);

	m_edtShareName.SetWindowText("nfs");
	m_edtShareName.SetLimitText(16);

	m_edtSaveFolder.SetWindowText("reader_data");
	m_edtSaveFolder.SetLimitText(16);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgNfs::OnBnClickedGet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
			AfxMessageBox("���ڴ�ʧ��");
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
			AfxMessageBox("�ļ�����ʧ��");
			break;
		}


		AfxMessageBox("�ļ��������");

	} while (0);

	GetDlgItem(ID_GET)->EnableWindow(TRUE);
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);

	g_Serial.Close();
}

void CDlgNfs::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	g_Serial.Close();
	CDialog::OnClose();
}

// δ������API
//BOOL DhcpNotifyConfigChange(LPWSTR lpwszServerName, // ���ػ���ΪNULL
//							LPWSTR lpwszAdapterName, // ����������
//							BOOL bNewIpAddress, // TRUE��ʾ����IP
//							DWORD dwIpIndex, // ָ���ڼ���IP��ַ�����ֻ�иýӿ�ֻ��һ��IP��ַ��Ϊ0
//							DWORD dwIpAddress, // IP��ַ
//							DWORD dwSubNetMask, // ��������
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

	//֪ͨIP��ַ�ı�
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