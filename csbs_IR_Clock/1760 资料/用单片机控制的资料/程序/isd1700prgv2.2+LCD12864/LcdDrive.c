#define uint unsigned int
#define uchar unsigned char


#include "reg51.h"
#include "intrins.h"
#include "lcd.c"
#include "display.h"

#define DisplayOff       0x3E
#define DisplayOn        0x3F
#define SetDiSartL       0xC0
#define SetPageAddr      0xB8
#define SetColumnAddr    0x40

#define LcdDPort P2 
#define Ldata    1
#define Lcomm    0
#define BackOn   0
#define BackOFF  1
#define ConlumnEndAddr   128
#define LcdRowNum        64
#define LcdPageNub       8-1

#define Chs              1
#define Chr              0
#define L_Lcd            1
#define R_Lcd            0
#define nLcd1            0
#define nLcd2            1
#define Lcd_En           1
#define Lcd_nEn          0

#define RecDis		 0
#define ClrAnd		 1
#define ClrAdd		 2


#define  nLcdBAK_MuxA    2
#define  nLcdA0_MuxA     1
#define  nLcd_RW         0


#define  COMM                0
#define  nLcd_RW_W           0

#define f_line  0x40   //row addr

sbit 	LCD_BRESET	= P2^4;                 // from LCD busy flag

sbit    LCD_BF      = P2^7;     
sbit    LCD_REST    = P2^4; 

sbit 	LCD_EN		= P0^0;                 // enable the LCD
sbit 	LCD_RW		= P0^1;                 // LCD read or write
sbit 	LCD_DI		= P0^2;                 // LCD instruction or data

sbit 	LCD_CSA		= P1^6;                 // select the section A to Display
sbit 	LCD_CSB		= P1^7;                 // select the section B to Display



unsigned char	LCD_busy1(void)
{
	unsigned char i,j;
	LCD_DI	= 0;                  
	LCD_RW	= 1;
	for(j=250;j>=1;j--)
	{
		LcdDPort=LcdDPort | 0x90;
		_nop_();
        _nop_();
		LCD_EN	= 1;
		i=10;
		while(i--);		    
        LCD_EN	= 0;
        _nop_();
        _nop_();
		if(!(LCD_BF || LCD_REST))
			{ break;}	
	   break;
	}
	return(j);
}




  /*------------------------------------------*/
void wrLcdOper(uchar idata choe1,uchar idata choe2,uchar idata data_comm,uchar idata content)
{  
    uchar i;
    LCD_busy1();
    if(data_comm!=0)
     {  LCD_DI	= 1;                  
	    LCD_RW	= 0; 
      }
    else
     { 
      LCD_DI	= 0;                  
	  LCD_RW	= 0;
     } 
  LCD_EN	= 1;
  LcdDPort=content;    //output data or comm  
  if(choe1==1)
   { LCD_CSA=Lcd_En; }
  if(choe2==1)
   { LCD_CSB=Lcd_En; }
  i=20; 
  while( i--);
  LCD_EN	= 0;
  _nop_();
  _nop_();
  if(choe1==1)
   { LCD_CSA=Lcd_nEn; }
  if(choe2==1)
   { LCD_CSB=Lcd_nEn; }
}




void WritAddr(bit LR_Lcd,uchar idata  CommData)
        {
           if(LR_Lcd)
             { wrLcdOper (1,0,Lcomm,CommData); }
		   else
		     { wrLcdOper (0,1,Lcomm,CommData); }
		}

