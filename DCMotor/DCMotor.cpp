#include "DCMotor.h"
#include <Command.h>
#include "wiring_private.h"
#include <ExtSRAMInterface.h>
#include <Metro.h>
#include <BEEP.h>
#include "Drive.h"

_DCMotor DCMotor;
Metro DCMotorMetro(20);

_DCMotor::_DCMotor()
{
	ExtSRAMInterface.Initialization();
}

_DCMotor::~_DCMotor()
{
}


/************************************************************************************************************
【函 数 名】：	Initialization	直流电机初始化
【参数说明】：	fHz		：		初始化PWM输出频率，单位：Hz
【返 回 值】：	无
【简    例】：	Initialization(8000);
************************************************************************************************************/
void _DCMotor::Initialization(uint32_t fHz)
{
	/*ExtSRAMInterface.Initialization();*/
	pinMode(L_CONTROL_PIN, OUTPUT);
	pinMode(R_CONTROL_PIN, OUTPUT);

	pinMode(R_F_M_PIN, OUTPUT);
	pinMode(R_B_M_PIN, OUTPUT);
	pinMode(L_F_M_PIN, OUTPUT);
	pinMode(L_B_M_PIN, OUTPUT);

	TCCR4A = 0x00;
	TCCR4B = 0x00;
	TCCR3A = 0x00;
	TCCR3B = 0x00;

	//选择工作模式,模式14--fast PWM
	TCCR4A |= _BV(WGM41);
	TCCR4B |= _BV(WGM42) | _BV(WGM43);
	TCCR3A |= _BV(WGM31);
	TCCR3B |= _BV(WGM32) | _BV(WGM33);

	//设置PWM波的频率
	MotorFrequency(fHz);

	//设置PWM波的占空比
	//SpeedCtr(0, 0);
	Stop();
	//MotorSpeed(MOVE_RUNMODE_STOP,_Fspeed, _Bspeed);
	//LeftMotorSpeed(_Fspeed, _Bspeed);

	//设置输出通道
	//TCCR4A |= _BV(COM4C1) | _BV(COM4B1) | _BV(COM4C0) | _BV(COM4B0);
	//TCCR3A |= _BV(COM3C1) | _BV(COM3B1) | _BV(COM3C0) | _BV(COM3B0);
	TCCR4A |= _BV(COM4C1) | _BV(COM4B1);
	TCCR3A |= _BV(COM3C1) | _BV(COM3B1);

	ParameterInit();
}

void _DCMotor::ParameterInit(void)
{
	for (uint8_t i = 0; i < 101; i++)	//占空比初始化
	{
		speed[i] = fHz * i / 100;
	}
}

void _DCMotor::SpeedSetOne(int16_t s, uint8_t *c1, uint8_t *c2)
{
	uint8_t t;
	t = (s >= 0) ? s : s*(-1);
	if (t > 100)
		t = 100;
	if (t < 5)
		t = 5;
	if (s == 0)
	{
		*c1 = speed[100];		//100;
		*c2 = speed[100];		//100;
	}
	else if (s > 0)
	{
		*c1 = speed[t];
		*c2 = speed[0];
	}
	else
	{
		*c1 = speed[0];
		*c2 = speed[t];
	}
}


void _DCMotor::SpeedCtr(int16_t L_speed, int16_t R_speed)
{
	uint8_t ocr3b, ocr3c, ocr4b, ocr4c;

	SpeedSetOne(L_speed, &ocr4c, &ocr4b);
	SpeedSetOne(R_speed, &ocr3b, &ocr3c);

	(ocr3b == 0) ? (TCCR3A |= _BV(COM3B0), ocr3b = fHz) : (TCCR3A &= ~_BV(COM3B0));
	(ocr3c == 0) ? (TCCR3A |= _BV(COM3C0), ocr3c = fHz) : (TCCR3A &= ~_BV(COM3C0));
	(ocr4b == 0) ? (TCCR4A |= _BV(COM4B0), ocr4b = fHz) : (TCCR4A &= ~_BV(COM4B0));
	(ocr4c == 0) ? (TCCR4A |= _BV(COM4C0), ocr4c = fHz) : (TCCR4A &= ~_BV(COM4C0));

	OCR4C = ocr4c;
	OCR4B = ocr4b;
	OCR3C = ocr3c;
	OCR3B = ocr3b;
}


