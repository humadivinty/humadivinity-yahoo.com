// VPRCtrl.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "VPRCtrl.h"


#include "hv_api_multi.h"
#include <stdio.h>
#include <time.h>
#include <atltrace.h>
#include "atltime.h"
#include "atlstr.h"
#include "Dbghelp.h"




#include <shlwapi.h>

CRITICAL_SECTION g_csWriteLog;
CRITICAL_SECTION g_csSync;

int global_WriteLog( char* chText );

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

static CRITICAL_SECTION g_csVPRCtrlSync;





BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		InitializeCriticalSection( &g_csVPRCtrlSync );
		InitializeCriticalSection(&g_csWriteLog);
		InitializeCriticalSection( &g_csSync );
		break;
	//case DLL_THREAD_ATTACH:
	//case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		DeleteCriticalSection( &g_csVPRCtrlSync );
		DeleteCriticalSection(&g_csWriteLog);
		DeleteCriticalSection(&g_csSync);
		break;
	}
    return TRUE;
}

int global_WriteLog( char* chText )
{
	if(NULL == chText)
		return 0;

	//取得当前的精确毫秒的时间
	static time_t starttime = time(NULL);
	static DWORD starttick = GetTickCount(); 
	DWORD dwNowTick = GetTickCount() - starttick;
	time_t nowtime = starttime + (time_t)(dwNowTick / 1000);
	struct tm *pTM = localtime(&nowtime);
	DWORD dwMS = dwNowTick % 1000;

	TCHAR szFileName[ MAX_PATH] = {0};
	GetModuleFileName(NULL, szFileName, MAX_PATH);	//取得包括程序名的全路径
	PathRemoveFileSpec(szFileName);				//去掉程序名
	CString strCurrentDir = szFileName;
	CString fileName = _T("");
	strCurrentDir.AppendFormat("\\LOG\\%04d-%02d-%02d\\CamerLog\\", pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday);
	MakeSureDirectoryPathExists(strCurrentDir);
	fileName.Format("%s%04d-%02d-%02d-%02d-Camera.log", strCurrentDir, pTM->tm_year + 1900, pTM->tm_mon + 1,pTM->tm_mday , pTM->tm_hour);

	EnterCriticalSection(&g_csWriteLog);

	FILE *file = NULL;
	file = fopen(fileName, "a+");
	if (file)
	{
		fprintf(file,"%04d-%02d-%02d %02d:%02d:%02d:%03d : %s\n",  pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
			pTM->tm_hour, pTM->tm_min, pTM->tm_sec, dwMS, chText);
		fclose(file);
		file = NULL;
	}

	LeaveCriticalSection(&g_csWriteLog);

	return 0;
}


//二值图
const int BIN_BIT_COUNT = 8;
const int BIN_WIDTH = 112;
const int BIN_HEIGHT = 20;
const int BIN_STRIDE = BIN_WIDTH / BIN_BIT_COUNT;
const int BIN_BYTE_COUNT = BIN_HEIGHT * BIN_STRIDE;

//车牌大图
const int BIG_IMG_WIDTH = 768;
const int BIG_IMG_HEIGHT = 576;
// 高清车牌大图
const int BIG_IMG_WIDTH_HIGH = 2048;
const int BIG_IMG_HEIGHT_HIGH = 1536;
// 大图缓冲区大小
const int BIG_IMG_SIZE = 200 * 1024;
const int BIG_IMG_SIZE_HIGH = 1024 * 1024;

//接收车牌延迟时间
const int RECV_PLATE_TIMEOUT = 3000;


const int  MAX_MUILT_CONNECT_COUNT = 200 ;   //能连接的最大连接数目

//实现
class CVPRImpl
{
public:
	CVPRImpl();
	virtual ~CVPRImpl();

	// 初始化
	BOOL Init(UINT uID, char *pchPort, HWND hWndHandle, UINT uMsg, BOOL bProvideBigPic=TRUE);
	BOOL InitEx(UINT uID, char *pchPort, DWORD idThread, UINT uMsg, BOOL bProvideBigPic=TRUE);
	// 关闭车牌自动识别仪
	BOOL Quit();
	// 车牌自动识别仪手动抓拍图象并进行识别
	BOOL Capture();
	int CaptureEx();
	// 取车牌自动识别仪识别结果
	BOOL GetVehicleInfo(
		char * pchPlate,int* piByteBinImagLen,BYTE * pbByteBinImage,
		int * piJpegImageLen,BYTE * pbByteJpegImage);

	BOOL GetVehicleInfoEx(
		char * pchPlate,int* piByteBinImagLen,BYTE * pbByteBinImage,
		int * piJpegImageLen,BYTE * pbByteJpegImage, UINT *puSpeed);
	BOOL GetVehicleInfoEx2(char * pchPlate,int* piByteBinImagLen,BYTE * pbByteBinImage,int * piJpegImageLen,BYTE * pbByteJpegImage,UINT *puSpeed,
		DWORD64* pdwTimeMs);

	// 检查牌照识别器状态
	int CheckStatus(char * pchVprDevStatus);
	BOOL SyncTime();
	void WriteLog(char* pszLog);

	//读取配置文件
	void ReadConfigFile();
	
protected:
	//车牌回调函数
	static int OnPlateProxy( void *pFirstParameter, DWORD32 dwCarID, PSTR pcPlateNo, DWORD64 dw64TimeMs );
	//车牌二值化小图回调函数
	static int OnBinImageProxy( void *pFirstParameter, DWORD32 dwCarID, WORD wImageWidth, WORD wImageHigh,
		BYTE bType, WORD wSize,	BYTE *pbImage, DWORD64 dw64TimeMs );
	//车牌大图回调函数
	static int OnJpgImageProxy( void *pFirstParameter, 
		DWORD32 dwCarID, WORD wImageWidth, WORD wImageHigh, BYTE bType,
		WORD wSize,	PBYTE pbImage, WORD wImageID, WORD wHighImgFlag,
		WORD wPlateWidth, WORD wPlateHigh, DWORD64 dw64TimeMs );
	//车牌记录开始回调函数
	static int OnRecordBegin( void* pFirstParameter, DWORD32 dwCarID );
	//车牌记录结果回调函数
	static int OnRecordEnd( void* pFirstParameter, DWORD32 dwCarID );	
	

	//识别器句柄
	HV_HANDLE m_hHV;
	char m_szAddress[ MAX_PATH ];
	UINT m_uid;

	DWORD32 m_dwCarID;
	char m_szPlate[ 30 ];
	int m_iBinSize;
	BYTE m_rgbBinImage[ 280 ];
	int m_iJpgSize;
	BYTE* m_pbJpgImage;
	IStream* m_pstmBigImg;
	CLSID m_jpgClsid;
	bool m_fHasBigImg;

	bool m_fInfoReady;
	int m_iRecvPlateTimeout;
	CRITICAL_SECTION m_csLock;
	ULONG_PTR m_gdiplusToken;

	bool m_fInitEx;
	HWND m_hWndHandle;
	DWORD m_idThread;
	UINT m_uMsg;

	int m_iClipWidth;
	int m_iClipHeight;

	// 车牌附加信息
	char m_szAppendInfo[512];

public:

	bool m_fQuit;
	HANDLE m_hThread;
	DWORD m_dwThreadId;
	BOOL m_bProvideBigPic;
	//连接状态检测(用于自动重连)
	static DWORD WINAPI ThreadCheckStatus(LPVOID lpParameter);

	UINT GetId()
	{
		return m_uid;
	}
	// 设备数目 (新增的代码)
	int m_iBoxCount;
	// 一个结果是否开始
	bool m_begin;
	// 一个结果是否结束
	bool m_finished;

	char  *m_prgszIp[MAX_MUILT_CONNECT_COUNT];   //多连接IP地址


	// 设备状态
	bool m_DevStaus;
	//识别的时间
	 DWORD64 m_dw64TimeMs;
	 // //较时函数

//-------------------------字符叠加----------------------------
	 //叠加字符
	 bool m_bOverlayStringEnable;
	 char m_chOverlayText[1024];
	 CString m_strOverlayText;			//需要叠加的字符
	 DWORD m_dwFontColor;					//字体颜色
	 int m_iFontSize;
	 int m_iOverlayX;									//叠加字符左顶点的横坐标
	 int m_iOverlayY;									//叠加字符左顶点的纵坐标

	 void AnalysisOverlayText();
	 bool OverlayStringToBigImg(char* overLayString,PBYTE DestImg, int ImgLen, int* finalLen, int overLayX, int overLayY);


	 
};

//////////////////////////////////////////////////////////////////////
CVPRImpl::CVPRImpl()
	:m_hHV( NULL )
	,m_dwCarID( -1 )
	,m_iBinSize( 0 )
	,m_iJpgSize( 0 )
	,m_fInfoReady( false )
	,m_iRecvPlateTimeout(0)
	,m_fQuit(false)
	,m_hThread(NULL)
	,m_dwThreadId(0)
	,m_hWndHandle(NULL)
	,m_idThread(0)
	,m_uMsg(0)
	,m_fInitEx(false)
	,m_uid(0)
	,m_pstmBigImg(NULL)
	,m_fHasBigImg(false)
	,m_iBoxCount(0)
	,m_begin(false)
	,m_finished(true)
	, m_DevStaus(false)
	,m_pbJpgImage(NULL)
{
	m_szAddress[ 0 ] = 0;
	m_szAppendInfo[0] = 0;
	for (int i = 0; i< sizeof(m_chOverlayText); i++)
	{
		m_chOverlayText[i] = 0;
	}
	m_bOverlayStringEnable = true;
	ReadConfigFile();
	CreateStreamOnHGlobal(NULL, TRUE, &m_pstmBigImg);
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	GetEncoderClsid(L"image/jpeg", &m_jpgClsid);
	InitializeCriticalSection( &m_csLock );




	//新增代码,主要是读取配置文件
	
	m_dw64TimeMs = 0;

	for(int i = 0; i < MAX_MUILT_CONNECT_COUNT; ++i)
	{
		m_prgszIp[i] = NULL;
	}


}

