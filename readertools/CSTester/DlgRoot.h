#pragma once
#include "afxwin.h"


// CDlgRoot �Ի���

class CDlgRoot : public CDialog
{
	DECLARE_DYNAMIC(CDlgRoot)

public:
	CDlgRoot(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgRoot();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_staticNotice;
	CEdit m_editPsw;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	static CString GetCpuID();
	static CString GetCpuID(int nItem);
};
