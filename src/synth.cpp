#include "Arduino.h"
#include "synth.h"



//MaxVoices settings (This increases memory usage, 22+ bytes per voice)
#if   defined(__AVR_ATmega2560__)
#  define   maxVOICES 10  //Arduino Mega2560; best performance would be 8
#elif defined(__AVR__)
#  define   maxVOICES 4   //Arduino Uno, Mini, Micro, Pro Micro, Nano, and Teensy 2.0; best performance would be 4, but change this to your microcontroller's processing power :)
#elif defined(__arm__) && defined(TEENSYDUINO)
#  define   maxVOICES 16  //Teensy 3.++; Teensy boards can handle a whooping 30+ simulentaneous voices(Teensy is a powerful 32-bit chip)
#elif defined(ESP8266)
#  define   maxVOICES 2   //ESP8266, ESP32; best performance would be 6
#else
#  error "Hey! This library isn't compatible with this board" //other unknown boards
#endif
//end of MaxVoice settings



static uint16_t       PCW[maxVOICES];		  	  //-Wave phase accumolators
static uint16_t       FTW[maxVOICES];         //-Wave frequency tuning words
static uint8_t        AMP[maxVOICES];         //-Wave amplitudes [0-255]
static uint16_t       PITCH[maxVOICES];       //-Voice pitch
static int8_t         MOD[maxVOICES];         //-Voice envelope modulation [0-1023 512=no mod. <512 pitch down >512 pitch up]
static uint8_t        wavs[maxVOICES];        //-Wave table selector [address of wave in memory]
static uint16_t       envs[maxVOICES];        //-Envelopte selector [address of envelope in memory]
static uint16_t       EPCW[maxVOICES];        //-Envelope phase accumolator
static uint16_t       EFTW[maxVOICES];        //-Envelope speed tuning word
static uint16_t       sustainIt[maxVOICES];   //-sustain variables
static uint16_t       revSustain[maxVOICES];  //-sustain variables
static uint16_t       volume[maxVOICES];      //-volume variables

static uint8_t        divider   = maxVOICES;  //-Sample rate decimator for envelope
static uint16_t       tim       = 0;
static uint8_t        tik       = 0;
static uint16_t       numVoice;


// Timer 0, used by milllis(), micros(), delay(), PWM pin 5 & 6
// Timer 1, used by Servo library, PWM pin 9 & 10, (10 SPI CS)
// Timer 2, used by tone(), PWM pin 3 & 11, (11 SPI MOSI)
//
//    Timer 0   Timer 1   Timer 2   Rôle
//    ---------------------------------------------------------
//    TCNT0     TCNT1L    TCNT2     Timer (bit 0 à 7)
//    -         TCNT1H    -         Timer (bit 8 à 15)
//    TCCR0A    TCCR1A    TCCR2A    Registre de contrôle
//    TCCR0B    TCCR1B    TCCR2B    Registre de contrôle
//    -         TCCR1C    -         Registre de contrôle
//    OCR0A     OCR1AL    OCR2A     Output Compare (bit 0 à 7)
//    -         OCR1AH    -         Output Compare (bit 8 à 15)
//    OCR0B     OCR1BL    OCR2B     Output Compare (bit 0 à 7)
//    -         OCR1BH    -         Output Compare (bit 8 à 15)
//    TIMSK0    TIMSK1    TIMSK2    Interrupt Mask
//    TIFR0     TIFR1     TIFR2     Interrupt Flag
#if   defined(__AVR_ATmega32U4__)
#  define     mTCCR2A       TCCR4A
#  define     mTCCR2B       TCCR4B
#  define     mOCR2A        OCR4D
#  if   defined(CORE_TEENSY)      // Teensy 2.0
#    define   speaker_Pin   A9
#  else                           // Sparkfun Pro Micro
#    define   speaker_Pin   6
#  endif
#elif defined(__AVR__)            //////////AVRs///////////
#  define     mTCCR2A       TCCR2A      // use timer2
#  define     mTCCR2B       TCCR2B
#  define     mOCR2A        OCR2A
#  define     mOCR2B        OCR2B
#  if   defined(__AVR_ATmega2560__) // Arduino Mega
#    define   speaker_PinA  10          // use PWM pin 9 & 10 from timer 2
#    define   speaker_PinB  9
#  else                             // Arduino Uno, Mini, and Nano
#    define   speaker_PinA  11          // use PWM pin 3 & 11 from timer 2
#    define   speaker_PinB  3
#  endif

