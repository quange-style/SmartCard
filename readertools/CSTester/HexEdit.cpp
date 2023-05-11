// HexEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "CSTester.h"
#include "HexEdit.h"


// CHexEdit

IMPLEMENT_DYNAMIC(CHexEdit, CEdit)

CHexEdit::CHexEdit()
{

}

CHexEdit::~CHexEdit()
{
}


BEGIN_MESSAGE_MAP(CHexEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()



// CHexEdit 消息处理程序



void CHexEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString ss;
	char ch = char(nChar);

	if (ch >= 'a' && ch <= 'f')
	{
		ch = ch + ('A' - 'a');
	}	
	else if (ch >= 'A' && ch <= 'F')
	{
		;
	}
	else if (ch >= '0' && ch <= '9')
	{
		;
	}
	else if (ch != '\b')
	{
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
