// CSTesterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "CSTesterDlg.h"
#include "GSFile.h"
#include "DlgHarder.h"
#include "TradeFileConvert.h"
#include "DlgConfirm.h"
#include "DlgNfs.h"
#include "DlgRoot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LARGE_INTEGER g_liCpuFreq;
LARGE_INTEGER g_liStart;
LARGE_INTEGER g_liEnd;
Serial g_Serial;
int g_nPortOpen;
WORD g_wDeviceID;
WORD g_wStation;
long g_lBalance;
BYTE g_bDeviceType;
BYTE g_nBeep;
map<BYTE, LINEINF> g_PrmStation;

BOOL CCSTesterDlg::m_bEnableDebuger = FALSE;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCSTesterDlg 对话框

CCSTesterDlg::CCSTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCSTesterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	LoadErrorString("TicketInfoDes.ini");
}

void CCSTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REDIT_OUT, m_rEditOut);
}

void CCSTesterDlg::ChildWindowAnchor()
{
	RECT mainRect, pageRect, editRect, statusRect, operRect;
	DWORD rebarSize = 0;

	GetClientRect(&mainRect);
	
	rebarSize = m_wndToolBar.GetToolBarCtrl().GetButtonSize();
	rebarSize = HIWORD(rebarSize) + 1;
	m_wndReBar.SetWindowPos(NULL, mainRect.left, 0, mainRect.right, rebarSize, SWP_SHOWWINDOW);


	m_WndStatus.GetClientRect(&statusRect);
	statusRect.top = mainRect.bottom - (statusRect.bottom - statusRect.top);
	statusRect.bottom = mainRect.bottom;
	statusRect.left = mainRect.left;
	statusRect.right = mainRect.right;
	m_WndStatus.MoveWindow(&statusRect);

	pageRect.left = mainRect.left;
	pageRect.right = 165;
	pageRect.top = rebarSize;
	pageRect.bottom = statusRect.top;
	m_pBom->MoveWindow(&pageRect);
	m_pAgm->MoveWindow(&pageRect);
	m_pTvm->MoveWindow(&pageRect);
	m_pTcm->MoveWindow(&pageRect);
	
	operRect = mainRect;
	operRect.bottom = statusRect.top;
	operRect.left = pageRect.right;
	operRect.top = operRect.bottom - 0x40;
	m_pOper->MoveWindow(&operRect);
	m_pDec->MoveWindow(&operRect);
	m_pInc->MoveWindow(&operRect);
	m_pSjtSale->MoveWindow(&operRect);
	m_pRefund->MoveWindow(&operRect);
	m_pUpdate->MoveWindow(&operRect);
	m_pParam->MoveWindow(&operRect);
	m_pPrice->MoveWindow(&operRect);
	m_pSamAct->MoveWindow(&operRect);
	m_pDescind->MoveWindow(&operRect);
	m_pDebug->MoveWindow(&operRect);
	m_pDegrade->MoveWindow(&operRect);
	m_pSaleEx->MoveWindow(&operRect);
	
	editRect = mainRect;
	editRect.top = pageRect.top;
	editRect.bottom = operRect.top;
	editRect.left = pageRect.right;
	m_rEditOut.MoveWindow(&editRect);

}

BOOL CCSTesterDlg::AddBars()
{
	CRect rectToolBar;
	CRect rectAddress;
	REBARBANDINFO rbbi;

	AddStatusBar();

	// 容器对象初始化
	if (!m_wndReBar.Create(this))
	{
		TRACE0("Failed to create rebar\n");
		return FALSE;
	}
	if (!AddToolBar(rectToolBar))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}

	if (!m_wndDlgBar.Create(IDD_DIALOGBAR, this))
	{
			TRACE0("Failed to create dialogbar\n");
			return -1;      // fail to create
	}

	m_wndReBar.AddBar(&m_wndDlgBar, NULL, NULL, RBBS_FIXEDBMP | RBBS_BREAK);
	m_wndReBar.AddBar(&m_wndToolBar,NULL, NULL, RBBS_GRIPPERALWAYS | RBBS_FIXEDBMP | RBBS_USECHEVRON);

	// set up min/max sizes and ideal sizes for pieces of the rebar


	rbbi.fMask = RBBIM_CHILDSIZE | RBBIM_IDEALSIZE;
	m_wndDlgBar.GetWindowRect(&rectAddress);
	rbbi.cyMinChild = rectAddress.Height();
	rbbi.cxMinChild = 80;
	rbbi.cxIdeal = 200;
	m_wndReBar.GetReBarCtrl().SetBandInfo(0, &rbbi);

	rbbi.cbSize = sizeof(rbbi);
	rbbi.fMask = RBBIM_CHILDSIZE | RBBIM_IDEALSIZE | RBBIM_SIZE;
	rbbi.cxMinChild = rectToolBar.Width();
	rbbi.cyMinChild = rectToolBar.Height();
	rbbi.cx = rbbi.cxIdeal = rectToolBar.Width() * 9;
	m_wndReBar.GetReBarCtrl().SetBandInfo(1, &rbbi);
	rbbi.cxMinChild = 0;

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED);


	return TRUE;
}

BOOL CCSTesterDlg::AddToolBar(CRect& rcToolBar)
{
	CImageList img;
	CBitmap bmp;
	const int nNumBtn = 6;
	TBBUTTON tb[nNumBtn];
	UINT bmpID[] = {IDB_BMP_BOM, IDB_BMP_AGM, IDB_BMP_TVM, IDB_BMP_TCM/*, IDB_BMP_AVM*/, IDB_BMP_ABOUT};

	if (!m_wndToolBar.CreateEx(this))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}

	CToolBarCtrl& tbCtrl = m_wndToolBar.GetToolBarCtrl();

	tbCtrl.SetButtonWidth(70, 150);
	tbCtrl.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

	img.Create(32, 32, ILC_COLOR32 | ILC_MASK, 1, 1);

	for (size_t i=0;i<sizeof(bmpID)/sizeof(bmpID[0]);i++)
	{
		if (bmp.LoadBitmap(bmpID[i]) == NULL)
			return FALSE;

		img.Add(&bmp, RGB(255, 0, 0));
		bmp.Detach();
	}

	m_wndToolBar.GetToolBarCtrl().SetImageList(&img);
	img.Detach();

	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT);

	AddTbBtn(tb[0], 0, IDM_DEV_BOM, TBSTATE_ENABLED | TBSTATE_CHECKED, TBSTYLE_CHECKGROUP, "bom");
	AddTbBtn(tb[1], 1, IDM_DEV_AGM, TBSTATE_ENABLED                  , TBSTYLE_CHECKGROUP, "agm");
	AddTbBtn(tb[2], 2, IDM_DEV_TVM, TBSTATE_ENABLED                  , TBSTYLE_CHECKGROUP, "tvm");
	AddTbBtn(tb[3], 3, IDM_DEV_TCM, TBSTATE_ENABLED                  , TBSTYLE_CHECKGROUP, "tcm");
	//AddTbBtn(tb[4], 4, IDM_DEV_AVM, TBSTATE_ENABLED                  , TBSTYLE_CHECKGROUP, "avm");
	AddTbBtn(tb[4], 0, 0          , TBSTATE_ENABLED                  , TBSTYLE_SEP       , NULL);
	AddTbBtn(tb[5], 4, 0x10005    , TBSTATE_ENABLED                  , TBSTYLE_BUTTON    , "关于");
	
	if (!tbCtrl.AddButtons(nNumBtn, tb))
		return FALSE;
	// set up toolbar button sizes
	m_wndToolBar.GetItemRect(0, &rcToolBar);
	m_wndToolBar.SetSizes(CSize(80, 55), CSize(32, 32));

	return TRUE;
}

void CCSTesterDlg::AddTbBtn(TBBUTTON& tb, int iBitmap, int iCmd, BYTE fsState, BYTE fsStyle, LPCTSTR pText)
{
	tb.iBitmap = iBitmap; 
	tb.idCommand = iCmd; 
	tb.fsState = fsState; 
	tb.fsStyle = fsStyle; 
	tb.dwData = 0; 
	tb.iString = (INT_PTR)pText; 
}

BOOL CCSTesterDlg::AddStatusBar()
{
	if (!m_WndStatus.Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW, CRect(0,0,0,0), this, 0))
	{
		TRACE0("Failed to create statusbar\n");
		return FALSE;
	}
	int strPartDim[3] = {200, 400, -1}; //分割数量
	m_WndStatus.SetParts(3, strPartDim);

	//设置状态栏文本
	m_WndStatus.SetText("就绪", 0, 0);
	m_WndStatus.SetText("就绪", 1, 0);
	m_WndStatus.SetText("就绪", 2, 0);

	return TRUE;
}

void CCSTesterDlg::AddBitmap(CImageList& imgList, UINT uID)
{
	CBitmap bmp;
	if (bmp.LoadBitmap(uID))
	{
		imgList.Add(&bmp, RGB(255, 255, 255));
	}
}

void CCSTesterDlg::DispResult(CString& strOut, CString strOper, RETINFO Error, LARGE_INTEGER liStart, LARGE_INTEGER liEnd)
{
	CString strOutAll;
	FormHeader(strOutAll, strOper);
	strOutAll += strOut;
	AppendText(strOut);
	SetStatusBar(strOper, Error, liStart, liEnd);
}

