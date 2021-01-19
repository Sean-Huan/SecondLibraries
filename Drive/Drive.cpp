#include "Drive.h"
#include "Openmv.h"

int getLightLevel = 1;

_Drive Drive;

_Drive::_Drive()
{

}

_Drive::~_Drive()
{

}


/*------------------------------------------
				**基本控制**
------------------------------------------*/
/*
功能：组合循迹
参数：循迹枚举中定义
*/
void _Drive::ZH_XJ(uint16_t xj_mode)
{
	switch (xj_mode)
	{
	case P_X:
		DCMotor.CarTrack(SPEED);    //普循
		break;
	case X:
		DCMotor.CarTrack(SPEED);    //循迹前进一小步
		DCMotor.Go(SPEED, GO_MP);
		break;
	case X_L:
		DCMotor.CarTrack(SPEED);    //循迹左转
		DCMotor.Go(SPEED, GO_MP);
		DCMotor.TurnLeft(SPEED);
		break;
	case X_R:
		DCMotor.CarTrack(SPEED);    //循迹右转
		DCMotor.Go(SPEED, GO_MP);
		DCMotor.TurnRight(SPEED);
		break;
	case L_X:
		DCMotor.TurnLeft(SPEED);
		DCMotor.CarTrack(SPEED);    //左转循迹
		DCMotor.Go(SPEED, GO_MP);
		break;
	case R_X:
		DCMotor.TurnLeft(SPEED);
		DCMotor.CarTrack(SPEED);    //右转循迹
		DCMotor.Go(SPEED, GO_MP);
		break;
	case LX_L:
		DCMotor.CarTrack(SPEED);    //连续循迹左转
		DCMotor.Go(SPEED, GO_MP);
		DCMotor.CarTrack(SPEED);
		DCMotor.Go(SPEED, GO_MP);
		DCMotor.TurnLeft(SPEED);
		break;
	case LX_R:
		DCMotor.CarTrack(SPEED);    //连续循迹右转
		DCMotor.Go(SPEED, GO_MP);
		DCMotor.CarTrack(SPEED);
		DCMotor.Go(SPEED, GO_MP);
		DCMotor.TurnRight(SPEED);
		break;
	case L_LX:
		DCMotor.TurnLeft(SPEED);
		DCMotor.CarTrack(SPEED);    //左转连续循迹
		DCMotor.Go(SPEED, GO_MP);
		DCMotor.CarTrack(SPEED);
		DCMotor.Go(SPEED, GO_MP);
		break;
	case R_LX:
		DCMotor.TurnRight(SPEED);
		DCMotor.CarTrack(SPEED);    //右转连续循迹
		DCMotor.Go(SPEED, GO_MP);
		DCMotor.CarTrack(SPEED);
		DCMotor.Go(SPEED, GO_MP);
		break;
	case LX:
		DCMotor.CarTrack(SPEED);    //连续循迹
		DCMotor.Go(SPEED,GO_MP);
		DCMotor.CarTrack(SPEED);
		DCMotor.Go(SPEED, GO_MP);
		break;
	default:
		DCMotor.CarTrack_MP(xj_mode);
		break;
	}
}


/*
功能：前进
参数：go_mp  前进的码盘
*/
void  _Drive::GO(uint16_t go_mp)
{
	DCMotor.Go(SPEED, go_mp);
}

/*
功能：后退
参数：go_mp  后退的码盘
*/
void  _Drive::BACK(uint16_t go_mp)
{
	DCMotor.Back(SPEED, go_mp);
}


/*
功能：方向矫正
参数：mode 为1时先后退到黑线再循迹  为2时先循迹再退到黑线
      cont 调整次数
      mp   码盘循迹所需的码盘
*/
void _Drive::FX_Correct(uint8_t mode=1, uint8_t cont = 3,uint16_t mp=350)
{
	if (mode == 1)
	{
		DCMotor.Back_Black_Line();
		for (int i = 0; i < cont; i++)
		{
			DCMotor.CarTrack_MP(mp, 30);
			DCMotor.Back_Black_Line();
		}
	}
	else {
		for (int i = 0; i < cont; i++)
		{
			DCMotor.CarTrack_MP(mp, 30);
			DCMotor.Back_Black_Line();
		}
	}

}
//方向矫正测距
void _Drive::FX_Correct_back()
{
	//DCMotor.Back_Black_Line();//后退到十字路口
	//DCMotor.CarTrack_MP(350, 70);//码盘循迹
	//DCMotor.Back_Black_Line();//后退到十字路口
	//DCMotor.CarTrack_MP(350, 70);//码盘循迹
	//DCMotor.Back_Black_Line();//后退到十字路口



		
		DCMotor.Back(70, 800);
		DCMotor.CarTrack(60);    //普循
		
		for (int i = 0; i < 2; i++)
		{
			DCMotor.Back(70, 700);
			DCMotor.CarTrack(50);    //普循
		}
		//DCMotor.CarTrack_MP(mp, 30);//码盘循迹
		/*DCMotor.Back_Black_Line();//后退到十字路口*/

	//DCMotor.Back(70, 200);
	//DCMotor.CarTrack(40);    //普循
}