CVPRImpl::~CVPRImpl()
{
	Quit();
	if( m_pstmBigImg != NULL )
	{
		m_pstmBigImg->Release();
		m_pstmBigImg = NULL;
	}
	if (m_pbJpgImage != NULL)
	{
		delete[] m_pbJpgImage;
		m_pbJpgImage = NULL;
	}
	GdiplusShutdown(m_gdiplusToken);
	DeleteCriticalSection( &m_csLock );
}
void CVPRImpl::WriteLog(char* pszLog)
{

	TCHAR tchFileName[MAX_PATH] = {0};
	GetModuleFileName(NULL, tchFileName, MAX_PATH);	//取得包括程序名的全路径
	PathRemoveFileSpec(tchFileName);				//去掉程序名
	CString strCurrentDir = tchFileName;

	char szIniName[256] = {0};
	sprintf(szIniName, "%s\\VprCtrl.ini", tchFileName);
	int iEnable = GetPrivateProfileInt("LogInf", "Enable", -1, szIniName);
	if (-1 == iEnable)
	{
		iEnable = 0;
		char szEnable[10];
		sprintf(szEnable, "%d", iEnable);
		WritePrivateProfileString("LogInf", "Enable", szEnable, szIniName);
	}
	if (iEnable == 0)
	{
		return;
	}



	CString strPath =  _T("log\\");
	CTime tm = CTime::GetCurrentTime();	
	CString szFileName;
	szFileName.Format("%s\\%s%s\\%s.log", tchFileName, strPath, tm.Format("%Y%m%d"), tm.Format("%H"));	
	MakeSureDirectoryPathExists(szFileName);
	FILE *myfile = fopen(szFileName, "a+");
	if (myfile)
	{
		//CTime tm = CTime::GetCurrentTime();
		char szTime[64] = {0};
		sprintf(szTime, "[%s] ", tm.Format("%Y-%m-%d %H:%M:%S"));
		fwrite(szTime, 1, strlen(szTime), myfile);
		fwrite(pszLog, 1, strlen(pszLog), myfile);
		fwrite("\r\n", 1, strlen("\r\n"), myfile);
		fclose(myfile);
	}
	return ;
}
//连接状态检测
DWORD WINAPI CVPRImpl::ThreadCheckStatus(LPVOID lpParameter)
{
	if (lpParameter == NULL)
	{
		return 0;
	}
	CVPRImpl *pThis = (CVPRImpl*)lpParameter;
	while (!pThis->m_fQuit)
	{
		HRESULT hr = E_FAIL;
		HV_CONNECTION_STATUS rgConnStatus[MAX_MUILT_CONNECT_COUNT];
		int iStatusCount;
		hr = MultiHvIsConnected(pThis->m_hHV, rgConnStatus, MAX_MUILT_CONNECT_COUNT, &iStatusCount);

		if (hr ==S_OK)
		{
			bool reconnect =false;
			for (int i=0; i<iStatusCount; i++)
			{
				if (rgConnStatus[i].m_iStatus !=0)
				{
					reconnect = true;
					pThis->m_DevStaus = false;
					break;
				}
			}

			if (!reconnect)
			{
				pThis->m_DevStaus = true;
			}
		}
		else
		{
			pThis->m_DevStaus = false;
		}
		Sleep(1000);

	}
	return 0;
}

////////////////////////////////////////////////////////////////////
//
//  函数名称 : VPR_Init(HWND hWndHandle,UINT uMsg)
//  函数功能 : 初始化车牌自动识别仪
//  参数说明 : hWndHandle----用于接收消息的窗体句柄
//			   uMsg----用户字定义消息，当接口库接收到车牌照识别器的识别结果时，通过这个消息发送给窗口（hWndHandle）
//  返回值   : TRUE，FALSE
//  备注     : 无
//
////////////////////////////////////////////////////////////////////
BOOL CVPRImpl::Init(UINT uID, char *pchPort, HWND hWndHandle, UINT uMsg, BOOL bProvideBigPic/*=TRUE*/)
{
	

	BOOL bSuccess=TRUE;
	m_hWndHandle = hWndHandle;
	m_idThread = 0;
	m_uMsg = uMsg;

	m_DevStaus = false;

	char address[260] = {0};
	strcpy(address, pchPort);

	strcpy(m_szAddress, pchPort);
	
	


//删除了一部分代码
	m_fInitEx = false;
	m_uid = uID;
	m_bProvideBigPic = bProvideBigPic;

	//  增加新代码

	//增加的代码
	if (m_hHV != NULL)
	{
		Quit();
	}


	char *token;
	token = strtok(address , "," );
	int i=-1;
	while( token != NULL )
	{

		++i;
		m_prgszIp[i] = new char[32];
		memset(m_prgszIp[i], 0, 32);
		strcpy(m_prgszIp[i], token);
		token = strtok( NULL, ",");
	}

	m_iBoxCount = i+1;

	for (i=0; i<m_iBoxCount; i++)
	{
		for (int j=0; j<32; j++)
		{
			if (m_prgszIp[i][j] ==':')
			{
				m_prgszIp[i][j] = '\0';
				break;
			}
		}
	}


	if (m_iBoxCount == 0)
	{
		bSuccess = FALSE;
		return  bSuccess;
	}

	HRESULT hr = OpenMultiHv( (char**)m_prgszIp, m_iBoxCount, &m_hHV );

	if(S_OK == hr && m_hHV != NULL)
	{

		if ( SetMultiHvCallBack( m_hHV, CVPRImpl::OnPlateProxy, this, 0, _TYPE_PLATE_STR ) != S_OK )
		{
			Quit();
			bSuccess = FALSE;
		}
		else
		{
			if ( SetMultiHvCallBack( m_hHV, CVPRImpl::OnBinImageProxy, this, 0, _TYPE_BINARY_IMAGE ) != S_OK )
			{
				Quit();
				bSuccess = FALSE;
			}
			else
			{
				if ( SetMultiHvCallBack( m_hHV, CVPRImpl::OnJpgImageProxy, this, 0, _TYPE_BIG_IMAGE ) != S_OK )
				{
					Quit();
					bSuccess = FALSE;
				}	
			}
		}

	}
	else
	{
		bSuccess = FALSE;
		m_DevStaus = false;
		return bSuccess;

	}


	Sleep(10000);
	hr = E_FAIL;
	HV_CONNECTION_STATUS rgConnStatus[MAX_MUILT_CONNECT_COUNT];
	int iStatusCount;
	hr = MultiHvIsConnected(m_hHV, rgConnStatus, MAX_MUILT_CONNECT_COUNT, &iStatusCount);

	if ( hr != S_OK || iStatusCount != m_iBoxCount)
	{
		Quit();
		bSuccess = FALSE;
		m_DevStaus = false;
		return bSuccess;
	}
	else
	{
		bool breconnect = false;
		for (int i=0; i<iStatusCount; i++)
		{
			if( rgConnStatus[i].m_iStatus !=0)
			{
				breconnect = true;
				break;
			}
		}

		if (breconnect)
		{
			Quit();
			bSuccess = FALSE;
		}

	}
	if (bSuccess)
	{
		m_DevStaus = true;
	}
	return bSuccess;
}

