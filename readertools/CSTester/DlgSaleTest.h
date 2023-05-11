#pragma once
#include "afxwin.h"
#include "resource.h"
#include "TestCounterRecord.h"

using namespace std;

#define WM_UPDATE_TRICE		(WM_USER + 200)

class DlgSaleTest : public CDialog
{
public:
	DECLARE_DYNAMIC(DlgSaleTest)

	DlgSaleTest(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgSaleTest();
	// �Ի�������
	enum { IDD = IDD_DLG_SALE_TEST };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()

	CComboBox m_CombCountsType;
	CEdit m_sale_count_input;

	RETINFO m_RetUptate;

	// �����ֶ��������
	void SetSaleCountInput(WORD wErr, BYTE& bUpdateType);
	
public:

	afx_msg void OnBnClickedBnSales();
	afx_msg void OnCbnSelechangeCombCountsType();
	void SetSaleCountsType();

private:
	TestCounterRecord testCounterRecord;
	//���ô�����������
	
	void SjtSale(WORD wValue);
	int getCurrentCount();

public:
	afx_msg void OnEnChangeEditInputValue();
};

