#define  uchar unsigned char 
#define  uint  unsigned int


#include "reg51.h"
#include "sound.h"
#include "ISD1700.H"
#include "LCD12864.h"



void  init(void);
void  PlaySoundTick(uchar  number);
void  LEDShow(void);
void  delay_isd(uint time);
void  DisplayInfo(void);
void  DisRunTime(void);

sbit ISD_SS=P0^7;
sbit ISD_MISO=P0^4;
sbit ISD_MOSI=P0^5;
sbit ISD_SCLK=P0^6;

sbit RUN_LED = P1^0;
sbit LCD_BL = P0^3;

sbit K1 = P3^3;
sbit K2 = P3^4;
sbit K3 = P3^5;
sbit K4 = P3^2;


uchar RunLedTime;
uchar WaitTime;
uchar Second,Minute;
uchar DisMenu;
bit playflg;
bit LCD_UP;


void main(void)
{
	init();
	LCD_init();
	lcd_xt_start();
	TR0=1;
	Clear_LCDRegion(0,0,128,8);
	DisplayInfo();
	while(1)
	{
		comm_sate();
		if(!K4)
		{
			while(!K4);
			spi_fwd();
			delay_isd(30000);
			spi_play();
		}
		if(!K1)	 //按下K1可切换液晶背光灯
		{
			while(!K1);
			LCD_BL = ~LCD_BL;
		}

		if(LCD_UP)
		{
			LCD_UP = 0;
			Clear_LCDRegion(0,6,128,2);
		}
		
		switch(DisMenu)
		{
			case 0: DisRunTime(); break;
			case 1:	PrintfChar(48,0,"K1:");
					PrintfHz(48,24,"切换背光");break;
			case 3:	PrintfChar(48,0,"Enjoy o(_)o "); break;
			case 2:	PrintfChar(48,0,"QQ:239447308"); break;
			default:DisRunTime(); break;
		}
		
	}
}

void DisplayInfo(void)
{
//	uchar ISDDevID;
	PrintfHz(0,0,"语音芯片开发系统");
	PrintfHz(16,0,"固件版本");
	PrintfChar(16,64,":Ver2.01");
	PrintfHz(32,0,"芯片型号");
	spi_devid();
//	ISDDevID = ISD_COMM_RAM_C[2];
	switch(ISD_COMM_RAM_C[2])
	{
		case 0x90: PrintfChar(32,64,":ISD1720"); break;
		case 0x80: PrintfChar(32,64,":ISD1730"); break;
		case 0xB0: PrintfChar(32,64,":ISD1740"); break;
		case 0xA8: PrintfChar(32,64,":ISD1750"); break;
		case 0xA0: PrintfChar(32,64,":ISD1760"); break;
		case 0xD0: PrintfChar(32,64,":ISD1790"); break;
		case 0xC8: PrintfChar(32,64,"ISD17120"); break;
		case 0xC0: PrintfChar(32,64,"ISD17150"); break;
		case 0xF0: PrintfChar(32,64,"ISD17180"); break;
		case 0xE8: PrintfChar(32,64,"ISD17210"); break;
		case 0xE0: PrintfChar(32,64,"ISD17240"); break;
		default:   PrintfChar(32,64,":"); 
				   PrintfHz(32,72,"未知");
	}
	PrintfHz(48,0,"运行时间");
	PrintfChar(48,64,":");
	PrintfChar(48,88,":");

}

void DisRunTime(void)
{
	PrintfHz(48,0,"运行时间");
	PrintfChar(48,64,":");
	PrintfChar(48,88,":");
	PrintfData(48,72,Minute/10);
	PrintfData(48,80,Minute%10);
	PrintfData(48,96,Second/10);
	PrintfData(48,104,Second%10);
}

void  init(void)
{	    
    TMOD=0x21;
    SCON=0x50;
	TL0=0x00;	//25ms
    TH0=0x70;   //25ms           
	TH1=0xE8;
    TL1=0xE8;   //波特率：1200bps（12MHz：0xE6 11.0592MHz：0xE8）
	ET0=1;
	EA=1;
 	
	TR1=1;
	IT0 = 0;
	EX0 = 0;
	spi_pu();
	P3 = 0xff;	
	RunLedTime = 25;
	WaitTime = 125;
}


void tim0_sever(void) interrupt 1
{
	TL0=0x00;
    TH0=0x70;	
	if(RunLedTime != 0)
	{
		if( --RunLedTime == 0) 
        {
			RUN_LED=~RUN_LED;
			Second++;
			if(Second>59)
			{
				Second = 0;
				Minute++;
				if(Minute>59) Minute = 0;
			}
            RunLedTime = 25;
		}
	}
	if(WaitTime  != 0)
	{
		if( --WaitTime  == 0) 
        {
			LCD_UP = 1;
			WaitTime = 125;
			DisMenu++;
			if(DisMenu>3) DisMenu = 0;
		}
	}
}


void delay_isd(uint time)
{
	while(time--!=0);
}

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

void PlaySoundTick(uchar  number)
{
	  spi_stop ();
	  delay_isd(30000);
      GetSound(number);
}