//////////////////////////////////////////
BOOL CVPRImpl::InitEx(UINT uID, char *pchPort, DWORD idThread, UINT uMsg, BOOL bProvideBigPic/*=TRUE*/)
{
	


	BOOL bSuccess=TRUE;
	m_hWndHandle = NULL;
	m_idThread = idThread;
	m_uMsg = uMsg;
	m_DevStaus = false;

	char address[MAX_PATH] = {0};
	strcpy(address, pchPort);

	strcpy( m_szAddress, pchPort);

	m_fInitEx = true;
	m_uid = uID;
	m_bProvideBigPic = bProvideBigPic;

	//增加的代码
	if (m_hHV != NULL)
	{
		Quit();
	}


	char *token;
	token = strtok( address, "," );
	int i=-1;
	while( token != NULL )
	{

		++i;
		m_prgszIp[i] = new char[32];
		memset(m_prgszIp[i], 0, 32);
		strcpy(m_prgszIp[i], token);
		token = strtok( NULL, ",");
	}

	m_iBoxCount = i+1;

	for (i=0; i<m_iBoxCount; i++)
	{
		for (int j=0; j<32; j++)
		{
			if (m_prgszIp[i][j] ==':')
			{
				m_prgszIp[i][j] = '\0';
				break;
			}
		}
	}


	if (m_iBoxCount == 0)
	{
		bSuccess = FALSE;
		return  bSuccess;
	}


	HRESULT	 hr = OpenMultiHv( (char**)m_prgszIp, m_iBoxCount, &m_hHV );

	 if(S_OK == hr && m_hHV != NULL)
	{

		/*if ( SetMultiHvCallBack( m_hHV, CVPRImpl::OnPlateProxy, this, 0, _TYPE_PLATE_STR ) != S_OK )
		{
			Quit();
			bSuccess = FALSE;
		}
		else
		{
			if ( SetMultiHvCallBack( m_hHV, CVPRImpl::OnBinImageProxy, this, 0, _TYPE_BINARY_IMAGE ) != S_OK )
			{
				Quit();
				bSuccess = FALSE;
			}
			else
			{
				if ( SetMultiHvCallBack( m_hHV, CVPRImpl::OnJpgImageProxy, this, 0, _TYPE_BIG_IMAGE ) != S_OK )
				{
					Quit();
					bSuccess = FALSE;
				}
			}
		}*/

	}
	else
	{
		bSuccess = FALSE;
		m_DevStaus  = false;
		return bSuccess;
	}


	Sleep(10000);
	 hr = E_FAIL;
	HV_CONNECTION_STATUS rgConnStatus[MAX_MUILT_CONNECT_COUNT];
	int iStatusCount;
	hr = MultiHvIsConnected(m_hHV, rgConnStatus, MAX_MUILT_CONNECT_COUNT, &iStatusCount);

	if ( hr != S_OK || iStatusCount != m_iBoxCount)
	{
		Quit();
		bSuccess = FALSE;
		m_DevStaus = false;
		return bSuccess;
	}
	else
	{
		bool breconnect = false;
		for (int i=0; i<iStatusCount; i++)
		{
			if( rgConnStatus[i].m_iStatus !=0)
			{
				breconnect = true;
				break;
			}
		}

		if (breconnect)
		{
			Quit();
			bSuccess = FALSE;
		}
	}
	if (m_hHV != NULL)
	{
		if ( SetMultiHvCallBack( m_hHV, CVPRImpl::OnPlateProxy, this, 0, _TYPE_PLATE_STR ) != S_OK )
		{
			Quit();
			bSuccess = FALSE;
		}
		else
		{
			if ( SetMultiHvCallBack( m_hHV, CVPRImpl::OnBinImageProxy, this, 0, _TYPE_BINARY_IMAGE ) != S_OK )
			{
				Quit();
				bSuccess = FALSE;
			}
			else
			{
				if ( SetMultiHvCallBack( m_hHV, CVPRImpl::OnJpgImageProxy, this, 0, _TYPE_BIG_IMAGE ) != S_OK )
				{
					Quit();
					bSuccess = FALSE;
				}
			}
		}
	}
	if (bSuccess )
	{
		m_DevStaus = true;
	}

	return bSuccess;
}


////////////////////////////////////////////////////////////////////
//
//  函数名称 : VPR_Quit()
//  函数功能 : 关闭车牌自动识别仪
//  参数说明 : 无
//  返回值   : TRUE，FALSE
//  备注     : 无
//
////////////////////////////////////////////////////////////////////
BOOL CVPRImpl::Quit()
{	
	BOOL bSuccess=TRUE;
	WriteLog("Quit():L1");
	if ( m_hHV != NULL )
	{
		CloseMultiHv( m_hHV );
		m_hHV = NULL;
	}
	WriteLog("Quit():L2");

	//增加的代码, 释放资源
	for(int i = 0; i < MAX_MUILT_CONNECT_COUNT; ++i)
	{
		if( m_prgszIp[i] != NULL )
		{
			delete[]  m_prgszIp[i];
			m_prgszIp[i] = NULL;
		}
	}

	m_iBoxCount = 0;
	WriteLog("Quit():L3");
	
	
	return bSuccess;
}

////////////////////////////////////////////////////////////////////
//
//  函数名称 : VPR_Capture()
//  函数功能 : 车牌自动识别仪手动抓拍图象并进行识别
//  参数说明 : 无
//  返回值   : TRUE，FALSE
//  备注     : 无
//
////////////////////////////////////////////////////////////////////
BOOL CVPRImpl::Capture()
{	
	BOOL bSuccess=TRUE;
	
	if ( m_hHV != NULL )
	{
		//HV_ForceSend( m_hHV );

		MultiHv_ForceSend( m_hHV );
	}
	else
	{
		bSuccess = FALSE;
	}
	
	return bSuccess;
}

int CVPRImpl::CaptureEx()
{
	int iRet = 0;

	int iTimeCount = 0;
	if ( m_hHV != NULL )
	{
		//HV_ForceSend( m_hHV );
		MultiHv_ForceSend( m_hHV );
		while( !m_fInfoReady )
		{
			if( iTimeCount > RECV_PLATE_TIMEOUT )
			{
				iRet = 2;
				break;
			}
			iTimeCount += 100;
			Sleep(100);
		}
	}
	else
	{
		iRet = 2;
	}

	if( iRet == 0 )
	{
		if( strcmp( m_szPlate, "无车牌" ) == 0 )
		{
			iRet = 1;
		}
	}
	
	return iRet;
}

BOOL CVPRImpl::SyncTime()
{

	BOOL bSuccess=TRUE;
	

	return bSuccess;
}


////////////////////////////////////////////////////////////////////
//
//  函数名称 : VPR_GetVehicleInfo(char * pchPlate,int * piByteBinImagLen,BYTE * pByteBinImage,int * piJpegImageLen,BYTE * pByteJpegImage)
//  函数功能 : 取车牌自动识别仪识别结果
//  参数说明 : pchPlate----返回牌照号
//             piByteBinImagLen----返回车辆二值图的大小
//             pByteBinImage----返回的车辆二值图
//             piJpegImageLen----返回车辆JPEG图像的大小
//             pByteJpegImage----返回车辆的图片, 为JPEG格式
//  返回值   : TRUE，FALSE
//  备注     : 无
//
////////////////////////////////////////////////////////////////////
BOOL CVPRImpl::GetVehicleInfo(
		char * pchPlate,int* piByteBinImagLen,BYTE * pbByteBinImage,
		int * piJpegImageLen,BYTE * pbByteJpegImage)
{	
	BOOL bSuccess=TRUE;

	EnterCriticalSection( &m_csLock );
	if ( !m_fInfoReady )
	{
		bSuccess = FALSE;
	}
	else
	{
		strncpy( pchPlate, m_szPlate, sizeof( m_szPlate ) -1 );
		//memcpy( pByteBinImage, m_rgbBinImage, m_iBinSize );

		for (int y = 0; y < BIN_HEIGHT; y++)
		{
			BYTE *pSrc = &m_rgbBinImage[(BIN_HEIGHT - 1) * BIN_STRIDE];
			BYTE *pDest = pbByteBinImage;
			for (int y = 0; y < BIN_HEIGHT; y++)
			{
				memcpy(pDest, pSrc, BIN_STRIDE);
				pSrc -= BIN_STRIDE;
				pDest += BIN_STRIDE;
			}
			//字节中前后bit位互换
			for(int i = 0; i < BIN_BYTE_COUNT; i++)
			{
				BYTE bTemp = pbByteBinImage[i];
				BYTE bTemp2 = 0;
				bTemp2 |= (bTemp & 0x80) >> 7;
				bTemp2 |= (bTemp & 0x40) >> 5;
				bTemp2 |= (bTemp & 0x20) >> 3;
				bTemp2 |= (bTemp & 0x10) >> 1;
				bTemp2 |= (bTemp & 0x08) << 1;
				bTemp2 |= (bTemp & 0x04) << 3;
				bTemp2 |= (bTemp & 0x02) << 5;
				bTemp2 |= (bTemp & 0x01) << 7;
				pbByteBinImage[i] = bTemp2;
			}
		}
		*piByteBinImagLen = m_iBinSize;
		if (pbByteJpegImage && m_bProvideBigPic && NULL != m_pbJpgImage)
		{
			memcpy( pbByteJpegImage, m_pbJpgImage, m_iJpgSize );
		}
		if (piJpegImageLen && m_bProvideBigPic)
		{
			*piJpegImageLen = m_iJpgSize;
		}
		m_fInfoReady = false;
		memset(m_rgbBinImage, 0, 280);
	}
	LeaveCriticalSection( &m_csLock );
	
	return bSuccess;
}

