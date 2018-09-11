#include "Arduino.h"
#include "synth.h"
//MaxVoices settings (This increases memory usage)
#if defined(__AVR_ATmega2560__)
#define maxVOICES 10  //Arduino Mega2560; best performance would be 8
#elif defined(__AVR__)
#define maxVOICES 4  //Arduino Uno, Mini, Micro, Pro Micro, Nano, and Teensy 2.0; best performance would be 4, but change this to your microcontroller's processing power :)
#elif defined(__arm__) && defined(TEENSYDUINO)
#define maxVOICES 16 //Teensy 3.++; Teensy boards can handle a whooping 30+ simulentaneous voices(Teensy is a powerful 32-bit chip)
#elif defined(ESP8266)
#define maxVOICES 2 //ESP8266, ESP32; best performance would be 6
#else
#error "Hey! This library isn't compatible with this board" //other unknown boards
#endif
//end of MaxVoice settings


static unsigned int PCW[maxVOICES];			//-Wave phase accumolators
static unsigned int FTW[maxVOICES];           //-Wave frequency tuning words
static unsigned char AMP[maxVOICES];           //-Wave amplitudes [0-255]
static unsigned int PITCH[maxVOICES];          //-Voice pitch
static unsigned int MOD[maxVOICES];                         //-Voice envelope modulation [0-1023 512=no mod. <512 pitch down >512 pitch up]
static unsigned int wavs[maxVOICES];                                  //-Wave table selector [address of wave in memory]
static unsigned int envs[maxVOICES];                                  //-Envelopte selector [address of envelope in memory]
static unsigned int EPCW[maxVOICES]; //-Envelope phase accumolator
static unsigned int EFTW[maxVOICES];               //-Envelope speed tuning word
static unsigned char divider = maxVOICES;                             //-Sample rate decimator for envelope
static unsigned int tim = 0;
static unsigned char tik = 0;
static unsigned int sustainIt[maxVOICES];//sustain variables
static unsigned int revSustain[maxVOICES];//sustain variables
static unsigned int volume[maxVOICES];//volume variables
static unsigned int numVoice;


#if defined(__AVR_ATmega32U4__)
#  if defined(CORE_TEENSY)  //Teensy 2.0
#define speaker_Pin A9
#  else  //Sparkfun Pro Micro
#define speaker_Pin 6
#  endif
#define mTCCR2A TCCR4A
#define mTCCR2B TCCR4B
#define mOCR2A OCR4D
#elif defined(__AVR__)/////////////////////
#  if defined(__AVR_ATmega2560__) //Arduino Mega
#define mTCCR2A TCCR2A
#define mTCCR2B TCCR2B
#define mOCR2A OCR2A
#define mOCR2B OCR2B
#define speaker_PinA 10
#define speaker_PinB 9
#  else                  //Arduino Uno, Mini, and Nano
#define mTCCR2A TCCR2A
#define mTCCR2B TCCR2B
#define mOCR2A OCR2A
#define mOCR2B OCR2B
#  endif
static unsigned int stereoMode[maxVOICES];//stereo variables
static uint8_t differentVoicesA=0; //number of voices per output
static uint8_t differentVoicesB=0;
static float maxVolume[maxVOICES] = {0};
static uint8_t volumeSetupCounter = 0;

#elif defined(__arm__) && defined(TEENSYDUINO)
static void timerInterrupt();
static IntervalTimer sampleTimer;
static unsigned char sample[maxVOICES];
static unsigned int originalOutput[maxVOICES] = {0};//voices that initiate the output
static unsigned int sameOutput[maxVOICES] = {0};//voices that share the same output with the originalOutput
static unsigned int stereoMode[maxVOICES];//outPut pin locations
static unsigned int differentVoices=0;
static float maxVolume[maxVOICES] = {0};
static uint8_t volumeSetupCounter = 0;
#elif defined(ESP8266)
uint32_t i2sACC;
uint8_t i2sCNT = 32;
uint16_t DAC = 0x8000;
uint16_t err;
#endif


