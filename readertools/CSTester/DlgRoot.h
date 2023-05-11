#pragma once
#include "afxwin.h"


// CDlgRoot 对话框

class CDlgRoot : public CDialog
{
	DECLARE_DYNAMIC(CDlgRoot)

public:
	CDlgRoot(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRoot();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_staticNotice;
	CEdit m_editPsw;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	static CString GetCpuID();
	static CString GetCpuID(int nItem);
};
