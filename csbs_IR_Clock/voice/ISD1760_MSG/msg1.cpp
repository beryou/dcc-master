#include <iostream>

using std::cout;
using std::endl;

typedef unsigned char uint8_t;

char voiceMsg[0x20];

uint8_t intToVoiceMsg(char * msg, int value)
{
  uint8_t col = 0; 	 
  
  if (value == 0)
  {
    msg[0] = 0x30; msg[1] = 0x00; return 1; 
  }

  if (value < 0)
  {
	msg[col++] = '-'; value = ~value + 1;
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

uint8_t longToVoiceMsg(char * msg, long value)
{
  uint8_t col = 0; 	 
  
  if (value == 0)
  {
    msg[0] = 0x30; msg[1] = 0x00; return 1; 
  }

  if (value < 0)
  {
	msg[col++] = '-'; value = ~value + 1;
  }

  uint8_t i; uint8_t digits[10];
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
	  
	  if (((i - 1) & 0x03) == 1)
	  {
         if (d1 > 1) msg[col++] = d1 + 0x30;
	  }
	  else
	  {
        msg[col++] = d1 + 0x30;
	  }
    
	  switch((i - 1) & 0x03)
	  {
      case 3 : msg[col++] = 'Q'; break; 
      case 2 : msg[col++] = 'B'; break;
      case 1 : msg[col++] = 'S'; break;
      }  
	}
    else
	{
      zero_cnt++;
	} 	
    
	if ((i - 1) == 8)
    {
	  msg[col++] = 'E'; zero_cnt = 0;
    }
    if ((i - 1) == 4)
    {
      if (zero_cnt < 4)
      {
        msg[col++] = 'W'; zero_cnt = 0;  
      }
    }
  }  
  
  msg[col] = 0x00; return col;
}

int main()
{
  long n1 = 2000000101L, n2 = -2010001011L, n3 = 1234567890L, n4 = 1000030000L;
  
  longToVoiceMsg(voiceMsg,n1);
  cout << n1 << endl << voiceMsg << endl << endl;

  longToVoiceMsg(voiceMsg,n2); 
  cout << n2 << endl << voiceMsg << endl << endl;

  longToVoiceMsg(voiceMsg,n3); 
  cout << n3 << endl << voiceMsg << endl << endl;

  longToVoiceMsg(voiceMsg,n4); 
  cout << n4 << endl << voiceMsg << endl << endl;

  int d1 = 10000, d2 = 1001, d3 = 10101, d4 = 120, d5 = 0, d6 = 7;

  intToVoiceMsg(voiceMsg,d1);
  cout << d1 << endl << voiceMsg << endl << endl;

  intToVoiceMsg(voiceMsg,d2); 
  cout << d2 << endl << voiceMsg << endl << endl;

  intToVoiceMsg(voiceMsg,d3); 
  cout << d3 << endl << voiceMsg << endl << endl;

  intToVoiceMsg(voiceMsg,d4); 
  cout << d4 << endl << voiceMsg << endl << endl;
  
  intToVoiceMsg(voiceMsg,d5);
  cout << d5 << endl << voiceMsg << endl << endl;

  intToVoiceMsg(voiceMsg,d6); 
  cout << d6 << endl << voiceMsg << endl << endl;*/

  return 0;
}

// g++ -Wall -o msg1 msg1.cpp
