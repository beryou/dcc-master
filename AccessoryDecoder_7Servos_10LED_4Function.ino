// Production 17 Function DCC Acessory Decoder Dual Address w/CV Access
// Version 4.0  Geoff Bunza 2014
// Uses modified software servo Lib
//
// ******** UNLESS YOU WANT ALL CV'S RESET UPON EVERY POWER UP
// ******** AFTER THE INITIAL DECODER LOAD REMOVE THE "//" IN THE FOOLOWING LINE!!
//#define DECODER_LOADED
  
#include <NmraDcc.h>
#include <SoftwareServo.h> 
#include <EEPROM.h>

SoftwareServo servo0;
SoftwareServo servo1;
SoftwareServo servo2;
SoftwareServo servo3;
SoftwareServo servo4;
SoftwareServo servo5;
SoftwareServo servo6;
SoftwareServo servo7;
SoftwareServo servo8;
SoftwareServo servo9;
SoftwareServo servo10;
SoftwareServo servo11;
SoftwareServo servo12;
SoftwareServo servo13;
SoftwareServo servo14;
SoftwareServo servo15;
SoftwareServo servo16;
#define servo_start_delay 50
#define servo_init_delay 7

#define	DATA_COMMAND	0X40
#define	DISP_COMMAND	0x80
#define	ADDR_COMMAND	0XC0

//TM1638模块引脚定义
int DIO =12;
int CLK =11;
int STB =10; //这里定义了那三个脚
//共阴数码管显示代码
unsigned char tab[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
                           0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x70,0x00};
unsigned char num[8];		//各个数码管显示的值 

int selectedId = 0;
int selectedValue = 0;
int requestrestart=0;
int debugon=0;
long lastDebounceTimeset = 0;
long lastDebounceTimeenter = 0;
int lastButtonStateset =0;
int lastButtonStateenter =0;
long lastLedTime =0;
int lastLedState =0;

unsigned char numi;
int setupmode=0;
int timer=0;
int dccaddrsetupok=0;
int romsaveaddr = 0;
int sensorValueenter=0;
int sensorValueset=0;

int tim_delay = 500;
int numfpins = 17;
byte fpins [] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
const int FunctionPin0 = 3;
const int FunctionPin1 = 4;
const int FunctionPin2 = 5;
const int FunctionPin3 = 6;
const int FunctionPin4 = 7;
const int FunctionPin5 = 8;
const int FunctionPin6 = 9;
const int FunctionPin7 = 10;
const int FunctionPin8 = 11;
const int FunctionPin9 = 12;
const int FunctionPin10 = 13;
const int FunctionPin11 = 14;     //A0
const int FunctionPin12 = 15;     //A1
const int FunctionPin13 = 16;     //A2
const int FunctionPin14 = 17;     //A3
const int FunctionPin15 = 18;     //A4
const int FunctionPin16 = 19;     //A5
NmraDcc  Dcc ;
DCC_MSG  Packet ;

int t;                                    // temp
#define SET_CV_Address       24           // THIS ADDRESS IS FOR SETTING CV'S Like a Loco
#define Accessory_Address    40           // THIS ADDRESS IS THE START OF THE SWITCHES RANGE
                                          // WHICH WILL EXTEND FOR 16 MORE SWITCH ADDRESSES
uint8_t CV_DECODER_MASTER_RESET =   120;  // THIS IS THE CV ADDRESS OF THE FULL RESET
#define CV_To_Store_SET_CV_Address	121
#define CV_Accessory_Address CV_ACCESSORY_DECODER_ADDRESS_LSB
						  
struct QUEUE
{
  int inuse;
  int current_position;
  int increment;
  int stop_value;
  int start_value;
};
QUEUE *ftn_queue = new QUEUE[16];

