
// 北京信路威车牌识别器对外开发接口

// 关于本API接口函数调用的注意事项:
// 由于本API函数内部实现中只能提供有限的保护,任何句柄和指针参数的错误传递都有可能导致后台软件的崩溃;

#ifndef _HV_API_BASE_H_
#define _HV_API_BASE_H_

#include "hv_opt_base.h"

#ifdef HV_API_EXPORTS
#define HV_API extern "C" __declspec(dllexport)
#else
#define HV_API extern "C" __declspec(dllimport)
#endif

typedef void * HV_HANDLE;
#define E_HV_INVALID_HANDLE NULL

//====================================================================================================

// 描述:  打开一个视频处理系统句柄; 在对视频处理系统作任何操作之前, 
//        必须使用该函数打开视频处理系统取得一个句柄后,
//        才可使用该句柄对视频处理系统进行操作;
// 参数:  pIP   视频处理系统的IP地址，以'\0'结尾; 
// 返回值: 当成功地打开一个视频处理系统连接时,将返回一个的视频处理系统的句柄; 
//         当打开失败时,函数返回E_HV_INVALID_HANDLE;
// 注意: 1. 此函数返回的是一个指针类型,用户应妥善保存该返回值,以用于后序的操作.
HV_API HV_HANDLE __cdecl OpenHv( const PSTR pcIP );


// 描述:  关闭一个视频处理系统句柄;
// 参数: hHandle  由OpenHv()函数打开的句柄;
//                此参数必须是OpenHv()函数的返回值, 否则将导致不可预想的后果;
//                如果此参数为E_HV_INVALID_HANDLE, 函数将不作任何操作,并且返回E_HANDLE;
// 返回值: 返回S_OK, 表示操作成功,
//         返回E_HANDLE, 表示hHandle参数是一个非法的句柄;
//         返回E_FAIL, 表示未知的错误导致操作失败;
// 注意: 此函数成功调用后, 将关闭与视频处理系统的所有连接,并释放所有与该视频处理系统有关的所有资源;
HV_API HRESULT __cdecl CloseHv( HV_HANDLE hHandle );

// 描述:  关闭指定IP的视频处理系统;
// 参数: pszIP	需要关闭的视频处理系统的IP;
// 返回值: 返回S_OK, 表示操作成功,
//			返回S_FALSE,表示没有找到该IP的句柄，
//         返回E_FAIL, 表示未知的错误导致操作失败;
// 注意: 此函数成功调用后, 将关闭与视频处理系统的所有连接,并释放所有与该视频处理系统有关的所有资源;
HV_API HRESULT __cdecl CloseIP( char *pszIP );

const INT32 HV_CONNECTED = 0;	//连接正常
const INT32 HV_CLOSED = 1;		//连接关闭
const INT32 HV_THREAD_EXIT = 2;	//线程退出

// 描述:  获取连接状态;
// 参数: hHandle  由OpenHv()函数打开的句柄;
//                此参数必须是OpenHv()函数的返回值, 否则将导致不可预想的后果;
//                如果此参数为E_HV_INVALID_HANDLE, 函数将不作任何操作,并且返回E_HANDLE;
//		 piStatus 输出连接状态 
// 返回值: 返回S_OK, 表示操作成功,
//         返回E_HANDLE, 表示hHandle参数是一个非法的句柄;
//         返回E_FAIL, 表示未知的错误导致操作失败;
HV_API HRESULT __cdecl HvIsConnected( HV_HANDLE hHandle, INT32 *piStatus );

