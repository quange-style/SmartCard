#pragma once


// CDlgSjtSale 对话框

class CDlgSjtSale : public CDialog
{
	DECLARE_DYNAMIC(CDlgSjtSale)

public:
	CDlgSjtSale(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSjtSale();

// 对话框数据
	enum { IDD = IDD_DLG_SJTSALE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	int m_nAntiMode;

public:
	afx_msg void OnBnClickedBnSale();
	afx_msg void OnBnClickedBnCancel();
	afx_msg void OnCbnSelchangeCombFare();

	void SetAntiMode(int nAntiMode)
	{
		m_nAntiMode = nAntiMode;
	}
};
