#pragma once


// DlgAvm 对话框

class CDlgAvm : public CDialog
{
	DECLARE_DYNAMIC(CDlgAvm)

public:
	CDlgAvm(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAvm();

// 对话框数据
	enum { IDD = IDD_DLG_AVM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
//public:
	//afx_msg void OnBnClickedBtnCom();
public:
	afx_msg void OnBnClickedBnOpen();
public:
	afx_msg void OnBnClickedBnInit();
public:
	afx_msg void OnBnClickedBnAnalyse();
	afx_msg LRESULT OnPanalChanged(WPARAM, LPARAM);
public:
	afx_msg void OnBnClickedBnTkinfo();
	CComboBox m_cmbAnt;
	virtual BOOL OnInitDialog();
};
