// TestVPRCtrl.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CTestVPRCtrlApp:
// �йش����ʵ�֣������ TestVPRCtrl.cpp
//

class CTestVPRCtrlApp : public CWinApp
{
public:
	CTestVPRCtrlApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestVPRCtrlApp theApp;
