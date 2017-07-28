#include "reg51.h"
#include "intrins.h"
#include "Display.h"

sbit 	LCD_BRESET	= P2^4;                 // from LCD busy flag
sbit    LCD_BF      = P2^7;     
sbit    LCD_REST    = P2^4; 

sbit 	LCD_EN		= P0^0;                 // enable the LCD
sbit 	LCD_RW		= P0^1;                 // LCD read or write
sbit 	LCD_DI		= P0^2;                 // LCD instruction or data

sbit 	LCD_CSA		= P1^6;                 // select the section A to Display
sbit 	LCD_CSB		= P1^7;                 // select the section B to Display


unsigned char	LCD_Reset(void);
void	        LCD_init(void);
unsigned char	LCD_commu(unsigned char comman,unsigned char comdata);
unsigned char	LCD_busy(void);


void	LCD_init(void)
{
    unsigned char i;
    i=5;
	while(i--);
      { if(LCD_Reset()==0)
         { i=0 ; }
      }
	LCD_CSA=1;
	LCD_CSB=0;
	LCD_commu(LCD_WI,0x3e);
	LCD_CSA=0;
	LCD_CSB=1;
	LCD_commu(LCD_WI,0x3e);
	Clear_LCDRegion(0,0,128,8);
	LCD_CSA=1;
	LCD_CSB=0;
	LCD_commu(LCD_WI,0xb8);
	LCD_commu(LCD_WI,0x40);
	LCD_commu(LCD_WI,0x3f);
	LCD_commu(LCD_WI,0xc0);
	LCD_CSA=0;
	LCD_CSB=1;
	LCD_commu(LCD_WI,0xb8);
	LCD_commu(LCD_WI,0x40);
	LCD_commu(LCD_WI,0x3f);
	LCD_commu(LCD_WI,0xc0);
	LCD_CSB=0;
}


void	Clear_LCDRegion(unsigned char Left_CorX,Left_CorYp,Right_CorX,nPage)
{
	unsigned char	nITEMP;
	if(Right_CorX>127)	Right_CorX=127;
	if(Left_CorX>63)
	{
		LCD_CSA=0;
		LCD_CSB=1;
		nITEMP=Right_CorX-Left_CorX+1;
		Left_CorX=Left_CorX-64;
		Clear_LCDSChip(Left_CorX,Left_CorYp,nITEMP,nPage);
	}
	else if(Right_CorX<64)
	{
		LCD_CSA=1;
		LCD_CSB=0;
		nITEMP=Right_CorX-Left_CorX+1;
		Clear_LCDSChip(Left_CorX,Left_CorYp,nITEMP,nPage);
	}
	else
	{
		LCD_CSA=1;
		LCD_CSB=0;
		Clear_LCDSChip(Left_CorX,Left_CorYp,64-Left_CorX,nPage);

		LCD_CSA=0;
		LCD_CSB=1;
		Clear_LCDSChip(0,Left_CorYp,Right_CorX-63,nPage);
	}
	LCD_CSA=0;
	LCD_CSB=0;
}

//------------------------------------------------------------------------------//
// Clear the LCD a region in the same drive chip.								//
//------------------------------------------------------------------------------//
void	Clear_LCDSChip(unsigned char LeftCorX,LeftCorYP,nDataCol,nDataPage)
{
	unsigned char	i,j;
	for(j=0;j<nDataPage;j++)
	{
		i=LeftCorYP+0xb8;
		LCD_commu(LCD_WI,i);
		i=LeftCorX+0x40;
		LCD_commu(LCD_WI,i);
		for(i=0;i<nDataCol;i++)
		{
			LCD_commu(LCD_WD,0x00);
		}
		LeftCorYP++;
	}
}


unsigned char	LCD_Reset(void)
{
	unsigned char	LcdStatus,i,j;
	for(j=0;j<3;j++)
	{
		LcdStatus=0;
		LCD_CSA = 1;
		LCD_CSB = 0;

		i=LCD_busy();
		if(i>LCDBUSY)	{LcdStatus=2;}
		LCD_CSA=0;
		LCD_CSB = 1;
		i=LCD_busy();
		if(i>LCDBUSY)	{LcdStatus=LcdStatus+4;}

		if(LcdStatus==0)	{LCD_CSB=0;	break;}	
	}
	return(LcdStatus);
}



unsigned char	LCD_commu(unsigned char comman,unsigned char comdata)
{
	unsigned char k;
	switch(comman)
	{
		case 0x00:										//write instruction to the LCD
		{	comman=LCD_busy();
			LCD_DI	= 0;                  
			LCD_RW	= 0;
			P2		= comdata;
			k=LCD_TAS;
			while(k--);	

			LCD_EN	= 1;
			k=LCD_TDDW;
			while(k--);	
			k=LCD_TDSW;
			while(k--);	

			LCD_EN	= 0;
			comdata	= comman;
			break;
		}

		case 0x01:	{	comdata=LCD_busy();	break;}		//the LCD status and instruction

		case 0x02:										//write the data to the LCD
		{	comman=LCD_busy();
			LCD_DI	= 1;                  
			LCD_RW	= 0;
			P2		= comdata;
			k=LCD_TAS;
			while(k--);	

			LCD_EN	= 1;
			k=LCD_TDDW;
			while(k--);	

			k=LCD_TDSW;
			while(k--);	

			LCD_EN	= 0;
			comdata	= comman+1;
			break;
		}

		case 0x03:										//read the data from the LCD
		{	comman=LCD_busy();
			LCD_DI	= 1;                  
			LCD_RW	= 1;
			k=LCD_TAS;
			while(k--);	

			LCD_EN	= 1;
			k=LCD_TDSW;
			while(k--);	
			comdata = P2;
			LCD_EN	= 0;
			break;
		}
		default:{comdata=0xee;break;}
	}
	LCD_DI	= 0;                  
	LCD_RW	= 1;
	return(comdata);									//return data\AC\status
}

//------------------------------------------------------------------------------//
// Read the LCD status,checking if LCD is busy and waiting to it is not busy 	//
//------------------------------------------------------------------------------//
unsigned char	LCD_busy(void)
{
	unsigned char i,j;
	LCD_EN	= 0;
	LCD_DI	= 0;                  
	LCD_RW	= 1;
	for(j=0;j<250;j++)
	{
		P2=P2 | 0x90;
		_nop_();
		LCD_EN	= 1;
		i=LCD_TDDW;
		while(i--);	

		if(!(LCD_BF || LCD_BRESET))	{j=P2;	break;}	
		LCD_EN	= 0;
		P2=0x00;
		i=LCD_TDDW;
		while(i--);	
	}
	LCD_EN = 0;
	return(j);
}





