#pragma once
#include "afxwin.h"


// CDataBar 对话框

class CDataBar : public CDialog
{
	DECLARE_DYNAMIC(CDataBar)

public:
	CDataBar(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDataBar();

// 对话框数据
	enum { IDD = IDD_DIALOGBAR };

public:
	BOOL GetSetting(int& nPort, WORD& wStation, WORD& wDevice);

	void ModifySetting(int nPortType, bool bModifyPort, WORD wStation, bool bModifyStation, WORD wDevice, bool bModifyDevice);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
