#include "StdAfx.h"
#include "TradeFileConvert.h"
#include "PubFuncs.h"
#include "GSFile.h"

CTradeFileConvert::CTradeFileConvert(void)
{
}

CTradeFileConvert::~CTradeFileConvert(void)
{
}

bool CTradeFileConvert::FileConvert(char * pszSrcFielName, char * pszSrcFilePath, char * pszTargetFolder)
{
	bool ret = false;
	char szTargetPath[MAX_PATH] = {0};

	char szStation[5]	= {0};
	char szDate[9]		= {0};
	BYTE bDate[4]		= {0};
	BYTE bTrade[512]	= {0};
	UINT size_trade		= 0;
	UINT nRecordCount	= 0;
	WORD nFileNo		= 0;

	CFile flSrc, flTarg;

	if (GetTradeInf(pszSrcFilePath, szStation, &size_trade))
	{
		nRecordCount = GetTradeCount(pszSrcFilePath, size_trade);
		memcpy(szDate, pszSrcFielName, 8);
		String2HexSeq(szDate, 8, bDate, 4);

		GetTargetFileName(szDate, pszTargetFolder, szStation, nFileNo, szTargetPath);

		if (flSrc.Open(pszSrcFilePath, CFile::modeRead))
		{
			if (flTarg.Open(szTargetPath, CFile::modeCreate | CFile::modeWrite))
			{
				flTarg.Write(szStation, 4);

				flTarg.Write(bDate, 4);
				flTarg.Write("\x00\x00\x00", 3);
				flTarg.Write(bDate, 4);

				flTarg.Write("\x23\x59\x59", 3);

				flTarg.Write(&nFileNo, 2);

				flTarg.Write(&nRecordCount, sizeof(nRecordCount));
				flTarg.Write("\r\n\r\n\r\n", 6);

				while(flSrc.Read(bTrade, (UINT)(size_trade)) > 0)
				{
					flTarg.Write(bTrade, (UINT)(size_trade));
					flTarg.Write("\r\n\r\n\r\n", 6);
				}

				flTarg.Close();

				ret = AddCrc32ToFile(szTargetPath);
			}

			flSrc.Close();
		}
	}

	return ret;
}

bool CTradeFileConvert::GetTradeInf(char * pszSrcFilePath, char * pStation, UINT * p_size_trade)
{
	bool ret				= true;

	SJTSALE sjt_sale		= {0};
	OTHERSALE svt_sale		= {0};
	ENTRYGATE entry_gate	= {0};
	PURSETRADE purse_trade	= {0};
	TICKETDEFER tk_deffer	= {0};
	TICKETUPDATE tk_update	= {0};
	DIRECTREFUND dt_refund	= {0};
	TICKETLOCK tk_lock		= {0};

	UINT size_trade			= 0;
	char cTradeType[2]		= {0};
	BYTE bTradeType			= 0;

	CFile fl;
	if (fl.Open(pszSrcFilePath, CFile::modeRead))
	{
		fl.Read(cTradeType, 2);
		String2Hex(cTradeType, 2, bTradeType);

		fl.SeekToBegin();
		switch (bTradeType)
		{
		case 0x50:
			size_trade = sizeof(sjt_sale);
			fl.Read(&sjt_sale, size_trade);
			memcpy(pStation, sjt_sale.cStationID, 4);
			break;
		case 0x51:
			size_trade = sizeof(svt_sale);
			fl.Read(&svt_sale, size_trade);
			memcpy(pStation, svt_sale.cStationID, 4);
			break;
		case 0x53:
			size_trade = sizeof(entry_gate);
			fl.Read(&entry_gate, size_trade);
			memcpy(pStation, entry_gate.cStationID, 4);
			break;
		case 0x54:
			size_trade = sizeof(purse_trade);
			fl.Read(&purse_trade, size_trade);
			memcpy(pStation, purse_trade.cStationID, 4);
			break;
		case 0x55:
			size_trade = sizeof(tk_deffer);
			fl.Read(&tk_deffer, size_trade);
			memcpy(pStation, tk_deffer.cStationID, 4);
			break;
		case 0x56:
			size_trade = sizeof(tk_update);
			fl.Read(&tk_update, size_trade);
			memcpy(pStation, tk_update.cStationID, 4);
			break;
		case 0x57:
			size_trade = sizeof(dt_refund);
			fl.Read(&dt_refund, size_trade);
			memcpy(pStation, dt_refund.cStationID, 4);
			break;
		case 0x59:
			size_trade = sizeof(tk_lock);
			fl.Read(&tk_lock, size_trade);
			memcpy(pStation, tk_lock.cStationID, 4);
			break;
		default:
			ret = false;
		}

		fl.Close();
	}
	else
	{
		ret = false;
	}

	*p_size_trade = size_trade;

	return ret;
}

UINT CTradeFileConvert::GetTradeCount(char * pszSrcFilePath, UINT size_trade)
{
	UINT ret = 0;

	CFile fl;
	if (fl.Open(pszSrcFilePath, CFile::modeRead))
	{
		ret = (UINT)(fl.GetLength() / size_trade);
		fl.Close();
	}

	return ret;
}

bool CTradeFileConvert::AddCrc32ToFile(char * pszSrcFilePath)
{
	BYTE bRead[1024]	= {0};
	DWORD dwCrcInit		= 0xFFFFFFFF;
	char szCrc[32]		= {0};
	UINT nRead			= 0;

	FILE * fp = fopen(pszSrcFilePath, "r+b");
	if (fp != NULL)
	{
		do 
		{
			nRead = (UINT)fread(bRead, 1, 1024, fp);
			dwCrcInit = Crc32Reverse(dwCrcInit, bRead, nRead);

		} while (!feof(fp));

		sprintf(szCrc, "CRC:%08X", ~dwCrcInit);

		fseek(fp, 0, SEEK_END);
		fwrite(szCrc, 1, 12, fp);

		fclose(fp);

		return true;
	}

	return false;
}

void CTradeFileConvert::GetTargetFileName(char * pszDate, char * pszTargFolder, char * pszStation, WORD& nFileNo, char * pszTargFilePathName)
{
	int nFileExit = -1;
	do 
	{
		nFileNo++;
		sprintf(pszTargFilePathName, "%sTRX%s.%s.%03d", pszTargFolder, pszStation, pszDate, nFileNo);
		nFileExit = _access(pszTargFilePathName, 0);

	} while (nFileExit == 0);
}
