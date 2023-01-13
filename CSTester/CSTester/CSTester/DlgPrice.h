#pragma once
#include "afxwin.h"


// CDlgPrice 对话框

class CDlgPrice : public CDialog
{
	DECLARE_DYNAMIC(CDlgPrice)

public:
	CDlgPrice(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPrice();

// 对话框数据
	enum { IDD = IDD_DLG_PRICE };

protected:
	CComboBox m_cmbStation;
	CComboBox m_cmbTkType;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnQuery();
	afx_msg void OnBnClickedBnCancel();
	virtual BOOL OnInitDialog();
	void SetStation();
};
