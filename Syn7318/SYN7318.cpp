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

//初始化
/************************************************************************************************************
【函 数 名】：	Initialization		初始化函数
【参数说明】：	无
【返 回 值】：	无
【简    例】：	Initialization();	初始化相关接口及变量
************************************************************************************************************/
void _SYN7318::Initialization(void)
{
	Serial2.begin(115200);
	while (!Serial2);
	pinMode(RESET_PIN, OUTPUT);
	digitalWrite(RESET_PIN, SYN7318_RST_OFF);
}

/************************************************************************************************************
【函 数 名】：	ResetCom		SYN7318模块复位函数
【参数说明】：	无
【返 回 值】：	无
【简    例】：	ResetCom();		产生一次复位信号，使SYN7318复位一次
************************************************************************************************************/
void _SYN7318::ResetCom(void)
{
	digitalWrite(RESET_PIN, SYN7318_RST_ON);
	delay(100);
	digitalWrite(RESET_PIN, SYN7318_RST_OFF);
}

/************************************************************************************************************
【函 数 名】：	ResetCheck		SYN7318模块复位函数
【参数说明】：	无
【返 回 值】：	bool类型		false:表示复位成功
【简    例】：	ResetCheck();	检测SYN7318模块复位回传命令
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
【函 数 名】：	ResetTest		SYN7318模块复位测试函数
【参数说明】：	无
【返 回 值】：	无
【简    例】：	ResetTest();	
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
【函 数 名】：	QueryStatus		SYN7318模块状态查询
【参数说明】：	无
【返 回 值】：	无
【简    例】：	QueryStatus();	检测SYN7318模块状态是否空闲
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
	while(CommandSendCheck())		// 模块接收成功标志
	{
		if (millis() - t > 3000)
		{
			Serial2.write(frame, 4);
			t = millis();
		}
	}
	while(CommandSendCheckBusy())	// 模块状态回传
	{
		if (millis() - t > 3000)
		{
			ResetTest();
			t = millis();
		}
	}
	
}

/************************************************************************************************************
【函 数 名】：	CommandSendCheck		发送命令的回传检测函数
【参数说明】：	无
【返 回 值】：	状态		true：表示命令接收成功
							
【简    例】：	CommandSendCheck();		判断发送是否正确接收
************************************************************************************************************/
bool _SYN7318::CommandSendCheck(void)		// 已修正
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
【函 数 名】：	CommandSendCheckBusy	发送命令的回传检测函数
【参数说明】：	无
【返 回 值】：	bool类型				false：表示模块空闲
【简    例】：	CommandSendCheckBusy();		判断模块是否忙碌
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
【函 数 名】：	VoiceSynthesisPlayback			语音合成播报函数
【参数说明】：	text：							语音合成播报的文本
				textcode：						文本编码格式的序号
【返 回 值】：	
【简    例】：	const char str[4] = { 0xB0,0xD9,0xC1,0xE9 };//百灵（GB2312编码）
				VoiceSynthesisPlayback(str,0);	播报“百灵”
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
【函 数 名】：	VSPCom			语音合成播报函数
【参数说明】：	text：			语音合成播报的文本
				textcode：		文本的编码格式
【返 回 值】：
【简    例】：	const char str[4] = { 0xB0,0xD9,0xC1,0xE9 };//百灵（GB2312编码）
				VSPCom(str,0);	播报“百灵”
************************************************************************************************************/
void _SYN7318::VSPCom(const char* text, uint8_t textcode)
{
	VoiceSynthesisPlayback(text, textcode);
}