/*
功能：转弯
参数：zw_mode  转弯模式
*/
void _Drive::ZW(uint8_t zw_mode)
{
	switch (zw_mode)
	{
	case L:    //左转
		DCMotor.TurnLeft(SPEED);
		break;
	case R:   //右转 
		DCMotor.TurnRight(SPEED);
		break;
	case DT:  //掉头
		DCMotor.TurnLeft(SPEED);
		DCMotor.TurnLeft(SPEED);
		break;
	case L_B:  //左半转
		DCMotor.TurnLeft(SPEED, SPEED);
		delay(B_DELAY);
		DCMotor.Stop();
		break;
	case R_B:  //右半转
		DCMotor.TurnRight(SPEED, SPEED);
		delay(B_DELAY);
		DCMotor.Stop();
		break;
	default:
		break;
	}
}


/*------------------------------------------
			   **标志物控制**
------------------------------------------*/


/*-----------------------------------------
类别：红外控制
包含：路灯控制、旋转LED控制、
------------------------------------------*/
/*
功能：发送4为红外数据
参数：*HW 红外数据
*/
void _Drive:: HW_4Send(uint8_t *HW)
{
	//发送四位  红外 数据
	Infrare.Transmition(HW, 4);
}

/*
功能：发送6为红外数据
参数：*HW 红外数据
*/
void _Drive:: HW_6Send(uint8_t *HW)
{
	//发送四位  红外 数据
	Infrare.Transmition(HW, 6);
}

/*
功能：固定挡位调光
参数：tiao  挡位
*/
void _Drive:: T_G(uint8_t tiao)//  单独调光
{
	if(tiao == 1)
		Infrare.Transmition(HW_Dimming1, 4);
	else if(tiao == 2)
		Infrare.Transmition(HW_Dimming2, 4);
	else if(tiao == 3)
		Infrare.Transmition(HW_Dimming3, 4);
	delay(200);
}

/*
功能：智能路灯自动调光
参数：gth_value 大于4为自动调光   小于4为固定挡位调光
*/
uint8_t _Drive:: Auto_T_G(uint8_t ght_value)
{
	DCMotor.Go(30,100);
	if(ght_value>4)
	{
		ght_value -= 4;
		uint8_t i;
		uint16_t ght_buf = 0;
		uint16_t buf = 0;
		delay(200);
		ght_buf = BH1750.ReadLightLevel();
		//测试光照强度	//测试光照强度
		for(i = 0; i < 4; i++)
		{
			Infrare.Transmition(HW_Dimming1, 4);
			delay(1200);
			buf = BH1750.ReadLightLevel();
			if(ght_buf <= buf)
				ght_buf = buf;
			else									//一种可能4-1挡  两一种可能3-4误判
				{
					ght_buf = buf;
					Infrare.Transmition(HW_Dimming1, 4);
					delay(1200);
					buf = BH1750.ReadLightLevel(); //测试光照强度
					if(ght_buf < buf ) //是一档
					{
						Infrare.Transmition(HW_Dimming3, 4);
						gz_res = 4 - (i);     //计算原始挡位
					}
					else if(ght_buf > buf)        //是4挡
					{
						Drive.Beep(1);
						gz_res = 4 - (i+1);  //计算原始挡位
						T_G(ght_value - 1);
					}
					break;
				}
		}
		delay(1200);
		T_G(ght_value - 1);
		delay(500);
	}
	else
	{	
		T_G(ght_value);
	}
	DCMotor.Back(30,100);
	return gz_res;
}

/*
功能：获取光照度
*/
uint8_t _Drive:: Get_Light()
{
	DCMotor.Go(100,100);
	uint16_t ght_buf = 0;
	uint16_t buf = 0;
	uint8_t i=0;
	delay(200);
	ght_buf = BH1750.ReadLightLevel();			//测试光照强度
	for(i= 0; i < 4; i++)
	{
		Infrare.Transmition(HW_Dimming1, 4);
		delay(1200);
		buf = BH1750.ReadLightLevel();
		if(ght_buf <= buf)
		{
			ght_buf = buf;
		}
		else	//一种可能4-1挡  两一种可能3-4误判
		{
				ght_buf = buf;
				Infrare.Transmition(HW_Dimming1, 4);
				delay(1200);
				buf =BH1750.ReadLightLevel();
				if(ght_buf < buf ) //是一档
				{
					Infrare.Transmition(HW_Dimming3, 4);
					gz_res = 4 - (i); //计算原始挡位
				}
				else if(ght_buf > buf)  //是4挡
				{
					gz_res = 4 - (i+1); //计算原始挡位
				}
				break;
		}
	}
	delay(1000);
	T_G(4 - (i) - 1);
	Drive.Beep(gz_res);
	DCMotor.Back(100,100);
	return gz_res;//
}



