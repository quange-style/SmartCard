#pragma once


// CDlgSamAct 对话框

class CDlgSamAct : public CDialog
{
	DECLARE_DYNAMIC(CDlgSamAct)

public:
	CDlgSamAct(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSamAct();

// 对话框数据
	enum { IDD =  IDD_DLG_SAM_ACT};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnSamAct();
	afx_msg void OnBnClickedBnCancel();
	virtual BOOL OnInitDialog();
};
