// TestVPRCtrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestVPRCtrl.h"
#include "TestVPRCtrlDlg.h"
#include ".\testvprctrldlg.h"
#include "VPRCtrl.h"
#include "Dbghelp.h"

#ifdef DEBUG
#pragma  comment(lib, "../../Debug/VPRCtrl.lib")
#else
#pragma  comment(lib, "../../Release/VPRCtrl.lib")
#endif

#pragma  comment(lib, "HVDLL.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  BINIMGSIZE 280
#define  JPGEIMGSIZE (2*1024*1024)



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTestVPRCtrlDlg 对话框



CTestVPRCtrlDlg::CTestVPRCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestVPRCtrlDlg::IDD, pParent)
	, m_plate(_T(""))
	, m_icurcount(0)
	, m_Ip(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestVPRCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_PLATE, m_plate);
	DDX_Text(pDX, IDC_EDIT_IP, m_Ip);
	DDX_Control(pDX, IDC_CHECK_FULL, m_chkFull);
	DDX_Control(pDX, IDC_COMPARE_COUNT, m_cetCount);
}

BEGIN_MESSAGE_MAP(CTestVPRCtrlDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_MESSAGE(WM_RECEIVE, OnReceive)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON_COMPARE, OnBnClickedButtonCompare)
	ON_BN_CLICKED(IDC_BUTTON_COMPARE2, OnBnClickedButtonCompare2)
END_MESSAGE_MAP()


// CTestVPRCtrlDlg 消息处理程序

BOOL CTestVPRCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_strMainDir = "c:\\VprResult\\";
	MakeSureDirectoryPathExists(m_strMainDir.GetBuffer());
	m_strMainDir.ReleaseBuffer();
	GetDlgItem(IDC_COMPARE_COUNT)->SetWindowText("3");
	// TODO: 在此添加额外的初始化代码
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CTestVPRCtrlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestVPRCtrlDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CTestVPRCtrlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//连接
void CTestVPRCtrlDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	char ip[256] = {0};

	strncpy(ip, m_Ip.GetBuffer(0), (size_t)(m_Ip.GetLength()));
	m_Ip.ReleaseBuffer();

	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	if (VPR_Init(12,ip,GetSafeHwnd(),WM_RECEIVE, FALSE))
	{		
		AfxMessageBox("连接成功");

	}
	else
	{
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		AfxMessageBox("连接失败");
	}

	UpdateData(FALSE);
}
	 


//断开
void CTestVPRCtrlDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	if (VPR_Quit(12))
	{
		AfxMessageBox("退出成功");
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	} 
	else
	{
		AfxMessageBox("退出失败");
	}
}

