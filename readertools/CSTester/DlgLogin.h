#pragma once


// CDlgLogin �Ի���

class CDlgLogin : public CDialog
{
	DECLARE_DYNAMIC(CDlgLogin)

public:
	CDlgLogin(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLogin();

// �Ի�������
	enum { IDD = IDD_DLG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_strUserName;
	CString m_StrPassWord;
	static int m_nShiftID;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void GetOperatorInfo(char * pstrOper, int& nShiftID);
};
