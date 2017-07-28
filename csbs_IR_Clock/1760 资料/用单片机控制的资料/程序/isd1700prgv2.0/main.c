
/***************************************************************************/
/***************************************************************************/
/**************ISD1700ϵ�п��������س��� Ver2.0*****************************/
//  ��������:
//	��Ƭ��������оƬ֮�����SPIͨ��;
//  ¼��������λ��ͨ������ͨ��;
//	��λ����¼��������ܶ�����оƬ������ز���;
//	����ʶ��оƬID���ܽ���¼�������š���������ȡ�����ε�ַ��;
//	����ʹ����ο���ISD17xxϵ��¼����ʹ���ֲ�Ver2.1��
/***************************************************************************/
//  Ӳ��˵����
//  MCU��AT89C51/STCϵ��
//  ����11.0592MHz
//	���ڲ����ʣ�1200bps
/***************************************************************************/
//  �汾��Ver2.0
//  ���ߣ�Cowboy
//  ���ڣ�2011-3
//	QQ:	  239447308	
//  http://our12580.taobao.com									
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

/*ͷ�ļ�*/
#include "reg51.h"
#include "sound.h"
#include "ISD1700.H"

#define  uchar unsigned char 
#define  uint  unsigned int

/*********��������**************/
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

/*********SPIͨ�Žӿ�**********/
sbit ISD_SS=P0^7;
sbit ISD_MISO=P0^4;
sbit ISD_MOSI=P0^5;
sbit ISD_SCLK=P0^6;

sbit RUN_LED = P1^0;  //����ָʾ��

uchar RunLedTime;
bit playflg;

void main(void)
{
	init();				//������ʼ��
	while(1)
	{
		comm_sate();  	//����λ��ͨ��
		if(playflg)		//���ű�־λ
		{
			playflg = 0;  //��0
			spi_fwd();	  //����ָ��ָ����һ��
			delay_isd(30000);  //��ʱ
			spi_play();		   //���ŵ�ǰ
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

/************�ⲿ�ж�0������**************/
void int0(void) interrupt 0
{	
	playflg = 1;  //���ű�־λ��1��������
}

void tim0_sever(void) interrupt 1
{
	TL0=0x00;
    TH0=0x70;	  				//���¸���ֵ
	if(RunLedTime != 0)
	{
		if( --RunLedTime == 0) 
        {
			RUN_LED=~RUN_LED;	 //1����ȡ��һ��
            RunLedTime = 25;
		}
	}
}


void delay_isd(uint time)
{
	while(time--!=0);
}

/**************��ȡָ�������ε�ַ�����ţ��û��ɸ���ʵ����Ҫ��������******************/
/**************��Ӧ�������ε�ַ��SOUND.H�ļ�������ַ��¼������ж�ȡ*************/

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

/**********����ָ��������************/
void PlaySoundTick(uchar  number)
{
	  spi_stop ();
	  delay_isd(30000);
      GetSound(number);
}
