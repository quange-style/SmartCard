// CSTester.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCSTesterApp:
// �йش����ʵ�֣������ CSTester.cpp
//

class CCSTesterApp : public CWinApp
{
public:
	CCSTesterApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCSTesterApp theApp;