/*
功能：旋转LED显示距离信息
参数：res  距离值
*/
void _Drive:: X_Z_JL(uint16_t res)//   红外发射显示距离
{
	uint8_t HW_J[6] = {0xff, 0x11, 0x00, 0x00, 0x00, 0x00};
	HW_J[2] = 0x30 + (res/ 100 % 10); //个位十位
	HW_J[3] = 0x30 + (res/ 10 % 10); //
	Infrare.Transmition(HW_J, 6);//   HW   send
}



/*
 ** 旋转Led 显示 图形 颜色
 mode   模式  data 数据

0X01 矩形
0X02 圆形
0X03 三角形
0X04  菱形
0X05  梯形
0X06 饼图
0X07  靶图
0X08  条形图

0X01  红色
0X02  绿色
0X03  蓝色
0X04  黄色
0X05  紫色
0X06  青色
0X07  黑色
0X08  白色
*/
void _Drive:: XZ_YS(uint8_t xz_data)
{
	uint8_t XZ_DATA[6];
	XZ_DATA[0] = 0xff;
	XZ_DATA[1] = 0x13;
	XZ_DATA[2] = xz_data;
	XZ_DATA[3] = 0;
	XZ_DATA[4] = 0;
	XZ_DATA[5] = 0;
	Infrare.Transmition(XZ_DATA,6);
}

/*
功能：旋转LED显示图形
参数：xz_data  数据
*/
void _Drive:: XZ_TX(uint8_t xz_data)
{
	uint8_t XZ_DATA[6];
	XZ_DATA[0] = 0xff;
	XZ_DATA[1] = 0x12;
	XZ_DATA[2] = xz_data;
	XZ_DATA[3] = 0;
	XZ_DATA[4] = 0;
	XZ_DATA[5] = 0;
	Infrare.Transmition(XZ_DATA,6);
}


/*
功能：旋转LED显示图形及颜色及其他
参数：xz_mode 模式   xz_data 数据
*/
void _Drive::XZ_TX_YS(uint8_t xz_mode, uint8_t xz_data)
{
	uint8_t XZ_DATA[6];      //旋转LED的数据
	XZ_DATA[0] = 0xff;
	if(xz_mode == 0x01)      //显示形状
		XZ_DATA[1] = 0x12;
	else if(xz_mode == 0x02) //显示颜色
		XZ_DATA[1] = 0x13;
	else if(xz_mode == 0x03) //显示路况
		XZ_DATA[1] = 0x14;
	XZ_DATA[2] = xz_data;
	XZ_DATA[3] = 0;
	XZ_DATA[4] = 0;
	XZ_DATA[5] = 0;
	Infrare.Transmition(XZ_DATA,6);
}


/*
功能：旋转LED显示车牌及坐标
参数：*x_z_data 待显示的数据
*/
void _Drive:: XZ_LED_CarPlate(uint8_t *x_z_data)
{
	uint8_t XZ_DATA[6];
	uint8_t i;
	XZ_DATA[0] = 0xff;
	XZ_DATA[1] = 0x20;
	for(i=0; i<4; i++)
	{
		XZ_DATA[i+2] = x_z_data[i];
	}
	delay(300);
	Infrare.Transmition(XZ_DATA,6);
	delay(200);
	XZ_DATA[1] = 0x10;
	for(i=0; i<4; i++)
	{
		XZ_DATA[i+2] = x_z_data[i+4];
	}
	Infrare.Transmition(XZ_DATA,6);
}



/*
功能：开报警器(默认红外码)
*/
void _Drive::Beeper()
{
	//  打开报警器
	uint8_t HW_K[6] = { 0x03,0x05,0x14,0x45,0xDE,0x92 };
	//uint8_t HW_K[6] = { 0x01,0x02,0x02,0x04,0x07,0x03 };
	Infrare.Transmition(HW_K, 6);
}


/*
功能：开启报警器
参数： *x_z_data 红外控制码
*/
void _Drive::Beeper(uint8_t *x_z_data)
{
	
	Infrare.Transmition(x_z_data, 6);
}



