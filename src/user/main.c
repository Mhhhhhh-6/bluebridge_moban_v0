#include <STC15F2K60S2.H>
#include "led.h"
#include "init.h"
#include "seg.h"
#include "key.h"

#include "ds1302.h"
#include "onewire.h"
#include "intrins.h"
#include "iic.h"
#include "ultrasound.h"
#include "uart.h"

#include "string.h"
#include "stdio.h"

idata unsigned long int uwTick;

							//1 2 3 4 5 6 7 8
pdata unsigned char ucLed[8]={0,0,0,0,0,0,0,0};
pdata unsigned char Seg_Buf[8]={16,16,16,16,16,16,16,16};
idata unsigned char Seg_Pos=0;

idata unsigned char Key_Val,Key_Old,Key_Up,Key_Down;
idata unsigned char Key_up_data,Key_down_data;

pdata unsigned char ucRtc[3]={0x23,0x59,0x55};

idata float Temperature;
idata unsigned int Temperature_100x;

idata unsigned int AD_1_Data_100x,AD_3_Data_100x;
idata unsigned char DA_data;

pdata unsigned char EEPROM_Data_W[8]={1,2,3,4,5,6,7,8};
pdata unsigned char EEPROM_Data_R[8]={0,0,0,0,0,0,0,0};

idata unsigned char Distance;

idata unsigned int Freq;
idata unsigned int Time_1s;

idata unsigned char pwm_period;//累加对比
idata unsigned char pwm_compare=6;//比较值 也就是亮度

idata unsigned char Uart_Rx_Index;//当前在第几个字节了
pdata unsigned char Uart_Rx_Buf[10]={0,0,0,0,0,0,0,0,0,0};//存放收到的数据 长度可变
idata unsigned char Uart_Rx_Flag;//是否正在接收
idata unsigned char Uart_Rx_Tick;//超时解析计时器

idata unsigned char Seg_Show_Mode;//0：时间 1：温度 2：AD 3：超声波 4：频率 5：PWM参数
	
void Key_Proc(){
	Key_Val=Key_Read();
	Key_Down=Key_Val&(Key_Val^Key_Old);
	Key_Up=~Key_Val&(Key_Val^Key_Old);
	Key_Old=Key_Val;
	if(Key_Down==4) pwm_compare=(++pwm_compare)%10;
	if(Key_Down!=0) printf("Key_Down:%bu",Key_Down);
	if(Key_Down==5) Seg_Show_Mode=(++Seg_Show_Mode)%6;
}

void Seg_Proc(){
	switch(Seg_Show_Mode){
		case 0:
			Seg_Buf[0]=ucRtc[0]/16;
			Seg_Buf[1]=ucRtc[0]%16;
			Seg_Buf[2]=17;
			Seg_Buf[3]=ucRtc[1]/16;
			Seg_Buf[4]=ucRtc[1]%16;
			Seg_Buf[5]=17;
			Seg_Buf[6]=ucRtc[2]/16;
			Seg_Buf[7]=ucRtc[2]%16;	
			break;
		case 1:
			Seg_Buf[0]=12;
			Seg_Buf[1]=16;
			Seg_Buf[2]=16;
			Seg_Buf[3]=16;
			Seg_Buf[4]=Temperature_100x/1000%10;
			Seg_Buf[5]=Temperature_100x/100%10+32;
			Seg_Buf[6]=Temperature_100x/10%10;
			Seg_Buf[7]=Temperature_100x%10;
			break;
		case 2:
			Seg_Buf[0]=AD_1_Data_100x/100%10+32;
			Seg_Buf[1]=AD_1_Data_100x/10%10;
			Seg_Buf[2]=AD_1_Data_100x%10;
			Seg_Buf[3]=16;
			Seg_Buf[4]=16;
			Seg_Buf[5]=AD_3_Data_100x/100%10+32;
			Seg_Buf[6]=AD_3_Data_100x/10%10;
			Seg_Buf[7]=AD_3_Data_100x%10;
			break;
		case 3:
			Seg_Buf[0]=16;
			Seg_Buf[1]=16;
			Seg_Buf[2]=16;
			Seg_Buf[3]=16;
			Seg_Buf[4]=Distance/10%10;
			Seg_Buf[5]=Distance%10;
			Seg_Buf[6]=12;
			Seg_Buf[7]=30;
			break;
		case 4:
			Seg_Buf[0]=16;
			Seg_Buf[1]=16;
			Seg_Buf[2]=16;
			Seg_Buf[3]=Freq/10000%10;
			Seg_Buf[4]=Freq/1000%10;
			Seg_Buf[5]=Freq/100%10;
			Seg_Buf[6]=Freq/10%10;
			Seg_Buf[7]=Freq%10;
			break;
		case 5:
			Seg_Buf[0]=16;
			Seg_Buf[1]=16;
			Seg_Buf[2]=16;
			Seg_Buf[3]=16;
			Seg_Buf[4]=16;
			Seg_Buf[5]=16;
			Seg_Buf[6]=pwm_compare/10%10;
			Seg_Buf[7]=pwm_compare%10;
			break;
	}
}

