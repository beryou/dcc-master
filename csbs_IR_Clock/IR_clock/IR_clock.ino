/******************************************************************************
 * 多 功 能 遥 控 语 音 时 钟
 * 
 * 所需器件 :
 *     Arduino UNO, DS3221时钟, BMP180气压计, DS18B20数字温度计, DHT11湿度计,
 *     ISD1760录音机, 8欧姆0.5瓦或1瓦扬声器, 1602液晶屏, VS1838红外一体接收器
 *     NEC编码遥控器
 *
 *  作者 : 草绳绑松                                2015/4/21 22:02
 *  邮箱 : rmjzqs0001@qq.com
 ******************************************************************************/
//#define _TWI_DEBUG

// include the library code:
#include <IRremote.h>      // IR remote control library
#include <LiquidCrystal.h> // LCD 1602 display library 
#include "voice60.h"

const char * __DS3231__   = "DS3231";
const char * __BMP180__   = "BMP180";
const char * __DHT11__    = "DHT11";
const char * __DS18B20__  = "DS18B20";
const char * __ISD1760__  = "ISD1760";
const char * __AT24C32__  = "AT24C32";

/******************************************************************************
LCD 1602 circuit :
 * LCD RS pin to digital pin 2
 * LCD Enable pin to digital pin 3
 * LCD D4 pin to digital pin 4
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 6
 * LCD D7 pin to digital pin 7
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to +5V
 * 10K resistor : ends to +5V and ground wiper to LCD VO pin
******************************************************************************/

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(2,3,4,5,6,7);

const uint8_t LCD_COLS = 0x10;
const uint8_t LCD_ROWS = 0x02;

char LINE[LCD_COLS + 1]; // 1602 LCD Display Buffer 

#define wait_500ms 500UL
#define wait_750ms 750UL
unsigned long flush_time1, flush_time2, flush_time3;

uint8_t LCD_PAGE; 

/******************************************************************************
VS1838 circuit :
 * VS1838 OUT pin to digital pin 8
 * VS1838 GND pin to to ground
 * VS1838 VDD pin to +5V
 * 22pF capacitor connected +5V with ground
******************************************************************************/

const uint8_t IR_REC_PIN = 8;    // pin connected to the output of the IR

const uint8_t IR_numOfKeys = 5;  // 5 keys are learned (number key 1 through 5)      
long IR_KeyCodes[IR_numOfKeys] = // holds the codes for each key
{ 
  0xff30cf, 0xff18e7, 0xff7a85, 0xff10ef, 0xff38c7 
};

// �����������ʵ��irrecv���������ң���źŴ����result
IRrecv irrecv(IR_REC_PIN);    
decode_results results; 

boolean IR_voice = false;

/******************************************************************************
TWI Setup :
 * SCL frequency 100kHz(TWBR = 72,no prescaler)
 * Clear TWINT, Enable TWI, Disiable TWIE
 
TWI function : 
 * TWI_read_bytes(dev_addr,reg_addr,nbytes,data);
 * TWI_write_bytes(dev_addr,reg_addr,nbytes,data);
******************************************************************************/

#define TWI_TRANS_BYTES 0x20
uint8_t TWI_BUF[TWI_TRANS_BYTES]; uint8_t TWI_ibyte;

/******************************************************************************
DS3231 | AT24C32 | BMP180 circuit : 

 * Model GND pin to to ground
 * Model Vcc pin to +5V

You will also need to connect the I2C pins (SCL and SDA) to your Arduino. 
The pins are different on different Arduinos:

Any Arduino pins labeled:  SDA  SCL
Uno, Redboard, Pro:        A4   A5
Mega2560, Due:             20   21
Leonardo:                   2    3
******************************************************************************/

#define CLOCK_ADDRESS  0x68
#define E2PROM_ADDRESS 0x57
#define BMP180_ADDRESS 0x77

uint8_t Second,Minute,Hour,Day,Date,Month,Year; int Temp025;

uint8_t OSS; boolean cali_read_first = true;
int AC1,AC2,AC3,b1,B2,MB,MC,MD; unsigned int AC4,AC5,AC6; 
long B5,UT,UP,Temp01,Pressure; double Altitude; 