void CCSTesterDlg::DispResult(_Struct_Type StructType, LPVOID lpStruct, WORD lenStruct, CString strOper, LARGE_INTEGER liStart, LARGE_INTEGER liEnd)
{
	char szTemp1[64] = {0};
	char szTemp2[64] = {0};
	char szTemp3[64] = {0};
	char szTemp4[128] = {0};
	char szTemp5[64] = {0};
	CString strOut, strTemp;

	if (lenStruct > 0)
	{
		switch (StructType)
		{
		case struct_init_arr:
			{
				uint8_t * p_sam_status = (uint8_t *)lpStruct;
				strTemp.Format("%s%s%02X\n%s%s%02X\n%s%s%02X\n%s%s%02X\n%s%s%02X\n%s%s%02X\n%s%s%02X\n%s%s%02X\n",
					"卡槽[1]sam卡状态", SPACE_MIDDLE, p_sam_status[0],
					"卡槽[2]sam卡状态", SPACE_MIDDLE, p_sam_status[1],
					"卡槽[3]sam卡状态", SPACE_MIDDLE, p_sam_status[2],
					"卡槽[4]sam卡状态", SPACE_MIDDLE, p_sam_status[3],
					"卡槽[5]sam卡状态", SPACE_MIDDLE, p_sam_status[4],
					"卡槽[6]sam卡状态", SPACE_MIDDLE, p_sam_status[5],
					"卡槽[7]sam卡状态", SPACE_MIDDLE, p_sam_status[6],
					"卡槽[8]sam卡状态", SPACE_MIDDLE, p_sam_status[7]);
				strOut += strTemp;
			}
			break;
		case struct_Login:
			{
				POPERLOGIN pStruct = (POPERLOGIN)lpStruct;
				strTemp.Format("%s%s%s\n%s%s%d",
					"操作员号", SPACE_MIDDLE, pStruct->szOperID,
					"BOM班次号", SPACE_MIDDLE, pStruct->nBomShiftID);
				strOut += strTemp;

				break;
			}
		case struct_ES_Analyse:
			{
				PESANALYZE pStruct = (PESANALYZE)lpStruct;
				memcpy(szTemp1, pStruct->cPhysicalID, sizeof(pStruct->cPhysicalID));
				memcpy(szTemp2, pStruct->cLogicalID, sizeof(pStruct->cLogicalID));
				memcpy(szTemp3, pStruct->cIssueDate, sizeof(pStruct->cIssueDate));
				memcpy(szTemp4, pStruct->cTicketType, sizeof(pStruct->cTicketType));
				//BCDTimetoString(pStruct->cIssueDate, 7, szTemp3);
				//BCDTimetoString(pStruct->bExpiry, 4, szTemp4);

				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%d\n%s%s%02X\n%s%s%02X\n",
					"物理卡号",		SPACE_MIDDLE, szTemp1, 
					"逻辑卡号",		SPACE_MIDDLE, szTemp2, 
					"制卡时间",		SPACE_MIDDLE, szTemp3,
					"票卡类型",		SPACE_MIDDLE, szTemp4,
					"票卡余额",		SPACE_MIDDLE, pStruct->lBalance, 
					"押金",			SPACE_MIDDLE, pStruct->lDeposite, 
					"发行状态",		SPACE_MIDDLE, pStruct->bIssueStatus, 
					"状态",			SPACE_MIDDLE, pStruct->bStatus);
				strOut += strTemp;

				/*
				BCDTimetoString(pStruct->bStartDate, 7, szTemp1);
				BCDTimetoString(pStruct->bEndDate, 7, szTemp2);
				sprintf(szTemp3, "%02x%02x/%02x/%x%02x", 
					pStruct->bLastStationID[0], pStruct->bLastStationID[1], 
					pStruct->bLastDeviceType,
					pStruct->bLastDeviceID[0], pStruct->bLastDeviceID[1]);
				BCDTimetoString(pStruct->dtLastDate, 7, szTemp4);
				BCDTimetoString(pStruct->dtEntryDate, 7, szTemp5);*/

				memcpy(szTemp1, pStruct->cExpire, sizeof(pStruct->cExpire));
				memcpy(szTemp2, pStruct->RFU, sizeof(pStruct->RFU));
				//memcpy(szTemp3, pStruct->cLine, sizeof(pStruct->cLine));
				//memcpy(szTemp4, pStruct->cStationNo, sizeof(pStruct->cStationNo));
				memcpy(szTemp3, pStruct->cDateStart, sizeof(pStruct->cDateStart));
				memcpy(szTemp4, pStruct->cDateEnd, sizeof(pStruct->cDateEnd));
				BCDTimetoString(pStruct->dtDaliyActive, 7, szTemp5);


				strTemp.Format("%s%s%02X\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n",
					"票卡物理类型",	SPACE_MIDDLE, pStruct->bCharacter, 
					"物理有效期",	SPACE_MIDDLE, szTemp1, 
					"E/S SAM逻辑卡号",			SPACE_MIDDLE, szTemp2,
					"逻辑有效期开始",	SPACE_MIDDLE, szTemp3,
					"逻辑有效期结束",	SPACE_MIDDLE, szTemp4, 
					"多日票激活时间",		SPACE_MIDDLE, szTemp5);

				strOut += strTemp;

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cardfactory, 4);

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->certificate_code, 32);
				//memcpy(szTemp3, pStruct->cCertificateCode, sizeof(pStruct->cCertificateCode));

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->certificate_name, sizeof(pStruct->certificate_name));


				strTemp.Format("%s%s%02X\n%s%s%02X\n%s%s%02X\n%s%s%s\n%s%s%s\n%s%s%02X\n%s%s%s\n",
					"个人卡",		SPACE_MIDDLE, pStruct->certificate_iscompany, 
					"地铁通卡",		SPACE_MIDDLE, pStruct->certificate_ismetro, 
					"证件类型",		SPACE_MIDDLE, pStruct->certificate_type, 
					"证件号码",		SPACE_MIDDLE, szTemp3, 
					"证件名称",		SPACE_MIDDLE, szTemp4, 
					"性别",	SPACE_MIDDLE, pStruct->certificate_sex, 
					"卡商代码",	SPACE_MIDDLE, szTemp2);
				strOut += strTemp;

				break;
			}
		case struct_Analyse:
			{
				PBOMANALYZE pStruct = (PBOMANALYZE)lpStruct;
				memcpy(szTemp1, pStruct->cPhysicalID, sizeof(pStruct->cPhysicalID));
				memcpy(szTemp2, pStruct->cLogicalID, sizeof(pStruct->cLogicalID));
				BCDTimetoString(pStruct->bIssueData, 4, szTemp3);
				BCDTimetoString(pStruct->bExpiry, 4, szTemp4);
		
				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%02X%02X\n%s%s%d\n%s%s%d\n%s%s%d\n%s%s%d\n%s%s%s\n%s%s%s\n",
					"物理卡号",		SPACE_MIDDLE, szTemp1, 
					"逻辑卡号",		SPACE_MIDDLE, szTemp2, 
					"票卡类型",		SPACE_MIDDLE, pStruct->bTicketType[0], pStruct->bTicketType[1], 
					"票卡余额",		SPACE_MIDDLE, pStruct->lBalance, 
					"押金",			SPACE_MIDDLE, pStruct->lDepositorCost, 
					"钱包上限",		SPACE_MIDDLE, pStruct->lLimitedBalance, 
					"更新罚金",		SPACE_MIDDLE, pStruct->lPenalty, 
					"发行日期",		SPACE_MIDDLE, szTemp3, 
					"物理有效期",	SPACE_MIDDLE, szTemp4);
				strOut += strTemp;

				BCDTimetoString(pStruct->bStartDate, 7, szTemp1);
				BCDTimetoString(pStruct->bEndDate, 7, szTemp2);
				sprintf(szTemp3, "%02x%02x/%02x/%x%02x", 
					pStruct->bLastStationID[0], pStruct->bLastStationID[1], 
					pStruct->bLastDeviceType,
					pStruct->bLastDeviceID[0], pStruct->bLastDeviceID[1]);
				BCDTimetoString(pStruct->dtLastDate, 7, szTemp4);
				BCDTimetoString(pStruct->dtEntryDate, 7, szTemp5);

				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%d\n%s%s%s\n%s%s%s\n%s%s%02x%02x\n%s%s%s\n%s%s%d\n%s%s%02x%02x\n%s%s%02x%02x\n",
					"票卡启用日期",	SPACE_MIDDLE, szTemp1, 
					"票卡停用日期",	SPACE_MIDDLE, szTemp2, 
					"状态",			SPACE_MIDDLE, pStruct->bStatus,
					"上次交易设备",	SPACE_MIDDLE, szTemp3,
					"上次交易时间",	SPACE_MIDDLE, szTemp4, 
					"进站站点",		SPACE_MIDDLE, pStruct->bEntrySationID[0], pStruct->bEntrySationID[1],
					"进站时间",		SPACE_MIDDLE, szTemp5,
					"限制模式",		SPACE_MIDDLE, pStruct->bLimitMode,
					"进站站点限制",	SPACE_MIDDLE, pStruct->bLimitEntryID[0], pStruct->bLimitEntryID[1],
					"出站站点限制",	SPACE_MIDDLE, pStruct->bLimitExitID[0], pStruct->bLimitExitID[1]);
				strOut += strTemp;

				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cCityCode, sizeof(pStruct->cCityCode));

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cSellerCode, sizeof(pStruct->cSellerCode));

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->cCertificateCode, 20);
				//memcpy(szTemp3, pStruct->cCertificateCode, sizeof(pStruct->cCertificateCode));

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cCertificateName, sizeof(pStruct->cCertificateName));

				memset(szTemp5, 0, sizeof(szTemp5));
				BCDTimetoString(pStruct->bCertExpire, 4, szTemp5);

				strTemp.Format("%s%s%d\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%d\n%s%s%x\n%s%s%s\n%s%s%s\n%s%s%c\n%s%s%d\n%s%s%d\n%s%s%d\n%s%s%c\n%s%s%s\n",
					"激活状态",		SPACE_MIDDLE, pStruct->bActiveStatus, 
					"城市代码",		SPACE_MIDDLE, szTemp1, 
					"发行商代码",	SPACE_MIDDLE, szTemp2, 
					"充值计数",		SPACE_MIDDLE, pStruct->lChargeCount, 
					"消费计数",		SPACE_MIDDLE, pStruct->lTradeCount, 
					"证件类型",		SPACE_MIDDLE, pStruct->bCertificateType, 
					"证件号码",		SPACE_MIDDLE, szTemp3, 
					"证件名称",		SPACE_MIDDLE, szTemp4, 
					"应用标识",		SPACE_MIDDLE, pStruct->cTestFlag,
					"充值上限",		SPACE_MIDDLE, pStruct->lChargeUpper,
					"发售押金",		SPACE_MIDDLE, pStruct->lDepositeSale,
					"手续费",		SPACE_MIDDLE, pStruct->lBrokerage,
					"应用模式",		SPACE_MIDDLE, pStruct->cTkAppMode,
					"证件有效期",	SPACE_MIDDLE, szTemp5);
				strOut += strTemp;

				break;
			}

		case struct_Sam:
			{
				PSAMSTATUS pStruct = (PSAMSTATUS)lpStruct;
				for (int i=0;i<8;i++)
				{
					sprintf(szTemp2, "卡槽%dSAM卡状态", i + 1);
					sprintf(szTemp3, "卡槽%dSAM卡号", i + 1);

					memcpy(szTemp1, pStruct[i].cSAMID, sizeof(pStruct[i].cSAMID));
					strTemp.Format("%s%s%02x\n%s%s%s\n",
						szTemp2,		SPACE_MIDDLE, pStruct[i].bSAMStatus,
						szTemp3,		SPACE_MIDDLE, szTemp1);
						strOut += strTemp;
				}
				break;
			}

		case struct_Purse:
			{
				PPURSETRADE pStruct = (PPURSETRADE)lpStruct;

				sprintf(szTemp1, "%C%C%C%C/%C%C/%C%C%C", 
					pStruct->cStationID[0], pStruct->cStationID[1], pStruct->cStationID[2],pStruct->cStationID[3],
					pStruct->cDevType[0], pStruct->cDevType[1],
					pStruct->cDeviceID[0], pStruct->cDeviceID[1], pStruct->cDeviceID[2]);
				memcpy(szTemp2, pStruct->cSAMID, sizeof(pStruct->cSAMID));
				BCDTimetoString(pStruct->dtDate, sizeof(pStruct->dtDate), szTemp3);
				memcpy(szTemp4, pStruct->cLogicalID, sizeof(pStruct->cLogicalID));
				memcpy(szTemp5, pStruct->cPhysicalID, sizeof(pStruct->cPhysicalID));

				strTemp.Format("%s%s%C%C\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%s\n%s%s%02x%02x\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%d\n%s%s%d\n%s%s%d\n%s%s%d\n%s%s%C%C\n",
					"交易类型",	SPACE_MIDDLE, pStruct->cTradeType[0], pStruct->cTradeType[1], 
					"设备信息",	SPACE_MIDDLE, szTemp1, 
					"SAM卡号",	SPACE_MIDDLE, szTemp2, 
					"SAM卡流水",SPACE_MIDDLE, pStruct->lSAMTrSeqNo, 
					"交易时间",	SPACE_MIDDLE, szTemp3, 
					"票卡类型",	SPACE_MIDDLE, pStruct->bTicketType[0], pStruct->bTicketType[1],
					"逻辑卡号",	SPACE_MIDDLE, szTemp4, 
					"物理卡号",	SPACE_MIDDLE, szTemp5,
					"交易状态",	SPACE_MIDDLE, pStruct->bStatus, 
					"交易金额",	SPACE_MIDDLE, pStruct->lTradeAmount, 
					"余额",		SPACE_MIDDLE, pStruct->lBalance, 
					"充值累计",	SPACE_MIDDLE, pStruct->lChargeCount, 
					"消费累计",	SPACE_MIDDLE, pStruct->lTradeCount, 
					"支付类型",	SPACE_MIDDLE, pStruct->cPaymentType[0], pStruct->cPaymentType[1]);
				strOut += strTemp;

				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cMACorTAC, sizeof(pStruct->cMACorTAC));
				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cEntrySAMID, sizeof(pStruct->cEntrySAMID));
				BCDTimetoString(pStruct->dtEntryDate, sizeof(pStruct->dtEntryDate), szTemp3);
				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cOperatorID, sizeof(pStruct->cOperatorID));

				strTemp.Format("%s%s%C%C%C%C\n%s%s%s\n%s%s%02x%02x\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%d\n",
					"支付凭证",		SPACE_MIDDLE, pStruct->cReceiptID[0], pStruct->cReceiptID[1], pStruct->cReceiptID[2], pStruct->cReceiptID[3], 
					"交易认证码",	SPACE_MIDDLE, szTemp1, 
					"进站站点",		SPACE_MIDDLE, pStruct->bEntryStationID[0], pStruct->bEntryStationID[1], 
					"进站SAM卡号",	SPACE_MIDDLE, szTemp2, 
					"进站时间",		SPACE_MIDDLE, szTemp3, 
					"操作员号",		SPACE_MIDDLE, szTemp4, 
					"BOM班次号",	SPACE_MIDDLE, pStruct->bBOMShiftID);
				strOut += strTemp;

				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cSamLast, sizeof(pStruct->cSamLast));
				BCDTimetoString(pStruct->dtLast, sizeof(pStruct->dtLast), szTemp2);
				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%d\n%s%s%c\n",
					"上次交易SAM卡号",	SPACE_MIDDLE, szTemp1, 
					"上次交易时间",		SPACE_MIDDLE, szTemp2, 
					"交易值",			SPACE_MIDDLE, pStruct->lTradeWallet, 
					"卡应用标识",		SPACE_MIDDLE, pStruct->cTestFlag);
				strOut += strTemp;

				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cCityCode, 4);

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cIndustryCode, 4);

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->cClassicType, 2);

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cSamPosId, sizeof(pStruct->cSamPosId));

				strTemp.Format("%s%s%C%C%C\n%s%s%02x%02x\n%s%s%02x%02x\n%s%s%c\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%C\n",
					"限制模式",		SPACE_MIDDLE, pStruct->cLimitMode[0], pStruct->cLimitMode[1], pStruct->cLimitMode[2],
					"限制进站站点", SPACE_MIDDLE, pStruct->bLimitEntryID[0], pStruct->bLimitEntryID[1],
					"限制出站站点", SPACE_MIDDLE, pStruct->bLimitExitID[0], pStruct->bLimitExitID[1],
					"出站工作模式", SPACE_MIDDLE, pStruct->cExitMode,
					"城市代码",		SPACE_MIDDLE, szTemp1,
					"行业代码",		SPACE_MIDDLE, szTemp2,
					"TAC交易分类",	SPACE_MIDDLE, szTemp3,
					"SAM卡终端号",	SPACE_MIDDLE, szTemp4,
					"卡应用模式",	SPACE_MIDDLE, pStruct->cTkAppMode);
				strOut += strTemp;
				
				//新增交通部返回结构
				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n",
					"记录版本",		SPACE_MIDDLE, pStruct->cRecordVer,
					"发卡机构代码", SPACE_MIDDLE, pStruct->cIssueCardCode,
					"密钥版本号",   SPACE_MIDDLE, pStruct->cKeyVer,
					"密钥索引",     SPACE_MIDDLE, pStruct->cKeyIndex,
					"伪随机数",		SPACE_MIDDLE, pStruct->cRandom,
					"算法标识",		SPACE_MIDDLE, pStruct->cAlgFlag,
					"持卡人姓名",	SPACE_MIDDLE, pStruct->cCardHolderName,
					"证件类型",	    SPACE_MIDDLE, pStruct->cCertificateType,
					"证件号码",	    SPACE_MIDDLE, pStruct->cCertificateCode,
				    "原来票卡类型",	SPACE_MIDDLE, pStruct->cOriginalCardType,
					"卡属地",	    SPACE_MIDDLE, pStruct->cCardLocation);
				strOut += strTemp;

				break;
			}

		case struct_SvtSale:
			{
				POTHERSALE pStruct = (POTHERSALE)lpStruct;

				sprintf(szTemp1, "%C%C%C%C/%C%C/%C%C%C", 
					pStruct->cStationID[0], pStruct->cStationID[1], pStruct->cStationID[2],pStruct->cStationID[3],
					pStruct->cDevType[0], pStruct->cDevType[1],
					pStruct->cDeviceID[0], pStruct->cDeviceID[1], pStruct->cDeviceID[2]);
				memcpy(szTemp2, pStruct->cLogicalID, sizeof(pStruct->cLogicalID));
				memcpy(szTemp3, pStruct->cPhysicalID, sizeof(pStruct->cPhysicalID));
				strTemp.Format("%s%s%C%C\n%s%s%s\n%s%s%02x%02x\n%s%s%s\n%s%s%s\n%s%s%d\n",
					"交易类型",	SPACE_MIDDLE, pStruct->cTradeType[0], pStruct->cTradeType[1], 
					"设备信息",	SPACE_MIDDLE, szTemp1, 
					"票卡类型",	SPACE_MIDDLE, pStruct->bTicketType[0], pStruct->bTicketType[1], 
					"逻辑卡号",	SPACE_MIDDLE, szTemp2, 
					"物理卡号",	SPACE_MIDDLE, szTemp3, 
					"交易状态",	SPACE_MIDDLE, pStruct->bStatus);
				strOut += strTemp;

				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cBusinessseqno, sizeof(pStruct->cBusinessseqno));
				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cSAMID, sizeof(pStruct->cSAMID));
				BCDTimetoString(pStruct->dtDate, sizeof(pStruct->dtDate), szTemp3);
				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cOperatorID, sizeof(pStruct->cOperatorID));
				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%d\n%s%s%d\n%s%s%d\n%s%s%s\n%s%s%C%C%C%C\n%s%s%s\n%s%s%d\n%s%s%d\n%s%s%c\n",
					"脱机业务流水",	SPACE_MIDDLE, szTemp1, 
					"SAM卡号",		SPACE_MIDDLE, szTemp2, 
					"SAM卡流水",	SPACE_MIDDLE, pStruct->lSAMTrSeqNo, 
					"金额类型",		SPACE_MIDDLE, pStruct->bAmountType, 
					"押金金额",		SPACE_MIDDLE, pStruct->nAmount, 
					"发售时间",		SPACE_MIDDLE, szTemp3, 
					"支付凭证",		SPACE_MIDDLE, pStruct->cReceiptID[0], pStruct->cReceiptID[1], pStruct->cReceiptID[2], pStruct->cReceiptID[3], 
					"操作员号",		SPACE_MIDDLE, szTemp4, 
					"BOM班次号",	SPACE_MIDDLE, pStruct->bBOMShfitID,
					"售卡费用",		SPACE_MIDDLE, pStruct->lBrokerage,
					"应用标识",		SPACE_MIDDLE, pStruct->cTestFlag);
				strOut += strTemp;
				break;
			}

		case struct_SjtSale:
			{
				PSJTSALE pStruct = (PSJTSALE)lpStruct;

				sprintf(szTemp1, "%C%C%C%C/%C%C/%C%C%C", 
					pStruct->cStationID[0], pStruct->cStationID[1], pStruct->cStationID[2],pStruct->cStationID[3],
					pStruct->cDevType[0], pStruct->cDevType[1],
					pStruct->cDeviceID[0], pStruct->cDeviceID[1], pStruct->cDeviceID[2]);
				memcpy(szTemp2, pStruct->cSAMID, sizeof(pStruct->cSAMID));
				BCDTimetoString(pStruct->dtDate, sizeof(pStruct->dtDate), szTemp3);
				memcpy(szTemp4, pStruct->cPaymentTKLogicalID, sizeof(pStruct->cPaymentTKLogicalID));

				strTemp.Format("%s%s%C%C\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%s\n%s%s%d\n%s%s%s\n%s%s%d\n",
					"交易类型",			SPACE_MIDDLE, pStruct->cTradeType[0], pStruct->cTradeType[1], 
					"设备信息",			SPACE_MIDDLE, szTemp1, 
					"SAM卡号",			SPACE_MIDDLE, szTemp2,
					"SAM卡流水",		SPACE_MIDDLE, pStruct->lSAMTrSeqNo, 
					"发售时间",			SPACE_MIDDLE, szTemp3, 
					"支付方式",			SPACE_MIDDLE, pStruct->bPaymentMeans, 
					"支付卡逻辑卡号",	SPACE_MIDDLE, szTemp4, 
					"扣款累计",			SPACE_MIDDLE, pStruct->lTradeCount);
				strOut += strTemp;

				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cLogicalID, sizeof(pStruct->cLogicalID));

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cPhysicalID, sizeof(pStruct->cPhysicalID));

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->cMACorTAC, sizeof(pStruct->cMACorTAC));

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cOperatorID, sizeof(pStruct->cOperatorID));
				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%d\n%s%s%d\n%s%s%02x%02x\n%s%s%d\n%s%s%s\n%s%s%d\n%s%s%d\n%s%s%s\n%s%s%d\n%s%s%d\n%s%s%c\n",
					"逻辑卡号",		SPACE_MIDDLE, szTemp1, 
					"物理卡号",		SPACE_MIDDLE, szTemp2, 
					"交易状态",		SPACE_MIDDLE, pStruct->bStatus, 
					"发售金额",		SPACE_MIDDLE, pStruct->nChargeValue,
					"票卡类型",		SPACE_MIDDLE, pStruct->bTicketType[0], pStruct->bTicketType[1], 
					"区段编码",		SPACE_MIDDLE, pStruct->bZoneID, 
					"交易认证码",	SPACE_MIDDLE, szTemp3, 
					"押金标识",		SPACE_MIDDLE, pStruct->bDepositorCost,
					"押金",			SPACE_MIDDLE, pStruct->nAmountCost, 
					"操作员号",		SPACE_MIDDLE, szTemp4, 
					"BOM班次号",	SPACE_MIDDLE, pStruct->bBOMShiftID,
					"售卡费用",		SPACE_MIDDLE, pStruct->lBrokerage,
					"应用标识",		SPACE_MIDDLE, pStruct->cTestFlag);
				strOut += strTemp;

				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cClassicType, sizeof(pStruct->cClassicType));

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cSamPosId, sizeof(pStruct->cSamPosId));

				strTemp.Format("%s%s%s\n%s%s%s\n",
					"TAC交易分类",	SPACE_MIDDLE, szTemp1,
					"SAM卡终端号",	SPACE_MIDDLE, szTemp2);
				strOut += strTemp;

				break;
			}

		case struct_Deffer:
			{
				PTICKETDEFER pStruct = (PTICKETDEFER)lpStruct;

				sprintf(szTemp1, "%C%C%C%C/%C%C/%C%C%C", 
					pStruct->cStationID[0], pStruct->cStationID[1], pStruct->cStationID[2],pStruct->cStationID[3],
					pStruct->cDevType[0], pStruct->cDevType[1],
					pStruct->cDeviceID[0], pStruct->cDeviceID[1], pStruct->cDeviceID[2]);
				memcpy(szTemp2, pStruct->cSAMID, sizeof(pStruct->cSAMID));
				memcpy(szTemp3, pStruct->cLogicalID, sizeof(pStruct->cLogicalID));
				memcpy(szTemp4, pStruct->cPhysicalID, sizeof(pStruct->cPhysicalID));

				strTemp.Format("%s%s%C%C\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%02x%02x\n%s%s%s\n%s%s%s\n",
					"交易类型",		SPACE_MIDDLE, pStruct->cTradeType[0], pStruct->cTradeType[1], 
					"设备信息",		SPACE_MIDDLE, szTemp1, 
					"SAM卡号",		SPACE_MIDDLE, szTemp2,
					"SAM卡流水",	SPACE_MIDDLE, pStruct->lSAMTrSeqNo, 
					"票卡类型",		SPACE_MIDDLE, pStruct->bTicketType[0], pStruct->bTicketType[1], 
					"逻辑卡号",		SPACE_MIDDLE, szTemp3, 
					"物理卡号",		SPACE_MIDDLE, szTemp4);
				strOut += strTemp;

				BCDTimetoString(pStruct->dtOldExpiryDate, sizeof(pStruct->dtOldExpiryDate), szTemp1);
				BCDTimetoString(pStruct->dtNewExpiryDate, sizeof(pStruct->dtNewExpiryDate), szTemp2);
				BCDTimetoString(pStruct->dtOperateDate, sizeof(pStruct->dtOperateDate), szTemp3);
				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cOperatorID, sizeof(pStruct->cOperatorID));

				strTemp.Format("%s%s%d\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%c\n",
					"交易状态",		SPACE_MIDDLE, pStruct->bStatus, 
					"原有效期",		SPACE_MIDDLE, szTemp1, 
					"现有效期",		SPACE_MIDDLE, szTemp2, 
					"操作时间",		SPACE_MIDDLE, szTemp3, 
					"操作员号",		SPACE_MIDDLE, szTemp4,
					"BOM班次号",	SPACE_MIDDLE, pStruct->bBOMShfitID,
					"应用标识",		SPACE_MIDDLE, pStruct->cTestFlag);
				strOut += strTemp;
				break;
			}

		case struct_Lock:
			{
				PTICKETLOCK pStruct = (PTICKETLOCK)lpStruct;

				sprintf(szTemp1, "%C%C%C%C/%C%C/%C%C%C", 
					pStruct->cStationID[0], pStruct->cStationID[1], pStruct->cStationID[2],pStruct->cStationID[3],
					pStruct->cDevType[0], pStruct->cDevType[1],
					pStruct->cDeviceID[0], pStruct->cDeviceID[1], pStruct->cDeviceID[2]);
				memcpy(szTemp2, pStruct->cSAMID, sizeof(pStruct->cSAMID));

				strTemp.Format("%s%s%C%C\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%02x%02x\n",
					"交易类型",		SPACE_MIDDLE, pStruct->cTradeType[0], pStruct->cTradeType[1], 
					"设备信息",		SPACE_MIDDLE, szTemp1, 
					"SAM卡号",		SPACE_MIDDLE, szTemp2,
					"SAM卡流水",	SPACE_MIDDLE, pStruct->lSAMTrSeqNo, 
					"票卡类型",		SPACE_MIDDLE, pStruct->bTicketType[0], pStruct->bTicketType[1]);
				strOut += strTemp;


				memcpy(szTemp1, pStruct->cLogicalID, sizeof(pStruct->cLogicalID));
				memcpy(szTemp2, pStruct->cPhysicalID, sizeof(pStruct->cPhysicalID));
				BCDTimetoString(pStruct->dtDate, sizeof(pStruct->dtDate), szTemp3);
				memcpy(szTemp4, pStruct->cOperatorID, sizeof(pStruct->cOperatorID));
				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%d\n%s%s%C\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%c\n%s%s%C\n",
					"逻辑卡号",		SPACE_MIDDLE, szTemp1, 
					"物理卡号",		SPACE_MIDDLE, szTemp2, 
					"交易状态",		SPACE_MIDDLE, pStruct->bStatus, 
					"锁卡标记",		SPACE_MIDDLE, pStruct->cLockFlag, 
					"交易时间",		SPACE_MIDDLE, szTemp3, 
					"操作员号",		SPACE_MIDDLE, szTemp4,
					"BOM班次号",	SPACE_MIDDLE, pStruct->bBOMShfitID,
					"应用标识",		SPACE_MIDDLE, pStruct->cTestFlag,
					"卡应用模式",	SPACE_MIDDLE, pStruct->cTkAppMode);
				strOut += strTemp;
				break;
			}

		case struct_Refund:
			{
				PDIRECTREFUND pStruct = (PDIRECTREFUND)lpStruct;

				sprintf(szTemp1, "%C%C%C%C/%C%C/%C%C%C", 
					pStruct->cStationID[0], pStruct->cStationID[1], pStruct->cStationID[2],pStruct->cStationID[3],
					pStruct->cDevType[0], pStruct->cDevType[1],
					pStruct->cDeviceID[0], pStruct->cDeviceID[1], pStruct->cDeviceID[2]);
				memcpy(szTemp2, pStruct->cSAMID, sizeof(pStruct->cSAMID));
				memcpy(szTemp3, pStruct->cLogicalID, sizeof(pStruct->cLogicalID));
				memcpy(szTemp4, pStruct->cPhysicalID, sizeof(pStruct->cPhysicalID));
				strTemp.Format("%s%s%C%C\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%02x%02x\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%d\n%s%s%d\n%s%s%d\n%s%s%d\n",
					"交易类型",		SPACE_MIDDLE, pStruct->cTradeType[0], pStruct->cTradeType[1], 
					"设备信息",		SPACE_MIDDLE, szTemp1, 
					"SAM卡号",		SPACE_MIDDLE, szTemp2, 
					"SAM卡流水",	SPACE_MIDDLE, pStruct->lSAMTrSeqNo,
					"票卡类型",		SPACE_MIDDLE, pStruct->bTicketType[0], pStruct->bTicketType[1], 
					"逻辑卡号",		SPACE_MIDDLE, szTemp3,
					"物理卡号",		SPACE_MIDDLE, szTemp4,
					"交易状态",		SPACE_MIDDLE, pStruct->bStatus,
					"退还余额",		SPACE_MIDDLE, pStruct->lBalanceReturned, 
					"退还押金",		SPACE_MIDDLE, pStruct->nDepositReturned,
					"罚金",			SPACE_MIDDLE, pStruct->nForfeiture, 
					"罚款原因",		SPACE_MIDDLE, pStruct->bForfeitReason);
				strOut += strTemp;

				BCDTimetoString(pStruct->dtDate, sizeof(pStruct->dtDate), szTemp1);

				BCDTimetoString(pStruct->dtApplyDate, sizeof(pStruct->dtApplyDate), szTemp2);

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->cMACOrTAC, sizeof(pStruct->cMACOrTAC));

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cOperatorID, sizeof(pStruct->cOperatorID));

				strTemp.Format("%s%s%d\n%s%s%C\n%s%s%s\n%s%s%C%C%C%C\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%d\n%s%s%c\n",
					"消费累计",		SPACE_MIDDLE, pStruct->lTradeCount, 
					"退款类型",		SPACE_MIDDLE, pStruct->bReturnTypeCode,
					"退款时间",		SPACE_MIDDLE, szTemp1, 
					"交易凭证",		SPACE_MIDDLE, pStruct->cReceiptID[0], pStruct->cReceiptID[1], pStruct->cReceiptID[2], pStruct->cReceiptID[3],
					"申请时间",		SPACE_MIDDLE, szTemp2,
					"交易认证码",	SPACE_MIDDLE, szTemp3,
					"操作员号",		SPACE_MIDDLE, szTemp4,
					"BOM班次号",	SPACE_MIDDLE, pStruct->bBOMShfitID,
					"退卡费用",		SPACE_MIDDLE, pStruct->lBrokerage,
					"应用标识",		SPACE_MIDDLE, pStruct->cTestFlag);
				strOut += strTemp;

				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cClassicType, sizeof(pStruct->cClassicType));

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cSamPosId, sizeof(pStruct->cSamPosId));

				strTemp.Format("%s%s%s\n%s%s%s\n",
					"TAC交易分类",	SPACE_MIDDLE, szTemp1,
					"SAM卡终端号",	SPACE_MIDDLE, szTemp2);
				strOut += strTemp;

				break;
			}

		case struct_TkInfo:
			{
				PTICKETINFO pStruct = (PTICKETINFO)lpStruct;

				memcpy(szTemp1, pStruct->cPhysicalID, sizeof(pStruct->cPhysicalID));
				memcpy(szTemp2, pStruct->cLogicalID, sizeof(pStruct->cLogicalID));
				BCDTimetoString(pStruct->bIssueData, 4, szTemp3);
				BCDTimetoString(pStruct->bExpiry, 4, szTemp4);

				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%02X%02X\n%s%s%d\n%s%s%d\n%s%s%d\n%s%s%s\n%s%s%s\n",
					"物理卡号",		SPACE_MIDDLE, szTemp1, 
					"逻辑卡号",		SPACE_MIDDLE, szTemp2, 
					"票卡类型",		SPACE_MIDDLE, pStruct->bTicketType[0], pStruct->bTicketType[1], 
					"票卡余额",		SPACE_MIDDLE, pStruct->lBalance, 
					"押金",			SPACE_MIDDLE, pStruct->lDepositorCost, 
					"钱包上限",		SPACE_MIDDLE, pStruct->lLimitedBalance, 
					"发行日期",		SPACE_MIDDLE, szTemp3, 
					"物理有效期",	SPACE_MIDDLE, szTemp4);
				strOut += strTemp;

				BCDTimetoString(pStruct->bStartDate, 7, szTemp1);
				BCDTimetoString(pStruct->bEndDate, 7, szTemp2);
				sprintf(szTemp3, "%02x%02x/%02x/%x%02x", 
					pStruct->bLastStationID[0], pStruct->bLastStationID[1], 
					pStruct->bLastDeviceType,
					pStruct->bLastDeviceID[0], pStruct->bLastDeviceID[1]);
				BCDTimetoString(pStruct->dtLastDate, 7, szTemp4);
				BCDTimetoString(pStruct->dtEntryDate, 7, szTemp5);

				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%d\n%s%s%s\n%s%s%s\n%s%s%02x%02x\n%s%s%s\n%s%s%d\n%s%s%02x%02x\n%s%s%02x%02x\n",
					"票卡启用日期",	SPACE_MIDDLE, szTemp1, 
					"票卡停用日期",	SPACE_MIDDLE, szTemp2, 
					"状态",			SPACE_MIDDLE, pStruct->bStatus,
					"上次交易设备",	SPACE_MIDDLE, szTemp3,
					"上次交易时间",	SPACE_MIDDLE, szTemp4, 
					"进站站点",		SPACE_MIDDLE, pStruct->bEntrySationID[0], pStruct->bEntrySationID[1],
					"进站时间",		SPACE_MIDDLE, szTemp5,
					"限制模式",		SPACE_MIDDLE, pStruct->bLimitMode,
					"进站站点限制",	SPACE_MIDDLE, pStruct->bLimitEntryID[0], pStruct->bLimitEntryID[1],
					"出站站点限制",	SPACE_MIDDLE, pStruct->bLimitExitID[0], pStruct->bLimitExitID[1]);
				strOut += strTemp;

				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cCityCode, sizeof(pStruct->cCityCode));

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cSellerCode, sizeof(pStruct->cSellerCode));

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->cCertificateCode, 20);

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cCertificateName, sizeof(pStruct->cCertificateName));

				memset(szTemp5, 0, sizeof(szTemp5));
				BCDTimetoString(pStruct->bCertExpire, 4, szTemp5);

				strTemp.Format("%s%s%d\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%d\n%s%s%x\n%s%s%s\n%s%s%s\n%s%s%c\n%s%s%c\n%s%s%s\n",
					"激活状态",		SPACE_MIDDLE, pStruct->bActiveStatus, 
					"城市代码",		SPACE_MIDDLE, szTemp1, 
					"发行商代码",	SPACE_MIDDLE, szTemp2, 
					"充值计数",		SPACE_MIDDLE, pStruct->lChargeCount, 
					"消费计数",		SPACE_MIDDLE, pStruct->lTradeCount, 
					"证件类型",		SPACE_MIDDLE, pStruct->bCertificateType, 
					"证件号码",		SPACE_MIDDLE, szTemp3, 
					"证件名称",		SPACE_MIDDLE, szTemp4, 
					"应用标识",		SPACE_MIDDLE, pStruct->cTestFlag,
					"应用模式",		SPACE_MIDDLE, pStruct->cTkAppMode,
					"证件有效期",		SPACE_MIDDLE, szTemp5);
				strOut += strTemp;

				FormHeader(strOut, strOper);

				strTemp.Format("\t     %-21s%-6s%-6s%-10s%-10s%-10s%-16s\n",
					"交易时间", "站点", "状态", "金额", "设备类型", "设备号", "SAM卡号");
				strOut += strTemp;

				for (BYTE i=0;i<pStruct->bUsefulCount;i++)
				{
					BCDTimetoString(pStruct->bhs[i].dtDate, sizeof(pStruct->bhs[i].dtDate), szTemp1);
					memset(szTemp2, 0, sizeof(szTemp2));
					memcpy(szTemp2, pStruct->bhs[i].cSAMID, 16);

					strTemp.Format("\t     %-21s%02X%02X  %-6d%-10d%-10d%02X%02X      %-16s\n",
						szTemp1, 
						pStruct->bhs[i].bStationID[0], pStruct->bhs[i].bStationID[1], 
						pStruct->bhs[i].bStatus,
						pStruct->bhs[i].lTradeAmount, 
						pStruct->bhs[i].bDeviceType, 
						pStruct->bhs[i].bDeviceID[0], pStruct->bhs[i].bDeviceID[1], 
						szTemp2);
					strOut += strTemp;
				}
				strOut += "\n";

				break;				   
			}
		case struct_Update:
			{
				PTICKETUPDATE pStruct = (PTICKETUPDATE)lpStruct;

				sprintf(szTemp1, "%C%C%C%C/%C%C/%C%C%C", 
					pStruct->cStationID[0], pStruct->cStationID[1], pStruct->cStationID[2],pStruct->cStationID[3],
					pStruct->cDevType[0], pStruct->cDevType[1],
					pStruct->cDeviceID[0], pStruct->cDeviceID[1], pStruct->cDeviceID[2]);

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cSAMID, sizeof(pStruct->cSAMID));

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->cLogicalID, sizeof(pStruct->cLogicalID));

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cPhysicalID, sizeof(pStruct->cPhysicalID));

				strTemp.Format("%s%s%C%C\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%02x%02x\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%d\n%s%s%C\n%s%s%d\n",
					"交易类型",		SPACE_MIDDLE, pStruct->cTradeType[0], pStruct->cTradeType[1],
					"更新设备",		SPACE_MIDDLE, szTemp1,
					"SAM卡号",		SPACE_MIDDLE, szTemp2,
					"SAM流水",		SPACE_MIDDLE, pStruct->lSAMTrSeqNo,
					"票卡类型",		SPACE_MIDDLE, pStruct->bTicketType[0], pStruct->bTicketType[1], 
					"逻辑卡号",		SPACE_MIDDLE, szTemp3,
					"物理卡号",		SPACE_MIDDLE, szTemp4,
					"消费计数",		SPACE_MIDDLE, pStruct->lTradeCount,
					"交易状态",		SPACE_MIDDLE, pStruct->bStatus,
					"更新区域",		SPACE_MIDDLE, pStruct->cUpdateZone,
					"更新原因",		SPACE_MIDDLE, pStruct->bUpdateReasonCode);
				strOut += strTemp;

				BCDTimetoString(pStruct->dtUpdateDate, sizeof(pStruct->dtUpdateDate), szTemp1);

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cReceiptID, sizeof(pStruct->cReceiptID));

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->cOperatorID, sizeof(pStruct->cOperatorID));

				strTemp.Format("%s%s%s\n%s%s%d\n%s%s%d\n%s%s%s\n%s%s%s\n%s%s%02x%02x\n%s%s%d\n%s%s%c\n",
					"更新时间",		SPACE_MIDDLE, szTemp1, 
					"支付方式",		SPACE_MIDDLE, pStruct->bPaymentMode,
					"更新罚金",		SPACE_MIDDLE, pStruct->nForfeiture,
					"支付凭证",		SPACE_MIDDLE, szTemp2,
					"操作员号",		SPACE_MIDDLE, szTemp3,
					"进站站点",		SPACE_MIDDLE, pStruct->bEntryStationID[0], pStruct->bEntryStationID[1], 
					"BOM班次号",	SPACE_MIDDLE, pStruct->bBOMShfitID,
					"应用标识",		SPACE_MIDDLE, pStruct->cTestFlag);
				strOut += strTemp;

				strTemp.Format("%s%s%C%C%C\n%s%s%02x%02x\n%s%s%02x%02x\n%s%s%C\n",
					"限制模式",		SPACE_MIDDLE, pStruct->cLimitMode[0], pStruct->cLimitMode[1], pStruct->cLimitMode[2],
					"限制进站站点", SPACE_MIDDLE, pStruct->bLimitEntryID[0], pStruct->bLimitEntryID[1],
					"限制出站站点", SPACE_MIDDLE, pStruct->bLimitExitID[0], pStruct->bLimitExitID[1],
					"卡应用模式",	SPACE_MIDDLE, pStruct->cTkAppMode);
				strOut += strTemp;

				break;				   
			}
		case struct_Entry:
			{
				PENTRYGATE pStruct = (PENTRYGATE)lpStruct;

				sprintf(szTemp1, "%C%C%C%C/%C%C/%C%C%C", 
					pStruct->cStationID[0], pStruct->cStationID[1], pStruct->cStationID[2],pStruct->cStationID[3],
					pStruct->cDevType[0], pStruct->cDevType[1],
					pStruct->cDeviceID[0], pStruct->cDeviceID[1], pStruct->cDeviceID[2]);

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cSAMID, sizeof(pStruct->cSAMID));

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->cLogicalID, sizeof(pStruct->cLogicalID));

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cPhysicalID, sizeof(pStruct->cPhysicalID));

				BCDTimetoString(pStruct->dtDate, sizeof(pStruct->dtDate), szTemp5);

				strTemp.Format("%s%s%C%C\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%02x%02x\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%d\n%s%s%d\n%s%s%c\n",
					"交易类型",		SPACE_MIDDLE, pStruct->cTradeType[0], pStruct->cTradeType[1], 
					"进站设备",		SPACE_MIDDLE, szTemp1, 
					"SAM卡号",		SPACE_MIDDLE, szTemp2,
					"SAM卡流水",	SPACE_MIDDLE, pStruct->lSAMTrSeqNo,
					"票卡类型",		SPACE_MIDDLE, pStruct->bTicketType[0], pStruct->bTicketType[1], 
					"逻辑卡号",		SPACE_MIDDLE, szTemp3,
					"物理卡号",		SPACE_MIDDLE, szTemp4,
					"交易时间",		SPACE_MIDDLE, szTemp5,
					"交易状态",		SPACE_MIDDLE, pStruct->bStatus,
					"余额",			SPACE_MIDDLE, pStruct->lBalance, 
					"应用标识",		SPACE_MIDDLE, pStruct->cTestFlag);
				strOut += strTemp;

				strTemp.Format("%s%s%C%C%C\n%s%s%02x%02x\n%s%s%02x%02x\n%s%s%c\n%s%s%d\n%s%s%C\n",
					"限制模式",		SPACE_MIDDLE, pStruct->cLimitMode[0], pStruct->cLimitMode[1], pStruct->cLimitMode[2],
					"限制进站站点", SPACE_MIDDLE, pStruct->bLimitEntryID[0], pStruct->bLimitEntryID[1],
					"限制出站站点", SPACE_MIDDLE, pStruct->bLimitExitID[0], pStruct->bLimitExitID[1],
					"进站工作模式", SPACE_MIDDLE, pStruct->cEntryMode,
					"消费计数",		SPACE_MIDDLE, pStruct->lTradeCount,
					"卡应用模式",	SPACE_MIDDLE, pStruct->cTkAppMode);
				strOut += strTemp;

				break;
			}
		case struct_Ver:
			{
				PREADERVERSION pStruct = (PREADERVERSION)lpStruct;

				BCDtoString(pStruct->verApiFile, sizeof(pStruct->verApiFile), szTemp1);
				BCDtoString(pStruct->verRfFile, sizeof(pStruct->verRfFile), szTemp2);
				BCDtoString(pStruct->verSamFile, sizeof(pStruct->verSamFile), szTemp3);


				strTemp.Format("%s%s%x.%02X\n%s%s%s\n%s%s%X\n%s%s%s\n%s%s%x\n%s%s%s\n",
					"API识别版本",		SPACE_MIDDLE, pStruct->verApi[0], pStruct->verApi[1], 
					"API文件版本",		SPACE_MIDDLE, szTemp1, 
					"RF驱动识别版本",	SPACE_MIDDLE, pStruct->verRfDev,
					"RF驱动文件版本",	SPACE_MIDDLE, szTemp2,
					"SAM驱动识别版本",	SPACE_MIDDLE, pStruct->verSamDev, 
					"SAM驱动文件版本",	SPACE_MIDDLE, szTemp3);
				strOut += strTemp;

				break;
			}
		case struct_ETicket:
			{
				PETICKETDEALINFO pStruct = (PETICKETDEALINFO)lpStruct;

				//BCDTimetoString(pStruct->bBussinessDate, sizeof(pStruct->bBussinessDate), szTemp1);

				//sprintf(szTemp2,"%02x%02x%02x%02x%02x%02x%02x%02x",pStruct->bLogicalID[0],pStruct->bLogicalID[1],pStruct->bLogicalID[2],
				//	pStruct->bLogicalID[3],pStruct->bLogicalID[4],pStruct->bLogicalID[5],pStruct->bLogicalID[6],pStruct->bLogicalID[7]);

				//sprintf(szTemp3,"%02x%02x%02x%02x%02x%02x%02x%02x",pStruct->bPhysicalID[0],pStruct->bPhysicalID[1],pStruct->bPhysicalID[2],
					//pStruct->bPhysicalID[3],pStruct->bPhysicalID[4],pStruct->bPhysicalID[5],pStruct->bPhysicalID[6],pStruct->bPhysicalID[7]);
				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cTxnDate, sizeof(pStruct->cTxnDate));

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cTransCode, sizeof(pStruct->cTransCode));

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->cTicketMainType, sizeof(pStruct->cTicketMainType));

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cTicketSubType, sizeof(pStruct->cTicketSubType));

				memset(szTemp5, 0, sizeof(szTemp5));
				memcpy(szTemp5, pStruct->cChipType, sizeof(pStruct->cChipType));


				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n",
					"运营日期",		SPACE_MIDDLE, szTemp1, 
					"交易状态",		SPACE_MIDDLE, szTemp2, 
					"车票主类型",	SPACE_MIDDLE, szTemp3,
					"车票子类型",	SPACE_MIDDLE, szTemp4,
					"票卡芯片类型",	SPACE_MIDDLE, szTemp5); 
					//"逻辑卡号",		SPACE_MIDDLE, pStruct->cTicketLogicalNo,
					//"票卡CSN",		SPACE_MIDDLE, pStruct->cTicketCSN);
				strOut += strTemp;

				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cTicketLogicalNo, sizeof(pStruct->cTicketLogicalNo));

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cTicketCSN, sizeof(pStruct->cTicketCSN));

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->cTicketCount, sizeof(pStruct->cTicketCount));

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cPsamNo, sizeof(pStruct->cPsamNo));

				memset(szTemp5, 0, sizeof(szTemp5));
				memcpy(szTemp5, pStruct->cPsamSeq, sizeof(pStruct->cPsamSeq));


				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n",
					"逻辑卡号",		SPACE_MIDDLE, szTemp1, 
					"票卡CSN",		SPACE_MIDDLE, szTemp2, 
					"票计数器",		SPACE_MIDDLE, szTemp3,
					"PSAM卡号",		SPACE_MIDDLE, szTemp4,
					"PSAM卡流水号",	SPACE_MIDDLE, szTemp5); 
				strOut += strTemp;

				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cTxnTms, sizeof(pStruct->cTxnTms));

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cPayType, sizeof(pStruct->cPayType));

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->cBeforeTxnBalance, sizeof(pStruct->cBeforeTxnBalance));

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cTxnAmountNo, sizeof(pStruct->cTxnAmountNo));

				memset(szTemp5, 0, sizeof(szTemp5));
				memcpy(szTemp5, pStruct->cTxnAmount, sizeof(pStruct->cTxnAmount));


				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n",
					"交易日期时间",		SPACE_MIDDLE, szTemp1, 
					"支付方式",		SPACE_MIDDLE, szTemp2, 
					"交易前票值",		SPACE_MIDDLE, szTemp3,
					"交易次数",		SPACE_MIDDLE, szTemp4,
					"交易金额",	SPACE_MIDDLE, szTemp5); 
				strOut += strTemp;

				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cTxnStation, sizeof(pStruct->cTxnStation));

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cLastStation, sizeof(pStruct->cLastStation));

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->cLastTxnTms, sizeof(pStruct->cLastTxnTms));

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cDevNodeId, sizeof(pStruct->cDevNodeId));

				memset(szTemp5, 0, sizeof(szTemp5));
				memcpy(szTemp5, pStruct->cDevSeqNo, sizeof(pStruct->cDevSeqNo));

				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n",
					"当前车站",			SPACE_MIDDLE, pStruct->cTxnStation,
					"上次使用车站",		SPACE_MIDDLE, pStruct->cLastStation,
					"上次使用时间",		SPACE_MIDDLE, pStruct->cLastTxnTms,
					"终端设备标识",		SPACE_MIDDLE, pStruct->cDevNodeId,
					"终端设备流水号",	SPACE_MIDDLE, pStruct->cDevSeqNo);
				strOut += strTemp;

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cCenterCode, sizeof(pStruct->cCenterCode));

				strTemp.Format("%s%s%s\n",
					"中心票号",			SPACE_MIDDLE, szTemp4);
				strOut += strTemp;
				break;
			}
		case struct_GetETicket:
			{
				PGETETICKETRESPINFO pStruct = (PGETETICKETRESPINFO)lpStruct;

				memset(szTemp1, 0, sizeof(szTemp1));
				memcpy(szTemp1, pStruct->cAmount, sizeof(pStruct->cAmount));

				memset(szTemp2, 0, sizeof(szTemp2));
				memcpy(szTemp2, pStruct->cSum, sizeof(pStruct->cSum));

				memset(szTemp4, 0, sizeof(szTemp4));
				memcpy(szTemp4, pStruct->cCenterCode, sizeof(pStruct->cCenterCode));

				memset(szTemp3, 0, sizeof(szTemp3));
				memcpy(szTemp3, pStruct->cStartStation, sizeof(pStruct->cStartStation));

				memset(szTemp5, 0, sizeof(szTemp5));
				memcpy(szTemp5, pStruct->cTerminalStation, sizeof(pStruct->cTerminalStation));

				strTemp.Format("%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n",
					"单程票张数",			SPACE_MIDDLE, szTemp1,
					"总金额",				SPACE_MIDDLE, szTemp2,
					"票号",					SPACE_MIDDLE, szTemp4,
					"开始站点编码",			SPACE_MIDDLE, szTemp3,
					"结束站点编码",			SPACE_MIDDLE, szTemp5);
				strOut += strTemp;

				memset(szTemp5, 0, sizeof(szTemp5));
				memcpy(szTemp5, pStruct->cTicketLogicalNo, sizeof(pStruct->cTicketLogicalNo));

				strTemp.Format("%s%s%s\n",
					"逻辑卡号",			SPACE_MIDDLE, szTemp5);
				strOut += strTemp;
				break;
			}
		}
	}
	if (StructType != struct_History && StructType != struct_TkInfo)
		FormHeader(strOut, strOper);
	AppendText(strOut);
	SetStatusBar(strOper, g_retInfo, liStart, liEnd);
}

