#pragma once


// CDlgDescind 对话框

class CDlgDescind : public CDialog
{
	DECLARE_DYNAMIC(CDlgDescind)

public:
	CDlgDescind(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDescind();

// 对话框数据
	enum { IDD = IDD_DLG_DESCIND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnDescind();
	afx_msg void OnBnClickedBnCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