/******************************************************************************
DS18B20连接方法 :
 * DS18B20 DQ pin to digital pin 9
 * DS18B20 GND pin to to ground
 * DS18B20 VCC pin to +5V
 * 4.7k pull-up resistor
*******************************************************************************/

const uint8_t DSTMP_PIN = 9; // pin connected to the DQ of the DS18B20

int Temp0625; boolean DS18B20_busy;

/******************************************************************************
DHT11连接方法 :
 * DHT11 DATA pin to digital pin A0 (PC0)
 * DHT11 GND pin to to ground
 * DHT11 VCC pin to +5V
 * 5k pull-up resistor
 * 100nF capacitor connected +5V with ground
*******************************************************************************/

const uint8_t DHT11_PIN = A0; // pin connected to the DATA of the DHT11

#define DHT11_SUCCESS         0
#define DHT11_ERROR_NOACK    -1
#define DHT11_ERROR_CHECKSUM -2

uint8_t DHT11_DATA[5]; // 湿度数据、未使用(0)、温度数据、未使用(0)、校验和

/******************************************************************************
ISD1760连接方法 :
 * ISD1760 ss pin to digital pin 10
 * ISD1760 MOSI pin to digital pin 11
 * ISD1760 MISO pin to digital pin 12
 * ISD1760 SCLK pin to digital pin 13
 * ISD1760 VCC pin to +5V
 * ISD1760 GND pin to to ground 
 * 8R 0.5W or 1W Speaker 
*******************************************************************************/

char voiceMsg[0x20];


/*******************************************************************************
  Main Procedure : setup & loop                                             
*******************************************************************************/
void setup()
{ 
#if defined(_DS3231_DEBUG) || defined(_BMP180_DEBUG)
  Serial.begin(9600);
#endif

  // set up the LCD's number of columns and rows:
  lcd.begin(LCD_COLS,LCD_ROWS);
 
  // Start the IR receiver
  pinMode(IR_REC_PIN,INPUT);
  irrecv.enableIRIn();              
  
  // �Ĵ�������TWI��Ԫ������Wire����IRremote���жϳ�ͻ
  TWI_INIT(); 
  
  ISD1760_SPI_INIT(); volume(3);

  LCD_PAGE = 0; begin_DS3231();
}
 
void loop()
{
  int key;
  if (irrecv.decode(&results))
  {
    // here if data is received
    irrecv.resume();
    key = convertCodeToKey(results.value);
    if((key >= 0) && (key <=3))
    {
      PAGE_switch(key);
      LCD_PAGE = key;
    }
    if (key == 4)
    {
	  IR_voice = true;
    }
  }

  switch(LCD_PAGE)
  {
    case 0 : now_DS3231();
             break;
    case 1 : Operator_DS18B20(DSTMP_PIN); 
             break;
    case 2 : dispData_DHT11(DHT11_PIN,DHT11_DATA);
             break;
    case 3 : dispData_BMP180();
             break;
  }
}

// converts a remote protocol code to a logical key code (or -1 if no digit received)
int convertCodeToKey(long code)
{
  for(int i = 0; i < IR_numOfKeys; i++)
  {
    if(code == IR_KeyCodes[i])
    {
      return i; // found the key so return it
    }
  }
  return -1;
}

void PAGE_switch(uint8_t new_page)
{
  if (new_page != LCD_PAGE)
  {
    switch(new_page)
    {
      case 0 : begin_DS3231();  break;
      case 1 : begin_DS18B20(); break;
	  case 2 : begin_DHT11();   break;
	  case 3 : begin_BMP180();  break;      
    }
	LCD_PAGE = new_page;
  }
}


/*******************************************************************************
  TWI Operater                                             
*******************************************************************************/
void TWI_INIT()
{
  TWBR = 0b01001000;  // rate 72
  TWSR &= 0b11111100; // no prescaler, SCL frequency 100kHz 
  TWCR = 0b10000100;  // Clear TWINT, Enale TWI
}

inline uint8_t TWI_READY(uint8_t OR_operator)
{
  TWCR = 0x84 | OR_operator;
  while (!(TWCR & 0x80));
  return TWSR & 0xf8; 
}

