// DlgRoot.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgRoot.h"
#include "CSTesterDlg.h"


// CDlgRoot �Ի���

IMPLEMENT_DYNAMIC(CDlgRoot, CDialog)

CDlgRoot::CDlgRoot(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRoot::IDD, pParent)
{

}

CDlgRoot::~CDlgRoot()
{
}

void CDlgRoot::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_NOTICE, m_staticNotice);
	DDX_Control(pDX, IDC_EDIT_PSW, m_editPsw);
}


BEGIN_MESSAGE_MAP(CDlgRoot, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgRoot::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgRoot::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgRoot ��Ϣ�������

void CDlgRoot::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strPsw;

	//m_editPsw.GetWindowText(strPsw);
	//if (strPsw == "GDMHJD-@2013%")
	{
		CCSTesterDlg::m_bEnableDebuger = TRUE;
		OnOK();
	}
	/*else
	{
		m_staticNotice.SetWindowText("���������������");
		m_editPsw.SetWindowText("");
		m_editPsw.SetFocus();
	}*/
}

void CDlgRoot::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

CString CDlgRoot::GetCpuID()
{
	CString strID = "";
	for (int i=0;i<2;i++)
	{
		strID += GetCpuID(i+1);
	}

	return strID;
}

CString CDlgRoot::GetCpuID(int nItem)
{
	CString ret;
	DWORD s1,s2;

	__asm{ 
		mov eax,nItem
			xor edx,edx 
			cpuid 
			mov s1, edx 
			mov s2, eax 
	}
	ret.Format("%08X%08X",s1,s2);

	return ret;
}