// 描述:  设置一个视频流的一种数据类型的回调函数; 
//        在数据连接上收到的任何数据将通过回调函数的形式传递给应用程序的上层;
// 参数: hHandle           由OpenHv()函数打开的句柄;
//                         此参数必须是OpenHv()函数的返回值, 否则将导致不可预想的后果;
//                         如果此参数为E_HV_INVALID_HANDLE, 函数将不作任何操作,并且返回E_HANDLE;
//       pFunction         回调函数的函数指针;
//                         该参数是一个函数指针, 当数据连接上收到数据时,将调用该函数;
//                         如果该参数为NULL, 则表示关闭该回调功能, 数据连接上的该类型数据流也随即被关闭;
//                         如果该参数为非NULL, 则表示打开该回调功能, 数据连接上的该类型数据流随即被打开;
//       pFirstParameter   回调pFunction时传递的第一个参数;
//                         该参数一般为一个对象的指针, 以便于在回调函数中直接访问一个对象;
//                         如果用户在回调函数中不准备使用该参数,则该参数可以为NULL;
//       wVideoID		   表示视频ID的2字节的无符号整数;
//		 wStream		   表示数据类型的2字节的无符号整数;
// 返回值: 返回S_OK, 表示操作成功,
//         返回E_HANDLE, 表示hHandle参数是一个非法的句柄;
//         返回E_FAIL, 表示未知的错误导致操作失败;
HV_API HRESULT __cdecl SetHvCallBack( HV_HANDLE hHandle, void *pFunction, void *pFirstParameter, WORD wVideoID, WORD wStream );

// 描述:  车牌号码数据类型的回调函数指针原型;
// 参数: pFirstParameter  回调时传递的第一个参数;
//                        该参数的值即为SetHvCallBack()调用中pFirstParameter的值;
//       dwCarID          车辆ID;
//       pPlateNo         车牌号码字符串; 该参数为以'\0'结尾的字符串指针;
//       dwTimeMs		  时间(1970-1-1 0:0:0以来的毫秒数);
// 返回值: 目前本接口的内部实现中不对此返回值作任何处理, 该返回值的存在只是为了将来的用途; 
//         但建议函数在成功处理了该数据之后返回0,否则返回-1;
// 注意:   pPlateNo这个指针所指向的内存在函数返回时即被释放, 因此如果用户希望函数返回后还使用
//         该指针所指的内容, 则必须把该内容复制下来;
typedef int ( __cdecl *PLATE_NO_CALLBACK )( void *pFirstParameter, DWORD32 dwCarID, PSTR pcPlateNo, DWORD64 dw64TimeMs );

// 描述:  车辆全景图数据类型的回调函数指针原型;
// 参数: pFirstParameter  回调时传递的第一个参数;
//       dwCarID          车辆ID;
//       wImageWidth      图像的宽度;
//       wImageHigh       图像的高度;
//		 bType			  图像类型;
//		 wSize 			  数据大小;
//       pbImage		  图像数据的指针;
//       wImageID	      图像的ID号，表示得到的是哪一张图片;
//       wHighImgFlag		 高清图片标志,如果该变量高8位值是0xFF00, 则该变量低8位值为图片大小的高位值
//						即该图片数据大小为(int(wHighImgFlag & 0x00FF) << 16) + wSize
//       wPlateWidth	  表示该图象中检测到的车牌的宽度;
//       wPlateHigh       表示该图象中检测到的车牌的高度;
//       dwTimeMs         时间(1970-1-1 0:0:0以来的毫秒数);
// 返回值: 目前本接口的内部实现中不对此返回值作任何处理, 该返回值的存在只是为了将来的用途; 
//         但建议函数在成功处理了该数据之后返回0,否则返回-1;
// 注意:   pbImage这个指针所指向的内存在函数返回时即被释放, 因此如果用户希望函数返回后还使用
//         该指针所指的内容, 则必须把该内容复制下来;
typedef int ( __cdecl *BIG_IMAGE_CALLBACK )( void *pFirstParameter, DWORD32 dwCarID, WORD wImageWidth, WORD wImageHigh, BYTE bType, WORD wSize,
               PBYTE pbImage, WORD wImageID, WORD wHighImgFlag, WORD wPlateWidth, WORD wPlateHigh, DWORD64 dwTimeMs );