uint8_t TWI_WRITE(uint8_t dev_addr, uint8_t reg_addr)
{
  uint8_t _status;
  
  _status = TWI_READY(0x20); // START
  if (_status != 0x08) return _status | 0x01; 

  TWDR = (dev_addr << 1) | 0x00; 
  _status = TWI_READY(0x00);
  if (_status != 0x18) return _status | 0x02; 
 
  TWDR = reg_addr;
  _status = TWI_READY(0x00);
  if (_status != 0x28) return _status | 0x03; 

  return _status;
}

uint8_t TWI_read_bytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t nbytes, uint8_t * data)
{
  uint8_t _status;
  
  _status = TWI_WRITE(dev_addr,reg_addr);
  if (_status != 0x28) return _status; 
  
  _status = TWI_READY(0x20); // RESTART
  if (_status != 0x10) return _status | 0x04; 

  TWDR = (dev_addr << 1) | 0x01; 
  _status = TWI_READY(0x00);
  if (_status != 0x40) return _status | 0x05; 

  for (TWI_ibyte = 0; TWI_ibyte < nbytes - 1; TWI_ibyte++)
  {
    _status = TWI_READY(0x40); // Enable TWEA, master ACK
    if (_status != 0x50) return _status | 0x06;  
    
    data[TWI_ibyte] = TWDR;
  }

  _status = TWI_READY(0x00);
  if (_status != 0x58) return _status | 0x07; 
  
  data[nbytes - 1] = TWDR; 

  TWCR = 0b10010100; // STOP  
  // delay(1);

  return _status;
}

uint8_t TWI_write_bytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t nbytes, const uint8_t * data)
{
  uint8_t _status;

  _status = TWI_WRITE(dev_addr,reg_addr);
  if (_status != 0x28) return _status; 
 
  for (TWI_ibyte = 0; TWI_ibyte < nbytes; TWI_ibyte++)
  {
    TWDR = data[TWI_ibyte]; 
    _status = TWI_READY(0x00);
    if (_status != 0x28) return _status | 0x03;
  }
  
  TWCR = 0b10010100; // STOP  
  // delay(5);

  return _status;
}

inline unsigned int TWI_getUINT(uint8_t i)
{
  uint8_t MSB = TWI_BUF[i << 1], LSB = TWI_BUF[i << 1 + 1];
  return ((unsigned int)MSB<<8)|(unsigned int)LSB; 
}

#ifdef _TWI_DEBUG  
void TWI_ERROR(const char * m, uint8_t s, uint8_t a)
{ 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(m);
  lcd.setCursor(0,1);  
  lcd.print("0x"); lcd.print(s,HEX);
  lcd.setCursor(5,1);
  lcd.print("0x"); lcd.print(a,HEX);
  lcd.setCursor(10,1);
  lcd.print(TWI_ibyte);

  while(true);
}
#endif

/*******************************************************************************
  LCD 1602 Operator 
*******************************************************************************/

boolean IntToStr(char * c_str, int value, uint8_t width)
{
  boolean _signed = false;
  if (value & 0x8000)
  {
    value = ~value + 1; _signed = true;
  }

  uint8_t col;
  for (col = width; value > 0; col--)
  {
    c_str[col - 1] = value % 10 + 0x30; value /= 10;
  }
  if (value > 0) return false;
  
  if (_signed)
  {	
    if (col == 0) return false;
    c_str[--col] = 0x2d;  
  }
    
  while (col > 0) c_str[--col] = 0x20;
  c_str[width] = 0x00;
  
  return true;
}

inline void dispSymbol(uint8_t col, uint8_t row, uint8_t code)
{
  lcd.setCursor(col,row); lcd.write(code);
}

void dispWidthValue(uint8_t col, uint8_t row, uint8_t width, int value)
{
  if (IntToStr(LINE,value,width))
  {
    lcd.setCursor(col,row); 
    lcd.print(LINE);	
  }
}

inline void dispValue(uint8_t col, uint8_t row, uint8_t value)
{
  lcd.setCursor(col,row);
  lcd.print(value / 10);
  lcd.print(value % 10);
}

void dispTempSymbol(uint8_t col, uint8_t row, uint8_t code)
{
#define _Degree B11011111 
  dispSymbol(col,row,_Degree);
#undef _Degree
  
  lcd.setCursor(col + 1,row);
  switch (code)
  {
  case 0 : lcd.print('C'); break;
  case 1 : lcd.print('F'); break;
  case 2 : lcd.print('K'); break;	  
  }
}