//========================= x saddr =================== x eaddr =================== y  saddr ============= y eaddr ==================
void DrawRectangle ( uchar idata RowAddr, uchar idata EndRowAddr,uchar idata StConlumnAddr,uchar idata EndConlumnAddr,uchar *DataPtr, uchar idata Pdata,uchar idata OpType)
        {
          uchar idata StartPageAddr;
		  uchar idata StartColumnAddr;
		  uchar idata WColumnTemp;
          uchar idata ColumnTemp;
		  uchar idata MovN;
		  uchar idata LcdDataTemp;
		  uchar idata PageNum;
		  uchar code *Ptr;
		  uchar idata Dtemp;
		  uchar idata j;
		  bit   LR_Lcd;
          bit   LR_Bit;

          Ptr=DataPtr;
          StartPageAddr=RowAddr/8|SetPageAddr;
		  MovN=RowAddr%8; 
		  PageNum=EndRowAddr/8-RowAddr/8;

 		  if(StConlumnAddr>=LcdRowNum)
		    {  StartColumnAddr=StConlumnAddr-LcdRowNum;
			   LR_Lcd=R_Lcd;
		    }
		  else
		    {  StartColumnAddr=StConlumnAddr;
			   LR_Lcd=L_Lcd;
		    }
          StartColumnAddr+=SetColumnAddr;

          if(StartPageAddr<=SetPageAddr+LcdPageNub)
           { 
              LR_Bit=LR_Lcd;
		      WritAddr(LR_Bit,StartPageAddr);
			  WColumnTemp=StartColumnAddr;
              ColumnTemp=StartColumnAddr-SetColumnAddr;

		      for(j=StConlumnAddr;j<=EndConlumnAddr;j++)
			   { 
			     			 
				 if(LR_Bit==L_Lcd)
                   { if(ColumnTemp++==LcdRowNum)
                       {
					     LR_Bit=~LR_Bit;
						 WColumnTemp=SetColumnAddr;
                         WritAddr(LR_Bit,StartPageAddr);				     
                       }
				   }

				 //WritAddr(LR_Bit,WColumnTemp);
				 //LcdDataTemp=RedLcdData(LR_Bit);         //读出显示RAM数据	+1	
				 LcdDataTemp=0;		 				 
				 switch( OpType )                                    //判断显示类型				              
			       { 
				      case RecDis: { LcdDataTemp |=*DataPtr++<<=MovN ; }break;				 
				                   
				      case ClrAnd: {  LcdDataTemp&=Pdata; }break;
					              
				      case ClrAdd: {  LcdDataTemp|=Pdata; }break; 
				    }                 
			
			     WritAddr(LR_Bit,WColumnTemp++);               //-1
			     if(LR_Bit)
                  { wrLcdOper (1,0,Ldata,LcdDataTemp); }
			     else
				  { wrLcdOper (0,1,Ldata,LcdDataTemp); }
		       }
			}

		  if(PageNum==0){ return; }
		  
		  while(PageNum--)
			 {   
			    StartPageAddr++;				
		        if(StartPageAddr>SetPageAddr+LcdPageNub){ return; }
				  
			    LR_Bit=LR_Lcd;
		        WritAddr(LR_Bit,StartPageAddr);
				WColumnTemp=StartColumnAddr;
                ColumnTemp=StartColumnAddr-SetColumnAddr;
		  
                for(j=StConlumnAddr;j<=EndConlumnAddr;j++)
			     { 			       
				   if(LR_Bit==L_Lcd)
                    { if(ColumnTemp++==LcdRowNum)
                       {
					     LR_Bit=~LR_Bit;
						 WColumnTemp=SetColumnAddr;
                         WritAddr(LR_Bit,StartPageAddr);				     
                       }
				    }													 

                   //WritAddr(LR_Bit,WColumnTemp);
				   //LcdDataTemp=RedLcdData(LR_Bit);         //读出显示RAM数据	+1	
				   LcdDataTemp=0;		 				 
				   switch( OpType )                                   //判断显示类型				              
			        { 
				      case RecDis: { 
                                     Dtemp=*Ptr++>>(8-MovN);
									 if((PageNum>1)||(MovN==0)) 				     //在最后一页,第一数据指针不用再取数据.              
				                        { LcdDataTemp|=*DataPtr++<<MovN;}
									 LcdDataTemp|=Dtemp;								 			 
				                   }break;

				      case ClrAnd: {  LcdDataTemp&=Pdata; }break;
					              
				      case ClrAdd: {  LcdDataTemp|=Pdata; }break; 
				    }                 
			
			       WritAddr(LR_Bit,WColumnTemp++);               //-1
			       if(LR_Bit)
                     { wrLcdOper (1,0,Ldata,LcdDataTemp); }
			       else
				     { wrLcdOper (0,1,Ldata,LcdDataTemp); }
			    }		
			}
		  
        }



