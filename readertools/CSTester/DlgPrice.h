#pragma once
#include "afxwin.h"


// CDlgPrice �Ի���

class CDlgPrice : public CDialog
{
	DECLARE_DYNAMIC(CDlgPrice)

public:
	CDlgPrice(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgPrice();

// �Ի�������
	enum { IDD = IDD_DLG_PRICE };

protected:
	CComboBox m_cmbStation;
	CComboBox m_cmbTkType;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnQuery();
	afx_msg void OnBnClickedBnCancel();
	virtual BOOL OnInitDialog();
	void SetStation();
};
