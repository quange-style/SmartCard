// CSTesterDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "DlgAgm.h"
#include "DlgBom.h"
#include "DlgTcm.h"
#include "DlgTvm.h"
#include "DlgCmd.h"
#include "DlgOper.h"
#include "DlgInc.h"
#include "DlgDec.h"
#include "DlgSjtSale.h"
#include "DlgRefund.h"
#include "DlgUpd.h"
#include "DataBar.h"
#include "DlgParam.h"
#include "DlgPrice.h"
#include "DlgSamAct.h"
#include "DlgDescind.h"
#include "DlgDebug.h"
#include "DlgDegrade.h"
#include "DlgSaleEx.h"


// 自定义消息
#define WM_ANALYSE		(WM_USER + 100)
#define WM_DECREASE		(WM_USER + 101)
#define WM_INCREASE		(WM_USER + 102)
#define WM_UPDATE		(WM_USER + 103)
#define WM_REFUND		(WM_USER + 104)
#define WM_SJTSALE		(WM_USER + 105)
#define WM_INITREADER	(WM_USER + 106)
#define WM_UNINITREADER	(WM_USER + 107)
#define WM_CHANGE_ANT	(WM_USER + 108)
#define WM_SAMACT		(WM_USER + 109)
#define WM_PAGECHANGED	(WM_USER + 110)
#define WM_ENDCHECK		(WM_USER + 111)
#define WM_DESCIND		(WM_USER + 112)
#define WM_SALEEX		(WM_USER + 113)

// 交易结构类型
enum _Struct_Type{
	struct_Null,		// 不做格式化时采用
	struct_init_arr,
	struct_Login,		// 自定义结构
	struct_Ver,
	struct_Analyse,
	struct_Sam,
	struct_Purse,
	struct_Update,
	struct_SvtSale,
	struct_SjtSale,
	struct_Deffer,
	struct_Lock,
	struct_Refund,
	struct_History,
	struct_TkInfo,
	struct_Entry,
	struct_ES_Analyse,
	struct_ETicket,
	struct_GetETicket
};

typedef struct
{
	CString						lineName;
	vector<pair<WORD, CString>>	vsta;
}LINEINF, * PLINEINF;

extern map<BYTE, LINEINF> g_PrmStation;

// CCSTesterDlg 对话框
class CCSTesterDlg : public CDialog
{
// 构造
public:
	CCSTesterDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 枚举可用的串口号
	static int EnumSerials(CComboBox& comb);

// 对话框数据
	enum { IDD = IDD_CSTESTER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	BOOL AddBars();
	BOOL AddToolBar(CRect& rcToolBar);
	void AddTbBtn(TBBUTTON& tb, int iBitmap, int iCmd, BYTE fsState, BYTE fsStyle, LPCTSTR pText);
	BOOL AddStatusBar();
	void AddBitmap(CImageList& imgList, UINT uID);

	void ChildWindowAnchor();
	virtual BOOL OnInitDialog();

	void LoadErrorString(CString strPath);


	// 卸载车站表
	void UnLoadStationTable();

	// 显示“通讯方式”菜单前的小钩
	void SetSubMenuCheck(int nSubPos, UINT uTargID);

	// 获取线路名称
	CString GetLineName(BYTE lineID);

#define DLG_OPER		0
#define DLG_INC			1
#define DLG_DEC			2
#define DLG_SSALE		3
#define DLG_RFD			4
#define DLG_UPD			5
#define DLG_CORE		6
#define DLG_SAM			7
#define DLG_PRICE		8
#define DLG_PARAM		9
#define DLG_SAM_ACT		10
#define DLG_DESCIND		11
#define DLG_DEBUG		12
#define DLG_DEGRADE		13
#define DLG_SALEEX		14