/*
功能：获取码盘值
返回值：码盘值
*/
uint16_t _DCMotor::Get_Distance()
{
	return uint16_t(ExtSRAMInterface.ExMem_Read(BASEADDRESS + CODEOFFSET) +
		(ExtSRAMInterface.ExMem_Read(BASEADDRESS + CODEOFFSET + 1) << 8));
}


/*
功能：码盘清零
*/
bool _DCMotor::ClearCodeDisc(void)
{
	uint16_t distance;
	unsigned long t;
	Command.Judgment(Command.command01);
	for (size_t i = 0; i < 8; i++)
	{
		ExtSRAMInterface.ExMem_JudgeWrite(WRITEADDRESS + i, Command.command01[i]);
	}
//	t = millis();
	DCMotorMetro.interval(20);
	for (size_t i = 0; i < 5; i++)
	{
		if (DCMotorMetro.check() == 1)   //延时到达时开始读取数据
		{
			//读取码盘高8位与低8位数据
			distance = Get_Distance();
			if (distance == 0x0000)
			{
				return false;
			}
		}
	}
	return true;
}



/*********************************************************************************************************
小车动作的相关函数
************************************************************************************************************/
/************************************************************************************************************
【函 数 名】：  Go		小车前进函数
【参数说明】：	speed	：设置速度
distance: 设置前进的距离
【返 回 值】：	无
【简    例】：	Go(70);	小车动作：前进，前进速度：70
************************************************************************************************************/
void _DCMotor::Go(uint8_t speed)
{
	SpeedCtr(speed, speed);
	StartUp();
}

uint16_t _DCMotor::Go(uint8_t speed, uint16_t _distance)
{
	unsigned long t;
	uint16_t distance;
	while (ClearCodeDisc());
	Go(speed);
	t = millis();
	do
	{
		distance = Get_Distance();
		if ((65516 > _distance) && (_distance > 20))
		{
			if ((distance >= _distance) || ((millis() - t) >= 30000))
			{
				Stop();
				//ShutDown();
				delay(50);
				distance = Get_Distance();
				break;
			}
		}
		delay(10);
	} while (true);
	return distance;
}

/************************************************************************************************************
【函 数 名】：  Back		小车后退函数
【参数说明】：	speed	:	设置速度
distance:	设置后退的距离
【返 回 值】：	无
【简    例】：	Back(70);	小车动作：后退，后退速度：70
************************************************************************************************************/
void _DCMotor::Back(uint8_t speed)
{
	SpeedCtr(speed*(-1), speed*(-1));
	StartUp();
}

uint16_t _DCMotor::Back(uint8_t speed, uint16_t _distance)
{
	unsigned long t;
	uint16_t distance;

	while (ClearCodeDisc());
	Serial.print("back init");
	Serial.println(_distance);
	Back(80);
	while (Get_Distance() <= 65530);
	//delay(10);
	t = millis();
	do
	{
		distance = Get_Distance();
		Serial.print("back_mp");
		Serial.println(distance);
		if ((65516 > distance) && (distance > 20))
		{
			if (((65536 - distance) >= _distance) || ((millis() - t) > 30000) )
			{
				Stop();
				delay(50);
				distance = Get_Distance();
				break;
			}
		}
		delay(10);
	} while (true);
	return (65536 - distance);
}

#define TRACK_Q7  1
#define TRACK_H8  2
/*
功能：获取循迹灯数据
返回值：高8位为前7个循迹灯数据，低8位为后8个循迹灯数据
*/
uint16_t Get_Track_Value(uint8_t mode)
{
	if (mode == TRACK_Q7)
	{
		return ExtSRAMInterface.ExMem_Read(BASEADDRESS + TRACKOFFSET + 1);
	}
	else if (mode == TRACK_H8) {
		return ExtSRAMInterface.ExMem_Read(BASEADDRESS + TRACKOFFSET);
	}
	return 0;
}


