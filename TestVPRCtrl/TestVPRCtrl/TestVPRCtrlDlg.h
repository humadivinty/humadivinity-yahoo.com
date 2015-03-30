// TestVPRCtrlDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

#define  WM_RECEIVE (WM_USER+100)

// CTestVPRCtrlDlg �Ի���
class CTestVPRCtrlDlg : public CDialog
{
// ����
public:
	CTestVPRCtrlDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTVPRCTRL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	LRESULT OnReceive(WPARAM uid, LPARAM param);

	// ���ɵ���Ϣӳ�亯��
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
	// //��ǰ����Ŀ
	int m_icurcount;
	afx_msg void OnBnClickedButton6();
	// Ip��ַ
	CString m_Ip;
	CString m_strMainDir;
	afx_msg void OnBnClickedButtonCompare();
	CButton m_chkFull;
	afx_msg void OnBnClickedButtonCompare2();
	CEdit m_cetCount;
};
