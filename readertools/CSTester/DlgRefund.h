#pragma once


// CDlgRefund  �Ի���

class CDlgRefund  : public CDialog
{
	DECLARE_DYNAMIC(CDlgRefund )

public:
	CDlgRefund (CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgRefund ();

// �Ի�������
	enum { IDD = IDD_DLG_REFUND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

protected:
	int m_nReturnMode;

public:
	afx_msg void OnBnClickedBnRefund();
	afx_msg void OnBnClickedBnCancel();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio3();
	virtual BOOL OnInitDialog();

	void SetChecker(BOOL bEnableReturnBalance, BOOL bEnableReturnDeposite);
};
