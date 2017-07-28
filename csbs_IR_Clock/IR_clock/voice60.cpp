#include "voice60.h"

/****************************************************************************************************/
byte SPI_transfer(byte data) 
{
  SPDR = data;
  while (!(SPSR & 0x80));
  return SPDR;
}

void ISD1760_SPI_INIT()
{
  pinMode(CS_PIN,OUTPUT);
  pinMode(DO_PIN,OUTPUT);
  pinMode(CK_PIN,OUTPUT);
  
  digitalWrite(CS_PIN,HIGH);
  digitalWrite(DO_PIN,LOW);
  digitalWrite(CK_PIN,HIGH);

  SPCR = 0x7f;
  SPSR = 0x00;
/* 
   7 - 禁止SPI中断 6 - SPI使能 5 - 低位 4 - 主机 
   3 - 时钟空闲时高电平 2 - 结束沿采样 1,0 - 128分频(SPI2X = 0)
*/
}

void ISD_PU()
{
  digitalWrite(CS_PIN,LOW);
  SPI_transfer(PU); 
  SPI_transfer(ISD_ZERO);
  digitalWrite(CS_PIN,HIGH);
}

void ISD_PD()
{
  digitalWrite(CS_PIN,LOW);
  SPI_transfer(PD); 
  SPI_transfer(ISD_ZERO);
  digitalWrite(CS_PIN,HIGH);
}

void ISD_CLR_INT()
{
  digitalWrite(CS_PIN,LOW);
  SPI_transfer(CLR_INT);
  SPI_transfer(ISD_ZERO);
  digitalWrite(CS_PIN,HIGH);
}

void ISD_RD_STATUS()
{
  digitalWrite(CS_PIN,LOW);
  SR0_L = SPI_transfer(RD_STATUS);
  SR0_H = SPI_transfer(ISD_ZERO);
  SR1_L = SPI_transfer(ISD_ZERO);
  digitalWrite(CS_PIN,HIGH);
} 

void ISD_SET_PLAY(uint16_t VOICE_A, uint16_t VOICE_B)
{
  digitalWrite(CS_PIN,LOW);
  SPI_transfer(SET_PLAY);
  SPI_transfer(ISD_ZERO);
  SPI_transfer(byte(VOICE_A));
  SPI_transfer(byte(VOICE_A >> 8) & 0x07);
  SPI_transfer(byte(VOICE_B));
  SPI_transfer(byte(VOICE_B >> 8) & 0x07);
  SPI_transfer(ISD_ZERO);
  digitalWrite(CS_PIN,HIGH);
}

void ISD_RD_APC()
{
  digitalWrite(CS_PIN,LOW);
  SR0_L = SPI_transfer(RD_APC);
  SR0_H = SPI_transfer(ISD_ZERO);
  APC_L = SPI_transfer(ISD_ZERO);
  APC_H = SPI_transfer(ISD_ZERO);
  digitalWrite(CS_PIN,HIGH);
}

void ISD_WR_APC2()
{
  digitalWrite(CS_PIN,LOW);
  SR0_L = SPI_transfer(WR_APC2);
  SR0_H = SPI_transfer(APC_L);
  SR0_L = SPI_transfer(APC_H);
  digitalWrite(CS_PIN,HIGH);
}

void volume(uint8_t value)
{
  ISD_PU(); delay(10);
 
  ISD_RD_APC(); delay(10);

  APC_L = (APC_L & 0xf8) | (value & 0x07);  
  
  ISD_WR_APC2(); delay(10);
 
  ISD_PD();
}

/****************************************************************************************************/
uint8_t Get_RDY_Bit()
{
  ISD_RD_STATUS();
  return SR1_L & 0x01;
} 

uint8_t Get_CMDERR_Bit()
{
  ISD_RD_STATUS();
  return SR0_L & 0x01;
}

uint8_t Get_PU_Bit()
{
  ISD_RD_STATUS();
  return SR0_L & 0x04;
}

// Monitor INT status for Message Completion
uint8_t Get_INT_Bit()
{
  ISD_RD_STATUS();
  return SR0_L & 0x10;
} 

uint8_t decode(char code)
{
  uint8_t value = 0x00;

  if ((code >= '0') && (code <= '9'))
  {
    value = code - 0x30;
  } 
  else
  if ((code >= 'A') && (code <= 'Z'))
  {
	value = code - 0x37;
  }
 
  return value;
}