static uint8_t        stereoMode[maxVOICES];    // stereo variables
static uint8_t        differentVoicesA          = 0;   // number of voices per output
static uint8_t        differentVoicesB          = 0;
static uint8_t        maxVolume[maxVOICES]      = {0};
static uint8_t        volumeSetupCounter        = 0;

#elif defined(__arm__) && defined(TEENSYDUINO)
static void           timerInterrupt();
static                IntervalTimer sampleTimer;
static uint8_t        sample[maxVOICES];
static uint8_t        originalOutput[maxVOICES] = {0};  // voices that initiate the output
static uint8_t        sameOutput[maxVOICES]     = {0};  // voices that share the same output with the originalOutput
static uint8_t        stereoMode[maxVOICES];            // outPut pin locations
static uint8_t        differentVoices           = 0;
static uint8_t        maxVolume[maxVOICES]      = {0};
static uint8_t        volumeSetupCounter        = 0;

#elif defined(ESP8266)
uint32_t              i2sACC;
uint8_t               i2sCNT                    = 32;
uint16_t              DAC                       = 0x8000;
uint16_t              err;
#endif


//*
//int8_t synth::_getWaveValue(uint8_t voice, uint8_t value)
int8_t _getWaveValue(uint8_t voice, uint8_t value)
{
  unsigned int  pWaveTable = 0;
  char          r;
  
  switch (wavs[voice])
  { //                    0         64        128       192         256
    case TRIANGLE:    //  0         127       0         -127        0
      r       = 1 + 2 * (value % 64);
      value  /= 64;
      if (value && value != 2)  r   = 127 - r;
      if (         value  > 1)  r   = -r;                  
      break;
    case SQUARE:      //  127             127,-127             -127,127
      r       = char((value < 128) ? 127 : - 127);
      break;
    case SAW:         //  127                 0                    -128   !! -128~127
      r       = char(127 - value);
      break;
    case RAMP:        //  -128                0                     127   !! -128~127
      r       = char(value - 128);
      break;
    case NOISE:       //  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
      pWaveTable                  = (unsigned int)NoiseTable;  
    case SINE:        // 0        127         0         -127        0
      if (!pWaveTable) pWaveTable = (unsigned int)SinTable;
      r       = value % 64;
      value  /= 64;
      if (value && value != 2)  r   = 63 - r;
      r       = (signed char)pgm_read_byte(pWaveTable + r);
      if (         value >  1)  r   = -r;
      break;
  }
  
  return r;
}
//*/



int8_t _getEnvValue(int8_t value)
{
  value  = (value * -1) + volume[divider];
  return value + (((unsigned char*) &(EPCW[divider] += EFTW[divider]))[1]);
}



#if   defined(__AVR__)
#  if   defined(__AVR_ATmega2560__)
SIGNAL(TIMER4_COMPB_vect)

#  else
SIGNAL(TIMER1_COMPB_vect)       // Timer 2 used for voices and output
#  endif

#elif defined(__arm__) && defined(TEENSYDUINO)
static void timerInterrupt()