BOOL CCSTesterDlg::GetSetting(int& nPort, WORD& wStation, WORD& wDevice)
{
	return m_wndDlgBar.GetSetting(nPort, wStation, wDevice);
}

void CCSTesterDlg::LoadErrorString(CString strPath)
{
	CStdioFile sfile;
	CString strRead;
	if (sfile.Open(strPath, CFile::modeRead))
	{
		while (sfile.ReadString(strRead))
		{
			m_ErrorDiscribe.AddTail(strRead);
		}
	}
}

void CCSTesterDlg::GetErrorString(DWORD nError, CString& strError)
{
	CString str, strElement;
	str.Format("%u", nError);
	int nLen;

	POSITION pos = m_ErrorDiscribe.GetHeadPosition();
	for (int i=0;i < m_ErrorDiscribe.GetCount();i++)
	{
		strElement = m_ErrorDiscribe.GetNext(pos);
		nLen = str.GetLength();
		if (str == strElement.Left(nLen))
		{
			nLen = strElement.GetLength() - nLen - 1;
			strError = strElement.Right(nLen);
			break;
		}
	}
}

void CCSTesterDlg::ShowDialogs(DWORD flag)
{
	m_pOper->ShowWindow((flag >> DLG_OPER) & 0x01);
	m_pInc->ShowWindow((flag >> DLG_INC) & 0x01);
	m_pDec->ShowWindow((flag >> DLG_DEC) & 0x01);
	m_pSjtSale->ShowWindow((flag >> DLG_SSALE) & 0x01);
	m_pUpdate->ShowWindow((flag >> DLG_UPD) & 0x01);
	m_pPrice->ShowWindow((flag >> DLG_PRICE) & 0x01);
	m_pParam->ShowWindow((flag >> DLG_PARAM) & 0x01);	
	m_pRefund->ShowWindow((flag >> DLG_RFD) & 0x01);	
	m_pSamAct->ShowWindow((flag >> DLG_SAM_ACT) & 0x01);
	m_pDescind->ShowWindow((flag >> DLG_DESCIND) & 0x01);
	m_pDebug->ShowWindow((flag >> DLG_DEBUG) & 0x01);
	m_pDegrade->ShowWindow((flag >> DLG_DEGRADE) & 0x01);
	m_pSaleEx->ShowWindow((flag >> DLG_SALEEX) & 0x01);
}

