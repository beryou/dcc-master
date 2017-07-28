#ifndef _VOICE_1760_1_
#define _VOICE_1760_1_

#include <Arduino.h>
#include "ISD1760.h"

#define CK_PIN 13 // SCK
#define DI_PIN 12 // MISO
#define DO_PIN 11 // MOSI
#define CS_PIN 10 // SS

#define V00 0x0013
#define V01 0x0018

#define V10 0x001d
#define V11 0x0022

#define V20 0x0026
#define V21 0x002b

#define V30 0x002f
#define V31 0x0034

#define V40 0x0039
#define V41 0x003e

#define V50 0x0043
#define V51 0x0048

#define V60 0x004d
#define V61 0x0052

#define V70 0x0055
#define V71 0x005a

#define V80 0x005e
#define V81 0x0063

#define V90 0x0067
#define V91 0x006c

// 数字进位 : 十(时)、百、千、万、亿、点、整、正、负、之、元、角

#define VS0 0x008d
#define VS1 0x0092

#define VB0 0x0095
#define VB1 0x009a

#define VQ0 0x009e
#define VQ1 0x00a3

#define VW0 0x00a8
#define VW1 0x00ad

#define VE0 0x0130
#define VE1 0x0135

#define VD0 0x0072
#define VD1 0x0077

#define VA0 0x0168
#define VA1 0x016d

#define VI0 0x0172
#define VI1 0x0177

#define VJ0 0X017b
#define VJ1 0x0180

#define VZ0 0X0138
#define VZ1 0x013d

#define VO0 0X011d
#define VO1 0x0122

#define VV0 0X0125
#define VV1 0x012a


// 时间 : 年、月、日、星期、时、分、秒、上、下
#define VN0 0x0142
#define VN1 0x0147

#define VY0 0x014c
#define VY1 0x0151

#define VR0 0x0155
#define VR1 0x015a

#define VX0 0x015d
#define VX1 0x0166

#define VF0 0x007b
#define VF1 0x0080

#define VM0 0x0084
#define VM1 0x0089

#define VK0 0x0182
#define VK1 0x0187

#define VL0 0x0188
#define VL1 0x018d

// 温、湿、度、摄氏、气压、帕
#define VT0 0x00b1
#define VT1 0x00b6

#define VH0 0x00bb
#define VH1 0x00c0

#define VG0 0x00c5
#define VG1 0x00ca

#define VC0 0x00d0
#define VC1 0x00d7

#define VU0 0x00e8
#define VU1 0x00f2

#define VP0 0x00f4
#define VP1 0x00f9


/********************************************************************************
 * 二十六发音图
 *
 *  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z
 *
 * 整 百 摄 点 亿 分 度 湿 正 负 上 下 秒 年 元 帕 千 日 十 温 气 角 万 星 月 之
 *       氏                                             (时)   压       期
 *
 *  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z                                                
 *
 ********************************************************************************/

static int voiceStartAddr[36] =
{
  V00, V10, V20, V30, V40, V50, V60, V70, V80, V90,
  VA0, VB0, VC0, VD0, VE0, VF0, VG0, VH0, VI0, VJ0, VK0, VL0, VM0,
  VN0, VO0, VP0, VQ0, VR0, VS0, VT0, VU0, VV0, VW0, VX0, VY0, VZ0
};

static int voiceLastAddr[36] =
{
  V01, V11, V21, V31, V41, V51, V61, V71, V81, V91,
  VA1, VB1, VC1, VD1, VE1, VF1, VG1, VH1, VI1, VJ1, VK1, VL1, VM1,
  VN1, VO1, VP1, VQ1, VR1, VS1, VT1, VU1, VV1, VW1, VX1, VY1, VZ1
};

/********************************************************************************/

static uint8_t SR0_L,SR0_H,SR1_L,APC_L,APC_H;

// ISD1760 SPI传输初始化 
void ISD1760_SPI_INIT();

// 调整1760音量(0最大，7最小)
void volume(uint8_t value);

// 整数转消息字串
uint8_t intToVoiceMsg(char * msg, int value);

// 解码消息字串，利用ISD1760缓冲连续播放发音
void playback(const char * message, uint8_t width);

// 时间转消息字串
uint8_t timeToVoiceMsg(char * msg, uint8_t h, uint8_t m, uint8_t s);

// DS18B20温度转消息字串
uint8_t tempToVoiceMsg(char * msg, int Temp);

// DHT11湿度转消息字串
uint8_t humiToVoiceMsg(char * msg, uint8_t Humi);

// BMP180气压转消息字串
uint8_t presToVoiceMsg(char * msg, long pres);

#endif // _VOICE_1760_1_
