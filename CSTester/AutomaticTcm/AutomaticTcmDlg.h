// AutomaticTcmDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Structs.h"


// CAutomaticTcmDlg 对话框
class CAutomaticTcmDlg : public CDialog
{
// 构造
public:
	CAutomaticTcmDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AUTOMATICTCM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


protected:
	BOOL m_bRunning;
	HANDLE m_hEventRun;
	int m_nPort;

	CListCtrl m_ListHis;
	CEdit m_EdtDev;
	CComboBox m_CmbPort;
	CButton m_BnSet;

protected:
	int EnumSerials(CComboBox& comb);
	int GetReaderPort();
	BOOL InitDevice();
	BOOL StartDevice();
	void StopDevice();

	void BCDTimetoString(LPBYTE pbSrc, BYTE bLenSrc, char * pszTarg);
	void DispTicketInf(BOOL bClear, TICKETINFO& tkInf);
	static void ReadProc(LPVOID lparam);
	void SetListCtrlIine(int nItem, HSSVT& his);
	int GetTicketInfo(TICKETINFO& tkinf);

public:
	afx_msg void OnBnClickedSet();
	afx_msg void OnCbnSelchangeCmbPort();
	afx_msg void OnClose();
};