/*
超声波测距
*/
void _Drive::Csb()
{
	delay(1500);
	uint16_t csb1 = (uint16_t)(Ultrasonic.Ranging(CM) * 10.0);
	csb1 += 5;				     //si  舍  wu 入
	JL_Z = csb1 / 10;			 //传送 距离值
	Serial.println(JL_Z);
}


/*
功能：蜂鸣器控制
参数：res  响的次数
*/
void _Drive:: Beep(uint8_t res)
{
	for(int i=0; i<res; i++)
	{
		BEEP.TurnOn();
		delay(500);
		BEEP.TurnOff();
		delay(500);
	}
}

/*
功能：转向灯控制
参数：res 0x10 左转向灯亮,0x01 右转向灯亮,0x11 全亮,0x00 全灭
*/
void _Drive:: Z_X_Led(uint8_t res)
{
	switch(res)
	{
		case 0x10:
			LED.LeftTurnOn();
			LED.RightTurnOff();
			break;
		case 0x01:
			LED.RightTurnOn();
			LED.LeftTurnOff();
			break;
		case 0x11:
			LED.LeftTurnOn();
			LED.RightTurnOn();
			break;
		case 0x00:
			LED.RightTurnOff();
			LED.LeftTurnOff();
			break;
	}
	if(res>4)
	{
		uint32_t ms=(uint32_t)(res*100);
		LED.LeftTurnOn();
		LED.RightTurnOn();
		delay(ms);
		LED.RightTurnOff();
		LED.LeftTurnOff();
	}
}

/*
功能：开启双闪加蜂鸣器
参数：res 次数
*/
void _Drive:: Double_Led(uint8_t res)
{
	LED.LeftTurnOn();
	LED.RightTurnOn();
	for(int i=0; i<3; i++)
	{
		BEEP.TurnOn();
		delay(500);
		BEEP.TurnOff();
		delay(500);
	}
	LED.RightTurnOff();
	LED.LeftTurnOff();
}





/*----------------------------------------------------------
类别：ZigBee标志物控制
包含：立体车库(A/B)、交通灯(A/B)、TFT(A/B)、道闸
----------------------------------------------------------*/

/*
功能：ZigBee数据发送函数
参数：*data  待发送的数据
*/
void _Drive::ZigBee_Data_Send(uint8_t *data)
{
	Command.Judgment(data);         //计算校验和
	ExtSRAMInterface.ExMem_Write_Bytes(0x6008, data, 8);
}


/*
功能：获取ZigBee反馈数据
*/
void _Drive::Get_ZigBee_Data()
{
	if (ExtSRAMInterface.ExMem_Read(0x6100) != 0x00)           //从车接收ZigBee数据
	{
		ExtSRAMInterface.ExMem_Read_Bytes(ZigBee_command, 8);
	}
}

/*
功能：清除ZigBee地址数据
*/
void _Drive::Clear_ZigBee_Data()
{
	uint8_t clear_data[8] = {0,0,0,0,0,0,0,0};
	ExtSRAMInterface.ExMem_Write_Bytes(0x6100, clear_data, 8);
}


/*
功能：清除FPGA地址数据
*/
void _Drive::Clear_FPGA_Addr_Data(uint32_t addr)
{
	uint8_t clear_data[8] = { 0,0,0,0,0,0,0,0 };
	ExtSRAMInterface.ExMem_Write_Bytes(addr, clear_data, 8);
}


/*
功能：获取立体车库层数
返回值： 立体车库层数 1~4
*/
uint8_t _Drive::C_K_Get_Layer(A_B a_b)
{
	uint8_t c_k_com[8] = { 0x55,0x0d,0x02,0x01,0x00,0x00,0x00,0xbb };

	if (a_b == A)
	{
		c_k_com[1] = 0x0d;
	}
	else {
		c_k_com[1] = 0x05;
	}
	ZigBee_Data_Send(c_k_com);
	delay(500);
	Get_ZigBee_Data();
	if (ZigBee_command[1] == c_k_com[1])
	{
		if (ZigBee_command[2] == 0x03)
		{
			return ZigBee_command[4];
		}
	}
}

/*
功能：立体车库控制
参数：layer  1~4层
*/
void _Drive::C_K_Set_Layer(A_B a_b, uint8_t layer)
{
	uint8_t c_k_com[8] = {0x55,0x0d,0x01,0x00,0x00,0x00,0x00,0xbb};
	uint32_t t = millis();
	if (a_b == A)
	{
		c_k_com[1] = 0x0d;
	}
	else {   
		c_k_com[1] = 0x05;
	}
	
	c_k_com[3] = layer;
	memset(ZigBee_command, 0, sizeof(ZigBee_command));
	Clear_ZigBee_Data();              //清除数据 
	while((millis() - t) <= 45000)    //45秒
	{
		ZigBee_Data_Send(c_k_com);
		delay(100);
		if (C_K_Get_Layer(a_b) == layer)
		{
			BEEP.TurnOn();
			delay(100);
			BEEP.TurnOff();
			delay(3000);
			break;
		}
	}
}

