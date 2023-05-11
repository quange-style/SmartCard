// DlgSaleEx.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgSaleEx.h"
#include "PubFuncs.h"
#include "CSTesterDlg.h"

// CDlgSaleEx 对话框

IMPLEMENT_DYNAMIC(CDlgSaleEx, CDialog)

CDlgSaleEx::CDlgSaleEx(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSaleEx::IDD, pParent)
{

}

CDlgSaleEx::~CDlgSaleEx()
{
}

void CDlgSaleEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMB_TYPE, m_cmbTkType);
	DDX_Control(pDX, IDC_COMB_CODE, m_cmbCode);
}


BEGIN_MESSAGE_MAP(CDlgSaleEx, CDialog)
	ON_BN_CLICKED(IDC_BN_SALE, &CDlgSaleEx::OnBnClickedBnSale)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CDlgSaleEx::OnBnClickedBnCancel)
	ON_CBN_SELCHANGE(IDC_COMB_TYPE, &CDlgSaleEx::OnCbnSelchangeCombType)
	ON_CBN_SELCHANGE(IDC_COMB_CODE, &CDlgSaleEx::OnCbnSelchangeCombCode)
END_MESSAGE_MAP()


// CDlgSaleEx 消息处理程序

void CDlgSaleEx::OnBnClickedBnSale()
{
	// TODO: 在此添加控件通知处理程序代码
	SJTSALE ss			= {0};
	WORD lenss			= 0;
	BYTE bData[128]		= {0};
	BYTE bRecv[1024]	= {0};
	int pos				= 7;
	WORD wValue			= 0;
	CString strValue;

	do 
	{
		if (m_cmbTkType.GetCurSel() < 0 || m_cmbCode.GetCurSel() < 0)
		{
			MessageBox("请选择行政代码和票卡类型");
			break;
		}

		GetDlgItemText(IDC_EDIT_PENALTY, strValue);
		wValue = (WORD)atoi(strValue);

		memcpy(bData, &wValue, 2);

		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 36, bData, 2, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		if (g_retInfo.wErrCode != 0)
			break;
		pos += sizeof(RETINFO);

		memcpy(&lenss, bRecv + pos, 2);
		pos += 2;

		if (lenss > 0)
		{
			memcpy(&ss, bRecv + pos, lenss);
		}

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_SjtSale, &ss, lenss, "发售出站票", g_liStart, g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);
}

void CDlgSaleEx::OnBnClickedBnCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
}

void CDlgSaleEx::OnCbnSelchangeCombType()
{
	// TODO: 在此添加控件通知处理程序代码
	QueryPolicyPenalty();
}

void CDlgSaleEx::OnCbnSelchangeCombCode()
{
	// TODO: 在此添加控件通知处理程序代码
	QueryPolicyPenalty();
}

BOOL CDlgSaleEx::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_cmbCode.SetDroppedWidth(200);

	m_cmbTkType.SetDroppedWidth(200);
	m_cmbTkType.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgSaleEx::QueryPolicyPenalty()
{
	BYTE bData[128]		= {0};
	BYTE bRecv[1024]	= {0};
	int pos				= 7;
	WORD wValue			= 0;
	CString strTmp;

	do 
	{
		GetDlgItemText(IDC_COMB_TYPE, strTmp);
		String2HexSeq(strTmp.GetBuffer(), 4, bData, 2);

		GetDlgItemText(IDC_COMB_CODE, strTmp);
		bData[2] = (BYTE)atoi(strTmp);

		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 48, bData, 3, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		if (g_retInfo.wErrCode != 0)
			break;
		pos += sizeof(RETINFO) + 2;

		memcpy(&wValue, bRecv + pos, 2);

		strTmp.Format("%d", wValue);
		SetDlgItemText(IDC_EDIT_PENALTY, strTmp);

	} while (0);
}