// DlgHarder.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgHarder.h"
#include "CSTesterDlg.h"

BOOL CDlgHarder::m_bEndThread = TRUE;
BOOL CDlgHarder::m_bEndCheck = TRUE;
BOOL CDlgHarder::m_bJumpToNext = FALSE;
HANDLE CDlgHarder::m_hWaitEvent = NULL;
HANDLE CDlgHarder::m_hCloseEvent = NULL;


// CDlgHarder 对话框

IMPLEMENT_DYNAMIC(CDlgHarder, CDialog)

CDlgHarder::CDlgHarder(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHarder::IDD, pParent)
{
	m_bEndThread = TRUE;
	m_bEndCheck = TRUE;
	m_bJumpToNext = FALSE;
	m_hWaitEvent = NULL;
}

CDlgHarder::~CDlgHarder()
{
}

void CDlgHarder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMB_MAIN_TYPE, m_cmbAntType[0]);
	DDX_Control(pDX, IDC_COMB_SUB_TYPE, m_cmbAntType[1]);
	DDX_Control(pDX, IDC_CHK_MAIN, m_bnAnt[0]);
	DDX_Control(pDX, IDC_CHK_SUB, m_bnAnt[1]);

	DDX_Control(pDX, IDC_CHK_SAM1, m_bnSam[0]);
	DDX_Control(pDX, IDC_CHK_SAM2, m_bnSam[1]);
	DDX_Control(pDX, IDC_CHK_SAM3, m_bnSam[2]);
	DDX_Control(pDX, IDC_CHK_SAM4, m_bnSam[3]);
	DDX_Control(pDX, IDC_CHK_SAM5, m_bnSam[4]);
	DDX_Control(pDX, IDC_CHK_SAM6, m_bnSam[5]);
	DDX_Control(pDX, IDC_CHK_SAM7, m_bnSam[6]);
	DDX_Control(pDX, IDC_CHK_SAM8, m_bnSam[7]);

	DDX_Control(pDX, IDC_COMB_SER, m_cmbSer);
	DDX_Control(pDX, IDC_REDIT_RESULT, m_rEditDisp);
	DDX_Control(pDX, IDC_COMB_BAUD, m_cmbBaud);
}


BEGIN_MESSAGE_MAP(CDlgHarder, CDialog)
	ON_BN_CLICKED(IDC_BN_START, &CDlgHarder::OnBnClickedBnStart)
	ON_BN_CLICKED(IDC_BN_NEXT, &CDlgHarder::OnBnClickedBnNext)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CDlgHarder::OnBnClickedBnCancel)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BN_REFRESH, &CDlgHarder::OnBnClickedBnRefresh)
END_MESSAGE_MAP()


// CDlgHarder 消息处理程序

void CDlgHarder::OnBnClickedBnStart()
{
	// TODO: 在此添加控件通知处理程序代码

	CString strPort;
	m_cmbSer.GetWindowText(strPort);
	g_nPortOpen = atoi(strPort.Right(strPort.GetLength() - 3));

	g_retInfo.wErrCode = g_Serial.Open(g_nPortOpen);
	if (g_retInfo.wErrCode == 0)
	{
		for (int i=0;i<8;i++)
		{
			m_bnSam[i].SetCheck(0);
		}
		for (int i=0;i<2;i++)
		{
			m_bnAnt[i].SetCheck(0);
		}

		GetDlgItem(IDC_BN_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_REFRESH)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMB_SER)->EnableWindow(FALSE);

		GetDlgItem(IDC_BN_NEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BN_CANCEL)->EnableWindow(TRUE);

		m_bEndCheck = FALSE;
		m_bEndThread = FALSE;
		m_bJumpToNext = FALSE;

		SetEvent(m_hWaitEvent);
	}
}

void CDlgHarder::OnBnClickedBnNext()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bJumpToNext = TRUE;
	m_rEditDisp.SetFocus();
}

void CDlgHarder::OnBnClickedBnCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bEndCheck = TRUE;

	if (WaitForSingleObject(m_hWaitEvent, INFINITE) == WAIT_OBJECT_0)
	{
		if (g_Serial.IsOpen())
			g_Serial.Close();

		GetDlgItem(IDC_BN_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_BN_REFRESH)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMB_SER)->EnableWindow(TRUE);

		GetDlgItem(IDC_BN_NEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_CANCEL)->EnableWindow(FALSE);

		AppendText(m_rEditDisp, "\t检测终止\n\n", RGB(200, 10, 10));
	}
}

void CDlgHarder::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bEndCheck = TRUE;
	m_bEndThread = TRUE;

	SetEvent(m_hWaitEvent);
	WaitForSingleObject(m_hCloseEvent, INFINITE);

	CloseHandle(m_hCloseEvent);
	m_hCloseEvent = NULL;

	CloseHandle(m_hWaitEvent);
	m_hWaitEvent = NULL;

	if (g_Serial.IsOpen())
		g_Serial.Close();

	CDialog::OnClose();
}

BOOL CDlgHarder::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CCSTesterDlg::EnumSerials(m_cmbSer);
	m_cmbBaud.SetCurSel(2);

	m_cmbAntType[0].SetCurSel(1);
	m_cmbAntType[1].SetCurSel(1);

	m_bEndThread = FALSE;
	m_bEndCheck = FALSE;
	m_bJumpToNext = FALSE;

	m_hWaitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hCloseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)HardCheckPro, this, 0, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgHarder::OnBnClickedBnRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	CCSTesterDlg::EnumSerials(m_cmbSer);
}

