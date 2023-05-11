// DlgUpd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgUpd.h"
#include "CSTesterDlg.h"


// CDlgUpd �Ի���

IMPLEMENT_DYNAMIC(CDlgUpd, CDialog)

CDlgUpd::CDlgUpd(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUpd::IDD, pParent)
{

}

CDlgUpd::~CDlgUpd()
{
}

void CDlgUpd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMB_UPD_TYPE, m_cbUpdType);
	DDX_Control(pDX, IDC_COMB_PAY_TYPE, m_cbPayType);
	DDX_Control(pDX, IDC_COMB_ENTRY_LINE, m_cbEntryLine);
	DDX_Control(pDX, IDC_COMB_ENTRY_STATION, m_cbEntryStation);
}

void CDlgUpd::SetUpdInfo(int nTimes)
{
	// ������״θ��£��ȿ��������ĸ������벻Ϊ0
	m_RetUptate = g_retInfo;

	if (nTimes == 0)
	{
			if (g_Analyse.wError != 0)
				SetUpdateType(g_Analyse.wError, m_bUpdateType);
			else if (g_Analyse.wError1 != 0)
				SetUpdateType(g_Analyse.wError1, m_bUpdateType);

			SetPayType(m_bUpdateType);
			SetTradeAmount(m_bUpdateType, m_wEntryStation);
	}
	else if(nTimes == 1)
	{
		SetUpdateType(g_Analyse.wError1, m_bUpdateType);
		SetPayType(m_bUpdateType);
		SetTradeAmount(m_bUpdateType, m_wEntryStation);
	}
	else if (nTimes == 2)
	{
		SetUpdateType(g_retInfo.wErrCode, m_bUpdateType);
		SetETDealType(m_bUpdateType);
	}
}

void CDlgUpd::SetUpdateType(WORD wErr, BYTE& bUpdateType)
{
	int nCursel = -1;
	switch (wErr)
	{
	case 0x0030:		//��ʱ����
		m_cbEntryLine.EnableWindow(FALSE);
		m_cbEntryStation.EnableWindow(FALSE);
		bUpdateType = 1;
		nCursel = 0;
		break;
	case 0x002A:		//��������վ��ʱ
		m_cbEntryLine.EnableWindow(FALSE);
		m_cbEntryStation.EnableWindow(FALSE);
		bUpdateType = 1;
		nCursel = 0;
		break;
	case 0x0029:		//��������վ����
		m_cbEntryLine.EnableWindow(FALSE);
		m_cbEntryStation.EnableWindow(FALSE);
		bUpdateType = 2;
		nCursel = 1;
		break;
	case 0x0028:		//�������޽�վ��
	case 0x002F:		//�ϴγ�վ10������
		m_cbEntryLine.EnableWindow();
		m_cbEntryStation.EnableWindow();
		bUpdateType = 3;
		nCursel = 2;
		break;
	case 0x0027:		//�Ǹ������н�վ��
		m_cbEntryLine.EnableWindow(FALSE);
		m_cbEntryStation.EnableWindow(FALSE);
		bUpdateType = 10;
		nCursel = 3;
		break;
	case 0x0026:		//�Ǹ�������վ��ʱ
		m_cbEntryLine.EnableWindow(FALSE);
		m_cbEntryStation.EnableWindow(FALSE);
		bUpdateType = 12;
		nCursel = 5;
		break;
	case 0x0025:		//�Ǹ������Ǳ�վ��վ
		m_cbEntryLine.EnableWindow(FALSE);
		m_cbEntryStation.EnableWindow(FALSE);
		bUpdateType = 11;
		nCursel = 4;
		break;
	default:
		m_cbEntryLine.EnableWindow(FALSE);
		m_cbEntryStation.EnableWindow(FALSE);
		break;
	}

	m_cbUpdType.SetCurSel(nCursel);
}

