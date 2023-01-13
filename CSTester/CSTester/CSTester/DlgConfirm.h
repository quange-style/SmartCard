#pragma once

#include "CSTesterDlg.h"

// CDlgConfirm 对话框

class CDlgConfirm : public CDialog
{
	DECLARE_DYNAMIC(CDlgConfirm)

public:
	CDlgConfirm(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgConfirm();

// 对话框数据
	enum { IDD = IDD_DLG_CONFIRM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	RETINFO Confirm(BYTE operType);
	void GetConfirmTradeInf(BYTE btradeType, _Struct_Type& type, WORD& len_type);
public:
	afx_msg void OnBnClickedConfirm();
	afx_msg void OnBnClickedUnlockScreen();
};
