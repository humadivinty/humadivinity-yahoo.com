
// ������·������ʶ�������⿪���ӿ�

// ���ڱ�API�ӿں������õ�ע������:
// ���ڱ�API�����ڲ�ʵ����ֻ���ṩ���޵ı���,�κξ����ָ������Ĵ��󴫵ݶ��п��ܵ��º�̨����ı���;

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

// ����:  ��һ����Ƶ����ϵͳ���; �ڶ���Ƶ����ϵͳ���κβ���֮ǰ, 
//        ����ʹ�øú�������Ƶ����ϵͳȡ��һ�������,
//        �ſ�ʹ�øþ������Ƶ����ϵͳ���в���;
// ����:  pIP   ��Ƶ����ϵͳ��IP��ַ����'\0'��β; 
// ����ֵ: ���ɹ��ش�һ����Ƶ����ϵͳ����ʱ,������һ������Ƶ����ϵͳ�ľ��; 
//         ����ʧ��ʱ,��������E_HV_INVALID_HANDLE;
// ע��: 1. �˺������ص���һ��ָ������,�û�Ӧ���Ʊ���÷���ֵ,�����ں���Ĳ���.
HV_API HV_HANDLE __cdecl OpenHv( const PSTR pcIP );


// ����:  �ر�һ����Ƶ����ϵͳ���;
// ����: hHandle  ��OpenHv()�����򿪵ľ��;
//                �˲���������OpenHv()�����ķ���ֵ, ���򽫵��²���Ԥ��ĺ��;
//                ����˲���ΪE_HV_INVALID_HANDLE, �����������κβ���,���ҷ���E_HANDLE;
// ����ֵ: ����S_OK, ��ʾ�����ɹ�,
//         ����E_HANDLE, ��ʾhHandle������һ���Ƿ��ľ��;
//         ����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;
// ע��: �˺����ɹ����ú�, ���ر�����Ƶ����ϵͳ����������,���ͷ����������Ƶ����ϵͳ�йص�������Դ;
HV_API HRESULT __cdecl CloseHv( HV_HANDLE hHandle );

// ����:  �ر�ָ��IP����Ƶ����ϵͳ;
// ����: pszIP	��Ҫ�رյ���Ƶ����ϵͳ��IP;
// ����ֵ: ����S_OK, ��ʾ�����ɹ�,
//			����S_FALSE,��ʾû���ҵ���IP�ľ����
//         ����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;
// ע��: �˺����ɹ����ú�, ���ر�����Ƶ����ϵͳ����������,���ͷ����������Ƶ����ϵͳ�йص�������Դ;
HV_API HRESULT __cdecl CloseIP( char *pszIP );

const INT32 HV_CONNECTED = 0;	//��������
const INT32 HV_CLOSED = 1;		//���ӹر�
const INT32 HV_THREAD_EXIT = 2;	//�߳��˳�

// ����:  ��ȡ����״̬;
// ����: hHandle  ��OpenHv()�����򿪵ľ��;
//                �˲���������OpenHv()�����ķ���ֵ, ���򽫵��²���Ԥ��ĺ��;
//                ����˲���ΪE_HV_INVALID_HANDLE, �����������κβ���,���ҷ���E_HANDLE;
//		 piStatus �������״̬ 
// ����ֵ: ����S_OK, ��ʾ�����ɹ�,
//         ����E_HANDLE, ��ʾhHandle������һ���Ƿ��ľ��;
//         ����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;
HV_API HRESULT __cdecl HvIsConnected( HV_HANDLE hHandle, INT32 *piStatus );

// ����:  ����һ����Ƶ����һ���������͵Ļص�����; 
//        �������������յ����κ����ݽ�ͨ���ص���������ʽ���ݸ�Ӧ�ó�����ϲ�;
// ����: hHandle           ��OpenHv()�����򿪵ľ��;
//                         �˲���������OpenHv()�����ķ���ֵ, ���򽫵��²���Ԥ��ĺ��;
//                         ����˲���ΪE_HV_INVALID_HANDLE, �����������κβ���,���ҷ���E_HANDLE;
//       pFunction         �ص������ĺ���ָ��;
//                         �ò�����һ������ָ��, �������������յ�����ʱ,�����øú���;
//                         ����ò���ΪNULL, ���ʾ�رոûص�����, ���������ϵĸ�����������Ҳ�漴���ر�;
//                         ����ò���Ϊ��NULL, ���ʾ�򿪸ûص�����, ���������ϵĸ������������漴����;
//       pFirstParameter   �ص�pFunctionʱ���ݵĵ�һ������;
//                         �ò���һ��Ϊһ�������ָ��, �Ա����ڻص�������ֱ�ӷ���һ������;
//                         ����û��ڻص������в�׼��ʹ�øò���,��ò�������ΪNULL;
//       wVideoID		   ��ʾ��ƵID��2�ֽڵ��޷�������;
//		 wStream		   ��ʾ�������͵�2�ֽڵ��޷�������;
// ����ֵ: ����S_OK, ��ʾ�����ɹ�,
//         ����E_HANDLE, ��ʾhHandle������һ���Ƿ��ľ��;
//         ����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;
HV_API HRESULT __cdecl SetHvCallBack( HV_HANDLE hHandle, void *pFunction, void *pFirstParameter, WORD wVideoID, WORD wStream );