void CDlgUpd::SetPayType(BYTE bUpdateType)
{
	m_cbPayType.ResetContent();
	if (bUpdateType == 1 || bUpdateType == 12 || bUpdateType == 11)
	{
		if (g_Analyse.lBalance > g_Analyse.lPenalty + g_Analyse.lPenalty1)
		{
			if (g_Analyse.bTicketType[0] == 0x06)
			{
				m_cbPayType.AddString("1-�ֽ�");
				m_cbPayType.AddString("3-һ��ͨ");
				m_cbPayType.EnableWindow(TRUE);
			}
			else if (g_Analyse.bTicketType[0] != 0x01)
			{
				m_cbPayType.AddString("1-�ֽ�");
				m_cbPayType.AddString("2-��ֵ��");
				m_cbPayType.EnableWindow(TRUE);
			}
			else
			{
				m_cbPayType.AddString("1-�ֽ�");
				m_cbPayType.EnableWindow(FALSE);
			}
		}
		else
		{
			m_cbPayType.AddString("1-�ֽ�");
			m_cbPayType.EnableWindow(FALSE);
		}
		m_cbPayType.SetCurSel(0);
	}
	else if (bUpdateType == 2)
	{
		m_cbPayType.AddString("1-�ֽ�");
		m_cbPayType.EnableWindow(FALSE);
		m_cbPayType.SetCurSel(0);
	}
	else
	{
		m_cbPayType.EnableWindow(FALSE);
	}
}

void CDlgUpd::SetETDealType(BYTE bUpdateType)//���ý��������� TODO:�Ի���û����
{
	m_cbPayType.ResetContent();
	if (g_Eticanalyse.cTicketSubType[1] == '0')
	{
		m_cbPayType.AddString("0-��Ʊ�ͳ�ʱ����֪ͨ����");
		m_cbPayType.AddString("1-��Ʊ�ͳ�ʱ����������");
		m_cbPayType.AddString("2-����������");
		m_cbPayType.AddString("3-����������");
		m_cbPayType.AddString("4-���վ����");
		m_cbPayType.AddString("5-���վ����");
		m_cbPayType.AddString("6-����վ����");
		m_cbPayType.AddString("7-����վ����");
	}
	else
	{
		m_cbPayType.AddString("2-����������");
		m_cbPayType.AddString("3-����������");
		m_cbPayType.AddString("4-���վ����");
		m_cbPayType.AddString("5-���վ����");
		m_cbPayType.AddString("6-����վ����");
		m_cbPayType.AddString("7-����վ����");
	}
	m_cbPayType.EnableWindow(TRUE);

}

void CDlgUpd::SetTradeAmount(BYTE bUpdateType, WORD wEntryStation)
{
	CString str;
	CString strTemp;
	long lUpdateValue	= 0;
	BYTE bRecv[64]		= {0};

	if (bUpdateType == 2)
		lUpdateValue = g_Analyse.lPenalty1;
	else
		lUpdateValue = g_Analyse.lPenalty;

	m_cbEntryStation.GetWindowText(strTemp);
	if (bUpdateType == 3 && strTemp.GetLength() > 0)
	{
		BYTE bData[32] = {0};
		memcpy(bData, g_Analyse.bTicketType, 2);
		memcpy(bData + 2, &g_Analyse.lBalance, 4);
		String2HexSeq(strTemp.GetBuffer(), 4, bData + 6, 2);

		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 49, bData, 8, bRecv, sizeof(bRecv));
		if (g_retInfo.wErrCode == 0)
		{
			memcpy(&lUpdateValue, bRecv + 14, 4);
			if (lUpdateValue > 0)
			{
				m_cbPayType.AddString("1-�ֽ�");
				m_cbPayType.SetCurSel(0);
				m_cbPayType.EnableWindow(FALSE);
			}
		}
	}
	str.Format("%d", lUpdateValue);

	GetDlgItem( IDC_EDIT_VALUE)->SetWindowText(str);
}

// ������վ��·������
void CDlgUpd::SetEntryLine()
{
	CString strItem;

	m_cbEntryLine.ResetContent();
	for (map<BYTE, LINEINF>::iterator itor=g_PrmStation.begin();itor!=g_PrmStation.end();itor++)
	{
		strItem.Format("%02X %s", itor->first, itor->second.lineName);
		m_cbEntryLine.AddString(strItem);
	}
}

