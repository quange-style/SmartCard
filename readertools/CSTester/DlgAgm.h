#pragma once
#include "afxwin.h"


// CDlgAgm �Ի���

class CDlgAgm : public CDialog
{
	DECLARE_DYNAMIC(CDlgAgm)

public:
	CDlgAgm(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgAgm();

// �Ի�������
	enum { IDD = IDD_DLG_AGM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnInit();
	afx_msg void OnBnClickedBnEntry();
	afx_msg void OnBnClickedBnExit();
	afx_msg void OnBnClickedBnOpenStart();
	afx_msg void OnBnClickedBnSerialOpen();
	afx_msg LRESULT OnPanalChanged(WPARAM, LPARAM);

protected:

	CComboBox m_cmbDevType;
	CComboBox m_cmbAsyleType;

public:
	virtual BOOL OnInitDialog();
	CComboBox m_cmbAnt;
	afx_msg void OnCbnSelchangeCmbAsyleType();
};
extern ETICKETDEALINFO g_ETicketOnline;