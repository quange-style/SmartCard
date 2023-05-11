// DlgAgm.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgAgm.h"
#include "CSTesterDlg.h"


// CDlgAgm �Ի���
ETICKETDEALINFO g_ETicketOnline = { 0 };
IMPLEMENT_DYNAMIC(CDlgAgm, CDialog)

CDlgAgm::CDlgAgm(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAgm::IDD, pParent)
{

}

CDlgAgm::~CDlgAgm()
{
}

void CDlgAgm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_DEV_TYPE, m_cmbDevType);
	DDX_Control(pDX, IDC_CMB_ASYLE_TYPE, m_cmbAsyleType);
	DDX_Control(pDX, IDC_CMB_ANT, m_cmbAnt);
}

BEGIN_MESSAGE_MAP(CDlgAgm, CDialog)
	ON_BN_CLICKED(IDC_BN_INIT, &CDlgAgm::OnBnClickedBnInit)
	ON_BN_CLICKED(IDC_BN_ENTRY, &CDlgAgm::OnBnClickedBnEntry)
	ON_BN_CLICKED(IDC_BN_EXIT, &CDlgAgm::OnBnClickedBnExit)
	ON_BN_CLICKED(IDC_BN_AGM_OPEN, &CDlgAgm::OnBnClickedBnSerialOpen)
	ON_BN_CLICKED(IDC_BN_AGM_OPEN_START, &CDlgAgm::OnBnClickedBnOpenStart)
	ON_MESSAGE(WM_PAGECHANGED, &CDlgAgm::OnPanalChanged)
	ON_CBN_SELCHANGE(IDC_CMB_ASYLE_TYPE, &CDlgAgm::OnCbnSelchangeCmbAsyleType)
END_MESSAGE_MAP()


// CDlgAgm ��Ϣ�������

void CDlgAgm::OnBnClickedBnInit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BYTE bData[32]		= {0};
	BYTE bRecv[1024]	= {0};
	WORD lenInf			= 0;

	CString strDevType;
	m_cmbDevType.GetWindowText(strDevType);
	g_bDeviceType = atoi(strDevType.Left(1));

	if (!((CCSTesterDlg *)AfxGetMainWnd())->GetSetting(g_nPortOpen, g_wStation, g_wDeviceID))
		return;

	bData[0] = Hex_Ex_Bcd((BYTE)(g_wStation / 100), true);
	bData[1] = Hex_Ex_Bcd((BYTE)(g_wStation % 100), true);
	bData[2] = g_bDeviceType;
	bData[3] = Hex_Ex_Bcd((BYTE)(g_wDeviceID / 100), true);
	bData[4] = Hex_Ex_Bcd((BYTE)(g_wDeviceID % 100), true);
	bData[5] = 0x55;
	bData[6] = 0x10;
	memcpy(&bData[7],"\x02\x19\x55\x10\xFF\xFF\xFF\xFF",8);

	QueryPerformanceCounter(&g_liStart);
	g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 0, bData, 15, bRecv, sizeof(bRecv));
	QueryPerformanceCounter(&g_liEnd);

	if (g_retInfo.wErrCode == 0)
	{
		switch(g_bDeviceType)
		{
		case 4:
			GetDlgItem(IDC_BN_ENTRY)->EnableWindow(TRUE);
			GetDlgItem(IDC_BN_EXIT)->EnableWindow(FALSE);
			break;
		case 5:
			GetDlgItem(IDC_BN_ENTRY)->EnableWindow(FALSE);
			GetDlgItem(IDC_BN_EXIT)->EnableWindow(TRUE);
			break;
		case 6:
			GetDlgItem(IDC_BN_ENTRY)->EnableWindow(TRUE);
			GetDlgItem(IDC_BN_EXIT)->EnableWindow(TRUE);
			break;
		}

		m_cmbAsyleType.EnableWindow(TRUE);
		memcpy(&lenInf, bRecv + 7 + sizeof(RETINFO), 2);
	}

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_init_arr, bRecv + 9 + sizeof(RETINFO), lenInf, "�豸������ʼ��", g_liStart, g_liEnd);
}

