#ifndef __MULTIHV_API_H__
#define __MULTIHV_API_H__

#include "hv_api_base.h"

// 描述:		发出连接多个车牌识别器的请求，立刻返回，不等待所有车牌识别器连接成功；
//				是否连接成功，通过调用MultiHvIsConnected函数检查；
// 参数:		ppBoxAddr			(in)需要连接的车牌识别器地址数组；
//				nBoxCount			(in)需要连接的车牌识别器个数；
//				phMultiHv			(out)返回的的连接句柄；
// 返回值:		S_OK				操作成功；
//				E_POINTER			传入非法指针；
//				E_INVALIDARG		参数中有非法数据；
//				E_FAIL				未知的错误导致操作失败。
HV_API HRESULT __cdecl OpenMultiHv(char **ppBoxAddr, int nBoxCount, HV_HANDLE *phMultiHv);

// 描述:		关闭由OpenMultiHv得到的HV_HANDLE；
// 参数:		hMultiHv			(in)需要关闭的句柄，必须是由OpenMultiHv获得的；
// 返回值:		S_OK				操作成功；
//				E_HANDLE			hMultiHv参数是一个非法的句柄；
//				E_FAIL				未知的错误导致操作失败。
HV_API HRESULT __cdecl CloseMultiHv(HV_HANDLE hMultiHv);

typedef struct _HV_CONNECTION_STATUS {
	char m_szBoxAddr[16];			//调用OpenMultiHv时指定的车牌识别器地址；
	int m_iStatus;					//车牌识别器的连接状态，取值参照HvIsConnected说明；
} HV_CONNECTION_STATUS;

// 描述:		检查所有连接的识别器的连接状态；
// 参数:		hMultiHv			(in)需要关闭的句柄，必须是由OpenMultiHv获得的；
//				pConnStatus			(out)保存车牌识别器连接状态的数组；
//				nBufferCount		(in)指定pConnStatus数组的元素个数；
//				piStatusCount		(out)返回实际打开的车牌识别器个数；
// 返回值:		S_OK				操作成功；
//				E_HANDLE			hMultiHv参数是一个非法的句柄;
//				E_POINTER			传入非法指针；
//				E_INVALIDARG		参数中有非法数据；
//				E_FAIL				未知的错误导致操作失败。
HV_API HRESULT __cdecl MultiHvIsConnected(HV_HANDLE hMultiHv, HV_CONNECTION_STATUS *pConnStatus, int nBufferCount, int *piStatusCount);

// 描述:		设置一个视频流的一种数据类型的回调函数；
//				在数据连接上收到的任何数据将通过回调函数的形式传递给应用程序的上层；
// 参数:		hMultiHv			(in)由OpenMultiHv()函数打开的句柄；
//				pFunction			(in)回调函数的函数指针，回调函数的声明在hv_api_base.h中；
//									该参数是一个函数指针, 当数据连接上收到数据时,将调用该函数；
//									如果该参数为NULL, 则表示关闭该回调功能, 数据连接上的该类型数据流也随即被关闭；
//									如果该参数为非NULL, 则表示打开该回调功能, 数据连接上的该类型数据流随即被打开；
//				pFirstParameter		(in)回调pFunction时传递的第一个参数；
//									该参数一般为一个对象的指针, 以便于在回调函数中直接访问一个对象；
//									如果用户在回调函数中不准备使用该参数,则该参数可以为NULL；
//				wVideoID			(in)表示视频ID的2字节的无符号整数(缺省为0)；
//				wStream				(in)表示数据类型的2字节的无符号整数(取值在hv_opt_base.h中定义)；
// 返回值:		S_OK				操作成功；
//				E_HANDLE			hMultiHv参数是一个非法的句柄；
//				E_FAIL				未知的错误导致操作失败。
HV_API HRESULT __cdecl SetMultiHvCallBack(HV_HANDLE hMultiHv, void *pFunction, void *pFirstParameter, WORD wVideoID, WORD wStream);

// 描述:		读取车牌附加信息（车速、车牌宽度、车牌亮度等等）。
// 参数:		hMultiHv			(in)由OpenMultiHv()函数打开的句柄；
//				ppszPlateInfo		(out)返回车牌附加信息指针；
// 返回值:		S_OK				操作成功；
//				E_HANDLE			hMultiHv参数是一个非法的句柄；
//				E_POINTER			传入非法指针；
//				E_FAIL				未知的错误导致操作失败。
HV_API HRESULT __cdecl MultiHv_GetPlateInfo(HV_HANDLE hMultiHv, char** ppszPlateInfo);

// 描述:		强制出大图；
// 参数:		hMultiHv			(in)由OpenMultiHv()函数打开的句柄；
// 返回值:		S_OK				操作成功；
//				E_HANDLE			hMultiHv参数是一个非法的句柄；
//				E_FAIL				未知的错误导致操作失败。
HV_API HRESULT __cdecl MultiHv_ForceSend(HV_HANDLE hMultiHv);

#endif