// 描述:  车牌小图像数据类型的回调函数指针原型;
// 参数: pFirstParameter  回调时传递的第一个参数;
//                        该参数的值即为SetHvCallBack()调用中pFirstParameter的值;
//       dwCarID          车辆ID;
//       wImageWidth      图像的宽度;
//       wImageHigh       图像的高度;
//		 bType			  图像类型;
//		 wSize 			  数据大小;
//       pbImage		  图像数据的指针;
//       dwTimeMs         时间(1970-1-1 0:0:0以来的毫秒数);
// 返回值: 目前本接口的内部实现中不对此返回值作任何处理, 该返回值的存在只是为了将来的用途; 
//         但建议函数在成功处理了该数据之后返回0,否则返回-1;
// 注意:   pbImage这个指针所指向的内存在函数返回时即被释放, 因此如果用户希望函数返回后还使用
//         该指针所指的内容, 则必须把该内容复制下来;
typedef int ( __cdecl *SMALL_IMAGE_CALLBACK )( void *pFirstParameter, DWORD32 dwCarID, WORD wImageWidth, WORD wImageHigh, BYTE bType, WORD wSize,
               BYTE *pbImage, DWORD64 dwTimeMs );

// 描述:  车牌小图像二值化数据类型的回调函数指针原型;
// 参数: pFirstParameter  回调时传递的第一个参数;
//                        该参数的值即为SetHvCallBack()调用中pFirstParameter的值;
//       dwCarID          车辆ID;
//       wImageWidth      图像的宽度;
//       wImageHigh       图像的高度;
//		 bType			  图像类型;
//		 wSize 			  数据大小;
//       pbImage		  图像数据的指针;
//       dwTimeMs         时间(1970-1-1 0:0:0以来的毫秒数);
// 返回值: 目前本接口的内部实现中不对此返回值作任何处理, 该返回值的存在只是为了将来的用途; 
//         但建议函数在成功处理了该数据之后返回0,否则返回-1;
// 注意:   pbImage这个指针所指向的内存在函数返回时即被释放, 因此如果用户希望函数返回后还使用
//         该指针所指的内容, 则必须把该内容复制下来;
typedef int ( __cdecl *BINARY_IMAGE_CALLBACK )( void *pFirstParameter, DWORD32 dwCarID, WORD wImageWidth, WORD wImageHigh, BYTE bType, WORD wSize,
               BYTE *pbImage, DWORD64 dwTimeMs );

// 描述:  实时视频数据类型的回调函数指针原型;
// 参数: pFirstParameter  回调时传递的第一个参数;
//                        该参数的值即为SetHvCallBack()调用中pFirstParameter的值;
//		 wVideoID		  视频流编号;
//		 dwSize 			  数据大小;
//       pbImage		  图像数据的指针;
// 返回值: 目前本接口的内部实现中不对此返回值作任何处理, 该返回值的存在只是为了将来的用途; 
//         但建议函数在成功处理了该数据之后返回0,否则返回-1;
// 注意:   pbImage这个指针所指向的内存在函数返回时即被释放, 因此如果用户希望函数返回后还使用
//         该指针所指的内容, 则必须把该内容复制下来;
typedef int ( __cdecl *VIDEO_CALLBACK )( void *pFirstParameter, WORD wVideoID, DWORD dwSize, BYTE *pbImage );

// 描述:  车辆信息开始接收信号数据类型的回调函数指针原型;
// 参数: pFirstParameter	回调时传递的第一个参数;
//       dwCarID			车辆ID;
// 返回值: 目前本接口的内部实现中不对此返回值作任何处理, 该返回值的存在只是为了将来的用途; 
//         但建议函数在成功处理了该数据之后返回0,否则返回-1;
//注意：   系统在接收一次识别结果前调用本函数

typedef int ( __cdecl *CARINFO_BEGIN_CALLBACK )( void *pFirstParameter, DWORD32 dwCarID );

