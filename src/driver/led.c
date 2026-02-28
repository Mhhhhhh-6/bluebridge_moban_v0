#include "led.h"

//Y4C LED部分
idata unsigned char temp_1=0x00;//当前led状态
idata unsigned char temp_1_old=0xff;//上次led状态
//ucLed为led状态数组 元素值仅为0(灭) 1(亮) ucLed[0]为LED1 ucLed[7]为LED8 
void Led_Disp(unsigned char *ucLed){
	unsigned char temp;
	temp_1=0x00;//先清零
	temp_1=(ucLed[0]<<0)|(ucLed[1]<<1)|(ucLed[2]<<2)|(ucLed[3]<<3)|
		   (ucLed[4]<<4)|(ucLed[5]<<5)|(ucLed[6]<<6)|(ucLed[7]<<7);//整合打包LED状态
	//当led状态发生变化
	if(temp_1!=temp_1_old){
		P0=~temp_1;
		
		temp=P2&0x1f;//读取P2清空高三位 保留第四位的数据
		temp=temp|0x80;//赋值Y4对应的高三位 100
		P2=temp;//开门 P2接收temp的值 让P0数据流进去
		temp=P2&0x1f;//清空高三位
		P2=temp;//关门
		
		temp_1_old=temp_1;
	}
}

//快速关闭led
void Led_Off(){
	unsigned char temp;
	
	P0=0xff;
	
	temp=P2&0x1f;//读取P2清空高三位 保留第四位的数据
	temp=temp|0x80;//赋值Y4对应的高三位 100
	P2=temp;//开门 P2接收temp的值 让P0数据流进去
	temp=P2&0x1f;//清空高三位
	P2=temp;//关门
	
	temp_1_old=0x00;//更新
}

//Y5C buzz relay motor部分
idata unsigned char temp_2=0x00;//buzz relay motor的数据 放到一块去 因为都是Y5C
idata unsigned char temp_2_old=0xff;

//buzz
void Beep(bit enable){
	unsigned char temp;
	if(enable)
		temp_2|=0x40;//开buzz
	else 
		temp_2&=(~0x40);//关buzz
	
	if(temp_2!=temp_2_old){
		P0=temp_2;
			
		temp=P2&0x1f;//读取P2清空高三位 保留第四位的数据
		temp=temp|0xA0;//赋值Y5对应的高三位 101
		P2=temp;//开门 P2接收temp的值 让P0数据流进去
		temp=P2&0x1f;//清空高三位
		P2=temp;//关门
		
		temp_2_old=temp_2;
	}
}

//motor
void Motor(bit enable){
	unsigned char temp;
	if(enable)
		temp_2|=0x20;//开motor
	else 
		temp_2&=(~0x20);//关motor
	
	if(temp_2!=temp_2_old){
		P0=temp_2;
			
		temp=P2&0x1f;//读取P2清空高三位 保留第四位的数据
		temp=temp|0xA0;//赋值Y5对应的高三位 101
		P2=temp;//开门 P2接收temp的值 让P0数据流进去
		temp=P2&0x1f;//清空高三位
		P2=temp;//关门
		
		temp_2_old=temp_2;
	}
}

//relay
void Relay(bit enable){
	unsigned char temp;
	if(enable)
		temp_2|=0x10;//开motor
	else 
		temp_2&=(~0x10);//关motor
	
	if(temp_2!=temp_2_old){
		P0=temp_2;
			
		temp=P2&0x1f;//读取P2清空高三位 保留第四位的数据
		temp=temp|0xA0;//赋值Y5对应的高三位 101
		P2=temp;//开门 P2接收temp的值 让P0数据流进去
		temp=P2&0x1f;//清空高三位
		P2=temp;//关门
		
		temp_2_old=temp_2;
	}
}