/*
功能：TFT显示距离信息
参数：jl  距离信息
*/
void _Drive::TFT_JL(A_B a_b,uint16_t jl)
{
	uint8_t TFT_J[8];
	uint8_t buf_jl = 0;
	if (a_b == A)
	{
		TFT_J[1] = 0x0B;
	}
	else {
		TFT_J[1] = 0x08;
	}
	TFT_J[0] = 0x55;
	TFT_J[2] = 0X50;
	TFT_J[3] = 0X00;
	TFT_J[4] = jl / 100 % 10;
	buf_jl = (jl / 10 % 10);
	buf_jl = (buf_jl << 4) | (jl % 10);
	TFT_J[5] = buf_jl;
	TFT_J[6] = (TFT_J[2] + TFT_J[3] + TFT_J[4] + TFT_J[5]) % 256;
	TFT_J[7] = 0XBB;

	ZigBee_Data_Send(TFT_J);
}



/*
功能：TFT翻页控制
参数：tft_fy_mode TFT翻页模式 1为上翻  2为下翻 3为自动翻页
*/
void _Drive::TFT_FY(A_B a_b, uint8_t tft_fy_mode)
{
	uint8_t TFT_F[8] = {0x55,0x00,0x10,0x00,0x00,0x00,0x00,0xBB};

	if (a_b == A)
	{
		TFT_F[1] = 0x0B;
	}
	else {
		TFT_F[1] = 0x08;
	}

	switch (tft_fy_mode)
	{
	case 1:    //上翻
		TFT_F[3] = 0x01;

		break;
	case 2:  //下翻
		TFT_F[3] = 0x02;
		break;
	case 3:  //自动
		TFT_F[3] = 0x03;
		break;
	default:
		break;
	}
	ZigBee_Data_Send(TFT_F);
}


/*
功能：获取道闸开启状态
*/
void  _Drive::Road_Gate_Test()
{
	uint8_t open_road_buf[] = { 0x55,0x03,0x20,0x01,0x00,0x00,0x00,0xBB };
	Command.Judgment(open_road_buf);         //计算校验和
	ExtSRAMInterface.ExMem_Write_Bytes(0x6008, open_road_buf, 8);
}



/*
功能：道闸显示车牌
参数：*dat   待发送的车牌字符
*/
void _Drive::Strobe_Display_Plate(uint8_t* dat)
{
	uint8_t DZ_Data[8] = {0x55, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};
	DZ_Data[2] = 0x10;
	DZ_Data[3] = dat[0];
	DZ_Data[4] = dat[1];
	DZ_Data[5] = dat[2];
	DZ_Data[6] = (DZ_Data[2] + DZ_Data[3]
		+ DZ_Data[4] + DZ_Data[5]) % 256;
	ZigBee_Data_Send(DZ_Data);
	ZigBee_Data_Send(DZ_Data);
	DZ_Data[2] = 0x11;
	DZ_Data[3] = dat[3];
	DZ_Data[4] = dat[4];
	DZ_Data[5] = dat[5];
	DZ_Data[6] = (DZ_Data[2] + DZ_Data[3]
		+ DZ_Data[4] + DZ_Data[5]) % 256;
	ZigBee_Data_Send(DZ_Data);
	ZigBee_Data_Send(DZ_Data);
}



/*-----------------------------------------------------------
类别：接收数据处理与任务执行
包含：全自动执行函数、ZigBee数据处理函数、数据接收通信交互
------------------------------------------------------------*/

/*
功能：任务结束后的数据反馈
说明：注意调用位置，使用它会提前告诉主车任务执行完毕
*/
void _Drive:: Task_END_Data_Send()
{
	for (int j = 0; j < 6; j++)
	{
		ZigBee_back[2] = 0x41;
		ZigBee_back[3] = 0xAA;
		for (int i = 0; i < 6; i++) {
			ZigBee_back[i + 4] = Openmv.qr_data[i];
		}
		ExtSRAMInterface.ExMem_Write_Bytes(0x6080, ZigBee_back, 12);  //发送忙碌
		delay(200);
	}
}


/*
功能：获取图形识别结果
*/
void _Drive::Get_Figure_Data(uint8_t *data)
{
	ZigBee_back[4] = data[0];
	ZigBee_back[5] = data[1];
	ZigBee_back[6] = data[2];
}

