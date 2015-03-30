// TestVPRCtrlDlg.cpp : ʵ���ļ�
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



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CTestVPRCtrlDlg �Ի���



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


// CTestVPRCtrlDlg ��Ϣ�������

BOOL CTestVPRCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��\������...\���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	m_strMainDir = "c:\\VprResult\\";
	MakeSureDirectoryPathExists(m_strMainDir.GetBuffer());
	m_strMainDir.ReleaseBuffer();
	GetDlgItem(IDC_COMPARE_COUNT)->SetWindowText("3");
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestVPRCtrlDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CTestVPRCtrlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//����
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
		AfxMessageBox("���ӳɹ�");

	}
	else
	{
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		AfxMessageBox("����ʧ��");
	}

	UpdateData(FALSE);
}
	 


//�Ͽ�
void CTestVPRCtrlDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	if (VPR_Quit(12))
	{
		AfxMessageBox("�˳��ɹ�");
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	} 
	else
	{
		AfxMessageBox("�˳�ʧ��");
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
	//��ֵͼ�ͳ��ƺ�������һ��Ŀ¼��
	if(piByteBinImagLen)                                     //�����ֵСͼ
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
	//�ѳ�����ͼ����ָ���Ŀؼ���
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

//����׽��ͼ��

void CTestVPRCtrlDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here

	int state = VPR_CaptureEx(12);
	switch(state)
	{
	case 0:
		AfxMessageBox("��ץ�ĳ���ͼ�񲢳ɹ�ʶ���ƣ���ǰ���ƻ������Ѹ���!");
		break;
	case 1:
		AfxMessageBox("��ץ�ĳ���ͼ��ʶ����ʧ�ܣ���ǰ���ƻ������г��ƺ�Ϊ\"�޳���\"");
		break;
	case 2:
		AfxMessageBox("��������(��ͨѶ����)");
		break;
	default:
		break;
	}	

}

//������׽��ͼ��
void CTestVPRCtrlDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here

	if (VPR_Capture(12))
	{
		AfxMessageBox("�ѽ�������");
	} 
	else
	{
		AfxMessageBox("�й��Ϸ���");
	}
}

//���״̬
void CTestVPRCtrlDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here

	char buff[32] = {0};
	int state=VPR_CheckStatus(12, buff);   //���״̬
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
		AfxMessageBox("δ֪����");


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
	{                                                                 //��ʾͼƬ
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
		AfxMessageBox("������Ƚϳ���");
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
		AfxMessageBox("��ȫƥ�䣨ƥ���� = 100%��");
		break;
	case 1:
		AfxMessageBox("ƥ�䣨85%�� ƥ���� ��100%��");
		break;
	case 2:
		AfxMessageBox("����ƥ�䣨70%��ƥ���� ��85%��");
		break;
	case 3:
		AfxMessageBox("��ƥ�䣨ƥ���� ��70%��");
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
		AfxMessageBox("������Ƚ��ļ���");
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
		AfxMessageBox("��������ȷ�ļ���");
		return;
	}
	BOOL fResult = VPR_ComparePlateBin(pbBinImg1, pbBinImg2);
	if (fResult)
	{
		AfxMessageBox("ƥ��");
	}
	else
	{
		AfxMessageBox("��ƥ��");
	}
	delete pbBinImg1;
	delete pbBinImg2;
}