/************************************************************************************************************
【函 数 名】：  TurnLeft	小车左转函数,Lspeed <= Rspeed
【参数说明】：	Lspeed	：	设置左轮速度
Rspeed	：	设置右轮速度
【返 回 值】：	无
【简    例】：	TurnLeft(70);	小车动作：左转，左转速度：70
************************************************************************************************************/
void _DCMotor::TurnLeft(int8_t Lspeed, int8_t Rspeed)
{
	SpeedCtr(Lspeed*(-1), Rspeed);
	StartUp();
}

void _DCMotor::TurnLeft(int8_t speed)
{
	uint8_t tgd, tp;
	unsigned long t;
	uint8_t  trackval;
	while (ClearCodeDisc());
	TurnLeft(speed, speed);
	do
	{
		tgd = Get_Track_Value(TRACK_H8);   //获取后8个循迹灯数据
		if (tgd == 0xff || tgd == 0xfe ) break;
	} while (true);

	t = millis();
	do
	{
		trackval = Get_Track_Value(TRACK_H8);
		if (!(trackval & 0x10) || ((millis() - t) > 10000))
		{
			Stop();
			//ShutDown();
			break;
		}
	} while (true);
}


/************************************************************************************************************
【函 数 名】：  TurnRight	小车右转函数,Rspeed <= Lspeed
【参数说明】：	Lspeed	：	设置左轮速度
Rspeed	：	设置右轮速度
【返 回 值】：	无
【简    例】：	TurnRight(70);	小车动作：右转，右转速度：70
************************************************************************************************************/
void _DCMotor::TurnRight(int8_t Lspeed, int8_t Rspeed)
{
	SpeedCtr(Lspeed, Rspeed*(-1));
	StartUp();
}
void _DCMotor::TurnRight(int8_t speed)
{
	uint8_t tgd, tp;
	unsigned long t;
	uint8_t  trackval;
	while (ClearCodeDisc());  //清除码盘
	TurnRight(speed, speed);
	do
	{
		tgd = Get_Track_Value(TRACK_H8);
		if (tgd == 0xff || tgd == 0x7f) break;
	} while (true);

	t = millis();
	do
	{
		trackval = Get_Track_Value(TRACK_H8);
		//if ((SearchBit(0, trackval) >= 0x08) || ((millis() - t) > 10000))
		if (((!(trackval & 0x08)) || ((millis() - t) > 10000)))
		{
			Stop();
			//ShutDown();
			break;
		}
		//Serial.println(trackval, 16);
	} while (true);
}



/************************************************************************************************************
【函 数 名】：  Stop	小车停止函数
【参数说明】：	无
【返 回 值】：	无
【简    例】：	Stop();	小车动作：停止
************************************************************************************************************/
void _DCMotor::Stop(void)
{
	/*这两段代码都有电机停止功能，但停止的效果不一样*/

	/**********电机不锁死******************/
	//PORTE &= ~_BV(PE3);
	//PORTH &= ~_BV(PH3);
	//SpeedCtr(0, 0);
	/***********END**************************/

	//电机瞬间锁死
	SpeedCtr(0, 0);
	PORTE |= _BV(PE3);
	PORTH |= _BV(PH3);
	/**********END************/
}

/************************************************************************************************************
【函 数 名】：  StartUp		小车启动函数
【参数说明】：	无
【返 回 值】：	无
【简    例】：	StartUp();	小车动作：启动
************************************************************************************************************/
void _DCMotor::StartUp(void)
{
	PORTE |= _BV(PE3);
	PORTH |= _BV(PH3);
	//digitalWrite(L_CONTROL_PIN, HIGH);
	//digitalWrite(R_CONTROL_PIN, HIGH);
}

/************************************************************************************************************
【函 数 名】：  ShutDown	小车关闭函数
【参数说明】：	无
【返 回 值】：	无
【简    例】：	ShutDown();	小车动作：关闭
************************************************************************************************************/
void _DCMotor::ShutDown(void)
{
	SpeedCtr(100, 100);
	PORTE |= _BV(PE3);
	PORTH |= _BV(PH3);
}
/*************************************************************END***********************************************/


