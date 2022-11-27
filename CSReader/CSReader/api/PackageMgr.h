#pragma once
#include "QFile.h"
#include <vector>
#include <stdint.h>

using namespace std;

typedef struct
{
	char sFileName[_POSIX_PATH_MAX];
	char sPathName[_POSIX_PATH_MAX];
	uint32_t dwFileSize;
	uint32_t dwFileOff;

}PKG_FILE_INF;

typedef vector<PKG_FILE_INF> VTR_LIST;
class PackageMgr
{
public:
	PackageMgr(void);
	~PackageMgr(void);

	static uint16_t UnpackageFiles(const char * pszPackagePath, const char * pszFileSaveFolder);

	static uint8_t ExcuteShell(const char * psz_scipt_path);

private:
	static bool WatermarkValid(const char * pszPackagePath);

};