CString CCSTesterDlg::GetLineName(BYTE lineID)
{
	CString strRet;

	switch (lineID)
	{
	case 0x01:
		strRet = "一号线";
		break;
	case 0x02:
		strRet = "二号线";
		break;
	}

	return strRet;
}

CString CCSTesterDlg::GetStationPrmPath()
{
	CString strRet;
	char szPath[MAX_PATH] = {0};

	::GetModuleFileName(NULL, szPath, MAX_PATH);

	for (size_t i=strlen(szPath);i>0;i--)
	{
		if (szPath[i-1] == '\\')
		{
			szPath[i] = 0;
			break;
		}
	}

	strRet = CString(szPath) + "PRM.0201";

	return strRet;
}

// 加载车站表
void CCSTesterDlg::LoadStationTable(CString strPath)
{
	char szRead[256]		= {0};
	char szTemp[32]			= {0};
	BYTE bLineID			= 0;
	WORD wStationID			= 0;
	CString strStationName;
	LINEINF lineinf;
	map<BYTE, LINEINF>::iterator itor;


	UnLoadStationTable();

	GSFile file(strPath.GetBuffer(), ModeRead);
	if (file.IsOpen())
	{
		while (file.ReadLine(szRead, sizeof(szRead)))
		{
			if (memcmp(szRead, "0201", 4) == 0)
			{
				memcpy(szTemp, szRead + 5, 24);

				bLineID = TwoChar2Hex(szRead + 5);

				itor = g_PrmStation.find(bLineID);

				wStationID = (bLineID << 8) + TwoChar2Hex(szRead + 7);
				memcpy(szTemp, szRead + 9, 20);
				strStationName = szTemp;

				if (itor == g_PrmStation.end())		// 需要新建一条线路
				{
					lineinf.vsta.clear();
					lineinf.lineName = GetLineName(bLineID);

					lineinf.vsta.push_back(make_pair(wStationID, strStationName));
					g_PrmStation.insert(make_pair(bLineID, lineinf));
				}
				else		// 线路已存在
				{
					itor->second.vsta.push_back(make_pair(wStationID, strStationName));
				}
			}
		}

		file.Close();
	}

}