/*
功能：循迹调整执行函数
参数：track_q7 前7路循迹灯数据 
      track_h8 后7路循迹灯数据
*/
void _DCMotor::Track_Adjust(uint8_t track_q7, uint8_t track_h8, uint8_t Car_Spend)
{
	if (track_h8 == 0xe7 && track_q7 == 0x77)  //全速前进
	{
		SpeedCtr(Car_Spend, Car_Spend);
	}
	else if (track_h8 == 0xef)
	{
		SpeedCtr(Car_Spend-60, Car_Spend+30);
	}
	else if (track_h8 == 0xf7)
	{
		SpeedCtr(Car_Spend+30, Car_Spend - 60);
	}
	else if (track_h8 == 0xcf || track_h8 == 0xdf)	//检测到微右边传感器，小车微左转
	{
		SpeedCtr(Car_Spend - 80, Car_Spend+30);
	}
	else if ((track_h8 == 0x9f) || (track_h8 == 0x8f))	//检测到再右边传感器，小车小左转
	{
		SpeedCtr(Car_Spend - 130, Car_Spend);
	}
	else if ((track_h8 == 0xbf) || (track_h8 == 0x1f))	//检测到更右边传感器，小车中左转
	{
		SpeedCtr(Car_Spend - 150, Car_Spend);
	}
	else if ((track_h8 == 0x7f) || (track_h8 == 0x3f))	//检测到最右边传感器，小车大左转。因为是最边上的传感器，所以要转大一些，防止跑飞
	{
		SpeedCtr(Car_Spend - 180, Car_Spend);
	}
	else if ((track_h8 == 0xf3) || (track_h8 == 0xfb))	//检测到微左边传感器，小车微右转
	{
		SpeedCtr(Car_Spend+30, Car_Spend - 80);
	}
	else if ((track_h8 == 0xf1) || (track_h8 == 0xf9))	//检测到再左边传感器，小车小左转
	{
		SpeedCtr(Car_Spend, Car_Spend - 130);
	}
	else if ((track_h8 == 0xf8) || (track_h8 == 0xfd))	//检测到更左边传感器，小车中右转
	{
		SpeedCtr(Car_Spend, Car_Spend - 150);
	}
	else if ((track_h8 == 0xfe) || (track_h8 == 0xfc))	//检测到最左边传感器，小车大右转
	{
		SpeedCtr(Car_Spend, Car_Spend - 180);
	}
	else {
		SpeedCtr(Car_Spend, Car_Spend);
	}
}

/*
功能：码盘循迹
参数：Car_Speed  默认值为80
*/
uint16_t _DCMotor::CarTrack_MP(uint16_t mp_value,uint8_t Car_Spend = 50)
{
	uint8_t track_q7, track_h8;
	uint8_t track_err_cont = 0;
	uint16_t mp = 0;

	track_q7 = Get_Track_Value(TRACK_Q7);
	track_h8 = Get_Track_Value(TRACK_H8);
	StartUp();
	SpeedCtr(Car_Spend, Car_Spend);
	//Serial.println(track_q7);
 //   Serial.println(track_h8);
	if (((track_h8 & 0x81) != 0x81) || ((track_q7 & 0x41) != 0x41))
	{
		do {
			track_q7 = Get_Track_Value(TRACK_Q7);
			track_h8 = Get_Track_Value(TRACK_H8);
		} while (((track_h8 & 0x81) != 0x81) || ((track_q7 & 0x41) != 0x41));
	}
	while (ClearCodeDisc());   //清除码盘

	while (true)
	{
		track_q7 = Get_Track_Value(TRACK_Q7);
		track_h8 = Get_Track_Value(TRACK_H8);
		mp = Get_Distance();
		if (mp >= mp_value && mp <= 20000)   //到达设定码盘值停止
		{
			Stop();
			return mp;
		}
		else {
			Track_Adjust(track_q7, track_h8, Car_Spend); //正常循迹调整 
		}
	}
}