void Led_Proc(){
	ucLed[0]=1;
	ucLed[1]=0;
	ucLed[2]=1;
	ucLed[3]=0;
	ucLed[4]=1;
	ucLed[5]=0;
	ucLed[6]=1;
	ucLed[7]=0;
//	Led_Disp(ucLed);
}

void Get_Time(){
	Read_Rtc(ucRtc);
}

void Get_Temperature(){
	Temperature=rd_temperature();
	Temperature_100x=(unsigned int)(Temperature*100);
}

void AD_DA(){
	AD_1_Data_100x=(float)Ad_Read(0x41)/51.0f*100;//255->5
	AD_3_Data_100x=(float)Ad_Read(0x43)/51.0f*100;//255->5
	DA_data=3;
	Da_Write(DA_data*51);
}

void Get_Distance(){
	Distance=Ut_Wave_Data();
}

void Uart_Proc(){
	unsigned char x,y;
	if(Uart_Rx_Index==0) return;
	if(Uart_Rx_Tick>=10){//超时
		Uart_Rx_Tick=0;
		Uart_Rx_Flag=0;
		printf("%s",Uart_Rx_Buf);
		if(sscanf(Uart_Rx_Buf,"(%bu,%bu)",&x,&y)==2){
			printf("I get x=%bu,y=%bu\r\n",x,y);
		}else{
			printf("ERROR\r\n");
		}
		memset(Uart_Rx_Buf,0,Uart_Rx_Index);
		Uart_Rx_Index=0;
	}
}

void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TMOD |= 0x05;			//设置定时器模式,开启计数器模式并不自动重装
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0x00;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
}


void Timer1_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;			//定时器时钟12T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x18;				//设置定时初始值
	TH1 = 0xFC;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
	ET1 = 1;				//使能定时器1中断
	EA = 1; 
}

void Timer1_Isr(void) interrupt 3
{
	uwTick++;
	
	Seg_Pos=(++Seg_Pos)%8;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos]);
	
	if(++Time_1s>=1000){
		Time_1s=0;
		Freq=(TH0<<8)|TL0;
		TH0=0;
		TL0=0;
	}
	
	pwm_period=(++pwm_period)%10;
	if(pwm_period<pwm_compare){
		Led_Disp(ucLed);
	}else{
		Led_Off();
	}
	
	if(Uart_Rx_Flag) Uart_Rx_Tick++;
}

void Uart1_Isr(void) interrupt 4
{
	if (RI)				//检测串口1接收中断
	{
		Uart_Rx_Flag=1;//表示正在接收
		Uart_Rx_Tick=0;//重置计数器 从现在开始
		Uart_Rx_Buf[Uart_Rx_Index++]=SBUF;//存数据 下标后移
		RI = 0;			//清除串口1接收中断请求位
		if(Uart_Rx_Index>10){//如果满了就清空一下缓冲区 防止溢出
			Uart_Rx_Index=0;
			memset(Uart_Rx_Buf,0,10);
		}
	}
}

typedef struct{
	void(*task_func)(void);
	unsigned long int rate_ms;
	unsigned long int last_ms;
}task_t;

idata task_t Scheduler_Task[]={
	{Led_Proc,1,0},
	{Key_Proc,10,0},
	{Seg_Proc,20,0},
	{Get_Time,100,0},
	{Get_Temperature,300,0},
	{AD_DA,150,0},
	{Get_Distance,120,0},
	{Uart_Proc,10,0}
};

idata unsigned char task_num;
void Scheduler_Init(){
	task_num=sizeof(Scheduler_Task)/sizeof(task_t);
}

void Scheduler_Run(){
	unsigned char i;
	for(i=0;i<task_num;i++){
		unsigned long int now_time=uwTick;
		if(now_time>=(Scheduler_Task[i].rate_ms+Scheduler_Task[i].last_ms)){
			Scheduler_Task[i].last_ms=now_time;
			Scheduler_Task[i].task_func();
		}
	}
}

void main(){
	System_Init();
	
	Set_Rtc(ucRtc);
	
	EEPROM_Read(EEPROM_Data_R, 0, 8);  // 读取原始数据
	EEPROM_Write(EEPROM_Data_W, 0, 8); // 写入新数据
	EEPROM_Read(EEPROM_Data_R, 0, 8);  // 再次读取以验证写入
	
	Timer0_Init();
	Scheduler_Init();
	Uart1_Init();
	Timer1_Init();
	while(1){
		Scheduler_Run();
	}
}