// 卸载车站表
void CCSTesterDlg::UnLoadStationTable()
{
	map<BYTE, LINEINF>::iterator itor;
	for(itor=g_PrmStation.begin();itor!=g_PrmStation.end();++itor)
	{
		itor->second.vsta.clear();
	}
	g_PrmStation.clear();
}

// 显示“通讯方式”菜单前的小钩
void CCSTesterDlg::SetSubMenuCheck(int nSubPos, UINT uTargID)
{
	UINT nID, nCount;

	CMenu * pMenu = GetMenu()->GetSubMenu(nSubPos);
	nCount = pMenu->GetMenuItemCount();
	for (UINT i=0;i<nCount;i++)
	{
		nID = pMenu->GetMenuItemID(i);

		if (nID == uTargID)
			pMenu->CheckMenuItem(nID, MF_CHECKED);
		else
			pMenu->CheckMenuItem(nID, MF_UNCHECKED);

	}
}

int CCSTesterDlg::EnumSerials(CComboBox& comb)
{
	HKEY			hKey;
	int				nIndex		= 0;
	TCHAR			szName[64]	= {0};
	TCHAR			szValue[64]	= {0};
	DWORD			dwSize		= 64;
	DWORD			dwType		= REG_SZ;
	if(ERROR_SUCCESS == ::RegOpenKey(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", &hKey))
	{
		comb.ResetContent();
		while(ERROR_NO_MORE_ITEMS != RegEnumValue(hKey, nIndex++, szName, &dwSize, NULL, &dwType, (LPBYTE)szValue, &dwSize))
		{
			comb.AddString(szValue);
			ZeroMemory(szName, 64);
			ZeroMemory(szValue, 64);
			dwSize		= 64;
			dwType		= REG_SZ;
		}
	}

	int ret = comb.GetCount();
	if (ret > 0)
		comb.SetCurSel(0);

	return ret;

}

BEGIN_MESSAGE_MAP(CCSTesterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	ON_COMMAND(IDM_DEV_BOM, &CCSTesterDlg::OnDevBom)
	ON_COMMAND(IDM_DEV_AGM, &CCSTesterDlg::OnDevAgm)
	ON_COMMAND(IDM_DEV_TVM, &CCSTesterDlg::OnDevTvm)
	ON_COMMAND(IDM_DEV_TCM, &CCSTesterDlg::OnDevTcm)
	ON_COMMAND(IDM_SAVE_DISP, &CCSTesterDlg::OnSaveDisp)
	ON_COMMAND(IDM_CLEAR_DISP, &CCSTesterDlg::OnClearDisp)
	ON_COMMAND(IDM_EXIT, &CCSTesterDlg::OnExit)
	//}}AFX_MSG_MAP
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_ANALYSE, &CCSTesterDlg::OnAnalyse)
	ON_MESSAGE(WM_INCREASE, &CCSTesterDlg::OnIncrease)
	ON_MESSAGE(WM_DECREASE, &CCSTesterDlg::OnDecrease)
	ON_MESSAGE(WM_UPDATE, &CCSTesterDlg::OnUpdate)
	ON_MESSAGE(WM_REFUND, &CCSTesterDlg::OnRefund)
	ON_MESSAGE(WM_SJTSALE, &CCSTesterDlg::OnSjtsale)
	ON_MESSAGE(WM_SALEEX, &CCSTesterDlg::OnSaleEx)
	ON_MESSAGE(WM_INITREADER, &CCSTesterDlg::OnInitReader)
	ON_MESSAGE(WM_UNINITREADER, &CCSTesterDlg::OnUnInitReader)
	ON_MESSAGE(WM_CHANGE_ANT, &CCSTesterDlg::OnChangeAnt)
	ON_MESSAGE(WM_SAMACT, &CCSTesterDlg::OnSamAct)
	ON_MESSAGE(WM_DESCIND, &CCSTesterDlg::OnChargeDescind)

	ON_COMMAND(IDM_QUERY_FARE, &CCSTesterDlg::OnQueryFare)
	ON_COMMAND(IDM_PARAM, &CCSTesterDlg::OnParam)
	ON_COMMAND(ID_SAM_INFO, &CCSTesterDlg::OnSamInfo)
	ON_COMMAND(IDM_CMD, &CCSTesterDlg::OnCmd)
	ON_COMMAND(IDM_TEST_HARD, &CCSTesterDlg::OnTestHard)
	ON_COMMAND(IDM_READER_VERSION, &CCSTesterDlg::OnReaderVersion)
	ON_COMMAND(IDM_TRADE_FILE_EXCH, &CCSTesterDlg::OnTradeFileExch)
	ON_COMMAND(IDM_DEBUG, &CCSTesterDlg::OnDebug)
	ON_COMMAND(IDM_GETFILE, &CCSTesterDlg::OnGetFileFromReader)
	ON_COMMAND(IDM_UPGRADE, &CCSTesterDlg::OnUpgrade)
	ON_COMMAND(IDM_ENALBE_DEBUGER, &CCSTesterDlg::OnEnalbeDebuger)
	ON_COMMAND(IDM_SET_DEGRADE, &CCSTesterDlg::OnSetDegrade)
END_MESSAGE_MAP()


// CCSTesterDlg 消息处理程序
BOOL CCSTesterDlg::OnDeviceChange(UINT nEventType,DWORD dwData)
{
	m_wndDlgBar.SendMessage(WM_DEVICECHANGE);
	return TRUE;
}

BOOL CCSTesterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	QueryPerformanceFrequency(&g_liCpuFreq);

	CString strPath = GetStationPrmPath();
	LoadStationTable(strPath);

	m_bEnableDebuger = HasRootAuth();
	EnableRootMenus(m_bEnableDebuger);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCSTesterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCSTesterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CCSTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCSTesterDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	AddBars();

	m_pOper = new CDlgOper;
	m_pOper->Create(IDD_DLG_OPER, this);

	m_pBom = new CDlgBom;
	m_pBom->Create(IDD_DLG_BOM, this);

	m_pAgm = new CDlgAgm;
	m_pAgm->Create(IDD_DLG_AGM, this);

	m_pTvm = new CDlgTvm;
	m_pTvm->Create(IDD_DLG_TVM, this);

	m_pTcm = new CDlgTcm;
	m_pTcm->Create(IDD_DLG_TCM, this);

	m_pInc = new CDlgInc;
	m_pInc->Create(IDD_DLG_INCR, this);

	m_pDec = new CDlgDec;
	m_pDec->Create(IDD_DLG_DECR, this);

	m_pSjtSale = new CDlgSjtSale;
	m_pSjtSale->Create(IDD_DLG_SJTSALE, this);

	m_pRefund = new CDlgRefund;
	m_pRefund->Create(IDD_DLG_REFUND, this);

	m_pUpdate = new CDlgUpd;
	m_pUpdate->Create(IDD_DLG_UPDATE, this);

	m_pPrice = new CDlgPrice;
	m_pPrice->Create(IDD_DLG_PRICE, this);

	m_pParam = new CDlgParam;
	m_pParam->Create(IDD_DLG_PARAM, this);

	m_pSamAct = new CDlgSamAct;
	m_pSamAct->Create(IDD_DLG_SAM_ACT, this);

	m_pDescind = new CDlgDescind;
	m_pDescind->Create(IDD_DLG_DESCIND, this);

	m_pDebug = new CDlgDebug;
	m_pDebug->Create(IDD_DLG_DEBUG, this);

	m_pDegrade = new CDlgDegrade;
	m_pDegrade->Create(IDD_DLG_DEGRADE, this);

	m_pSaleEx = new CDlgSaleEx;
	m_pSaleEx->Create(IDD_DLG_SALE_EX, this);

	ChildWindowAnchor();

	m_pBom->SendMessage(WM_PAGECHANGED);
	m_pBom->ShowWindow(SW_SHOW);
	m_pPageLast = m_pBom;

	ShowWindow(SW_MAXIMIZE);

}

void CCSTesterDlg::OnDevBom()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pPageLast != m_pBom)
	{
		m_pPageLast->ShowWindow(SW_HIDE);

		m_pBom->SendMessage(WM_PAGECHANGED);
		m_pBom->ShowWindow(SW_SHOW);

		// 所有天线模式换为副天线
		SendMessage(WM_CHANGE_ANT, 0x01);
		m_pPageLast = m_pBom;
		ShowDialogs(0);
	}
}

