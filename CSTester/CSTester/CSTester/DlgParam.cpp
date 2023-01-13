// DlgParam.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "DlgParam.h"
#include "CSTesterDlg.h"


// CDlgParam 对话框

IMPLEMENT_DYNAMIC(CDlgParam, CDialog)

CDlgParam::CDlgParam(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgParam::IDD, pParent)
{

}

CDlgParam::~CDlgParam()
{
}

void CDlgParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CDlgParam::IsValidParam(LPCTSTR lpszName, int& nType)
{
	BOOL bRet = FALSE;
	if (memcmp(lpszName, "PRM.", 4) == 0 && lpszName[8] == '.' && lpszName[17] == '.')
	{
		nType = atoi(lpszName + 4);
		bRet = TRUE;
	}
	return bRet;
}

BEGIN_MESSAGE_MAP(CDlgParam, CDialog)
	ON_BN_CLICKED(IDC_BN_CUR, &CDlgParam::OnBnClickedBnCur)
	ON_BN_CLICKED(IDC_BN_CFG, &CDlgParam::OnBnClickedBnCfg)
	ON_BN_CLICKED(IDC_BN_CFG2, &CDlgParam::OnBnClickedBnCfg2)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CDlgParam::OnBnClickedBnCancel)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BN_STATION, &CDlgParam::OnBnClickedBnStation)
	//ON_BN_CLICKED(IDC_BN_CUR2, &CDlgParam::OnBnClickedBnCur2)
END_MESSAGE_MAP()


// CDlgParam 消息处理程序

void CDlgParam::OnBnClickedBnCur()
{
	// TODO: 在此添加控件通知处理程序代码
	char szName[MAX_PATH] = {0};
	WORD lenName = 0;
	BYTE bRecv[512] = {0};
	BYTE bData[8] = {0};
	int nType[] = { 0x0101,0x0202, 0x0203, 0x0204, 0x0301, 0x0302, 0x0303, 0x0400, 0x0501, 0x0601, 0x0602, 0x0603, 0x0604, 0x0605, 0x0606, 0x0801, 0x9320 };
	int nItpType[] = { 0x0001,0x0002, 0x0003, 0x0004, 0x0010, 0x0012, 0x1002};
	CString strParams;
	CString strTemp;

	((CCSTesterDlg * )AfxGetMainWnd())->FormHeader(strParams, "当前参数");
	strParams.Delete(strParams.GetLength() - 1);	// 删除换行符
	for (int i=0;i<sizeof(nType)/sizeof(nType[0]);i++)
	{
		bData[0] = (BYTE)((nType[i] >> 8) & 0xFF);
		bData[1] = (BYTE)(nType[i] & 0xFF);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 5, bData, 2, bRecv, sizeof(bRecv));
		if (g_retInfo.wErrCode == 0)
		{
			memset(szName, 0, MAX_PATH);
			memcpy(&lenName, bRecv + 12, 2);
			memcpy(szName, bRecv + 14, lenName);
			strTemp.Format("\t\t\t%04x:%s\n", nType[i], szName);
		}
		else
		{
			strTemp.Format("\t\t\t%04x:获取信息失败，错误%04x\n", nType[i], g_retInfo.wErrCode);
		}

		strParams += strTemp;
	}
	strParams += "\n";

	for (int i=0;i<sizeof(nItpType)/sizeof(nItpType[0]);i++)
	{
		bData[0] = (BYTE)((nItpType[i] >> 8) & 0xFF);
		bData[1] = (BYTE)(nItpType[i] & 0xFF);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 5, bData, 2, bRecv, sizeof(bRecv));
		if (g_retInfo.wErrCode == 0)
		{
			memset(szName, 0, MAX_PATH);
			memcpy(&lenName, bRecv + 12, 2);
			memcpy(szName, bRecv + 14, lenName);
			strTemp.Format("\t\t\t%04x:%s\n", nItpType[i], szName);
		}
		else
		{
			strTemp.Format("\t\t\t%04x:获取信息失败，错误%04x\n", nItpType[i], g_retInfo.wErrCode);
		}

		strParams += strTemp;
	}
	strParams += "\n";

	((CCSTesterDlg * )AfxGetMainWnd())->AppendText(strParams);
}

