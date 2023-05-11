#pragma once
#include "hexedit.h"
#include "afxwin.h"


// CDlgDebug �Ի���

class CDlgDebug : public CDialog
{
	DECLARE_DYNAMIC(CDlgDebug)

public:
	CDlgDebug(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgDebug();

// �Ի�������
	enum { IDD = IDD_DLG_DEBUG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CHexEdit m_EditClassic;
	CHexEdit m_EditAddr;
	CHexEdit m_EditData;

	afx_msg void OnBnClickedBnSend();
	afx_msg void OnBnClickedBnCancel();
	virtual BOOL OnInitDialog();
	CStatic m_StaticLen;
	afx_msg void OnEnChangeEditData();
	afx_msg void OnEnChangeEditAddr();
};
