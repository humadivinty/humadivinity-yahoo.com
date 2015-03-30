#ifndef __MULTIHV_API_H__
#define __MULTIHV_API_H__

#include "hv_api_base.h"

// ����:		�������Ӷ������ʶ�������������̷��أ����ȴ����г���ʶ�������ӳɹ���
//				�Ƿ����ӳɹ���ͨ������MultiHvIsConnected������飻
// ����:		ppBoxAddr			(in)��Ҫ���ӵĳ���ʶ������ַ���飻
//				nBoxCount			(in)��Ҫ���ӵĳ���ʶ����������
//				phMultiHv			(out)���صĵ����Ӿ����
// ����ֵ:		S_OK				�����ɹ���
//				E_POINTER			����Ƿ�ָ�룻
//				E_INVALIDARG		�������зǷ����ݣ�
//				E_FAIL				δ֪�Ĵ����²���ʧ�ܡ�
HV_API HRESULT __cdecl OpenMultiHv(char **ppBoxAddr, int nBoxCount, HV_HANDLE *phMultiHv);

// ����:		�ر���OpenMultiHv�õ���HV_HANDLE��
// ����:		hMultiHv			(in)��Ҫ�رյľ������������OpenMultiHv��õģ�
// ����ֵ:		S_OK				�����ɹ���
//				E_HANDLE			hMultiHv������һ���Ƿ��ľ����
//				E_FAIL				δ֪�Ĵ����²���ʧ�ܡ�
HV_API HRESULT __cdecl CloseMultiHv(HV_HANDLE hMultiHv);

typedef struct _HV_CONNECTION_STATUS {
	char m_szBoxAddr[16];			//����OpenMultiHvʱָ���ĳ���ʶ������ַ��
	int m_iStatus;					//����ʶ����������״̬��ȡֵ����HvIsConnected˵����
} HV_CONNECTION_STATUS;

// ����:		����������ӵ�ʶ����������״̬��
// ����:		hMultiHv			(in)��Ҫ�رյľ������������OpenMultiHv��õģ�
//				pConnStatus			(out)���泵��ʶ��������״̬�����飻
//				nBufferCount		(in)ָ��pConnStatus�����Ԫ�ظ�����
//				piStatusCount		(out)����ʵ�ʴ򿪵ĳ���ʶ����������
// ����ֵ:		S_OK				�����ɹ���
//				E_HANDLE			hMultiHv������һ���Ƿ��ľ��;
//				E_POINTER			����Ƿ�ָ�룻
//				E_INVALIDARG		�������зǷ����ݣ�
//				E_FAIL				δ֪�Ĵ����²���ʧ�ܡ�
HV_API HRESULT __cdecl MultiHvIsConnected(HV_HANDLE hMultiHv, HV_CONNECTION_STATUS *pConnStatus, int nBufferCount, int *piStatusCount);

// ����:		����һ����Ƶ����һ���������͵Ļص�������
//				�������������յ����κ����ݽ�ͨ���ص���������ʽ���ݸ�Ӧ�ó�����ϲ㣻
// ����:		hMultiHv			(in)��OpenMultiHv()�����򿪵ľ����
//				pFunction			(in)�ص������ĺ���ָ�룬�ص�������������hv_api_base.h�У�
//									�ò�����һ������ָ��, �������������յ�����ʱ,�����øú�����
//									����ò���ΪNULL, ���ʾ�رոûص�����, ���������ϵĸ�����������Ҳ�漴���رգ�
//									����ò���Ϊ��NULL, ���ʾ�򿪸ûص�����, ���������ϵĸ������������漴���򿪣�
//				pFirstParameter		(in)�ص�pFunctionʱ���ݵĵ�һ��������
//									�ò���һ��Ϊһ�������ָ��, �Ա����ڻص�������ֱ�ӷ���һ������
//									����û��ڻص������в�׼��ʹ�øò���,��ò�������ΪNULL��
//				wVideoID			(in)��ʾ��ƵID��2�ֽڵ��޷�������(ȱʡΪ0)��
//				wStream				(in)��ʾ�������͵�2�ֽڵ��޷�������(ȡֵ��hv_opt_base.h�ж���)��
// ����ֵ:		S_OK				�����ɹ���
//				E_HANDLE			hMultiHv������һ���Ƿ��ľ����
//				E_FAIL				δ֪�Ĵ����²���ʧ�ܡ�
HV_API HRESULT __cdecl SetMultiHvCallBack(HV_HANDLE hMultiHv, void *pFunction, void *pFirstParameter, WORD wVideoID, WORD wStream);

// ����:		��ȡ���Ƹ�����Ϣ�����١����ƿ�ȡ��������ȵȵȣ���
// ����:		hMultiHv			(in)��OpenMultiHv()�����򿪵ľ����
//				ppszPlateInfo		(out)���س��Ƹ�����Ϣָ�룻
// ����ֵ:		S_OK				�����ɹ���
//				E_HANDLE			hMultiHv������һ���Ƿ��ľ����
//				E_POINTER			����Ƿ�ָ�룻
//				E_FAIL				δ֪�Ĵ����²���ʧ�ܡ�
HV_API HRESULT __cdecl MultiHv_GetPlateInfo(HV_HANDLE hMultiHv, char** ppszPlateInfo);

// ����:		ǿ�Ƴ���ͼ��
// ����:		hMultiHv			(in)��OpenMultiHv()�����򿪵ľ����
// ����ֵ:		S_OK				�����ɹ���
//				E_HANDLE			hMultiHv������һ���Ƿ��ľ����
//				E_FAIL				δ֪�Ĵ����²���ʧ�ܡ�
HV_API HRESULT __cdecl MultiHv_ForceSend(HV_HANDLE hMultiHv);

#endif
