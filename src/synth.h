//*************************************************************************************
//  Arduino synth V4.1
//  Optimized audio driver, modulation engine, envelope engine.
//
//  Dzl/Illutron 2014
//
//*************************************************************************************
#ifndef _SYNTH
#define _SYNTH

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "tables.h"

#define DIFF 1
#define CHA 2
#define CHB 3

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

#define FS 20000.0                                              //-Sample rate (NOTE: must match tables.h)

#define SET(x,y) (x |=(1<<y))		        		//-Bit set/clear macros
#define CLR(x,y) (x &= (~(1<<y)))       			// |
#define CHK(x,y) (x & (1<<y))           			// |
#define TOG(x,y) (x^=(1<<y))            			//-+
class synth
{
  public:
    synth();
    void begin();
    void begin(unsigned char d);
    unsigned char synthTick(void);
    unsigned char voiceFree(unsigned char voice);
    void setupVoice(unsigned char voice, unsigned char wave, unsigned char pitch, unsigned char env, unsigned char length, unsigned int mod);
    void setWave(unsigned char voice, unsigned char wave);
    void setPitch(unsigned char voice,unsigned char MIDInote);
    void setEnvelope(unsigned char voice, unsigned char env);
    void setLength(unsigned char voice,unsigned char length);
    void setMod(unsigned char voice,unsigned char mod);
    void mTrigger(unsigned char voice,unsigned char MIDInote);
    void setFrequency(unsigned char voice,float f);
    void setTime(unsigned char voice,float t);
    void trigger(unsigned char voice);
    void suspend();
    void resume();
private:
  //nothing :D
};
#endif
