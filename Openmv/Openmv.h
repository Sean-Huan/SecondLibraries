// BEEP.h

#ifndef _OPENMV_h
#define _OPENMV_h
#include <DCMotor.h>
#include <Drive.h>
#include <CoreLED.h>
#include <CoreKEY.h>
#include <CoreBeep.h>
#include <ExtSRAMInterface.h>
#include <LED.h>
#include <BH1750.h>
#include <Command.h>
#include <BEEP.h>
#include <infrare.h>
#include <Ultrasonic.h>
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif



class _Openmv
{
public:
	_Openmv();
	~_Openmv();

	uint8_t OpenMv_AGVDataFlag = 0;      //为  1  开启 接收 openmv 返回的数据 默认关
	uint8_t DataType;
	uint8_t DataFlag;                    //读取的识别成功与失败状态 ，，，，不准确 判断 数组下标【4】大于2 即可 即判断长度大于2 即为正常数据
	uint8_t DataLength;                  //二维码 长度
	uint8_t DataOTABuf[48];              //从openmv读出 的二维码 数据

	void Set_Debug_Mode();               //启动调试模式
	void Res_Debug_Mode();               //关闭调试模式   

	void Traffic_Disc_StartUp(uint8_t mode = 1);  //启动交通灯识别
	void Traffic_Distinguish(_Drive::A_B a_b);    //交通灯识别

	void QR_Disc_StartUp(void);                   //启动二维码识别
	uint8_t QR_Data_Handle(uint8_t *qr_data);     //二维码数据提取
	uint8_t QR_Distinguish(_Drive::A_B a_b);      //二维码全套识别

	void Figure_Disc_StartUp(void);               //启动图形识别  
	void Figure_Distinguish(uint8_t *data);       //图形识别  

	uint8_t  Check(uint8_t *data);
	uint16_t _Openmv::judge(uint16_t a, uint16_t b, uint8_t mode);
private:
	
};

extern _Openmv	Openmv;

#endif