void CDlgParam::OnBnClickedBnCfg()
{
	// TODO: 在此添加控件通知处理程序代码
	#define ONCE_TRANSFER	10240

	CString strOut, strPath, strName;
	BYTE bParamReady[64]	= {0};
	BYTE bParamSend[10300]	= {0};
	BYTE bRecv[128]			= {0};
	WORD lenData			= 0;
	DWORD dwFileSize		= 0;
	DWORD dwPos				= 0;
	DWORD dwLast			= 0;
	DWORD dwFileCrc32		= 0;
	char szRate[64]			= {0};
	CProgressCtrl * pCtrl	= (CProgressCtrl *)GetDlgItem(IDC_PROGRESS_SEND);


	CFileDialog dlg(TRUE, "PRM", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "参数文件 (PRM.*)|PRM.*|所有文件 (*.*)|*.*||");
	
	if (dlg.DoModal() == IDOK)
	{
		strPath = dlg.GetPathName();
		strName = dlg.GetFileName();

		LPBYTE pFileData = PrepareSendParam(strPath, dwFileSize, dwFileCrc32);
		if (pFileData == NULL)	return;

		pCtrl->ShowWindow(SW_SHOW);

		QueryPerformanceCounter(&g_liStart);

		// 参数通知
		memcpy(bParamReady + 1, &dwFileSize, 4);
		memcpy(bParamReady + 5, &dwFileCrc32, 4);
		bParamReady[9] = (BYTE)strName.GetLength();
		memcpy(bParamReady + 10, strName.GetBuffer(), strName.GetLength());
		lenData = 10 + strName.GetLength();

		pCtrl->SetRange32(0, dwFileSize);

		SetDlgItemText(IDC_STATIC_RATE, "参数下发通知");

		g_retInfo = g_Serial.Communicate(0, g_nBeep, 2, 0, bParamReady, lenData, bRecv, sizeof(bRecv));
		if (g_retInfo.wErrCode == 0)
		{
			// 参数下载
			dwLast = dwFileSize;
			while(dwPos < dwFileSize)
			{
				if (dwLast < ONCE_TRANSFER)
					lenData = (WORD)dwLast;
				else
					lenData = (WORD)ONCE_TRANSFER;

				dwLast = dwFileSize - dwPos - lenData;

				memcpy(bParamSend, &lenData, 2);
				memcpy(bParamSend + 2, &dwLast, 4);
				memcpy(bParamSend + 6, pFileData + dwPos, lenData);
				g_retInfo = g_Serial.Communicate(0, g_nBeep, 2, 1, bParamSend, lenData + 6, bRecv, sizeof(bRecv));
				if (g_retInfo.wErrCode != 0)
				{
					sprintf(szRate, "参数下载失败：%d%%", dwPos * 100 / dwFileSize);
					SetDlgItemText(IDC_STATIC_RATE, szRate);
					break;
				}

				dwPos += lenData;

				pCtrl->SetPos(dwPos);
				sprintf(szRate, "%d%%", dwPos * 100 / dwFileSize);
				SetDlgItemText(IDC_STATIC_RATE, szRate);
			}

			// 参数启用
			if (g_retInfo.wErrCode == 0)
			{
				SetDlgItemText(IDC_STATIC_RATE, "参数启用通知");

				bParamReady[0] = 0x01;
				lenData = 10 + strName.GetLength();
				g_retInfo = g_Serial.Communicate(0, g_nBeep, 2, 0, bParamReady, lenData, bRecv, sizeof(bRecv));

				SetDlgItemText(IDC_STATIC_RATE, "完成");
				pCtrl->ShowWindow(SW_HIDE);
			}
		}

		delete []pFileData;

		QueryPerformanceCounter(&g_liEnd);

		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Null, NULL, 0, "参数下载", g_liStart, g_liEnd);

	}
}

void CDlgParam::OnBnClickedBnCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
}

LPBYTE CDlgParam::PrepareSendParam(CString strPath, DWORD& dwSize, DWORD& dwCRC32)
{
	CFile file;
	BYTE * pFile;

	if (file.Open(strPath, CFile::modeRead))
	{
		dwSize = (DWORD)file.GetLength();
		pFile = new(std::nothrow) BYTE[dwSize];
		if (pFile != NULL)
		{
			file.Read(pFile, dwSize);
			dwCRC32 = Crc32(pFile, dwSize);
		}
	}

	return pFile;
}

void CDlgParam::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	GetDlgItem(IDC_PROGRESS_SEND)->ShowWindow(SW_HIDE);
}