// ����:  ���ƺ����������͵Ļص�����ָ��ԭ��;
// ����: pFirstParameter  �ص�ʱ���ݵĵ�һ������;
//                        �ò�����ֵ��ΪSetHvCallBack()������pFirstParameter��ֵ;
//       dwCarID          ����ID;
//       pPlateNo         ���ƺ����ַ���; �ò���Ϊ��'\0'��β���ַ���ָ��;
//       dwTimeMs		  ʱ��(1970-1-1 0:0:0�����ĺ�����);
// ����ֵ: Ŀǰ���ӿڵ��ڲ�ʵ���в��Դ˷���ֵ���κδ���, �÷���ֵ�Ĵ���ֻ��Ϊ�˽�������;; 
//         �����麯���ڳɹ������˸�����֮�󷵻�0,���򷵻�-1;
// ע��:   pPlateNo���ָ����ָ����ڴ��ں�������ʱ�����ͷ�, �������û�ϣ���������غ�ʹ��
//         ��ָ����ָ������, �����Ѹ����ݸ�������;
typedef int ( __cdecl *PLATE_NO_CALLBACK )( void *pFirstParameter, DWORD32 dwCarID, PSTR pcPlateNo, DWORD64 dw64TimeMs );

// ����:  ����ȫ��ͼ�������͵Ļص�����ָ��ԭ��;
// ����: pFirstParameter  �ص�ʱ���ݵĵ�һ������;
//       dwCarID          ����ID;
//       wImageWidth      ͼ��Ŀ��;
//       wImageHigh       ͼ��ĸ߶�;
//		 bType			  ͼ������;
//		 wSize 			  ���ݴ�С;
//       pbImage		  ͼ�����ݵ�ָ��;
//       wImageID	      ͼ���ID�ţ���ʾ�õ�������һ��ͼƬ;
//       wHighImgFlag		 ����ͼƬ��־,����ñ�����8λֵ��0xFF00, ��ñ�����8λֵΪͼƬ��С�ĸ�λֵ
//						����ͼƬ���ݴ�СΪ(int(wHighImgFlag & 0x00FF) << 16) + wSize
//       wPlateWidth	  ��ʾ��ͼ���м�⵽�ĳ��ƵĿ��;
//       wPlateHigh       ��ʾ��ͼ���м�⵽�ĳ��Ƶĸ߶�;
//       dwTimeMs         ʱ��(1970-1-1 0:0:0�����ĺ�����);
// ����ֵ: Ŀǰ���ӿڵ��ڲ�ʵ���в��Դ˷���ֵ���κδ���, �÷���ֵ�Ĵ���ֻ��Ϊ�˽�������;; 
//         �����麯���ڳɹ������˸�����֮�󷵻�0,���򷵻�-1;
// ע��:   pbImage���ָ����ָ����ڴ��ں�������ʱ�����ͷ�, �������û�ϣ���������غ�ʹ��
//         ��ָ����ָ������, �����Ѹ����ݸ�������;
typedef int ( __cdecl *BIG_IMAGE_CALLBACK )( void *pFirstParameter, DWORD32 dwCarID, WORD wImageWidth, WORD wImageHigh, BYTE bType, WORD wSize,
               PBYTE pbImage, WORD wImageID, WORD wHighImgFlag, WORD wPlateWidth, WORD wPlateHigh, DWORD64 dwTimeMs );


