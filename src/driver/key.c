#include "key.h"

unsigned char Key_Read(){
	unsigned char temp=0;
	
	//独立按键
	/*
	if(P30==0) temp=7;
	if(P31==0) temp=6;
	if(P32==0) temp=5;
	if(P33==0) temp=4;
	*/
	
	//矩阵按键 
	//如果需要测量频率 那么必须注释P34相关的所有东西 前三段的P34=1要注释 最后一大段全注释
	//处理串口 需要在每段前加两句代码
	P44=1;P42=1;P35=1;//P34=1;
	if(P30==0) return 0;
	P44=0;P42=1;P35=1;//P34=1;
	if(P33==0) temp=4;
	if(P32==0) temp=5;
	if(P31==0) temp=6;
	if(P30==0) temp=7;
	
	P44=1;P42=1;P35=1;//P34=1;
	if(P30==0) return 0;
	P44=1;P42=0;P35=1;//P34=1;
	if((P33==0)&&(P32==0)) temp=89;
	else if(P33==0) temp=8;
	else if(P32==0) temp=9;
	if(P31==0) temp=10;
	if(P30==0) temp=11;
	
	P44=1;P42=1;P35=1;//P34=1;
	if(P30==0) return 0;
	P44=1;P42=1;P35=0;//P34=1;
	if(P33==0) temp=12;
	if(P32==0) temp=13;
	if(P31==0) temp=14;
	if(P30==0) temp=15;
	
	/*P44=1;P42=1;P35=1;P34=1;
	if(P30==0) return 0;
	P44=1;P42=1;P35=1;P34=0;
	if(P33==0) temp=16;
	if(P32==0) temp=17;
	if(P31==0) temp=18;
	if(P30==0) temp=19;*/

	P44=1;P42=1;P35=1;//P34=1;
	P3=P3|0xef;//1110 1111 不影响P34 也就是测量频率位
	return temp;
}

