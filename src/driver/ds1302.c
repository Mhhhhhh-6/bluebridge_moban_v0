/*	# 	DS1302代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/								

//
#include "ds1302.h"
#include "intrins.h"

sbit SCK=P1^7;
sbit SDA=P2^3;
sbit RST=P1^3;

void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

//
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

//
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}

//写入 ucRtc=0x23(时),0x59(分),0x55(秒)
void Set_Rtc(unsigned char *ucRtc){
	Write_Ds1302_Byte(0x8e,0x00);//关闭写保护 能够写入
	Write_Ds1302_Byte(0x80,0x80);//关闭CH 停止计时
	Write_Ds1302_Byte(0x84,ucRtc[0]);//时
	Write_Ds1302_Byte(0x82,ucRtc[1]);//分
	Write_Ds1302_Byte(0x80,ucRtc[2]);//秒 并重新对CH位修改 
	//这里很巧妙 因为秒的范围是0-60 对应0000 0000-0011 1100 最高位必为0 所以正好把CH位打开 开始计时
	Write_Ds1302_Byte(0x8e,0x80);//开启写保护
}

//读取 并存入ucRtc=0x23(时),0x59(分),0x55(秒)
void Read_Rtc(unsigned char *ucRtc){
	EA=0;
	ucRtc[0]=Read_Ds1302_Byte(0x85);//时
	ucRtc[1]=Read_Ds1302_Byte(0x83);//分
	ucRtc[2]=Read_Ds1302_Byte(0x81);//秒
	EA=1;
}