#elif defined(ESP8266)
void        ICACHE_RAM_ATTR onTimerISR()
#endif
{
  //-------------------------------
  // Time division
  //-------------------------------
#if   defined(ESP8266)    // for ESP8266
  divider++;
  if (divider >= numVoice)
  {
    timer1_write(10);           // Next in 2mS
    divider = -1;
    tik     = 1;
    return;
  }
  while (!(i2s_is_full()))
  {

#else                     // for other boards
  divider++;
  if (divider >= numVoice)
  {
    divider = -1;
    tik     = 1;
    return;
  }
#endif



    //-------------------------------
    // Volume envelope generator
    //-------------------------------
    if (!(((unsigned char*)&EPCW[divider])[1] & 0x80))
    {
      uint8_t value = (((unsigned char*) &(EPCW[divider] += EFTW[divider]))[1]);
      
      switch (sustainIt[divider])
      {
        case SUSTAIN:           //default
          if (value < volume[divider])
          {
            value  = _getEnvValue(value);
            //value  = (value * -1) + volume[divider];
            //value += (((unsigned char*) &(EPCW[divider] += EFTW[divider]))[1]);
          }
          break;
        case REV_SUSTAIN:
          if (volume[divider] >= 70)
          {
            if (value < volume[divider])
            {
              value  = _getEnvValue(value);
              //value  = (value * -1) + volume[divider];
              //value += (((unsigned char*) &(EPCW[divider] += EFTW[divider]))[1]);
            }
          }
          else if (value < revSustain[divider])
          {
            value = (value * -1) + revSustain[divider];
            if (value < volume[divider])
            {
              //value  = (value);
              value += (((unsigned char*) &(EPCW[divider] += EFTW[divider]))[1]);
            }
          }
          break;
        case NONE:
          value  = _getEnvValue(value);
          //value  = (value * -1) + volume[divider];
          //value += (((unsigned char*) &(EPCW[divider] += EFTW[divider]))[1]);
          break;
        case SLUR:
          if (value > 24)   // sounds like best results from 16~32
          {
            value  = _getEnvValue(value);
            //value  = (value * -1) + volume[divider];
            //value += (((unsigned char*) &(EPCW[divider] += EFTW[divider]))[1]);
          }
          break;
      }
      //AMP[divider] = pgm_read_byte((const void *)(envs[divider] + value));
      AMP[divider] = pgm_read_byte((const void *)(envs[divider] + (value / 4)));
    }
    else
    {
      AMP[divider] = 0;
    }


    
    //-------------------------------
    //  Synthesizer/audio mixer
    //-------------------------------
#if defined(__AVR_ATmega32U4__)
    uint8_t sample = 127;
    for (uint8_t i = 0; i < numVoice; i++)
    {
      //sample += ((((signed char)pgm_read_byte(wavs[i] + ((unsigned char *) & (PCW[i] += FTW[i]))[1]) * AMP[i]) >> 8) >> 2);
      sample += (((_getWaveValue(i, ((unsigned char *) & (PCW[i] += FTW[i]))[1]) * AMP[i]) >> 8) >> 2);
    }
    analogWrite(speaker_Pin, sample);  //The pro micro has a 10 bit timer, so analogWrite can shrink it to 8 bit

#elif defined(ESP8266)
    uint8_t sample = 127;
    for (uint8_t i = 0; i < numVoice; i++)
    {
      //sample += ((((signed char)pgm_read_byte((const void *)(wavs[i] + ((unsigned char *) & (PCW[i] += FTW[i]))[1])) * AMP[i]) >> 8) >> 2);
      //sample += (((_getWaveValue((const void *)(i, ((unsigned char *) & (PCW[i] += FTW[i]))[1])) * AMP[i]) >> 8) >> 2);
      sample += (((_getWaveValue((const void *)(i, ((unsigned char *) & (PCW[i] += FTW[i]))[1])) * AMP[i]) >> 8) >> 2);
    }
    DAC = map(sample, 0, 255, 0, 65538);

    //Pulse Density Modulated 16-bit I2S DAC
    for (uint8_t i = 0; i < 32; i++)
    {
      i2sACC = i2sACC << 1;
      if (DAC >= err)
      {
        i2sACC |= 1;
        err    += 0xFFFF - DAC;
      }
      else
      {
        err    -= DAC;
      }
    }
    i2s_write_sample(i2sACC);
    
#elif defined(__AVR__)
    uint8_t sample = 127;
    uint8_t shared = 127;
    for (uint8_t i = 0; i < numVoice; i++)
    {
      if (stereoMode[i] == 0)
      {
        //sample += ((((signed char)pgm_read_byte(wavs[i] + ((unsigned char *) & (PCW[i] += FTW[i]))[1]) * AMP[i]) >> 8) >> 2);
        sample += (((_getWaveValue(i, ((unsigned char *) &(PCW[i] += FTW[i]))[1]) * AMP[i]) >> 8) >> 2);
      }
      else
      {
        //shared += ((((signed char)pgm_read_byte(wavs[i] + ((unsigned char *) & (PCW[i] += FTW[i]))[1]) * AMP[i]) >> 8) >> 2);
        shared += (((_getWaveValue(i, ((unsigned char *) &(PCW[i] += FTW[i]))[1]) * AMP[i]) >> 8) >> 2);
      }
    }
    mOCR2A = sample;
    mOCR2B = shared;

#elif defined(__arm__) && defined(TEENSYDUINO)  //For Teensy LC, and 3.+
    for (uint8_t i = 0; i < numVoice; i++)
    {
      sample[i] = 127;  //reset values to middle of wave
    }
    
    for (uint8_t i = 0; i < numVoice; i++)
    {
      uint8_t giveME = (sameOutput[i] != 0) ? sameOutput[i] : originalOutput[i]; //Here is the most important part; I connect the voices that share the same output pin :)
      //sample[giveME] += ((((signed char)pgm_read_byte(wavs[i] + ((unsigned char *) & (PCW[i] += FTW[i]))[1]) * AMP[i]) >> 8) >> 2); //update wave
      sample[giveME] += (((_getWaveValue(i, ((unsigned char *) &(PCW[i] += FTW[i]))[1]) >> 2)) * AMP[i]) >> 8) >> 2); //update wave
    }
    
    for (uint8_t i = 0; i < numVoice; i++)
    {
      if (sample[i] != 127)analogWrite(stereoMode[i], sample[i]); //Play the wave to corresponding PWM pins
    }