/*
功能：获取距离值
*/
void _Drive::Get_JL_Value()
{
	ZigBee_back[3] = uint8_t(JL_Z);
}
//上1，右2，下3，左4.
char _Drive::CheckRoad(char dir, char Alpha, char Num) {
	if (dir == 1) {
		if ((Alpha == -2) && (Num == 0)) {
			//Left
			DCMotor.TurnLeft(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);
			dir = 4;
		}
		else if ((Alpha == 0) && (Num == -2)) {
			//FW
			ZH_XJ(X);
			delay(500);
		}
		else if ((Alpha == 2) && (Num == 0)) {
			//Right
			DCMotor.TurnRight(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);
			dir = 2;
		}
		else if ((Alpha == -1) && (Num == 0)) {
			//Left
			DCMotor.TurnLeft(SPEED);
			delay(500);
			DCMotor.Go(SPEED, 1260);
			delay(500);
			dir = 4;
		}
		else if ((Alpha == 0) && (Num == -1)) {
			//FW
			DCMotor.Go(SPEED, 1260);
			delay(500);
		}
		else if ((Alpha == 1) && (Num == 0)) {
			//Right
			DCMotor.TurnRight(SPEED);
			delay(500);
			DCMotor.Go(SPEED, 1260);
			delay(500);
			dir = 2;
		}
		else if ((Alpha == 0) && (Num == 2)) {
			DCMotor.TurnRight(SPEED);
			delay(500);
			DCMotor.TurnRight(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);
			dir = 3;
		}
	}
	else if (dir == 2) {
		if ((Alpha == 0) && (Num == -2)) {
			//Left
			DCMotor.TurnLeft(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);
			dir = 1;
		}
		else if ((Alpha == 2) && (Num == 0)) {
			//FW
			ZH_XJ(X);
			delay(500);
		}
		else if ((Alpha == 0) && (Num == 2)) {
			//Right
			DCMotor.TurnRight(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);
			dir = 3;
		}
		else if ((Alpha == 0) && (Num == -1)) {
			//Left
			DCMotor.TurnLeft(SPEED);
			delay(500);
			DCMotor.Go(SPEED, 1260);
			delay(500);
			dir = 1;
		}
		else if ((Alpha == 1) && (Num == 0)) {
			//FW
			DCMotor.Go(SPEED, 1260);
			delay(500);
		}
		else if ((Alpha == 0) && (Num == 1)) {
			//Right
			DCMotor.TurnRight(SPEED);
			delay(500);
			DCMotor.Go(SPEED, 1260);
			delay(500);
			dir = 3;
		}
		else if ((Alpha == -2) && (Num == 0)) {
			DCMotor.TurnRight(SPEED);
			delay(500);
			DCMotor.TurnRight(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);
			dir = 4;
		}
	}
	else if (dir == 3) {
		if ((Alpha == 2) && (Num == 0)) {
			//Left
			DCMotor.TurnLeft(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);
			dir = 2;
		}
		else if ((Alpha == 0) && (Num == 2)) {
			//FW
			ZH_XJ(X);
			delay(500);
		}
		else if ((Alpha == -2) && (Num == 0)) {
			//Right
			DCMotor.TurnRight(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);
			dir = 4;
		}
		else if ((Alpha == 1) && (Num == 0)) {
			//Left
			DCMotor.TurnLeft(SPEED);
			delay(500);
			DCMotor.Go(SPEED, 1260);
			delay(500);
			dir = 2;
		}
		else if ((Alpha == 0) && (Num == 1)) {
			//FW
			DCMotor.Go(SPEED, 1260);
			delay(500);
		}
		else if ((Alpha == -1) && (Num == 0)) {
			//Right
			DCMotor.TurnRight(SPEED);
			delay(500);
			DCMotor.Go(SPEED, 1260);
			delay(500);
			dir = 4;
		}
		else if ((Alpha == 0) && (Num == -2)) {
			DCMotor.TurnRight(SPEED);
			delay(500);
			DCMotor.TurnRight(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);
			dir = 1;
		}
	}
	else if (dir == 4) {
		if ((Alpha == 0) && (Num == 2)) {
			//Left
			DCMotor.TurnLeft(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);
			dir = 3;
		}
		else if ((Alpha == -2) && (Num == 0)) {
			//FW
			ZH_XJ(X);
			delay(500);
		}
		else if ((Alpha == 0) && (Num == -2)) {
			//Right
			DCMotor.TurnRight(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);
			dir = 1;
		}
		else if ((Alpha == 0) && (Num == 1)) {
			//Left
			DCMotor.TurnLeft(SPEED);
			delay(500);
			DCMotor.Go(SPEED, 1260);
			delay(500);
			dir = 3;
		}
		else if ((Alpha == -1) && (Num == 0)) {
			//FW
			DCMotor.Go(SPEED, 1260);
			delay(500);
		}
		else if ((Alpha == 0) && (Num == -1)) {
			//Right
			DCMotor.TurnRight(SPEED);
			delay(500);
			DCMotor.Go(SPEED, 1260);
			delay(500);
			dir = 1;
		}
		else if ((Alpha == 2) && (Num == 0)) {
			DCMotor.TurnRight(SPEED);
			delay(500);
			DCMotor.TurnRight(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);
			dir = 2;
		}
	}
	return dir;
}
/*
功能：全自动执行
参数：AUTO_MODE  对应0~10个路线
*/
uint8_t roadWay[20];
uint8_t OpenCode[] = { 0x01,0x02,0x02,0x04,0x07,0x03 };
void _Drive::Auto_Drive(AUTO_MODE auto_mode)
{
	uint8_t rgb_data[3];
	uint8_t layer = 0;
	int dir = 2;
	switch (auto_mode)
	{
		case AUTO_01:
			BEEP.TurnOn();
			delay(200);
			BEEP.TurnOff();
			delay(200);

			if (roadWay[6] == 1) {
				DCMotor.TurnLeft(SPEED);
				delay(500);
				DCMotor.TurnLeft(SPEED);
				delay(500);
			}

			ZH_XJ(X);
			delay(500);

			DCMotor.TurnLeft(SPEED);
			delay(500);

			ZH_XJ(X);
			delay(500);

			DCMotor.TurnLeft(SPEED);
			delay(500);

			ZH_XJ(X);
			delay(500);

			DCMotor.TurnLeft(SPEED);
			delay(500);
			ZW(L_B);
			delay(500);
			Beeper(roadWay);
			delay(500);

			DCMotor.TurnLeft(SPEED);
			delay(500);



			if (roadWay[7] == 1) {
				ZH_XJ(X);
				delay(500);

				DCMotor.TurnLeft(SPEED);
				delay(500);
				DCMotor.TurnLeft(SPEED);
				delay(500);
			}
			else if (roadWay[7] == 2) {
				ZH_XJ(X);
				delay(500);

				DCMotor.TurnRight(SPEED);
				delay(500);
				ZH_XJ(X);
				delay(500);
				ZH_XJ(X);
				delay(500);
				DCMotor.TurnRight(SPEED);
				delay(500);
			}
			else if (roadWay[7] == 3) {
				ZH_XJ(X);
				delay(500);

				DCMotor.TurnRight(SPEED);
				delay(500);
				ZH_XJ(X);
				delay(500);
				DCMotor.TurnRight(SPEED);
				delay(500);
			}

			FX_Correct();
			delay(500);
			DCMotor.Back(SPEED, BACK_C_K);
			delay(500);
			Task_END_Data_Send();

			break;
		case AUTO_02:
			if (roadWay[3] == 'U') {
				DCMotor.TurnRight(SPEED);
				delay(500);
			}
			else if (roadWay[3] == 'D') {
				DCMotor.TurnLeft(SPEED);
				delay(500);
			}

			if ((roadWay[1] == 'B') && (roadWay[2] == '6')) {
				ZH_XJ(X);
				delay(500);
			}

			ZH_XJ(X);
			delay(500);
			
			uint8_t TFT_LockLoc[8] = { 0x55,0x0B,0x10,0x00,roadWay[4],0x00,0x00,0xBB };
			ZigBee_Data_Send(TFT_LockLoc);
			/*for (int i = 0; i < roadWay[4]; i++) {
				TFT_FY(A, 2);
				delay(500);
			}*/

			DCMotor.TurnLeft(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);

			DCMotor.TurnLeft(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);

			DCMotor.TurnRight(SPEED);
			delay(500);
			ZW(R_B);
			delay(500);
			Beeper();
			DCMotor.TurnLeft(SPEED);
			delay(500);

			ZH_XJ(X);
			delay(500);

			uint8_t TFT_GetLight[8] = { 0x55,0x04,0x04,0x00,0x00,Get_Light(),0x00,0xBB };
			ZigBee_Data_Send(TFT_GetLight);

			DCMotor.TurnLeft(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);

			DCMotor.TurnLeft(SPEED);
			delay(500);
			ZH_XJ(X);
			delay(500);

			ZH_XJ(X);
			delay(500);

			DCMotor.TurnLeft(SPEED);
			delay(500);
			DCMotor.TurnLeft(SPEED);
			delay(500);
			FX_Correct();

			DCMotor.Back(SPEED, BACK_C_K);
			Task_END_Data_Send();
			break;
		case AUTO_03:  
			Csb();           //超声波测距
			Get_JL_Value();  //获取距离值
			ZigBee_back[2] = 0xD1;
			ExtSRAMInterface.ExMem_Write_Bytes(0x6080, ZigBee_back, 12);  //发送忙碌
			break;
		case AUTO_04:  break;
		case AUTO_05:  break;
		case AUTO_06:  break;
		case AUTO_07:  break;
		case AUTO_08:  break;
		case AUTO_09:  break;
		case AUTO_10:  break;
		default:
			break;
	}
}

