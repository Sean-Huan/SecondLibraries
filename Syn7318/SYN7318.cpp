#include "SYN7318.h"
#include "BEEP.h"
#include "ExtSRAMInterface.h"
#include "Chinese.h"

_SYN7318 SYN7318;


_SYN7318::_SYN7318()
{
}

_SYN7318::~_SYN7318()
{
}

//��ʼ��
/************************************************************************************************************
���� �� ������	Initialization		��ʼ������
������˵������	��
���� �� ֵ����	��
����    ������	Initialization();	��ʼ����ؽӿڼ�����
************************************************************************************************************/
void _SYN7318::Initialization(void)
{
	Serial2.begin(115200);
	while (!Serial2);
	pinMode(RESET_PIN, OUTPUT);
	digitalWrite(RESET_PIN, SYN7318_RST_OFF);
}

/************************************************************************************************************
���� �� ������	ResetCom		SYN7318ģ�鸴λ����
������˵������	��
���� �� ֵ����	��
����    ������	ResetCom();		����һ�θ�λ�źţ�ʹSYN7318��λһ��
************************************************************************************************************/
void _SYN7318::ResetCom(void)
{
	digitalWrite(RESET_PIN, SYN7318_RST_ON);
	delay(100);
	digitalWrite(RESET_PIN, SYN7318_RST_OFF);
}

/************************************************************************************************************
���� �� ������	ResetCheck		SYN7318ģ�鸴λ����
������˵������	��
���� �� ֵ����	bool����		false:��ʾ��λ�ɹ�
����    ������	ResetCheck();	���SYN7318ģ�鸴λ�ش�����
************************************************************************************************************/
bool _SYN7318::ResetCheck(void)
{
	while (Serial2.available() > 0)
	{
		if(0xfc == Serial2.read())
		{
			Serial2.readBytes(respose_com, 3);
			if (respose_com[2] == 0x4a)
			{
				return false;
			}
		}
	}
	return true;
}

/************************************************************************************************************
���� �� ������	ResetTest		SYN7318ģ�鸴λ���Ժ���
������˵������	��
���� �� ֵ����	��
����    ������	ResetTest();	
************************************************************************************************************/
void _SYN7318::ResetTest(void)
{
	unsigned long t;
	ResetCom();
	t = millis();
	while (ResetCheck())
	{
		if (millis() - t > 5000)
		{
			ResetCom();
			t = millis();
		}
	}
}

/************************************************************************************************************
���� �� ������	QueryStatus		SYN7318ģ��״̬��ѯ
������˵������	��
���� �� ֵ����	��
����    ������	QueryStatus();	���SYN7318ģ��״̬�Ƿ����
************************************************************************************************************/
void _SYN7318::QueryStatus(void)
{
	unsigned long t;
	t = millis();
	uint8_t frame[4];
	frame[0] = 0xfd;
	frame[1] = 0x00;
	frame[2] = 0x01;
	frame[3] = 0x21;
	Serial2.write(frame, 4);
	while(CommandSendCheck())		// ģ����ճɹ���־
	{
		if (millis() - t > 3000)
		{
			Serial2.write(frame, 4);
			t = millis();
		}
	}
	while(CommandSendCheckBusy())	// ģ��״̬�ش�
	{
		if (millis() - t > 3000)
		{
			ResetTest();
			t = millis();
		}
	}
	
}

/************************************************************************************************************
���� �� ������	CommandSendCheck		��������Ļش���⺯��
������˵������	��
���� �� ֵ����	״̬		true����ʾ������ճɹ�
							
����    ������	CommandSendCheck();		�жϷ����Ƿ���ȷ����
************************************************************************************************************/
bool _SYN7318::CommandSendCheck(void)		// ������
{
	unsigned long t;
	t = millis();
	while ((Serial2.available() > 0) && ((millis() - t) < 3000))
	{
		if (0xfc == Serial2.read())
		{
			Serial2.readBytes(respose_com, 3);
			if (respose_com[2] == 0x41)
			{
				return true;
			}
		}
	}
	return false;
}

/************************************************************************************************************
���� �� ������	CommandSendCheckBusy	��������Ļش���⺯��
������˵������	��
���� �� ֵ����	bool����				false����ʾģ�����
����    ������	CommandSendCheckBusy();		�ж�ģ���Ƿ�æµ
************************************************************************************************************/
bool _SYN7318::CommandSendCheckBusy(void)
{
	while (Serial2.available() > 0)
	{
		if (0xfc == Serial2.read())
		{
			Serial2.readBytes(respose_com, 3);
			if (respose_com[2] == 0x4f)
			{
				return false;
			}
		}
	}
	return true;
}

