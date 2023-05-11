#pragma once

class CTradeFileConvert
{
public:
	CTradeFileConvert(void);
	~CTradeFileConvert(void);

	static bool FileConvert(char * pszSrcFielName, char * pszSrcFilePath, char * pszTargetFolder);

private:
	static bool GetTradeInf(char * pszSrcFilePath, char * pStation, UINT * p_size_trade);

	static UINT GetTradeCount(char * pszSrcFilePath, UINT size_trade);

	static bool AddCrc32ToFile(char * pszSrcFilePath);

	static void GetTargetFileName(char * pszDate, char * pszTargFolder, char * pszStation, WORD& nFileNo, char * pszTargFilePathName);
};
