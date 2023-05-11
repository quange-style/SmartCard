// DlgPrice.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgPrice.h"
#include "CSTesterDlg.h"
#include "PubFuncs.h"


// CDlgPrice �Ի���

IMPLEMENT_DYNAMIC(CDlgPrice, CDialog)

CDlgPrice::CDlgPrice(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPrice::IDD, pParent)
{

}

CDlgPrice::~CDlgPrice()
{
}

void CDlgPrice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMB_SATTION, m_cmbStation);
	DDX_Control(pDX, IDC_COMB_TYPE, m_cmbTkType);
}


BEGIN_MESSAGE_MAP(CDlgPrice, CDialog)
	ON_BN_CLICKED(IDC_BN_QUERY, &CDlgPrice::OnBnClickedBnQuery)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CDlgPrice::OnBnClickedBnCancel)
END_MESSAGE_MAP()


// CDlgPrice ��Ϣ�������

void CDlgPrice::OnBnClickedBnQuery()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strTemp;
	BYTE bData[32] = {0};
	BYTE bRecv[256] = {0};
	long lFare = 0;

	m_cmbTkType.GetWindowText(strTemp);
	strTemp = strTemp.Left(4);
	String2HexSeq(strTemp.GetBuffer(), 4, bData, 2);

	m_cmbStation.GetWindowText(strTemp);
	strTemp = strTemp.Left(4);
	String2HexSeq(strTemp.GetBuffer(), 4, bData + 2, 2);

	QueryPerformanceCounter(&g_liStart);
	g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 4, bData, 4, bRecv, sizeof(bRecv));
	QueryPerformanceCounter(&g_liEnd);

	if (g_retInfo.wErrCode == 0)
		memcpy(&lFare, bRecv + 14, 4);

	strTemp.Format("\t��վ<----->%02X%02X, Ʊ������ : %02X%02X, Ʊ�� : %d\n\n", bData[2], bData[3], bData[0], bData[1], lFare);

	((CCSTesterDlg *)AfxGetMainWnd())->DispResult(strTemp, "��ѯƱ��", g_retInfo, g_liStart, g_liEnd);
}

void CDlgPrice::OnBnClickedBnCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ShowWindow(SW_HIDE);
}

BOOL CDlgPrice::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	m_cmbStation.SetDroppedWidth(150);
	m_cmbTkType.SetDroppedWidth(160);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgPrice::SetStation()
{
	CString strStation;
	m_cmbStation.ResetContent();
	for (map<BYTE, LINEINF>::iterator itor=g_PrmStation.begin();itor!=g_PrmStation.end();itor++)
	{
		for (DWORD j=0;j<itor->second.vsta.size();j++)
		{
			strStation.Format("%04X %s", itor->second.vsta[j].first, itor->second.vsta[j].second);
			m_cmbStation.AddString(strStation);
		}
	}
}
