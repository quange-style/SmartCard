// DlgDegrade.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgDegrade.h"
#include "CSTesterDlg.h"


// CDlgDegrade �Ի���

IMPLEMENT_DYNAMIC(CDlgDegrade, CDialog)

CDlgDegrade::CDlgDegrade(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDegrade::IDD, pParent)
{

}

CDlgDegrade::~CDlgDegrade()
{
}

void CDlgDegrade::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MODE_TYPE, m_cmbType);
	DDX_Control(pDX, IDC_DP_DATE, m_dpDate);
	DDX_Control(pDX, IDC_MODE_STATION, m_cmbStation);
	DDX_Control(pDX, IDC_MODE_FLAG, m_cmbFlag);
}


BEGIN_MESSAGE_MAP(CDlgDegrade, CDialog)
	ON_BN_CLICKED(IDC_BN_SET, &CDlgDegrade::OnBnClickedBnSet)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CDlgDegrade::OnBnClickedBnCancel)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDlgDegrade ��Ϣ�������

void CDlgDegrade::OnBnClickedBnSet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BYTE bSend[64]	= {0};
	BYTE bRecv[64]	= {0};

	CString strTemp;

	m_cmbType.GetWindowText(strTemp);
	bSend[0] = (BYTE)atoi(strTemp.Left(1));

	SYSTEMTIME tmDegrade = {0};
	m_dpDate.GetTime(&tmDegrade);
	Time_Bcd_Ex_Sys(bSend + 1, &tmDegrade, false);

	m_cmbStation.GetWindowText(strTemp);
	String2HexSeq(strTemp.GetBuffer(), 4, bSend + 8, 2);

	m_cmbFlag.GetWindowText(strTemp);
	bSend[10] = (BYTE)atoi(strTemp.Left(1));

	QueryPerformanceCounter(&g_liStart);
	g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 3, bSend, 11, bRecv, sizeof(bRecv));
	QueryPerformanceCounter(&g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Null, NULL, 0, "���ý���ģʽ", g_liStart, g_liEnd);
}

void CDlgDegrade::OnBnClickedBnCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ShowWindow(SW_HIDE);
}

void CDlgDegrade::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: �ڴ˴������Ϣ����������
	if (bShow)
	{
		m_cmbType.SetDroppedWidth(120);
		m_cmbType.SetCurSel(0);

		SetStationList();
		m_cmbStation.SetDroppedWidth(150);
		m_cmbStation.SetCurSel(0);

		m_cmbFlag.SetCurSel(0);

		m_dpDate.SetFormat(_T("yyyy-MM-dd HH:mm:ss"));
		CTime TimeTemp = TimeTemp.GetCurrentTime();
		m_dpDate.SetTime(&TimeTemp);

	}
}

void CDlgDegrade::SetStationList()
{
	CString strItem;

	m_cmbStation.ResetContent();
	for (map<BYTE, LINEINF>::iterator itor=g_PrmStation.begin();itor!=g_PrmStation.end();itor++)
	{
		for (size_t sta_cnt=0;sta_cnt<itor->second.vsta.size();sta_cnt++)
		{
			strItem.Format("%04X %s", itor->second.vsta[sta_cnt].first, itor->second.vsta[sta_cnt].second);
			m_cmbStation.AddString(strItem);
		}
	}
}