void CCSTesterDlg::OnDevAgm()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pPageLast != m_pAgm)
	{
		m_pPageLast->ShowWindow(SW_HIDE);

		m_pAgm->SendMessage(WM_PAGECHANGED);
		m_pAgm->ShowWindow(SW_SHOW);
		m_pPageLast = m_pAgm;
		ShowDialogs(0);
	}
}

void CCSTesterDlg::OnDevTvm()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pPageLast != m_pTvm)
	{
		m_pPageLast->ShowWindow(SW_HIDE);

		m_pTvm->SendMessage(WM_PAGECHANGED);
		m_pTvm->ShowWindow(SW_SHOW);
		m_pPageLast = m_pTvm;
		ShowDialogs(0);
	}
}

void CCSTesterDlg::OnDevTcm()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pPageLast != m_pTcm)
	{
		m_pPageLast->ShowWindow(SW_HIDE);

		m_pTcm->SendMessage(WM_PAGECHANGED);
		m_pTcm->ShowWindow(SW_SHOW);
		m_pPageLast = m_pTcm;
		ShowDialogs(0);
	}
}

void CCSTesterDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	if (m_rEditOut.GetSafeHwnd())
	{
		ChildWindowAnchor();
	}
}

void CCSTesterDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnGetMinMaxInfo(lpMMI);
	lpMMI-> ptMinTrackSize.x = 800; 
	lpMMI-> ptMinTrackSize.y = 500; 
}