/************************************************************************************************************
���� �� ������	VoiceSynthesisPlayback			�����ϳɲ�������
������˵������	text��							�����ϳɲ������ı�
				textcode��						�ı������ʽ�����
���� �� ֵ����	
����    ������	const char str[4] = { 0xB0,0xD9,0xC1,0xE9 };//���飨GB2312���룩
				VoiceSynthesisPlayback(str,0);	���������顱
************************************************************************************************************/
void _SYN7318::VoiceSynthesisPlayback(const char* text,uint8_t textcode)
{
	uint8_t  frame[5];
	size_t lenght;
	lenght = strlen(text);
	frame[0] = 0xfd;
	frame[1] = uint8_t(((lenght + 2)>> 8) & 0xff);
	frame[2] = uint8_t((lenght + 2)& 0xff);
	frame[3] = 0x01;
	frame[4] = textcode;
	Serial2.write(frame, 5);
	Serial2.write(text, lenght);	
}

/************************************************************************************************************
���� �� ������	VSPCom			�����ϳɲ�������
������˵������	text��			�����ϳɲ������ı�
				textcode��		�ı��ı����ʽ
���� �� ֵ����
����    ������	const char str[4] = { 0xB0,0xD9,0xC1,0xE9 };//���飨GB2312���룩
				VSPCom(str,0);	���������顱
************************************************************************************************************/
void _SYN7318::VSPCom(const char* text, uint8_t textcode)
{
	VoiceSynthesisPlayback(text, textcode);
}

/************************************************************************************************************
���� �� ������	VSPTest			�����ϳɲ����ۺϲ��Ժ���
������˵������	text��			�����ϳɲ������ı�
				textcode��		�ı������ʽ�����
���� �� ֵ����
����    ������	const char str[4] = { 0xB0,0xD9,0xC1,0xE9 };//���飨GB2312���룩
				VSPTest(str,0);	���������顱
************************************************************************************************************/
void _SYN7318::VSPTest(const char* text, uint8_t textcode)
{
	unsigned long t;
	
	VSPCom(text, textcode);
	t = millis();
	while(CommandSendCheck())
	{
		if (millis() - t > 8000)
		{
			VSPCom(text, textcode);
			t = millis();
		}
	}
	while(CommandSendCheckBusy())
	{
		if (millis() - t > 8000)
		{
			
			t = millis();
		}
	}
}

/************************************************************************************************************
���� �� ������	CommandSendCheckACK		��������Ļش���⺯��
������˵������	��
���� �� ֵ����	bool����				false����ʾ�յ��ش����
����    ������	CommandSendCheckACK();	ʶ��ش���־����
************************************************************************************************************/
bool _SYN7318::CommandSendCheckACK(void)
{
	while (Serial2.available() > 0)
	{
		if (0xfc == Serial2.read())
		{
			Serial2.readBytes(respose_com, 3);
			if (respose_com[2] == 0x01)			//�յ���ȷʶ��ش�ָ��
			{
				REc_Flag = 0x55;
				return false;
			}
			else if(respose_com[2] == 0x04||respose_com[2] == 0x03)		//������ʱ\������ʱ
			{
				REc_Flag = 0x43;
				Back_Flag = 0xfe;
				return false;
			}
			else if(respose_com[2] == 0x07 ||respose_com[2] == 0x05)	//ʶ���ʶ
			{
				REc_Flag = 0x57;
				Back_Flag = 0xff;
				return false;
			}
		}
	}	
	return true;
	
}
/*********************************************************************
���� �� ������Start_ASR				����ʶ����Ժ���
������˵������Dict��				�ʵ���
����    ������Start_ASR(0x00); 		ʶ��0x00�ʵ��еĴ���
*********************************************************************/
void _SYN7318::Start_ASR(uint8_t Dict)
{
	Back_Flag = 0;
	uint8_t Frame[5];   //���淢�����������
	unsigned long t;
	t = millis();
	Frame[0] = 0xFD;      //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = 0x02;   
	Frame[3] = 0x10;      //��ʼ����ʶ������
	Frame[4] = Dict;      //�ʵ���  �������޸���Ҫʶ��Ĵʵ���
	QueryStatus();
	Serial2.write(Frame, 5);	
	while(CommandSendCheck())
	{
		if (millis() - t > 8000)
		{
			t = millis();
		}
	}
	while(CommandSendCheckACK())
	{	
		if (millis() - t > 8000)
		{
			t = millis();
		}
	}
	if(REc_Flag == 0x55)
	{
		Serial2.readBytes(SSRId,5);
		if(SSRId[4] == 101)
			VSPTest("a",1);
		else if(SSRId[4] == 102)
			VSPTest("b",1);
		else if(SSRId[4] == 103)
			VSPTest("c",1);
		else if(SSRId[4] == 104)
			VSPTest("d",1);
		else if(SSRId[4] == 105)
			VSPTest("e",1);
		else VSPTest("f",4);
	}	
	
}
/*********************************************************************
���� �� ������Start_ASR_return		����ʶ�𷵻ز��Խ��
������˵������Dict��				�ʵ���
			  whatID:				����ID/����ID  false/true
����    ������Start_ASR(0x04��true); 		ʶ��0x04�ʵ��еĴ���,����ʶ�𵽵Ĵ�������ID
*********************************************************************/
uint8_t _SYN7318::Start_ASR_return(uint8_t Dict,bool whatID)
{
	Back_Flag = 0;
	uint8_t Frame[5];   //���淢�����������
	uint8_t IDdex = 2;
	unsigned long t;
	
	t = millis();
	Frame[0] = 0xFD;		// ֡ͷ
	Frame[1] = 0x00;
	Frame[2] = 0x02;   
	Frame[3] = 0x10;		// ��ʼ����ʶ������
	Frame[4] = Dict;		// �ʵ���  �������޸���Ҫʶ��Ĵʵ���
	if(whatID == true)
		IDdex = 4;
	QueryStatus();				// ģ��״̬��ѯ
	Serial2.write(Frame, 5);	// ��������ʶ��
	while(CommandSendCheck())	// ������ճɹ�״̬�ش�
	{
		if (millis() - t > 3000)
		{
			t = millis();
		}
	}
	while(CommandSendCheckACK())
	{	
		if (millis() - t > 8000)
		{
			t = millis();
		}
	}
	if(REc_Flag == 0x55)
	{
		Serial2.readBytes(SSRId,5);
		Back_Flag = SSRId[IDdex];
	}	
	return Back_Flag;
}

