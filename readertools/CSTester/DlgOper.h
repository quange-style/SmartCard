#pragma once
#include "afxwin.h"


// CDlgOper �Ի���

class CDlgOper : public CDialog
{
	DECLARE_DYNAMIC(CDlgOper)

public:
	CDlgOper(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgOper();

// �Ի�������
	enum { IDD = IDD_DLG_OPER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnUpdate();
	afx_msg void OnBnClickedBnSale();
	afx_msg void OnBnClickedBnInc();
	afx_msg void OnBnClickedBnRefund();
	afx_msg void OnBnClickedBnUnlock();
	afx_msg void OnBnClickedBnDeffer();
	afx_msg LRESULT OnAnalyse(WPARAM, LPARAM);
	afx_msg void OnPaint();

protected:
	DWORD m_dwOperStatus;
	CString m_strAnalyseErr;
	void EnableButtons(DWORD dwOperStatus);
	void DispAnalyseErr(CString str);
public:
	afx_msg void OnBnClickedBnRead();
	afx_msg void OnBnClickedBnDescind();
	afx_msg void OnBnClickedChkForbbid();
	BOOL m_bForbid;
	CButton m_bnVislbe;
};
