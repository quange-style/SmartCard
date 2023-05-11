#pragma once
#include "afxwin.h"
#include <vector>
#include <map>

using namespace std;

#define WM_UPDATE_TRICE		(WM_USER + 200)


// CDlgUpd 对话框

class CDlgUpd : public CDialog
{
	DECLARE_DYNAMIC(CDlgUpd)

public:
	CDlgUpd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgUpd();

// 对话框数据
	enum { IDD = IDD_DLG_UPDATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	void SetUpdInfo(int nTimes);

protected:
	CComboBox m_cbUpdType;
	CComboBox m_cbPayType;
	CComboBox m_cbEntryLine;
	CComboBox m_cbEntryStation;

	
	BYTE m_bUpdateType;
	WORD m_wEntryStation;
	RETINFO m_RetUptate;

	// 设置更新类型
	void SetUpdateType(WORD wErr, BYTE& bUpdateType);
	

	// 设置支付方式
	void SetPayType(BYTE bUpdateType);

	// 设置罚金
	void SetTradeAmount(BYTE bUpdateType, WORD wEntryStation);

	// 设置入站站点下拉框
	void SetEntryStation(BYTE bLineNo);

	// 卡扣罚金
	void Decrease(long lDecVal);

	//设置电子交易类型
	void SetETDealType(BYTE bUpdateType);
public:
	// 设置入站线路下拉框
	void SetEntryLine();

	afx_msg void OnBnClickedBnUpdate();
	afx_msg void OnCbnSelchangeCombEntryStation();
	afx_msg void OnBnClickedBnCancel();
	afx_msg void OnCbnSelchangeCombEntryLine();
	afx_msg void OnCbnSelchangeCombPayType();
	afx_msg void OnCbnSelchangeCombUpdType();
	afx_msg void OnEnChangeEditValue2();
	afx_msg void OnEnChangeEditValue();
};