BOOL CVPRImpl::GetVehicleInfoEx(
			      char * pchPlate,int* piByteBinImagLen,BYTE * pbByteBinImage,
			      int * piJpegImageLen,BYTE * pbByteJpegImage,UINT *puSpeed)
{	
	BOOL bSuccess=TRUE;

	EnterCriticalSection( &m_csLock );
	if ( !m_fInfoReady )
	{
		bSuccess = FALSE;
	}
	else
	{
		strncpy( pchPlate, m_szPlate, sizeof( m_szPlate ) -1 );
		//memcpy( pByteBinImage, m_rgbBinImage, m_iBinSize );

		for (int y = 0; y < BIN_HEIGHT; y++)
		{
			BYTE *pSrc = &m_rgbBinImage[(BIN_HEIGHT - 1) * BIN_STRIDE];
			BYTE *pDest = pbByteBinImage;
			for (int y = 0; y < BIN_HEIGHT; y++)
			{
				memcpy(pDest, pSrc, BIN_STRIDE);
				pSrc -= BIN_STRIDE;
				pDest += BIN_STRIDE;
			}
			//字节中前后bit位互换
			for(int i = 0; i < BIN_BYTE_COUNT; i++)
			{
				BYTE bTemp = pbByteBinImage[i];
				BYTE bTemp2 = 0;
				bTemp2 |= (bTemp & 0x80) >> 7;
				bTemp2 |= (bTemp & 0x40) >> 5;
				bTemp2 |= (bTemp & 0x20) >> 3;
				bTemp2 |= (bTemp & 0x10) >> 1;
				bTemp2 |= (bTemp & 0x08) << 1;
				bTemp2 |= (bTemp & 0x04) << 3;
				bTemp2 |= (bTemp & 0x02) << 5;
				bTemp2 |= (bTemp & 0x01) << 7;
				pbByteBinImage[i] = bTemp2;
			}
		}
		*piByteBinImagLen = m_iBinSize;
		if (pbByteJpegImage && m_bProvideBigPic && NULL != m_pbJpgImage)
		{
			memcpy( pbByteJpegImage, m_pbJpgImage, m_iJpgSize );
		}
		if (piJpegImageLen && m_bProvideBigPic)
		{
			*piJpegImageLen = m_iJpgSize;
		}
		char* pcSpeed = strstr(m_szAppendInfo, "车速");
		int iSpeed = 0;
		if (pcSpeed)
		{
			sscanf(pcSpeed, "车速:%d", &iSpeed);
		}
		else
		{
			pcSpeed = strstr(m_szAppendInfo, "视频测速");
			if (pcSpeed)
			{
				sscanf(pcSpeed, "视频测速:%d", &iSpeed);
			}
			else
			{
				pcSpeed = strstr(m_szAppendInfo, "雷达测速");
				if (pcSpeed)
				{
					sscanf(pcSpeed, "雷达测速:%d", &iSpeed);
				}
				else
				{
					pcSpeed = strstr(m_szAppendInfo, "线圈测速");
					if (pcSpeed)
					{
						sscanf(pcSpeed, "线圈测速:%d", &iSpeed);
					}
				}
			}
		}
		*puSpeed = iSpeed * 1000;
		memset(m_rgbBinImage, 0, 280);
		m_fInfoReady = false;
	}
	LeaveCriticalSection( &m_csLock );

	return bSuccess;
}


BOOL CVPRImpl::GetVehicleInfoEx2(char * pchPlate,int* piByteBinImagLen,BYTE * pbByteBinImage,int * piJpegImageLen,BYTE * pbByteJpegImage,UINT *puSpeed,
								  DWORD64* pdwTimeMs)
{

	BOOL bSuccess=TRUE;
	EnterCriticalSection(&m_csLock);
	if ( !m_fInfoReady )
	{
		bSuccess = FALSE;
	}
	else
	{
		*pdwTimeMs = m_dw64TimeMs;
		strncpy( pchPlate, m_szPlate, sizeof( m_szPlate ) -1 );
		//memcpy( pByteBinImage, m_rgbBinImage, m_iBinSize );

		for (int y = 0; y < BIN_HEIGHT; y++)
		{
			BYTE *pSrc = &m_rgbBinImage[(BIN_HEIGHT - 1) * BIN_STRIDE];
			BYTE *pDest = pbByteBinImage;
			for (int y = 0; y < BIN_HEIGHT; y++)
			{
				memcpy(pDest, pSrc, BIN_STRIDE);
				pSrc -= BIN_STRIDE;
				pDest += BIN_STRIDE;
			}
			//字节中前后bit位互换
			for(int i = 0; i < BIN_BYTE_COUNT; i++)
			{
				BYTE bTemp = pbByteBinImage[i];
				BYTE bTemp2 = 0;
				bTemp2 |= (bTemp & 0x80) >> 7;
				bTemp2 |= (bTemp & 0x40) >> 5;
				bTemp2 |= (bTemp & 0x20) >> 3;
				bTemp2 |= (bTemp & 0x10) >> 1;
				bTemp2 |= (bTemp & 0x08) << 1;
				bTemp2 |= (bTemp & 0x04) << 3;
				bTemp2 |= (bTemp & 0x02) << 5;
				bTemp2 |= (bTemp & 0x01) << 7;
				pbByteBinImage[i] = bTemp2;
			}
		}
		*piByteBinImagLen = m_iBinSize;
		if (pbByteJpegImage && m_bProvideBigPic &&  NULL !=m_pbJpgImage)
		{
			memcpy( pbByteJpegImage, m_pbJpgImage, m_iJpgSize );
		}
		if (piJpegImageLen && m_bProvideBigPic)
		{
			*piJpegImageLen = m_iJpgSize;
		}
		char* pcSpeed = strstr(m_szAppendInfo, "车速");
		int iSpeed = 0;
		if (pcSpeed)
		{
			sscanf(pcSpeed, "车速:%d", &iSpeed);
		}
		else
		{
			pcSpeed = strstr(m_szAppendInfo, "视频测速");
			if (pcSpeed)
			{
				sscanf(pcSpeed, "视频测速:%d", &iSpeed);
			}
			else
			{
				pcSpeed = strstr(m_szAppendInfo, "雷达测速");
				if (pcSpeed)
				{
					sscanf(pcSpeed, "雷达测速:%d", &iSpeed);
				}
				else
				{
					pcSpeed = strstr(m_szAppendInfo, "线圈测速");
					if (pcSpeed)
					{
						sscanf(pcSpeed, "线圈测速:%d", &iSpeed);
					}
				}
			}
		}
		*puSpeed = iSpeed * 1000;
		memset(m_rgbBinImage, 0, 280);
		m_fInfoReady = false;
	}
	LeaveCriticalSection( &m_csLock );

	return bSuccess;


}

////////////////////////////////////////////////////////////////////////
// 检查牌照识别器状态
int CVPRImpl::CheckStatus(char * pchVprDevStatus)
{
	if( pchVprDevStatus == NULL )
	{
		return 4;
	}

	int bSuccess=0;

	if ( m_hHV == NULL  || !m_DevStaus)
	{
		bSuccess = 4;
		sprintf(pchVprDevStatus, "通讯故障");
	}
	else
	{
		sprintf(pchVprDevStatus,"设备正常");
	}
	
	return bSuccess;
}

inline int SetInt32ToByteArray( BYTE *pbDest, INT32 i32Src )
{
	memcpy( pbDest, &i32Src, sizeof( i32Src ) );
	return sizeof( i32Src );
}

inline int SetInt16ToByteArray( BYTE *pbDest, SHORT i16Src )
{
	memcpy( pbDest, &i16Src, sizeof( i16Src ) );
	return sizeof( i16Src );
}

int CVPRImpl::OnRecordBegin( void* pFirstParameter, DWORD32 dwCarID )
{
	if ( pFirstParameter == NULL )
	{
		return 0;
	}
	CVPRImpl *pVPR = ( CVPRImpl* )pFirstParameter;
	pVPR->WriteLog("OnRecordBegin开始");

	EnterCriticalSection( &pVPR->m_csLock );
	pVPR->m_dwCarID = dwCarID;
	pVPR->m_fInfoReady = false;
	pVPR->m_fHasBigImg = false;
	memset(pVPR->m_rgbBinImage, 0, 280);
	LeaveCriticalSection( &pVPR->m_csLock );

	pVPR->WriteLog("OnRecordBegin结束");
	return 0;
}

int CVPRImpl::OnRecordEnd( void* pFirstParameter, DWORD32 dwCarID )
{
	if ( pFirstParameter == NULL )
	{
		return 0;
	}
	CVPRImpl *pVPR = ( CVPRImpl* )pFirstParameter;
	bool fSendMsg = false;
	pVPR->WriteLog("OnRecordEnd开始");
	EnterCriticalSection( &pVPR->m_csLock );

	if( !pVPR->m_fInfoReady && pVPR->m_dwCarID == dwCarID )
	{
		pVPR->m_fInfoReady = true;
		fSendMsg = true;
	}
	LeaveCriticalSection( &pVPR->m_csLock );

	if(fSendMsg && pVPR->m_hWndHandle != NULL && !pVPR->m_fInitEx)
	{
		::PostMessage( pVPR->m_hWndHandle, pVPR->m_uMsg, (WPARAM)pVPR->m_uid, 0 );
		pVPR->WriteLog("OnRecordEnd结束PostMessage");
	}
	if(fSendMsg && pVPR->m_idThread != 0 && pVPR->m_fInitEx)
	{
		::PostThreadMessage( pVPR->m_idThread, pVPR->m_uMsg, (WPARAM)pVPR->m_uid, 0 );
		pVPR->WriteLog("OnRecordEnd结束PostThreadMessage");
	}

	return 0;
}

