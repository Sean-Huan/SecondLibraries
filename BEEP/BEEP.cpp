#include "BEEP.h"

_BEEP	BEEP;

_BEEP::_BEEP()
{
}

_BEEP::~_BEEP()
{
}

/************************************************************************************************************
���� �� ������	Initialization		��ʼ������
������˵������	��
���� �� ֵ����	��
����    ������	Initialization();	��ʼ����������������
************************************************************************************************************/
void _BEEP::Initialization(void)
{
	pinMode(BEEP_PIN, OUTPUT);   //���ö˿�Ϊ���ģʽ
	TurnOff();
}

/************************************************************************************************************
���� �� ������	TurnOn			����������������
������˵������	��
���� �� ֵ����	��
����    ������	TurnOn();		������������
************************************************************************************************************/
void _BEEP::TurnOn(void)
{
	digitalWrite(BEEP_PIN, BEEP_ON);  //����ߵ�ƽ
}

/************************************************************************************************************
���� �� ������	TurnOff			�ر���������������
������˵������	��
���� �� ֵ����	��
����    ������	TurnOff();		�ر�����������
************************************************************************************************************/
void _BEEP::TurnOff(void)
{
	digitalWrite(BEEP_PIN, BEEP_OFF);  //����͵�ƽ
}
