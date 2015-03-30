#ifndef _HV_OPT_BASE_H_
#define _HV_OPT_BASE_H_

// 下面的常量定义了HV_API函数可能的返回值.
#include "swWinError.h"

#define E_HV_READ_FAILED    _HRESULT_TYPEDEF_(0x90000001L)
#define E_HV_WRITE_FAILED  _HRESULT_TYPEDEF_(0x90000002L)
//为用户提供的检测二进制信息长度
#define MAX_CUSTOMER_DETECT_LEN 256

//定义图像格式
#define IMAGE_YUV		1
#define IMAGE_JPEG		2
#define IMAGE_BIN		3

//数据流类型定义
#define  _TYPE_PLATE_STR         0x8002        //  该数据所承载的信息流是"车牌号码字符流" 
#define  _TYPE_BIG_IMAGE         0x8003        //  该数据所承载的信息流是"车牌大图象流"
#define  _TYPE_SMALL_IMAGE       0x8004        //  该数据所承载的信息流是"车牌小图象流"
#define  _TYPE_FULL_CMP_IMAGE    0x8005        //  该数据所承载的信息流是"完整的压缩视频流"（暂不支持）
#define  _TYPE_BINARY_IMAGE      0x8006        //  该数据所承载的信息流是"车牌二值化小图象流"
#define  _TYPE_CARINFO_BEGIN     0x8008        //  该数据所承载的信息流是"车辆信息开始接收信号"
#define  _TYPE_CARINFO_END       0x8009        //  该数据所承载的信息流是"车辆信息结束接收信号"
#define  _TYPE_CUSTOMERDETECT_BIN    0x810D	   //  该数据所承载的信息流是"与检测有关的调试二进制流"

#endif