void dispWeek_DS3231(uint8_t col, uint8_t row)
{
  lcd.setCursor(col,row);
  switch(Day)
  {
  case 1 : lcd.print("MON"); break;
  case 2 : lcd.print("TUE"); break;
  case 3 : lcd.print("WED"); break;
  case 4 : lcd.print("THU"); break;
  case 5 : lcd.print("FRI"); break;
  case 6 : lcd.print("SAT"); break;
  case 7 : lcd.print("SUN"); break;
  }
}

void dispDate_DS3231(uint8_t col, uint8_t row)
{
  snprintf(LINE,0x0b,"20%02d-%02d-%02d",Year,Month,Date);
  
  lcd.setCursor(col,row); 
  lcd.print(LINE);
}  

void dispTime_DS3231(uint8_t col, uint8_t row)
{
  snprintf(LINE,0x09,"%02d:%02d:%02d",Hour,Minute,Second);
 
  lcd.setCursor(col,row); 
  lcd.print(LINE);
}

/*******************************************************************************
  DS3231 Operator
*******************************************************************************/

boolean readDate_DS3231(uint8_t & _date, uint8_t & _month, uint8_t & _year, uint8_t & _day)
{
  uint8_t result;

#define REG_ADDR 0x03  
  result = TWI_read_bytes(CLOCK_ADDRESS,REG_ADDR,4,TWI_BUF + REG_ADDR);
  if (result != 0x58)
  {
#ifdef _TWI_DEBUG
    TWI_ERROR(__DS3231__,result,REG_ADDR);
#endif
#undef REG_ADDR
    return false;
  }

  _day   = bcdToDec(TWI_BUF[3]); 
  _date  = bcdToDec(TWI_BUF[4]); 
  _month = bcdToDec(TWI_BUF[5]);
  _year  = bcdToDec(TWI_BUF[6]);

  return true;
}

boolean readTime_DS3231(uint8_t & _second, uint8_t & _minute, uint8_t & _hour)
{
  uint8_t result;

#define REG_ADDR 0x00
  result = TWI_read_bytes(CLOCK_ADDRESS,REG_ADDR,3,TWI_BUF);
  if (result != 0x58)
  {
#ifdef _TWI_DEBUG
    TWI_ERROR(__DS3231__,result,REG_ADDR);
#endif
#undef REG_ADDR
    return false;
  }

  _second = bcdToDec(TWI_BUF[0]);
  _minute = bcdToDec(TWI_BUF[1]);
  _hour   = bcdToDec(TWI_BUF[2]);
 
  return true;
}

boolean readTemp_DS3231(int & new_temp)
{
  uint8_t result;

#define REG_ADDR 0x11
  result = TWI_read_bytes(CLOCK_ADDRESS,REG_ADDR,2,TWI_BUF + REG_ADDR);
  if (result != 0x58)
  {
#ifdef _TWI_DEBUG
    TWI_ERROR(__DS3231__,result,REG_ADDR);
#endif
#undef REG_ADDR
    return false;
  }

  new_temp = TWI_BUF[0x11];
  if (new_temp & 0x80) new_temp |= 0xff00;

  return true;
}

boolean write_DS3231()
{
  TWI_BUF[0] = decToBcd(Second);
  TWI_BUF[1] = decToBcd(Minute);
  TWI_BUF[2] = decToBcd(Hour);
  TWI_BUF[3] = decToBcd(Day);
  TWI_BUF[4] = decToBcd(Date);
  TWI_BUF[5] = decToBcd(Month);
  TWI_BUF[6] = decToBcd(Year);

  uint8_t result;

#define REG_ADDR 0x00
  result = TWI_write_bytes(CLOCK_ADDRESS,REG_ADDR,7,TWI_BUF);
  if (result != 0x28)
  {
#ifdef _TWI_DEBUG
    TWI_ERROR(__DS3231__,result,REG_ADDR);
#endif
#undef REG_ADDR
    return false;
  }
  
  return true;
}

byte decToBcd(byte val) // Convert normal decimal numbers to binary coded decimal 
{
  return ((val / 10) << 4) + (val % 10);
}

byte bcdToDec(byte val) // Convert binary coded decimal to normal decimal numbers 
{
  byte ten = val >> 4;
  return ((ten << 3) + (ten << 1)) + (val & 0xf);
}