#if defined(__AVR__)
#  if defined(__AVR_ATmega2560__)
SIGNAL(TIMER4_COMPB_vect)
#  else
SIGNAL(TIMER1_COMPB_vect)
#  endif
#elif defined(__arm__) && defined(TEENSYDUINO)
static void timerInterrupt()
#elif defined(ESP8266)
void ICACHE_RAM_ATTR onTimerISR()
#endif
{
  # if defined(ESP8266)
  divider++;
  if(divider>=numVoice){
    timer1_write(10);//Next in 2mS
    divider=-1;
    tik=1;
    return;
  }
  while (!(i2s_is_full())) {  //for ESP8266

    #else                     //for other boards
    divider++;
    if(divider>=numVoice){
      divider=-1;
      tik=1;
      return;
    }
    #endif
    //-------------------------------
    // Time division
    //-------------------------------

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
      AMP[divider] = pgm_read_byte((const void *)(envs[divider]+value));
    }
    else{
      AMP[divider] = 0;
    }
    //-------------------------------
    //  Synthesizer/audio mixer
    //-------------------------------
    #if defined(__AVR_ATmega32U4__)
    unsigned char sample = 127;
    for (uint8_t i = 0; i < numVoice; i++) {
      sample+=((((signed char)pgm_read_byte(wavs[i] + ((unsigned char *)&(PCW[i] += FTW[i]))[1]) * AMP[i]) >> 8) >> 2);
    }
    analogWrite(speaker_Pin, sample);  //The pro micro has a 10 bit timer, so analogWrite can shrink it to 8 bit

    #elif defined(ESP8266)
    unsigned char sample = 127;
    for (uint8_t i = 0; i < numVoice; i++) {
      sample+= ((((signed char)pgm_read_byte((const void *)(wavs[i] + ((unsigned char *)&(PCW[i] += FTW[i]))[1])) * AMP[i]) >> 8) >> 2);
    }
    DAC = map(sample,0,255,0,65538);

    //Pulse Density Modulated 16-bit I2S DAC
    for (uint8_t i = 0; i < 32; i++) {
      i2sACC = i2sACC << 1; if (DAC >= err) {
        i2sACC |= 1;
        err += 0xFFFF - DAC;
      }
      else
      {
        err -= DAC;
      }
    }
    i2s_write_sample(i2sACC);
    #elif defined(__AVR__)
    unsigned char sample = 127;
    unsigned char shared = 127;
    for (uint8_t i = 0; i < numVoice; i++) {
      if(stereoMode[i]==0){
        sample+=((((signed char)pgm_read_byte(wavs[i] + ((unsigned char *)&(PCW[i] += FTW[i]))[1]) * AMP[i]) >> 8) >> 2);
      }
      else{
        shared+=((((signed char)pgm_read_byte(wavs[i] + ((unsigned char *)&(PCW[i] += FTW[i]))[1]) * AMP[i]) >> 8) >> 2);
      }
    }
    mOCR2A = sample;
    mOCR2B = shared;

    #elif defined(__arm__) && defined(TEENSYDUINO)  //For Teensy LC, and 3.+
    for(uint8_t i = 0; i<numVoice; i++){
      sample[i] = 127;  //reset values to middle of wave
    }
    for (uint8_t i = 0; i < numVoice; i++) {
      uint8_t giveME = (sameOutput[i]!=0)?sameOutput[i]:originalOutput[i];   //Here is the most important part; I connect the voices that share the same output pin :)
      sample[giveME]+=((((signed char)pgm_read_byte(wavs[i] + ((unsigned char *)&(PCW[i] += FTW[i]))[1]) * AMP[i]) >> 8) >> 2); //update wave
    }
    for(uint8_t i = 0; i < numVoice; i++){
      if(sample[i]!=127)analogWrite(stereoMode[i], sample[i]);  //Play the wave to corresponding PWM pins
    }
    #endif


    #if defined(ESP8266)
  }
  timer1_write(500);//Next in 2mS
  #endif

  //************************************************
  //  Modulation engine
  //************************************************
  FTW[divider] = PITCH[divider] + (int)   (((PITCH[divider]/64)*(EPCW[divider]/64)) /128)*MOD[divider];
  FTW[divider] = PITCH[divider] + (int)   (((PITCH[divider]>>6)*(EPCW[divider]>>6))/128)*MOD[divider];
  tim++;
}