/*********************************************************************
���� �� ������Start_ASR_send		��������ʶ��
������˵������Dict��				�ʵ���
			 
����    ������Start_ASR(4); 		ʶ��0x04�ʵ��еĴ���,
*********************************************************************/
void _SYN7318::Start_ASR_send(uint8_t Dict)
{
	uint8_t Frame[5];   //���淢�����������
	unsigned long t;
	t = millis();
	Frame[0] = 0xFD;   //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = 0x02;   
	Frame[3] = 0x10;   //��ʼ����ʶ������
	Frame[4] = Dict;   //�ʵ���  �������޸���Ҫʶ��Ĵʵ���
	QueryStatus();
	Serial2.write(Frame, 5);	
	while(CommandSendCheck())
	{
		if (millis() - t > 8000)
		{
			t = millis();
		}
	}
}
/*********************************************************************
���� �� ������Start_ASR_rec			����ʶ�𷵻ز��Խ��
������˵������
			  whatID:				����ID/����ID  false/true
����    ������Start_ASR(true); 		����ʶ�𵽵Ĵ�������ID
*********************************************************************/
uint8_t _SYN7318::Start_ASR_rec(bool whatID)
{
	Back_Flag = 0;
	uint8_t IDdex = 2;
	unsigned long t;
	t = millis();
	if(whatID == true)
		IDdex = 4;
	while(CommandSendCheckACK())
	{	
		if (millis() - t > 8000)
		{
			t = millis();
		}
	}
	if(REc_Flag == 0x55)
	{
		Serial2.readBytes(SSRId,5);
		Back_Flag = SSRId[IDdex];
	}	
	return Back_Flag;
	
}

