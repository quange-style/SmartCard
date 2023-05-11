#pragma once
#include "afxwin.h"


// CDataBar �Ի���

class CDataBar : public CDialog
{
	DECLARE_DYNAMIC(CDataBar)

public:
	CDataBar(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDataBar();

// �Ի�������
	enum { IDD = IDD_DIALOGBAR };

public:
	BOOL GetSetting(int& nPort, WORD& wStation, WORD& wDevice);

	void ModifySetting(int nPortType, bool bModifyPort, WORD wStation, bool bModifyStation, WORD wDevice, bool bModifyDevice);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_cmbPort;
	CEdit m_edtStation;
	CEdit m_edtDevice;

	virtual BOOL OnInitDialog();
	CComboBox m_cmbBeep;
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
	afx_msg void OnCbnSelchangeCombBeep();
};