//=====================================================================================================
void LcdDisplayChar( uchar idata RowAddr, uchar idata ConlumnAddr,uchar *DataPtr,bit Chs_Char)
        { uchar idata DipTemp[16];
          uchar idata StartPageAddr;
		  uchar idata StartColumnAddr;
          uchar idata ColumnTemp;
		  uchar idata MovN;
		  uchar idata DipNum;
		  uchar idata LcdDataTemp;
		  uchar idata j;
          bit   LastWrFig;
		  bit   LR_Lcd;
          bit   LR_Bit;

//		  bit AddDrwBit;

		  LcdDataTemp=0; 
          StartPageAddr=RowAddr/8|SetPageAddr;
		  MovN=RowAddr%8; 
 		  if(ConlumnAddr>=LcdRowNum)
		    {  StartColumnAddr=ConlumnAddr-LcdRowNum;
			   LR_Lcd=R_Lcd;
		    }
		  else
		    {  StartColumnAddr=ConlumnAddr;
			   LR_Lcd=L_Lcd;
		    }
          StartColumnAddr+=SetColumnAddr;
          if(RowAddr%16!=0)
            { LastWrFig=1; }
          else
            { LastWrFig=0; } 

          if(Chs_Char==Chs)
		        { DipNum=16;}
		      else
		        { DipNum=8; } 

          if(StartPageAddr<=SetPageAddr+LcdPageNub)
           { 
              LR_Bit=LR_Lcd;
		      WritAddr(LR_Bit,StartPageAddr);
		      WritAddr(LR_Bit,StartColumnAddr);
              ColumnTemp=StartColumnAddr-SetColumnAddr;
             
		      for(j=0;j<DipNum;j++)
			   { 
			     DipTemp[j]=*DataPtr>>(8-MovN);
				 if(LR_Bit==L_Lcd)
                   { if(ColumnTemp++==LcdRowNum)
                       {
					     LR_Bit=~LR_Bit;
                         WritAddr(LR_Bit,StartPageAddr);
		                 WritAddr(LR_Bit,SetColumnAddr);					     
                       }
				   }
		//		 LcdDataTemp=BackLcdData(AddDrwBit);
		/*		 if(AddDrwBit)
				   { WritAddr(LR_Bit,ColumnTemp-1); }*/
			     if(LR_Bit)
                  { wrLcdOper (1,0,Ldata,*DataPtr++<<MovN|LcdDataTemp); }
			     else
				  { wrLcdOper (0,1,Ldata,*DataPtr++<<MovN|LcdDataTemp); }
		       }
			}

		  StartPageAddr++;
		  if(StartPageAddr<=SetPageAddr+LcdPageNub)
           { 
		      LR_Bit=LR_Lcd;
		      WritAddr(LR_Bit,StartPageAddr);
		      WritAddr(LR_Bit,StartColumnAddr);
              ColumnTemp=StartColumnAddr-SetColumnAddr;
		  
              for(j=0;j<DipNum;j++)
			    { 
			      DipTemp[j]|=*DataPtr++<<MovN;
				  if(LR_Bit==L_Lcd)
                   { if(ColumnTemp++==LcdRowNum)
                       {
					     LR_Bit=~LR_Bit;
                         WritAddr(LR_Bit,StartPageAddr);
		                 WritAddr(LR_Bit,SetColumnAddr);
                       }
				   }
               //   LcdDataTemp=BackLcdData(AddDrwBit);
				  if(LR_Bit)
                   { wrLcdOper (1,0,Ldata,DipTemp[j]|LcdDataTemp); }
				  else
				   { wrLcdOper (0,1,Ldata,DipTemp[j]|LcdDataTemp); }
			    }			
			}

		   StartPageAddr++;
		   if(StartPageAddr<=SetPageAddr+LcdPageNub)
           { 
             if(LastWrFig)
		       {    
			        LR_Bit=LR_Lcd;
			        WritAddr(LR_Bit,StartPageAddr);
		            WritAddr(LR_Bit,StartColumnAddr);
				    DataPtr=DataPtr-DipNum;
					ColumnTemp=StartColumnAddr-SetColumnAddr;               

				    for(j=0;j<DipNum;j++) 
 			          { 
					    if(LR_Bit==L_Lcd)
                          { if(ColumnTemp++==LcdRowNum)
                              {
							    LR_Bit=~LR_Bit;
                                WritAddr(LR_Bit,StartPageAddr);
		                        WritAddr(LR_Bit,SetColumnAddr);
                              }
				          }
					//	LcdDataTemp=BackLcdData(AddDrwBit);
				        if(LR_Bit)
                          { wrLcdOper (1,0,Ldata,*DataPtr++>>(8-MovN)|LcdDataTemp); }
				        else
				          { wrLcdOper (0,1,Ldata,*DataPtr++>>(8-MovN)|LcdDataTemp); }
				      }
			    }
			}
		 }

uchar FindCharIndx(char *pchar)
       { uchar index=0;
	     uchar temp0;

         while(*(&ascii+index)!='\0')
           { 
		     temp0=*(&ascii+index);   
             if(*pchar==*(&ascii+index))    
               {
                  return index;
               }
             index++;
          }   
         return 255;                          //没有找到序号，代表没有这个字   
      }