/************************************************************************************************************
【函 数 名】：	VSPTest			语音合成播报综合测试函数
【参数说明】：	text：			语音合成播报的文本
				textcode：		文本编码格式的序号
【返 回 值】：
【简    例】：	const char str[4] = { 0xB0,0xD9,0xC1,0xE9 };//百灵（GB2312编码）
				VSPTest(str,0);	播报“百灵”
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
【函 数 名】：	CommandSendCheckACK		发送命令的回传检测函数
【参数说明】：	无
【返 回 值】：	bool类型				false：表示收到回传结果
【简    例】：	CommandSendCheckACK();	识别回传标志更新
************************************************************************************************************/
bool _SYN7318::CommandSendCheckACK(void)
{
	while (Serial2.available() > 0)
	{
		if (0xfc == Serial2.read())
		{
			Serial2.readBytes(respose_com, 3);
			if (respose_com[2] == 0x01)			//收到正确识别回传指令
			{
				REc_Flag = 0x55;
				return false;
			}
			else if(respose_com[2] == 0x04||respose_com[2] == 0x03)		//静音超时\语音超时
			{
				REc_Flag = 0x43;
				Back_Flag = 0xfe;
				return false;
			}
			else if(respose_com[2] == 0x07 ||respose_com[2] == 0x05)	//识别拒识
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
【函 数 名】：Start_ASR				语音识别测试函数
【参数说明】：Dict：				词典编号
【简    例】：Start_ASR(0x00); 		识别0x00词典中的词条
*********************************************************************/
void _SYN7318::Start_ASR(uint8_t Dict)
{
	Back_Flag = 0;
	uint8_t Frame[5];   //保存发送命令的数组
	unsigned long t;
	t = millis();
	Frame[0] = 0xFD;      //帧头
	Frame[1] = 0x00;
	Frame[2] = 0x02;   
	Frame[3] = 0x10;      //开始语音识别命令
	Frame[4] = Dict;      //词典编号  在这里修改想要识别的词典编号
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
【函 数 名】：Start_ASR_return		语音识别返回测试结果
【参数说明】：Dict：				词典编号
			  whatID:				词条ID/命令ID  false/true
【简    例】：Start_ASR(0x04，true); 		识别0x04词典中的词条,返回识别到的词条命令ID
*********************************************************************/
uint8_t _SYN7318::Start_ASR_return(uint8_t Dict,bool whatID)
{
	Back_Flag = 0;
	uint8_t Frame[5];   //保存发送命令的数组
	uint8_t IDdex = 2;
	unsigned long t;
	
	t = millis();
	Frame[0] = 0xFD;		// 帧头
	Frame[1] = 0x00;
	Frame[2] = 0x02;   
	Frame[3] = 0x10;		// 开始语音识别命令
	Frame[4] = Dict;		// 词典编号  在这里修改想要识别的词典编号
	if(whatID == true)
		IDdex = 4;
	QueryStatus();				// 模块状态查询
	Serial2.write(Frame, 5);	// 开启语音识别
	while(CommandSendCheck())	// 命令接收成功状态回传
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
【函 数 名】：Start_ASR_send		开启语音识别
【参数说明】：Dict：				词典编号
			 
【简    例】：Start_ASR(4); 		识别0x04词典中的词条,
*********************************************************************/
void _SYN7318::Start_ASR_send(uint8_t Dict)
{
	uint8_t Frame[5];   //保存发送命令的数组
	unsigned long t;
	t = millis();
	Frame[0] = 0xFD;   //帧头
	Frame[1] = 0x00;
	Frame[2] = 0x02;   
	Frame[3] = 0x10;   //开始语音识别命令
	Frame[4] = Dict;   //词典编号  在这里修改想要识别的词典编号
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
【函 数 名】：Start_ASR_rec			语音识别返回测试结果
【参数说明】：
			  whatID:				词条ID/命令ID  false/true
【简    例】：Start_ASR(true); 		返回识别到的词条命令ID
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
 * 函 数 名 ：  语音随即指令识别函数
 * 参    数 ：  Dict -> 词典编号
 * 返 回 值 ：  命令ID
 * 简    例 ：  number = 5;  // 语音最大识别次数
				do { SYN7318_ID = SYN7318.Start_ASR_WEN(0x03); 
				} while ((!SYN7318_ID) && (--number));
附录1：
--------------------------------------------------------------------------------
| 帧头 | 数据区长度	| 命令字            | 匹配度 | 词条ID    | 命令ID 
|------|------------|-------------------|--------|------------------------------
| 0xFC | 0x00  0x06 | 0x01 识别成功     | 0xXX   | 0xXX 0xXX | 0xXX 0xXX
|      |------------|-------------------|--------|-----------|------------------
|      | 0x00  0x04 | 0x02 识别成功     | 0xXX   | 0xXX 0xXX | 无
|      |------------|-------------------|--------|-----------|------------------
|      | 0x00  0x01 | 0x03 用户静音超时 | 无     | 无        | 无
|      | 0x00  0x01 | 0x04 用户语音超时 | 无     | 无        | 无
|      | 0x00  0x01 | 0x05 识别拒识     | 无     | 无        | 无
|      | 0x00  0x01 | 0x06 识别内部错误 | 无     | 无        | 无
--------------------------------------------------------------------------------
*********************************************************************************/
uint8_t _SYN7318::Start_ASR_WEN(uint8_t Dict)
{
	uint8_t SYN_ID = 0;
	uint16_t str_len = 0;
	unsigned long t;
	
	uint8_t RX_Buf[9];
	uint8_t Inquire[4] = {0xFD,0x00,0x01,0x21};		// 模块状态查询
	uint8_t Start[5] = {0xFD,0x00,0x02,0x10,0x03};	// 模块开启识别 词典编号
	uint8_t YY_Random[8] = {0x55, 0x06, 0x20, 0x01, 0x00, 0x00, 0x21, 0xBB};   // 语音播报随机指令
	
	Serial2.write(Inquire, 4);		// 模块状态查询
	t = millis();
	while (true)		// 命令接收成功状态回传
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
	while (true)		// 模块状态回传
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
	Serial2.write(Start, 5);	// 开启语音识别
	
	t = millis();
	while (true)		// 命令接收成功状态回传
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
	delay(600);   //延时600ms
	
	ExtSRAMInterface.ExMem_Write_Bytes(0x6008, YY_Random, 8);
	delay(100);
	
	t = millis();
	while (true)
	{
		if (Serial2.available() > 0)
		{
			if (0xFC == Serial2.read())		     // 读取帧头
			{
				Serial2.readBytes(RX_Buf, 2);	// 读取数据长度
				str_len = (RX_Buf[0] << 8) + RX_Buf[1];		// 数据长度
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