COLORREF corText[3] = {RGB(200, 0, 0), RGB(0, 200, 0), RGB(0, 0, 200),};
int g_nCount = 0;
void CCSTesterDlg::AppendText(CString& strText)
{

	int nLen;
	CHARFORMAT cf = {0};
	g_nCount = (g_nCount + 1) % 3;
	cf.crTextColor = corText[g_nCount];
	cf.dwEffects = ~CFE_AUTOCOLOR; 
	cf.dwMask = CFM_COLOR;	


	nLen = m_rEditOut.GetWindowTextLength(); 
	m_rEditOut.SetFocus (); 
	m_rEditOut.SetSel (nLen, nLen); 
	m_rEditOut.SetWordCharFormat(cf);
	m_rEditOut.ReplaceSel (strText);
}

void CCSTesterDlg::SetStatusBar(CString strOper, RETINFO Error, LARGE_INTEGER liStart, LARGE_INTEGER liEnd)
{
	char szError[128] = {0};
	char szTime[32] = {0};
	char szTemp[32]	= {0};
	long gap = 0;

	sprintf_s(szError, sizeof(szError), "返回码：%04X,%02X  %s", Error.wErrCode, Error.bNoticeCode, szTemp);

	//gap = liEnd.QuadPart - liStart.QuadPart;

	//if(gap > 500)
		//gap = gap - 300;


	sprintf_s(szTime, sizeof(szTime), "耗时:%8lf", ((double)(liEnd.QuadPart - liStart.QuadPart) / (double)(g_liCpuFreq.QuadPart)));

	m_WndStatus.SetText(strOper, 0, SBT_POPOUT);
	m_WndStatus.SetText(szError, 1, SBT_POPOUT);
	m_WndStatus.SetText(szTime, 2, SBT_POPOUT);
}

void CCSTesterDlg::FormHeader(CString& strTarg, CString strSrc)
{
	FormOut(strTarg, 2);
	CString strTemp = COMPART_LINE;
	int nPos = (int)(strTemp.GetLength() - strSrc.GetLength()) / 2;

	// 2是前面空格的长度
	memcpy(strTemp.GetBuffer() + nPos + 2, strSrc.GetBuffer(), strSrc.GetLength());
	strTarg = strTemp + "\n" + strTarg + "\n";
}

void CCSTesterDlg::FormOut(CString& strTarg, int nRow)
{
	int nPosEnter;
	int nIndex, nLen;
	CString strTemp, strSrc;

	nIndex = 0;
	strSrc = strTarg;
	strTarg = "";

	while(1) 
	{
		nLen = strSrc.GetLength();
		if (nLen <= 0)	
		{
			if (nIndex % nRow != 0)
				strTarg += "\n";				
			break;
		}

		nPosEnter = strSrc.Find("\n");
		if (nPosEnter > 0)
		{
			strTemp = strSrc.Left(nPosEnter);
			strSrc = strSrc.Right(nLen - nPosEnter - 1);
		}
		else
		{
			strTemp = strSrc;
			strSrc = "";
		}

		AddSpace(strTemp);
		nIndex ++;

		strTarg += strTemp;
		if (nIndex % nRow == 0)
			strTarg += "\n";
		else
			strTarg += "|";
	}
}

#define LEN_NAME	26		// 输出字段名带空格的长度
#define LEN_VALUE	28		// 输出字段值带空格的长度
void CCSTesterDlg::AddSpace(CString& str)
{
	int nPosSep;
	int nLen;
	char szlSPace[LEN_NAME] = {0};
	char szrSpace[LEN_VALUE] = {0};

	nPosSep = str.Find(":");
	if (nPosSep > 0)
	{
		nLen = str.GetLength();
		memset(szlSPace, 0x20, LEN_NAME - nPosSep);
		if (LEN_VALUE > nLen - nPosSep - 1)
			memset(szrSpace, 0x20, LEN_VALUE - (nLen - nPosSep - 1));
		//else
		//	memset(szrSpace, 0x20, LEN_VALUE - (nLen - nPosSep - 1));

		str = CString(szlSPace) + str + CString(szrSpace);
	}
}

void CCSTesterDlg::BCDTimetoString(LPBYTE pbSrc, BYTE bLenSrc, char * pszTarg)
{
	char szTemp[16] = {0};

	sprintf_s(szTemp, sizeof(szTemp), "%02X%02X-%02X-%02X", pbSrc[0], pbSrc[1], pbSrc[2], pbSrc[3]);
	strcpy(pszTarg, szTemp);

	if (bLenSrc > 4)
	{
		sprintf_s(szTemp, sizeof(szTemp), " %02X:%02X:%02X", pbSrc[4], pbSrc[5], pbSrc[6]);
		strcat_s(pszTarg, bLenSrc * 2 + 6, szTemp);
	}
}

void CCSTesterDlg::BCDtoString(LPBYTE pbSrc, BYTE bLenSrc, char * pszTarg)
{
	for (BYTE i=0;i<bLenSrc;i++)
	{
		sprintf(pszTarg + 2 * i, "%02X", pbSrc[i]);
	}
}

void CCSTesterDlg::OnSaveDisp()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(FALSE, "txt", NULL, 4 | 2, "文本文件 (*.txt)|*.txt|所有文件 (*.*)|*.*||");
	if (dlg.DoModal() == IDOK)
	{
		CString str, strPath;

		strPath = dlg.GetPathName();
		m_rEditOut.GetWindowText(str);

		CStdioFile sfile;
		if (sfile.Open(strPath, CFile::modeCreate | CFile::modeWrite))
		{
			sfile.WriteString(str);
			sfile.Close();
		}
	}
}

void CCSTesterDlg::OnClearDisp()
{
	// TODO: 在此添加命令处理程序代码
	m_rEditOut.SetWindowText("");
}

void CCSTesterDlg::OnExit()
{
	// TODO: 在此添加命令处理程序代码
	g_Serial.Close();

	CDialog::OnCancel();
}

void CCSTesterDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	g_Serial.Close();

	CDialog::OnClose();
}

LRESULT CCSTesterDlg::OnAnalyse(WPARAM wparam, LPARAM lparam)
{
	m_pOper->SendMessage(WM_ANALYSE);

	if (g_bDeviceType == 3)
		ShowDialogs(1 << DLG_OPER);

	g_lBalance = g_Analyse.lBalance;

	return 0;
}

LRESULT CCSTesterDlg::OnIncrease(WPARAM wparam, LPARAM lparam)
{
	ShowDialogs(1 << DLG_INC);
	m_pInc->SetOldBalance(g_lBalance);

	return 0;
}

