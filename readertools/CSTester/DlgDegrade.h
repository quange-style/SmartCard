#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


// CDlgDegrade �Ի���

class CDlgDegrade : public CDialog
{
	DECLARE_DYNAMIC(CDlgDegrade)

public:
	CDlgDegrade(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgDegrade();

// �Ի�������
	enum { IDD = IDD_DLG_DEGRADE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	void SetStationList();

public:
	CComboBox m_cmbType;
	CDateTimeCtrl m_dpDate;
	CComboBox m_cmbStation;
	CComboBox m_cmbFlag;

	afx_msg void OnBnClickedBnSet();
	afx_msg void OnBnClickedBnCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
