#ifndef _DRIVE_h
#define _DRIVE_h
#include <DCMotor.h>
//#include <Auto.h>
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
//#include <FlexiTimer2.h>
//#include <Openmv.h>
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class _Drive
{
public:
	_Drive();
	~_Drive();

	//全自动模式
	typedef enum _AUTO_MODE_{
		AUTO_01 = 1, AUTO_02, AUTO_03,AUTO_04,AUTO_05
		,AUTO_06,AUTO_07,AUTO_08,AUTO_09,AUTO_10
	}AUTO_MODE;

	//组合循迹模式
	typedef enum _XJ_GROUP_{
		P_X = 1,       //普通循迹到十字路口停止
		X,             //到十字路口前进一小步
		X_L,           //循迹左转
		X_R,           //循迹右转
		L_X,           //左转循迹
		R_X,           //右转循迹
		LX_L,          //连续循迹左转
		LX_R,          //连续循迹右转
		L_LX,          //左转连续循迹
		R_LX,          //右转连续循迹    
		LX             //连续循迹
	}XJ_GROUP;

	//转弯参数
	typedef enum _ZW_GROUP_{
		L = 1,        //左转
		R,			  //右转
		DT,           //掉头
		L_B,          //左半转
		R_B           //右半转
	}ZW_GROUP;


	typedef enum _A_B_ {
		A,B
	}A_B;


	typedef enum _PARAMETER_ {
		GO_MP = 420,    //前进一小步码盘
		SPEED = 80,     //速度
		B_DELAY = 480,  //半转延时
		//L_MP   = 400, //左半转码盘（预留）  
		//R_MP   = 400, //右半转码盘（预留）
		GO_C_K = 600,   //出立体车库时的码盘
		BACK_C_K = 600, //进立体车库时所需的码盘
		GO_D_X = 1300,  //通过地形标志物所需的码盘
	}PARAMETER;

	uint8_t HW_Dimming1[4] = { 0x00,0xff,0x0c,~(0x0c) };		//光源档位 加1 命令	修改
	uint8_t HW_Dimming2[4] = { 0x00,0xff,0x18,~(0x18) };	   //光源档位 加2 命令	不修改
	uint8_t HW_Dimming3[4] = { 0x00,0xff,0x5e,~(0x5e) };       //光源档位加3 命令
	uint8_t ZigBee_back[12] = {0x55,0x02,0xC1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xbb};//zigbee 返回数组
	uint8_t Rbuf[8];           //zigbee  收到的指令

	uint8_t ZigBee_command[8];

	uint8_t gz_res = 0;        //光照值
	uint8_t  Car_Plate_Data[8];//八位 车牌 数据
	uint8_t  HW4_Data[4];      //四位红外数据
	uint8_t  HW6_Data[6];      //六位数据
	uint16_t JL_Z= 0;          //超声波测距距离值保存
	
	void Beep(uint8_t res);            //打开蜂鸣器传入 次数
	void Z_X_Led(uint8_t res);         //传入 毫秒*10
	void Double_Led(uint8_t res);      //打开双闪 加 蜂鸣器 //  入库完成
	//uint8_t Check_Data(uint8_t *data); //数据校验求出校验和


	//基本运动控制
	void ZH_XJ(uint16_t xj_mode);
	void GO(uint16_t go_mp);
	void BACK(uint16_t go_mp);
	void ZW(uint8_t zw_mode);
	void FX_Correct(uint8_t mode = 1, uint8_t cont = 3, uint16_t mp = 350);

	//红外控制
	void HW_4Send(uint8_t *HW);  //发送 四位 红外 数据
	void HW_6Send(uint8_t *HW);  //发送 六位 红外 数据
	void Beeper(void);
	void Beeper(uint8_t *x_z_data);      //打开报警器
	void T_G(uint8_t tiao);              //调光
	uint8_t Auto_T_G(uint8_t ght_value); //自动调光
	void X_Z_JL(uint16_t res);           //旋转led 显示距离
	void XZ_YS(uint8_t xz_data);         //旋转led 显示 颜色
	void XZ_TX(uint8_t xz_data);         //旋转led 显示 形状
	void XZ_TX_YS(uint8_t xz_mode, uint8_t xz_data);  //旋转led 显示 颜色   //旋转led 显示 形状
	void XZ_LED_CarPlate(uint8_t *x_z_data);          //旋转 led 显示 车牌
	//void X_Z_LK(uint8_t res);          //旋转 led  显示 路况   0  隧道事故   请绕行 1  前方 施工 扰行
	

	//数据采集
	void Csb();						         //超声波测距
	uint8_t Get_Light(void);                 //获取路灯光照挡位
	void Get_Figure_Data(uint8_t *data);     //获取图形识别结果
	void Get_JL_Value(void);                 //获取距离值 

	//ZigBee控制
	void ZigBee_Data_Send(uint8_t *data);        //发送ZigBee数据
	void Get_ZigBee_Data(void);                  //获取ZigBee接收到的数据
	void Clear_ZigBee_Data(void);                //清除ZigBee地址数据
	void Clear_FPGA_Addr_Data(uint32_t addr);    //清除FPGA地址数据
	uint8_t C_K_Get_Layer(A_B a_b);              //获取立体车库层数
	void C_K_Set_Layer(A_B a_b, uint8_t layer);  //设置立体车库层数
	void Road_Gate_Test(void);                   //获取道闸开启状态
	void Strobe_Display_Plate(uint8_t* dat);     //道闸显示车牌
	void TFT_JL(A_B a_b, uint16_t jl);           //TFT显示距离
	void TFT_FY(A_B a_b, uint8_t tft_fy_mode);   //TFT翻页控制
	void Task_END_Data_Send(void);               //任务结束数据发送

	//全自动执行函数
	void Auto_Drive(AUTO_MODE auto_mode);
	void Analyze_Handle(uint8_t *com);
	void Task_Out(void);

private:


};

extern _Drive Drive;

#endif