LRESULT CTestVPRCtrlDlg::OnReceive(WPARAM uid, LPARAM param)
{
	BYTE *pbBytebinImage, *pbBytejpegImage;
	int piByteBinImagLen, piJpegImagelen;
	char pchPlate[50]={0};
	pbBytebinImage = new BYTE[BINIMGSIZE];
	pbBytejpegImage = new BYTE[JPGEIMGSIZE];	
	UINT speed = 0;;
	DWORD64 time4 = 0;
	piByteBinImagLen = 0;
	piJpegImagelen = 0;

	//BOOL fRet = VPR_GetVehicleInfo(12, pchPlate, &piByteBinImagLen, pbBytebinImage, &piJpegImagelen, pbBytejpegImage);
	//VPR_GetVehicleInfoEx(12, pchPlate, &piByteBinImagLen, pbBytebinImage, &piJpegImagelen, pbBytejpegImage, &speed);
	BOOL fRet= VPR_GetVehicleInfoEx2(12, pchPlate, &piByteBinImagLen, pbBytebinImage, &piJpegImagelen, pbBytejpegImage, &speed, &time4);
	if (fRet == FALSE)
	{
		if (pbBytebinImage)
		{
			delete[] pbBytebinImage;
			pbBytebinImage = NULL;
		}
		if (pbBytejpegImage)
		{
			delete[] pbBytejpegImage;
			pbBytejpegImage = NULL;	
		}
		return 0;
	}
	m_icurcount+=1;
	
	CTime ctCurTime(time4 / 1000);
	DWORD64 iMS = time4 % 1000;
	if (time4 == 0 || strlen(pchPlate) == 0)
	{
		char szLog[1024] = {0};
		sprintf(szLog, "index=%d,plate=%s,time=%d,speed=%d,binlen=%d,biglen=%d\r\n", m_icurcount, pchPlate,time4,speed,piByteBinImagLen,piJpegImagelen);
		OutputDebugString(szLog);
	}
	//二值图和车牌号码存放在一个目录上
	if(piByteBinImagLen)                                     //保存二值小图
	{
		CString  m_dirbin;
		 m_dirbin.Format("%s%d\\%s\\%02d\\%s%03d%s_%d",m_strMainDir,12,ctCurTime.Format("%Y%m%d"),ctCurTime.GetHour(),ctCurTime.Format("%Y%m%d%H%M%S"),iMS,pchPlate, m_icurcount);
		MakeSureDirectoryPathExists(m_dirbin.GetBuffer());
		m_dirbin.ReleaseBuffer();
		CString file = m_dirbin  + ".bin";
		FILE *PSmallPic = fopen(file, "wb" );
		if( PSmallPic && pbBytebinImage)
		{
			fwrite(pbBytebinImage, sizeof(BYTE), piByteBinImagLen, PSmallPic);
			fclose(PSmallPic);
		}
		
		file = m_dirbin  + ".bmp";
		VPR_BinImage2BMP(112,20,pbBytebinImage,file.GetBuffer());
		file.ReleaseBuffer();		
		
		file = m_dirbin + ".txt";
		FILE *platefile =  fopen(file,"wb");
		if (platefile)
		{
			fwrite(pchPlate, sizeof(CHAR), 50, platefile);
			fclose(platefile);
		}

	}

	if (piJpegImagelen)
	{
		CString  m_dirbin;
		m_dirbin.Format("%s%d\\%s\\%02d\\%s%03d%s_%d",m_strMainDir,12,ctCurTime.Format("%Y%m%d"),ctCurTime.GetHour(),ctCurTime.Format("%Y%m%d%H%M%S"),iMS,pchPlate, m_icurcount);
		MakeSureDirectoryPathExists(m_dirbin.GetBuffer());
		m_dirbin.ReleaseBuffer();
		CString file = m_dirbin +".jpg";
		FILE *PSmallPic = fopen(file, "wb" );
		if( PSmallPic && pbBytebinImage)
		{
			fwrite(pbBytejpegImage, sizeof(BYTE),piJpegImagelen, PSmallPic);
			fclose(PSmallPic);
		}
		

	}
	//把车辆大图画到指定的控件上
	DrawPic(pbBytejpegImage, piJpegImagelen, GetDlgItem(IDC_STATIC_BIGIMG));
	CString plate(pchPlate);
	m_plate = plate;
	UpdateData(FALSE);
	CString strCount;
	strCount.Format("%d", m_icurcount);
	GetDlgItem(IDC_STATIC_COUNT)->SetWindowText(strCount);

	if (pbBytebinImage)
	{
		delete[] pbBytebinImage;
		pbBytebinImage = NULL;
	}
	if (pbBytejpegImage)
	{
		delete[] pbBytejpegImage;
		pbBytejpegImage = NULL;	
	}
	return 1;

}

//阻塞捉拍图象

void CTestVPRCtrlDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here

	int state = VPR_CaptureEx(12);
	switch(state)
	{
	case 0:
		AfxMessageBox("已抓拍车辆图像并成功识别车牌，当前车牌缓冲区已更新!");
		break;
	case 1:
		AfxMessageBox("已抓拍车辆图像但识别车牌失败，当前车牌缓冲区中车牌号为\"无车牌\"");
		break;
	case 2:
		AfxMessageBox("其它故障(如通讯故障)");
		break;
	default:
		break;
	}	

}

//非阻塞捉拍图象
void CTestVPRCtrlDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here

	if (VPR_Capture(12))
	{
		AfxMessageBox("已接收命令");
	} 
	else
	{
		AfxMessageBox("有故障发生");
	}
}

//检测状态
void CTestVPRCtrlDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here

	char buff[32] = {0};
	int state=VPR_CheckStatus(12, buff);   //检查状态
	CString msg(buff);
	if (state == 0 && msg.GetLength() > 0)
	{
		AfxMessageBox(msg);
	}
	else if(state == 4 && msg.GetLength() > 0)
	{
		AfxMessageBox(msg);
	}
	else
		AfxMessageBox("未知错误");


}


