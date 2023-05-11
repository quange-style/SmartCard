#pragma once


// CDlgParam �Ի���

class CDlgParam : public CDialog
{
	DECLARE_DYNAMIC(CDlgParam)

public:
	CDlgParam(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgParam();

// �Ի�������
	enum { IDD = IDD_DLG_PARAM };

protected:
	HANDLE m_hEventFinish;

	static void UpdateProc(LPVOID lparam);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BOOL IsValidParam(LPCTSTR lpszName, int& nType);
	LPBYTE PrepareSendParam(CString strPath, DWORD& dwSize, DWORD& dwCRC32);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnCur();
	afx_msg void OnBnClickedBnCfg();
	afx_msg void OnBnClickedBnCfg2();
	afx_msg void OnBnClickedBnCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBnStation();
public:
	afx_msg void OnBnClickedBnCur2();
};