//车牌回调函数
int CVPRImpl::OnPlateProxy( void *pFirstParameter, DWORD32 dwCarID, PSTR pcPlateNo, DWORD64 dw64TimeMs )
{

	if ( pFirstParameter == NULL )
	{
		return 0;
	}
	
	CVPRImpl *pVPR = ( CVPRImpl* )pFirstParameter;
	pVPR->WriteLog("OnPlateProxy开始");

	bool bnoplate = true;
	EnterCriticalSection( &pVPR->m_csLock );

	pVPR->m_dw64TimeMs  = dw64TimeMs;

	if (!pVPR->m_fInfoReady && pVPR->m_dwCarID == dwCarID)
	{
		if ( strcmp( pcPlateNo, "  未检测" ) == 0  || strstr( pcPlateNo, "无车牌" ) != NULL)
		{
			//bnoplate = false;
			strcpy( pVPR->m_szPlate, "无车牌" );
			//全黑的二值化图
			memset( pVPR->m_rgbBinImage, 0, BIN_BYTE_COUNT );
			pVPR->m_iBinSize = BIN_BYTE_COUNT;
		}
		else
		{
			strncpy( pVPR->m_szPlate, &pcPlateNo[0], sizeof( pVPR->m_szPlate ) - 1 );
		}
		char *pcPlateinfo=NULL;
		MultiHv_GetPlateInfo(pVPR->m_hHV, &pcPlateinfo);
		strcpy(pVPR->m_szAppendInfo, pcPlateinfo);
	}

	//字符叠加替换函数
	bool bOverlay = false;
	int iFinalLen = 0;

	
	pVPR->AnalysisOverlayText();

	pVPR->WriteLog("大图字符叠加开始:1");

	if (NULL != pVPR->m_pbJpgImage && pVPR->m_bOverlayStringEnable)
	{
		bOverlay = pVPR->OverlayStringToBigImg(pVPR->m_strOverlayText.GetBuffer(), pVPR->m_pbJpgImage, pVPR->m_iJpgSize, &iFinalLen, pVPR->m_iOverlayX, pVPR->m_iOverlayY);
		pVPR->m_strOverlayText.ReleaseBuffer();
		pVPR->WriteLog("大图字符叠加:2");
		if (!bOverlay)
		{
			if (iFinalLen > 0)
			{				
				BYTE* pbTempData = NULL;
				pbTempData = new BYTE[iFinalLen];				
				if (NULL != pbTempData)
				{
					memset(pbTempData, 0, iFinalLen);
					memcpy(pbTempData, pVPR->m_pbJpgImage, pVPR->m_iJpgSize);

					delete[] pVPR->m_pbJpgImage;
					pVPR->m_pbJpgImage = NULL;
					
					pVPR->m_pbJpgImage = pbTempData;
					pVPR->m_iJpgSize = iFinalLen;
				}				
				iFinalLen = 0;
				pVPR->WriteLog("大图字符叠加:3");

				bOverlay = pVPR->OverlayStringToBigImg(pVPR->m_strOverlayText.GetBuffer(), pVPR->m_pbJpgImage, pVPR->m_iJpgSize, &iFinalLen, pVPR->m_iOverlayX, pVPR->m_iOverlayY);
				pVPR->m_strOverlayText.ReleaseBuffer();

				pVPR->WriteLog("大图字符叠加:4");
				if (bOverlay)
				{
					pVPR->WriteLog("大图字符叠加成功");
				}
				else
				{
					pVPR->WriteLog("大图字符叠加失败");
				}
			}
			else
			{
				pVPR->WriteLog("大图字符叠加:5");
				pVPR->WriteLog("大图字符叠加失败");
			}
		}
		else
		{
			pVPR->WriteLog("大图字符叠加:6");
			pVPR->WriteLog("大图字符叠加成功");
		}
		
		pVPR->m_iJpgSize = iFinalLen;
	}

	//新增代码
	bool fSendMsg = false;


	if(pVPR->m_begin == true && pVPR->m_finished == false)
	{
		if( !pVPR->m_fInfoReady && pVPR->m_dwCarID == dwCarID )
		{
			pVPR->m_fInfoReady = true;
			fSendMsg = true;
		}

	}

	pVPR->m_begin = false;
	pVPR->m_finished = true;
	
	/////

	LeaveCriticalSection( &pVPR->m_csLock );
	

	if(fSendMsg && pVPR->m_hWndHandle != NULL && !pVPR->m_fInitEx && bnoplate)
	{
		::PostMessage( pVPR->m_hWndHandle, pVPR->m_uMsg, (WPARAM)pVPR->m_uid, 0 );
		pVPR->WriteLog("OnPlateProxy结束PostMessage");
	}
	if(fSendMsg && pVPR->m_idThread != 0 && pVPR->m_fInitEx&& bnoplate)
	{
		::PostThreadMessage( pVPR->m_idThread, pVPR->m_uMsg, (WPARAM)pVPR->m_uid, 0 );
		pVPR->WriteLog("OnPlateProxy结束PostThreadMessage");
	}
	


	return 0;
}

//车牌二值化小图回调函数
int CVPRImpl::OnBinImageProxy( 
		void *pFirstParameter, DWORD32 dwCarID, WORD wImageWidth, WORD wImageHigh,
		BYTE bType, WORD wSize, BYTE *pbImage, DWORD64 dw64TimeMs )
{

	if ( pFirstParameter == NULL )
	{
		return 0;
	}

	CVPRImpl *pVPR = ( CVPRImpl* )pFirstParameter;
	
	pVPR->WriteLog("OnBinImageProxy开始");

	EnterCriticalSection( &pVPR->m_csLock );
	if (!pVPR->m_fInfoReady && pVPR->m_dwCarID == dwCarID)
	{
		memcpy( pVPR->m_rgbBinImage, pbImage, wSize );
		pVPR->m_iBinSize = wSize;
		
	}
	LeaveCriticalSection( &pVPR->m_csLock );
	pVPR->WriteLog("OnBinImageProxy结束");
	
	return 0;
}

//车牌大图回调函数
int CVPRImpl::OnJpgImageProxy( void *pFirstParameter, 
	DWORD32 dwCarID, WORD wImageWidth, WORD wImageHigh, BYTE bType,
	WORD wSize,	PBYTE pbImage, WORD wImageID, WORD wHighImgFlag, WORD wPlateWidth, WORD wPlateHigh, DWORD64 dw64TimeMs )
{

	if ( pFirstParameter == NULL )
	{
		return 0;
	}
	CVPRImpl *pVPR = ( CVPRImpl* )pFirstParameter;
	EnterCriticalSection( &pVPR->m_csLock );

	pVPR->WriteLog("OnJpgImageProxy开始");

	//增加的代码
	if (pVPR->m_begin == false && pVPR->m_finished == true)   //第一次回调
	{
		pVPR->m_begin = true;
		pVPR->m_finished = false;


		pVPR->m_dwCarID = dwCarID;
		pVPR->m_fInfoReady = false;
		pVPR->m_fHasBigImg = false;
	}

	if (!pVPR->m_fInfoReady && pVPR->m_dwCarID == dwCarID && !pVPR->m_fHasBigImg) 
	{
		int iSize = 0;
		if( (wHighImgFlag & 0xFF00) != 0 )
		{
			iSize = wSize + (int)((wHighImgFlag & 0x00FF) << 16);
		}
		else
		{
			iSize = wSize;
		}
		if (NULL != pVPR->m_pbJpgImage)
		{
			delete[] pVPR->m_pbJpgImage;
			pVPR->m_pbJpgImage = NULL;
		}
		int iMaxImgSize = iSize * 2;
		pVPR->m_pbJpgImage = new BYTE[iMaxImgSize];
		if (pVPR->m_pbJpgImage == NULL)
		{
			return 0;
		}
		memset(pVPR->m_pbJpgImage, 0, iMaxImgSize);

		pVPR->m_fHasBigImg = true;
		LARGE_INTEGER liTemp = {0};
		pVPR->m_pstmBigImg->Seek( liTemp, STREAM_SEEK_SET, NULL );
		pVPR->m_pstmBigImg->Write(pbImage, iSize, NULL);

		Bitmap bmpImg(pVPR->m_pstmBigImg);
		ULONG ulRead = 0;


		bool bOverlay = false;
		int iFinalLen = 0;

		if (bmpImg.GetWidth() == 720)
		{
			Bitmap bmpTmp( BIG_IMG_WIDTH, BIG_IMG_HEIGHT );
			Graphics grfTmp(&bmpTmp);
			grfTmp.DrawImage( &bmpImg, 0, 0, BIG_IMG_WIDTH, BIG_IMG_HEIGHT );
			pVPR->m_pstmBigImg->Seek( liTemp, STREAM_SEEK_SET, NULL );
			bmpTmp.Save(pVPR->m_pstmBigImg, &pVPR->m_jpgClsid);

			pVPR->m_pstmBigImg->Seek( liTemp, STREAM_SEEK_SET, NULL );
			pVPR->m_pstmBigImg->Read(pVPR->m_pbJpgImage, BIG_IMG_SIZE, &ulRead);

		}
		else
		{
			Bitmap bmpTmp( BIG_IMG_WIDTH_HIGH, BIG_IMG_HEIGHT_HIGH );
			Graphics grfTmp(&bmpTmp);
			grfTmp.DrawImage( &bmpImg, 0, 0, BIG_IMG_WIDTH_HIGH, BIG_IMG_HEIGHT_HIGH );
			pVPR->m_pstmBigImg->Seek( liTemp, STREAM_SEEK_SET, NULL );
			bmpTmp.Save(pVPR->m_pstmBigImg, &pVPR->m_jpgClsid);

			pVPR->m_pstmBigImg->Seek( liTemp, STREAM_SEEK_SET, NULL );
			pVPR->m_pstmBigImg->Read(pVPR->m_pbJpgImage, iMaxImgSize, &ulRead);

		}
		
		pVPR->m_iJpgSize = (int)ulRead;
	}
	LeaveCriticalSection( &pVPR->m_csLock );

	pVPR->WriteLog("OnJpgImageProxy结束");
	
	return 0;
}

