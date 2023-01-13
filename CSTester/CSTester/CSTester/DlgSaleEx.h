#pragma once
#include "afxwin.h"


// CDlgSaleEx 对话框

class CDlgSaleEx : public CDialog
{
	DECLARE_DYNAMIC(CDlgSaleEx)

public:
	CDlgSaleEx(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSaleEx();

// 对话框数据
	enum { IDD = IDD_DLG_SALE_EX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