synth::synth(){}
void synth::begin(unsigned char voice)
{
  #if defined(__AVR_ATmega32U4__)
  pinMode(speaker_Pin, OUTPUT);
  cli(); // stop interrupts
  TCCR1A = 0x00;                                  //-Start audio interrupt
  TCCR1B = 0x09;
  // TCCR4C = 0x00;
  OCR1A=16000000.0 / FS_music;			    //-Auto sample rate
  SET(TIMSK1, OCIE1B);                            //-Start audio interrupt
  sei();                                          //-+
  mTCCR2A = 0xB0;                                  //-8 bit audio PWM
  mTCCR2B = 0x01;                                  // |
  mOCR2A = 127;
  #elif defined(__AVR__) || defined(__arm__) && defined(TEENSYDUINO)
  stereoMode[voice]= stereoMode[0];
  #elif defined(ESP8266)
  i2s_begin(); //Start the i2s DMA engine
  i2s_set_rate(44100); //Set sample rate
  pinMode(2, INPUT); //restore GPIOs taken by i2s
  pinMode(15, INPUT);
  timer1_attachInterrupt(onTimerISR); //Attach our sampling ISR
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(2000); //Service at 2mS intervall
  #endif
}
void synth::setNumVoices(uint8_t num){
  numVoice = num;
}
//*********************************************************************
//  Startup fancy selecting varoius output modes
//*********************************************************************

