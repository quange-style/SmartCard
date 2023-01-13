// * .cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgRefund.h"
#include "CSTesterDlg.h"


// CDlgRefund  对话框

IMPLEMENT_DYNAMIC(CDlgRefund , CDialog)

CDlgRefund ::CDlgRefund (CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRefund ::IDD, pParent)
{

}

CDlgRefund ::~CDlgRefund ()
{
}

void CDlgRefund ::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRefund , CDialog)
	ON_BN_CLICKED(IDC_BN_REFUND, &CDlgRefund ::OnBnClickedBnRefund)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CDlgRefund ::OnBnClickedBnCancel)
	ON_BN_CLICKED(IDC_RADIO2, &CDlgRefund::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO1, &CDlgRefund::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO3, &CDlgRefund::OnBnClickedRadio3)
END_MESSAGE_MAP()


// CDlgRefund  消息处理程序

void CDlgRefund ::OnBnClickedBnRefund()
{
	// TODO: 在此添加控件通知处理程序代码
	// dRefund = {0};

	DIRECTREFUND dr		= {0};
	WORD lendr			= 0;
	BYTE bData[8]		= {0};
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		bData[0] = (BYTE)m_nModalResult;

		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 39, bData, 4, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(&lendr, bRecv + pos, 2);
		pos += 2;

		if (lendr > 0)
		{
			memcpy(&dr, bRecv + pos, lendr);
			pos += lendr;
		}

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Refund, &dr, lendr, "退款", g_liStart, g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);

	ShowWindow(SW_HIDE);
}

void CDlgRefund ::OnBnClickedBnCancel()
{
	// TODO: 在此添加控件通知处理程序代码

	ShowWindow(SW_HIDE);
}

void CDlgRefund::OnBnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nModalResult = 1;
}

void CDlgRefund::OnBnClickedRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nModalResult = 2;
}

void CDlgRefund::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nModalResult = 3;
}

BOOL CDlgRefund::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_nModalResult = 1;
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(1);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgRefund::SetChecker(BOOL bEnableReturnBalance, BOOL bEnableReturnDeposite)
{
	if (bEnableReturnBalance)
	{
		if (bEnableReturnDeposite)
		{
			((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(1);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(1);
			((CButton *)GetDlgItem(IDC_RADIO2))->EnableWindow(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO3))->EnableWindow(FALSE);

		}
	}
	else
	{
		if (bEnableReturnDeposite)
		{
			((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(1);
			((CButton *)GetDlgItem(IDC_RADIO1))->EnableWindow(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO3))->EnableWindow(FALSE);

		}
		else
		{
			((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(0);
			((CButton *)GetDlgItem(IDC_RADIO1))->EnableWindow(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(0);
			((CButton *)GetDlgItem(IDC_RADIO2))->EnableWindow(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(0);
			((CButton *)GetDlgItem(IDC_RADIO3))->EnableWindow(FALSE);
		}
	}
}
