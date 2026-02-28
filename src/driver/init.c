#include "init.h"
void System_Init(){
	unsigned char temp;
	
	//关闭LED
	P0=0xff;
	temp=P2&0x1f;//读取P2清空高三位 保留第四位的数据
	temp=temp|0x80;//赋值Y4对应的高三位 100
	P2=temp;//开门 P2接收temp的值 让P0数据流进去
	temp=P2&0x1f;//清空高三位
	P2=temp;//关门
	
	//关闭buzz relay motor
	P0=0x00;
	temp=P2&0x1f;//读取P2清空高三位 保留第四位的数据
	temp=temp|0xA0;//赋值Y5对应的高三位 101
	P2=temp;//开门 P2接收temp的值 让P0数据流进去
	temp=P2&0x1f;//清空高三位
	P2=temp;//关门
}
