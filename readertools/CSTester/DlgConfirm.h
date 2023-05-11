#pragma once

#include "CSTesterDlg.h"

// CDlgConfirm �Ի���

class CDlgConfirm : public CDialog
{
	DECLARE_DYNAMIC(CDlgConfirm)

public:
	CDlgConfirm(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgConfirm();

// �Ի�������
	enum { IDD = IDD_DLG_CONFIRM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

protected:
	RETINFO Confirm(BYTE operType);
	void GetConfirmTradeInf(BYTE btradeType, _Struct_Type& type, WORD& len_type);
public:
	afx_msg void OnBnClickedConfirm();
	afx_msg void OnBnClickedUnlockScreen();
};