void CVPRImpl::ReadConfigFile()
{
	char chFileName[MAX_PATH];
	GetModuleFileNameA(NULL, chFileName, MAX_PATH-1);
	PathRemoveFileSpecA(chFileName);
	char chIniFileName[MAX_PATH] = { 0 };
	strcpy(chIniFileName, chFileName);
	strcat(chIniFileName, "\\VPRCtrlConfig.ini");

	//获取叠加字符的内容
	//char chOverlayString[1024] = {0};
	GetPrivateProfileStringA("OverlayString","Content","",m_chOverlayText,1024,chIniFileName);

	int iOverlayStringEnable = GetPrivateProfileIntA("OverlayString", "Enable", 1, chIniFileName);
	m_bOverlayStringEnable = iOverlayStringEnable >0 ? true : false;

	m_iOverlayX = GetPrivateProfileIntA("OverlayString", "OverlayX", 0, chIniFileName);
	m_iOverlayX = m_iOverlayX >0 ? m_iOverlayX : 0;

	m_iOverlayY = GetPrivateProfileIntA("OverlayString", "OverlayY", 0, chIniFileName);
	m_iOverlayY = m_iOverlayY >0 ? m_iOverlayY : 0;

	m_iFontSize = GetPrivateProfileIntA("OverlayString", "FontSize", 32, chIniFileName);
	//if (m_iFontSize <0 || m_iFontSize > 50)
	//{
	//	m_iFontSize = 32;
	//}

	//字体颜色
	char chTemp[256] = {0};
	int r,g,b;
	GetPrivateProfileString("OverlayString", "FontColor", "255,255,255", chTemp, 256, chIniFileName);
	sscanf(chTemp, "%d,%d,%d", &r, &g, &b);
	m_dwFontColor = RGB(r,g,b);

	int iClipWidth = 720;
	int iClipHeight =576;

	iClipWidth = GetPrivateProfileIntA("ClipImg", "ClipWidth", 720, chIniFileName);
	m_iClipWidth = iClipWidth >0 ? iClipWidth : 720;

	iClipHeight = GetPrivateProfileIntA("ClipImg", "ClipHeight", 576, chIniFileName);
	m_iClipHeight = iClipHeight >0 ? iClipHeight : 576;

}

bool CVPRImpl::OverlayStringToBigImg( char* overLayString,PBYTE DestImg, int ImgLen, int* finalLen, int overLayX, int overLayY )
{
	WriteLog("进入OverlayStringToBigImg……");
	IStream *pstmp = NULL;
	CreateStreamOnHGlobal(NULL, TRUE, &pstmp);
	LARGE_INTEGER LiTemp = {0};
	ULARGE_INTEGER uLiZero = {0};

	pstmp->Seek(LiTemp,STREAM_SEEK_SET,NULL);
	pstmp->SetSize(uLiZero);
	ULONG ulRealSize = 0;
	pstmp->Write(DestImg, ImgLen,&ulRealSize);

	Bitmap tmpBMP(pstmp);

	//Bitmap* bitmap2 = new Bitmap(tmpBMP.GetWidth(), tmpBMP.GetHeight(), PixelFormat24bppRGB);
	Bitmap* bitmap2 = new Bitmap(m_iClipWidth, m_iClipHeight);
	Graphics grfNew(bitmap2);
	char chChinese[256] = {0};

	if (Ok == grfNew.GetLastStatus())
	{
		//绘制背景图
		grfNew.DrawImage(&tmpBMP, 0, 0, bitmap2->GetWidth(), bitmap2->GetHeight());
		if (Ok == grfNew.GetLastStatus())
		{
			//开始叠加字符
			FontFamily fFamily(L"宋体");
			PointF tmpPoint(0, 0);

			//根据字符串长度设置字体大小
			int iTempFontSize = m_iFontSize;

			//字体排列
			StringFormat fmtString;
			fmtString.SetLineAlignment(StringAlignmentNear);

			//字体颜色
			BYTE r, g, b;
			r = (BYTE)(m_dwFontColor);
			g = (BYTE)(m_dwFontColor >> 8 & 0xFF);
			b = (BYTE)(m_dwFontColor >> 16 & 0xFF);
			SolidBrush solidBrush(Color(255, r, g, b));

			wchar_t wchText[1024] = {0};
			MultiByteToWideChar(CP_ACP, 0, overLayString, (int)strlen(overLayString)+1, wchText, sizeof(wchText));

			int iFontHeight = 0;
			int iFontWidth = 0;

			//int iImageWidth = tmpBMP.GetWidth();
			//int iImageHeight = tmpBMP.GetHeight();

			int iImageWidth = bitmap2->GetWidth();
			int iImageHeight = bitmap2->GetHeight();
			

			while (true)
			{
				Gdiplus::Font fontTmp(&fFamily, (REAL)iTempFontSize, FontStyleBold);
				RectF outrect;
				grfNew.MeasureString(wchText, -1, &fontTmp,tmpPoint,&outrect);	//测量字体宽高，得出框架
				grfNew.MeasureString(wchText, -1, &fontTmp, tmpPoint, &outrect); 
				//if (outrect.Width > (tmpBMP.GetWidth() - 1) )
				if (outrect.Width > (bitmap2->GetWidth() - 1) )
				{
					iTempFontSize = iTempFontSize - 2;
					continue;
				}

				iFontWidth = (int)outrect.Width;
				iFontHeight = (int)outrect.Height;
				break;
			}
			//刷新字体框架
			Gdiplus::Font fontTmp(&fFamily, (REAL)iTempFontSize,FontStyleBold);
			RectF rfMString1((REAL)overLayX, (REAL)overLayY, (REAL)iImageWidth, (REAL)(iFontHeight+5));
			grfNew.DrawString(wchText, -1, &fontTmp, rfMString1,&fmtString,&solidBrush);

			//-------------字符叠加结束
			IStream* pStreamIn = NULL;
			CreateStreamOnHGlobal( NULL, TRUE, &pStreamIn);
			if (pStreamIn == NULL)
			{
				
				if (NULL != bitmap2)
				{
					delete bitmap2;
					bitmap2 = NULL;
				}
				if (NULL != pstmp)
				{
					pstmp->Release();
					pstmp = NULL;
				}
				return false;
			}
			LARGE_INTEGER liTempstart = { 0 };
			ULARGE_INTEGER uliTempZero = { 0 };

			pStreamIn->Seek(liTempstart, STREAM_SEEK_SET, NULL);
			pStreamIn->SetSize(uliTempZero);
			bitmap2->Save(pStreamIn, &m_jpgClsid, 0);
			pStreamIn->Seek(liTempstart, STREAM_SEEK_SET,NULL);

			const int  MAX_BIG_SIZE = 5*1024*1024;
			BYTE* pImgBuff = new BYTE[MAX_BIG_SIZE];
			if (NULL == pImgBuff)
			{
				if (NULL != bitmap2)
				{
					delete bitmap2;
					bitmap2 = NULL;
				}
				if (NULL != pstmp)
				{
					pstmp->Release();
					pstmp = NULL;
				}
				
				if (NULL != pStreamIn)
				{
					pStreamIn->Release();
					pStreamIn = NULL;
				}

				WriteLog("字符叠加pImgBuff失败");
				return false;
			}
			memset(pImgBuff,0,MAX_BIG_SIZE);

			ULONG iSize = 0;
			if (Ok == (pStreamIn->Read(pImgBuff, MAX_BIG_SIZE,&iSize)))
			{
				*finalLen = iSize;
				if ( ImgLen >= iSize)
				{
					memcpy(DestImg, pImgBuff,iSize);					
				}
				else
				{
					
					if (pImgBuff)
					{
						delete[] pImgBuff;
						pImgBuff = NULL;
					}
					
					if (bitmap2)
					{
						delete bitmap2;
						bitmap2 = NULL;
					}

					if (NULL != pstmp)
					{
						pstmp->Release();
						pstmp = NULL;
					}
					if (NULL != pStreamIn)
					{
						pStreamIn->Release();
						pStreamIn =NULL;
					}					
					WriteLog("字符叠加失败,原数组空间不足");
					return false;
				}				
			}
			else
			{
				WriteLog("字符叠加pStreamIn->Read失败");
			}

			if (NULL != pImgBuff)
			{
				delete[] pImgBuff;
				pImgBuff = NULL;
			}
			if (NULL != pStreamIn)
			{
				pStreamIn->Release();
				pStreamIn =NULL;
			}
		}
	}
	else
	{
		WriteLog("grfNew失败");
		if (NULL != bitmap2)
		{
			delete bitmap2;
			bitmap2 = NULL;
		}

		if (NULL != pstmp)
		{
			pstmp->Release();
			pstmp = NULL;
		}
		return false;
	}


	if (NULL != bitmap2)
	{
		delete bitmap2;
		bitmap2 = NULL;
	}

	if (NULL != pstmp)
	{
		pstmp->Release();
		pstmp = NULL;
	}


	WriteLog("字符叠加成功，离开OverlayStringToBigImg……");
	return true;
}

void CVPRImpl::AnalysisOverlayText()
{
	m_strOverlayText.Format("%s", m_chOverlayText);
	//获取车牌时间
	char chPlateTime[256] = {0};
	if (m_dw64TimeMs >= 0)
	{
		CTime tm(m_dw64TimeMs/1000);
		sprintf(chPlateTime, "%d-%02d-%02d %02d:%02d:%02d", tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(), tm.GetSecond());
	}
	else
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		sprintf(chPlateTime, "%d-%02d-%02d %02d:%02d%02d", st.wYear, st.wMonth, st.wDay	,st.wHour, st.wMinute,st.wSecond);
	}


	if (m_strOverlayText.Find("$(PLATE)"))
	{
		m_strOverlayText.Replace("$(PLATE)", m_szPlate);
	}
	if (m_strOverlayText.Find( "$(TIME)" ))
	{
		m_strOverlayText.Replace("$(TIME)", chPlateTime);
	}
	if (m_strOverlayText.Find("$(SPEED)"))
	{
		//....
	}
	while(-1 != m_strOverlayText.Find("$(NEWLINE)"))
	{
		m_strOverlayText.Replace("$(NEWLINE)", "\n");
	}

}

