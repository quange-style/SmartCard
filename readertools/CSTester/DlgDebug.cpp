// DlgDebug.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgDebug.h"
#include "CSTesterDlg.h"
#include "PubFuncs.h"

// CDlgDebug 对话框

IMPLEMENT_DYNAMIC(CDlgDebug, CDialog)

CDlgDebug::CDlgDebug(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDebug::IDD, pParent)
{

}

CDlgDebug::~CDlgDebug()
{
}

void CDlgDebug::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CLASSIC, m_EditClassic);
	DDX_Control(pDX, IDC_EDIT_ADDR, m_EditAddr);
	DDX_Control(pDX, IDC_EDIT_DATA, m_EditData);
	DDX_Control(pDX, IDC_STATIC_LEN, m_StaticLen);
}


BEGIN_MESSAGE_MAP(CDlgDebug, CDialog)
	ON_BN_CLICKED(IDC_BN_SEND, &CDlgDebug::OnBnClickedBnSend)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CDlgDebug::OnBnClickedBnCancel)
	ON_EN_CHANGE(IDC_EDIT_DATA, &CDlgDebug::OnEnChangeEditData)
	ON_EN_CHANGE(IDC_EDIT_ADDR, &CDlgDebug::OnEnChangeEditAddr)
END_MESSAGE_MAP()


// CDlgDebug 消息处理程序

void CDlgDebug::OnBnClickedBnSend()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strOut;
	CString strText;
	BYTE bClassic;
	BYTE bAddr;
	BYTE bData[1024];
	BYTE bRecv[1024];
	WORD lenData;
	WORD lenRecv = 0;;
	int pos = 7;

	m_EditClassic.GetWindowText(strText);
	String2Hex(strText.GetBuffer(), 2, bClassic);

	m_EditAddr.GetWindowText(strText);
	String2Hex(strText.GetBuffer(), 2, bAddr);

	m_EditData.GetWindowText(strText);
	lenData = strText.GetLength() / 2;
	String2HexSeq(strText.GetBuffer(), strText.GetLength(), bData, sizeof(bData));

	g_retInfo = g_Serial.Communicate(0, g_nBeep, bClassic, bAddr, bData, lenData, bRecv, sizeof(bRecv));
	if (g_retInfo.wErrCode == 0)
	{
		pos += sizeof(RETINFO);
		memcpy(&lenRecv, bRecv + pos, 2);
		pos += 2;
	}
	for (WORD i=0;i<lenRecv;i++)
	{
		strText.Format("%02X ", bRecv[pos]);
		pos++;
		strOut += strText;
	}

	strOut = "\t接收：" + strOut + "\n\n";

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(strOut, "调试指令", g_retInfo, g_liStart, g_liEnd);
}

void CDlgDebug::OnBnClickedBnCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
}

BOOL CDlgDebug::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_EditClassic.SetLimitText(2);
	m_EditAddr.SetLimitText(4);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgDebug::OnEnChangeEditData()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString strLen;
	strLen.Format("% 4d", m_EditData.GetWindowTextLength());
	m_StaticLen.SetWindowText(strLen);
}


void CDlgDebug::OnEnChangeEditAddr()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
