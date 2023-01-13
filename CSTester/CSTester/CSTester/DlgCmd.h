#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "HexEdit.h"
//
//typedef struct _CMD_INFO
//{
//	BYTE	cmdMain;
//	BYTE	cmdSub;
//	WORD	lenSend;
//	BYTE	bSend[1024];
//	WORD	lenRecv;
//	BYTE	bRecv[1024];
//
//}CMDINF, *PCMDINF;

typedef struct _LAST_RESULT
{
	WORD	len;
	BYTE	recv[256];

}LASTRESULT, *PLASTRESULT;

// CDlgCmd 对话框

class CDlgCmd : public CDialog
{
	DECLARE_DYNAMIC(CDlgCmd)

public:
	CDlgCmd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCmd();

// 对话框数据
	enum { IDD = IDD_DLG_CMD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void AppendText(CString& strText);
	void DispCmd(CString strOper, RETINFO ret, WORD lenParam, BYTE * pParam, WORD lenRecv, BYTE * pRecv);
	void DispCmd(CString strOper, RETINFO ret, CMDINF cmdinf);
	RETINFO RunCommand(CMDINF& cmdinf);

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	CComboBox m_cmdAnt;
	CComboBox m_cmbAntType;
	afx_msg void OnBnClickedBnTest();
	afx_msg void OnBnClickedBnTestkk();
	afx_msg void OnBnClickedBnRfVer();
	afx_msg void OnBnClickedBnMinit();
	afx_msg void OnBnClickedBnMatc();
	afx_msg void OnBnClickedBnMsel();
	afx_msg void OnBnClickedBnMread();
	afx_msg void OnBnClickedBnMwrite();
	afx_msg void OnBnClickedBnMauth();
	afx_msg void OnBnClickedBnUlReq();
	afx_msg void OnBnClickedBnUlAntc();
	afx_msg void OnBnClickedBnUlSel();
	afx_msg void OnBnClickedBnUlRead();
	afx_msg void OnBnClickedBnUlWrite();
	afx_msg void OnBnClickedBnUlCwrite();
	afx_msg void OnBnClickedBnCpuRst();
	afx_msg void OnBnClickedBnCpuCmd();
	afx_msg void OnBnClickedBnSamBaud();
	afx_msg void OnBnClickedBnSamRst();
	afx_msg void OnBnClickedBnSamCmd();
	afx_msg void OnBnClickedBnOpen();
	afx_msg void OnBnClickedBnRefresh();
	afx_msg void OnBnClickedBnClose();
	afx_msg void OnClose();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedBnMreq();
	afx_msg void OnBnClickedBnUlAntc2();
	afx_msg void OnBnClickedBnUlSel2();
	afx_msg void OnBnClickedBnLoad();
	afx_msg void OnCbnSelchangeCombCmd();
	afx_msg void OnBnClickedBnTac();

private:
	CComboBox m_cmbPort;
	CRichEditCtrl m_rEditDsp;
	CComboBox m_cmbSockID;
	CComboBox m_cmbSamBaud;
	CHexEdit m_editCmd;
	CStatic m_lenCmd;
	CComboBox m_cmbLineNo;
	CComboBox m_cmbKey;
	CComboBox m_cmbCommnd;

	bool CheckOneRecord(char * strBufferToCheck);

	// 获取TAC计算因子,并返回TAC在但条记录的位置
	int GetTacFactor(char * pRecord, LPBYTE lpLogiclID, LPBYTE lpTimeTrade, long& lBalanceAfter, long& lTradeAmount, long& lTradeCounter);

	bool GetOctMfTAC(LPBYTE lpLogiclID, LPBYTE lpTimeTrade, long lBalanceAfter, long lTradeAmount, long lTradeCounter, char * lpTAC);

	bool SamReset();

	bool TacPrepare(LPBYTE lpLogiclID);

	bool CalTac(LPBYTE lpTimeTrade, long lBalanceAfter, long lTradeAmount, long lTradeCounter, char * pTac);
};