void CDlgAgm::OnBnClickedBnEntry()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	ENTRYGATE eng		= {0};
	ETICKETONLINE etg = {0};
	WORD leneng			= 0;
	WORD lenetg			= 0;
	TICKETLOCK tl		= {0};
	WORD lentl			= 0;
	BYTE bRecv[512]		= {0};
	int pos				= 7;
	BYTE bRfu[2] = {0};

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate((BYTE)m_cmbAnt.GetCurSel(), g_nBeep, 1, 14, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(bRfu,bRecv + 7 + 3, 2);

		if(bRfu[0] == 0x11)
			memcpy(&lenetg, bRecv + pos, 2);
		else
			memcpy(&leneng, bRecv + pos, 2);
		pos += 2;

		if (leneng > 0)
		{
			memcpy(&eng, bRecv + pos, leneng);
			pos += leneng;
		}

		if(lenetg > 0)
		{
			memcpy(&etg, bRecv + pos, lenetg);
			memcpy(&g_ETicketOnline.cCenterCode, etg.cCenterCode, 32);
			pos += lenetg;
		}

		memcpy(&lentl, bRecv + pos, 2);
		pos += 2;

		if (lentl > 0)
		{
			memcpy(&tl, bRecv + pos, lentl);
		}

	} while (0);

	if (lentl > 0)
		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Lock, &tl, lentl, "����������", g_liStart, g_liEnd);
	if(bRfu[0] == 0x11)
	{
		printf("��ά���բ������Ʊ");
		((CCSTesterDlg*)AfxGetMainWnd())->DispResult(struct_ETOnline, &etg, lenetg, "��ά���բ������Ʊ", g_liStart, g_liEnd);
		GetDlgItem(IDC_BN_AGM_OPEN_START)->EnableWindow(TRUE);
	}
	else
		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Entry, &eng, leneng, "��բ", g_liStart, g_liEnd);
}

void CDlgAgm::OnBnClickedBnExit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	PEPURSETRADE ps		= {0};
	ETICKETONLINE etg	= {0};
	WORD lenps			= 0;
	WORD lenetg         = 0;
	TICKETLOCK tl		= {0};
	WORD lentl			= 0;
	BYTE bRecv[512]		= {0};
	int pos				= 7;
	BYTE bRfu[2] = {0};

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate((BYTE)m_cmbAnt.GetCurSel(), g_nBeep, 1, 15, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

		memcpy(bRfu,bRecv + 7 + 3, 2);

		if(bRfu[0] == 0x11)
			memcpy(&lenetg, bRecv + pos, 2);
		else
			memcpy(&lenps, bRecv + pos, 2);

		
		pos += 2;

		if (lenps > 0)
		{
			memcpy(&ps, bRecv + pos, lenps);
			pos += lenps;
		}

		if(lenetg > 0)
		{
			memcpy(&etg, bRecv + pos, lenetg);
			memcpy(&g_ETicketOnline.cCenterCode, etg.cCenterCode, 32);
			pos += lenetg;
		}

		memcpy(&lentl, bRecv + pos, 2);
		pos += 2;

		if (lentl > 0)
		{
			memcpy(&tl, bRecv + pos, lentl);
		}

	} while (0);

	if (lentl > 0)
		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Lock, &tl, lentl, "����������", g_liStart, g_liEnd);
	if (bRfu[0] == 0x11)
	{
		((CCSTesterDlg*)AfxGetMainWnd())->DispResult(struct_ETOnline, &etg, lenetg, "��ά���բ������Ʊ", g_liStart, g_liEnd);
		GetDlgItem(IDC_BN_AGM_OPEN_START)->EnableWindow(TRUE);
	}
	else
		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Purse, &ps, lenps, "��բ", g_liStart, g_liEnd);
}

void CDlgAgm::OnBnClickedBnOpenStart()
{	
	AfxGetMainWnd()->SendMessage(WM_AGM_OPEN);
}

LRESULT CDlgAgm::OnPanalChanged(WPARAM wparam, LPARAM lparam)
{
	g_Serial.Close();

	AfxGetMainWnd()->SendMessage(WM_UNINITREADER);
	GetDlgItem(IDC_BN_INIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_ENTRY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_EXIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_AGM_OPEN_START)->EnableWindow(FALSE);
	g_bDeviceType = 6;

	return 0;
}

BOOL CDlgAgm::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	m_cmbAsyleType.SetCurSel(2);
	m_cmbDevType.SetCurSel(2);
	m_cmbAnt.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgAgm::OnBnClickedBnSerialOpen()//�򿪴���
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (!((CCSTesterDlg *)AfxGetMainWnd())->GetSetting(g_nPortOpen, g_wStation, g_wDeviceID))
		return;

	g_retInfo.wErrCode = (uint16_t)g_Serial.Open(g_nPortOpen);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Null, NULL, 0, "�򿪴���", g_liStart, g_liEnd);

	if (g_retInfo.wErrCode == 0)
	{
		AfxGetMainWnd()->SendMessage(WM_INITREADER);

		GetDlgItem(IDC_BN_INIT)->EnableWindow(TRUE);
	}

}

void CDlgAgm::OnCbnSelchangeCmbAsyleType()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BYTE bData[8]		= {0};
	BYTE bRecv[512]		= {0};

	bData[0] = (BYTE)(m_cmbAsyleType.GetCurSel() + 1);

	QueryPerformanceCounter(&g_liStart);
	g_retInfo = g_Serial.Communicate((BYTE)m_cmbAnt.GetCurSel(), g_nBeep, 1, 13, bData, 1, bRecv, sizeof(bRecv));
	QueryPerformanceCounter(&g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Null, NULL, 0, "����ͨ������", g_liStart, g_liEnd);
}
