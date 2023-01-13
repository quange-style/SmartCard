#pragma once
#include "afxwin.h"


// CDlgNfs 对话框

class CDlgNfs : public CDialog
{
	DECLARE_DYNAMIC(CDlgNfs)

public:
	CDlgNfs(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgNfs();

// 对话框数据
	enum { IDD = IDD_DLG_GET_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edtShareName;
	CEdit m_edtTime;
	CComboBox m_cbPort;
	afx_msg void OnBnClickedGet();
	virtual BOOL OnInitDialog();
	CEdit m_edtSaveFolder;
	afx_msg void OnClose();
};