#endif


#if   defined(ESP8266)
  }
  timer1_write(500);    //Next in 2mS
#endif



  //************************************************
  //  Modulation engine
  //************************************************
  FTW[divider] = PITCH[divider] + (int)(((PITCH[divider] / 64) * (EPCW[divider] / 64)) / 128) * MOD[divider];
  FTW[divider] = PITCH[divider] + (int)(((PITCH[divider] >> 6) * (EPCW[divider] >> 6)) / 128) * MOD[divider];
  tim++;
}





synth::synth() {}



void synth::begin(uint8_t voice)
{
#if   defined(__AVR_ATmega32U4__)
  pinMode(speaker_Pin, OUTPUT);
  cli(); // stop interrupts
  TCCR1A  = 0x00;                         //-Start audio interrupt
  TCCR1B  = 0x09;
  //OCR1A   = 16000000.0 / FS_music;			  //-Auto sample rate
  OCR1A   = 16000000UL / FS_music;        //-Auto sample rate
  SET(TIMSK1, OCIE1B);                    //-Start audio interrupt
  sei();                                  //-+
  mTCCR2A = 0xB0;                         //-8 bit audio PWM
  mTCCR2B = 0x01;                         // |
  mOCR2A  = 127;
  
#elif defined(__AVR__) || defined(__arm__) && defined(TEENSYDUINO)
  stereoMode[voice] = stereoMode[0];
  
#elif defined(ESP8266)
  i2s_begin();                            //Start the i2s DMA engine
  i2s_set_rate(44100);                    //Set sample rate
  pinMode(2,  INPUT);                     //restore GPIOs taken by i2s
  pinMode(15, INPUT);
  timer1_attachInterrupt(onTimerISR);     //Attach our sampling ISR
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(2000);                     //Service at 2mS intervall
#endif
}


