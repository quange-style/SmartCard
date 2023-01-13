// DlgCmd.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgCmd.h"
#include "CSTesterDlg.h"
#include "PubFuncs.h"
#include "GSFile.h"


// CDlgCmd 对话框

IMPLEMENT_DYNAMIC(CDlgCmd, CDialog)

CDlgCmd::CDlgCmd(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCmd::IDD, pParent)
{

}

CDlgCmd::~CDlgCmd()
{
}

void CDlgCmd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_DEV_TYPE, m_cmdAnt);
	DDX_Control(pDX, IDC_CMB_ANT_TYPE, m_cmbAntType);
	DDX_Control(pDX, IDC_CMB_PORT, m_cmbPort);
	DDX_Control(pDX, IDC_RICHEDIT21, m_rEditDsp);
	DDX_Control(pDX, IDC_CMB_SAM_SOCK, m_cmbSockID);
	DDX_Control(pDX, IDC_CMB_SAM_BAUD, m_cmbSamBaud);
	DDX_Control(pDX, IDC_EDIT1, m_editCmd);
	DDX_Control(pDX, IDC_STATIC_LEN, m_lenCmd);
	DDX_Control(pDX, IDC_CMB_LINE, m_cmbLineNo);
	DDX_Control(pDX, IDC_CMB_KEY, m_cmbKey);
	DDX_Control(pDX, IDC_COMB_CMD, m_cmbCommnd);
}

COLORREF corText1[3] = {RGB(200, 0, 0), RGB(0, 200, 0), RGB(0, 0, 200),};
int g_nCount1 = 0;
void CDlgCmd::AppendText(CString& strText)
{

	int nLen;
	CHARFORMAT cf = {0};
	g_nCount1 = (g_nCount1 + 1) % 3;
	cf.crTextColor = corText1[g_nCount1];
	cf.dwEffects = ~CFE_AUTOCOLOR; 
	cf.dwMask = CFM_COLOR;	


	nLen = m_rEditDsp.GetWindowTextLength(); 
	m_rEditDsp.SetFocus (); 
	m_rEditDsp.SetSel (nLen, nLen); 
	m_rEditDsp.SetWordCharFormat(cf);
	m_rEditDsp.ReplaceSel (strText);
}

void CDlgCmd::DispCmd(CString strOper, RETINFO ret, WORD lenParam, BYTE * pParam, WORD lenRecv, BYTE * pRecv)
{
	CString strOut, strTemp;

	strOut.Format("\t%s:%04X\n\tsend:");

	for (WORD i=0;i<lenParam;i++)
	{
		strTemp.Format("%02X", pParam[i]);
		strOut += strTemp;
	}

	strOut += "\n\trecv:";
	for (WORD i=0;i<lenRecv;i++)
	{
		strTemp.Format("%02X", pRecv[i]);
		strOut += strTemp;
	}
	strOut += "\n\n";
}

void CDlgCmd::DispCmd(CString strOper, RETINFO ret, CMDINF cmdinf)
{
	CString strOut, strTemp;

	strOut.Format("\t%s : %04X\n\tSEND(%04d) : ", strOper, ret.wErrCode, cmdinf.lenSend);

	for (WORD i=0;i<cmdinf.lenSend;i++)
	{
		strTemp.Format("%02X", cmdinf.bSend[i]);
		strOut += strTemp;
	}

	strTemp.Format("\n\tRECV(%04d) : ", cmdinf.lenRecv);
	strOut += strTemp;
	for (WORD i=0;i<cmdinf.lenRecv;i++)
	{
		strTemp.Format("%02X", cmdinf.bRecv[i]);
		strOut += strTemp;
	}
	strOut += "\n\n";

	AppendText(strOut);
}

