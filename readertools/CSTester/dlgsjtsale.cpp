// DlgSjtSale.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgSjtSale.h"
#include "CSTesterDlg.h"


// CDlgSjtSale �Ի���

IMPLEMENT_DYNAMIC(CDlgSjtSale, CDialog)

CDlgSjtSale::CDlgSjtSale(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSjtSale::IDD, pParent)
{

}

CDlgSjtSale::~CDlgSjtSale()
{
}

void CDlgSjtSale::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSjtSale, CDialog)
	ON_BN_CLICKED(IDC_BN_SALE, &CDlgSjtSale::OnBnClickedBnSale)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CDlgSjtSale::OnBnClickedBnCancel)
	ON_CBN_SELCHANGE(IDC_COMB_FARE, &CDlgSjtSale::OnCbnSelchangeCombFare)
END_MESSAGE_MAP()


// CDlgSjtSale ��Ϣ�������

void CDlgSjtSale::OnBnClickedBnSale()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SJTSALE ss			= {0};
	WORD lenss			= 0;
	ETICKETDEALINFO etg = {0};
	WORD lenetg			= 0;
	BYTE bData[128]		= {0};
	BYTE bRecv[1024]	= {0};
	int pos				= 7;
	WORD wValue			= 0;
	CString strValue;
	CString strValue3;
	int iValue = 0;
	BYTE bRfu[2] = {0};

	do 
	{
		GetDlgItemText(IDC_EDIT_VALUE, strValue);
		wValue = (WORD)atoi(strValue);

		GetDlgItemText(IDC_EDIT_VALUE3, strValue3);
		iValue = atoi(strValue3);

		memcpy(bData, &wValue, 2);
		bData[2] = 0x01;
		bData[3] = 0x02;

		QueryPerformanceCounter(&g_liStart);
		
		if(iValue == 1)
		{
			memcpy(&bData[4], "00112233445566778899AABBCCDDEEFF",32);

			memcpy(&bData[36], "00112233445566778899",20);

			g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 29, bData, 4, bRecv, sizeof(bRecv));
		}else if(iValue == 2)
		{
			memcpy(&bData[4], "00112233445566778899AABBCCDDEEFF00112233445566778899AABBCCDDEEFF",64);
			g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 67, bData, 68, bRecv, sizeof(bRecv));
		}else{
			if (g_bDeviceType == 2)
				g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 20, bData, 4, bRecv, sizeof(bRecv));
			else
				g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 33, bData, 4, bRecv, sizeof(bRecv));
		}

		QueryPerformanceCounter(&g_liEnd);

		if (g_retInfo.wErrCode != 0)
			break;

		pos += sizeof(RETINFO);

		memcpy(bRfu,bRecv + 7 + 3, 2);

		memcpy(&lenss, bRecv + pos, 2);
		pos += 2;

		if (lenss > 0)
		{
			memcpy(&ss, bRecv + pos, lenss);
			pos += lenss;
		}

		if(bRfu[0] == 0x11)
		{
			memcpy(&lenetg, bRecv + pos, 2);
			pos += 2;

			if (lenetg > 0)
			{
				memcpy(&etg, bRecv + pos, lenetg);
			}
		}

	} while (0);

	if(bRfu[0] == 0x11)
		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_ETicket, &etg, lenetg, "ȡƱ����", g_liStart, g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_SjtSale, &ss, lenss, "����", g_liStart, g_liEnd);

	((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);
}

void CDlgSjtSale::OnBnClickedBnCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	ShowWindow(SW_HIDE);
}

void CDlgSjtSale::OnCbnSelchangeCombFare()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char szFare[8] = {0};
	GetDlgItemText(IDC_COMB_FARE, szFare, 5);
	SetDlgItemText(IDC_EDIT_VALUE, szFare);
}
