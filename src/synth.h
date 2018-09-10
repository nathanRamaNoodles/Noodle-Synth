//*************************************************************************************
//  Arduino synth V4.1
//  Optimized audio driver, modulation engine, envelope engine.
//
//  Dzl/Illutron 2014
//
//*************************************************************************************
#ifndef _SYNTH
#define _SYNTH
#if defined(__AVR__) || defined(__arm__) && defined(TEENSYDUINO)
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#include <i2s.h>
#include <i2s_reg.h>
#include <Ticker.h>
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

#define ENVELOPE0 0
#define ENVELOPE1 1
#define ENVELOPE2 2
#define ENVELOPE3 3

#if defined(ESP8266)
#define FS_music 1700000.0
#else
#define FS_music 20000.0
#endif

#define SET(x,y) (x |=(1<<y))		        		//-Bit set/clear macros
#define CLR(x,y) (x &= (~(1<<y)))       			// |
#define CHK(x,y) (x & (1<<y))           			// |
#define TOG(x,y) (x^=(1<<y))            			//-+
class synth
{
public:
  synth();
  void setNumVoices(uint8_t num);
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
  void mTrigger(unsigned char voice,unsigned char MIDInote);
  void trigger(unsigned char voice);
  void setSustain(unsigned char voice, int v);
private:
  //nothing :D
};
#endif
