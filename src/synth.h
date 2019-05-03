//*************************************************************************************
//  Arduino synth V4.1
//  Optimized audio driver, modulation engine, envelope engine.
//
//  Dzl/Illutron 2014
//
//*************************************************************************************

#ifndef _SYNTH
#define _SYNTH



#if   defined(__AVR__) || defined(__arm__) && defined(TEENSYDUINO)
#  include <avr/pgmspace.h>
#  include <avr/interrupt.h>
#elif defined(ESP8266)
#  include <pgmspace.h>
#  include <i2s.h>
#  include <i2s_reg.h>
#  include <Ticker.h>
#endif

#include    "tables.h"



#define     CHA           3
#define     CHB           4

#define     NONE          0
#define     SUSTAIN       1
#define     REV_SUSTAIN   2

#define     SINE          0
#define     TRIANGLE      1
#define     SQUARE        2
#define     SAW           3
#define     RAMP          4
#define     NOISE         5

#define     ENVELOPE0     0
#define     ENVELOPE1     1
#define     ENVELOPE2     2
#define     ENVELOPE3     3

#if   defined(ESP8266)
//#  define   FS_music      1700000.0
#  define   FS_music      1700000UL
#else
//#  define   FS_music      20000.0
#  define   FS_music      20000UL
#endif

// macros
#define     SET(x,y) (x |=(1<<y))		        //-Bit set/clear macros
#define     CLR(x,y) (x &= (~(1<<y)))       // |
#define     CHK(x,y) (x & (1<<y))           // |
#define     TOG(x,y) (x^=(1<<y))            //-+



class synth
{
  public:
    synth();
    
    void    setNumVoices(uint8_t num);
    void    begin(uint8_t voice);
    void    begin(uint8_t voice, uint8_t d);
    void    setupVoice(uint8_t voice, uint8_t wave, uint8_t pitch, uint8_t env, uint8_t length, uint8_t mod);
    void    setWave(uint8_t voice, uint8_t wave);
    void    setPitch(uint8_t voice, uint8_t MIDInote);
    void    setEnvelope(uint8_t voice, uint8_t env);
    void    setLength(uint8_t voice, uint8_t noteLength);
    void    setMod(uint8_t voice, uint8_t mod);
    void    setVolume(uint8_t voice, uint8_t v);
    void    setFrequency(uint8_t voice, float f);
    void    mTrigger(uint8_t voice, uint8_t MIDInote);
    void    trigger(uint8_t voice);
    void    setSustain(uint8_t voice, uint8_t v);
    
  private:
    //int8_t  _getWaveValue(uint8_t voice, uint8_t value);
    //int8_t  _getEnvValue(int8_t value);
    //nothing :D
};

#endif