uchar FindIndexHz(uchar  *pchar)         //寻找一个汉字的标号。
 {
   uchar idata index=0;
   while(*(&hzku+index*2)!='\0')
    { 	   
     if(*(pchar)==*(hzku+index*2)&&*(pchar+1)==*(hzku+index*2+1))    
         {
           return (index);
         }
      index++;
	if(index>=250) { break;}
    }   
     return (0xff);                          //没有找到序号，代表没有这个字    
  }



void PrintfHz(uchar idata RowAddr, uchar idata ConlumnAddr,uchar  *pchar)
       {  uchar PtrAddr;
	      while(*pchar!='\0')
           { 
 	         PtrAddr=FindIndexHz(pchar);
		     if(PtrAddr!=0xff)
		       { LcdDisplayChar(RowAddr, ConlumnAddr,&HzTabl+PtrAddr*32,Chs); } 
			 else
			   { return; } 
			 pchar+=2;
			 ConlumnAddr+=16; 
		   }       	      
       }


void PrintfChar(uchar idata RowAddr, uchar idata ConlumnAddr,uchar *pchar)
       {  uchar PtrAddr;
          while(*pchar!='\0')
           { 
 	         PtrAddr=FindCharIndx(pchar);
		     if(PtrAddr!=255)
		       {  LcdDisplayChar(RowAddr, ConlumnAddr,AsciiTabl+PtrAddr*16,Chr); }
             //else
               //{  return; } 
             pchar++;
			 ConlumnAddr+=8;
		   }	      
       }

void PrintfData(uchar idata RowAddr, uchar idata ConlumnAddr,uchar idata DDtat)
       {
         switch(DDtat)
		    {  
              case 0:{ PrintfChar(RowAddr, ConlumnAddr,"0"); }break;
			  case 1:{ PrintfChar(RowAddr, ConlumnAddr,"1"); }break;
			  case 2:{ PrintfChar(RowAddr, ConlumnAddr,"2"); }break;
			  case 3:{ PrintfChar(RowAddr, ConlumnAddr,"3"); }break;
			  case 4:{ PrintfChar(RowAddr, ConlumnAddr,"4"); }break;
			  case 5:{ PrintfChar(RowAddr, ConlumnAddr,"5"); }break;
			  case 6:{ PrintfChar(RowAddr, ConlumnAddr,"6"); }break;
			  case 7:{ PrintfChar(RowAddr, ConlumnAddr,"7"); }break;
			  case 8:{ PrintfChar(RowAddr, ConlumnAddr,"8"); }break;
			  case 9:{ PrintfChar(RowAddr, ConlumnAddr,"9"); }break;
			  case 10:{ PrintfChar(RowAddr, ConlumnAddr,"A"); }break;
			  case 11:{ PrintfChar(RowAddr, ConlumnAddr,"B"); }break;
			  case 12:{ PrintfChar(RowAddr, ConlumnAddr,"C"); }break;
			  case 13:{ PrintfChar(RowAddr, ConlumnAddr,"D"); }break;
			  case 14:{ PrintfChar(RowAddr, ConlumnAddr,"E"); }break;
			  case 15:{ PrintfChar(RowAddr, ConlumnAddr,"F"); }break;
			  default:{ PrintfChar(RowAddr, ConlumnAddr,"Z"); }break;
			}
	   }


void wrlattice (uchar idata data1,uchar idata data2)
{
  uchar idata i,j;
  wrLcdOper (1,1,Lcomm,SetDiSartL); 
  for(j=0;j<8;j++)
  {
    wrLcdOper (1,1,Lcomm,SetPageAddr+j);
	wrLcdOper (1,1,Lcomm,SetColumnAddr);     
    for(i=0;i<LcdRowNum;i=i+2)
    {
      wrLcdOper (1,1,Ldata,data1);
      wrLcdOper (1,1,Ldata,data2);
    }
  }
}


void LCD_delay (unsigned int us)   //delay time
{
   while(us--);
}


void lcd_xt_start(void)
{
    uchar i;
    wrlattice (0,0);
	DrawRectangle (0,31,31,96, bimp,0, 0);
	PrintfHz(36 ,24 ,  "系统启动中");
	DrawRectangle (56,63,8,8,0,0xff, 2);

	DrawRectangle (56,63,9,118,0,0x81, 2);

	DrawRectangle (56,63,119,119,0,0xff,2);
	for(i=9;i<=117;i+=4)
	{  DrawRectangle (56,63,i,i+3,0,0xff, 2);
	   LCD_delay(10000);
	}
}