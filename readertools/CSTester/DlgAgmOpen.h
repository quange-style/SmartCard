#pragma once
#include "afxwin.h"
#include "resource.h"
#include "TestCounterRecord.h"
#include "DlgAgm.h"
using namespace std;
#define WM_UPDATE_TRICE		(WM_USER + 200)
class DlgAgmOpen : public CDialog
{

public:
	DECLARE_DYNAMIC(DlgAgmOpen)
	void openStart(ETICKETDEALINFO g_ETicketOnline);
	DlgAgmOpen(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgAgmOpen();
	// 对话框数据
	enum { IDD = IDD_DLG_AGM_OPEN };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

	CComboBox m_CombAgmOpenResultType;
	CEdit m_agm_center_num_input;


public:
	
	afx_msg void OnBnClickedAgmOpen();
	void SeResultType();//设置验票结果类型

private:
	//设置次数类型类型



public:
	afx_msg void OnBnClickedBnAgmOpenStart();
};