void begin_DS3231()
{
  TWI_INIT();

  readDate_DS3231(Date,Month,Year,Day);
  readTime_DS3231(Second,Minute,Hour);
  readTemp_DS3231(Temp025);

  lcd.clear();
  dispDate_DS3231(1,0);
  dispWeek_DS3231(12,0);
  dispTime_DS3231(1,1);
  dispWidthValue(10,1,3,Temp025);
  dispTempSymbol(13,1,0);
}

void now_DS3231()
{
  uint8_t new_second, new_minute, new_hour; int new_temp;
  
  if (IR_voice) voice_sound(); 
  
  readTime_DS3231(new_second,new_minute,new_hour);

  if (new_second == Second) return;
  Second = new_second; dispValue(7,1,Second);

  readTemp_DS3231(new_temp); new_temp--;
  if (new_temp != Temp025)
  {
     Temp025 = new_temp; 
     dispWidthValue(10,1,3,Temp025);
  }  

  if (new_minute == Minute) return;
  Minute = new_minute; dispValue(4,1,Minute);
  
  if (new_hour == Hour) return;
  Hour = new_hour; dispValue(1,1,Hour);

  readDate_DS3231(Date,Month,Year,Day);  
  dispDate_DS3231(1,0); dispWeek_DS3231(12,0);


}

/*******************************************************************************
  BMP180 Operator
*******************************************************************************/

// Read calibration data from the E2PROM of the BMP180
// read out E2PROM registers, 16 bit, MSB first
bool readCali_BMP180()
{
  if (!readINT_BMP180(0xaa,AC1)) return false;
  if (!readINT_BMP180(0xac,AC2)) return false;
  if (!readINT_BMP180(0xae,AC3)) return false; 
  if (!readUINT_BMP180(0xb0,AC4)) return false;
  if (!readUINT_BMP180(0xb2,AC5)) return false; 
  if (!readUINT_BMP180(0xb4,AC6)) return false;
  if (!readINT_BMP180(0xb6,b1)) return false; 
  if (!readINT_BMP180(0xb8,B2)) return false;
  if (!readINT_BMP180(0xba,MB)) return false;  
  if (!readINT_BMP180(0xbc,MC)) return false;
  if (!readINT_BMP180(0xbe,MD)) return false;

#ifdef _BMP180_DEBUG
  Serial.print(" AC1 = "); Serial.println(AC1);
  Serial.print(" AC2 = "); Serial.println(AC2);
  Serial.print(" AC3 = "); Serial.println(AC3);
  Serial.print(" AC4 = "); Serial.println(AC4);
  Serial.print(" AC5 = "); Serial.println(AC5);
  Serial.print(" AC6 = "); Serial.println(AC6);
  Serial.print("  B1 = "); Serial.println(b1);
  Serial.print("  B2 = "); Serial.println(B2);
  Serial.print("  MB = "); Serial.println(MB);
  Serial.print("  MC = "); Serial.println(MC);
  Serial.print("  MD = "); Serial.println(MD);
  Serial.println();
#endif

  return true;
}

boolean writeByte_BMP180(uint8_t reg_addr, uint8_t value)
{
  uint8_t result;
  uint8_t data[2]; data[0] = reg_addr; data[1] = value;
  
  result = TWI_write_bytes(BMP180_ADDRESS,reg_addr,1,data + 1);
  if (result != 0x28)
  {
#ifdef _TWI_DEBUG
	TWI_ERROR(__BMP180__,result,reg_addr);
#endif
	return false;
  }
  
  return true;
}

boolean readUINT_BMP180(uint8_t reg_addr, unsigned int & value)
{
  uint8_t result;
  uint8_t data[2];
  
  result = TWI_read_bytes(BMP180_ADDRESS,reg_addr,2,data);
  if (result != 0x58)
  {
#ifdef _TWI_DEBUG
	TWI_ERROR(__BMP180__,result,reg_addr);
#endif
	return false;
  }
  
  value = ((unsigned int)data[0]<<8)|(unsigned int)data[1];
  return true;
}

boolean readINT_BMP180(uint8_t reg_addr, int & value)
{
  unsigned int _value; boolean result;
  result = readUINT_BMP180(reg_addr,_value);
  
  value = _value;
  return result;
}

