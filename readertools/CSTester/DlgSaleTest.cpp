#include "stdafx.h"
#include "DlgSaleTest.h"

#include "CSTester.h"
#include "CSTesterDlg.h"

IMPLEMENT_DYNAMIC(DlgSaleTest, CDialog)

DlgSaleTest::DlgSaleTest(CWnd* pParent /*=NULL*/)
	: CDialog(DlgSaleTest::IDD, pParent)
{

}

DlgSaleTest::~DlgSaleTest()
{
}

BEGIN_MESSAGE_MAP(DlgSaleTest, CDialog)
	ON_BN_CLICKED(IDC_BN_SALES_START, &DlgSaleTest::OnBnClickedBnSales)
	ON_CBN_SELCHANGE(IDC_COMB_SAL_COUNTS_TYPE, &DlgSaleTest::OnCbnSelechangeCombCountsType)

	ON_EN_CHANGE(IDC_EDIT_INPUT_VALUE, &DlgSaleTest::OnEnChangeEditInputValue)
END_MESSAGE_MAP()

void DlgSaleTest::DoDataExchange(CDataExchange* pDX)
{
	//DlgSaleTest::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMB_SAL_COUNTS_TYPE, m_CombCountsType);
	DDX_Control(pDX, IDC_EDIT_INPUT_VALUE, m_sale_count_input);
	//DDX_Control(pDX, IDC_COMB_PAY_TYPE, m_cbPayType);
}



void DlgSaleTest::SetSaleCountsType()
{
	m_CombCountsType.ResetContent();
	m_CombCountsType.AddString("100");
	m_CombCountsType.AddString("1000");
	m_CombCountsType.AddString("2000");
	m_CombCountsType.AddString("5000");
	m_CombCountsType.AddString("10000");
	m_CombCountsType.EnableWindow(TRUE);

	CString test = "100";
	m_sale_count_input.SetWindowTextA(test);

}

void DlgSaleTest::OnBnClickedBnSales() {

	int nPriceArry[4] = { 200, 300, 400, 500 };
	int pos = 0;
	int counts = getCurrentCount();
	QueryPerformanceCounter(&g_liStart);
	for (int index = 0;index < counts;index++)
	{
		pos = index % 4;
		SjtSale(nPriceArry[pos]);
	}
	QueryPerformanceCounter(&g_liEnd);
	CString csFinish = "�������\n";
	((CCSTesterDlg*)AfxGetMainWnd())->DispResult(csFinish, "�������", g_retInfo, g_liStart, g_liEnd);


}
void DlgSaleTest::OnCbnSelechangeCombCountsType() {

}
int DlgSaleTest::getCurrentCount() {
	int count = 0;
	int edit_count = 0;
	int combox_count = 0;
	CString strCombox;
	m_CombCountsType.GetWindowText(strCombox);
	CString strEdit;
	m_sale_count_input.GetWindowText(strEdit);
	//Ĭ���������100�Σ����������Ϊ0 ����������  �������Ϊ0 ����ȡ��Ĳ���
	if (strCombox.GetLength()==0&& strEdit.GetLength() == 0) {

	}else if (strCombox.GetLength()==0) {
		count = _ttoi(strEdit);
	}
	else if (strEdit.GetLength() == 0) {
		count = _ttoi(strCombox);
	}
	else {
		edit_count = _ttoi(strEdit);
		combox_count = _ttoi(strCombox);
		if (edit_count>=combox_count) {
			count = edit_count;
		}
		else if (edit_count < combox_count) {
			count = combox_count;
		}
	}
	return count;
}

void DlgSaleTest::SjtSale(WORD wValue)
{
	SJTSALE data = { 0 };
	WORD len_ss = 0;
	BYTE bData[128] = { 0 };
	BYTE bRecv[1024] = { 0 };
	int pos = 7;
	do
	{
		memcpy(bData, &wValue, 2);
		bData[2] = 0x01;
		bData[3] = 0x02;

		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 20, bData, 4, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		CString cs = "good";
		if (g_retInfo.wErrCode != 0) {
			testCounterRecord.setFailureCount();
			//ʧ�ܼ���  
			cs.Format("SuccessCount:  %ld  TotleCount:	%ld\n ",
				testCounterRecord.getCurrentFailureCount(), testCounterRecord.getCurrentTotleCounts());
			break;
		}
		else {
			//�ɹ�����
			testCounterRecord.setSuccessCount();
			cs.Format("SuccessCount:  %ld  TotleCount:	%ld\n ",
				testCounterRecord.getCurrentSuccessCount(), testCounterRecord.getCurrentTotleCounts());
		}
		((CCSTesterDlg*)AfxGetMainWnd())->DispResult(cs, "����", g_retInfo, g_liStart, g_liEnd);
		pos += sizeof(RETINFO);

		memcpy(&len_ss, bRecv + pos, 2);
		pos += 2;

		if (len_ss > 0)
		{
			memcpy(&data, bRecv + pos, len_ss);
		}

	} while (0);

	//((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_SjtSale, &data, len_ss, "����", g_liStart, g_liEnd);

}


void DlgSaleTest::OnEnChangeEditInputValue()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
