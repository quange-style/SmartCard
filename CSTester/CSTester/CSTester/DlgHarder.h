#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDlgHarder 对话框

class CDlgHarder : public CDialog
{
	DECLARE_DYNAMIC(CDlgHarder)

public:
	CDlgHarder(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgHarder();

// 对话框数据
	enum { IDD = IDD_DLG_TEST_HARD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cmbAntType[2];
	CButton m_bnAnt[2];
	CButton m_bnSam[8];
	afx_msg void OnBnClickedBnStart();
	afx_msg void OnBnClickedBnNext();
	afx_msg void OnBnClickedBnCancel();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBnRefresh();
	afx_msg LRESULT OnEndCheck(WPARAM, LPARAM);

private:
	static BOOL m_bEndThread;
	static BOOL m_bEndCheck;
	static BOOL m_bJumpToNext;
	static HANDLE m_hWaitEvent;
	static HANDLE m_hCloseEvent;

	static void HardCheckPro(LPVOID lparam);

	BOOL CheckSamSock(BYTE nSockID, BYTE nBaudID);

	BOOL CheckAnt(BYTE antMode, BYTE antType);

	static void AppendText(CRichEditCtrl& edt, CString strText, COLORREF cor = RGB(10, 200, 10));

	RETINFO RunCommand(CMDINF& cmdinf, BYTE ant, BYTE beep);

	RETINFO SamBaud(BYTE nSockID, BYTE nBaudID);

	RETINFO SamRst(BYTE nSockID);

public:
	CComboBox m_cmbSer;
	virtual BOOL OnInitDialog();
	CRichEditCtrl m_rEditDisp;
	CComboBox m_cmbBaud;
};