void synth::begin(unsigned char voice, unsigned char d)
{

  #if defined(__AVR_ATmega32U4__)||defined(ESP8266)
  begin(voice);
  #else
  #  if defined(__AVR__)
  cli();
  #    if defined(__AVR_ATmega2560__)
  TCCR4A = 0x00;                                  //-Start audio interrupt
  TCCR4B = 0x09;
  OCR4A=16000000.0 / FS_music;			    //-Auto sample rate
  SET(TIMSK4, OCIE4B);                            //-Start audio interrupt
  #    else
  TCCR1A = 0x00;                                  //-Start audio interrupt
  TCCR1B = 0x09;
  OCR1A=16000000.0 / FS_music;			    //-Auto sample rate
  SET(TIMSK1, OCIE1B);                            //-Start audio interrupt
  #    endif
  sei();                                          //-+
  #  elif defined(__arm__) && defined(TEENSYDUINO)
  sampleTimer.begin(timerInterrupt, 1000000.0 / FS_music);
  #  endif

  switch(d)
  {
    #if defined(__arm__) && defined(TEENSYDUINO)
    default:
    for(uint8_t i = 0; i<numVoice;i++){  //check to see if output pin is different from the other voices
      if(stereoMode[i]==d){
        // Serial.print("Match: ");Serial.print(voice);Serial.print(" = ");Serial.println(i);
        sameOutput[voice] = i;
        break; //not different
      }
      else if(((unsigned)i)==(numVoice-1)){ //we have reached the end, and have found a unique pin
        // differentVoices++;//different
        // Serial.print("Original: ");Serial.println(voice);
        originalOutput[voice] = voice;
        differentVoices++;
        // Serial.println(differentVoices);
      }
    }
    stereoMode[voice]= d;
    analogWriteFrequency(d, FS_music);
    break;

    #else
    case CHB:                                         //-Single ended signal on CHB pin (3)
    stereoMode[voice]= 1;
    differentVoicesB++;
    mTCCR2A = 0xB3;                                  //-8 bit audio PWM
    mTCCR2B = 0x01;                                  // |
    mOCR2B = 127;
    #  if defined(__AVR_ATmega2560__)
    pinMode(speaker_PinB,OUTPUT);
    #  elif defined(__AVR__)
    SET(DDRD, 3);				      //-PWM pin
    #  endif
    break;

    case CHA:
    stereoMode[voice]= 0;
    differentVoicesA++;
    mTCCR2A = 0xB3;                                  //-8 bit audio PWM
    mTCCR2B = 0x01;                                  // |
    mOCR2A = 127;
    #  if defined(__AVR_ATmega2560__)
    pinMode(speaker_PinA,OUTPUT);
    #  elif defined(__AVR__)
    SET(DDRB, 3);				      //-PWM pin
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

void synth::setupVoice(unsigned char voice, unsigned char wave, unsigned char pitch, unsigned char env, unsigned char length, unsigned int mod)
{
  setWave(voice,wave);
  setPitch(voice,pitch);
  setEnvelope(voice,env);
  setLength(voice,length);
  setMod(voice,mod);
  volume[voice] = 20;
  revSustain[voice] = 1;
  sustainIt[voice] = 1;
  AMP[voice] = 255;
  EPCW[voice] = 0x8000;
  EFTW[voice] = 10;
  #if defined(__arm__) && defined(TEENSYDUINO)
  // stereoMode[voice]=100;
  #endif
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
  #if defined(ESP8266)
  PITCH[voice]=f/(FS_music/65535.0)*38;
  #else
  PITCH[voice]=f/(FS_music/65535.0);
  #endif
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
  #if defined(ESP8266)
  EFTW[voice]=pgm_read_word(&EFTWS[length+10]);
  revSustain[voice] = length+6;
  #else
  EFTW[voice]=pgm_read_word(&EFTWS[length]);
  revSustain[voice] = length;
  #endif
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
  #if defined(__arm__) && defined(TEENSYDUINO)
  if(volumeSetupCounter<=numVoice){  //skip the first time this method is called because we don't know how many instruments will share an output pin yet.
  if(volumeSetupCounter==numVoice){  //one time setup to find maxVolume for each output pin
    unsigned int store[numVoice] = {0};
    for (uint8_t j = 0; j < numVoice; j++) {
      int giveME = (sameOutput[j]!=0)?sameOutput[j]:originalOutput[j];
      store[giveME] = store[giveME] + 1;   //add up pins that use same output
      // Serial.println(giveME);
    }
    //Calculate maxVolume for each pin.
    for (uint8_t i = 0; i < numVoice; i++) {
      maxVolume[i] = (store[i%differentVoices]>4)?(127*(((51-store[i%differentVoices])*2)/120.0)):127;  //apply formula that finds maxVolume depending on the number of voices per pin.
      // Serial.print("Voice ");Serial.print(i);Serial.print(" , maxVolume: ");Serial.print(maxVolume[i]);Serial.print(" , numberPins: ");Serial.println(store[i%differentVoices]);
    }
  }
  volumeSetupCounter++;  //never come back here again
}
volume[voice] = map(v, 127, 0, 127-maxVolume[voice], 127);

#elif defined(__AVR_ATmega32U4__) || defined(ESP8266)  //no need to check same output, because these boards don't support Stereo mode :'(
  float maxVolume = (numVoice>4)?(127*(((49-numVoice)*2)/120.0)):127;
  volume[voice] = map(v, 127, 0,127 - maxVolume, 127);

  #else  //Uno and Mega
  if(volumeSetupCounter<=numVoice){  //skip the first time this method is called because we don't know how many instruments will share an output pin yet.
  if(volumeSetupCounter==numVoice){  //one time setup to find maxVolume for each output pin
    float maxVolumeA = (differentVoicesA>3)?(127*(((49-differentVoicesA)*2)/120.0)):127;  //Calculate maxVolume for each pin.
    float maxVolumeB = (differentVoicesB>3)?(127*(((49-differentVoicesB)*2)/120.0)):127;
    for (uint8_t j = 0; j < numVoice; j++) {
      maxVolume[j] = (stereoMode[j]==0)? maxVolumeA:maxVolumeB;
      // Serial.print("Voice ");Serial.print(j);Serial.print(" , maxVolume: ");Serial.print(maxVolume[j]);Serial.print(" , numberPins: ");Serial.println((stereoMode[j]==0)? differentVoicesA:differentVoicesB);
    }
  }
  volumeSetupCounter++;  //never come back here again
}
// volume[voice] = map(v, 127, 0, 0, 127);
  volume[voice] = map(v, 127, 0, 127-maxVolume[voice], 127);  //set volume using maxVolume
#endif
}

//*********************************************************************
//  Midi trigger
//*********************************************************************

void synth::mTrigger(unsigned char voice,unsigned char MIDInote)
{
  #if defined(ESP8266)
  PITCH[voice]=pgm_read_word(&PITCHS[MIDInote-12]);
  #else
  PITCH[voice]=pgm_read_word(&PITCHS[MIDInote]);
  #endif
  EPCW[voice]=0;
  FTW[divider] = PITCH[voice] + (int)   (((PITCH[voice]>>6)*(EPCW[voice]>>6))/128)*MOD[voice];
}
