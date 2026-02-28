/*	#   I2C代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/

#include "iic.h"
#include "intrins.h"

#define DELAY_TIME	5   //原来是10 最好改成5

sbit scl=P2^0;
sbit sda=P2^1;
//
static void I2C_Delay(unsigned char n)
{
    do
    {
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();		
    }
    while(n--);      	
}

//
void I2CStart(void)
{
    sda = 1;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 0;
	I2C_Delay(DELAY_TIME);
    scl = 0;    
}

//
void I2CStop(void)
{
    sda = 0;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 1;
	I2C_Delay(DELAY_TIME);
}

//
void I2CSendByte(unsigned char byt)
{
    unsigned char i;
	
    for(i=0; i<8; i++){
        scl = 0;
		I2C_Delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
		I2C_Delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
		I2C_Delay(DELAY_TIME);
    }
	
    scl = 0;  
}

//
unsigned char I2CReceiveByte(void)
{
	unsigned char da;
	unsigned char i;
	for(i=0;i<8;i++){   
		scl = 1;
		I2C_Delay(DELAY_TIME);
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		I2C_Delay(DELAY_TIME);
	}
	return da;    
}

//
unsigned char I2CWaitAck(void)
{
	unsigned char ackbit;
	
    scl = 1;
	I2C_Delay(DELAY_TIME);
    ackbit = sda; 
    scl = 0;
	I2C_Delay(DELAY_TIME);
	
	return ackbit;
}

//
void I2CSendAck(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit; 
	I2C_Delay(DELAY_TIME);
    scl = 1;
	I2C_Delay(DELAY_TIME);
    scl = 0; 
	sda = 1;
	I2C_Delay(DELAY_TIME);
}

//0-5V ->0-255
unsigned char Ad_Read(unsigned char addr){
	unsigned char temp;
	
	I2CStart();
	I2CSendByte(0x90);//1001 0000 对PCF8591发送写指令
	I2CWaitAck();//等待应答
	I2CSendByte(addr);//发送控制字 0000 00XX 
	//不模拟输出 单端输入 不自增（不读完自动跳到下一个通道） 
	//XX:00(外部输入) 01(光敏电阻) 10(差分输入) 11(电位器)
	I2CWaitAck();//等待应答
	
	I2CStart();//直接跳过第一阶段的写 
	//因为我AD读取只需要告诉PCF8591要读哪个通道就好了
	I2CSendByte(0x91);//1001 0001 对PCF8591发送读指令
	I2CWaitAck();//等待应答
	temp=I2CReceiveByte();//接收数据
	
	I2CSendAck(1);//读取结束 仅读取1字节数据
	I2CStop();//发送停止信号
	
	return temp;
}

//0-255 -> 0-5V
void Da_Write(unsigned char dat){
	I2CStart();
	I2CSendByte(0x90);//1001 0000 对PCF8591发送写指令
	I2CWaitAck();//等待应答
	I2CSendByte(0x41);//0100 00XX XX是什么并不影响 主要是使能DA 
	I2CWaitAck();//等待应答
	I2CSendByte(dat);//发送数据 写入数字电压
	I2CWaitAck();
	I2CStop();
}

//写入的数组，写入的起始位置，写入数据的个数（uchar的个数，uint个数相当于uchar*2）
void EEPROM_Write(unsigned char *str,unsigned char addr,unsigned char num){
	I2CStart();
	I2CSendByte(0xa0);//1010 0000 对AT24C02发送写指令
	I2CWaitAck();//等待应答
	I2CSendByte(addr);//写入起始地址
	I2CWaitAck();//等待应答
	
	while(num--){
		I2CSendByte(*str++);//写入一位数据，指针递增，为下一次写数据做准备
		I2CWaitAck();//等待应答
		I2C_Delay(200);//为保证完全写入
	}
	I2CStop();
	I2C_Delay(255);//10次
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
}

void EEPROM_Read(unsigned char *str,unsigned char addr,unsigned char num){
	I2CStart();
	I2CSendByte(0xa0);//1010 0000 对AT24C02发送写指令
	I2CWaitAck();//等待应答
	I2CSendByte(addr);//读取起始地址
	I2CWaitAck();//等待应答
	
	I2CStart();
	I2CSendByte(0xa1);//1010 0001 对AT24C02发送读指令
	I2CWaitAck();
	//EA=0; //如果在有中断的地方读取 一定要关中断
	while(num--){
		*str++=I2CReceiveByte();//依次读取到数组里面
		if(num) I2CSendAck(0);//num不为0 还有要读的 继续读取
		else I2CSendAck(1);//num为0 读完了
	}	
	//EA=1;
	I2CStop();
}
