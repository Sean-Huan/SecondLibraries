#include "Openmv.h"
#include "string.h"

_Openmv	  Openmv;

_Openmv::_Openmv()
{

}

_Openmv::~_Openmv()
{

}

/*---------------------二维码识别相关--------------------------*/

/*
功    能：OpenMV识别二维码启动函数
参    数：无
返 回 值：无
*/
void _Openmv::QR_Disc_StartUp(void)
{
	uint8_t openmv_com[8] = {0x55,0x02,0x92,0x01,0x00,0x00,0x00,0xBB };
	Command.Judgment(openmv_com);       //计算校验和
	ExtSRAMInterface.ExMem_Write_Bytes(0x6008, openmv_com, 8);
}


/*
功能：检查是否为数字字符
*/
uint8_t  _Openmv::Check(uint8_t *data)
{
	uint8_t state[3] = { 0x00,0x00,0x00 };
	if (data[2] == '*')
	{
		state[2] = 0x04;
	}
	if (data[1] == '*')
	{
		state[1] = 0x02;
	}
	if (data[0] == '*')
	{
		state[0] = 0x01;
	}
	return (state[0] + state[1] + state[2]);
}



/*
功能：计算值
*/
uint16_t _Openmv::judge(uint16_t a, uint16_t b, uint8_t mode)
{
	switch (mode)
	{
	case '+':
		return a + b;
		break;
	case '-':
		return a - b;
		break;
	case '*':
		return a * b;
		break;
	default:
		break;
	}
	return 0;
}


/*
功能：二维码数据处理
*/
uint8_t _Openmv::QR_Data_Handle(uint8_t *qr_data)
{
	uint16_t value = 0;
	uint8_t num_flag = 0;
	uint8_t num_buf = 0;
	uint8_t cont = 0;
	uint8_t symbol_buf[3];
	uint8_t num_value[8];
	Serial.print("QR_DATA:");
	Serial.println((char *)qr_data);
	for (int i = 0; i < strlen((char *)qr_data); i++)
	{
		if (qr_data[i] >= '0' && qr_data[i] <= '9')
		{
			num_buf = qr_data[i] - 0x30;   //转变为数字
			num_flag++;
			if (num_flag > 1)
			{
				num_flag = 0;
				value = value * 10 + num_buf;
			}
			else {
				value = num_buf;
			}
			if (i == strlen((char*)qr_data) - 1)   //最后一个数字
			{
				num_value[cont] = value;
			}
		}
		else {
			if (cont <= 2)
			{
				num_flag = 0;
				Serial.print("qr_data:");
				Serial.println(qr_data[i]);
				symbol_buf[cont] = qr_data[i];
			}
			num_value[cont] = value;
			cont++;
		}
	}

	Serial.print("symbol:");
	for (int i = 0; i < 3; i++)
	{
		Serial.print(" ");
		Serial.print(symbol_buf[i]);
	}
	Serial.println();

	Serial.print("num_value:");
	for (int i = 0; i < 4; i++)
	{
		Serial.print(" ");
		Serial.print(num_value[i]);
	}
	Serial.println();

	
	switch (Check(symbol_buf))
	{
	case 0x00:
		value = judge(num_value[0], num_value[1], symbol_buf[0]);
		value = judge(value, num_value[2], symbol_buf[1]);
		value = judge(value, num_value[3], symbol_buf[2]);
		break;
	case 0x01:
		value = judge(num_value[0], num_value[1], symbol_buf[0]);
		value = judge(value, num_value[2], symbol_buf[1]);
		value = judge(value, num_value[3], symbol_buf[2]);
		break;
	case 0x02:
		value = judge(num_value[0], judge(num_value[1], num_value[2], symbol_buf[1]), symbol_buf[0]);
		value = judge(value, num_value[3], symbol_buf[2]);
		break;
	case 0x03:
		value = judge(num_value[0], num_value[1], symbol_buf[0]);
		value = judge(value, num_value[2], symbol_buf[1]);
		value = judge(value, num_value[3], symbol_buf[2]);
		break;
	case 0x04:
		value = judge(num_value[0], num_value[1], symbol_buf[0]);
		value = judge(value, judge(num_value[2], num_value[3], symbol_buf[2]), symbol_buf[1]);
		break;
	case 0x05:
		value = judge(judge(num_value[0], num_value[1], symbol_buf[0])
			, judge(num_value[2], num_value[3], symbol_buf[2]), symbol_buf[1]);
		break;
	case 0x06:
		value = judge(num_value[1], num_value[2], symbol_buf[1]);
		value = judge(value, num_value[3], symbol_buf[2]);
		value = judge(num_value[0], value, symbol_buf[0]);
		break;
	case 0x07:
		value = judge(num_value[2], num_value[3], symbol_buf[2]);
		value = judge(value, num_value[1], symbol_buf[1]);
		value = judge(value, num_value[0], symbol_buf[0]);
		break;
	defalut:
		break;
	}

	Serial.print("VALUE:");
	Serial.println(value);
	return (value % 4 + 1);
}