RETINFO CDlgCmd::RunCommand(CMDINF& cmdinf)
{
	WORD wLenRecv		= 0;
	BYTE bRecv[1024]	= {0};
	BYTE ant			= (BYTE)m_cmdAnt.GetCurSel();
	int pos				= 7;

	RETINFO ret = g_Serial.Communicate(ant, 0, cmdinf.cmdMain, cmdinf.cmdSub, cmdinf.bSend, cmdinf.lenSend, bRecv, sizeof(bRecv));

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

BEGIN_MESSAGE_MAP(CDlgCmd, CDialog)
	ON_BN_CLICKED(IDC_BN_TEST, &CDlgCmd::OnBnClickedBnTest)
	ON_BN_CLICKED(IDC_BN_TESTKK, &CDlgCmd::OnBnClickedBnTestkk)
	ON_BN_CLICKED(IDC_BN_RF_VER, &CDlgCmd::OnBnClickedBnRfVer)
	ON_BN_CLICKED(IDC_BN_MInit, &CDlgCmd::OnBnClickedBnMinit)
	ON_BN_CLICKED(IDC_BN_MATC, &CDlgCmd::OnBnClickedBnMatc)
	ON_BN_CLICKED(IDC_BN_MSel, &CDlgCmd::OnBnClickedBnMsel)
	ON_BN_CLICKED(IDC_BN_MREAD, &CDlgCmd::OnBnClickedBnMread)
	ON_BN_CLICKED(IDC_BN_MWRITE, &CDlgCmd::OnBnClickedBnMwrite)
	ON_BN_CLICKED(IDC_BN_MAUTH, &CDlgCmd::OnBnClickedBnMauth)
	ON_BN_CLICKED(IDC_BN_UL_REQ, &CDlgCmd::OnBnClickedBnUlReq)
	ON_BN_CLICKED(IDC_BN_UL_ANTC, &CDlgCmd::OnBnClickedBnUlAntc)
	ON_BN_CLICKED(IDC_BN_UL_SEL, &CDlgCmd::OnBnClickedBnUlSel)
	ON_BN_CLICKED(IDC_BN_UL_READ, &CDlgCmd::OnBnClickedBnUlRead)
	ON_BN_CLICKED(IDC_BN_UL_WRITE, &CDlgCmd::OnBnClickedBnUlWrite)
	ON_BN_CLICKED(IDC_BN_UL_CWRITE, &CDlgCmd::OnBnClickedBnUlCwrite)
	ON_BN_CLICKED(IDC_BN_CPU_RST, &CDlgCmd::OnBnClickedBnCpuRst)
	ON_BN_CLICKED(IDC_BN_CPU_CMD, &CDlgCmd::OnBnClickedBnCpuCmd)
	ON_BN_CLICKED(IDC_BN_SAM_BAUD, &CDlgCmd::OnBnClickedBnSamBaud)
	ON_BN_CLICKED(IDC_BN_SAM_RST, &CDlgCmd::OnBnClickedBnSamRst)
	ON_BN_CLICKED(IDC_BN_SAM_CMD, &CDlgCmd::OnBnClickedBnSamCmd)
	ON_BN_CLICKED(IDC_BN_OPEN, &CDlgCmd::OnBnClickedBnOpen)
	ON_BN_CLICKED(IDC_BN_REFRESH, &CDlgCmd::OnBnClickedBnRefresh)
	ON_BN_CLICKED(IDC_BN_CLOSE, &CDlgCmd::OnBnClickedBnClose)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_EDIT1, &CDlgCmd::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BN_MREQ, &CDlgCmd::OnBnClickedBnMreq)
	ON_BN_CLICKED(IDC_BN_UL_ANTC2, &CDlgCmd::OnBnClickedBnUlAntc2)
	ON_BN_CLICKED(IDC_BN_UL_SEL2, &CDlgCmd::OnBnClickedBnUlSel2)
	ON_BN_CLICKED(IDC_BN_LOAD, &CDlgCmd::OnBnClickedBnLoad)
	ON_CBN_SELCHANGE(IDC_COMB_CMD, &CDlgCmd::OnCbnSelchangeCombCmd)
	ON_BN_CLICKED(IDC_BN_TAC, &CDlgCmd::OnBnClickedBnTac)