// ����:  ����Сͼ���������͵Ļص�����ָ��ԭ��;
// ����: pFirstParameter  �ص�ʱ���ݵĵ�һ������;
//                        �ò�����ֵ��ΪSetHvCallBack()������pFirstParameter��ֵ;
//       dwCarID          ����ID;
//       wImageWidth      ͼ��Ŀ��;
//       wImageHigh       ͼ��ĸ߶�;
//		 bType			  ͼ������;
//		 wSize 			  ���ݴ�С;
//       pbImage		  ͼ�����ݵ�ָ��;
//       dwTimeMs         ʱ��(1970-1-1 0:0:0�����ĺ�����);
// ����ֵ: Ŀǰ���ӿڵ��ڲ�ʵ���в��Դ˷���ֵ���κδ���, �÷���ֵ�Ĵ���ֻ��Ϊ�˽�������;; 
//         �����麯���ڳɹ������˸�����֮�󷵻�0,���򷵻�-1;
// ע��:   pbImage���ָ����ָ����ڴ��ں�������ʱ�����ͷ�, �������û�ϣ���������غ�ʹ��
//         ��ָ����ָ������, �����Ѹ����ݸ�������;
typedef int ( __cdecl *SMALL_IMAGE_CALLBACK )( void *pFirstParameter, DWORD32 dwCarID, WORD wImageWidth, WORD wImageHigh, BYTE bType, WORD wSize,
               BYTE *pbImage, DWORD64 dwTimeMs );

// ����:  ����Сͼ���ֵ���������͵Ļص�����ָ��ԭ��;
// ����: pFirstParameter  �ص�ʱ���ݵĵ�һ������;
//                        �ò�����ֵ��ΪSetHvCallBack()������pFirstParameter��ֵ;
//       dwCarID          ����ID;
//       wImageWidth      ͼ��Ŀ��;
//       wImageHigh       ͼ��ĸ߶�;
//		 bType			  ͼ������;
//		 wSize 			  ���ݴ�С;
//       pbImage		  ͼ�����ݵ�ָ��;
//       dwTimeMs         ʱ��(1970-1-1 0:0:0�����ĺ�����);
// ����ֵ: Ŀǰ���ӿڵ��ڲ�ʵ���в��Դ˷���ֵ���κδ���, �÷���ֵ�Ĵ���ֻ��Ϊ�˽�������;; 
//         �����麯���ڳɹ������˸�����֮�󷵻�0,���򷵻�-1;
// ע��:   pbImage���ָ����ָ����ڴ��ں�������ʱ�����ͷ�, �������û�ϣ���������غ�ʹ��
//         ��ָ����ָ������, �����Ѹ����ݸ�������;
typedef int ( __cdecl *BINARY_IMAGE_CALLBACK )( void *pFirstParameter, DWORD32 dwCarID, WORD wImageWidth, WORD wImageHigh, BYTE bType, WORD wSize,
               BYTE *pbImage, DWORD64 dwTimeMs );

// ����:  ʵʱ��Ƶ�������͵Ļص�����ָ��ԭ��;
// ����: pFirstParameter  �ص�ʱ���ݵĵ�һ������;
//                        �ò�����ֵ��ΪSetHvCallBack()������pFirstParameter��ֵ;
//		 wVideoID		  ��Ƶ�����;
//		 dwSize 			  ���ݴ�С;
//       pbImage		  ͼ�����ݵ�ָ��;
// ����ֵ: Ŀǰ���ӿڵ��ڲ�ʵ���в��Դ˷���ֵ���κδ���, �÷���ֵ�Ĵ���ֻ��Ϊ�˽�������;; 
//         �����麯���ڳɹ������˸�����֮�󷵻�0,���򷵻�-1;
// ע��:   pbImage���ָ����ָ����ڴ��ں�������ʱ�����ͷ�, �������û�ϣ���������غ�ʹ��
//         ��ָ����ָ������, �����Ѹ����ݸ�������;
typedef int ( __cdecl *VIDEO_CALLBACK )( void *pFirstParameter, WORD wVideoID, DWORD dwSize, BYTE *pbImage );

// ����:  ������Ϣ��ʼ�����ź��������͵Ļص�����ָ��ԭ��;
// ����: pFirstParameter	�ص�ʱ���ݵĵ�һ������;
//       dwCarID			����ID;
// ����ֵ: Ŀǰ���ӿڵ��ڲ�ʵ���в��Դ˷���ֵ���κδ���, �÷���ֵ�Ĵ���ֻ��Ϊ�˽�������;; 
//         �����麯���ڳɹ������˸�����֮�󷵻�0,���򷵻�-1;
//ע�⣺   ϵͳ�ڽ���һ��ʶ����ǰ���ñ�����

typedef int ( __cdecl *CARINFO_BEGIN_CALLBACK )( void *pFirstParameter, DWORD32 dwCarID );

// ����:  ������Ϣ���������ź��������͵Ļص�����ָ��ԭ��;
// ����: pFirstParameter	�ص�ʱ���ݵĵ�һ������;
//       dwCarID			����ID;
// ����ֵ: Ŀǰ���ӿڵ��ڲ�ʵ���в��Դ˷���ֵ���κδ���, �÷���ֵ�Ĵ���ֻ��Ϊ�˽�������;; 
//         �����麯���ڳɹ������˸�����֮�󷵻�0,���򷵻�-1;
//ע��:    ϵͳ�ڽ�����һ��ʶ��������ñ�����

typedef int ( __cdecl *CARINFO_END_CALLBACK )( void *pFirstParameter, DWORD32 dwCarID );

// ����:	��ȡ��ǰ���յ��ĳ�����źͳ����ַ���
// ����:	hHV			������OpenHv()�����򿪵ľ��;
//			pdwCarID	���ص�ǰ���յ��ĳ������;
//			pszPlate	���ص�ǰ���յ��ĳ����ַ�����û���յ����Ƶ�ʱ�򷵻�"";
//			iPlateLen	���복���ַ������ջ���������;
// ����ֵ: ����S_OK, ��ʾ�����ɹ�;
//         ����E_HANDLE, ��ʾhHandle������һ���Ƿ��ľ��;
//         ����E_POINTER, �����а����зǷ���ָ��;
//         ����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;

HV_API HRESULT __cdecl HVGetResult( HV_HANDLE hHV, DWORD32 *pdwCarID, LPSTR pszPlate, int iPlateLen );

// ����:  ���������������͵Ļص�����ָ��ԭ��;
// ����: pFirstParameter    �ص�ʱ���ݵĵ�һ������;
//       iLen               �������������ĳ���;
//       pString            ָ���������������ָ��;
// ����ֵ: ���麯���ڳɹ������˸�����֮�󷵻�0,���򷵻�-1;
// ע��:   pString���ָ����ָ����ڴ��ں�������ʱ�����ͷ�, �������û�ϣ���������غ�ʹ��
//         ��ָ����ָ������, �����Ѹ����ݸ�������;
//     	   ��������ʶ��������Ϊ��������Ϣʱ����ȡ������
typedef int ( __cdecl *STREAM_CALLBACK )( void *pFirstParameter, INT32 iLen, PSTR pString );

// ����:	���������������е���Ƶ�����豸�������豸��Ŀ
// ����:	pdwMaxCount  �˲���ֵΪ����������Ƶ����ϵͳ����Ŀ;
// ����ֵ: ����S_OK, ��ʾ�����ɹ�;
//         ����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;
HV_API HRESULT __cdecl SearchHVDeviceCount( DWORD32 *pdwCount );

// ����:	����������ѯ�豸ip��ַ
// ����:	iIndex			ָ����ѯ������,��������������Ƶ����ϵͳ�������Ŀ;
//			pdw64MacAddr	�����豸��̫����ַ
//			pdwIP			�����豸IP��ַ
//			pdwMask			�����豸IP����
//			pdwGateway		�����豸IP���ص�ַ
// ����ֵ:	����S_OK, ��ʾ�����ɹ�;
//			����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;
// ע��:	�ڵ��ñ�����֮ǰ�ȵ���SearchHVDeviceCount�����豸
HV_API HRESULT __cdecl GetHVDeviceAddr( int iIndex, DWORD64 *pdw64MacAddr, DWORD32 *pdwIP, DWORD32 *pdwMask, DWORD32 *pdwGateway );

// ����:	�����豸����̫����ַ�����豸���޸��豸��IP��ַ
// ����:	dw64MacAddr		����Ҫ�������豸��̫����ַ
//			dwIP			����Ҫ���õ��豸IP��ַ
//			dwMask			����Ҫ���õ��豸IP����
//			dwGateway		����Ҫ���õ��豸IP���ص�ַ
// ����ֵ:	����S_OK, ��ʾ�����ɹ�;
//			����S_FALSE, ��ʾ����֤;
//			����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;
// ע��:	�豸����̫����ַͨ������SearchHVDeviceCount��GetHVDeviceAddr�õ�
HV_API HRESULT __cdecl SetHvIPFromMac( DWORD64 dw64MacAddr, DWORD32 dwIP, DWORD32 dwMask, DWORD32 dwGateway );