void CDlgParam::UpdateProc(LPVOID lparam)
{
//#define ONCE_TRANSFER	10240
//
//	CString strOut, strPath, strName;
//	BYTE bParamReady[64]	= {0};
//	BYTE bParamSend[10300]	= {0};
//	BYTE bRecv[128]			= {0};
//	WORD lenData			= 0;
//	DWORD dwFileSize		= 0;
//	DWORD dwPos				= 0;
//	DWORD dwLast			= 0;
//	DWORD dwFileCrc32		= 0;
//	char szRate[64]			= {0};
//	CProgressCtrl * pCtrl	= (CProgressCtrl *)GetDlgItem(IDC_PROGRESS_SEND);
//
//
//	CFileDialog dlg(TRUE, "PRM", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "参数文件 (PRM.*)|PRM.*|所有文件 (*.*)|*.*||");
//
//	if (dlg.DoModal() == IDOK)
//	{
//		strPath = dlg.GetPathName();
//		strName = dlg.GetFileName();
//
//		LPBYTE pFileData = PrepareSendParam(strPath, dwFileSize, dwFileCrc32);
//		if (pFileData == NULL)	return;
//
//		pCtrl->ShowWindow(SW_SHOW);
//
//		QueryPerformanceCounter(&g_liStart);
//
//		// 参数通知
//		memcpy(bParamReady + 1, &dwFileSize, 4);
//		memcpy(bParamReady + 5, &dwFileCrc32, 4);
//		bParamReady[9] = (BYTE)strName.GetLength();
//		memcpy(bParamReady + 10, strName.GetBuffer(), strName.GetLength());
//		lenData = 10 + strName.GetLength();
//
//		pCtrl->SetRange32(0, dwFileSize);
//
//		SetDlgItemText(IDC_STATIC_RATE, "参数下发通知");
//
//		g_retInfo = g_Serial.Communicate(0, g_nBeep, 2, 0, bParamReady, lenData, bRecv, sizeof(bRecv));
//		if (g_retInfo.wErrCode == 0)
//		{
//			// 参数下载
//			dwLast = dwFileSize;
//			while(dwPos < dwFileSize)
//			{
//				if (dwLast < ONCE_TRANSFER)
//					lenData = (WORD)dwLast;
//				else
//					lenData = (WORD)ONCE_TRANSFER;
//
//				dwLast = dwFileSize - dwPos - lenData;
//
//				memcpy(bParamSend, &lenData, 2);
//				memcpy(bParamSend + 2, &dwLast, 4);
//				memcpy(bParamSend + 6, pFileData + dwPos, lenData);
//				g_retInfo = g_Serial.Communicate(0, g_nBeep, 2, 1, bParamSend, lenData + 6, bRecv, sizeof(bRecv));
//				if (g_retInfo.wErrCode != 0)
//				{
//					sprintf(szRate, "参数下载失败：%d%%", dwPos * 100 / dwFileSize);
//					SetDlgItemText(IDC_STATIC_RATE, szRate);
//					break;
//				}
//
//				dwPos += lenData;
//
//				pCtrl->SetPos(dwPos);
//				sprintf(szRate, "%d%%", dwPos * 100 / dwFileSize);
//				SetDlgItemText(IDC_STATIC_RATE, szRate);
//			}
//
//			// 参数启用
//			if (g_retInfo.wErrCode == 0)
//			{
//				SetDlgItemText(IDC_STATIC_RATE, "参数启用通知");
//
//				bParamReady[0] = 0x01;
//				lenData = 10 + strName.GetLength();
//				g_retInfo = g_Serial.Communicate(0, g_nBeep, 2, 0, bParamReady, lenData, bRecv, sizeof(bRecv));
//
//				SetDlgItemText(IDC_STATIC_RATE, "完成");
//				pCtrl->ShowWindow(SW_HIDE);
//			}
//		}
//
//		delete []pFileData;
//
//		QueryPerformanceCounter(&g_liEnd);
//
//		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Null, NULL, 0, "参数下载", g_liStart, g_liEnd);
//
//	}
}
void CDlgParam::OnBnClickedBnStation()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE, "PRM", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "参数文件 (PRM.0201.*)|PRM.0201.*|所有文件 (*.*)|*.*||");

	if (dlg.DoModal() == IDOK)
	{
		CString strPathSrc = dlg.GetPathName();
		CString strPathTarg = ((CCSTesterDlg * )AfxGetMainWnd())->GetStationPrmPath();
		CopyFile(strPathSrc.GetBuffer(), strPathTarg.GetBuffer(), FALSE);
		((CCSTesterDlg * )AfxGetMainWnd())->LoadStationTable(strPathTarg);
	}
}


