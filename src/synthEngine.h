//*************************************************************************************
//  Arduino synth
//  Optimized audio driver, modulation engine, envelope engine.
//
//  Current Maintainer/Co-author: Nathan Ramanathan 2019  nathan6ramanathan@gmail.com
//  Original Retired Author:  Dzl/Illutron 2014
//
//*************************************************************************************
#ifndef synthEngine_h
#define synthEngine_h
#if defined(__AVR__) || defined(__arm__) && defined(TEENSYDUINO)
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#include <i2s.h>
#include <i2s_reg.h>
#include <Ticker.h>
#elif defined(ESP32)
#include "esp32-hal-timer.h"
#endif
#include "tables.h"


#define CHA 3
#define CHB 4

#define SUSTAIN 1
#define REV_SUSTAIN 2
#define NONE 0

#define SINE     0
#define TRIANGLE 1
#define SQUARE   2
#define SAW      3
#define RAMP     4
#define NOISE    5
#define PIANO    6

#define ENVELOPE0 0
#define ENVELOPE1 1
#define ENVELOPE2 2
#define ENVELOPE3 3


#define SET(x,y) (x |=(1<<y))		        		//-Bit set/clear macros
#define CLR(x,y) (x &= (~(1<<y)))       			// |
#define CHK(x,y) (x & (1<<y))           			// |
#define TOG(x,y) (x^=(1<<y))            			//-+
class synthEngine
{
public:
  synthEngine();
  synthEngine(float sampleRate);
  void begin(unsigned char voice);
  void begin(unsigned char voice, unsigned char d);
  void setupVoice(unsigned char voice, unsigned char wave, unsigned char pitch, unsigned char env, unsigned char length, unsigned int mod);
  void setWave(unsigned char voice, unsigned char wave);
  void setPitch(unsigned char voice,unsigned char MIDInote);
  void setEnvelope(unsigned char voice, unsigned char env);
  void setLength(unsigned char voice,unsigned char length);
  void setMod(unsigned char voice,unsigned char mod);
  void setVolume(unsigned char voice, int v);
  void setFrequency(unsigned char voice,float f);
  void setNote(unsigned char voice, int noteValue);
  void mTrigger(unsigned char voice,unsigned char MIDInote);
  void trigger(unsigned char voice);
  void setSustain(unsigned char voice, int v);
  void setChannel(unsigned char voice, int channel);
  void setPedalSustain(unsigned char voice, bool pedalPressed);
  static float getNoteAsFrequency(int n, int pitchBend);

  int getWave(unsigned char voice);
  int getAvailablePiston();
  int getVolume(unsigned char voice);
  int getNote(unsigned char voice);
  int getChannel(unsigned char voice);
  int getSustain(unsigned char voice);
  bool isPedalSustain(unsigned char voice);
private:
  static float FS_music;
};
#endif
