// ISD_STATUS
#include "ISD1760.h"

#define CK_PIN 13 // SCK
#define DI_PIN 12 // MISO
#define DO_PIN 11 // MOSI
#define CS_PIN 10 // SS

byte SR0_L,SR0_H,SR1_L,APC_L,APC_H,data1,data2,data3,data4;

#define STR_BUF_LEN 0x20
char c_str[STR_BUF_LEN];

/****************************************************************************************************/

void setup()
{
  ISD1760_SPI_INIT();
  delay(100);

  Serial.begin(9600);
  delay(100);

  ISD_PU();
  delay(100);  

  ISD_CLR_INT();
  delay(100);

  uint8_t ID = ISD_DEVID();
  delay(100);
  Serial.print("CHIP ID : ");
  Serial.println(ID);
  Serial.println();

  //ISD_Volume(3);

  ISD_RD_STATUS();
  delay(100);
  DISP_SR0(); 
  DISP_SR1();

  ISD_RD_APC();
  delay(100);
  DISP_APC();

  APC_L = (APC_L & 0xf8) | 0x07;  
  ISD_WR_APC1();
  delay(100);
  DISP_APC();

  ISD_RD_PLAY_PTR();
  delay(100);
  Serial.print("PLAY Pointer : ");
  DISP_ADDR(data2,data1);
  Serial.println();

  ISD_RD_REC_PTR();
  delay(100); 
  Serial.print("RECORD Pointer : ");
  DISP_ADDR(data2,data1);
  Serial.println();

  delay(100);
}

void loop()
{
  // PLAY
  digitalWrite(CS_PIN,LOW);
  SPI_transfer(PLAY); 
  SPI_transfer(ISD_ZERO);
  digitalWrite(CS_PIN,HIGH);
  delay(100);
  
  // WAIT PLAY DONE
  ISD_RD_STATUS();
  while (!(SR1_L & 1))
  {
    delay(100);
    ISD_RD_STATUS();
  }
  // DISP_SR1();
  Serial.println("PLAY done.");
  delay(100);
  
  digitalWrite(CS_PIN,LOW);
  SPI_transfer(FWD);
  SPI_transfer(ISD_ZERO);
  digitalWrite(CS_PIN,HIGH);
  delay(100);

  ISD_RD_PLAY_PTR();
  delay(100);
  Serial.print("PLAY Pointer : ");
  DISP_ADDR(data2,data1);
  Serial.println();
  delay(100);
}

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

uint8_t ISD_DEVID()
{
  digitalWrite(CS_PIN,LOW);
  SR0_L = SPI_transfer(DEVID);
  SR0_H = SPI_transfer(ISD_ZERO);
  data1 = SPI_transfer(ISD_ZERO);
  digitalWrite(CS_PIN,HIGH);

  return data1 >> 3;
}

void DISP_ADDR(byte ADR_H, byte ADR_L)
{
  snprintf(c_str,STR_BUF_LEN,"0x%02x%02x",ADR_H,ADR_L);
  Serial.println(c_str);
}

void DISP_SR0()
{
  Serial.println("Status Register 0 :");
  Serial.println();
  Serial.print("CMDERR : "); Serial.println(SR0_L & 1);
  Serial.print("  FULL : "); Serial.println((SR0_L >> 1) & 1);
  Serial.print("    PU : "); Serial.println((SR0_L >> 2) & 1);
  Serial.print("   EOM : "); Serial.println((SR0_L >> 3) & 1);
  Serial.print("   INT : "); Serial.println((SR0_L >> 4) & 1);
  Serial.print("  LINE : ");
  DISP_ADDR((SR0_H >> 5), (SR0_L >> 5) | (SR0_H << 3));
  Serial.println();
}

void DISP_SR1()
{
  Serial.println("Status Register 1 :");
  Serial.println();
  Serial.print("   RDY : "); Serial.println(SR1_L & 1);
  Serial.print(" REASE : "); Serial.println((SR1_L >> 1) & 1);
  Serial.print("  PLAY : "); Serial.println((SR1_L >> 2) & 1);
  Serial.print("   REC : "); Serial.println((SR1_L >> 3) & 1);
  Serial.print("   SE4 : "); Serial.println((SR1_L >> 4) & 1);
  Serial.print("   SE3 : "); Serial.println((SR1_L >> 5) & 1);
  Serial.print("   SE2 : "); Serial.println((SR1_L >> 6) & 1);
  Serial.print("   SE1 : "); Serial.println(SR1_L >> 7);
  Serial.println();
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

void DISP_APC()
{
  Serial.println("APC Register :");
  Serial.println();
  Serial.print("       Volume : "); Serial.println(APC_L & 0x07);
  Serial.print("Monitor_Input : "); Serial.println((APC_L >> 3) & 1);
  Serial.print("    Mix_Input : "); Serial.println((APC_L >> 4) & 1);
  Serial.print("   SE_Editing : "); Serial.println((APC_L >> 5) & 1);
  Serial.print("       SPI_FT : "); Serial.println((APC_L >> 6) & 1);
  Serial.print("Analog Output : "); Serial.println(APC_L >> 7);
  Serial.print("      PWM SPK : "); Serial.println(APC_H & 1);
  Serial.print("PU Analog Out : "); Serial.println((APC_H >> 1) & 1);
  Serial.print("       VAlert : "); Serial.println((APC_H >> 2) & 1);
  Serial.print("   EOM Enable : "); Serial.println((APC_H >> 3) & 1);
  Serial.println();  
}

void ISD_RD_PLAY_PTR()
{
  digitalWrite(CS_PIN,LOW);
  SR0_L = SPI_transfer(RD_PLAY_PTR);
  SR0_H = SPI_transfer(ISD_ZERO);
  data1 = SPI_transfer(ISD_ZERO);
  data2 = SPI_transfer(ISD_ZERO);
  digitalWrite(CS_PIN,HIGH);
}

void ISD_RD_REC_PTR()
{
  digitalWrite(CS_PIN,LOW);
  SR0_L = SPI_transfer(RD_REC_PTR);
  SR0_H = SPI_transfer(ISD_ZERO);
  data1 = SPI_transfer(ISD_ZERO);
  data2 = SPI_transfer(ISD_ZERO);
  digitalWrite(CS_PIN,HIGH);
}

void ISD_WR_APC1()
{
  digitalWrite(CS_PIN,LOW);
  SR0_L = SPI_transfer(WR_APC1);
  SR0_H = SPI_transfer(APC_L);
  SR0_L = SPI_transfer(APC_H);
  digitalWrite(CS_PIN,HIGH);
}

/*void ISD_Volume(uint8_t value)
{
  ISD_RD_APC();
  delay();
  APC_L = (APC_L & 0xf8) | (value & 0x07);
  ISD_WR_APC1();
}*/