// 11.7.4 Playback 3 Messages as 1 Message (using SetPlay)
void playback(const char * message, uint8_t width)
{
  do
  {
	ISD_PU();
  } while (Get_CMDERR_Bit());
  
  while (!Get_PU_Bit());
  
  ISD_CLR_INT();
 
  char code = message[0];
  uint8_t value = decode(code);

  while(!Get_RDY_Bit());
  do
  {
    ISD_SET_PLAY(voiceStartAddr[value],voiceLastAddr[value]);
  } while (Get_CMDERR_Bit());
  
  uint8_t i = 1; 
  while ( i < width )
  {
	code = message[i]; value = decode(code);

	while(!Get_RDY_Bit());	  
    do
	{
	  ISD_SET_PLAY(voiceStartAddr[value],voiceLastAddr[value]);
    } while (Get_CMDERR_Bit());
 
    while(!Get_INT_Bit()); ISD_CLR_INT(); i++;
  }
    
  while(!Get_INT_Bit());
  
  ISD_PD();
}

/****************************************************************************************************/
uint8_t intToVoiceMsg(char * msg, int value)
{
  uint8_t col = 0; 	 
  
  if (value == 0)
  {
    msg[0] = 0x30; msg[1] = 0x00; return 1; 
  }

  if (value < 0)
  {
	msg[col++] = 'J'; value = ~value + 1;
  }

  uint8_t i; uint8_t digits[5];
  for ( i = 0 ; value > 0; i++ )
  {
	digits[i] = value % 10; value /= 10; 
  }

  uint8_t zero_cnt = 0;
  for ( ; i > 0; i-- )
  {
    uint8_t d1 = digits[i - 1];
    if (d1 > 0)
	{
      if (zero_cnt > 0)
      {
        msg[col++] = 0x30; zero_cnt = 0;
      }
	  
	  if (i == 2)
	  {
         if (d1 > 1) msg[col++] = d1 + 0x30;
	  }
	  else
	  {
        msg[col++] = d1 + 0x30;
	  }
    
	  switch(i - 1)
	  {
      case 4 : msg[col++] = 'W'; break;
	  case 3 : msg[col++] = 'Q'; break; 
      case 2 : msg[col++] = 'B'; break;
      case 1 : msg[col++] = 'S'; break;
      }  
	}
    else
	{
      zero_cnt++;
	} 	    
  }  
  
  msg[col] = 0x00; return col;
}

/****************************************************************************************************/
void digitToMsg(char * msg, uint8_t & i, uint8_t value, uint8_t mask)
{
  uint8_t d2 = value / 10, d1 = value % 10;
  
  if (mask & 0x01)
  {
    if (d2 > 1)
    {
      msg[i++] = d2 + 0x30;
    }
  }
  else
  {
    if (d2 != 1)
    {
      msg[i++] = d2 + 0x30;
    }
  }
  if (d2 > 0)
  {
    msg[i++] = 'S';
  }  
  
  if (d1 > 0)
  {
    msg[i++] = d1 + 0x30;
  }
}

uint8_t timeToVoiceMsg(char * msg, uint8_t h, uint8_t m, uint8_t s)
{
  uint8_t col = 0;
  
  if (h == 0)
  {
	  msg[col++] = 0x30;
  }
  else
  {
    digitToMsg(msg,col,h,0x01);
  }
  msg[col++] = 'D'; // "��"

  if ((m == 0) && (s == 0)) msg[col++] = 'A'; // "��" 
  else
  {	
    digitToMsg(msg,col,m,0x00); msg[col++] = 'F'; // "��"
	
	if (s > 0)
	{
	  digitToMsg(msg,col,s,0x00); msg[col++] = 'M'; // "��"
	}
  }

  msg[col] = 0x00; return col;
}

uint8_t tempToVoiceMsg(char * msg, int Temp)
{
  uint8_t col = 0;

  if (Temp == 0)
  {
     msg[0] = 0x30; msg[1] = 0x00; return 1; 
  }

  if (Temp < 0)
  {
    msg[col++] = 'J'; Temp = ~Temp + 1; 
  }
  
  // �ֽ������С���
  uint8_t temp1 = Temp >> 4; 
  uint8_t temp2 = Temp & 0xf;
  temp2 = ((temp2 << 3) + (temp2 << 1)) >> 4;
  
  digitToMsg(msg,col,temp1,0x01);
  
  if (temp2 > 0)
  {
	msg[col++] = 'D'; msg[col++] = temp2 + 0x30;
  }

  msg[col++] = 'C'; msg[col++] = 'G';

  msg[col] = 0x00; return col;
}

uint8_t humiToVoiceMsg(char * msg, uint8_t Humi)
{
  uint8_t col = 0;
 
  msg[col++] = 'H'; msg[col++] = 'G';
   
  msg[col++] = 'B'; msg[col++] = 'F'; msg[col++] = 'Z';
   
  digitToMsg(msg,col,Humi,0x01);
  
  msg[col] = 0x00; return col;
}

uint8_t presToVoiceMsg(char * msg, long pres)
{
   uint8_t col = intToVoiceMsg(msg,int(pres / 100));
   
   msg[col++] = 'B'; msg[col++] = 'P';
   
   msg[col] = 0x00; return col;
}

/****************************************************************************************************/