// read uncompensated temperature value
boolean readUT_BMP180(long & value)
{  	
  if (!writeByte_BMP180(0xf4,0x2e)) return false;
  delay(5); // wait 4.5ms

  unsigned int UINT_UT;
  if (!readUINT_BMP180(0xf6,UINT_UT)) return false;
 
  value = UINT_UT;  
  return true;
}

// read uncompensated pressure value
boolean readUP_BMP180(long & value)
{
  uint8_t CRV, wait_ms;
  switch (OSS)
  {
    case 0 : CRV = 0x34; wait_ms = 5;  break; // 4.5ms
    case 1 : CRV = 0x74; wait_ms = 8;  break; // 7.5ms
    case 2 : CRV = 0xb4; wait_ms = 14; break; // 13.5ms
    case 3 : CRV = 0xf4; wait_ms = 26; break; // 25.5ms
    default: CRV = 0x34; wait_ms = 5;
  }
  
  if (!writeByte_BMP180(0xf4,CRV)) return false;
  delay(wait_ms);
   
  uint8_t result;
  uint8_t data[3];

#define REG_ADDR 0xf6
  result = TWI_read_bytes(BMP180_ADDRESS,REG_ADDR,3,data);
  if (result != 0x58)
  {
#ifdef _TWI_DEBUG
	TWI_ERROR(__BMP180__,result,REG_ADDR);	
#endif
#undef REG_ADDR
	return false;
  }

  value = ((unsigned long)data[0]<<16)|((unsigned long)data[1]<<8)|(unsigned long)data[2];
  value = value >> (8 - OSS);
  return true;
}

// calculate true temperature
long calcTemp_BMP180()
{
  long X1, X2, T1;
  
  X1 = (UT - AC6) * AC5 >> 15;
  X2 = ((long)MC << 11) / (X1 + MD);
  B5 = X1 + X2;
  T1 = (B5 + 8) >> 4;
  
  return T1;
}

// calculate true pressure
long calcPressure_BMP180()
{
  long X1, X2, X3, B3, B6, P1; unsigned long B4, B7;
  
  B6 = B5 - 4000;
  X1 = B2 * (B6 * B6 >> 12) >> 11; 
  X2 = AC2 * B6 >> 11;
  X3 = X1 + X2;
  B3 = (((((long)AC1 << 2) + X3) << OSS) + 2) >> 2; 

  X1 = (AC3 * B6) >> 13;
  X2 = (b1 * ((B6 * B6) >> 12)) >> 16;
  X3 = (X1 + X2 + 2) >> 2;
  B4 = AC4 * (unsigned long)(X3 + 32768) >> 15;
  B7 = ((unsigned long)UP - B3) * (50000 >> OSS);
  if (B7 < 0x80000000) 
  { 
	P1 = (B7 << 1) / B4; 
  }
  else 
  { 
	P1 = (B7 / B4) << 1; 
  }

  X1 = (P1 >> 8) * (P1 >> 8);

  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * P1) >> 16;
  P1 = P1 + ((X1 + X2 + 3791) >> 4);

  return P1;
}

// altitude = 44330*(1-(pressure/101325)^0.190295);
double calcAltitude_BMP180(long P1)
{
  double P2 = P1 / 101325.0; 
  P2 = pow(P2,0.190295);
  return 44330 * (1 - P2); 
}

void begin_BMP180()
{
  OSS = 2; // high resolution, Conversion time pressure max. 13.5ms
  if (cali_read_first)
  {
    readCali_BMP180(); cali_read_first = false;
  }
  
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print(__BMP180__);
  lcd.setCursor(7,0);
  lcd.print(':');
  lcd.setCursor(12,0);
  lcd.print('.');
  dispTempSymbol(14,0,0);
  
  lcd.setCursor(4,1);
  lcd.print('m');
  lcd.setCursor(10,1);
  lcd.print('.');  
  lcd.setCursor(13,1);
  lcd.print("hPa");
}

