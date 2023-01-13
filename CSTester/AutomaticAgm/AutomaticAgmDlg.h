// AutomaticAgmDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "Structs.h"

typedef enum
{
	Service_Off,				// 服务未启动
	Service_Starting,			// 服务启动中
	Service_On,					// 请出示车票
	Service_Pass_Entry,			// 请通过
	Service_Pass_Exit,			// 请通过
	Service_Forbid_Pass			// 静止通行
}DEVSTATUS;

typedef union
{
	ENTRYGATE	EntryInf;
	PURSETRADE	ExitInf;
}TRADEINF;

// CAutomaticAgmDlg 对话框
class CAutomaticAgmDlg : public CDialog
{
// 构造
public:
	CAutomaticAgmDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AUTOMATICAGM_DIALOG };

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
public:
	afx_msg void OnBnClickedSet();

protected:
	BOOL m_bRunning;
	CComboBox m_cmbType;
	CComboBox m_cmbPort;
	int m_nPort;
	CEdit m_edtDevice;
	CButton m_bnSet;
	HANDLE m_hEventFlush;
	HANDLE m_hEventRun;
	RECT m_rcFlush;

	DEVSTATUS m_devStatus;
	TRADEINF m_tdInf;
	RETINFO m_Result;

	BOOL InitDevice();
	BOOL StartDevice(int nType);
	void StopDevice();
	int EnumSerials(CComboBox& comb);

	RETINFO Entry(PENTRYGATE pEntry);
	RETINFO Exit(PPURSETRADE pExit, BYTE bAnti);

	void DisplayStatus();

	static void FlushUiProc(LPVOID lparam);
	static void EntryProc(LPVOID lparam);
	static void ExitProc(LPVOID lparam);

	int GetReaderPort();
public:
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnClose();
};
