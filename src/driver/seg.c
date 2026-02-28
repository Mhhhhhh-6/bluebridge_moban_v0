#include "seg.h"
unsigned char code seg_dula[]={                       //标准字库 要取反
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

void Seg_Disp(unsigned char wela,unsigned char dula){
	unsigned char temp;
	
	//消影
	P0=0xff;//选择全部数码管
	temp=P2&0x1f;//读取P2清空高三位 保留第四位的数据
	temp=temp|0xe0;//赋值Y7对应的高三位 111 是数码管的段选 决定是否亮
	P2=temp;//开门 P2接收temp的值 让P0数据流进去
	temp=P2&0x1f;//清空高三位
	P2=temp;//关门
	
	//位选
	P0=0x01<<wela;//选择某一位亮
	temp=P2&0x1f;//读取P2清空高三位 保留第四位的数据
	temp=temp|0xc0;//赋值Y6对应的高三位 110 是数码管的位选 决定哪一位
	P2=temp;//开门 P2接收temp的值 让P0数据流进去
	temp=P2&0x1f;//清空高三位
	P2=temp;//关门
	
	//段选
	P0=~seg_dula[dula];//数据 dula是码表对应的下表 比如dula=0 对应的是0的具体段值
	temp=P2&0x1f;//读取P2清空高三位 保留第四位的数据
	temp=temp|0xe0;//赋值Y7对应的高三位 111 是数码管的段选 决定是否亮
	P2=temp;//开门 P2接收temp的值 让P0数据流进去
	temp=P2&0x1f;//清空高三位
	P2=temp;//关门
}