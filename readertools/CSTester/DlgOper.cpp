// DlgOper.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgOper.h"
#include "CSTesterDlg.h"


// CDlgOper �Ի���

IMPLEMENT_DYNAMIC(CDlgOper, CDialog)

CDlgOper::CDlgOper(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOper::IDD, pParent)
	, m_bForbid(FALSE)
{

}

CDlgOper::~CDlgOper()
{
}

void CDlgOper::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_FORBBID, m_bForbid);
	DDX_Control(pDX, IDC_CHK_FORBBID, m_bnVislbe);
}

BEGIN_MESSAGE_MAP(CDlgOper, CDialog)
	ON_BN_CLICKED(IDC_BN_UPDATE, &CDlgOper::OnBnClickedBnUpdate)
	ON_BN_CLICKED(IDC_BN_SALE, &CDlgOper::OnBnClickedBnSale)
	ON_BN_CLICKED(IDC_BN_INC, &CDlgOper::OnBnClickedBnInc)
	ON_BN_CLICKED(IDC_BN_REFUND, &CDlgOper::OnBnClickedBnRefund)
	ON_BN_CLICKED(IDC_BN_UNLOCK, &CDlgOper::OnBnClickedBnUnlock)
	ON_BN_CLICKED(IDC_BN_DEFFER, &CDlgOper::OnBnClickedBnDeffer)
	ON_MESSAGE(WM_ANALYSE, &CDlgOper::OnAnalyse)
	ON_BN_CLICKED(IDC_BN_READ, &CDlgOper::OnBnClickedBnRead)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BN_DESCIND, &CDlgOper::OnBnClickedBnDescind)
	ON_BN_CLICKED(IDC_CHK_FORBBID, &CDlgOper::OnBnClickedChkForbbid)
END_MESSAGE_MAP()


// CDlgOper ��Ϣ�������

void CDlgOper::OnBnClickedBnUpdate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_UPDATE);
}

void CDlgOper::OnBnClickedBnSale()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (g_Analyse.bTicketType[0] == 0x01)
	{
		((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_SJTSALE, 0x01);
	}
	else
	{
		OTHERSALE os		= {0};
		WORD lenos			= 0;
		WORD lenss			= 0;
		BYTE bData[128]		= {0};
		BYTE bRecv[1024]	= {0};
		int pos				= 7;
		WORD wValue			= 0;
		CString strValue;

		do 
		{
			GetDlgItemText(IDC_EDIT_VALUE, strValue);
			wValue = (WORD)atoi(strValue);

			QueryPerformanceCounter(&g_liStart);
			g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 33, bData, 4, bRecv, sizeof(bRecv));
			QueryPerformanceCounter(&g_liEnd);

			if (g_retInfo.wErrCode != 0)
				break;
			pos += sizeof(RETINFO);

			memcpy(&lenss, bRecv + pos, 2);
			pos += 2;

			if (lenss > 0)
			{
				pos += lenss;
			}

			memcpy(&lenos, bRecv + pos, 2);
			pos += 2;

			if (lenos > 0)
			{
				memcpy(&os, bRecv + pos, lenos);
			}

		} while (0);

		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_SvtSale, &os, lenos, "����", g_liStart, g_liEnd);	

		((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);
	}
}

void CDlgOper::OnBnClickedBnInc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//if (CCSTesterDlg::m_bEnableDebuger)
		((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_INCREASE);
	//else
		//AfxMessageBox("Ȩ�޴���");
}

void CDlgOper::OnBnClickedBnRefund()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_REFUND);
}

void CDlgOper::OnBnClickedBnUnlock()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	TICKETLOCK tu		= {0};
	WORD lentu			= 0;
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 41, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(&lentu, bRecv + pos, 2);
		pos += 2;

		if (lentu > 0)
		{
			memcpy(&tu, bRecv + pos, lentu);
			pos += lentu;
		}

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Lock, &tu, lentu, "��Ʊ����", g_liStart, g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);
}