// 描述:  车辆信息结束接收信号数据类型的回调函数指针原型;
// 参数: pFirstParameter	回调时传递的第一个参数;
//       dwCarID			车辆ID;
// 返回值: 目前本接口的内部实现中不对此返回值作任何处理, 该返回值的存在只是为了将来的用途; 
//         但建议函数在成功处理了该数据之后返回0,否则返回-1;
//注意:    系统在接收完一次识别结果后调用本函数

typedef int ( __cdecl *CARINFO_END_CALLBACK )( void *pFirstParameter, DWORD32 dwCarID );

// 描述:	读取当前接收到的车牌序号和车牌字符串
// 参数:	hHV			输入由OpenHv()函数打开的句柄;
//			pdwCarID	返回当前接收到的车辆序号;
//			pszPlate	返回当前接收到的车牌字符串，没有收到车牌的时候返回"";
//			iPlateLen	输入车牌字符串接收缓冲区长度;
// 返回值: 返回S_OK, 表示操作成功;
//         返回E_HANDLE, 表示hHandle参数是一个非法的句柄;
//         返回E_POINTER, 参数中包含有非法的指针;
//         返回E_FAIL, 表示未知的错误导致操作失败;

HV_API HRESULT __cdecl HVGetResult( HV_HANDLE hHV, DWORD32 *pdwCarID, LPSTR pszPlate, int iPlateLen );

// 描述:  二进制数据流类型的回调函数指针原型;
// 参数: pFirstParameter    回调时传递的第一个参数;
//       iLen               二进制数据流的长度;
//       pString            指向二进制数据流的指针;
// 返回值: 建议函数在成功处理了该数据之后返回0,否则返回-1;
// 注意:   pString这个指针所指向的内存在函数返回时即被释放, 因此如果用户希望函数返回后还使用
//         该指针所指的内容, 则必须把该内容复制下来;
//     	   本函数在识别器设置为输出检测信息时才能取得数据
typedef int ( __cdecl *STREAM_CALLBACK )( void *pFirstParameter, INT32 iLen, PSTR pString );

// 描述:	搜索局域网内所有的视频处理设备，返回设备数目
// 参数:	pdwMaxCount  此参数值为搜索到的视频处理系统的数目;
// 返回值: 返回S_OK, 表示操作成功;
//         返回E_FAIL, 表示未知的错误导致操作失败;
HV_API HRESULT __cdecl SearchHVDeviceCount( DWORD32 *pdwCount );

// 描述:	根据索引查询设备ip地址
// 参数:	iIndex			指定查询的索引,该索引不大于视频处理系统的最大数目;
//			pdw64MacAddr	返回设备以太网地址
//			pdwIP			返回设备IP地址
//			pdwMask			返回设备IP掩码
//			pdwGateway		返回设备IP网关地址
// 返回值:	返回S_OK, 表示操作成功;
//			返回E_FAIL, 表示未知的错误导致操作失败;
// 注意:	在调用本函数之前先调用SearchHVDeviceCount搜索设备
HV_API HRESULT __cdecl GetHVDeviceAddr( int iIndex, DWORD64 *pdw64MacAddr, DWORD32 *pdwIP, DWORD32 *pdwMask, DWORD32 *pdwGateway );

// 描述:	根据设备的以太网地址搜索设备并修改设备的IP地址
// 参数:	dw64MacAddr		输入要搜索的设备以太网地址
//			dwIP			输入要设置的设备IP地址
//			dwMask			输入要设置的设备IP掩码
//			dwGateway		输入要设置的设备IP网关地址
// 返回值:	返回S_OK, 表示操作成功;
//			返回S_FALSE, 表示需验证;
//			返回E_FAIL, 表示未知的错误导致操作失败;
// 注意:	设备的以太网地址通过调用SearchHVDeviceCount和GetHVDeviceAddr得到
HV_API HRESULT __cdecl SetHvIPFromMac( DWORD64 dw64MacAddr, DWORD32 dwIP, DWORD32 dwMask, DWORD32 dwGateway );

