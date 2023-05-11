// AutomaticAgmDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "Structs.h"

typedef enum
{
	Service_Off,				// ����δ����
	Service_Starting,			// ����������
	Service_On,					// ���ʾ��Ʊ
	Service_Pass_Entry,			// ��ͨ��
	Service_Pass_Exit,			// ��ͨ��
	Service_Forbid_Pass			// ��ֹͨ��
}DEVSTATUS;

typedef union
{
	ENTRYGATE	EntryInf;
	PURSETRADE	ExitInf;
}TRADEINF;

// CAutomaticAgmDlg �Ի���
class CAutomaticAgmDlg : public CDialog
{
// ����
public:
	CAutomaticAgmDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_AUTOMATICAGM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
