// DlgLogin.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgLogin.h"


// CDlgLogin �Ի���
int CDlgLogin::m_nShiftID = 0;

IMPLEMENT_DYNAMIC(CDlgLogin, CDialog)

CDlgLogin::CDlgLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogin::IDD, pParent)
	, m_strUserName(_T(""))
	, m_StrPassWord(_T(""))
{

}

CDlgLogin::~CDlgLogin()
{
}

void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_OPERID, m_strUserName);
	DDX_Text(pDX, IDC_EDIT_PWD, m_StrPassWord);
}


BEGIN_MESSAGE_MAP(CDlgLogin, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgLogin::OnBnClickedOk)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDlgLogin ��Ϣ�������

void CDlgLogin::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (m_strUserName == "999999" && m_StrPassWord == "111111")
	{
		m_nShiftID++;
		OnOK();
	}
	else
	{
		SetWindowText("�û������������");
		m_strUserName = "";
		m_StrPassWord = "";
		UpdateData(TRUE);
	}
}

BOOL CDlgLogin::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CEdit * pEdit = (CEdit *)GetDlgItem(IDC_EDIT_PWD);
	pEdit->SetLimitText(6);
	pEdit->SetWindowText("111111");

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_OPERID);
	pEdit->SetLimitText(6);
	pEdit->SetWindowText("999999");

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgLogin::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: �ڴ˴������Ϣ����������
	GetDlgItem(IDC_EDIT_OPERID)->SetFocus();
}

void CDlgLogin::GetOperatorInfo(char * pstrOper, int& nShiftID)
{
	strcpy(pstrOper, m_strUserName);
	nShiftID = m_nShiftID;
}