// 描述:	转换YUV数据到RGB
// 参数:	pbDest			输出RGB数据的缓冲区指针;
//			pbSrc			输入YUV数据的缓冲区指针;
//			iSrcWidth		图像宽度;
//			iSrcHeight		图像高度;
//			iBGRStride		RGB数据每行的步长;
// 返回值:  返回S_OK, 表示操作成功,
//          返回E_POINTER, 参数中包含有非法的指针;
//          返回E_FAIL, 表示未知的错误导致操作失败;
HV_API HRESULT __cdecl Yuv2Rgb(
	BYTE *pbDest,
	BYTE *pbSrc,
	int iSrcWidth,
	int iSrcHeight,
	int iBGRStride
);

// 描述:	转换YUV数据成BMP格式
// 参数:	pbDest			输出BMP数据的缓冲区指针;
//			iDestBufLen		输出缓冲区大小
//			piDestLen		实际输出数据大小
//			pbSrc			输入YUV数据的缓冲区指针;
//			iSrcWidth		图像宽度;
//			iSrcHeight		图像高度;
// 返回值:  返回S_OK, 表示操作成功,
//          返回E_POINTER, 参数中包含有非法的指针;
//          返回E_FAIL, 表示未知的错误导致操作失败;
HV_API HRESULT __cdecl Yuv2BMP(
	BYTE *pbDest,
	int iDestBufLen,
	int *piDestLen,
	BYTE *pbSrc,
	int iSrcWidth,
	int iSrcHeight
);

// 描述:	转换YUV数据成JPG格式
// 参数:	pbDest			输出JPG数据的缓冲区指针;
//			iDestBufLen		输出缓冲区大小
//			piDestLen		实际输出数据大小
//			pbSrc			输入YUV数据的缓冲区指针;
//			iSrcWidth		图像宽度;
//			iSrcHeight		图像高度;
// 返回值:  返回S_OK, 表示操作成功,
//          返回E_POINTER, 参数中包含有非法的指针;
//          返回E_FAIL, 表示未知的错误导致操作失败;
HV_API HRESULT __cdecl Yuv2Jpg(
	BYTE *pbDest,
	int iDestBufLen,
	int *piDestLen,
	BYTE *pbSrc,
	int iSrcWidth,
	int iSrcHeight
);

// 描述:	强制识别器输出识别结果和大图
// 参数:	hHandle	 由OpenHv()函数打开的句柄;
// 返回值:  返回S_OK, 表示操作成功,
//          返回E_POINTER, 参数中包含有非法的指针;
//          返回E_FAIL, 表示未知的错误导致操作失败;
//注意：	本函数在识别器设置为可触发时方可生效
HV_API HRESULT __cdecl HV_ForceSend( HV_HANDLE hHandle );


// 描述:	读取车牌附加信息(根据实际情况包括车速，车牌位置，亮度等)
// 参数:	hHandle			由OpenHv()函数打开的句柄;
//			ppszPlateInfo	输入指向车牌附加信息的指针的地址
//			示例:
//           char *pInfo;//指向附加信息
//           HV_GetPlateInfo(hHandle,&pInfo);          
// 返回值:  返回S_OK, 表示操作成功,
//          返回E_POINTER, 参数中包含有非法的指针;
//          返回E_FAIL, 表示未知的错误导致操作失败;
//注意：	本函数在识别器设置为输出附加信息时才能取得数据
HV_API HRESULT __cdecl HV_GetPlateInfo(  HV_HANDLE hHandle, char** ppszPlateInfo );

// 描述:	解析用户串口数据
// 参数:	hHandle			由OpenHv()函数打开的句柄;
//		ppszPlateInfo		输入串口数据的首地址
//		dwDataSize		输入所要处理数据的长度
// 返回值:  返回S_OK, 表示操作成功,
//          返回E_POINTER, 参数中包含有非法的指针;
//          返回E_FAIL, 表示未知的错误导致操作失败;
//注意：	本函数在成功执行后会触发结果回调函数
HV_API HRESULT __cdecl HV_ProcessUserData( HV_HANDLE hHandle, PBYTE pbData, DWORD32 dwDataSize );
#endif
