// AutomaticAgm.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAutomaticAgmApp:
// �йش����ʵ�֣������ AutomaticAgm.cpp
//

class CAutomaticAgmApp : public CWinApp
{
public:
	CAutomaticAgmApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAutomaticAgmApp theApp;