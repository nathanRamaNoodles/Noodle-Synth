#include "Arduino.h"
#include "synth.h"
static unsigned int PCW[4] = {0, 0, 0, 0};			//-Wave phase accumolators
static unsigned int FTW[4] = {1000, 200, 300, 400};           //-Wave frequency tuning words
static unsigned char AMP[4] = {255, 255, 255, 255};           //-Wave amplitudes [0-255]
static unsigned int PITCH[4] = {500, 500, 500, 500};          //-Voice pitch
static int MOD[4] = {20, 0, 64, 127};                         //-Voice envelope modulation [0-1023 512=no mod. <512 pitch down >512 pitch up]
static unsigned int wavs[4];                                  //-Wave table selector [address of wave in memory]
static unsigned int envs[4];                                  //-Envelopte selector [address of envelope in memory]
static unsigned int EPCW[4] = {0x8000, 0x8000, 0x8000, 0x8000}; //-Envelope phase accumolator
static unsigned int EFTW[4] = {10, 10, 10, 10};               //-Envelope speed tuning word
static unsigned char divider = 4;                             //-Sample rate decimator for envelope
static unsigned int tim = 0;
static unsigned char tik = 0;
static unsigned char output_mode;
static unsigned int sustainIt[4] = {1, 1, 1, 1};//sustain variables
static unsigned int revSustain[4] = {1, 1, 1, 1};//sustain variables
static unsigned int volume[4] = {20, 20, 20, 20};//volume variables

#if defined(__arm__) && defined(TEENSYDUINO)
static void timerInterrupt();
static IntervalTimer sampleTimer;
static uint8_t globalPWM=3; // default to PIN 3
#endif

synth::synth(){}
void synth:: begin()
{
  output_mode=CHA;
  #if defined(__AVR__)
  cli();
  TCCR1A = 0x00;                                  //-Start audio interrupt
  TCCR1B = 0x09;
  TCCR1C = 0x00;
  OCR1A=16000000.0 / FS;			    //-Auto sample rate
  SET(TIMSK1, OCIE1A);                            //-Start audio interrupt
  sei();                                          //-+

  TCCR2A = 0x83;                                  //-8 bit audio PWM
  TCCR2B = 0x01;                                  // |
  OCR2A = 127;                                    //-+
  SET(DDRB, 3);				    //-PWM pin
  #elif defined(__arm__) && defined(TEENSYDUINO)
  sampleTimer.begin(timerInterrupt, 1000000.0 / FS);
  analogWriteFrequency(globalPWM, FS);
  #endif
}

//*********************************************************************
//  Startup fancy selecting varoius output modes
//*********************************************************************

void synth::begin(unsigned char d)
{
  #if defined(__AVR__)
  cli();
  TCCR1A = 0x00;                                  //-Start audio interrupt
  TCCR1B = 0x09;
  TCCR1C = 0x00;
  OCR1A=16000000.0 / FS;			    //-Auto sample rate
  SET(TIMSK1, OCIE1A);                            //-Start audio interrupt
  sei();                                          //-+

  output_mode=d;

  switch(d)
  {
    case DIFF:                                        //-Differntial signal on CHA and CHB pins (11,3)
    TCCR2A = 0xB3;                                  //-8 bit audio PWM
    TCCR2B = 0x01;                                  // |
    OCR2A = OCR2B = 127;                            //-+
    SET(DDRB, 3);				      //-PWM pin
    SET(DDRD, 3);				      //-PWM pin
    break;

    case CHB:                                         //-Single ended signal on CHB pin (3)
    TCCR2A = 0x23;                                  //-8 bit audio PWM
    TCCR2B = 0x01;                                  // |
    OCR2A = OCR2B = 127;                            //-+
    SET(DDRD, 3);				      //-PWM pin
    break;

    case CHA:
    default:
    output_mode=CHA;                                //-Single ended signal in CHA pin (11)
    TCCR2A = 0x83;                                  //-8 bit audio PWM
    TCCR2B = 0x01;                                  // |
    OCR2A = OCR2B = 127;                            //-+
    SET(DDRB, 3);				      //-PWM pin
    break;

  }
  #else
  globalPWM=(uint8_t)d;
  begin(); // TODO: other modes on non-AVR chips...
  #endif
}
//*********************************************************************
//  Setup all voice parameters in MIDI range
//  voice[0-3],wave[0-6],pitch[0-127],envelope[0-4],length[0-127],mod[0-127:64=no mod]
//*********************************************************************

void synth::setupVoice(unsigned char voice, unsigned char wave, unsigned char pitch, unsigned char env, unsigned char length, unsigned int mod)
{
  setWave(voice,wave);
  setPitch(voice,pitch);
  setEnvelope(voice,env);
  setLength(voice,length);
  setMod(voice,mod);
}

//*********************************************************************
//  Setup wave [0-6]
//*********************************************************************

void synth::setWave(unsigned char voice, unsigned char wave)
{
  switch (wave)
  {
    case TRIANGLE:
    wavs[voice] = (unsigned int)TriangleTable;
    break;
    case SQUARE:
    wavs[voice] = (unsigned int)SquareTable;
    break;
    case SAW:
    wavs[voice] = (unsigned int)SawTable;
    break;
    case RAMP:
    wavs[voice] = (unsigned int)RampTable;
    break;
    case NOISE:
    wavs[voice] = (unsigned int)NoiseTable;
    break;
    default:
    wavs[voice] = (unsigned int)SinTable;
    break;
  }
}
//*********************************************************************
//  Setup Pitch [0-127]
//*********************************************************************

