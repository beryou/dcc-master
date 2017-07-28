
/***************************************************************************/
/***************************************************************************/
/**************ISD1700系列开发板主控程序 Ver2.0*****************************/
//  功能描述:
//	单片机与语音芯片之间采用SPI通信;
//  录音板与上位机通过串口通信;
//	上位机（录音软件）能对语音芯片进行相关操作;
//	包括识别芯片ID、能进行录音、播放、擦除、读取语音段地址等;
//	具体使用请参考《ISD17xx系列录音板使用手册Ver2.1》
/***************************************************************************/
//  硬件说明：
//  MCU：AT89C51/STC系列
//  晶振：11.0592MHz
//	串口波特率：1200bps
/***************************************************************************/
//  版本：Ver2.0
//  作者：Cowboy
//  日期：2011-3
//	QQ:	  239447308	
//  http://our12580.taobao.com									
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

/*头文件*/
#include "reg51.h"
#include "sound.h"
#include "ISD1700.H"

#define  uchar unsigned char 
#define  uint  unsigned int

/*********函数声明**************/
extern void  spi_pu (void);
extern void  comm_sate(void);
extern void  spi_stop (void);
extern void  spi_fwd (void);
extern void  spi_play(void);
extern void  isd1700_7byte_comm(uchar comm_par, uint star_addr, uint end_addr);
void  init(void);
void  PlaySoundTick(uchar  number);
void  LEDShow(void);
void  delay_isd(uint time);

/*********SPI通信接口**********/
sbit ISD_SS=P0^7;
sbit ISD_MISO=P0^4;
sbit ISD_MOSI=P0^5;
sbit ISD_SCLK=P0^6;

sbit RUN_LED = P1^0;  //运行指示灯

uchar RunLedTime;
bit playflg;

void main(void)
{
	init();				//器件初始化
	while(1)
	{
		comm_sate();  	//与上位机通信
		if(playflg)		//播放标志位
		{
			playflg = 0;  //清0
			spi_fwd();	  //播放指针指向下一曲
			delay_isd(30000);  //延时
			spi_play();		   //播放当前
		}
	}
}

void  init(void)
{	    
    TMOD=0x21;
    SCON=0x50;
	TL0=0x00;	//25ms
    TH0=0x70;   //25ms           
	TH1=0xE8;
    TL1=0xE8;   //12M/1200bit/s/E6  11.0592M/E8
	ET0=1;
	EA=1;
 	TR0=1; 
	TR1=1;
	IT0 = 0;
	EX0 = 1;
	spi_pu();
	P3 = 0xff;	
	RunLedTime = 25;
}

/************外部中断0服务函数**************/
void int0(void) interrupt 0
{	
	playflg = 1;  //播放标志位置1，允许播放
}

void tim0_sever(void) interrupt 1
{
	TL0=0x00;
    TH0=0x70;	  				//重新赋初值
	if(RunLedTime != 0)
	{
		if( --RunLedTime == 0) 
        {
			RUN_LED=~RUN_LED;	 //1秒钟取反一次
            RunLedTime = 25;
		}
	}
}


void delay_isd(uint time)
{
	while(time--!=0);
}

/**************获取指定语音段地址并播放，用户可根据实际需要进行增减******************/
/**************对应的语音段地址在SOUND.H文件里，具体地址从录音软件中读取*************/

void GetSound(uchar soundtick)
{
	ISD_SS=0;
	switch(soundtick)
	{  
		case 0:{ isd1700_7byte_comm(ISD1700_SET_PLAY|ISD_LED, sound_0A, sound_0B); }break;
		case 1:{ isd1700_7byte_comm(ISD1700_SET_PLAY|ISD_LED, sound_1A, sound_1B); }break;
	    case 2:{ isd1700_7byte_comm(ISD1700_SET_PLAY|ISD_LED, sound_2A, sound_2B); }break;
	    case 3:{ isd1700_7byte_comm(ISD1700_SET_PLAY|ISD_LED, sound_3A, sound_3B); }break;
	    case 4:{ isd1700_7byte_comm(ISD1700_SET_PLAY|ISD_LED, sound_4A, sound_4B); }break;
	    case 5:{ isd1700_7byte_comm(ISD1700_SET_PLAY|ISD_LED, sound_5A, sound_5B); }break;
	    case 6:{ isd1700_7byte_comm(ISD1700_SET_PLAY|ISD_LED, sound_6A, sound_6B); }break;
	    case 7:{ isd1700_7byte_comm(ISD1700_SET_PLAY|ISD_LED, sound_7A, sound_7B); }break;
	    case 8:{ isd1700_7byte_comm(ISD1700_SET_PLAY|ISD_LED, sound_8A, sound_8B); }break;
	    case 9:{ isd1700_7byte_comm(ISD1700_SET_PLAY|ISD_LED, sound_9A, sound_9B); }break;
	    case 10:{ isd1700_7byte_comm(ISD1700_SET_PLAY|ISD_LED, sound_10A, sound_10B); }break;
	    case 11:{ isd1700_7byte_comm(ISD1700_SET_PLAY|ISD_LED, sound_11A, sound_11B); }break;
	    default: break;
     }
	ISD_SS=1;
}

/**********播放指定语音段************/
void PlaySoundTick(uchar  number)
{
	  spi_stop ();
	  delay_isd(30000);
      GetSound(number);
}