// ����:	ת��YUV���ݵ�RGB
// ����:	pbDest			���RGB���ݵĻ�����ָ��;
//			pbSrc			����YUV���ݵĻ�����ָ��;
//			iSrcWidth		ͼ����;
//			iSrcHeight		ͼ��߶�;
//			iBGRStride		RGB����ÿ�еĲ���;
// ����ֵ:  ����S_OK, ��ʾ�����ɹ�,
//          ����E_POINTER, �����а����зǷ���ָ��;
//          ����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;
HV_API HRESULT __cdecl Yuv2Rgb(
	BYTE *pbDest,
	BYTE *pbSrc,
	int iSrcWidth,
	int iSrcHeight,
	int iBGRStride
);

// ����:	ת��YUV���ݳ�BMP��ʽ
// ����:	pbDest			���BMP���ݵĻ�����ָ��;
//			iDestBufLen		�����������С
//			piDestLen		ʵ��������ݴ�С
//			pbSrc			����YUV���ݵĻ�����ָ��;
//			iSrcWidth		ͼ����;
//			iSrcHeight		ͼ��߶�;
// ����ֵ:  ����S_OK, ��ʾ�����ɹ�,
//          ����E_POINTER, �����а����зǷ���ָ��;
//          ����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;
HV_API HRESULT __cdecl Yuv2BMP(
	BYTE *pbDest,
	int iDestBufLen,
	int *piDestLen,
	BYTE *pbSrc,
	int iSrcWidth,
	int iSrcHeight
);

// ����:	ת��YUV���ݳ�JPG��ʽ
// ����:	pbDest			���JPG���ݵĻ�����ָ��;
//			iDestBufLen		�����������С
//			piDestLen		ʵ��������ݴ�С
//			pbSrc			����YUV���ݵĻ�����ָ��;
//			iSrcWidth		ͼ����;
//			iSrcHeight		ͼ��߶�;
// ����ֵ:  ����S_OK, ��ʾ�����ɹ�,
//          ����E_POINTER, �����а����зǷ���ָ��;
//          ����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;
HV_API HRESULT __cdecl Yuv2Jpg(
	BYTE *pbDest,
	int iDestBufLen,
	int *piDestLen,
	BYTE *pbSrc,
	int iSrcWidth,
	int iSrcHeight
);

// ����:	ǿ��ʶ�������ʶ�����ʹ�ͼ
// ����:	hHandle	 ��OpenHv()�����򿪵ľ��;
// ����ֵ:  ����S_OK, ��ʾ�����ɹ�,
//          ����E_POINTER, �����а����зǷ���ָ��;
//          ����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;
//ע�⣺	��������ʶ��������Ϊ�ɴ���ʱ������Ч
HV_API HRESULT __cdecl HV_ForceSend( HV_HANDLE hHandle );


// ����:	��ȡ���Ƹ�����Ϣ(����ʵ������������٣�����λ�ã����ȵ�)
// ����:	hHandle			��OpenHv()�����򿪵ľ��;
//			ppszPlateInfo	����ָ���Ƹ�����Ϣ��ָ��ĵ�ַ
//			ʾ��:
//           char *pInfo;//ָ�򸽼���Ϣ
//           HV_GetPlateInfo(hHandle,&pInfo);          
// ����ֵ:  ����S_OK, ��ʾ�����ɹ�,
//          ����E_POINTER, �����а����зǷ���ָ��;
//          ����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;
//ע�⣺	��������ʶ��������Ϊ���������Ϣʱ����ȡ������
HV_API HRESULT __cdecl HV_GetPlateInfo(  HV_HANDLE hHandle, char** ppszPlateInfo );

// ����:	�����û���������
// ����:	hHandle			��OpenHv()�����򿪵ľ��;
//		ppszPlateInfo		���봮�����ݵ��׵�ַ
//		dwDataSize		������Ҫ�������ݵĳ���
// ����ֵ:  ����S_OK, ��ʾ�����ɹ�,
//          ����E_POINTER, �����а����зǷ���ָ��;
//          ����E_FAIL, ��ʾδ֪�Ĵ����²���ʧ��;
//ע�⣺	�������ڳɹ�ִ�к�ᴥ������ص�����
HV_API HRESULT __cdecl HV_ProcessUserData( HV_HANDLE hHandle, PBYTE pbData, DWORD32 dwDataSize );
#endif