void synth::setNumVoices(uint8_t num)
{
  numVoice = num;
}



//*********************************************************************
//  Startup fancy selecting various output modes
//*********************************************************************

void synth::begin(uint8_t voice, uint8_t d)
{
#if   defined(__AVR_ATmega32U4__) || defined(ESP8266)
  begin(voice);
  
#else
#  if defined(__AVR__)
  cli();
  
#    if defined(__AVR_ATmega2560__)
  TCCR4A  = 0x00;                         //-Start audio interrupt
  TCCR4B  = 0x09;
  //OCR4A   = 16000000.0 / FS_music;			  //-Auto sample rate
  OCR4A   = 16000000UL / FS_music;        //-Auto sample rate
  SET(TIMSK4, OCIE4B);                    //-Start audio interrupt
  
#    else
  TCCR1A  = 0x00;                         //-Start audio interrupt
  TCCR1B  = 0x09;
  //OCR1A   = 16000000.0 / FS_music;			  //-Auto sample rate
  OCR1A   = 16000000UL / FS_music;        //-Auto sample rate
  SET(TIMSK1, OCIE1B);                    //-Start audio interrupt
#    endif

  sei();                                  //-restart interuption
  
#  elif defined(__arm__) && defined(TEENSYDUINO)
  //sampleTimer.begin(timerInterrupt, 1000000.0 / FS_music);
  sampleTimer.begin(timerInterrupt, 1000000UL / FS_music);
#  endif

  switch (d)
  {
    
#if   defined(__arm__) && defined(TEENSYDUINO)
    default:
      for (uint8_t i = 0; i < numVoice; i++)
      { 
        //check to see if output pin is different from the other voices
        if      (stereoMode[i] == d)
        {
          // Serial.print("Match: ");Serial.print(voice);Serial.print(" = ");Serial.println(i);
          sameOutput[voice] = i;
          break; //not different
        }
        else if (i == (numVoice - 1))
        { 
          //we have reached the end, and have found a unique pin
          // differentVoices++;//different
          // Serial.print("Original: ");Serial.println(voice);
          originalOutput[voice] = voice;
          differentVoices++;
          // Serial.println(differentVoices);
        }
      }
      stereoMode[voice] = d;
      analogWriteFrequency(d, FS_music);
      break;

#else
    case CHB:                                         //-Single ended signal on CHB pin (3)
      stereoMode[voice] = 1;
      differentVoicesB++;
      mTCCR2A = 0xB3;                                 //-8 bit audio PWM
      mTCCR2B = 0x01;                                 // |
      mOCR2B  = 127;
      
#  if   defined(__AVR_ATmega2560__)
      pinMode(speaker_PinB, OUTPUT);
      
#  elif defined(__AVR__)
      pinMode(speaker_PinB, OUTPUT);
      
#  endif
      break;

    case CHA:
      stereoMode[voice] = 0;
      differentVoicesA++;
      mTCCR2A = 0xB3;                                  //-8 bit audio PWM
      mTCCR2B = 0x01;                                  // |
      mOCR2A  = 127;
      
#  if   defined(__AVR_ATmega2560__)
      pinMode(speaker_PinA, OUTPUT);
      
#  elif defined(__AVR__)
      pinMode(speaker_PinA, OUTPUT);
#  endif
      break;
      
#endif
  }
#endif
}



//*********************************************************************
//  Setup all voice parameters in MIDI range
//  voice[0-3],wave[0-6],pitch[0-127],envelope[0-4],length[0-127],mod[0-127:64=no mod]
//*********************************************************************

