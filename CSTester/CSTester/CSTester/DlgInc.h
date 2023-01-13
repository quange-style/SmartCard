#pragma once


// CDlgInc 对话框

class CDlgInc : public CDialog
{
	DECLARE_DYNAMIC(CDlgInc)

public:
	CDlgInc(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgInc();

// 对话框数据
	enum { IDD = IDD_DLG_INCR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	long m_lOldBalance;
	long m_lNewBalance;
	long m_lIncrease;
	UINT m_uCurEdit;
	int m_nAntiMode;

public:
	void SetOldBalance(long lOldBalance);

	void SetAntiMode(int nAntiMode)
	{
		m_nAntiMode = nAntiMode;
	}

	afx_msg void OnBnClickedBnIncrease();
	afx_msg void OnBnClickedBnCancel();
	afx_msg void OnEnChangeEditToincr();
	afx_msg void OnEnChangeEditAfter();
	afx_msg void OnEnSetfocusEditToincr();
	afx_msg void OnEnSetfocusEditAfter();
	CString m_strToIncr;
	CString m_strAfter;
	virtual BOOL OnInitDialog();
};