boolean calcData_BMP180(long & _Temp, long & _pressure, double  & _altitude)
{
  if (!readUT_BMP180(UT)) return false;
  if (!readUP_BMP180(UP)) return false;

#ifdef _BMP180_DEBUG
  Serial.print("UT = "); Serial.println(UT);
  Serial.print("UP = "); Serial.println(UP);
#endif

  _Temp     = calcTemp_BMP180();
  _pressure = calcPressure_BMP180();
  _altitude = calcAltitude_BMP180(_pressure);

#ifdef _BMP180_DEBUG
  Serial.print("Temperature = ");
  Serial.println(_Temp*0.1,1);
  
  Serial.print("Pressure = ");
  Serial.print(_pressure*0.01,1);
  Serial.println("hPa");

  Serial.print("altitude = ");
  Serial.print(_altitude,1);
  Serial.println('m');
  Serial.println();
#endif

  return true;
}

void dispData_BMP180()
{
  if ((millis() - flush_time3) > wait_750ms)
  {
    if (calcData_BMP180(Temp01,Pressure,Altitude))
    {
      flush_time3 = millis();
      
      dispWidthValue(9,0,3,int(Temp01/10));
      lcd.setCursor(13,0);
      lcd.print(Temp01 % 10);

      dispWidthValue(6,1,4,int(Pressure/100)); 
      dispWidthValue(11,1,2,int(Pressure%100));

      dispWidthValue(0,1,4,int(Altitude));
    
	  if (IR_voice) voice_sound();	
	}
  }  
}

/*******************************************************************************
  DS18B20 Operator
*******************************************************************************/

int DS18B20_RESET(uint8_t DATA_PIN)
{
  int ack; 
 
  pinMode(DATA_PIN,OUTPUT);
  digitalWrite(DATA_PIN,HIGH);

  digitalWrite(DATA_PIN,LOW);
  delayMicroseconds(600); // 480 ~ 960 us
  
  digitalWrite(DATA_PIN,HIGH);
  delayMicroseconds(60); // 15 ~ 60 us

  pinMode(DATA_PIN,INPUT);
  ack = digitalRead(DATA_PIN);
  
  while (!digitalRead(DATA_PIN));
  delayMicroseconds(240);
    
  return ack;
}

void DS18B20_WRITE(uint8_t DATA_PIN, uint8_t value)
{
  pinMode(DATA_PIN,OUTPUT);
  for (uint8_t mask = 0x01; mask; mask <<= 1)
  {
    digitalWrite(DATA_PIN,LOW);
    delayMicroseconds(1);
    if (value & mask)
    {
      digitalWrite(DATA_PIN,HIGH);
    }
    else
    {
      digitalWrite(DATA_PIN,LOW);
    }

    delayMicroseconds(90); // 60 ~ 120us
   
    digitalWrite(DATA_PIN,HIGH);
    delayMicroseconds(1);
  }
}

uint8_t DS18B20_READ(uint8_t DATA_PIN)
{
  uint8_t value;
  for (uint8_t mask = 0x01; mask; mask <<=1)
  {
    SREG &= 0x7f; 
    pinMode(DATA_PIN,OUTPUT);
    digitalWrite(DATA_PIN,LOW);
    delayMicroseconds(1);
    
    digitalWrite(DATA_PIN,HIGH);
    pinMode(DATA_PIN,INPUT);
    if (digitalRead(DATA_PIN))
    {
      value |= mask; 
    }
    else
    {
      value &= ~mask;
    } 
    SREG |= 0x80; 

    delayMicroseconds(60);
  }

  return value;
}

int convTemp_DS18B20(uint8_t DATA_PIN)
{
  int ack = DS18B20_RESET(DATA_PIN);
  if (!ack)
  {
    DS18B20_WRITE(DATA_PIN,0xcc);
    DS18B20_WRITE(DATA_PIN,0x44);
  }
  return !ack; 
}

int readTemp_DS18B20(uint8_t DATA_PIN, int & _Temp)
{
  uint8_t LSB, MSB;
  
  int ack = DS18B20_RESET(DATA_PIN);
  if (!ack)
  {
    DS18B20_WRITE(DATA_PIN,0xcc);
    DS18B20_WRITE(DATA_PIN,0xbe);
      
    LSB = DS18B20_READ(DATA_PIN);
    MSB = DS18B20_READ(DATA_PIN);
    
    _Temp = MSB; _Temp = (_Temp << 8) + LSB; 
  }
  return !ack; 
}

