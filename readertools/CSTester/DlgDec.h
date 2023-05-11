#pragma once


// CDlgDec 对话框

class CDlgDec : public CDialog
{
	DECLARE_DYNAMIC(CDlgDec)

public:
	CDlgDec(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDec();

// 对话框数据
	enum { IDD = IDD_DLG_DECR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	CString m_strToDecr;
	CString m_strAfter;

	long m_lOldBalance;
	long m_lNewBalance;
	long m_lDecrease;
	UINT m_uCurEdit;
	int m_nAntiMode;

public:
	void SetOldBalance(long lOldBalance);

	void SetAntiMode(int nAntiMode)
	{
		m_nAntiMode = nAntiMode;
	}

	afx_msg void OnBnClickedBnDecrease();
	afx_msg void OnBnClickedBnCancel();
	afx_msg void OnEnChangeEditTodecr();
	afx_msg void OnEnChangeEditAfter();
	afx_msg void OnEnSetfocusEditTodecr();
	afx_msg void OnEnSetfocusEditAfter();
};