END_MESSAGE_MAP()


// CDlgCmd 消息处理程序

BOOL CDlgCmd::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_cmdAnt.SetCurSel(0);
	m_cmbAntType.SetCurSel(0);
	m_cmbSockID.SetCurSel(0);
	m_cmbSamBaud.SetCurSel(2);
	m_cmbKey.SetCurSel(0);

	char szData[8] = {0};
	for (int i=0;i<64;i++)
	{
		sprintf(szData, "%d", i);
		m_cmbLineNo.InsertString(i, szData);
	}
	m_cmbLineNo.SetCurSel(0);

	m_editCmd.SetWindowText("00A40000023F00");
//	m_editCmd
//12A7D1AAF452
	CCSTesterDlg::EnumSerials(m_cmbPort);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
	
}

void CDlgCmd::OnBnClickedBnRfVer()
{
	// TODO: 在此添加控件通知处理程序代码
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x02;
	g_retInfo = RunCommand(cmd);
	DispCmd("rf_version", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnMinit()
{
	// TODO: 在此添加控件通知处理程序代码
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x03;
	cmd.lenSend = 1;
	cmd.bSend[0] = (BYTE)m_cmbAntType.GetCurSel();

	g_retInfo = RunCommand(cmd);
	DispCmd("14443A_Init", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnMreq()
{
	// TODO: 在此添加控件通知处理程序代码
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x04;
	cmd.lenSend = 1;
	cmd.bSend[0] = 0x52;
	g_retInfo = RunCommand(cmd);
	DispCmd("14443A_Request", g_retInfo, cmd);
}

LASTRESULT anticoll_rslt;
void CDlgCmd::OnBnClickedBnMatc()
{
	// TODO: 在此添加控件通知处理程序代码

	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x05;
	g_retInfo = RunCommand(cmd);
	DispCmd("14443A_Anticoll", g_retInfo, cmd);

	memset(&anticoll_rslt, 0, sizeof(LASTRESULT));
	if (g_retInfo.wErrCode == 0)
	{
		anticoll_rslt.len = cmd.lenRecv;
		memcpy(anticoll_rslt.recv, cmd.bRecv, cmd.lenRecv);
	}
}

void CDlgCmd::OnBnClickedBnMsel()
{
	// TODO: 在此添加控件通知处理程序代码
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x06;
	cmd.lenSend = anticoll_rslt.len;
	memcpy(cmd.bSend, anticoll_rslt.recv, cmd.lenSend);
	g_retInfo = RunCommand(cmd);

	DispCmd("14443A_Select", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnMread()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_cmbLineNo.GetCurSel() < 0)
	{
		MessageBox("请选择要读取的行!");
		return;
	}

	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x07;
	cmd.lenSend = 1;
	cmd.bSend[0] = (BYTE)m_cmbLineNo.GetCurSel();
	g_retInfo = RunCommand(cmd);
	DispCmd("14443A_Read_Block", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnMwrite()
{
	// TODO: 在此添加控件通知处理程序代码

	CString strLineData;
	m_editCmd.GetWindowText(strLineData);
	if (strLineData.GetLength() != 32)
	{
		MessageBox("数据长度错误!");
		return;
	}

	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x08;

	cmd.lenSend = 1 + (strLineData.GetLength() / 2);
	cmd.bSend[0] = (BYTE)m_cmbLineNo.GetCurSel();
	String2HexSeq(strLineData.GetBuffer(), strLineData.GetLength(), cmd.bSend + 1, sizeof(cmd.bSend) - 1);

	g_retInfo = RunCommand(cmd);
	DispCmd("14443A_Write_Block", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnMauth()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strKey;
	m_editCmd.GetWindowText(strKey);

	int nCurselKey = m_cmbKey.GetCurSel();

	if (nCurselKey < 0)
	{
		MessageBox("请选择认证密钥!");
		return;
	}
	if (m_cmbLineNo.GetCurSel() < 0)
	{
		MessageBox("请选择要认证的行!");
		return;
	}
	if (strKey.GetLength() != 12)
	{
		MessageBox("密钥长度错误!");
		return;
	}

	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x09;

	cmd.lenSend = 2 + anticoll_rslt.len + (strKey.GetLength() / 2);

	if (nCurselKey == 0)
		cmd.bSend[0] = 0x60;
	else if (nCurselKey == 1)
		cmd.bSend[0] = 0x61;

	cmd.bSend[1] = (BYTE)m_cmbLineNo.GetCurSel();

	memcpy(cmd.bSend + 2, anticoll_rslt.recv, anticoll_rslt.len);
	String2HexSeq(strKey.GetBuffer(), strKey.GetLength(), cmd.bSend + 2 + anticoll_rslt.len, sizeof(cmd.bSend) - 2 - anticoll_rslt.len);

	QueryPerformanceCounter(&g_liStart);
	g_retInfo = RunCommand(cmd);
	QueryPerformanceCounter(&g_liEnd);
	CString strTime;
	strTime.Format("%8lf", ((double)(g_liEnd.QuadPart - g_liStart.QuadPart) / (double)(g_liCpuFreq.QuadPart)));
	DispCmd("14443A_Authentication", g_retInfo, cmd);

	MessageBox(strTime);
}

void CDlgCmd::OnBnClickedBnUlReq()
{
	// TODO: 在此添加控件通知处理程序代码
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x0A;
	cmd.lenSend = 1;
	cmd.bSend[0] = 0x52;
	g_retInfo = RunCommand(cmd);
	DispCmd("Ul_Request", g_retInfo, cmd);
}

LASTRESULT ulatll_last;
void CDlgCmd::OnBnClickedBnUlAntc()
{
	// TODO: 在此添加控件通知处理程序代码
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x0B;
	cmd.lenSend = 1;
	cmd.bSend[0] = 0x93;
	g_retInfo = RunCommand(cmd);
	DispCmd("Ul_Anticoll_93", g_retInfo, cmd);

	memset(&ulatll_last, 0, sizeof(LASTRESULT));
	if (g_retInfo.wErrCode == 0)
	{
		ulatll_last.len = cmd.lenRecv;
		memcpy(ulatll_last.recv, cmd.bRecv, ulatll_last.len);
	}
}

void CDlgCmd::OnBnClickedBnUlSel()
{
	// TODO: 在此添加控件通知处理程序代码
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x0C;
	cmd.lenSend = 1 + ulatll_last.len;
	cmd.bSend[0] = 0x93;
	memcpy(cmd.bSend + 1, ulatll_last.recv, ulatll_last.len);
	g_retInfo = RunCommand(cmd);
	DispCmd("Ul_Select_93", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnUlAntc2()
{
	// TODO: 在此添加控件通知处理程序代码
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x0B;
	cmd.lenSend = 1;
	cmd.bSend[0] = 0x95;
	g_retInfo = RunCommand(cmd);
	DispCmd("Ul_Anticoll_95", g_retInfo, cmd);

	memset(&ulatll_last, 0, sizeof(LASTRESULT));
	if (g_retInfo.wErrCode == 0)
	{
		ulatll_last.len = cmd.lenRecv;
		memcpy(ulatll_last.recv, cmd.bRecv, ulatll_last.len);
	}
}

void CDlgCmd::OnBnClickedBnUlSel2()
{
	// TODO: 在此添加控件通知处理程序代码
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x0C;
	cmd.lenSend = 1 + ulatll_last.len;
	cmd.bSend[0] = 0x95;
	memcpy(cmd.bSend + 1, ulatll_last.recv, ulatll_last.len);
	g_retInfo = RunCommand(cmd);
	DispCmd("Ul_Select_95", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnUlRead()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_cmbLineNo.GetCurSel() < 0)
	{
		MessageBox("请选择要读的页号!");
		return;
	}
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x0D;
	cmd.lenSend = 1;
	cmd.bSend[0] = (BYTE)m_cmbLineNo.GetCurSel();
	g_retInfo = RunCommand(cmd);
	DispCmd("Ul_Read_Block", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnUlWrite()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strData;
	if (m_cmbLineNo.GetCurSel() < 0)
	{
		MessageBox("请选择要写的页号!");
		return;
	}
	m_editCmd.GetWindowText(strData);
	if (strData.GetLength() != 8)
	{
		MessageBox("数据长度错误!");
		return;
	}

	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x0E;

	cmd.lenSend = 5;
	cmd.bSend[0] = (BYTE)m_cmbLineNo.GetCurSel();
	String2HexSeq(strData.GetBuffer(), strData.GetLength(), cmd.bSend + 1, sizeof(cmd.bSend) - 1);

	g_retInfo = RunCommand(cmd);
	DispCmd("Ul_Write_Block", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnUlCwrite()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strData;
	m_editCmd.GetWindowText(strData);
	if (strData.GetLength() != 32)
	{
		MessageBox("数据长度错误!");
		return;
	}

	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x0F;

	cmd.lenSend = 17;
	cmd.bSend[0] = (BYTE)m_cmbLineNo.GetCurSel();
	String2HexSeq(strData.GetBuffer(), strData.GetLength(), cmd.bSend + 1, sizeof(cmd.bSend) - 1);

	g_retInfo = RunCommand(cmd);
	DispCmd("Ul_CWrite_Block", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnCpuRst()
{
	// TODO: 在此添加控件通知处理程序代码
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x10;
	cmd.lenSend = 1;
	cmd.bSend[0] = 0x40;
	g_retInfo = RunCommand(cmd);
	DispCmd("MifareProRst", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnCpuCmd()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strCmd;
	m_editCmd.GetWindowText(strCmd);

	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x011;
	cmd.bSend[0] = (BYTE)(strCmd.GetLength() / 2);
	String2HexSeq(strCmd.GetBuffer(), strCmd.GetLength(), cmd.bSend + 1, sizeof(cmd.bSend) - 1);
	cmd.lenSend = 1 + cmd.bSend[0];
	g_retInfo = RunCommand(cmd);
	DispCmd("MifareProCom", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnSamBaud()
{
	// TODO: 在此添加控件通知处理程序代码

	if (m_cmbSockID.GetCurSel() < 0)
	{
		MessageBox("请选择SAM卡槽号!");
		return;
	}
	if (m_cmbSamBaud.GetCurSel() < 0)
	{
		MessageBox("请选择SAM波特率");
		return;
	}

	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x12;
	cmd.lenSend = 2;
	cmd.bSend[0] = (BYTE)m_cmbSockID.GetCurSel();
	cmd.bSend[1] = (BYTE)m_cmbSamBaud.GetCurSel();
	g_retInfo = RunCommand(cmd);

	DispCmd("Sam_Baud_Set", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnSamRst()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_cmbSockID.GetCurSel() < 0)
	{
		MessageBox("请选择SAM卡槽号!");
		return;
	}

	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x13;
	cmd.lenSend = 1;
	cmd.bSend[0] = (BYTE)m_cmbSockID.GetCurSel();
	g_retInfo = RunCommand(cmd);

	DispCmd("Sam_Reset", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnSamCmd()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strCmd;
	m_editCmd.GetWindowText(strCmd);

	if (m_cmbSockID.GetCurSel() < 0)
	{
		MessageBox("请选择SAM卡槽号!");
		return;
	}

	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x14;
	cmd.bSend[0] = (BYTE)m_cmbSockID.GetCurSel();
	cmd.bSend[1] = (BYTE)(strCmd.GetLength() / 2);
	cmd.lenSend = 2 + cmd.bSend[1];

	String2HexSeq(strCmd.GetBuffer(), strCmd.GetLength(), cmd.bSend + 2, sizeof(cmd.bSend) - 2);

	g_retInfo = RunCommand(cmd);

	DispCmd("Sam_Command", g_retInfo, cmd);
}

void CDlgCmd::OnBnClickedBnTest()
{
	// TODO: 在此添加控件通知处理程序代码

	BYTE bData[8]		= {0};
	BYTE bRecv[512]		= {0};
	BYTE ant			= (BYTE)m_cmdAnt.GetCurSel();

	bData[0] = (BYTE)m_cmbAntType.GetCurSel();
	g_Serial.Communicate(ant, 0, 0x10, 0x00, bData, 1, bRecv, sizeof(bRecv));

}

void CDlgCmd::OnBnClickedBnTestkk()
{
	// TODO: 在此添加控件通知处理程序代码
	BYTE bData[8]		= {0};
	BYTE bRecv[512]		= {0};
	BYTE ant			= (BYTE)m_cmdAnt.GetCurSel();

	bData[0] = (BYTE)m_cmbAntType.GetCurSel();
	g_Serial.Communicate(ant, 0, 0x10, 0x01, bData, 1, bRecv, sizeof(bRecv));

}

void CDlgCmd::OnBnClickedBnOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPort;
	m_cmbPort.GetWindowText(strPort);
	g_nPortOpen = atoi(strPort.Right(strPort.GetLength() - 3));
	g_retInfo.wErrCode = g_Serial.Open(g_nPortOpen);
	if (g_retInfo.wErrCode == 0)
	{
		GetDlgItem(IDC_BN_OPEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_CLOSE)->EnableWindow(TRUE);
	}
}

void CDlgCmd::OnBnClickedBnRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	CCSTesterDlg::EnumSerials(m_cmbPort);
}

void CDlgCmd::OnBnClickedBnClose()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_Serial.IsOpen())
	{
		g_retInfo.wErrCode = g_Serial.Close();
		GetDlgItem(IDC_BN_OPEN)->EnableWindow(TRUE);
	}
}

void CDlgCmd::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (g_Serial.IsOpen())
		g_Serial.Close();
	CDialog::OnClose();
}

void CDlgCmd::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString strCmd;
	m_editCmd.GetWindowText(strCmd);
	strCmd.Format("%d", strCmd.GetLength());
	m_lenCmd.SetWindowText(strCmd);
}

void CDlgCmd::OnBnClickedBnLoad()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE, "txt", NULL, 4 | 2, "文本文件 (*.txt)|*.txt|所有文件 (*.*)|*.*||");
	if (dlg.DoModal() == IDOK)
	{
		CString strRead;
		CString strPath = dlg.GetPathName();
		CStdioFile fl;
		if (fl.Open(strPath, CFile::modeRead))
		{
			m_cmbCommnd.ResetContent();
			while(fl.ReadString(strRead))
			{
				if (strRead.Left(1) != "\n" && strRead.Left(2) != "//" && strRead != "")
					m_cmbCommnd.AddString(strRead);
			}
		}
	}
}

void CDlgCmd::OnCbnSelchangeCombCmd()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strCmd;
	m_cmbCommnd.GetWindowText(strCmd);

	int nPosExplain = strCmd.Find("//");

	if (nPosExplain > 0)
	{
		strCmd = strCmd.Left(nPosExplain);
		strCmd.TrimRight();
	}

	m_editCmd.SetWindowText(strCmd);
}

void CDlgCmd::OnBnClickedBnTac()
{
	// TODO: 在此添加控件通知处理程序代码
	char szFilter[1024] = {0};
	strcat(szFilter, "TAB文件 (*.tab)|*.tab|");
	strcat(szFilter, "所有文件 (*.*)|*.*|");
	char cReadBuf[1024] = {0};
	bool bRepair = false;

	CString strFileName, strFolder, strPathName, strNewPathName;
	GSFile fileSrc;
	CStdioFile fileTarg;

	do 
	{
		CFileDialog dlg(TRUE, "TAB", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
		if (dlg.DoModal() != IDOK)
			break;

		strFileName = dlg.GetFileName();
		strPathName = dlg.GetPathName();
		strFolder = strPathName.Left(strPathName.GetLength() - strFileName.GetLength());
		strNewPathName = strFolder + "Checked_" + strFileName;

		if (!fileSrc.Open(strFileName, ModeRead))
		{
			MessageBox("打开文件错误");
			break;
		}

		if (!fileTarg.Open(strNewPathName, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("转存文件错误");
			break;
		}

		//if (!SamReset())
		//{
		//	MessageBox("SAM复位错误");
		//	break;
		//}

		while(fileSrc.ReadLine(cReadBuf, sizeof(cReadBuf)))
		{
			if (!CheckOneRecord(cReadBuf))
			{
				bRepair = false;
				break;
			}
			fileTarg.WriteString(cReadBuf);
		}

		MessageBox("完成");
		bRepair = true;

	} while (0);

	if (fileSrc.IsOpen())
		fileSrc.Close();
	if (!fileTarg.GetFileName().IsEmpty())
		fileTarg.Close();

}

bool CDlgCmd::CheckOneRecord(char * strBufferToCheck)
{
	BYTE bLogicalID[4] = {0};
	BYTE bTimeTrade[7] = {0};
	long lBalance = 0;
	long lTradeAmount = 0;
	long lTradeCount = 0;
	char szNewMac[11] = {0};

	int pos = GetTacFactor(strBufferToCheck, bLogicalID, bTimeTrade, lBalance, lTradeAmount, lTradeCount);
	if (GetOctMfTAC(bLogicalID, bTimeTrade, lBalance, lTradeAmount, lTradeCount, szNewMac))
	{
		memcpy(strBufferToCheck + pos, szNewMac, 10);
		strcat(strBufferToCheck, "\n");
		return true;
	}

	return false;
}

// 获取TAC计算因子,并返回TAC在但条记录的位置
int CDlgCmd::GetTacFactor(char * pRecord, LPBYTE lpLogiclID, LPBYTE lpTimeTrade, long& lBalanceAfter, long& lTradeAmount, long& lTradeCounter)
{
	int tabCouter = 0;
	char szTemp[32];
	int ret = 0;

	for (DWORD i=0;i<strlen(pRecord);i++)
	{
		if (pRecord[i] == '\t')
		{
			i++;

			if (tabCouter == 6)			// 交易时间
			{
				memset(szTemp, 0, sizeof(szTemp));
				memcpy(szTemp, pRecord + i, 4);
				memcpy(szTemp + 4, pRecord + i + 5, 2);
				memcpy(szTemp + 6, pRecord + i + 8, 2);
				memcpy(szTemp + 8, pRecord + i + 11, 2);
				memcpy(szTemp + 10, pRecord + i + 14, 2);
				memcpy(szTemp + 12, pRecord + i + 17, 2);

				String2HexSeq(szTemp, 14, lpTimeTrade, 7);
			}
			else if (tabCouter == 9)	// 票卡逻辑卡号
			{
				memset(szTemp, 0, sizeof(szTemp));
				memcpy(szTemp, pRecord + i, 20);
				String2HexSeq(szTemp + 12, 8, lpLogiclID, 4);
			}
			else if (tabCouter == 12)	// 交易金额
			{
				lTradeAmount = (long)(atof(pRecord + i) * 100);
			}
			else if (tabCouter == 13)	// 余额
			{
				lBalanceAfter = (long)(atof(pRecord + i) * 100);
			}
			else if (tabCouter == 15)	// 交易计数
			{
				lTradeCounter = atol(pRecord + i);
			}
			else if (tabCouter == 18)	// TAC码
			{
				ret = i;
				break;
			}

			tabCouter ++;
		}
	}

	return ret;
}

bool CDlgCmd::GetOctMfTAC(LPBYTE lpLogiclID, LPBYTE lpTimeTrade, long lBalanceAfter, long lTradeAmount, long lTradeCounter, char * lpTAC)
{
	if (TacPrepare(lpLogiclID))
	{
		if (CalTac(lpTimeTrade, lBalanceAfter, lTradeAmount, lTradeCounter, lpTAC))
			return true;
	}

	return false;
}

bool CDlgCmd::SamReset()
{
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x13;
	cmd.lenSend = 1;
	cmd.bSend[0] = 0x02;
	g_retInfo = RunCommand(cmd);

	return (bool)(g_retInfo.wErrCode == 0);
}

bool CDlgCmd::TacPrepare(LPBYTE lpLogiclID)
{
	int off = 0;
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x14;

	// SAM卡槽
	cmd.bSend[off] = 0x02;
	off ++;

	// SAM卡命令数据长度
	cmd.bSend[off] = 8 + 5;		// 逻辑卡号
	off ++;

	// 串口协议数据长度
	cmd.lenSend = 2 + cmd.bSend[1];

	// SAM命令头
	memcpy(cmd.bSend + off, "\x80\x1A\x28\x01\x08", 5);
	off += 5;

	// 票卡逻辑卡号
	memcpy(cmd.bSend + off, lpLogiclID, 4);
	off += 4;

	// 后缀
	memcpy(cmd.bSend + off, "\x80\x00\x00\x00", 4);

	g_retInfo = RunCommand(cmd);

	return (bool)(memcmp(cmd.bRecv, "\x90\x00", 2) == 0 && g_retInfo.wErrCode == 0);
}

bool CDlgCmd::CalTac(LPBYTE lpTimeTrade, long lBalanceAfter, long lTradeAmount, long lTradeCounter, char * pTac)
{
	int off = 0;
	CMDINF cmd = {0};
	cmd.cmdMain = 0x10;
	cmd.cmdSub = 0x14;

	// SAM卡槽
	cmd.bSend[off] = 0x02;
	off ++;

	// SAM卡命令数据长度
	cmd.bSend[off] = 16 + 5;
	off ++;

	// 串口协议数据长度
	cmd.lenSend = 2 + cmd.bSend[1];

	// SAM命令头
	memcpy(cmd.bSend + off, "\x80\xFA\x01\x00\x10", 5);
	off += 5;

	// 消费类型
	cmd.bSend[off] = 0x03;
	off ++;

	// 交易时间
	memcpy(cmd.bSend + off, lpTimeTrade, 7);
	off += 7;

	long lBalanceBefore = lBalanceAfter + lTradeAmount;
	cmd.bSend[off] = (BYTE)((lBalanceBefore >> 24) & 0xFF);
	cmd.bSend[off + 1] = (BYTE)((lBalanceBefore >> 16) & 0xFF);
	cmd.bSend[off + 2] = (BYTE)((lBalanceBefore >> 8) & 0xFF);
	cmd.bSend[off + 3] = (BYTE)((lBalanceBefore >> 0) & 0xFF);
	off += 4;

	cmd.bSend[off] = (BYTE)((lTradeAmount >> 8) & 0xFF);
	cmd.bSend[off + 1] = (BYTE)((lTradeAmount >> 0) & 0xFF);
	off += 2;

	cmd.bSend[off] = (BYTE)((lTradeCounter >> 8) & 0xFF);
	cmd.bSend[off + 1] = (BYTE)((lTradeCounter >> 0) & 0xFF);
	off += 2;

	g_retInfo = RunCommand(cmd);

	if (g_retInfo.wErrCode == 0 && memcmp(cmd.bRecv + 4, "\x90\x00", 2) == 0)
	{
		sprintf(pTac, "%02X%02X%02X%02X  ", cmd.bRecv[0], cmd.bRecv[1], cmd.bRecv[2], cmd.bRecv[3]);
		return true;
	}

	return false;
}