void CDlgHarder::HardCheckPro(LPVOID lparam)
{
	CDlgHarder * pDlg = (CDlgHarder *)lparam;
	CString strNotice, strTemp;
	BYTE bSock, bBaud;
	BYTE bMode, bType;

	while (!m_bEndThread)
	{
		if (WaitForSingleObject(m_hWaitEvent, INFINITE) == WAIT_OBJECT_0 && !m_bEndThread)
		{
			for (bSock=0;bSock<8 && !m_bEndCheck;bSock++)
			{
				bBaud = (BYTE)(pDlg->m_cmbBaud.GetCurSel());

				strNotice.Format("\t卡槽%d检测中……\n", bSock + 1);
				AppendText(pDlg->m_rEditDisp, strNotice);

				while (!m_bEndCheck)
				{
					if (m_bJumpToNext)
					{
						strNotice.Format("\t卡槽%d已跳过\n", bSock + 1);
						AppendText(pDlg->m_rEditDisp, strNotice, RGB(200, 10, 10));
						m_bJumpToNext = FALSE;
						break;
					}

					if (pDlg->CheckSamSock(bSock, bBaud))
					{
						pDlg->m_bnSam[bSock].SetCheck(1);

						strNotice.Format("\t卡槽%d检测，OK！\n", bSock + 1);
						AppendText(pDlg->m_rEditDisp, strNotice);
						break;
					}
				}
			}

			for (bMode=0;bMode<2,!m_bEndCheck;bMode++)
			{
				bType = (BYTE)(pDlg->m_cmbAntType[bMode].GetCurSel());
				
				pDlg->m_bnAnt[bMode].GetWindowText(strTemp);
				strNotice.Format("\t%s天线检测中……\n", strTemp);
				AppendText(pDlg->m_rEditDisp, strNotice);

				while (!m_bEndCheck)
				{
					if (m_bJumpToNext)
					{
						m_bJumpToNext = FALSE;
						break;
					}

					if (pDlg->CheckAnt(bMode, bType) && !m_bEndCheck)
					{
						pDlg->m_bnAnt[bMode].SetCheck(1);
					}
				}
			}
		}

		if (!m_bEndCheck)
		{
			AppendText(pDlg->m_rEditDisp, "\t检测完毕！\n\n");

			if (g_Serial.IsOpen())
				g_Serial.Close();

			pDlg->GetDlgItem(IDC_BN_START)->EnableWindow(TRUE);
			pDlg->GetDlgItem(IDC_BN_REFRESH)->EnableWindow(TRUE);
			pDlg->GetDlgItem(IDC_COMB_SER)->EnableWindow(TRUE);

			pDlg->GetDlgItem(IDC_BN_NEXT)->EnableWindow(FALSE);
			pDlg->GetDlgItem(IDC_BN_CANCEL)->EnableWindow(FALSE);

			continue;
		}

		SetEvent(m_hWaitEvent);
	}

	SetEvent(m_hCloseEvent);
}

void CDlgHarder::AppendText(CRichEditCtrl& edt, CString strText, COLORREF cor)
{
	int nLen;
	CHARFORMAT cf = {0};
	cf.crTextColor = cor;
	cf.dwEffects = ~CFE_AUTOCOLOR; 
	cf.dwMask = CFM_COLOR;	

	nLen = edt.GetWindowTextLength();
	edt.SetFocus (); 
	edt.SetSel (nLen, nLen); 
	edt.SetWordCharFormat(cf);
	edt.ReplaceSel (strText);
}


RETINFO CDlgHarder::SamBaud(BYTE nSockID, BYTE nBaudID)
{
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x12;
	cmd.lenSend = 2;
	cmd.bSend[0] = nSockID;
	cmd.bSend[1] = nBaudID;
	
	return RunCommand(cmd, 0, 0);
}

RETINFO CDlgHarder::SamRst(BYTE nSockID)
{
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x13;
	cmd.lenSend = 1;
	cmd.bSend[0] = nSockID;
	
	return RunCommand(cmd, 0, 1);
}

BOOL CDlgHarder::CheckSamSock(BYTE nSockID, BYTE nBaudID)
{
	RETINFO ret = SamBaud(nSockID, nBaudID);
	if (ret.wErrCode == 0)
	{
		ret = SamRst(nSockID);
	}

	return (BOOL)(ret.wErrCode == 0);
}

BOOL CDlgHarder::CheckAnt(BYTE antMode, BYTE antType)
{
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x15;
	cmd.lenSend = 1;
	cmd.bSend[0] = antType;
	RETINFO ret = RunCommand(cmd, antMode, 1);

	return (BOOL)(ret.wErrCode == 0);
}

RETINFO CDlgHarder::RunCommand(CMDINF& cmdinf, BYTE ant, BYTE beep)
{
	WORD wLenRecv		= 0;
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	RETINFO ret = g_Serial.Communicate(ant, beep, cmdinf.cmdMain, cmdinf.cmdSub, cmdinf.bSend, cmdinf.lenSend, bRecv, sizeof(bRecv));

	pos += sizeof(RETINFO);

	memcpy(&cmdinf.lenRecv, bRecv + pos, 2);
	pos += 2;

	if (cmdinf.lenRecv > 0)
	{
		memcpy(cmdinf.bRecv, bRecv + pos, cmdinf.lenRecv);
		pos += cmdinf.lenRecv;
	}

	return ret;
}

LRESULT CDlgHarder::OnEndCheck(WPARAM wparam, LPARAM lparam)
{
	return 0;
}