/*
功能：ZigBee命令处理函数
参数：ZigBee接收到的数据
*/
void _Drive::Analyze_Handle(uint8_t *com)
{
	uint8_t rgb_data[3];
		switch (com[3])   //com[3] 为要启动的全自动路线
		{
		case 0x01:      //全自动启动  
			getLightLevel = com[5];
			Auto_Drive(com[4]);
			break;
		case 0x02:      //二维码识别
			BEEP.TurnOn();
			Openmv.QR_Distinguish(Drive.A);
			break;
		case 0x03:      //交通识别
			Openmv.Traffic_Distinguish(Drive.B);
			break;
		case 0x04:      //自动学习交通灯颜色阈值
			Openmv.Traffic_Disc_StartUp(2);
			break;
		case 0x05:      //图形识别
			Openmv.Figure_Distinguish(rgb_data);
			ZigBee_back[4] = rgb_data[0];
			ZigBee_back[5] = rgb_data[1];
			ZigBee_back[6] = rgb_data[2];
			break;
		case 0x06:
			break;
		}
}

/*
功能：从车任务执行函数
*/
void _Drive::Task_Out()
{
	uint8_t zigbee_data[8];
	uint8_t zigbee_judge = 0;
	if (ExtSRAMInterface.ExMem_Read(0x6100) != 0x00)            //从车接收ZigBee数据
	{
		ExtSRAMInterface.ExMem_Read_Bytes(zigbee_data, 8);
		zigbee_judge = zigbee_data[6];                          //获取校验和
		Command.Judgment(zigbee_data);                          //计算校验和
		if (zigbee_data[2] == 0x86) {
			BEEP.TurnOn();
			delay(200);
			roadWay[0] = zigbee_data[3];
			roadWay[1] = zigbee_data[4];
			roadWay[2] = zigbee_data[5];
		}
		if (zigbee_data[2] == 0x87) {
			roadWay[3] = zigbee_data[3];
			roadWay[4] = zigbee_data[4];
			roadWay[5] = zigbee_data[5];
		}
		if (zigbee_data[2] == 0x88) {
			roadWay[6] = zigbee_data[3];
			roadWay[7] = zigbee_data[4];
			roadWay[8] = zigbee_data[5];
		}
		if (zigbee_data[2] == 0x89) {
			roadWay[9] = zigbee_data[3];
			roadWay[10] = zigbee_data[4];
			roadWay[11] = zigbee_data[5];
		}
		if (zigbee_data[2] == 0x90) {
			roadWay[12] = zigbee_data[3];
			roadWay[13] = zigbee_data[4];
			roadWay[14] = zigbee_data[5];
		}
		if (zigbee_data[2] == 0x91) {
			roadWay[15] = zigbee_data[3];
			roadWay[16] = zigbee_data[4];
			roadWay[17] = zigbee_data[5];
		}
		if (zigbee_data[2] == 0x92) {
			roadWay[18] = zigbee_data[3];
			roadWay[19] = zigbee_data[4];
			roadWay[20] = zigbee_data[5];
			BEEP.TurnOff();
		}

		if ((zigbee_judge == zigbee_data[6])&& (zigbee_judge != 0 && zigbee_data[2] == 0xcc)
			&& (zigbee_data[0] == 0x55) && (zigbee_data[7] == 0xBB))
		{
			for (int j = 0; j < 6; j++)
			{
				ZigBee_back[2] = 0x31;
				ExtSRAMInterface.ExMem_Write_Bytes(0x6080, ZigBee_back, 12);  //发送忙碌
				delay(200);
			}
			Analyze_Handle(zigbee_data);                   //执行指令
			//反馈任务结束标志
			for (int j = 0; j < 6; j++)
			{
				ZigBee_back[2] = 0x41;
				ExtSRAMInterface.ExMem_Write_Bytes(0x6080, ZigBee_back, 12);  //发送忙碌
				delay(200);
			}
			Clear_ZigBee_Data();   //清除数据防止重复操作
		}
	}
}

