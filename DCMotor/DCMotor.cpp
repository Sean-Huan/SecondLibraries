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
���� �� ������	Initialization	ֱ�������ʼ��
������˵������	fHz		��		��ʼ��PWM���Ƶ�ʣ���λ��Hz
���� �� ֵ����	��
����    ������	Initialization(8000);
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

	//ѡ����ģʽ,ģʽ14--fast PWM
	TCCR4A |= _BV(WGM41);
	TCCR4B |= _BV(WGM42) | _BV(WGM43);
	TCCR3A |= _BV(WGM31);
	TCCR3B |= _BV(WGM32) | _BV(WGM33);

	//����PWM����Ƶ��
	MotorFrequency(fHz);

	//����PWM����ռ�ձ�
	//SpeedCtr(0, 0);
	Stop();
	//MotorSpeed(MOVE_RUNMODE_STOP,_Fspeed, _Bspeed);
	//LeftMotorSpeed(_Fspeed, _Bspeed);

	//�������ͨ��
	//TCCR4A |= _BV(COM4C1) | _BV(COM4B1) | _BV(COM4C0) | _BV(COM4B0);
	//TCCR3A |= _BV(COM3C1) | _BV(COM3B1) | _BV(COM3C0) | _BV(COM3B0);
	TCCR4A |= _BV(COM4C1) | _BV(COM4B1);
	TCCR3A |= _BV(COM3C1) | _BV(COM3B1);

	ParameterInit();
}