/*
功能：后退到十字路口
*/
void _DCMotor::Back_Black_Line()
{
	uint8_t track_q7;
	uint8_t track_h8;

	track_q7 = Get_Track_Value(TRACK_Q7);
	track_h8 = Get_Track_Value(TRACK_H8);

	StartUp();
	SpeedCtr(-80, -80);
	delay(200);
	if (((track_h8 & 0x81) != 0x81) || ((track_q7 & 0x41) != 0x41))
	{
		while (true)
		{
			track_q7 = Get_Track_Value(TRACK_Q7);
			track_h8 = Get_Track_Value(TRACK_H8);
			if (((track_h8 & 0x81) == 0x81) && ((track_q7 & 0x41) == 0x41))
			{
				if (track_q7 != 0x7f && track_h8 != 0xff)
				{
					break;
				}
			}
		}
	}

	while (true)
	{
		track_q7 = Get_Track_Value(TRACK_Q7);
		track_h8 = Get_Track_Value(TRACK_H8);
		if (((track_h8 & 0x81) != 0x81) && ((track_q7 & 0x41) != 0x41))
		{
			Stop();
			break;
		}
	}
}


/*
功能：循迹
说明：可顺利通过白卡
      循迹灯数据前7与后8都是左边为高位右边为低位
*/
void _DCMotor::CarTrack(uint8_t Car_Spend = 80)
{
	uint8_t track_q7, track_h8;
	uint16_t  mp_value;
	uint32_t t = 0;
	uint8_t track_err_cont = 0;
	while (ClearCodeDisc());   //清除码盘
	StartUp();
	while (true)
	{
		track_q7 = Get_Track_Value(TRACK_Q7);
		track_h8 = Get_Track_Value(TRACK_H8);
		mp_value = Get_Distance();      //获取码盘值

		if (track_q7 == 0x7f && track_h8 == 0xff)
		{
			track_err_cont = 1;
		}

		if (((track_q7 == 0x00) || (track_h8 == 0x00)) && (mp_value >= 50))   //循迹灯全灭
		{
			if (track_err_cont != 1)
			{
				Stop();   //停止 循迹完成
				break;
			}
		}
		if (((track_h8 & 0x88) == 0x08) || ((track_h8 & 0x11) == 0x10) || (track_q7 == 0x00)
			|| (track_h8 == 0x00)|| ((track_h8 & 0x98)== 0x18) || ((track_h8 & 0x19)== 0x18) 
			|| ((track_h8 & 0x99) == 0x18) || ((track_h8 & 0x84) == 0x04) || ((track_h8 & 0x21) == 0x20))
		{
			if (mp_value <= 500 && track_err_cont)    //小于这个距离时说明是特殊路段
			{
				Go(80, 150);
				track_q7 = Get_Track_Value(TRACK_Q7);
				track_h8 = Get_Track_Value(TRACK_H8);
				if (((track_h8 & 0x81) != 0x81) && ((track_q7 & 0x41) != 0x41))
				{
					if (mp_value >= 50)                  
					{
						Drive.FX_Correct(1, 3, 350);  //先后退到十字路口再矫正车头
						CarTrack_MP(500,50);
						Go(Car_Spend, _Drive::GO_D_X);
					}
				}
				else {
					Back(80, 150);
					Stop(); //停止 循迹完成  //到达十字路口并且为白卡
					break;
				}
				track_err_cont = 0;
			}
			else if(mp_value >= 50){     //防止刚起步的时候就满足条件
				Stop(); //停止 循迹完成  //到达十字路口并且为白卡
				break;
			}
			else {
				SpeedCtr(Car_Spend, Car_Spend);
				while (true)
				{
					track_q7 = Get_Track_Value(TRACK_Q7);
					track_h8 = Get_Track_Value(TRACK_H8);
					if (((track_h8 & 0x81) == 0x81) && ((track_q7 & 0x41) == 0x41))
					{
						if (track_h8 != 0xff && track_q7 != 0x7f)
						{
							break;
						}
					}
				}
			}
		}
		else {
			Track_Adjust(track_q7, track_h8, Car_Spend); //正常循迹调整 
		}
	}
}


