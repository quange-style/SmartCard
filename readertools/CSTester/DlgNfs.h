#pragma once
#include "afxwin.h"


// CDlgNfs �Ի���

class CDlgNfs : public CDialog
{
	DECLARE_DYNAMIC(CDlgNfs)

public:
	CDlgNfs(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgNfs();

// �Ի�������
	enum { IDD = IDD_DLG_GET_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edtShareName;
	CEdit m_edtTime;
	CComboBox m_cbPort;
	afx_msg void OnBnClickedGet();
	virtual BOOL OnInitDialog();
	CEdit m_edtSaveFolder;
	afx_msg void OnClose();
};
