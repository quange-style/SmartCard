#pragma once


// CDlgDescind �Ի���

class CDlgDescind : public CDialog
{
	DECLARE_DYNAMIC(CDlgDescind)

public:
	CDlgDescind(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgDescind();

// �Ի�������
	enum { IDD = IDD_DLG_DESCIND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnDescind();
	afx_msg void OnBnClickedBnCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