// ������վվ��������
void CDlgUpd::SetEntryStation(BYTE bLineNo)
{
	CString strItem;

	m_cbEntryStation.ResetContent();
	for (map<BYTE, LINEINF>::iterator itor=g_PrmStation.begin();itor!=g_PrmStation.end();itor++)
	{
		if (itor->first == bLineNo)
		{
			for (size_t sta_cnt=0;sta_cnt<itor->second.vsta.size();sta_cnt++)
			{
				strItem.Format("%04X %s", itor->second.vsta[sta_cnt].first, itor->second.vsta[sta_cnt].second);
				m_cbEntryStation.AddString(strItem);
			}
			break;
		}
	}
}

BEGIN_MESSAGE_MAP(CDlgUpd, CDialog)
	ON_BN_CLICKED(IDC_BN_UPDATE, &CDlgUpd::OnBnClickedBnUpdate)
	ON_CBN_SELCHANGE(IDC_COMB_ENTRY_STATION, &CDlgUpd::OnCbnSelchangeCombEntryStation)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CDlgUpd::OnBnClickedBnCancel)
	ON_CBN_SELCHANGE(IDC_COMB_ENTRY_LINE, &CDlgUpd::OnCbnSelchangeCombEntryLine)
	ON_CBN_SELCHANGE(IDC_COMB_PAY_TYPE, &CDlgUpd::OnCbnSelchangeCombPayType)
	ON_CBN_SELCHANGE(IDC_COMB_UPD_TYPE, &CDlgUpd::OnCbnSelchangeCombUpdType)
	ON_EN_CHANGE(IDC_EDIT_VALUE, &CDlgUpd::OnEnChangeEditValue)
END_MESSAGE_MAP()


// CDlgUpd ��Ϣ�������
void CDlgUpd::Decrease(long lDecVal)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PEPURSETRADE ps		= {0};
	WORD lenps			= 0;
	BYTE bRecv[512]		= {0};
	int pos				= 7;

	do 
	{
		m_cbEntryLine.EnableWindow();
		m_cbEntryStation.EnableWindow();
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 38, (LPBYTE)(&lDecVal), sizeof(lDecVal), bRecv, sizeof(bRecv));
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

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Purse, &ps, lenps, "���۷���", g_liStart, g_liEnd);
}

