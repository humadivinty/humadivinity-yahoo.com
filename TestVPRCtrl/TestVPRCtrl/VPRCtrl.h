// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VPRCTRL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VPRCTRL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef _INCLUDE_VPRCTRL_H_
#define _INCLUDE_VPRCTRL_H_

#ifdef VPRCTRL_EXPORTS
#define VPRCTRL_API __declspec(dllexport)
#else
#define VPRCTRL_API __declspec(dllimport)
#endif


VPRCTRL_API BOOL WINAPI VPR_Init(UINT uID,char * pchPort,HWND hWndHandle,UINT uMsg,BOOL bProvideBigPic=TRUE);

VPRCTRL_API BOOL WINAPI VPR_InitEx(UINT uID,char * pchPort,DWORD idThread,UINT uMsg,BOOL bProvideBigPic=TRUE);

VPRCTRL_API BOOL WINAPI VPR_Quit(UINT uID);

VPRCTRL_API BOOL WINAPI VPR_Capture(UINT uID);

VPRCTRL_API int WINAPI VPR_CaptureEx(UINT uID);

VPRCTRL_API BOOL WINAPI VPR_GetVehicleInfo(
	UINT uID,char * pchPlate,int* piByteBinImagLen,BYTE * pbByteBinImage,
	int * piJpegImageLen,BYTE * pbByteJpegImage);

VPRCTRL_API BOOL WINAPI VPR_GetVehicleInfoEx(
	UINT uID,char * pchPlate,int* piByteBinImagLen,BYTE * pbByteBinImage,
	int * piJpegImageLen,BYTE * pbByteJpegImage, UINT *puSpeed);
VPRCTRL_API BOOL WINAPI VPR_GetVehicleInfoEx2(
	UINT uID,char * pchPlate,int* piByteBinImagLen,BYTE * pbByteBinImage,
	int * piJpegImageLen,BYTE * pbByteJpegImage,UINT *puSpeed, 
	DWORD64* pdwTimeMs);

VPRCTRL_API int WINAPI VPR_CheckStatus(UINT uID,char * pchVprDevStatus);

VPRCTRL_API int WINAPI VPR_ComparePlate(
										BOOL bFullCompare,int iRule,char * pchPlateIn,char * pchPlateOut);

VPRCTRL_API BOOL WINAPI VPR_ComparePlateBin(BYTE * pbBinImageIn,BYTE * pbBinImageOut);

VPRCTRL_API BOOL WINAPI VPR_BinImage2BMP(
	int iCX,int iCY,BYTE* pbByteBinImage,char * pchBMPFileName);
VPRCTRL_API BOOL WINAPI VPR_SyncTime(UINT uID);

#endif