#pragma once


// CDlgSjtSale �Ի���

class CDlgSjtSale : public CDialog
{
	DECLARE_DYNAMIC(CDlgSjtSale)

public:
	CDlgSjtSale(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSjtSale();

// �Ի�������
	enum { IDD = IDD_DLG_SJTSALE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

protected:
	int m_nAntiMode;

public:
	afx_msg void OnBnClickedBnSale();
	afx_msg void OnBnClickedBnCancel();
	afx_msg void OnCbnSelchangeCombFare();

	void SetAntiMode(int nAntiMode)
	{
		m_nAntiMode = nAntiMode;
	}
};