void CDlgUpd::OnBnClickedBnUpdate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	TICKETUPDATE tu = { 0 };
	ETICKETDEALINFO ts = { 0 };
	WORD lentu = 0;
	BYTE bData[128] = { 0 };
	BYTE bRecv[1024] = { 0 };
	int pos = 7;
	WORD wUpdValue = 0;
	CString strTemp;
	BYTE nUpdateType = 0;
	WORD wEntryID = 0;
	BYTE bPayType = 1;
	BYTE zChange[2] = {0};
	char zUpdateType[2] = { 0 };
	char zEntryID[4] = { 0 };
	char zUpdValue[2] = { 0 };
	if ((g_Analyse.wError == 0) && (g_Analyse.wError1 == 0))
	{
		
		do//����Ʊ������
		{
			m_cbPayType.GetWindowText(strTemp);
			nUpdateType = (BYTE)atoi(strTemp.Left(1));
			zUpdateType[0] = Hex2Char((nUpdateType >> 4) & 0x0F);
			zUpdateType[1] = Hex2Char(nUpdateType & 0x0F);
			m_cbEntryStation.GetWindowText(strTemp);
			wEntryID = (WORD)atoi(strTemp.Left(4));
			
			zChange[0] = Hex_Ex_Bcd((BYTE)(wEntryID / 100), true);
			zChange[1] = Hex_Ex_Bcd((BYTE)(wEntryID % 100), true);
			
			zEntryID[0] = Hex2Char((zChange[0] >> 4) & 0x0F);
			zEntryID[1] = Hex2Char(zChange[0] & 0x0F);
			zEntryID[2] = Hex2Char((zChange[1] >> 4) & 0x0F);
			zEntryID[3] = Hex2Char(zChange[1] & 0x0F);

			GetDlgItemText(IDC_EDIT_VALUE, strTemp);
			wUpdValue = (WORD)atoi(strTemp);
			memcpy(zUpdValue, &wUpdValue, 4);
			
			memcpy(bData , zUpdateType, 2);
			memcpy(bData + 2, &g_Eticanalyse.cCenterCode, 32);
			memcpy(bData + 34, &g_Eticanalyse.lPenalty1, 4);
			memcpy(bData + 38, &g_Eticanalyse.lPenalty2, 4);
			memcpy(bData + 42, &zEntryID, 4);
			
			QueryPerformanceCounter(&g_liStart);
			g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 51, bData, 46, bRecv, sizeof(bRecv));
			QueryPerformanceCounter(&g_liEnd);

			if (g_retInfo.wErrCode != 0)
				break;
			pos += sizeof(RETINFO);

			memcpy(&lentu, bRecv + pos, 2);
			pos += 2;

			if (lentu > 0)
			{
				memcpy(&ts, bRecv + pos, lentu);
			}
			
		} while (0);
		((CCSTesterDlg*)AfxGetMainWnd())->DispResult(struct_ETDealInfo, &ts, lentu, "����Ʊ������", g_liStart, g_liEnd);
	}
	else
	{
		do
		{
			m_cbUpdType.GetWindowText(strTemp);
			nUpdateType = (BYTE)atoi(strTemp.Left(2));

			m_cbEntryStation.GetWindowText(strTemp);
			wEntryID = (WORD)atoi(strTemp.Left(4));

			GetDlgItemText(IDC_EDIT_VALUE, strTemp);
			wUpdValue = (WORD)atoi(strTemp);

			m_cbPayType.GetWindowText(strTemp);
			bPayType = (BYTE)atoi(strTemp.Left(1));

			if (bPayType >= 2 && wUpdValue > 0)
			{
				memset(&g_retInfo, 0, sizeof(RETINFO));
				Decrease(wUpdValue);

				if (g_retInfo.wErrCode != 0)
					break;
			}

			bData[0] = nUpdateType;
			bData[1] = Hex_Ex_Bcd((BYTE)(wEntryID / 100), true);
			bData[2] = Hex_Ex_Bcd((BYTE)(wEntryID % 100), true);
			memcpy(bData + 3, &wUpdValue, 2);
			bData[5] = bPayType;

			QueryPerformanceCounter(&g_liStart);
			g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 37, bData, 6, bRecv, sizeof(bRecv));
			QueryPerformanceCounter(&g_liEnd);

			if (g_retInfo.wErrCode != 0)
				break;
			pos += sizeof(RETINFO);

			memcpy(&lentu, bRecv + pos, 2);
			pos += 2;

			if (lentu > 0)
			{
				memcpy(&tu, bRecv + pos, lentu);
			}

		} while (0);
		((CCSTesterDlg*)AfxGetMainWnd())->DispResult(struct_Update, &tu, lentu, "Ʊ������", g_liStart, g_liEnd);
	}

	((CCSTesterDlg * )AfxGetMainWnd())->TransConfirm(g_retInfo);

	if (m_RetUptate.wErrCode == 0x0030)
	{
		SetUpdInfo(1);
		SendMessage(WM_COMMAND, MAKELONG(IDC_BN_UPDATE, BN_CLICKED));
	}
	else
	{
		m_cbEntryLine.SetCurSel(-1);
		m_cbEntryStation.ResetContent();
		m_cbUpdType.SetCurSel(-1);
		SetDlgItemText(IDC_EDIT_VALUE, "");

		ShowWindow(SW_HIDE);
	}
}

void CDlgUpd::OnCbnSelchangeCombEntryStation()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;

	m_cbEntryStation.GetWindowText(str);
	str = str.Left(4);
	m_wEntryStation = atoi(str);

	SetTradeAmount(m_bUpdateType, m_wEntryStation);
}

void CDlgUpd::OnBnClickedBnCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_cbEntryLine.SetCurSel(-1);
	m_cbEntryStation.ResetContent();
	m_cbUpdType.SetCurSel(-1);
	SetDlgItemText(IDC_EDIT_VALUE, "");

	ShowWindow(SW_HIDE);
}

void CDlgUpd::OnCbnSelchangeCombEntryLine()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	m_cbEntryLine.GetWindowText(str);
	BYTE bLineNo = (BYTE)atoi(str.Left(2));
	SetEntryStation(bLineNo);
}


void CDlgUpd::OnCbnSelchangeCombPayType()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CDlgUpd::OnCbnSelchangeCombUpdType()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CDlgUpd::OnEnChangeEditValue()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
