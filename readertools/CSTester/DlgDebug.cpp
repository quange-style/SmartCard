// DlgDebug.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgDebug.h"
#include "CSTesterDlg.h"
#include "PubFuncs.h"

// CDlgDebug �Ի���

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


// CDlgDebug ��Ϣ�������

void CDlgDebug::OnBnClickedBnSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

	strOut = "\t���գ�" + strOut + "\n\n";

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(strOut, "����ָ��", g_retInfo, g_liStart, g_liEnd);
}

void CDlgDebug::OnBnClickedBnCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ShowWindow(SW_HIDE);
}

BOOL CDlgDebug::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_EditClassic.SetLimitText(2);
	m_EditAddr.SetLimitText(4);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgDebug::OnEnChangeEditData()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString strLen;
	strLen.Format("% 4d", m_EditData.GetWindowTextLength());
	m_StaticLen.SetWindowText(strLen);
}


void CDlgDebug::OnEnChangeEditAddr()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