	void ShowDialogs(DWORD flag);

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	CDlgBom * m_pBom;
	CDlgAgm * m_pAgm;
	CDlgTvm * m_pTvm;
	CDlgTcm * m_pTcm;
	CDialog * m_pPageLast;
	CDlgOper * m_pOper;
	CDlgInc	* m_pInc;
	CDlgDec * m_pDec;
	CDlgSjtSale * m_pSjtSale;
	CDlgRefund * m_pRefund;
	CDlgUpd * m_pUpdate;
	CDlgPrice * m_pPrice;
	CDlgParam * m_pParam;
	CDlgSamAct * m_pSamAct;
	CDlgDescind * m_pDescind;
	CDlgDebug * m_pDebug;
	CDlgDegrade * m_pDegrade;
	CDlgSaleEx * m_pSaleEx;

	CReBar			m_wndReBar;
	CStatusBarCtrl	m_WndStatus;
	CToolBar		m_wndToolBar;
	CDataBar		m_wndDlgBar;

	CList<CString, CString> m_ErrorDiscribe;

	BOOL m_bMenuEnable;


public:
	// 输出API调用结果
	CRichEditCtrl m_rEditOut;
	static BOOL m_bEnableDebuger;

	void AppendText(CString& strText);
	void SetStatusBar(CString strOper, RETINFO Error, LARGE_INTEGER liStart, LARGE_INTEGER liEnd);
	void FormHeader(CString& strTarg, CString strSrc);
	void FormOut(CString& strTarg, int nRow);
	void AddSpace(CString& str);
	void BCDTimetoString(LPBYTE pbSrc, BYTE bLenSrc, char * pszTarg);
	void BCDtoString(LPBYTE pbSrc, BYTE bLenSrc, char * pszTarg);
	void GetErrorString(DWORD nError, CString& strError);

	void DispResult(CString& strOut, CString strOper, RETINFO Error, LARGE_INTEGER liStart, LARGE_INTEGER liEnd);
	void DispResult(_Struct_Type StructType, LPVOID lpStruct, WORD lenStruct, CString strOper, LARGE_INTEGER liStart, LARGE_INTEGER liEnd);

	BOOL GetSetting(int& nPort, WORD& wStation, WORD& wDevice);

	// 加载车站表
	void LoadStationTable(CString strPath);

	// 获取车站参数的名称
	CString GetStationPrmPath();

	// 交易确认
	void TransConfirm(RETINFO ret);

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDevBom();
	afx_msg void OnDevAgm();
	afx_msg void OnDevTvm();
	afx_msg void OnDevTcm();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSaveDisp();
	afx_msg void OnClearDisp();
	afx_msg void OnExit();
	afx_msg void OnClose();
	afx_msg LRESULT OnAnalyse(WPARAM, LPARAM);
	afx_msg LRESULT OnIncrease(WPARAM, LPARAM);
	afx_msg LRESULT OnDecrease(WPARAM, LPARAM);
	afx_msg LRESULT OnSamAct(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnUpdate(WPARAM, LPARAM);
	afx_msg LRESULT OnRefund(WPARAM, LPARAM);
	afx_msg LRESULT OnSjtsale(WPARAM, LPARAM);
	afx_msg LRESULT OnSaleEx(WPARAM, LPARAM);
	afx_msg LRESULT OnInitReader(WPARAM, LPARAM);
	afx_msg LRESULT OnUnInitReader(WPARAM, LPARAM);
	afx_msg LRESULT OnChangeAnt(WPARAM, LPARAM);
	afx_msg LRESULT OnChargeDescind(WPARAM, LPARAM);
	afx_msg void OnCoreUpdt();
	afx_msg void OnQueryFare();
	afx_msg void OnParam();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnSamInfo();
	afx_msg void OnCmd();
	afx_msg void OnTestHard();
	afx_msg void OnReaderVersion();
	afx_msg void OnTradeFileExch();
	afx_msg void OnDebug();
	afx_msg void OnGetFileFromReader();
	afx_msg void OnUpgrade();
	afx_msg void OnEnalbeDebuger();
	afx_msg void OnSetDegrade();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
protected:

	virtual void PostNcDestroy();

	BOOL HasRootAuth();

	void EnableRootMenus(BOOL bRoot);

};