///////////////////////////////////////////////////
//
//接口实现
///////////////////////////////////

//设备数组
const int MAX_VPRCTRL = 100;
static CVPRImpl* g_rgpVPR[MAX_VPRCTRL] = {NULL};

BOOL WINAPI VPR_Init(UINT uID,char * pchPort,HWND hWndHandle,UINT uMsg, BOOL bProvideBigPic/*=TRUE*/)
{
	BOOL fRet = TRUE;
	global_WriteLog("VPR_Init::开始");
	EnterCriticalSection( &g_csVPRCtrlSync );
	int iPos = -1;
	for(int i = 0; i < MAX_VPRCTRL; ++i)
	{
		if( g_rgpVPR[i] == NULL && iPos == -1 )
		{
			iPos = i;
		}
		if( g_rgpVPR[i] != NULL && g_rgpVPR[i]->GetId() == uID )
		{
			fRet = FALSE;
			break;
		}
	}

	if( fRet )
	{
		if( iPos != -1 )
		{
			g_rgpVPR[iPos] = new CVPRImpl();
			fRet = g_rgpVPR[iPos]->Init(uID, pchPort, hWndHandle, uMsg);
			g_rgpVPR[iPos]->m_fQuit = FALSE;
			if (fRet)
			{
				g_rgpVPR[iPos]->m_hThread = CreateThread(
					NULL, 0, CVPRImpl::ThreadCheckStatus, g_rgpVPR[iPos], 0, &g_rgpVPR[iPos]->m_dwThreadId);
			}
			
			
		}
		else
		{
			fRet = FALSE;
		}
	}
	LeaveCriticalSection( &g_csVPRCtrlSync );
	global_WriteLog("VPR_Init::结束");
	return fRet;
}

BOOL WINAPI VPR_InitEx(UINT uID,char * pchPort,DWORD idThread,UINT uMsg,BOOL bProvideBigPic/*=TRUE*/)
{
	BOOL fRet = TRUE;
	int iPos = -1;
	global_WriteLog("VPR_InitEx::开始");
	EnterCriticalSection( &g_csVPRCtrlSync );
	for(int i = 0; i < MAX_VPRCTRL; ++i)
	{
		if( g_rgpVPR[i] == NULL && iPos == -1 )
		{
			iPos = i;
		}
		if( g_rgpVPR[i] != NULL && g_rgpVPR[i]->GetId() == uID )
		{
			fRet = FALSE;
			break;
		}
	}

	if( fRet )
	{
		if( iPos != -1 )
		{
			g_rgpVPR[iPos] = new CVPRImpl();
			fRet = g_rgpVPR[iPos]->InitEx(uID, pchPort, idThread, uMsg);
			g_rgpVPR[iPos]->m_fQuit = FALSE;
			if (fRet)
			{
				g_rgpVPR[iPos]->m_hThread = CreateThread(
					NULL, 0, CVPRImpl::ThreadCheckStatus, g_rgpVPR[iPos], 0, &g_rgpVPR[iPos]->m_dwThreadId);
			}
			
		}
		else
		{
			fRet = FALSE;
		}
	}
	LeaveCriticalSection( &g_csVPRCtrlSync );
	global_WriteLog("VPR_InitEx::结束");
	return fRet;
}

BOOL WINAPI VPR_Quit(UINT uID)
{
	BOOL fRet = FALSE;
	int iPos = -1;
	global_WriteLog("VPR_Quit::开始");
	EnterCriticalSection( &g_csVPRCtrlSync );
	for(int i = 0; i < MAX_VPRCTRL; ++i)
	{
		if( g_rgpVPR[i] != NULL && g_rgpVPR[i]->GetId() == uID )
		{
			iPos = i;
			break;
		}
	}
	if( iPos != -1 )
	{
		g_rgpVPR[iPos]->m_fQuit = TRUE;
		g_rgpVPR[iPos]->WriteLog("VPR_Quit:L1");
		if (g_rgpVPR[iPos]->m_hThread != NULL)
		{
			WaitForSingleObject(g_rgpVPR[iPos]->m_hThread, INFINITE);
			CloseHandle(g_rgpVPR[iPos]->m_hThread);
		}
		g_rgpVPR[iPos]->WriteLog("VPR_Quit:L2");
		fRet = g_rgpVPR[iPos]->Quit();
		delete g_rgpVPR[iPos];
		g_rgpVPR[iPos] = NULL;
		g_rgpVPR[iPos]->WriteLog("VPR_Quit:L3");
	}
	LeaveCriticalSection( &g_csVPRCtrlSync );
	global_WriteLog("VPR_Quit::结束");
	return fRet;
}

BOOL WINAPI VPR_Capture(UINT uID)
{
	BOOL fRet = FALSE;
	int iPos = -1;
	global_WriteLog("VPR_Capture::开始");

	EnterCriticalSection( &g_csVPRCtrlSync );
	for(int i = 0; i < MAX_VPRCTRL; ++i)
	{
		if( g_rgpVPR[i] != NULL && g_rgpVPR[i]->GetId() == uID )
		{
			iPos = i;
			break;
		}
	}
	if( iPos != -1 )
	{
		fRet = g_rgpVPR[iPos]->Capture();
	}
	LeaveCriticalSection( &g_csVPRCtrlSync );

	global_WriteLog("VPR_Capture::结束");
	return fRet;
}


int WINAPI VPR_CaptureEx(UINT uID)
{
	int iRet = 2;
	int iPos = -1;
	global_WriteLog("VPR_CaptureEx::开始");
	EnterCriticalSection( &g_csVPRCtrlSync );
	for(int i = 0; i < MAX_VPRCTRL; ++i)
	{
		if( g_rgpVPR[i] != NULL && g_rgpVPR[i]->GetId() == uID )
		{
			iPos = i;
			break;
		}
	}
	if( iPos != -1 )
	{
		iRet = g_rgpVPR[iPos]->CaptureEx();
	}
	LeaveCriticalSection( &g_csVPRCtrlSync );
	global_WriteLog("VPR_CaptureEx::结束");
	return iRet;
}

BOOL WINAPI VPR_GetVehicleInfo(
	UINT uID,char * pchPlate,int* piByteBinImagLen,BYTE * pbByteBinImage,
	int * piJpegImageLen,BYTE * pbByteJpegImage)
{
	BOOL fRet = FALSE;
	int iPos = -1;
	global_WriteLog("VPR_GetVehicleInfo::开始");
	EnterCriticalSection( &g_csVPRCtrlSync );
	for(int i = 0; i < MAX_VPRCTRL; ++i)
	{
		if( g_rgpVPR[i] != NULL && g_rgpVPR[i]->GetId() == uID )
		{
			iPos = i;
			break;
		}
	}
	if( iPos != -1 )
	{
		fRet = g_rgpVPR[iPos]->GetVehicleInfo( pchPlate, piByteBinImagLen, pbByteBinImage,
				piJpegImageLen, pbByteJpegImage );
	}
	LeaveCriticalSection( &g_csVPRCtrlSync );
	global_WriteLog("VPR_GetVehicleInfo::结束");
	return fRet;
}

BOOL WINAPI VPR_GetVehicleInfoEx(
			       UINT uID,char * pchPlate,int* piByteBinImagLen,BYTE * pbByteBinImage,
			       int * piJpegImageLen,BYTE * pbByteJpegImage, UINT *puSpeed)
{
	BOOL fRet = FALSE;
	int iPos = -1;
	global_WriteLog("VPR_GetVehicleInfoEx::开始");
	EnterCriticalSection( &g_csVPRCtrlSync );
	for(int i = 0; i < MAX_VPRCTRL; ++i)
	{
		if( g_rgpVPR[i] != NULL && g_rgpVPR[i]->GetId() == uID )
		{
			iPos = i;
			break;
		}
	}
	if( iPos != -1 )
	{
		fRet = g_rgpVPR[iPos]->GetVehicleInfoEx( pchPlate, piByteBinImagLen, pbByteBinImage,
			piJpegImageLen, pbByteJpegImage, puSpeed );
	}
	LeaveCriticalSection( &g_csVPRCtrlSync );
	global_WriteLog("VPR_GetVehicleInfoEx::结束");
	return fRet;
}


BOOL WINAPI VPR_GetVehicleInfoEx2(
								  UINT uID,char * pchPlate,int* piByteBinImagLen,BYTE * pbByteBinImage,
								  int * piJpegImageLen,BYTE * pbByteJpegImage,UINT *puSpeed, 
								  DWORD64* pdwTimeMs)
{
	BOOL fRet = FALSE;
	int iPos = -1;
	global_WriteLog("VPR_GetVehicleInfoEx2::开始");
	EnterCriticalSection( &g_csVPRCtrlSync );
	for(int i = 0; i < MAX_VPRCTRL; ++i)
	{
		if( g_rgpVPR[i] != NULL && g_rgpVPR[i]->GetId() == uID )
		{
			iPos = i;
			break;
		}
	}
	if( iPos != -1 )
	{
		fRet = g_rgpVPR[iPos]->GetVehicleInfoEx2( pchPlate, piByteBinImagLen, pbByteBinImage,
			piJpegImageLen , pbByteJpegImage, puSpeed ,pdwTimeMs);
	}
	LeaveCriticalSection( &g_csVPRCtrlSync );
	global_WriteLog("VPR_GetVehicleInfoEx2::结束");
	return fRet;

}