void synth::setupVoice(uint8_t voice, uint8_t wave, uint8_t pitch, uint8_t env, uint8_t length, uint8_t mod)
{
  setWave(voice, wave);
  setPitch(voice, pitch);
  setEnvelope(voice, env);
  setLength(voice, length);
  setMod(voice, mod);
  volume[voice]               = 20;
  revSustain[voice]           = 1;
  sustainIt[voice]            = 1;
  AMP[voice]                  = 255;
  EPCW[voice]                 = 0x8000;
  EFTW[voice]                 = 10;
  
#if defined(__arm__) && defined(TEENSYDUINO)
  stereoMode[voice]=100;
#endif
}



//*********************************************************************
//  Setup wave [0-6]
//*********************************************************************

void synth::setWave(uint8_t voice, uint8_t wave)
{
  wavs[voice] = wave;
  /*
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
  //*/
}



//*********************************************************************
//  Setup Pitch [0-127]
//*********************************************************************

void synth::setPitch(uint8_t voice, uint8_t MIDInote)
{
  PITCH[voice] = pgm_read_word(&PITCHS[MIDInote]);
}



//*********************************************************************
//  Setup Envelope [0-4]
//*********************************************************************

void synth::setEnvelope(uint8_t voice, uint8_t env)
{
  switch (env)
  {
    case 0:
      envs[voice] = (unsigned int)Env0;
      break;
    case 1:
      envs[voice] = (unsigned int)Env1;
      break;
    case 2:
      envs[voice] = (unsigned int)Env2;
      break;
    case 3:
      envs[voice] = (unsigned int)Env3;
      break;
    default:
      envs[voice] = (unsigned int)Env0;
      break;
  }
}



void synth::setFrequency(uint8_t voice, float f)
{
#if   defined(ESP8266)
  //PITCH[voice] = f / (FS_music / 65535.0) * 38;
  PITCH[voice] = f / (FS_music / 65535U) * 38;
  
#else
  //PITCH[voice] = f / (FS_music / 65535.0);
  PITCH[voice] = f / (FS_music / 65535U);
#endif
}



void synth::trigger(uint8_t voice)
{
  EPCW[voice] = 0;
  FTW[voice]  = PITCH[voice];
  //    FTW[voice]=PITCH[voice]+(PITCH[voice]*(EPCW[voice]/(32767.5*128.0  ))*((int)MOD[voice]-512));
}



//*********************************************************************
//  Setup Length [0-128]
//*********************************************************************

void synth::setLength(uint8_t voice, uint8_t length)
{
#if   defined(ESP8266)
  EFTW[voice]       = pgm_read_word(&EFTWS[length + 10]);
  revSustain[voice] = length + 6;
  
#else
  EFTW[voice]       = pgm_read_word(&EFTWS[length]);
  revSustain[voice] = length;
#endif
}



//*********************************************************************
//  Setup mod
//*********************************************************************

void synth::setMod(uint8_t voice, uint8_t mod)
{
  //MOD[voice]=(unsigned int)mod*8;//0-1023 512=no mod
  MOD[voice] = mod - 64; //  -64~63, 0=no mod
}



void synth::setSustain(uint8_t voice, uint8_t v)
{
  sustainIt[voice] = v;
}