void dispTemp_DS18B20(int _Temp)
{
  uint8_t temp1 = _Temp >> 4; 
  uint8_t temp2 = _Temp & 0xf;
  temp2 = ((temp2 << 3) + (temp2 << 1)) >> 4;

  dispWidthValue(6,1,3,temp1);
  lcd.setCursor(10,1);
  lcd.print(temp2);         
}

void Operator_DS18B20(uint8_t DATA_PIN)
{
  if (!DS18B20_busy)
  {
    if (convTemp_DS18B20(DATA_PIN))
    {
      DS18B20_busy = true; 
      flush_time1 = millis();
    }
  }
  else
  {
    if ((millis() - flush_time1) > wait_750ms)
    {
      if (readTemp_DS18B20(DATA_PIN,Temp0625))
      {
		dispTemp_DS18B20(Temp0625);
	  }
      
	  if (IR_voice) voice_sound();
 	  
	  DS18B20_busy = false; 
    }
  }
}

void begin_DS18B20()
{
  DS18B20_busy = false;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("DS18B20 : ");
  
  lcd.setCursor(9,1);
  lcd.print('.');         
  dispTemp_DS18B20(Temp0625);
  dispTempSymbol(11,1,0);
}

/*******************************************************************************
  DHT11
*******************************************************************************/

void begin_DHT11()
{
  DHT11_READ(DHT11_PIN,DHT11_DATA);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("DHT11 : ");
  
  dispWidthValue(2,1,2,int(DHT11_DATA[0]));
  lcd.setCursor(4,1);
  lcd.print("% RH");

  dispWidthValue(10,1,2,int(DHT11_DATA[2]));
  dispTempSymbol(12,1,0);
}

void dispData_DHT11(uint8_t DATA_PIN, uint8_t * data)
{
  if ((millis() - flush_time2) > wait_750ms)
  {
    if (!DHT11_READ(DATA_PIN,DHT11_DATA))
    {
      flush_time2 = millis();
      
      dispWidthValue(2,1,2,int(data[0]));
      dispWidthValue(10,1,2,int(data[2]));

      if (IR_voice) voice_sound();
    }
  }  
}

uint8_t DHT11_READ_BYTE(uint8_t DATA_PIN)
{
  uint8_t value = 0;

  for (uint8_t mask = 0x80; mask; mask >>= 1)
  {
    while (!digitalRead(DATA_PIN));
    delayMicroseconds(35);
    
    if (digitalRead(DATA_PIN)) value |= mask;

    while (digitalRead(DATA_PIN));
  }

  return value;
}

int DHT11_READ(uint8_t DATA_PIN, uint8_t * data)
{
  pinMode(DATA_PIN,OUTPUT);
  digitalWrite(DATA_PIN,HIGH);
  
  digitalWrite(DATA_PIN,LOW);
  delay(20);
  digitalWrite(DATA_PIN,HIGH);
  delayMicroseconds(40);
  
  pinMode(DATA_PIN,INPUT);
  // digitalWrite(DATA_PIN,LOW);
 
  if (digitalRead(DATA_PIN)) return DHT11_ERROR_NOACK;

  while (!digitalRead(DATA_PIN));
  while (digitalRead(DATA_PIN));
      
  for (int i =  0; i < 5; i++)
  {
    data[i] = DHT11_READ_BYTE(DATA_PIN);
  }  
    
  uint8_t sum = data[0] + data[2];
  if (data[4] != sum) return DHT11_ERROR_CHECKSUM; 
  
  return DHT11_SUCCESS;  
}

/***************************************************************************************************/

void voice_sound()
{
  uint8_t width;

  SREG &= 0x7f; 

  if (LCD_PAGE == 0)
  {
	width = timeToVoiceMsg(voiceMsg,Hour,Minute,Second);
    playback(voiceMsg,width);
  }
  else
  if (LCD_PAGE == 1)
  {
    width = tempToVoiceMsg(voiceMsg,Temp0625);
    playback(voiceMsg,width);
  }
  else 
  if (LCD_PAGE == 2)
  {
    width = humiToVoiceMsg(voiceMsg,DHT11_DATA[0]);
    playback(voiceMsg,width);
  }
  else
  if (LCD_PAGE == 3)
  {
    width = presToVoiceMsg(voiceMsg,Pressure);
    playback(voiceMsg,width);
  }
  
  IR_voice = false;

  SREG |= 0x80; 
}
