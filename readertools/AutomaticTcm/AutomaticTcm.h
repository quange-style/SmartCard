// AutomaticTcm.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAutomaticTcmApp:
// �йش����ʵ�֣������ AutomaticTcm.cpp
//

class CAutomaticTcmApp : public CWinApp
{
public:
	CAutomaticTcmApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAutomaticTcmApp theApp;