#pragma once
#include "afxwin.h"


// CDlgBom 对话框

typedef struct OPER_LOGIN{
	int		nBomShiftID;
	char	szOperID[7];
}OPERLOGIN, * POPERLOGIN;

class CDlgBom : public CDialog
{
	DECLARE_DYNAMIC(CDlgBom)

public:
	CDlgBom(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgBom();

// 对话框数据
	enum { IDD = IDD_DLG_BOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	bool m_bLogin;
	OPERLOGIN m_opl;
	BYTE m_nWorkZone;
	CStatic m_StaticZone;
	HBITMAP m_hbpZoneF;
	HBITMAP m_hbpZoneP;

public:
	afx_msg void OnBnClickedBnLogin();
	afx_msg void OnBnClickedBnInit();
	afx_msg void OnBnClickedBnAnalyse();
	afx_msg void OnBnClickedBnSaleExit();
	afx_msg void OnBnClickedBnSaleSjt();
	afx_msg void OnBnClickedBnSvtDec();
	afx_msg void OnBnClickedBnDestory();
	afx_msg void OnBnClickedBnReturnQR();
	afx_msg void OnBnClickedBnITPAnalyze();
	afx_msg void OnBnScanGetCode();

	afx_msg LRESULT OnPanalChanged(WPARAM, LPARAM);
	afx_msg void OnStnClickedStaticZone();
	afx_msg void OnStnDblclickStaticZone();
	afx_msg void OnBnClickedBnOpen();
	afx_msg void OnBnClickedBnSamActv();
};

extern BOMANALYZE	g_Analyse;
extern RETINFO		g_retInfo;
extern BOMETICANALYZE g_Eticanalyse;