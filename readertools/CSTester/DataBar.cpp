// DataBar.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CSTester.h"
#include "DataBar.h"
#include "CSTesterDlg.h"

// CDataBar �Ի���

IMPLEMENT_DYNAMIC(CDataBar, CDialog)

CDataBar::CDataBar(CWnd* pParent /*=NULL*/)
	: CDialog(CDataBar::IDD, pParent)
{

}

CDataBar::~CDataBar()
{
}

void CDataBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMB_PORT, m_cmbPort);
	DDX_Control(pDX, IDC_EDIT_STATION, m_edtStation);
	DDX_Control(pDX, IDC_EDIT_DEVICE, m_edtDevice);
	DDX_Control(pDX, IDC_COMB_BEEP, m_cmbBeep);
}

BOOL CDataBar::GetSetting(int& nPort, WORD& wStation, WORD& wDevice)
{
	CString str;

	GetDlgItemText(IDC_COMB_PORT, str);
	str = str.Right(str.GetLength() - 3);
	nPort = atoi(str);

	m_edtStation.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		MessageBox("��ǰվ�㲻��Ϊ��");
		return FALSE;
	}
	wStation = (WORD)atoi(str);

	m_edtDevice.GetWindowText(str);
	if (str.GetLength() <= 0)
	{
		MessageBox("��ǰ�豸����Ϊ��");
		return FALSE;
	}
	wDevice = (WORD)atoi(str);

	return TRUE;
}

void CDataBar::ModifySetting(int nPortType, bool bModifyPort, WORD wStation, bool bModifyStation, WORD wDevice, bool bModifyDevice)
{
	CString strTemp;
	int nCursel = nPortType - 1;

	if (bModifyPort)
	{
		if (nPortType == 0xFF)
			nCursel = 3;

		m_cmbPort.SetCurSel(nCursel);
	}

	if (bModifyStation)
	{
		strTemp.Format("%04d", wStation);
		m_edtStation.SetWindowText(strTemp);
	}

	if (bModifyDevice)
	{
		strTemp.Format("%04d", wDevice);
		m_edtDevice.SetWindowText(strTemp);
	}
}

BEGIN_MESSAGE_MAP(CDataBar, CDialog)
	ON_WM_DEVICECHANGE()
	ON_CBN_SELCHANGE(IDC_COMB_BEEP, &CDataBar::OnCbnSelchangeCombBeep)
END_MESSAGE_MAP()


// CDataBar ��Ϣ�������
BOOL CDataBar::OnDeviceChange(UINT nEventType,DWORD dwData)
{
	CString strCurrent;
	m_cmbPort.GetWindowText(strCurrent);
	CCSTesterDlg::EnumSerials(m_cmbPort);

	if (strCurrent != "")
	{
		m_cmbPort.SetCurSel(m_cmbPort.FindString(0, strCurrent));
	}

	return TRUE;
}
BOOL CDataBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CCSTesterDlg::EnumSerials(m_cmbPort);

	m_edtStation.SetLimitText(4);
	m_edtDevice.SetLimitText(4);

	m_edtStation.SetWindowText("0201");
	m_edtDevice.SetWindowText("004");

	m_cmbBeep.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDataBar::OnCbnSelchangeCombBeep()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_nBeep = m_cmbBeep.GetCurSel();
}
