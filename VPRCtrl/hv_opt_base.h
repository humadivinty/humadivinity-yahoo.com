#ifndef _HV_OPT_BASE_H_
#define _HV_OPT_BASE_H_

// ����ĳ���������HV_API�������ܵķ���ֵ.
#include "swWinError.h"

#define E_HV_READ_FAILED    _HRESULT_TYPEDEF_(0x90000001L)
#define E_HV_WRITE_FAILED  _HRESULT_TYPEDEF_(0x90000002L)
//Ϊ�û��ṩ�ļ���������Ϣ����
#define MAX_CUSTOMER_DETECT_LEN 256

//����ͼ���ʽ
#define IMAGE_YUV		1
#define IMAGE_JPEG		2
#define IMAGE_BIN		3

//���������Ͷ���
#define  _TYPE_PLATE_STR         0x8002        //  �����������ص���Ϣ����"���ƺ����ַ���" 
#define  _TYPE_BIG_IMAGE         0x8003        //  �����������ص���Ϣ����"���ƴ�ͼ����"
#define  _TYPE_SMALL_IMAGE       0x8004        //  �����������ص���Ϣ����"����Сͼ����"
#define  _TYPE_FULL_CMP_IMAGE    0x8005        //  �����������ص���Ϣ����"������ѹ����Ƶ��"���ݲ�֧�֣�
#define  _TYPE_BINARY_IMAGE      0x8006        //  �����������ص���Ϣ����"���ƶ�ֵ��Сͼ����"
#define  _TYPE_CARINFO_BEGIN     0x8008        //  �����������ص���Ϣ����"������Ϣ��ʼ�����ź�"
#define  _TYPE_CARINFO_END       0x8009        //  �����������ص���Ϣ����"������Ϣ���������ź�"
#define  _TYPE_CUSTOMERDETECT_BIN    0x810D	   //  �����������ص���Ϣ����"�����йصĵ��Զ�������"

#endif