void synth::setVolume(uint8_t voice, uint8_t v)
{
#if   defined(__arm__) && defined(TEENSYDUINO)
  if (volumeSetupCounter <= numVoice)
  { 
    //skip the first time this method is called because we don't know how many instruments will share an output pin yet.
    if (volumeSetupCounter == numVoice)
    { 
      //one time setup to find maxVolume for each output pin
      uint8_t store[numVoice] = {0};
      for (uint8_t i = 0; i < numVoice; i++)
      {
        uint8_t giveME  = (sameOutput[i] != 0) ? sameOutput[i] : originalOutput[i];
        store[giveME]  += 1;                  //add up pins that use same output
        // Serial.println(giveME);
      }
      
      //Calculate maxVolume for each pin.
      for (uint8_t i = 0; i < numVoice; i++)
      {
        //apply formula that finds maxVolume depending on the number of voices per pin.
        //maxVolume[i] = (store[i % differentVoices] > 4) ? (127 * (((51 - store[i % differentVoices]) * 2) / 120.0)) : 127;
        //maxVolume[i] = (store[i % differentVoices] > 4) ? (127 * ((51 - store[i % differentVoices]) / 60)) : 127;
        maxVolume[i] = (store[i % differentVoices] > 4) ? (2 * (51 - store[i % differentVoices])) : 127;
        // Serial.print("Voice ");Serial.print(i);Serial.print(" , maxVolume: ");Serial.print(maxVolume[i]);Serial.print(" , numberPins: ");Serial.println(store[i%differentVoices]);
      }
    }
    volumeSetupCounter++;  //never come back here again
  }
  volume[voice] = map(v, 127, 0, 127 - maxVolume[voice], 127);

#elif defined(__AVR_ATmega32U4__) || defined(ESP8266)  
  //no need to check same output, because these boards don't support Stereo mode :'(
  //float maxVolume = (numVoice > 4) ? (127 * (((49 - numVoice) * 2) / 120.0)) : 127;
  //uint8_t maxVolume = (numVoice > 4) ? (127 * ((49 - numVoice) / 60)) : 127;
  uint8_t maxVolume = (numVoice > 4) ? (2 * (49 - numVoice)) : 127;
  volume[voice]     = map(v, 127, 0, 127 - maxVolume, 127);

#else  //Uno and Mega
  if (volumeSetupCounter <= numVoice)
  { 
    //skip the first time this method is called because we don't know how many instruments will share an output pin yet.
    if (volumeSetupCounter == numVoice)
    { 
      //one time setup to find maxVolume for each output pin, calculate maxVolume for each pin.
      //float maxVolumeA = (differentVoicesA > 3) ? (127 * (((49 - differentVoicesA) * 2) / 120.0)) : 127;
      //float maxVolumeB = (differentVoicesB > 3) ? (127 * (((49 - differentVoicesB) * 2) / 120.0)) : 127;
      //uint8_t maxVolumeA = (differentVoicesA > 3) ? (127 * ((49 - differentVoicesA) / 60)) : 127;
      //uint8_t maxVolumeB = (differentVoicesB > 3) ? (127 * ((49 - differentVoicesB) / 60)) : 127;
      uint8_t maxVolumeA = (differentVoicesA > 3) ? (2 * (49 - differentVoicesA)) : 127;    // 2.11
      uint8_t maxVolumeB = (differentVoicesB > 3) ? (2 * (49 - differentVoicesB)) : 127;
      
      //Calculate maxVolume for each pin.
      for (uint8_t i = 0; i < numVoice; i++)
      {
        maxVolume[i] = (stereoMode[i] == 0) ? maxVolumeA : maxVolumeB;
        // Serial.print("Voice ");Serial.print(j);Serial.print(" , maxVolume: ");Serial.print(maxVolume[j]);Serial.print(" , numberPins: ");Serial.println((stereoMode[j]==0)? differentVoicesA:differentVoicesB);
      }
    }
    volumeSetupCounter++;  //never come back here again
  }
  // volume[voice] = map(v, 127, 0, 0, 127);
  volume[voice] = map(v, 127, 0, 127 - maxVolume[voice], 127); //set volume using maxVolume
#endif
}



//*********************************************************************
//  Midi trigger
//*********************************************************************

void synth::mTrigger(uint8_t voice, uint8_t MIDInote)
{
#if   defined(ESP8266)
  PITCH[voice] = pgm_read_word(&PITCHS[MIDInote - 12]);
  
#else
  PITCH[voice] = pgm_read_word(&PITCHS[MIDInote]);
#endif

  EPCW[voice]  = 0;
  FTW[divider] = PITCH[voice] + (uint16_t)(((PITCH[voice] >> 6) * (EPCW[voice] >> 6)) / 128) * MOD[voice];
}


