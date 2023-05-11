#include "stdafx.h"
#include "DlgAgmOpen.h"
#include "CSTester.h"
#include "CSTesterDlg.h"

IMPLEMENT_DYNAMIC(DlgAgmOpen, CDialog)

DlgAgmOpen::DlgAgmOpen(CWnd* pParent /*=NULL*/)
	: CDialog(DlgAgmOpen::IDD, pParent)
{

}

DlgAgmOpen::~DlgAgmOpen()
{
}

BEGIN_MESSAGE_MAP(DlgAgmOpen, CDialog)
	ON_BN_CLICKED(IDC_BN_SALES_START, &DlgAgmOpen::OnBnClickedAgmOpen)
	ON_BN_CLICKED(IDC_BN_AGM_OPEN_START, &DlgAgmOpen::OnBnClickedBnAgmOpenStart)
END_MESSAGE_MAP()

void DlgAgmOpen::DoDataExchange(CDataExchange* pDX)
{
	//DlgSaleTest::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMB_AGM_OPEN_RESULT_TYPE, m_CombAgmOpenResultType);
	DDX_Control(pDX, IDC_EDIT_INPUT_VALUE, m_agm_center_num_input);
	//DDX_Control(pDX, IDC_COMB_PAY_TYPE, m_cbPayType);
}



void DlgAgmOpen::SeResultType()
{
	m_CombAgmOpenResultType.ResetContent();
	m_CombAgmOpenResultType.AddString("0-验证成功");
	m_CombAgmOpenResultType.AddString("1-验证失败");
	m_CombAgmOpenResultType.SetCurSel(0);
	m_CombAgmOpenResultType.EnableWindow(TRUE);
}

void DlgAgmOpen::OnBnClickedAgmOpen() {
	// TODO: 在此添加控件通知处理程序代码

}

void DlgAgmOpen::openStart(ETICKETDEALINFO g_ETicketOnline)
{	
	CString strTemp;
	
	SeResultType();
	strTemp.Format("%s", g_ETicketOnline.cCenterCode);
	GetDlgItem(IDC_EDIT_INPUT_VALUE)->SetWindowText(strTemp);
}





void DlgAgmOpen::OnBnClickedBnAgmOpenStart()
{
	// TODO: 在此添加控件通知处理程序代码
	BYTE bData[33] = { 0 };
	BYTE bRecv[512] = { 0 };
	BYTE bRfu[2] = { 0 };
	uint16_t lenetg = 0;
	char zOnlineResult = 0;
	CString strTemp;
	ETICKETDEALINFO edg = { 0 };
	do
	{
		memcpy(bData, g_ETicketOnline.cCenterCode, 32);
		memset(&edg, 0, sizeof(edg));
		m_CombAgmOpenResultType.GetWindowText(strTemp);
		zOnlineResult = atoi(strTemp.Left(1));

		bData[32] = zOnlineResult;

		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 16, bData, 33, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		memcpy(&lenetg, bRecv + 12, 2);
		if (lenetg > 0)
		{
			memcpy(&edg, (bRecv + 14), sizeof(edg));
		}
		
	} while (0);

		((CCSTesterDlg*)AfxGetMainWnd())->DispResult(struct_ETDealInfo, &edg, lenetg, "电子车票交易信息", g_liStart, g_liEnd);

}