void CDlgParam::OnBnClickedBnCfg2()
{
	// TODO: 在此添加控件通知处理程序代码
#define ONCE_TRANSFER	10240

	CString strOut, strPath, strName;
	BYTE bParamReady[64]	= {0};
	BYTE bParamSend[10300]	= {0};
	BYTE bRecv[128]			= {0};
	WORD lenData			= 0;
	DWORD dwFileSize		= 0;
	DWORD dwPos				= 0;
	DWORD dwLast			= 0;
	DWORD dwFileCrc32		= 0;
	char szRate[64]			= {0};
	CProgressCtrl * pCtrl	= (CProgressCtrl *)GetDlgItem(IDC_PROGRESS_SEND);


	CFileDialog dlg(TRUE, "IPRM", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "参数文件 (IPRM.*)|IPRM.*|所有文件 (*.*)|*.*||");

	if (dlg.DoModal() == IDOK)
	{
		strPath = dlg.GetPathName();
		strName = dlg.GetFileName();

		LPBYTE pFileData = PrepareSendParam(strPath, dwFileSize, dwFileCrc32);
		if (pFileData == NULL)	return;

		pCtrl->ShowWindow(SW_SHOW);

		QueryPerformanceCounter(&g_liStart);

		// 参数通知
		memcpy(bParamReady + 1, &dwFileSize, 4);
		memcpy(bParamReady + 5, &dwFileCrc32, 4);
		bParamReady[9] = (BYTE)strName.GetLength();
		memcpy(bParamReady + 10, strName.GetBuffer(), strName.GetLength());
		lenData = 10 + strName.GetLength();

		pCtrl->SetRange32(0, dwFileSize);

		SetDlgItemText(IDC_STATIC_RATE, "参数下发通知");

		g_retInfo = g_Serial.Communicate(0, g_nBeep, 4, 0, bParamReady, lenData, bRecv, sizeof(bRecv));
		if (g_retInfo.wErrCode == 0)
		{
			// 参数下载
			dwLast = dwFileSize;
			while(dwPos < dwFileSize)
			{
				if (dwLast < ONCE_TRANSFER)
					lenData = (WORD)dwLast;
				else
					lenData = (WORD)ONCE_TRANSFER;

				dwLast = dwFileSize - dwPos - lenData;

				memcpy(bParamSend, &lenData, 2);
				memcpy(bParamSend + 2, &dwLast, 4);
				memcpy(bParamSend + 6, pFileData + dwPos, lenData);
				g_retInfo = g_Serial.Communicate(0, g_nBeep, 4, 1, bParamSend, lenData + 6, bRecv, sizeof(bRecv));
				if (g_retInfo.wErrCode != 0)
				{
					sprintf(szRate, "参数下载失败：%d%%", dwPos * 100 / dwFileSize);
					SetDlgItemText(IDC_STATIC_RATE, szRate);
					break;
				}

				dwPos += lenData;

				pCtrl->SetPos(dwPos);
				sprintf(szRate, "%d%%", dwPos * 100 / dwFileSize);
				SetDlgItemText(IDC_STATIC_RATE, szRate);
			}

			// 参数启用
			if (g_retInfo.wErrCode == 0)
			{
				SetDlgItemText(IDC_STATIC_RATE, "参数启用通知");

				bParamReady[0] = 0x01;
				lenData = 10 + strName.GetLength();
				g_retInfo = g_Serial.Communicate(0, g_nBeep, 4, 0, bParamReady, lenData, bRecv, sizeof(bRecv));

				SetDlgItemText(IDC_STATIC_RATE, "完成");
				pCtrl->ShowWindow(SW_HIDE);
			}
		}

		delete []pFileData;

		QueryPerformanceCounter(&g_liEnd);

		((CCSTesterDlg * )AfxGetMainWnd())->DispResult(struct_Null, NULL, 0, "参数下载", g_liStart, g_liEnd);

	}
}

void CDlgParam::OnBnClickedBnCur2()
{
	// TODO: 在此添加控件通知处理程序代码
	char szName[MAX_PATH] = {0};
	WORD lenName = 0;
	BYTE bRecv[512] = {0};
	BYTE bData[8] = {0};
	int nType[] = { 0x0001,0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0080};
	CString strParams;
	CString strTemp;

	((CCSTesterDlg * )AfxGetMainWnd())->FormHeader(strParams, "当前参数");
	strParams.Delete(strParams.GetLength() - 1);	// 删除换行符
	for (int i=0;i<sizeof(nType)/sizeof(nType[0]);i++)
	{
		bData[0] = (BYTE)((nType[i] >> 8) & 0xFF);
		bData[1] = (BYTE)(nType[i] & 0xFF);
		g_retInfo = g_Serial.Communicate(0, g_nBeep, 1, 5, bData, 2, bRecv, sizeof(bRecv));
		if (g_retInfo.wErrCode == 0)
		{
			memset(szName, 0, MAX_PATH);
			memcpy(&lenName, bRecv + 12, 2);
			memcpy(szName, bRecv + 14, lenName);
			strTemp.Format("\t\t\t%04x:%s\n", nType[i], szName);
		}
		else
		{
			strTemp.Format("\t\t\t%04x:获取信息失败，错误%04x\n", nType[i], g_retInfo.wErrCode);
		}

		strParams += strTemp;
	}
	strParams += "\n";
	((CCSTesterDlg * )AfxGetMainWnd())->AppendText(strParams);
}
