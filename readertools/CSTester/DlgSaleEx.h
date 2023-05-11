#pragma once
#include "afxwin.h"


// CDlgSaleEx �Ի���

class CDlgSaleEx : public CDialog
{
	DECLARE_DYNAMIC(CDlgSaleEx)

public:
	CDlgSaleEx(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSaleEx();

// �Ի�������
	enum { IDD = IDD_DLG_SALE_EX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnSale();
	afx_msg void OnBnClickedBnCancel();
	afx_msg void OnCbnSelchangeCombType();
	afx_msg void OnCbnSelchangeCombCode();
	virtual BOOL OnInitDialog();

protected:
	CComboBox m_cmbTkType;
	CComboBox m_cmbCode;

	void QueryPolicyPenalty();
};