bool CTestVPRCtrlDlg::DrawPic(BYTE *pData, int iLen, CWnd *pdraw)
{

	if( !pData || !pdraw || iLen < 1 )
	{
		return false;
	}
	ULONG iWritten = NULL;
	CDC *pDC = pdraw->GetDC(); 
	RECT rect;	   
	IStream *pStm = NULL;
	CreateStreamOnHGlobal( NULL, TRUE, &pStm );
	if( !pStm )
	{
		ReleaseDC(pDC);
		return false;
	}
	pdraw->GetClientRect( &rect );
	CPictureHolder picholder;  
	LARGE_INTEGER liTempstar ={0};
	pStm->Seek( liTempstar, STREAM_SEEK_SET, NULL );
	pStm->Write(pData, iLen, &iWritten);
	pStm->Seek( liTempstar, STREAM_SEEK_SET, NULL );
	if( SUCCEEDED(OleLoadPicture(pStm, iLen, TRUE, IID_IPicture, (void **)&picholder))) 
	{                                                                 //显示图片
		picholder.Render(pDC, rect, CRect( 0, 0, 0, 0 ) );
	}
	ReleaseDC(pDC);
	pStm->Release();
	return true;
}

void CTestVPRCtrlDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	VPR_SyncTime(12);
	
}

void CTestVPRCtrlDlg::OnBnClickedButtonCompare()
{
	// TODO: Add your control notification handler code here
	CString strPlate1,strPlate2;
	GetDlgItem(IDC_PLATE1)->GetWindowText(strPlate1);
	GetDlgItem(IDC_PLATE2)->GetWindowText(strPlate2);
	if (strPlate1.IsEmpty() || strPlate2.IsEmpty())
	{
		AfxMessageBox("请输入比较车牌");
		return;
	}
	int iCmpType = m_chkFull.GetCheck();
	CString strCmpCount;
	GetDlgItem(IDC_COMPARE_COUNT)->GetWindowText(strCmpCount);
	int iCmpCount = atoi(strCmpCount); 
	if (iCmpCount < 3 || iCmpCount > 7)
	{
		iCmpCount = 3;
		GetDlgItem(IDC_COMPARE_COUNT)->SetWindowText("3");
	}
	int iCmpResult = VPR_ComparePlate(iCmpType, iCmpCount, strPlate1.GetBuffer(), strPlate2.GetBuffer());
	strPlate1.ReleaseBuffer();
	strPlate2.ReleaseBuffer();
	switch(iCmpResult)
	{
	case 0:
		AfxMessageBox("完全匹配（匹配率 = 100%）");
		break;
	case 1:
		AfxMessageBox("匹配（85%≤ 匹配率 ＜100%）");
		break;
	case 2:
		AfxMessageBox("基本匹配（70%≤匹配率 ＜85%）");
		break;
	case 3:
		AfxMessageBox("不匹配（匹配率 ＜70%）");
		break;
	}
}

void CTestVPRCtrlDlg::OnBnClickedButtonCompare2()
{
	// TODO: Add your control notification handler code here
	CString strBinFile1,strBinFile2;
	GetDlgItem(IDC_BIN_FILE1)->GetWindowText(strBinFile1);
	GetDlgItem(IDC_BIN_FILE2)->GetWindowText(strBinFile2);
	if (strBinFile1.IsEmpty() || strBinFile2.IsEmpty())
	{
		AfxMessageBox("请输入比较文件名");
		return;
	}
	BYTE* pbBinImg1 = new BYTE[1024];
	BYTE* pbBinImg2 = new BYTE[1024];
	FILE* pFile1 = fopen(strBinFile1, "rb");
	size_t iLen1 = 0;
	if (pFile1)
	{
		iLen1 = fread(pbBinImg1, 1, 1024, pFile1);
		fclose(pFile1);
	}
	FILE* pFile2 = fopen(strBinFile2, "rb");
	size_t iLen2 = 0;
	if (pFile1)
	{
		iLen2 = fread(pbBinImg2, 1, 1024, pFile2);
		fclose(pFile2);
	}
	if (iLen1 <= 0 || iLen1 > 500 || iLen2 <= 0 || iLen2 > 500)
	{
	
		delete pbBinImg1;
		delete pbBinImg2;
		AfxMessageBox("请输入正确文件名");
		return;
	}
	BOOL fResult = VPR_ComparePlateBin(pbBinImg1, pbBinImg2);
	if (fResult)
	{
		AfxMessageBox("匹配");
	}
	else
	{
		AfxMessageBox("不匹配");
	}
	delete pbBinImg1;
	delete pbBinImg2;
}
