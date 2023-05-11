// DlgDec.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgDec.h"
#include "CSTesterDlg.h"


// CDlgDec 对话框

IMPLEMENT_DYNAMIC(CDlgDec, CDialog)

CDlgDec::CDlgDec(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDec::IDD, pParent)
	, m_strToDecr(_T(""))
	, m_strAfter(_T(""))
{

}

CDlgDec::~CDlgDec()
{
}

void CDlgDec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TODECR, m_strToDecr);
	DDX_Text(pDX, IDC_EDIT_AFTER, m_strAfter);
}

void CDlgDec::SetOldBalance(long lOldBalance)
{
	m_lOldBalance = lOldBalance;
	CString str;
	str.Format("%ld", m_lOldBalance);
	GetDlgItem(IDC_EDIT_BEFORE)->SetWindowText(str);
	GetDlgItem(IDC_EDIT_TODECR)->SetWindowText("0");
	GetDlgItem(IDC_EDIT_AFTER)->SetWindowText(str);
}

BEGIN_MESSAGE_MAP(CDlgDec, CDialog)
	ON_BN_CLICKED(IDC_BN_DECREASE, &CDlgDec::OnBnClickedBnDecrease)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CDlgDec::OnBnClickedBnCancel)
	ON_EN_CHANGE(IDC_EDIT_TODECR, &CDlgDec::OnEnChangeEditTodecr)
	ON_EN_CHANGE(IDC_EDIT_AFTER, &CDlgDec::OnEnChangeEditAfter)
	ON_EN_SETFOCUS(IDC_EDIT_TODECR, &CDlgDec::OnEnSetfocusEditTodecr)
	ON_EN_SETFOCUS(IDC_EDIT_AFTER, &CDlgDec::OnEnSetfocusEditAfter)
END_MESSAGE_MAP()


// CDlgDec 消息处理程序

void CDlgDec::OnBnClickedBnDecrease()
{
	// TODO: 在此添加控件通知处理程序代码
	PEPURSETRADE ps		= {0};
	WORD lenps			= 0;
	BYTE bRecv[512]		= {0};
	int pos				= 7;

	do 
	{
		CString strAmount = "";
		GetDlgItemText(IDC_EDIT_TODECR, strAmount);
		long lAmount = atol(strAmount);

		QueryPerformanceCounter(&g_liStart);
		if (g_bDeviceType == 2)
			g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 23, (LPBYTE)(&lAmount), sizeof(lAmount), bRecv, sizeof(bRecv));
		else
			g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 38, (LPBYTE)(&lAmount), sizeof(lAmount), bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(&lenps, bRecv + pos, 2);
		pos += 2;

		if (lenps > 0)
		{
			memcpy(&ps, bRecv + pos, lenps);
			pos += lenps;
		}

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Purse, &ps, lenps, "储值票减值", g_liStart, g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);

	ShowWindow(SW_HIDE);
}

void CDlgDec::OnBnClickedBnCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
}

void CDlgDec::OnEnChangeEditTodecr()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if (m_uCurEdit == IDC_EDIT_TODECR)
	{
		UpdateData();
		int nLen = m_strToDecr.GetLength();
		m_lDecrease = atol(m_strToDecr);
		m_lNewBalance = m_lOldBalance - m_lDecrease;
		m_strAfter.Format("%ld", m_lNewBalance);

		SetDlgItemText(IDC_EDIT_AFTER, m_strAfter);
	}
}

void CDlgDec::OnEnChangeEditAfter()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if (m_uCurEdit == IDC_EDIT_AFTER)
	{
		UpdateData();
		int nLen = m_strAfter.GetLength();
		m_lNewBalance = atol(m_strAfter);
		m_lDecrease = m_lOldBalance - m_lNewBalance;
		m_strToDecr.Format("%ld", m_lDecrease);

		SetDlgItemText(IDC_EDIT_TODECR, m_strToDecr);
	}
}

void CDlgDec::OnEnSetfocusEditTodecr()
{
	// TODO: 在此添加控件通知处理程序代码
	m_uCurEdit = IDC_EDIT_TODECR;
}

void CDlgDec::OnEnSetfocusEditAfter()
{
	// TODO: 在此添加控件通知处理程序代码
	m_uCurEdit = IDC_EDIT_AFTER;
}