void CDlgOper::OnBnClickedBnDeffer()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������


	TICKETDEFER td		= {0};
	WORD lentd			= 0;
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 40, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(&lentd, bRecv + pos, 2);
		pos += 2;

		if (lentd > 0)
		{
			memcpy(&td, bRecv + pos, lentd);
			pos += lentd;
		}

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Deffer, &td, lentd, "��Ʊ����", g_liStart, g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);
}

// Ʊ�������ɲ���״̬
#define OSTA_CHARGE					(DWORD)(0x01)							// �Ƿ�ɳ�(ֵ/�˴�)
#define OSTA_UPDATE					(DWORD)(0x01<<1)						// �Ƿ�ɸ���
#define OSTA_SALE					(DWORD)(0x01<<2)						// �Ƿ�ɷ���
#define OSTA_ACTIVE					(DWORD)(0x01<<3)						// �Ƿ�ɼ���
#define OSTA_DIFFER					(DWORD)(0x01<<4)						// �Ƿ������
#define OSTA_REFUND					(DWORD)(0x01<<5)						// �Ƿ���˿�
#define OSTA_UNLOCK					(DWORD)(0x01<<6)						// �Ƿ�ɽ���
#define OSTA_REFUND_DE				(DWORD)(0x01<<7)						// �Ƿ����Ѻ��

void CDlgOper::EnableButtons(DWORD dwOperStatus)
{
	m_dwOperStatus = dwOperStatus;


	GetDlgItem(IDC_BN_INC)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_SALE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_DEFFER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_REFUND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_UNLOCK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_UPDATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_REFUND)->EnableWindow(FALSE);

	DWORD dwMap[][2] = 
	{
		IDC_BN_INC,		OSTA_CHARGE,
		IDC_BN_SALE,	OSTA_SALE,
		IDC_BN_DEFFER,	OSTA_DIFFER,
		IDC_BN_REFUND,	OSTA_REFUND,
		IDC_BN_UNLOCK,	OSTA_UNLOCK,
		IDC_BN_UPDATE,	OSTA_UPDATE,
		IDC_BN_REFUND,	OSTA_REFUND_DE
	};

	for (int i=0;i<sizeof(dwMap)/sizeof(dwMap[0]);i++)
	{
		if (dwOperStatus & dwMap[i][1])
			GetDlgItem(dwMap[i][0])->EnableWindow(TRUE);
		/*
		else
			GetDlgItem(dwMap[i][0])->EnableWindow(FALSE);*/
	}
}

void CDlgOper::DispAnalyseErr(CString str)
{
	CRect rc(0, 0, 300, 30);

	m_strAnalyseErr = str;
	InvalidateRect(&rc);
}

LRESULT CDlgOper::OnAnalyse(WPARAM wparam, LPARAM lparam)
{
	m_dwOperStatus = g_Analyse.dwOperationStauts;
	UpdateData();
	if (!m_bForbid)
		EnableButtons(g_Analyse.dwOperationStauts);

	m_bnVislbe.ShowWindow(CCSTesterDlg::m_bEnableDebuger);
	
	return 0;
}

void CDlgOper::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
	
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255, 20, 10));
	dc.TextOut(10, 1, m_strAnalyseErr, m_strAnalyseErr.GetLength());
}

void CDlgOper::OnBnClickedBnRead()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TICKETINFO tkinf	= {0};
	WORD lenInf			= 0;
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 32, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(&lenInf, bRecv + pos, 2);
		pos += 2;

		if (lenInf > 0)
		{
			memcpy(&tkinf, bRecv + pos, lenInf);
			pos += lenInf;
		}

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_TkInfo, &tkinf, lenInf, "BOM��Ʊ����Ϣ", g_liStart, g_liEnd);
}

void CDlgOper::OnBnClickedBnDescind()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	((CCSTesterDlg * )AfxGetMainWnd())->SendMessage(WM_DESCIND);
}

void CDlgOper::OnBnClickedChkForbbid()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	if (!m_bForbid)
		EnableButtons(g_Analyse.dwOperationStauts);
	else
		EnableButtons(0xFFFFFFFF);

}