/*********************************************************************************
 * �� �� �� ��  �����漴ָ��ʶ����
 * ��    �� ��  Dict -> �ʵ���
 * �� �� ֵ ��  ����ID
 * ��    �� ��  number = 5;  // �������ʶ�����
				do { SYN7318_ID = SYN7318.Start_ASR_WEN(0x03); 
				} while ((!SYN7318_ID) && (--number));
��¼1��
--------------------------------------------------------------------------------
| ֡ͷ | ����������	| ������            | ƥ��� | ����ID    | ����ID 
|------|------------|-------------------|--------|------------------------------
| 0xFC | 0x00  0x06 | 0x01 ʶ��ɹ�     | 0xXX   | 0xXX 0xXX | 0xXX 0xXX
|      |------------|-------------------|--------|-----------|------------------
|      | 0x00  0x04 | 0x02 ʶ��ɹ�     | 0xXX   | 0xXX 0xXX | ��
|      |------------|-------------------|--------|-----------|------------------
|      | 0x00  0x01 | 0x03 �û�������ʱ | ��     | ��        | ��
|      | 0x00  0x01 | 0x04 �û�������ʱ | ��     | ��        | ��
|      | 0x00  0x01 | 0x05 ʶ���ʶ     | ��     | ��        | ��
|      | 0x00  0x01 | 0x06 ʶ���ڲ����� | ��     | ��        | ��
--------------------------------------------------------------------------------
*********************************************************************************/
uint8_t _SYN7318::Start_ASR_WEN(uint8_t Dict)
{
	uint8_t SYN_ID = 0;
	uint16_t str_len = 0;
	unsigned long t;
	
	uint8_t RX_Buf[9];
	uint8_t Inquire[4] = {0xFD,0x00,0x01,0x21};		// ģ��״̬��ѯ
	uint8_t Start[5] = {0xFD,0x00,0x02,0x10,0x03};	// ģ�鿪��ʶ�� �ʵ���
	uint8_t YY_Random[8] = {0x55, 0x06, 0x20, 0x01, 0x00, 0x00, 0x21, 0xBB};   // �����������ָ��
	
	Serial2.write(Inquire, 4);		// ģ��״̬��ѯ
	t = millis();
	while (true)		// ������ճɹ�״̬�ش�
	{
		if (Serial2.available() > 0)
		{
			if (0xFC == Serial2.read())
			{
				Serial2.readBytes(RX_Buf, 3);
				if (RX_Buf[2] == 0x41)
				{
					break;
				}
			}
		}
		if ((millis() - t) > 3000)
		{
			return 0x00;
		}
	}
	t = millis();
	while (true)		// ģ��״̬�ش�
	{
		if (Serial2.available() > 0)
		{
			if (0xFC == Serial2.read())
			{
				Serial2.readBytes(RX_Buf, 3);
				if (RX_Buf[2] == 0x4F)
				{
					break;
				}
			}
		}
		if ((millis() - t) > 3000)
		{
			return 0x00;
		}
	}
	Start[5] = Dict;
	Serial2.write(Start, 5);	// ��������ʶ��
	
	t = millis();
	while (true)		// ������ճɹ�״̬�ش�
	{
		if (Serial2.available() > 0)
		{
			if (0xFC == Serial2.read())
			{
				Serial2.readBytes(RX_Buf, 3);
				if (RX_Buf[2] == 0x41)
				{
					break;
				}
			}
		}
		if ((millis() - t) > 3000)
		{
			return 0x00;
		}
	}
	delay(600);   //��ʱ600ms
	
	ExtSRAMInterface.ExMem_Write_Bytes(0x6008, YY_Random, 8);
	delay(100);
	
	t = millis();
	while (true)
	{
		if (Serial2.available() > 0)
		{
			if (0xFC == Serial2.read())		     // ��ȡ֡ͷ
			{
				Serial2.readBytes(RX_Buf, 2);	// ��ȡ���ݳ���
				str_len = (RX_Buf[0] << 8) + RX_Buf[1];		// ���ݳ���
				Serial2.readBytes(RX_Buf, str_len);
				
				if (RX_Buf[0] == 0x01)
				{
					switch (RX_Buf[5])
					{
						case 101: 
						{
							SYN_ID = RX_Buf[5];
							VSPTest(str[0], 0); 
							break;
						}
						case 102: 
						{
							SYN_ID = RX_Buf[5];
							VSPTest(str[1], 0); 
							break;
						}
						case 103: 
						{
							SYN_ID = RX_Buf[5];
							VSPTest(str[2], 0); 
							break;
						}
						case 104: 
						{
							SYN_ID = RX_Buf[5];
							VSPTest(str[3], 0); 
							break;
						}
						case 105: 
						{
							SYN_ID = RX_Buf[5];
							VSPTest(str[4], 0); 
							break;
						}
						default : 
						{
							SYN_ID = 0x00;
							break;
						}
					}
				} else if (RX_Buf[0] == 0x02)
				{
					VSPTest("2", 0);
					SYN_ID = 0x00;
				} else if (RX_Buf[0] == 0x03)
				{
					VSPTest("3", 0);
					SYN_ID = 0x00;
				} else if (RX_Buf[0] == 0x04)
				{
					VSPTest("4", 0);
					SYN_ID = 0x00;
				} else if (RX_Buf[0] == 0x05)
				{
					VSPTest("5", 0);
					SYN_ID = 0x00;
				} else if (RX_Buf[0] == 0x06)
				{
					VSPTest("6", 0);
					SYN_ID = 0x00;
				}  else 
				{
					VSPTest("2", 0);
					SYN_ID = 0x00;
				}
				return SYN_ID;
			}
		}
		if ((millis() - t) > 8000)
		{
			return 0x00;
		}
	}
	return 0x00;
}