uint8_t _DCMotor::SearchBit(uint8_t mode,uint8_t s)
{
	uint8_t cont = 0;
	if (mode == 1)
	{
		for (int i = 0x80; i > 0x00; i >>= 1)
		{
			if (s & i)
			{
				cont++;
			}
		}
	}
	else
	{
		for (int i = 0x01; i < 0x100; i <<=1)
		{
			if (s & i)
			{
				cont++;
			}
		}
	}
	return cont;
}


/************************************************************************************************************
【函 数 名】：  RightMotorSpeed	设置PWM波的占空比，频率保持最后一次设置值  通道A-----pin6，通道B-----pin7
【参数说明】：	_Fduty	：设置通道 A 输出PWM波的占空比
				_Bduty	：设置通道 B 输出PWM波的占空比
【返 回 值】：	无
【简    例】：	RightMotorSpeed(70,0);
************************************************************************************************************/
void _DCMotor::MotorSpeed(uint8_t runmode, int8_t l_speed, int8_t r_speed)
{
	/*sbi(TCCR4A, COM4C1);
	sbi(TCCR4A, COM4B1);
	sbi(TCCR3A, COM3C1);
	sbi(TCCR3A, COM3B1);*/
	switch (runmode)
	{
	case MOVE_RUNMODE_STOP:
		PORTE &= ~(_BV(PE4) | _BV(PE5));
		PORTH &= ~(_BV(PH4) | _BV(PH5));
		OCR4C = 0x0000;
		OCR4B = 0x0000;
		OCR3C = 0x0000;
		OCR3B = 0x0000;
		break;
	case MOVE_RUNMODE_FF:
		OCR4C = 0;
		OCR4B = fHz * l_speed / 100;
		OCR3C = 0;
		OCR3B = fHz * r_speed / 100;
		break;
	case MOVE_RUNMODE_BB:
		OCR4B = 0;
		OCR4C = fHz * l_speed / 100;
		OCR3B = 0;
		OCR3C = fHz * r_speed / 100;
		break;
	case MOVE_RUNMODE_LIFT:
		OCR4C = 0;
		OCR4B = fHz * l_speed / 100;
		OCR3B = 0;
		OCR3C = fHz * r_speed / 100;
		break;
	case MOVE_RUNMODE_RIGHT:
		OCR4B = 0;
		OCR4C = fHz * l_speed / 100;
		OCR3C = 0;
		OCR3B = fHz * r_speed / 100;
		break;
	case MOVE_RUNMODE_TRACK:

		break;
	}
}

/************************************************************************************************************
【函 数 名】：  RightMotorFrequency	设置PWM波的频率，占空比保持最后一次设置值  通道A-----pin6，通道B-----pin7
【参数说明】：	_fHz	：设置通道 A/B 输出PWM波的频率
【返 回 值】：	无
【简    例】：	RightMotorFrequency(7000);	//频率范围为245Hz～8MHz
************************************************************************************************************/
void _DCMotor::MotorFrequency(uint32_t _fHz)
{
	fHz = 16000000 / _fHz;
	TCCR4B &= ~(_BV(CS40) | _BV(CS41) | _BV(CS42));
	TCCR4B |= _BV(CS40);
	TCCR3B &= ~(_BV(CS30) | _BV(CS31) | _BV(CS32));
	TCCR3B |= _BV(CS30);

	ICR4 = fHz;
	ICR3 = fHz;
}
/***************************************************END*****************************************************/


/***************************Timer5设置Fast PWM工作模式，以下是一些设置函数**********************************/