struct CVPair
{
  uint16_t  CV;
  uint8_t   Value;
};
CVPair FactoryDefaultCVs [] =
{
  {CV_ACCESSORY_DECODER_ADDRESS_LSB, Accessory_Address},
  {CV_ACCESSORY_DECODER_ADDRESS_MSB, 0},
  {CV_MULTIFUNCTION_EXTENDED_ADDRESS_MSB, 0},
  {CV_MULTIFUNCTION_EXTENDED_ADDRESS_LSB, 0},
  {CV_DECODER_MASTER_RESET, 0},
  {CV_To_Store_SET_CV_Address, SET_CV_Address},
  {CV_To_Store_SET_CV_Address+1, 0},
  {30, 2}, //F0 Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
  {31, 1},    //F0 Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {32, 28},   //F0  Start Position F0=0
  {33, 140},  //F0  End Position   F0=1
  {34, 28},   //F0  Current Position
  {35, 2},  //F1 Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
  {36, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {37, 28},   //  Start Position Fx=0
  {38, 140},  //  End Position   Fx=1
  {39, 28},  //  Current Position
  {40, 2},  //F2 Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
  {41, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {42, 28},   //  Start Position Fx=0
  {43, 140},  //  End Position   Fx=1
  {44, 28},    //  Current Position
  {45, 2}, //F3 Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
  {46, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {47, 28},   //  Start Position Fx=0
  {48, 140},  //  End Position   Fx=1
  {49, 28},    //  Current Position
  {50, 2}, //F4 Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
  {51, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {52, 28},    //  Start Position Fx=0
  {53, 140},    //  End Position   Fx=1
  {54, 28},    //  Current Position
  {55, 0}, //F5 Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
  {56, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {57, 28},    //  Start Position Fx=0
  {58, 140},    //  End Position   Fx=1
  {59, 28},    //  Current Position
  {60, 0}, //F6 Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
  {61, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {62, 28},    //  Start Position Fx=0
  {63, 140},    //  End Position   Fx=1
  {64, 28},    //  Current Position
  {65, 0}, //F7 Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
  {66, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {67, 28},   //  Start Position Fx=0
  {68, 140},  //  End Position   Fx=1
  {69, 28},    //  Current Position
  {70, 0}, //F8 Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
  {71, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {72, 28},   //  Start Position Fx=0
  {73, 140},  //  End Position   Fx=1
  {74, 28},    //  Current Position
  {75, 0}, //F9 Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
  {76, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {77, 28},   //  Start Position Fx=0
  {78, 140},  //  End Position   Fx=1
  {79, 28},    //  Current Position
  {80, 0}, //F10 Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
  {81, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {82, 28},   //  Start Position Fx=0
  {83, 140},  //  End Position   Fx=1
  {84, 28},    //  Current Position
  {85, 0}, //F11 Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
  {86, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {87, 28},   //  Start Position Fx=0
  {88, 140},  //  End Position   Fx=1
  {89, 28},    //  Current Position
  {90, 0}, //F12 Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
  {91, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {92, 28},   //  Start Position Fx=0
  {93, 140},  //  End Position   Fx=1
  {94, 28},    //  Current Position
  {95, 0}, //F13 Config  0=On/Off,1=Blink,2=Servo,3=PWM
  {96, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {97, 0},   //  Start Position Fx=0
  {98, 20},  //  End Position   Fx=1
  {99, 1},    //  Current Position
  {100, 0}, //F14 Config  0=On/Off,1=Blink,2=Servo,3=PWM
  {101, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {102, 1},   //  Start Position Fx=0
  {103, 4},  //  End Position   Fx=1
  {104, 1},    //  Current Position
  {105, 0}, //F15 Config  0=On/Off,1=Blink,2=Servo,3=PWM
  {106, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {107, 1},   //  Start Position Fx=0
  {108, 60},  //  End Position   Fx=1
  {109, 20},    //  Current Position
  {110, 0}, //F16 Config  0=On/Off,1=Blink,2=Servo,3=PWM
  {111, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {112, 1},   //  Start Position Fx=0
  {113, 4},  //  End Position   Fx=1
  {114, 1},    //  Current Position
//FUTURE USE
  {115, 0}, //F17 Config  0=On/Off,1=Blink,2=Servo,3=PWM
  {116, 1},    // Rate  Blink=Eate,PWM=Rate,Servo=Rate
  {117, 28},   //  Start Position Fx=0
  {118, 50},  //  End Position   Fx=1
  {119, 28},    //  Current Position
};
uint8_t FactoryDefaultCVIndex = sizeof(FactoryDefaultCVs)/sizeof(CVPair);

void setup()   //******************************************************
{
  int i;
  uint8_t cv_value;
  if(debugon==1) Serial.begin(9600);
  // initialize the digital pins as outputs
    for (int i=0; i < numfpins; i++) {
      pinMode(fpins[i], OUTPUT);
      digitalWrite(fpins[i], 0);
     }
  for (int i=0; i < numfpins; i++) {
     digitalWrite(fpins[i], 1);
     delay (tim_delay/10);
  }
  delay( tim_delay);
  for (int i=0; i < numfpins; i++) {
     digitalWrite(fpins[i], 0);
     delay (tim_delay/10);
  }
  delay( tim_delay);
  
  // Setup which External Interrupt, the Pin it's associated with that we're using 
  Dcc.pin(0, 2, 0);
  // Call the main DCC Init function to enable the DCC Receiver
  Dcc.init( MAN_ID_DIY, 100, FLAGS_OUTPUT_ADDRESS_MODE | FLAGS_DCC_ACCESSORY_DECODER, CV_To_Store_SET_CV_Address);
  delay(800);
   
  #if defined(DECODER_LOADED)
  if ( Dcc.getCV(CV_DECODER_MASTER_RESET)== CV_DECODER_MASTER_RESET ) 
  #endif  
  
     {
       for (int j=0; j < sizeof(FactoryDefaultCVs)/sizeof(CVPair); j++ ){
         if(debugon==1) Serial.print(" IO: ");
   if(debugon==1) Serial.println(j, DEC) ;
    if(debugon==1) Serial.print(" ORACV: ");
    if(debugon==1) Serial.println(FactoryDefaultCVs[j].CV, DEC) ;
    if(debugon==1) Serial.print(" ORAVAL: ");
   if(debugon==1) Serial.println(FactoryDefaultCVs[j].Value, DEC) ;
       if(EEPROM.read(romsaveaddr)==0){
          if(debugon==1) Serial.print(" FactoryDefaultCVs: ");
        if(debugon==1) Serial.println(FactoryDefaultCVs[j].Value, DEC) ;
         Dcc.setCV( FactoryDefaultCVs[j].CV, FactoryDefaultCVs[j].Value);
         EEPROM.write(FactoryDefaultCVs[j].CV, FactoryDefaultCVs[j].Value);
     }else{
        if(debugon==1) Serial.print(" EEPROM: ");
    if(debugon==1) Serial.println(EEPROM.read(FactoryDefaultCVs[j].CV), DEC) ;
       Dcc.setCV(FactoryDefaultCVs[j].CV,EEPROM.read(FactoryDefaultCVs[j].CV));
     }
       }
         EEPROM.write(romsaveaddr, 1);
         digitalWrite(fpins[14], 1);
         delay (1000);
         digitalWrite(fpins[14], 0);
     }
  for ( i=0; i < numfpins; i++) {
    cv_value = Dcc.getCV( 30+(i*5)) ;   
    if(debugon==1) Serial.print(" cv_value: ");
   if(debugon==1) Serial.println(cv_value, DEC) ;
    switch ( cv_value ) {
      case 0:   // LED on/off
        ftn_queue[i].inuse = 0;
        break;
      case 1:   // LED Blink
         {
           ftn_queue[i].inuse = 0;
		   ftn_queue[i].current_position = 0;
           ftn_queue[i].start_value = 0;
           ftn_queue[i].increment = int (char (Dcc.getCV( 31+(i*5))));
           digitalWrite(fpins[i], 0);
           ftn_queue[i].stop_value = int(Dcc.getCV( 33+(i*5))) ;
         }
        break;
      case 2:   //servo
       { ftn_queue[i].current_position =int (Dcc.getCV( 34+(i*5)));
         ftn_queue[i].stop_value = int (Dcc.getCV( 33+(i*5)));
         ftn_queue[i].start_value = int (Dcc.getCV( 32+(i*5)));
         ftn_queue[i].increment = -int (char (Dcc.getCV( 31+(i*5)))); 
         switch ( i ) {
         case 0: servo0.attach(FunctionPin0);  // attaches servo on pin to the servo object 
           ftn_queue[i].inuse = 1;
           servo0.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 1:  servo1.attach(FunctionPin1);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo1.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 2: servo2.attach(FunctionPin2);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo2.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 3: servo3.attach(FunctionPin3);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo3.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 4: servo4.attach(FunctionPin4);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo4.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 5: servo5.attach(FunctionPin5);  // attaches servo on pin to the servo object  
           ftn_queue[i].inuse = 1;
           servo5.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 6: servo6.attach(FunctionPin6);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo6.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 7: servo7.attach(FunctionPin7);  // attaches servo on pin to the servo object  
           ftn_queue[i].inuse = 1;
           servo7.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 8: servo8.attach(FunctionPin8);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo8.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 9: servo9.attach(FunctionPin9);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo9.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 10: servo10.attach(FunctionPin10);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo10.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 11: servo11.attach(FunctionPin11);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo11.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 12: servo12.attach(FunctionPin12);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo12.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 13: servo13.attach(FunctionPin13);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo13.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 14: servo14.attach(FunctionPin14);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo14.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 15: servo15.attach(FunctionPin15);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo15.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         case 16: servo16.attach(FunctionPin16);  // attaches servo on pin to the servo object
           ftn_queue[i].inuse = 1;
           servo16.write(ftn_queue[i].start_value);
           for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
           break;
         default:
           break;
          }
        }
        break;
       case 3:   // DOUBLE ALTERNATING LED Blink
         {
           ftn_queue[i].inuse = 0;
		   ftn_queue[i].current_position = 0;
           ftn_queue[i].start_value = 0;
           ftn_queue[i].increment = Dcc.getCV( 31+(i*5));
           digitalWrite(fpins[i], 0);
           digitalWrite(fpins[i+1], 0);
           ftn_queue[i].stop_value = int(Dcc.getCV( 33+(i*5)));
         }
         break;
       case 4:   // NEXT FEATURE to pin
         break;         
       default:
         break;
    }
  }

}
void( *resetFunc) (void) = 0;

void loop()   //**********************************************************************
{
  //MUST call the NmraDcc.process() method frequently 
  // from the Arduino loop() function for correct library operation

  //setup ok and reboot
  if(requestrestart==1){
    analogWrite(A5, 0);
    resetFunc();//reboot
  }
  if(setupmode>0){
    //setup mode led
    timer=timer+1;
      if(timer/10%2 ==0){
          analogWrite(A5, 255);
          
      }
      else{
          analogWrite(A5, 0);
      }
      //timeout and exit setupmode
      if(timer>300){
        analogWrite(A5, 0);
        setupfailled();
        setupmode=0;
        if(dccaddrsetupok ==1){
              dccaddrsetupok=0;
              requestrestart=1;
            }
      }
        //sensorValueenter = analogRead(A7);//enter next setup
        sensorValueenter =Read_key();
        if (sensorValueenter != lastButtonStateenter) {        
              lastDebounceTimeenter = millis(); 
        } 
        //enter next setup
        //if(sensorValueenter==1023 && (millis() - lastDebounceTimeenter) > 50){
          if(sensorValueenter==6 && (millis() - lastDebounceTimeenter) > 50){
          sensorValueenter =0;  
          setupmode++;  
            setupsuccessled();
            if(setupmode>4) {
              setupmode=0;
              //resetFunc();
                    for(int knumi=0;knumi<8;knumi++)
	    Write_DATA(knumi<<1,tab[16]);
              if(dccaddrsetupok ==1){
                dccaddrsetupok=0;
                requestrestart=1;
              }
              
            }
        }
        lastButtonStateenter = sensorValueenter;
  numi=Read_key();                          //读按键值
        if (numi != lastLedState) {        
              lastLedTime = millis(); 
        } 
        if(millis() - lastLedTime > 50){
              //ID select
		  Write_DATA(3*2,tab[selectedId]);
		  Write_allLED(1<<3);
		if(numi==1)
		{
                  timer=0;
                  selectedId++;
                  if(selectedId>5)selectedId=0;
		  Write_DATA(3*2,tab[selectedId]);
		  Write_allLED(1<<3);
                  delay(400);
		} else if(numi==0){
                    timer=0;
                    selectedId--;
                    if(selectedId<0)selectedId=4;
		  Write_DATA(3*2,tab[selectedId]);
		  Write_allLED(1<<3);
                delay(400);
                  }
                  
        }
        lastLedState=numi;
    
  }
  Dcc.process();
  SoftwareServo::refresh();
  delay(8);
  switch ( setupmode ) {
    //normal mode
      case 0:  
      //add start by go
        sensorValueenter = analogRead(A7);//enter setup
        if (sensorValueenter != lastButtonStateenter) {        
              lastDebounceTimeenter = millis(); 
        } 
        sensorValueset = Read_key();//reset/set
        if (sensorValueset != lastButtonStateset) {        
              lastDebounceTimeset = millis(); 
        } 
        if(sensorValueenter==1023 && (millis() - lastDebounceTimeenter) > 1500){
          sensorValueenter =0;
          setupmode=1;//dcc setup mode
          dccaddrsetupok==0;
          analogWrite(A5, 255);
          delay(2000);
            init_TM1638();	                           //初始化TM1638
	  for(numi=0;numi<8;numi++)
	    Write_DATA(numi<<1,tab[16]);
          
        }
        if(sensorValueset==7 && (millis() - lastDebounceTimeset) > 3000){
          sensorValueset=0;
          for (int i = 0; i < 512; i++)
              EEPROM.write(i, 0);
            resetFunc();//reboot
        }
        lastButtonStateenter = sensorValueenter;
        lastButtonStateset = sensorValueset;
      //add end by go
  
  for (int i=0; i < numfpins; i++) {
    if (ftn_queue[i].inuse==1)  {
    ftn_queue[i].current_position = ftn_queue[i].current_position + ftn_queue[i].increment;
    switch (Dcc.getCV( 30+(i*5))) {
      case 0:
        break;
      case 1:
        if (ftn_queue[i].current_position > ftn_queue[i].stop_value) {
          ftn_queue[i].start_value = ~ftn_queue[i].start_value;
          digitalWrite(fpins[i], ftn_queue[i].start_value);
          ftn_queue[i].current_position = 0;
          ftn_queue[i].stop_value = int(Dcc.getCV( 33+(i*5)));
        }
        break;
      case 2:
        {
        if (ftn_queue[i].increment > 0) {
          if (ftn_queue[i].current_position > ftn_queue[i].stop_value) 
            ftn_queue[i].current_position = ftn_queue[i].stop_value;
        } 
        if (ftn_queue[i].increment < 0) { 
          if (ftn_queue[i].current_position < ftn_queue[i].start_value) 
            ftn_queue[i].current_position = ftn_queue[i].start_value;
        }
        set_servo(i, ftn_queue[i].current_position);
        }
        break;
      case 3:
        if (ftn_queue[i].current_position > ftn_queue[i].stop_value) {
          ftn_queue[i].start_value = ~ftn_queue[i].start_value;
          digitalWrite(fpins[i], ftn_queue[i].start_value);
          digitalWrite(fpins[i]+1, ~ftn_queue[i].start_value);
          ftn_queue[i].current_position = 0;
          ftn_queue[i].stop_value = int(Dcc.getCV( 33+(i*5)));
        }
        i++;
        break;
      case 4:  //FUTURE FUNCTION
          break;
        default:
        break;  
      }
    }
  }
  break;
      //DCC Addr Setupmode
      case 1:
      //call dcc
      //led dcc
      for(int knumi=0;knumi<4;knumi++)
	    Write_DATA(knumi<<1,tab[16]);
       Write_DATA(0*2,tab[13]);
		  Write_allLED(1<<0);
Write_DATA(1*2,tab[12]);
		  Write_allLED(1<<1);
Write_DATA(2*2,tab[12]);
		  Write_allLED(1<<2);
      
      break;
      //servos start config
      case 2:
            //led aa
                  for(int knumi=0;knumi<4;knumi++)
	    Write_DATA(knumi<<1,tab[16]);
       Write_DATA(0*2,tab[10]);
		  Write_allLED(1<<0);
Write_DATA(1*2,tab[10]);
		  Write_allLED(1<<1);
      ftn_queue[selectedId].start_value = int(Dcc.getCV( 32+(selectedId*5)));
      dispvalueled(ftn_queue[selectedId].start_value);
      set_servo(selectedId,ftn_queue[selectedId].start_value);
      //value chang
      if(millis() - lastLedTime > 100){
      if(numi==3)
		{
  timer=0;
        ftn_queue[selectedId].start_value = ftn_queue[selectedId].start_value + 1;
       if(ftn_queue[selectedId].start_value>140)ftn_queue[selectedId].start_value=28;
		} else if(numi==2){
  timer=0;
        ftn_queue[selectedId].start_value = ftn_queue[selectedId].start_value - 1;
        if(ftn_queue[selectedId].start_value<28)ftn_queue[selectedId].start_value=140;
}
       Dcc.setCV(32+(selectedId*5),ftn_queue[selectedId].start_value);
		  dispvalueled(ftn_queue[selectedId].start_value);
      set_servo(selectedId,ftn_queue[selectedId].start_value);
      }
           //for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
      
      break;
      //servos end config
      case 3:
            for(int knumi=0;knumi<4;knumi++)
	    Write_DATA(knumi<<1,tab[16]);
                  //led bb
       Write_DATA(0*2,tab[11]);
		  Write_allLED(1<<0);
Write_DATA(1*2,tab[11]);
		  Write_allLED(1<<1);
      ftn_queue[selectedId].stop_value = int(Dcc.getCV( 33+(selectedId*5)));
      dispvalueled(ftn_queue[selectedId].stop_value);
      set_servo(selectedId,ftn_queue[selectedId].stop_value);
      //value chang
      if(millis() - lastLedTime > 100){
      if(numi==3)
		{
  timer=0;
        ftn_queue[selectedId].stop_value = ftn_queue[selectedId].stop_value + 1;
        if(ftn_queue[selectedId].stop_value>140)ftn_queue[selectedId].stop_value=28;
		} else if(numi==2){
  timer=0;
        ftn_queue[selectedId].stop_value = ftn_queue[selectedId].stop_value - 1;
        if(ftn_queue[selectedId].stop_value<28)ftn_queue[selectedId].stop_value=140;
}
Dcc.setCV(33+(selectedId*5),ftn_queue[selectedId].stop_value);
		  dispvalueled(ftn_queue[selectedId].stop_value);
      set_servo(selectedId,ftn_queue[selectedId].stop_value);
      }
           //for (t=0; t<servo_start_delay; t++) {SoftwareServo::refresh();delay(servo_init_delay);}
      break;
      //servos speed config
      case 4:
            for(int knumi=0;knumi<4;knumi++)
	    Write_DATA(knumi<<1,tab[16]);
                  //led cc
       Write_DATA(0*2,tab[12]);
		  Write_allLED(1<<0);
Write_DATA(1*2,tab[12]);
		  Write_allLED(1<<1);


      ftn_queue[selectedId].increment = int(Dcc.getCV( 31+(selectedId*5)));
      dispvalueled(ftn_queue[selectedId].increment);
      //value chang
      if(millis() - lastLedTime > 100){
      if(numi==3)
		{
  timer=0;
        ftn_queue[selectedId].increment = ftn_queue[selectedId].increment + 1;
        if(ftn_queue[selectedId].increment>50)ftn_queue[selectedId].increment=1;

		} else if(numi==2){
  timer=0;
        ftn_queue[selectedId].increment = ftn_queue[selectedId].increment - 1;
        if(ftn_queue[selectedId].increment<1)ftn_queue[selectedId].increment=50;
}
        Dcc.setCV(31+(selectedId*5),ftn_queue[selectedId].increment);
		  dispvalueled(ftn_queue[selectedId].increment);
      }
      
      
      break;
default:
         break;
      
  }
  
  
}

extern void notifyDccAccState( uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, uint8_t State) {
  uint16_t Current_Decoder_Addr;
  uint8_t Bit_State;
  Current_Decoder_Addr = Dcc.getAddr();
  Bit_State = OutputAddr & 0x01;
    switch ( setupmode ) {
      case 0: 
  if ( Addr >= Current_Decoder_Addr || Addr < Current_Decoder_Addr+5) { //Controls Accessory_Address+16
    switch (Addr-Current_Decoder_Addr) {
         case 0: 
         exec_function( 5, FunctionPin5, Bit_State );
         exec_function( 6, FunctionPin6, 1-Bit_State );
         exec_function( 0, FunctionPin0, Bit_State );

           break;
         case 1: 
                  exec_function( 7, FunctionPin6, Bit_State );
         exec_function( 8, FunctionPin6, 1-Bit_State );
         exec_function( 1, FunctionPin1, Bit_State );

           break;
         case 2: 
                  exec_function( 9, FunctionPin6, Bit_State );
         exec_function( 10, FunctionPin6, 1-Bit_State );
         exec_function( 2, FunctionPin2, Bit_State );

           break;
         case 3: 
                  exec_function( 11, FunctionPin6, Bit_State );
         exec_function( 12, FunctionPin6, 1-Bit_State );
         exec_function( 3, FunctionPin3, Bit_State );

           break;
         case 4: 
                  exec_function( 13, FunctionPin6, Bit_State );
         exec_function( 14, FunctionPin6, 1-Bit_State );
         exec_function( 4, FunctionPin4, Bit_State );

           break;
//         case 5: exec_function( 5, FunctionPin5, Bit_State );
//           break;
//         case 6: exec_function( 6, FunctionPin6, Bit_State );
//           break;
//         case 7: exec_function( 7, FunctionPin7, Bit_State );  
//           break;
//         case 8: exec_function( 8, FunctionPin8, Bit_State );
//           break;
//         case 9: exec_function( 9, FunctionPin9, Bit_State );
//           break;
//         case 10: exec_function( 10, FunctionPin10, Bit_State );
//           break;
//         case 11: exec_function( 11, FunctionPin11, Bit_State );
//           break;
//         case 12: exec_function( 12, FunctionPin12, Bit_State );
//           break;
//         case 13: exec_function( 13, FunctionPin13, Bit_State );
//           break;
//         case 14: exec_function( 14, FunctionPin14, Bit_State );
//           break;
         case 15: exec_function( 15, FunctionPin15, Bit_State );
           break;
         case 16: exec_function( 16, FunctionPin16, Bit_State );
           break;
         default:
           break;
    }
  } 
  break;
  case 1:
  //add start by go
  if(Addr>20){
      Dcc.setCV( CV_ACCESSORY_DECODER_ADDRESS_LSB, Addr);
      EEPROM.write(CV_ACCESSORY_DECODER_ADDRESS_LSB, Addr);
        
        setupmode=2;//next mode
        dccaddrsetupok=1;
        setupsuccessled();
  }else{
    setupmode=0;//next mode
        
        setupfailled();
  }
  break;
  default:
         break;
    }
}

void setupsuccessled(){
      for (int i=0; i < 10; i++) {
        analogWrite(A5, 255);
        delay(80);
        analogWrite(A5, 0);
        delay(80);
      }
      timer=0;
      
}
void setupfailled(){
      for (int i=0; i < 4; i++) {
        analogWrite(A5, 255);
        delay(800);
        analogWrite(A5, 0);
        delay(300);
      }
       timer=0;
}


void exec_function (int function, int pin, int FuncState)  {
  switch ( Dcc.getCV( 30+(function*5)) )  {  // Config  0=On/Off,1=Blink,2=Servo,3=Double LED Blink
    case 0:    // On - Off LED
      digitalWrite (pin, FuncState);
      ftn_queue[function].inuse = 0;
      break;
    case 1:    // Blinking LED
      if ((ftn_queue[function].inuse==0) && (FuncState==1))  {
        ftn_queue[function].inuse = 1;
        ftn_queue[function].start_value = 0;
        digitalWrite(pin, 0);
        ftn_queue[function].stop_value = int(Dcc.getCV( 33+(function*5)));
      } else {
          if ((ftn_queue[function].inuse==1) && (FuncState==0)) {
            ftn_queue[function].inuse = 0;
            digitalWrite(pin, 0);
          }
        }
      break;
    case 2:    // Servo
      ftn_queue[function].inuse = 1;
      if (FuncState==1) ftn_queue[function].increment = char ( Dcc.getCV( 31+(function*5)));
        else ftn_queue[function].increment = - char(Dcc.getCV( 31+(function*5)));
      if (FuncState==1) ftn_queue[function].stop_value = Dcc.getCV( 33+(function*5));
        else ftn_queue[function].stop_value = Dcc.getCV( 32+(function*5));
      break;
    case 3:    // Blinking LED PAIR
      if ((ftn_queue[function].inuse==0) && (FuncState==1))  {
        ftn_queue[function].inuse = 1;
        ftn_queue[function].start_value = 0;
        digitalWrite(fpins[function], 0);
        digitalWrite(fpins[function+1], 1);
        ftn_queue[function].stop_value = int(Dcc.getCV( 33+(function*5)));
      } else {
          if (FuncState==0) {
            ftn_queue[function].inuse = 0;
            digitalWrite(fpins[function], 0);
            digitalWrite(fpins[function+1], 0);
          }
        }
      break;
    case 4:    // Future Function
      ftn_queue[function].inuse = 0;
      break;
    default:
      ftn_queue[function].inuse = 0;
      break;
  }
}
void set_servo (int servo_num, int servo_pos)  {
    switch (servo_num) {
         case 0: servo0.write(servo_pos);
           break;
         case 1: servo1.write(servo_pos);
           break;
         case 2: servo2.write(servo_pos);
           break;
         case 3: servo3.write(servo_pos);
           break;
         case 4: servo4.write(servo_pos);
           break;
         case 5: servo5.write(servo_pos);
           break;
         case 6: servo6.write(servo_pos);
           break;
         case 7: servo7.write(servo_pos);  
           break;
         case 8: servo8.write(servo_pos);
           break;
         case 9: servo9.write(servo_pos);
           break;
         case 10: servo10.write(servo_pos);
           break;
         case 11: servo11.write(servo_pos);
           break;
         case 12: servo12.write(servo_pos);
           break;
         case 13: servo13.write(servo_pos);
           break;
         case 14: servo14.write(servo_pos);
           break;
         case 15: servo15.write(servo_pos);
           break;
         case 16: servo16.write(servo_pos);
           break;
         default:
           break;
    }
}

void TM1638_Write(unsigned char	DATA)			//写数据函数
{
	unsigned char i;
        pinMode(DIO,OUTPUT);
	for(i=0;i<8;i++)
	{
            digitalWrite(CLK,LOW);
	    if(DATA&0X01)
               digitalWrite(DIO,HIGH);
	    else
               digitalWrite(DIO,LOW);
	    DATA>>=1;
            digitalWrite(CLK,HIGH);
	}
}
unsigned char TM1638_Read(void)					//读数据函数
{
	unsigned char i;
	unsigned char temp=0;;	
        pinMode(DIO,INPUT);//设置为输入
	for(i=0;i<8;i++)
	{
  	    temp>>=1;
            digitalWrite(CLK,LOW);
  	    if(digitalRead(DIO)==HIGH)
  	      temp|=0x80;
            digitalWrite(CLK,HIGH);
               
	}
	return temp;
}

void dispvalueled(unsigned char value)
{
  for(int knumi=4;knumi<8;knumi++)
	    Write_DATA(knumi<<1,tab[16]);
         Write_DATA(5*2,tab[value/100]);
		  Write_allLED(1<<5);
          Write_DATA(6*2,tab[value/10%10]);
		  Write_allLED(1<<6);
          Write_DATA(7*2,tab[value%10]);
		  Write_allLED(1<<7);
  
}

void Write_COM(unsigned char cmd)		//发送命令字
{
        digitalWrite(STB,LOW);
	TM1638_Write(cmd);
        digitalWrite(STB,HIGH);
}
unsigned char Read_key(void)
{
	unsigned char c[4],i,key_value=0;
        digitalWrite(STB,LOW);
	TM1638_Write(0x42);		           //读键扫数据 命令
	for(i=0;i<4;i++)		
  	{
            c[i]=TM1638_Read();
        }
        digitalWrite(STB,HIGH);					           //4个字节数据合成一个字节
	for(i=0;i<4;i++)
	{
            key_value|=c[i]<<i;
        }	
        for(i=0;i<8;i++)
        {
            if((0x01<<i)==key_value)
            break;
        }
	return i;
}
void Write_DATA(unsigned char add,unsigned char DATA)		//指定地址写入数据
{
	Write_COM(0x44);
        digitalWrite(STB,LOW);
	TM1638_Write(0xc0|add);
	TM1638_Write(DATA);
        digitalWrite(STB,HIGH);
}
void Write_allLED(unsigned char LED_flag)					//控制全部LED函数，LED_flag表示各个LED状态
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
	    if(LED_flag&(1<<i))
	      Write_DATA(2*i+1,1);
	    else
	      Write_DATA(2*i+1,0);
        }
}

//TM1638初始化函数
void init_TM1638(void)
{
	unsigned char i;
	Write_COM(0x8b);       //亮度 (0x88-0x8f)8级亮度可调
	Write_COM(0x40);       //采用地址自动加1
        digitalWrite(STB,LOW);		           //
	TM1638_Write(0xc0);    //设置起始地址
	for(i=0;i<16;i++)	   //传送16个字节的数据
	  TM1638_Write(0x00);
        digitalWrite(STB,HIGH);
}
