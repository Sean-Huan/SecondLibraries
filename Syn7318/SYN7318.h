// SYN7318.h

#ifndef _SYN7318_h
#define _SYN7318_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


#define SYN7318_RST_ON			LOW			/*低电平：使能模块复位*/
#define SYN7318_RST_OFF			HIGH		/*高电平：禁止模块复位*/

#define SYN7318_FRAME_RH		0xFC
#define SYN7318_FRAME_SH		0xFD

#define RESET_PIN				PIN_A13

#define STOP_FLAG				0
#define SUSPEND_FLAG			1
#define RESTORE_FLAG			2
#define VOLUME_FLAG				3

class _SYN7318
{
public:
	_SYN7318();
	~_SYN7318();

	void Initialization(void);							/*初始化相关接口及变量*/
								
	void VSPTest(const char* text, uint8_t textcode);	/*语音合成播报函数*/
	void Start_ASR(uint8_t Dict);				/*语音识别测试函数*/
	uint8_t Start_ASR_return(uint8_t Dict,bool whatID);
	void Start_ASR_send(uint8_t Dict);		/*发起语音识别*/
	uint8_t Start_ASR_rec(bool whatID);		/*返回识别结果*/
	uint8_t Start_ASR_WEN(uint8_t Dict);
	
private:
	uint8_t REc_Flag;
	uint8_t Back_Flag;
	void _SYN7318::ResetCom(void);
	bool _SYN7318::ResetCheck(void);
	void ResetTest(void);								/*SYN7318复位函数*/
	void _SYN7318::QueryStatus(void);					/*状态查询、模块忙碌复位*/
	bool CommandSendCheck(void);
	bool CommandSendCheckBusy(void);
	bool CommandSendCheckASR(void);
	bool _SYN7318::CommandSendCheckACK(void);
	void _SYN7318::VoiceSynthesisPlayback(const char* text,uint8_t textcode);
	void _SYN7318::VSPCom(const char* text, uint8_t textcode);



	uint8_t respose_com[4] = { 0x00, 0x00, 0x00, 0x00 };
	uint8_t SSRId[5] = { 0x00, 0x00, 0x00, 0x00 ,0x00 };

};

extern _SYN7318 SYN7318;



#endif

