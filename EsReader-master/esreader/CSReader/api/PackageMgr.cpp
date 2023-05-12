#include "PackageMgr.h"
#include "DataSecurity.h"
#include "QFile.h"
#include "Errors.h"
#include <string.h>
#include <sys/stat.h>
#include<unistd.h>
#include <stdlib.h>
#include "Records.h"

PackageMgr::PackageMgr(void)
{
}

PackageMgr::~PackageMgr(void)
{
}

uint16_t PackageMgr::UnpackageFiles(const char * pszPackagePath, const char * pszFileSaveFolder)
{
	uint16_t ret = 0;
	FILE * fp = NULL;
	uint8_t FileNum = 0;
	uint8_t i, lenFileName;
	VTR_LIST fileList;
	PKG_FILE_INF inf;

	do
	{
		if (access(pszPackagePath, 0) != 0)
		{
			ret = ERR_PARAM_NOT_EXIST;
			break;
		}

		if (!WatermarkValid(pszPackagePath))
		{
			ret = ERR_PARAM_INVALID;
			break;
		}

		fp = fopen(pszPackagePath, "rb");
		if (fp == NULL)
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		fread(&FileNum, sizeof(FileNum), 1, fp);
		for (i=0;i<FileNum;i++)
		{
			memset(&inf, 0, sizeof(inf));
			fread(&lenFileName, sizeof(lenFileName), 1, fp);
			fread(inf.sFileName, lenFileName, 1, fp);
			fread(&inf.dwFileSize, sizeof(inf.dwFileSize), 1, fp);
			fread(&inf.dwFileOff, sizeof(inf.dwFileOff), 1, fp);
			sprintf(inf.sPathName, "%s/%s", pszFileSaveFolder, inf.sFileName);
			fileList.push_back(inf);
		}

		FILE * fpsub = NULL;
		uint8_t * pFileBuf = NULL;
		for (i=0;i<FileNum;i++)
		{
			inf = fileList[i];

			fpsub = fopen(inf.sPathName, "wb");
			if (fpsub == NULL)
			{
				ret = ERR_FILE_ACCESS;
				break;
			}

			pFileBuf = new(std::nothrow) uint8_t[inf.dwFileSize];
			if (pFileBuf == NULL)
			{
				fclose(fp);
				ret = ERR_UNDEFINED;
				break;
			}
			memset(pFileBuf, 0, inf.dwFileSize);

			fseek(fp, inf.dwFileOff, SEEK_SET);
			fread(pFileBuf, inf.dwFileSize, 1, fp);
			if (fwrite(pFileBuf, 1, inf.dwFileSize, fpsub) != inf.dwFileSize)
			{
				fclose(fp);
				delete []pFileBuf;
				ret = ERR_FILE_ACCESS;
				break;
			}

			fclose(fpsub);
			delete []pFileBuf;

		}

	} while (0);

	if (fp != NULL)
		fclose(fp);

	g_Record.log_out(ret, level_invalid, "UnpackageFiles(%s,%s)=%04x", pszPackagePath, pszFileSaveFolder, ret);

	return ret;
}

//bool PackageMgr::WatermarkValid(const char * pszPackagePath)
//{
//	uint32_t dwCrcCal		= 0xffffffff;
//	uint32_t dwCrcFile		= 0x00000000;
//	const uint32_t readOnce	= 2048;
//	uint8_t bRead[readOnce]	= {0};
//	uint32_t actualRead		= 0;
//	bool b_end_file			= false;
//
//	static const char * pWaterMark = "GDMH-HUNAN.CS.METRO READER #@2013";
//
//	dwCrcCal = DataSecurity::Crc32Reverse(dwCrcCal, (uint8_t *)pWaterMark, (uint32_t)strlen(pWaterMark));
//
//	if (access(pszPackagePath, 0) == 0)
//	{
//		FILE * fp = fopen(pszPackagePath, "rb");
//		if (fp == NULL)
//			return false;
//
//		while (!b_end_file)
//		{
//			actualRead = (uint32_t)fread(bRead, 1, readOnce, fp);
//
//			if (feof(fp) > 0)
//			{
//				memcpy(&dwCrcFile, bRead + actualRead - 4, 4);
//				dwCrcCal = DataSecurity::Crc32Reverse(dwCrcCal, bRead, actualRead - 4);
//				b_end_file = true;
//				dwCrcFile = ~dwCrcFile;
//			}
//			else
//			{
//				dwCrcCal = DataSecurity::Crc32Reverse(dwCrcCal, bRead, actualRead);
//			}
//		}
//
//		fclose(fp);
//
//		return (bool)(dwCrcCal == dwCrcFile);
//	}
//
//	return false;
//}
bool PackageMgr::WatermarkValid(const char * pszPackagePath)
{
#define MAX_READ	2048


	uint32_t dwCrcCal			= 0xffffffff;
	uint32_t dwCrcFile			= 0x00000000;
	uint32_t readOnce			= 0;
	uint8_t bRead[MAX_READ]		= {0};
	uint32_t actualRead			= 0;
	long file_size				= 0;
	FILE * fp					= NULL;

	do
	{
		static const char * pWaterMark = "GDMH-HUNAN.CS.METRO READER #@2013";
		dwCrcCal = DataSecurity::Crc32Reverse(dwCrcCal, (uint8_t *)pWaterMark, (uint32_t)strlen(pWaterMark));

		if (access(pszPackagePath, 0) != 0)
			break;

		file_size = QFile::length(pszPackagePath);
		if (file_size < 4)
			break;

		file_size -= 4;

		fp = fopen(pszPackagePath, "rb");
		if (fp == NULL)
			break;

		while (file_size > 0)
		{
			readOnce = file_size < MAX_READ ? file_size : MAX_READ;

			actualRead = (uint32_t)fread(bRead, 1, readOnce, fp);
			dwCrcCal = DataSecurity::Crc32Reverse(dwCrcCal, bRead, actualRead);
			file_size -= readOnce;
		}

		// 读出校验码
		actualRead = (uint32_t)fread(bRead, 1, MAX_READ, fp);
		memcpy(&dwCrcFile, bRead, 4);
		dwCrcFile = ~dwCrcFile;

	} while (0);

	if (fp != NULL)
		fclose(fp);

	return (bool)(dwCrcCal == dwCrcFile);
}

bool PackageMgr::ExcuteShell(const char * psz_scipt_path)
{
	bool ret = false;
	char sys_cmd[512] = {0};

	do
	{

		sprintf(sys_cmd, "chmod 777 %s", psz_scipt_path);
		if (system(sys_cmd) != 0)	break;

		sprintf(sys_cmd, "sh %s", psz_scipt_path);
		if (system(sys_cmd) != 0)	break;

		ret = true;

	} while (0);

	if (!ret)
		g_Record.log_out(0, level_error, "ExcuteShell(%s)=false", psz_scipt_path);

	return ret;
}