/************************************************************************************************************
【函 数 名】：  LeftMotorSpeed	设置PWM波的占空比，频率保持最后一次设置值  通道A-----pin46，通道B-----pin45
【参数说明】：	_front_speed	：设置通道 A 输出PWM波的占空比
				_behind_speed	：设置通道 B 输出PWM波的占空比
【返 回 值】：	无
【简    例】：	LeftMotorSpeed(70,50);
************************************************************************************************************/
void _DCMotor::LeftMotorSpeed(uint8_t _front_speed, uint8_t _behind_speed)
{
	//LfHz = TCNT3;
	LFSpeed = _front_speed;
	LBSpeed = _behind_speed;
	if ((_front_speed == 100) && (_behind_speed == 100))
	{
		sbi(TCCR3A, COM3C1);
		sbi(TCCR3A, COM3B1);
		OCR3C = 0xffff;// LfHz;
		OCR3B = 0xffff;// LfHz;
	}
	else
	{
		if (_front_speed >= 100)
		{
			sbi(TCCR3A, COM3C1);
			OCR3C = 0xffff;
		}
		else if (_front_speed <= 0)
		{
			sbi(TCCR3A, COM3C1);
			OCR3C = 0;
		}
		else
		{
			sbi(TCCR3A, COM3C1);
			OCR3C = LfHz * _front_speed / 100;
		}

		if (_behind_speed >= 100)
		{
			sbi(TCCR3A, COM3B1);
			OCR3B = 0xffff;

		}
		else if(_behind_speed <= 0)
		{
			sbi(TCCR3A, COM3B1);
			OCR3B = 0;

		}
		else
		{
			sbi(TCCR3A, COM3B1);
			OCR3B = LfHz * _behind_speed / 100;
		}

	}


}

/************************************************************************************************************
【函 数 名】：  LeftMotorFrequency	设置PWM波的频率，占空比保持最后一次设置值  通道A-----pin46，通道B-----pin45
【参数说明】：	_fHz	：设置通道 A/B 输出PWM波的频率
【返 回 值】：	无
【简    例】：	LeftMotorFrequency(7000);
************************************************************************************************************/
void _DCMotor::LeftMotorFrequency(uint32_t _fHz)
{
	LfHz = _fHz;

	if (_fHz > 1000000)	//频率最大为1MHz
	{
		TCCR3B &= ~(_BV(CS30) | _BV(CS31) | _BV(CS32));
		TCCR3B |= _BV(CS30);
		ICR3 = 16;
		LfHz = 16;
		LeftMotorSpeed(LFSpeed, LBSpeed);
	}
	else if (_fHz < 30)	//频率最小为30Hz
	{
		TCCR3B &= ~(_BV(CS30) | _BV(CS31) | _BV(CS32));
		TCCR3B |= _BV(CS31);
		ICR3 = 0xffff;
		LfHz = 0xffff;
		LeftMotorSpeed(LFSpeed, LBSpeed);
	}
	else				//频率范围为30Hz~1MHz
	{
		LfHz = 16000000 / _fHz;
		if (LfHz >= 0xffff)
		{
			TCCR3B &= ~(_BV(CS30) | _BV(CS31) | _BV(CS32));
			TCCR3B |= _BV(CS31);
			LfHz = 2000000 / _fHz;
			ICR3 = LfHz;
			LeftMotorSpeed(LFSpeed, LBSpeed);
		}
		else
		{
			TCCR3B &= ~(_BV(CS30) | _BV(CS31) | _BV(CS32));
			TCCR3B |= _BV(CS30);
			/*LfHz = 16000000 / _fHz;*/
			ICR3 = LfHz;
			LeftMotorSpeed(LFSpeed, LBSpeed);
		}
	}
}


uint8_t _DCMotor::ShiftBitAdd(uint8_t n)
{
	uint8_t bit = 0;
	for (uint8_t i = 0; i < 8; i++)
	{
		/*if (((n >> i) & 0x01) == 0x01)	bit ++;*/
		bit += bitRead(n, i);
	}
	return bit;
}

uint8_t _DCMotor::JudgeAround(uint8_t n)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		if ((((n >> 3) & 0x01) == 0x00) && (((n >> 4) & 0x01) == 0x00))
		{
			return 8;
		}
		else if (((n >> (3 - i)) & 0x01) == 0x00)
		{
			return (3 - i);
		}
		else if (((n >> (4 + i)) & 0x01) == 0x00)
		{
			return (4 + i);
		}
	}
	return 9;
}


//基本控制  自动行走  标志物控制  从主交互