int WINAPI VPR_CheckStatus(UINT uID,char * pchVprDevStatus)
{
	int iRet = 4;
	global_WriteLog("VPR_CheckStatus::开始");
	EnterCriticalSection( &g_csVPRCtrlSync );
	int iPos = -1;
	for(int i = 0; i < MAX_VPRCTRL; ++i)
	{
		if( g_rgpVPR[i] != NULL && g_rgpVPR[i]->GetId() == uID )
		{
			iPos = i;
			break;
		}
	}
	if( iPos != -1 )
	{
		iRet = g_rgpVPR[iPos]->CheckStatus( pchVprDevStatus );
	}
	LeaveCriticalSection( &g_csVPRCtrlSync );
	global_WriteLog("VPR_CheckStatus::结束");
	return iRet;
}

int WINAPI VPR_ComparePlate(
	BOOL bFullCompare,int iRule,char * pchPlateIn,char * pchPlateOut)
{
	if ( ( pchPlateIn == NULL ) || ( pchPlateOut == NULL ) )
	{
		return 3;
	}
	global_WriteLog("VPR_ComparePlate::开始");

	WCHAR wchPlateIn[100], wchPlateOut[100];

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pchPlateIn, -1, wchPlateIn, 100);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pchPlateOut, -1, wchPlateOut, 100);

	int iInLen=(int)wcslen(wchPlateIn);
	int iOutLen=(int)wcslen(wchPlateOut);
	int iMaxLen=max(iInLen, iOutLen);
	
	int i(0);
	int iCount(0);
	while (wchPlateIn[i] && wchPlateOut[i])
	{
		if (wchPlateIn[i]==wchPlateOut[i])
		{
			iCount++;
		}
		i++;
	}
	//非完全匹配
	if (bFullCompare == FALSE)
	{
		iMaxLen = iRule;
		int iMax = iInLen;
		WCHAR* pMaxPlate = NULL;
		WCHAR* pMinPlate = NULL;
		if (iInLen > iOutLen)
		{
			iMax = iOutLen;
			pMaxPlate = wchPlateIn;
			pMinPlate = wchPlateOut;
		}
		else
		{
			pMaxPlate = wchPlateOut;
			pMinPlate = wchPlateIn;
		}
		
		i = 0;
		int j = 0;
		int iMaxMatch = 0;
		while(pMaxPlate[i])
		{
			if (pMaxPlate[i]==pMinPlate[j])
			{
				j++;
			}
			else
			{
				j = 0;
			}
			if (j > iMaxMatch)
			{
				iMaxMatch = j;
			}
			i++;
		}
		iCount = iMaxMatch;
	}
	global_WriteLog("VPR_ComparePlate::结束");
	if (iCount >= iMaxLen)
	{
		return 0;
	}
	else if (iCount >= iMaxLen * 0.85)
	{
		return 1;
	}
	else if (iCount >= iMaxLen * 0.7)
	{
		return 2;
	}
	else
	{
		return 3;
	}
}

//取得二值图指定的二进制位
inline bool GetBin(
	const BYTE *pbBin,
	int iPosition )
{
	return ( pbBin[ iPosition >> 3 ] & ( 1 << ( iPosition & 0x07 ) ) ) > 0;
}


BOOL WINAPI VPR_ComparePlateBin(BYTE * pbBinImageIn,BYTE * pbBinImageOut)
{
	BOOL bSuccess=TRUE;
	if( pbBinImageIn == NULL || pbBinImageOut == NULL )
	{
		return FALSE;
	}
	global_WriteLog("VPR_ComparePlateBin::开始");
	const int TOTAL_PERFORMANCE = BIN_BYTE_COUNT * BIN_BIT_COUNT;
	const int THRESHOLD = 100;
	int iMaxPerformance = 0;
	for ( int iOffsetY = -1; iOffsetY <= 1; iOffsetY++ )
	{
		for ( int iOffsetX = -1; iOffsetX <= 1; iOffsetX++ )
		{
			int iPerformance = 0;
			for ( int i = 0; i < BIN_HEIGHT; i++ )
			{
				int iY = i + iOffsetY;
				if ( iY < 0 )
				{
					iY = 0;
				}
				else if ( iY >= BIN_HEIGHT )
				{
					iY = BIN_HEIGHT - 1;
				}
				for ( int j = 0; j < BIN_WIDTH; j++ )
				{
					int iX = j + iOffsetX;
					if ( iX < 0 )
					{
						iX = 0;
					}
					else if ( iX >= BIN_WIDTH )
					{
						iX = BIN_WIDTH - 1;
					}
					if ( GetBin( pbBinImageIn, iY * BIN_WIDTH + iX ) == GetBin( pbBinImageOut, i * BIN_WIDTH + j ) )
					{
						iPerformance++;
					}
				}
			}
			if ( iPerformance > iMaxPerformance )
			{
				iMaxPerformance = iPerformance;
			}
		}
	}
	if ( ( iMaxPerformance * 100 / ( BIN_BYTE_COUNT * BIN_BIT_COUNT ) ) < THRESHOLD )
	{
		bSuccess = FALSE;
	}
	global_WriteLog("VPR_ComparePlateBin::结束");
	return bSuccess;
}

BOOL WINAPI VPR_BinImage2BMP(
	int iCX,int iCY,BYTE* pbByteBinImage,char * pchBMPFileName)
{
	global_WriteLog("VPR_BinImage2BMP::开始");

	BOOL bSuccess=TRUE;
	int iByteBinImageLen = BIN_BYTE_COUNT;
	if ( iCX != BIN_WIDTH || iCY != BIN_HEIGHT || pbByteBinImage == NULL)
	{
		bSuccess = FALSE;
	}
	else
	{
		FILE *pfile = fopen( pchBMPFileName, "wb" );
		if (pfile == NULL)
		{
			bSuccess = FALSE;
		}
		else
		{
			int iStride = ((BIN_STRIDE + 3) >> 2) << 2;
			int iBmpDataSize = iStride * BIN_HEIGHT;
			
			BITMAPFILEHEADER bmfHdr;
			bmfHdr.bfType = 0x4d42; 
			bmfHdr.bfSize = iBmpDataSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD);
			bmfHdr.bfReserved1 = 0; 
			bmfHdr.bfReserved2 = 0; 
			bmfHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD); 

			BITMAPINFO *pInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD));
			pInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
			pInfo->bmiHeader.biWidth = BIN_WIDTH;
			pInfo->bmiHeader.biHeight = BIN_HEIGHT;
			pInfo->bmiHeader.biPlanes = 1; 
			pInfo->bmiHeader.biBitCount = 1; 
			pInfo->bmiHeader.biCompression = BI_RGB; 
			pInfo->bmiHeader.biSizeImage = 0;
			pInfo->bmiHeader.biXPelsPerMeter = 0; 
			pInfo->bmiHeader.biYPelsPerMeter = 0; 
			pInfo->bmiHeader.biClrUsed = 0; 
			pInfo->bmiHeader.biClrImportant = 0; 

			pInfo->bmiColors[0].rgbBlue = 0;
			pInfo->bmiColors[0].rgbGreen = 0;
			pInfo->bmiColors[0].rgbRed = 0;
			pInfo->bmiColors[0].rgbReserved = 0;
			pInfo->bmiColors[1].rgbBlue = 255;
			pInfo->bmiColors[1].rgbGreen = 255;
			pInfo->bmiColors[1].rgbRed = 255;
			pInfo->bmiColors[1].rgbReserved = 0;

			BYTE *pBmpData = (BYTE*)malloc(iBmpDataSize);
			memset(pBmpData, 0, iBmpDataSize);
			BYTE *pSrc = pbByteBinImage;
			BYTE *pDest = pBmpData;
			for (int i = 0; i < BIN_HEIGHT; i++)
			{
				memcpy(pDest, pSrc, BIN_STRIDE);
				pDest += iStride;
				pSrc += BIN_STRIDE;
			}

			fwrite(&bmfHdr, sizeof(BITMAPFILEHEADER), 1, pfile);
			fwrite(pInfo, sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD), 1, pfile);
			fwrite(pBmpData, sizeof(BYTE), iBmpDataSize, pfile);
			fclose(pfile);

			free(pBmpData);
			free(pInfo);
		}
	}
	global_WriteLog("VPR_BinImage2BMP::结束");
	return bSuccess;
}

BOOL WINAPI VPR_SyncTime(UINT uID)
{
	BOOL fRet = FALSE;
	int iPos = -1;
	global_WriteLog("VPR_SyncTime::开始");
	EnterCriticalSection( &g_csVPRCtrlSync );
	for(int i = 0; i < MAX_VPRCTRL; ++i)
	{
		if( g_rgpVPR[i] != NULL && g_rgpVPR[i]->GetId() == uID )
		{
			iPos = i;
			break;
		}
	}
	if( iPos != -1 )
	{
		fRet =g_rgpVPR[iPos]->SyncTime();
	}
	LeaveCriticalSection( &g_csVPRCtrlSync );

	global_WriteLog("VPR_SyncTime::结束");
	return fRet;
}