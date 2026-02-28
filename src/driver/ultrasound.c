#include "ultrasound.h"
#include "intrins.h"

sbit US_TX=P1^0;//发射
sbit US_RX=P1^1;//接收

void Delay12us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	i = 3;
	while (--i);
}

void Ut_wave_Init(){//发送频率为40M的方波
	unsigned char i;
	EA=0;
	for(i=0;i<8;i++){
		US_TX=1;
		Delay12us();
		US_TX=0;
		Delay12us();
	}
	EA=1;
}

unsigned char Ut_Wave_Data(){
	unsigned int time;
	CMOD=0X00;
	CH=CL=0;
	CF=0;
	Ut_wave_Init();
	CR=1;//PCA开始计时
	while((US_RX==1)&&(CF==0));//当接收到回波或者溢出的时候退出循环
	CR=0;
	//如果接收到回波 此时CF为0 因为没有溢出
	if(CF==0){
		time=CH<<8|CL;
		return (unsigned char)((unsigned long)time*17/1000);//cm
	}else{
		CF=0;
		return 0;
	}
}