void _DCMotor::ParameterInit(void)
{
	for (uint8_t i = 0; i < 101; i++)	//ռ�ձȳ�ʼ��
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
���ܣ���ȡ����ֵ
����ֵ������ֵ
*/
uint16_t _DCMotor::Get_Distance()
{
	return uint16_t(ExtSRAMInterface.ExMem_Read(BASEADDRESS + CODEOFFSET) +
		(ExtSRAMInterface.ExMem_Read(BASEADDRESS + CODEOFFSET + 1) << 8));
}


/*
���ܣ���������
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
		if (DCMotorMetro.check() == 1)   //��ʱ����ʱ��ʼ��ȡ����
		{
			//��ȡ���̸�8λ���8λ����
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
С����������غ���
************************************************************************************************************/
/************************************************************************************************************
���� �� ������  Go		С��ǰ������
������˵������	speed	�������ٶ�
distance: ����ǰ���ľ���
���� �� ֵ����	��
����    ������	Go(70);	С��������ǰ����ǰ���ٶȣ�70
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
���� �� ������  Back		С�����˺���
������˵������	speed	:	�����ٶ�
distance:	���ú��˵ľ���
���� �� ֵ����	��
����    ������	Back(70);	С�����������ˣ������ٶȣ�70
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
���ܣ���ȡѭ��������
����ֵ����8λΪǰ7��ѭ�������ݣ���8λΪ��8��ѭ��������
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
���� �� ������  TurnLeft	С����ת����,Lspeed <= Rspeed
������˵������	Lspeed	��	���������ٶ�
Rspeed	��	���������ٶ�
���� �� ֵ����	��
����    ������	TurnLeft(70);	С����������ת����ת�ٶȣ�70
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
		tgd = Get_Track_Value(TRACK_H8);   //��ȡ��8��ѭ��������
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
���� �� ������  TurnRight	С����ת����,Rspeed <= Lspeed
������˵������	Lspeed	��	���������ٶ�
Rspeed	��	���������ٶ�
���� �� ֵ����	��
����    ������	TurnRight(70);	С����������ת����ת�ٶȣ�70
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
	while (ClearCodeDisc());  //�������
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
���� �� ������  Stop	С��ֹͣ����
������˵������	��
���� �� ֵ����	��
����    ������	Stop();	С��������ֹͣ
************************************************************************************************************/
void _DCMotor::Stop(void)
{
	/*�����δ��붼�е��ֹͣ���ܣ���ֹͣ��Ч����һ��*/

	/**********���������******************/
	//PORTE &= ~_BV(PE3);
	//PORTH &= ~_BV(PH3);
	//SpeedCtr(0, 0);
	/***********END**************************/

	//���˲������
	SpeedCtr(0, 0);
	PORTE |= _BV(PE3);
	PORTH |= _BV(PH3);
	/**********END************/
}

/************************************************************************************************************
���� �� ������  StartUp		С����������
������˵������	��
���� �� ֵ����	��
����    ������	StartUp();	С������������
************************************************************************************************************/
void _DCMotor::StartUp(void)
{
	PORTE |= _BV(PE3);
	PORTH |= _BV(PH3);
	//digitalWrite(L_CONTROL_PIN, HIGH);
	//digitalWrite(R_CONTROL_PIN, HIGH);
}

/************************************************************************************************************
���� �� ������  ShutDown	С���رպ���
������˵������	��
���� �� ֵ����	��
����    ������	ShutDown();	С���������ر�
************************************************************************************************************/
void _DCMotor::ShutDown(void)
{
	SpeedCtr(100, 100);
	PORTE |= _BV(PE3);
	PORTH |= _BV(PH3);
}
/*************************************************************END***********************************************/


/*
���ܣ�ѭ������ִ�к���
������track_q7 ǰ7·ѭ�������� 
      track_h8 ��7·ѭ��������
*/
void _DCMotor::Track_Adjust(uint8_t track_q7, uint8_t track_h8, uint8_t Car_Spend)
{
	if (track_h8 == 0xe7 && track_q7 == 0x77)  //ȫ��ǰ��
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
	else if (track_h8 == 0xcf || track_h8 == 0xdf)	//��⵽΢�ұߴ�������С��΢��ת
	{
		SpeedCtr(Car_Spend - 80, Car_Spend+30);
	}
	else if ((track_h8 == 0x9f) || (track_h8 == 0x8f))	//��⵽���ұߴ�������С��С��ת
	{
		SpeedCtr(Car_Spend - 130, Car_Spend);
	}
	else if ((track_h8 == 0xbf) || (track_h8 == 0x1f))	//��⵽���ұߴ�������С������ת
	{
		SpeedCtr(Car_Spend - 150, Car_Spend);
	}
	else if ((track_h8 == 0x7f) || (track_h8 == 0x3f))	//��⵽���ұߴ�������С������ת����Ϊ������ϵĴ�����������Ҫת��һЩ����ֹ�ܷ�
	{
		SpeedCtr(Car_Spend - 180, Car_Spend);
	}
	else if ((track_h8 == 0xf3) || (track_h8 == 0xfb))	//��⵽΢��ߴ�������С��΢��ת
	{
		SpeedCtr(Car_Spend+30, Car_Spend - 80);
	}
	else if ((track_h8 == 0xf1) || (track_h8 == 0xf9))	//��⵽����ߴ�������С��С��ת
	{
		SpeedCtr(Car_Spend, Car_Spend - 130);
	}
	else if ((track_h8 == 0xf8) || (track_h8 == 0xfd))	//��⵽����ߴ�������С������ת
	{
		SpeedCtr(Car_Spend, Car_Spend - 150);
	}
	else if ((track_h8 == 0xfe) || (track_h8 == 0xfc))	//��⵽����ߴ�������С������ת
	{
		SpeedCtr(Car_Spend, Car_Spend - 180);
	}
	else {
		SpeedCtr(Car_Spend, Car_Spend);
	}
}

/*
���ܣ�����ѭ��
������Car_Speed  Ĭ��ֵΪ80
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
	while (ClearCodeDisc());   //�������

	while (true)
	{
		track_q7 = Get_Track_Value(TRACK_Q7);
		track_h8 = Get_Track_Value(TRACK_H8);
		mp = Get_Distance();
		if (mp >= mp_value && mp <= 20000)   //�����趨����ֵֹͣ
		{
			Stop();
			return mp;
		}
		else {
			Track_Adjust(track_q7, track_h8, Car_Spend); //����ѭ������ 
		}
	}
}


/*
���ܣ����˵�ʮ��·��
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
���ܣ�ѭ��
˵������˳��ͨ���׿�
      ѭ��������ǰ7���8�������Ϊ��λ�ұ�Ϊ��λ
*/
void _DCMotor::CarTrack(uint8_t Car_Spend = 80)
{
	uint8_t track_q7, track_h8;
	uint16_t  mp_value;
	uint32_t t = 0;
	uint8_t track_err_cont = 0;
	while (ClearCodeDisc());   //�������
	StartUp();
	while (true)
	{
		track_q7 = Get_Track_Value(TRACK_Q7);
		track_h8 = Get_Track_Value(TRACK_H8);
		mp_value = Get_Distance();      //��ȡ����ֵ

		if (track_q7 == 0x7f && track_h8 == 0xff)
		{
			track_err_cont = 1;
		}

		if (((track_q7 == 0x00) || (track_h8 == 0x00)) && (mp_value >= 50))   //ѭ����ȫ��
		{
			if (track_err_cont != 1)
			{
				Stop();   //ֹͣ ѭ�����
				break;
			}
		}
		if (((track_h8 & 0x88) == 0x08) || ((track_h8 & 0x11) == 0x10) || (track_q7 == 0x00)
			|| (track_h8 == 0x00)|| ((track_h8 & 0x98)== 0x18) || ((track_h8 & 0x19)== 0x18) 
			|| ((track_h8 & 0x99) == 0x18) || ((track_h8 & 0x84) == 0x04) || ((track_h8 & 0x21) == 0x20))
		{
			if (mp_value <= 500 && track_err_cont)    //С���������ʱ˵��������·��
			{
				Go(80, 150);
				track_q7 = Get_Track_Value(TRACK_Q7);
				track_h8 = Get_Track_Value(TRACK_H8);
				if (((track_h8 & 0x81) != 0x81) && ((track_q7 & 0x41) != 0x41))
				{
					if (mp_value >= 50)                  
					{
						Drive.FX_Correct(1, 3, 350);  //�Ⱥ��˵�ʮ��·���ٽ�����ͷ
						CarTrack_MP(500,50);
						Go(Car_Spend, _Drive::GO_D_X);
					}
				}
				else {
					Back(80, 150);
					Stop(); //ֹͣ ѭ�����  //����ʮ��·�ڲ���Ϊ�׿�
					break;
				}
				track_err_cont = 0;
			}
			else if(mp_value >= 50){     //��ֹ���𲽵�ʱ�����������
				Stop(); //ֹͣ ѭ�����  //����ʮ��·�ڲ���Ϊ�׿�
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
			Track_Adjust(track_q7, track_h8, Car_Spend); //����ѭ������ 
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
���� �� ������  RightMotorSpeed	����PWM����ռ�ձȣ�Ƶ�ʱ������һ������ֵ  ͨ��A-----pin6��ͨ��B-----pin7
������˵������	_Fduty	������ͨ�� A ���PWM����ռ�ձ�
				_Bduty	������ͨ�� B ���PWM����ռ�ձ�
���� �� ֵ����	��
����    ������	RightMotorSpeed(70,0);
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
���� �� ������  RightMotorFrequency	����PWM����Ƶ�ʣ�ռ�ձȱ������һ������ֵ  ͨ��A-----pin6��ͨ��B-----pin7
������˵������	_fHz	������ͨ�� A/B ���PWM����Ƶ��
���� �� ֵ����	��
����    ������	RightMotorFrequency(7000);	//Ƶ�ʷ�ΧΪ245Hz��8MHz
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


/***************************Timer5����Fast PWM����ģʽ��������һЩ���ú���**********************************/

/************************************************************************************************************
���� �� ������  LeftMotorSpeed	����PWM����ռ�ձȣ�Ƶ�ʱ������һ������ֵ  ͨ��A-----pin46��ͨ��B-----pin45
������˵������	_front_speed	������ͨ�� A ���PWM����ռ�ձ�
				_behind_speed	������ͨ�� B ���PWM����ռ�ձ�
���� �� ֵ����	��
����    ������	LeftMotorSpeed(70,50);
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
���� �� ������  LeftMotorFrequency	����PWM����Ƶ�ʣ�ռ�ձȱ������һ������ֵ  ͨ��A-----pin46��ͨ��B-----pin45
������˵������	_fHz	������ͨ�� A/B ���PWM����Ƶ��
���� �� ֵ����	��
����    ������	LeftMotorFrequency(7000);
************************************************************************************************************/
void _DCMotor::LeftMotorFrequency(uint32_t _fHz)
{
	LfHz = _fHz;

	if (_fHz > 1000000)	//Ƶ�����Ϊ1MHz
	{
		TCCR3B &= ~(_BV(CS30) | _BV(CS31) | _BV(CS32));
		TCCR3B |= _BV(CS30);
		ICR3 = 16;
		LfHz = 16;
		LeftMotorSpeed(LFSpeed, LBSpeed);
	}
	else if (_fHz < 30)	//Ƶ����СΪ30Hz
	{
		TCCR3B &= ~(_BV(CS30) | _BV(CS31) | _BV(CS32));
		TCCR3B |= _BV(CS31);
		ICR3 = 0xffff;
		LfHz = 0xffff;
		LeftMotorSpeed(LFSpeed, LBSpeed);
	}
	else				//Ƶ�ʷ�ΧΪ30Hz~1MHz
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


//��������  �Զ�����  ��־�����  ��������




