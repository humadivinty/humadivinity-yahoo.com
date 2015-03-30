// TestVPRCtrlDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#define  WM_RECEIVE (WM_USER+100)

// CTestVPRCtrlDlg 对话框
class CTestVPRCtrlDlg : public CDialog
{
// 构造
public:
	CTestVPRCtrlDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTVPRCTRL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	LRESULT OnReceive(WPARAM uid, LPARAM param);

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	CString m_plate;
	bool DrawPic(BYTE *pData, int iLen, CWnd *pdraw);
	// //当前个数目
	int m_icurcount;
	afx_msg void OnBnClickedButton6();
	// Ip地址
	CString m_Ip;
	CString m_strMainDir;
	afx_msg void OnBnClickedButtonCompare();
	CButton m_chkFull;
	afx_msg void OnBnClickedButtonCompare2();
	CEdit m_cetCount;
};