/*
功能：二维码识别
参数：a_b  A为TFT（A） B为TFT(B)
返回值：二维码数据提取结果
*/
uint8_t _Openmv::QR_Distinguish(_Drive::A_B a_b)
{
	uint8_t Data_Type;
	uint8_t Data_Flag;
	uint8_t Data_Length;
	uint8_t qr_data_buf[20];
	uint32_t t = millis();

	Drive.Clear_FPGA_Addr_Data(0x6038);     //清除地址数据
	QR_Disc_StartUp();                      //启动二维码识别
	while((millis() -t) <= 20000)           //超时判断 20秒退出
	{
		if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)
		{
			Data_Type = ExtSRAMInterface.ExMem_Read(0x603A);
			Data_Flag = ExtSRAMInterface.ExMem_Read(0x603B);
			Data_Length = ExtSRAMInterface.ExMem_Read(0x603C);
			Data_Length = Data_Length + 6;
			ExtSRAMInterface.ExMem_Read_Bytes(0x6038, DataOTABuf, Data_Length);
			if ((DataOTABuf[0] == 0x55) && (DataOTABuf[1] == 0x02) && (DataOTABuf[2] == 0x92))
			{
				ExtSRAMInterface.ExMem_Write_Bytes(0x6080, DataOTABuf, Data_Length);  //使用自定义数据区上传OpenMV识别结果
				for (int i = 0; i < Data_Length; i++)
				{
					qr_data_buf[i] = DataOTABuf[i + 5];
					if (i == Data_Length-1)
					{
						qr_data_buf[i + 1] = '\0';
					}
				}
				memset(DataOTABuf,0,sizeof(DataOTABuf));
				return QR_Data_Handle(qr_data_buf);      //接收OpenMV，数据处理函数
			}
		}
		else {
		   //左右调整
			delay(500);
			delay(500);
			Drive.TFT_FY(a_b,2);   //翻页控制
		}
	}
	return 0;
}


/*---------------------交通灯识别------------------------*/

/*
功能：开启交通灯识别
参数：mode 默认为启动交通灯识别  2先自学习阈值再进行交通灯识别  
*/
void _Openmv::Traffic_Disc_StartUp(uint8_t mode = 1)
{
	uint8_t openmv_com[8] = { 0x55,0x02,0x91,0x01,0x00,0x00,0x00,0xBB };
	if (mode == 2)   
	{
		openmv_com[3] = 0xff;
	}
	Drive.ZigBee_Data_Send(openmv_com);
}

/*
功能：交通灯识别
参数：a_b  A为交通灯（A）  B为交通灯(B)
*/
void _Openmv::Traffic_Distinguish(_Drive::A_B a_b)
{
	uint32_t t = millis();
	uint8_t traffic_com[8] = {0x55,0x0e,0x00,0x00,0x00,0x00,0x00,0xbb};
	uint8_t data_buf[8];
	if (a_b == Drive.A)
	{
		traffic_com[1] = 0x0e;
	}
	else {
		traffic_com[1] = 0x0f;
	}
	traffic_com[2] = 0x01;   //进入识别模式
	Drive.ZigBee_Data_Send(traffic_com);
	Drive.ZigBee_Data_Send(traffic_com);
	Drive.ZigBee_Data_Send(traffic_com);
	delay(500);
	delay(500);

	Drive.Clear_FPGA_Addr_Data(0x6038);     //清除地址数据
	Traffic_Disc_StartUp();                 //启动交通灯识别

	while ((millis()-t) < 10000)
	{
		if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)
		{
			ExtSRAMInterface.ExMem_Read_Bytes(0x6038, data_buf, 8);
			if ((data_buf[0] == 0x55) && (data_buf[1] == 0x02) && (data_buf[2] == 0x91))
			{
				traffic_com[2] = 0x02;        //识别结果主指令
				traffic_com[3] = data_buf[4]; //识别结果
				Drive.ZigBee_Data_Send(traffic_com);
				Drive.ZigBee_Data_Send(traffic_com);
				Drive.ZigBee_Data_Send(traffic_com);
				break;                        //接收OpenMV，数据处理函数
			}
		}
	}
}


/*-----------------------图形识别—-----------------------*/

/*
功能：启动图形识别
*/
void _Openmv::Figure_Disc_StartUp()
{
	uint8_t openmv_com[8] = { 0x55,0x02,0x91,0x12,0x00,0x00,0x00,0xBB };
	Drive.ZigBee_Data_Send(openmv_com);
}

/*
功能：图形识别
参数：*data  用于存放识别结果   
      data[0] 表示红色识别结果
	  data[1] 表示绿色识别结果
	  data[2] 表示蓝色识别结果
*/
void _Openmv::Figure_Distinguish(uint8_t *data)
{
	uint8_t data_buf[8];
	uint32_t t = millis();

	Drive.Clear_FPGA_Addr_Data(0x6038);     //清除地址数据
	Figure_Disc_StartUp();                  //启动图形识别

	while ((millis() - t) <= 10000)
	{
		if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)
		{
			ExtSRAMInterface.ExMem_Read_Bytes(0x6038, data_buf, 8);
			if ((data_buf[0] == 0x55) && (data_buf[1] == 0x02) && (data_buf[2] == 0x91))
			{
				if (data_buf[3] == 0x02)
				{
					data[0] = data_buf[4];
					data[1] = data_buf[5];
					data[2] = data_buf[6];
					break;                        //接收OpenMV，数据处理函数
				}
			}
		}
	}
}


/*
功能：启用调试模式
*/
void _Openmv::Set_Debug_Mode()
{
	uint8_t openmv_com[8] = { 0x55,0x02,0x92,0x02,0x00,0x00,0x00,0xBB };
	Command.Judgment(openmv_com);       //计算校验和
	ExtSRAMInterface.ExMem_Write_Bytes(0x6008, openmv_com, 8);
}


/*
功能：关闭Debug模式
*/
void _Openmv::Res_Debug_Mode()
{
	uint8_t openmv_com[8] = { 0x55,0x02,0x92,0x03,0x00,0x00,0x00,0xBB };
	Command.Judgment(openmv_com);       //计算校验和
	ExtSRAMInterface.ExMem_Write_Bytes(0x6008, openmv_com, 8);
}



