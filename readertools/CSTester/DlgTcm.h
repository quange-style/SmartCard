#pragma once


// CDlgTcm �Ի���

class CDlgTcm : public CDialog
{
	DECLARE_DYNAMIC(CDlgTcm)

public:
	CDlgTcm(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTcm();

// �Ի�������
	enum { IDD = IDD_DLG_TCM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnInit();
	afx_msg void OnBnClickedBnTkinfo();

	afx_msg LRESULT OnPanalChanged(WPARAM, LPARAM);

	afx_msg void OnBnClickedBnOpen();
	afx_msg void OnBnClickedBnAnalyse();
	afx_msg void OnBnClickedBnAnalyse2();
	afx_msg void OnBnClickedBnEvaluate();
	afx_msg void OnBnClickedBnDestroy();
	afx_msg void OnBnClickedBnRecode();
	afx_msg void OnBnClickedBnPrivate();
	afx_msg void OnBnClickedEsBnAnalyse();
	afx_msg void OnBnClickedEsBnInit();
};