void synth::setPitch(unsigned char voice,unsigned char MIDInote)
{
  PITCH[voice]=pgm_read_word(&PITCHS[MIDInote]);
}

//*********************************************************************
//  Setup Envelope [0-4]
//*********************************************************************

void synth::setEnvelope(unsigned char voice, unsigned char env)
{
  switch (env)
  {
    case 1:
    envs[voice] = (unsigned int)Env0;
    break;
    case 2:
    envs[voice] = (unsigned int)Env1;
    break;
    case 3:
    envs[voice] = (unsigned int)Env2;
    break;
    case 4:
    envs[voice] = (unsigned int)Env3;
    break;
    default:
    envs[voice] = (unsigned int)Env0;
    break;
  }
}
void synth::setFrequency(unsigned char voice,float f)
{
  PITCH[voice]=f/(FS/65535.0);
}
void synth::trigger(unsigned char voice)
{
    EPCW[voice]=0;
  FTW[voice]=PITCH[voice];
  //    FTW[voice]=PITCH[voice]+(PITCH[voice]*(EPCW[voice]/(32767.5*128.0  ))*((int)MOD[voice]-512));
}
//*********************************************************************
//  Setup Length [0-128]
//*********************************************************************

void synth::setLength(unsigned char voice,unsigned char length)
{
  EFTW[voice]=pgm_read_word(&EFTWS[length]);
  revSustain[voice] = length;
}

//*********************************************************************
//  Setup mod
//*********************************************************************

void synth::setMod(unsigned char voice,unsigned char mod)
{
  //    MOD[voice]=(unsigned int)mod*8;//0-1023 512=no mod
  MOD[voice]=(int)mod-64;//0-1023 512=no mod
}
void synth::setSustain(unsigned char voice, int v)
{
  sustainIt[voice] = v;
}
void synth::setVolume(unsigned char voice, int v)
{
  int _v = map(v, 100, 0, 0, 127);
  volume[voice] = _v;
}
//*********************************************************************
//  Midi trigger
//*********************************************************************

void synth::mTrigger(unsigned char voice,unsigned char MIDInote)
{
  PITCH[voice]=pgm_read_word(&PITCHS[MIDInote]);
  EPCW[voice]=0;
  FTW[divider] = PITCH[voice] + (int)   (((PITCH[voice]>>6)*(EPCW[voice]>>6))/128)*MOD[voice];
}

#if defined(__AVR__)
SIGNAL(TIMER1_COMPA_vect)
#elif defined(__arm__) && defined(TEENSYDUINO)
static void timerInterrupt()
#endif
{
  //-------------------------------
  // Time division
  //-------------------------------
  divider++;
  if(!(divider&=0x03))
  tik=1;

  //-------------------------------
  // Volume envelope generator
  //-------------------------------

  if (!(((unsigned char*)&EPCW[divider])[1]&0x80)){
    int value = (((unsigned char*)&(EPCW[divider]+=EFTW[divider]))[1]);
    switch (sustainIt[divider]) {
      case SUSTAIN:
      //default
      if(((unsigned)value)<volume[divider]){
        value= (value*-1)+volume[divider];
        value+=(((unsigned char*)&(EPCW[divider]+=EFTW[divider]))[1]);
      }
      break;
      case REV_SUSTAIN:
      if(volume[divider]>=70){
        if(((unsigned)value)<volume[divider]){
          value= (value*-1)+volume[divider];
          value+=(((unsigned char*)&(EPCW[divider]+=EFTW[divider]))[1]);
        }
      }
      else if(((unsigned)value)<revSustain[divider]){
        value= (value*-1)+revSustain[divider];
        if(((unsigned)value)<volume[divider]){
          value= (value);
          value+=(((unsigned char*)&(EPCW[divider]+=EFTW[divider]))[1]);
        }
      }
      break;
      case NONE:
      value= (value*-1)+volume[divider];
      value+=(((unsigned char*)&(EPCW[divider]+=EFTW[divider]))[1]);
      break;
    }
    AMP[divider] = pgm_read_byte(envs[divider]+value);
  }
  else{
    AMP[divider] = 0;
  }
  //-------------------------------
  //  Synthesizer/audio mixer
  //-------------------------------

  unsigned char sample = 127 +
  ((
    (((signed char)pgm_read_byte(wavs[0] + ((unsigned char *)&(PCW[0] += FTW[0]))[1]) * AMP[0]) >> 8) +
    (((signed char)pgm_read_byte(wavs[1] + ((unsigned char *)&(PCW[1] += FTW[1]))[1]) * AMP[1]) >> 8) +
    (((signed char)pgm_read_byte(wavs[2] + ((unsigned char *)&(PCW[2] += FTW[2]))[1]) * AMP[2]) >> 8) +
    (((signed char)pgm_read_byte(wavs[3] + ((unsigned char *)&(PCW[3] += FTW[3]))[1]) * AMP[3]) >> 8)
  ) >> 2);
  #if defined(__AVR__)
  OCR2A = OCR2B = sample;
  #else
  analogWrite(globalPWM, sample);
  #endif
  //************************************************
  //  Modulation engine
  //************************************************
  //  FTW[divider] = PITCH[divider] + (int)   (((PITCH[divider]/64)*(EPCW[divider]/64)) /128)*MOD[divider];
  FTW[divider] = PITCH[divider] + (int)   (((PITCH[divider]>>6)*(EPCW[divider]>>6))/128)*MOD[divider];
  tim++;
}
