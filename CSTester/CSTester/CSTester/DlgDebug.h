#pragma once
#include "hexedit.h"
#include "afxwin.h"


// CDlgDebug 对话框

class CDlgDebug : public CDialog
{
	DECLARE_DYNAMIC(CDlgDebug)

public:
	CDlgDebug(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDebug();

// 对话框数据
	enum { IDD = IDD_DLG_DEBUG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CHexEdit m_EditClassic;
	CHexEdit m_EditAddr;
	CHexEdit m_EditData;

	afx_msg void OnBnClickedBnSend();
	afx_msg void OnBnClickedBnCancel();
	virtual BOOL OnInitDialog();
	CStatic m_StaticLen;
	afx_msg void OnEnChangeEditData();
};
