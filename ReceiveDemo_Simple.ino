/*
  Simple example for receiving
  
  https://github.com/sui77/rc-switch/
*/

#include <RCSwitch.h>
#include <EEPROM.h>
RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  pinMode(13,OUTPUT);
    // write a 0 to all 512 bytes of the EEPROM
//  for (int i = 0; i < 512; i++)
//    EEPROM.write(i, 0);
if(EEPROM.read(99)==0) EEPROM.write(99,100);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
}

void loop() {
  if (mySwitch.available()) {
    int addr=EEPROM.read(99);
    int value = mySwitch.getReceivedValue();
    
    if (value == 0) {
      Serial.print("Unknown encoding");
    } else {
      int led=0;
      for(int i=0;i<10;i++){
      digitalWrite(13, led);
      led=1-led;
      delay(60);
      }
      unsigned long rv=mySwitch.getReceivedValue();
      int looptime=mySwitch.getReceivedBitlength()/8;
            Serial.print("BIN ");
      Serial.println( rv,BIN);
      for(int i=0;i<looptime;i++){
        int rvb = rv & 0xff;
        EEPROM.write(i+addr, rvb);
      Serial.print("EEPROM ");
      Serial.print( EEPROM.read(i+addr),BIN);
      rv=rv>>8;
      }
      
      unsigned long recre=0l;
      for(int i=0;i<looptime;i++){
      recre=EEPROM.read(addr-1+looptime-i)+recre<<8;
      
      }
       recre= recre>>8;
       EEPROM.write(99, addr+10);
     //recre =EEPROM.read(100)|EEPROM.read(101)<<8|EEPROM.read(102)<<16;
      Serial.print("Received2 ");
      Serial.println( recre);
      Serial.print("Received ");
      Serial.print( mySwitch.getReceivedValue() );
      Serial.print(" / ");
      Serial.print( mySwitch.getReceivedBitlength() );
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println( mySwitch.getReceivedProtocol() );
    }

    mySwitch.resetAvailable();
  }
}
