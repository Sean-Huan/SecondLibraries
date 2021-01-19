#include "CoreKEY.h"
#include <Metro.h>

Metro KeyMetro(10);
_CoreKEY CoreKEY;

static uint32_t key_times;
static uint8_t Key_cnt[4];
static uint8_t Key_Rt;

_CoreKEY::_CoreKEY()
{
}

_CoreKEY::~_CoreKEY()
{
}

/************************************************************************************************************
���� �� ������  Initialization		С�����İ尴����ʼ������
������˵������	��
���� �� ֵ����	��
����    ������	Initialization();	С�����İ尴����ʼ��
************************************************************************************************************/
void _CoreKEY::Initialization(void)
{
	uint8_t i;
	/*���İ尴���˿� K0/1/2/3*/
	PORTK &= 0xf0;
	DDRK  &= 0xf0;

	key_times = 0;
	for (i = 0; i<4; i++)
		Key_cnt[i] = 0;
	Key_Rt = 0;
}

/************************************************************************************************************
���� �� ������  Check		С�����İ尴����⺯��
������˵������	��
���� �� ֵ����	Rt			������Ӧ�ļ�ֵ
����    ������	Check();	С�����İ尴�����
************************************************************************************************************/
uint8_t _CoreKEY::Check(void)
{
	uint8_t i;
	uint8_t Rt = 0;

	for (i = 0; i<4; i++)
	{
		if (Key_cnt[i] > 8)
		{
			Rt = i + 1;
			break;
		}
	}
	if (Rt == 0)
		Key_Rt = 0;
	else if (Key_Rt == 0)
		Key_Rt = Rt;
	else
		Rt = 0;
	return Rt;
}

void _CoreKEY::Run(void)
{
	uint8_t fk;
	fk = Check();

	if (fk != 0)
	{
		Key_Cmd_CB(fk);
	}
}
void _CoreKEY::Run(void(*Key_Cmd_CB_)(uint8_t fk))
{
	uint8_t fk;
	fk = Check();

	if (fk != 0)
	{
		(*Key_Cmd_CB_)(fk);
	}
}


void _CoreKEY::Kwhile(void)
{
	if (KeyMetro.check() == 1)
	{
		uint8_t i, rt;
		rt = PINK & 0x0f;
		for (i = 0; i<4; i++)
		{
			if (rt & (0x01 << i))
			{
				if (Key_cnt[i]>8)
					Key_cnt[i]--;
				else
					Key_cnt[i] = 0;
			}
			else
			{
				if (Key_cnt[i] < 12)
					Key_cnt[i]++;
			}
		}
		Run();
	}
}

/************************************************************************************************************
���� �� ������  Kwhile					С�����İ尴��ִ�к���
������˵������	function(uint8_t fk)	��ڲ����Ǻ�����������Ӧ����
���� �� ֵ����	��
����    ������	Kwhile();				С�����İ尴��ִ��
************************************************************************************************************/
void _CoreKEY::Kwhile(void(*function)(uint8_t fk))
{
	if (KeyMetro.check() == 1)
	{
		uint8_t i, rt;
		rt = PINK & 0x0f;
		for (i = 0; i<4; i++)
		{
			if (rt & (0x01 << i))
			{
				if (Key_cnt[i]>8)
					Key_cnt[i]--;
				else
					Key_cnt[i] = 0;
			}
			else
			{
				if (Key_cnt[i] < 12)
					Key_cnt[i]++;
			}
		}
		Run((*function));
	}
}


//// 
//// 
//// 
//
//#include "CoreKEY.h"
//#include <Metro.h>
//
//Metro KeyMetro(10);
//_CoreKEY CoreKEY;
//
//_CoreKEY::_CoreKEY()
//{
//}
//
//_CoreKEY::~_CoreKEY()
//{
//}
//
///************************************************************************************************************
//���� �� ������  Initialization		С�����İ尴����ʼ������
//������˵������	��
//���� �� ֵ����	��
//����    ������	Initialization();	С�����İ尴����ʼ��
//************************************************************************************************************/
//void _CoreKEY::Initialization(void)
//{
//	/*���İ尴���˿� K0/1/2/3*/
//	PORTK &= 0xf0;
//	DDRK  &= 0xf0;
//}
//
///************************************************************************************************************
//���� �� ������  Scan		С�����İ尴��ɨ�躯��
//������˵������	mode		true:��ʾ֧��������false;��ʾ��֧������
//���� �� ֵ����	��
//����    ������	Scan();		С�����İ尴����ʼ��
//************************************************************************************************************/
//uint8_t _CoreKEY::Scan(boolean mode)
//{
//	uint8_t key = 0;
//	if ((PINK & 0x0f) != 0x0f)
//	{
//		//delay(1);
//		//if ((PINK & 0x0f) != 0x0f)
//		//{
//			key = PINK & 0x0f;
//			switch (key)
//			{
//			case 0x0e:
//				key = 1;
//				break;
//			case 0x0d:
//				key = 2;
//				break;
//			case 0x0b:
//				key = 3;
//				break;
//			case 0x07:
//				key = 4;
//				break;
//			default:
//				key = 0;
//				break;
//			}
//			while ((!mode) && ((PINK & 0x0f) != 0x0f));
//		//}
//	}
//	return key;
//}
//
