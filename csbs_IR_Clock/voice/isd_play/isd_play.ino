#include "voice60.h"

char voiceMsg[0x20]; uint8_t width;

/****************************************************************************************************/
void setup()
{
  ISD1760_SPI_INIT();
  delay(100); 
  
  volume(1);

  width = timeToVoiceMsg(voiceMsg,17,5,0);
  playback(voiceMsg,width);
  delay(1000);
  
  width = tempToVoiceMsg(voiceMsg,-604);
  playback(voiceMsg,width);
  delay(1000);
 
  width = humiToVoiceMsg(voiceMsg,65);
  playback(voiceMsg,width);
  delay(1000);

  width = presToVoiceMsg(voiceMsg,101300);
  playback(voiceMsg,width);
  delay(1000);
}

void loop()
{
}