LRESULT CCSTesterDlg::OnDecrease(WPARAM wparam, LPARAM lparam)
{
	ShowDialogs(1 << DLG_DEC);
	m_pDec->SetOldBalance(g_lBalance);

	return 0;
}

LRESULT CCSTesterDlg::OnUpdate(WPARAM wparam, LPARAM lparam)
{
	ShowDialogs(1 << DLG_UPD);
	m_pUpdate->SetUpdInfo(0);

	return 0;
}

LRESULT CCSTesterDlg::OnRefund(WPARAM wparam, LPARAM lparam)
{
	m_pRefund->SetChecker((BOOL)(g_Analyse.dwOperationStauts & (0x01<<5)), (BOOL)(g_Analyse.dwOperationStauts & (0x01<<7)));
	ShowDialogs(1 << DLG_RFD);
	return 0;
}

LRESULT CCSTesterDlg::OnSjtsale(WPARAM wparam, LPARAM lparam)
{
	ShowDialogs(1 << DLG_SSALE);
	return 0;
}

LRESULT CCSTesterDlg::OnSaleEx(WPARAM wparam, LPARAM lparam)
{
	ShowDialogs(1 << DLG_SALEEX);
	return 0;
}

LRESULT CCSTesterDlg::OnSamAct(WPARAM wparam, LPARAM lparam)
{
	ShowDialogs(1 << DLG_SAM_ACT);
	return 0;
}

LRESULT CCSTesterDlg::OnChargeDescind(WPARAM wparam, LPARAM lparam)
{
	ShowDialogs(1 << DLG_DESCIND);
	return 0;
}

LRESULT CCSTesterDlg::OnInitReader(WPARAM wparam, LPARAM lparam)
{
	m_pUpdate->	SetEntryLine();
	m_pPrice->SetStation();

	CMenu * pMenu = GetMenu()->GetSubMenu(2);
	pMenu->EnableMenuItem(IDM_QUERY_FARE, MF_ENABLED);
	pMenu->EnableMenuItem(IDM_PARAM, MF_ENABLED);

	pMenu = GetMenu()->GetSubMenu(5);
	pMenu->EnableMenuItem(IDM_READER_VERSION, MF_ENABLED);

	pMenu = GetMenu()->GetSubMenu(4);
	pMenu->EnableMenuItem(IDM_DEBUG, MF_ENABLED);

	return 0;
}

LRESULT CCSTesterDlg::OnUnInitReader(WPARAM wparam, LPARAM lparam)
{
	CMenu * pMenu = GetMenu()->GetSubMenu(2);
	pMenu->EnableMenuItem(IDM_QUERY_FARE, MF_DISABLED);
	pMenu->EnableMenuItem(IDM_PARAM, MF_DISABLED);

	pMenu = GetMenu()->GetSubMenu(5);
	pMenu->EnableMenuItem(IDM_READER_VERSION, MF_DISABLED);

	pMenu = GetMenu()->GetSubMenu(4);
	pMenu->EnableMenuItem(IDM_DEBUG, MF_DISABLED);

	return 0;
}

LRESULT CCSTesterDlg::OnChangeAnt(WPARAM wparam, LPARAM lparam)
{
	int nAnt = (int)wparam;
	m_pSjtSale->SetAntiMode(nAnt);
	m_pDec->SetAntiMode(nAnt);
	m_pInc->SetAntiMode(nAnt);

	return 0;
}
BOOL CCSTesterDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: 在此添加专用代码和/或调用基类
	//if (m_wndDlgBar.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	//	return TRUE;
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CCSTesterDlg::OnCoreUpdt()
{
	// TODO: 在此添加命令处理程序代码
	ShowDialogs(1 << DLG_CORE);

}

void CCSTesterDlg::OnQueryFare()
{
	// TODO: 在此添加命令处理程序代码
	ShowDialogs(1 << DLG_PRICE);
}

void CCSTesterDlg::OnParam()
{
	// TODO: 在此添加命令处理程序代码
	ShowDialogs(1 << DLG_PARAM);
}

void CCSTesterDlg::OnSamInfo()
{
	// TODO: 在此添加命令处理程序代码
	BYTE bRecv[1024]	= {0};
	SAMSTATUS ss[8]		= {0};
	WORD lenss			= 0;

	CString strDevType;

	QueryPerformanceCounter(&g_liStart);
	g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 2, NULL, 0, bRecv, sizeof(bRecv));
	QueryPerformanceCounter(&g_liEnd);

	if (g_retInfo.wErrCode == 0)
	{
		memcpy(&lenss, bRecv + 7 + sizeof(RETINFO), 2);

		if (lenss > 0)
		{
			memcpy(&ss, bRecv + 9 + sizeof(RETINFO), lenss);
		}

	}

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Sam, &ss, lenss, "获取SAM卡信息", g_liStart, g_liEnd);
}

void CCSTesterDlg::OnCmd()
{
	// TODO: 在此添加命令处理程序代码

	m_pPageLast->SendMessage(WM_PAGECHANGED);
	
	CDlgCmd dlg;
	dlg.DoModal();
}

void CCSTesterDlg::OnTestHard()
{
	// TODO: 在此添加命令处理程序代码


	m_pPageLast->SendMessage(WM_PAGECHANGED);

	CDlgHarder dlg;
	dlg.DoModal();
}

void CCSTesterDlg::OnReaderVersion()
{
	// TODO: 在此添加命令处理程序代码

	// TODO: 在此添加命令处理程序代码
	BYTE bRecv[1024]	= {0};
	READERVERSION rv[8]	= {0};
	WORD lenrv			= 0;

	CString strDevType;

	QueryPerformanceCounter(&g_liStart);
	g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 1, NULL, 0, bRecv, sizeof(bRecv));
	QueryPerformanceCounter(&g_liEnd);

	if (g_retInfo.wErrCode == 0)
	{
		memcpy(&lenrv, bRecv + 7 + sizeof(RETINFO), 2);

		if (lenrv > 0)
		{
			memcpy(&rv, bRecv + 9 + sizeof(RETINFO), lenrv);
		}

	}

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Ver, &rv, lenrv, "获取版本信息", g_liStart, g_liEnd);

}

void CCSTesterDlg::OnTradeFileExch()
{
	// TODO: 在此添加命令处理程序代码

	char szFilter[1024] = {0};
	strcat(szFilter, "单程票发售 (*.sjt)|*.sjt|");
	strcat(szFilter, "储值票发售 (*.ots)|*.ots|");
	strcat(szFilter, "进闸 (*.eng)|*.eng|");
	strcat(szFilter, "钱包交易 (*.pur)|*.pur|");
	strcat(szFilter, "延期 (*.dfr)|*.dfr|");
	strcat(szFilter, "更新 (*.upd)|*.upd|");
	strcat(szFilter, "退款 (*.ref)|*.ref|");
	strcat(szFilter, "加解锁 (*.loc)|*.loc|");
	strcat(szFilter, "所有文件 (*.*)|*.*|");

	CString strFileName, strFolder, strPathName;

	CFileDialog dlg(TRUE, "PRM", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	if (dlg.DoModal() == IDOK)
	{
		strFileName = dlg.GetFileName();
		strPathName = dlg.GetPathName();
		strFolder = strPathName.Left(strPathName.GetLength() - strFileName.GetLength());

		if (CTradeFileConvert::FileConvert(strFileName.GetBuffer(), strPathName.GetBuffer(), strFolder.GetBuffer()))
		{
			AfxMessageBox("转换完成！");
		}
	}
}

void CCSTesterDlg::OnDebug()
{
	// TODO: 在此添加命令处理程序代码
	ShowDialogs(1 << DLG_DEBUG);
}

void CCSTesterDlg::OnGetFileFromReader()
{
	m_pPageLast->SendMessage(WM_PAGECHANGED);

	CDlgNfs dlg;
	dlg.DoModal();
}

void CCSTesterDlg::OnUpgrade()
{
	// TODO: 在此添加命令处理程序代码
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	do 
	{
		QueryPerformanceCounter(&g_liStart);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 255, NULL, 0, bRecv, sizeof(bRecv));
		QueryPerformanceCounter(&g_liEnd);

		pos += sizeof(RETINFO);

	} while (0);

	((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Null, NULL, 0, "票卡格式更改", g_liStart, g_liEnd);

}

// 交易确认
void CCSTesterDlg::TransConfirm(RETINFO ret)
{
	BYTE bRecv[1024]	= {0};
	int pos				= 7;

	if (ret.bNoticeCode == 0x11)
	{
		CDlgConfirm dlg;
		dlg.DoModal();
	}
}

void CCSTesterDlg::OnEnalbeDebuger()
{
	// TODO: 在此添加命令处理程序代码
	CMenu * pMenuTools = GetMenu()->GetSubMenu(4);
	CMenu * pMenuHelp = GetMenu()->GetSubMenu(5);

	if (!m_bEnableDebuger)
	{
		CDlgRoot dlg;
		dlg.DoModal();

		EnableRootMenus(m_bEnableDebuger);
	}
}

void CCSTesterDlg::OnSetDegrade()
{
	// TODO: 在此添加命令处理程序代码
	ShowDialogs(1 << DLG_DEGRADE);
}

void CCSTesterDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类

	delete m_pBom;
	delete m_pAgm;
	delete m_pTvm;
	delete m_pTcm;
	delete m_pOper;
	delete m_pInc;
	delete m_pDec;
	delete m_pSjtSale;
	delete m_pRefund;
	delete m_pUpdate;
	delete m_pPrice;
	delete m_pParam;
	delete m_pDebug;
	delete m_pDegrade;
	delete m_pSaleEx;

	CDialog::PostNcDestroy();
}

BOOL CCSTesterDlg::HasRootAuth()
{
	BOOL bRet = FALSE;

	CString strCpuID = CDlgRoot::GetCpuID();

	const CString strPermitID[2] = 
	{
		"BFEBFBFF000306D400C3000076036301",
		"BFEBFBFF000206A700CA000076035A01"
	};

	for (int i=0;i<2;i++)
	{
		if (strCpuID == strPermitID[i])
			bRet = TRUE;
	}

	return bRet;
}

void CCSTesterDlg::EnableRootMenus(BOOL bRoot)
{
	CMenu * pMenuTools = GetMenu()->GetSubMenu(4);
	CMenu * pMenuHelp = GetMenu()->GetSubMenu(5);

	if (bRoot)
	{
		pMenuHelp->CheckMenuItem(IDM_ENALBE_DEBUGER, MF_CHECKED);

		pMenuTools->EnableMenuItem(IDM_CMD, MF_ENABLED);
		pMenuTools->EnableMenuItem(IDM_TEST_HARD, MF_ENABLED);
		pMenuTools->EnableMenuItem(IDM_TRADE_FILE_EXCH, MF_ENABLED);
		pMenuTools->EnableMenuItem(IDM_DEBUG, MF_ENABLED);
		pMenuTools->EnableMenuItem(IDM_UPGRADE, MF_ENABLED);
		pMenuTools->EnableMenuItem(IDM_GETFILE, MF_ENABLED);
		pMenuTools->EnableMenuItem(IDM_SET_DEGRADE, MF_ENABLED);
	}
	else
	{
		pMenuHelp->CheckMenuItem(IDM_ENALBE_DEBUGER, MF_UNCHECKED);
		pMenuTools->EnableMenuItem(IDM_CMD, MF_DISABLED);
		pMenuTools->EnableMenuItem(IDM_TEST_HARD, MF_DISABLED);
		pMenuTools->EnableMenuItem(IDM_TRADE_FILE_EXCH, MF_DISABLED);
		pMenuTools->EnableMenuItem(IDM_DEBUG, MF_DISABLED);
		pMenuTools->EnableMenuItem(IDM_UPGRADE, MF_DISABLED);
		pMenuTools->EnableMenuItem(IDM_GETFILE, MF_DISABLED);
		pMenuTools->EnableMenuItem(IDM_SET_DEGRADE, MF_DISABLED);
	}
}