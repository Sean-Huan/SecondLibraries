// infrare.h

#ifndef _INFRARE_h
#define _INFRARE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define RITX_PIN	13
#define TXD_ON		HIGH	//�͵�ƽ����ʾ�����
#define TXD_OFF		LOW		//�ߵ�ƽ����ʾ����ر�

class _Infrare
{
public:
	_Infrare(uint8_t _pin);
	_Infrare();
	~_Infrare();

	void Initialization(void);
	void Transmition(uint8_t */* s:���������*/,int/* n���ݳ���*/);

private:
	uint8_t pin;

};

extern _Infrare Infrare;

#endif

