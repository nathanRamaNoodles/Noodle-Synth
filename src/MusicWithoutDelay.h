/*
MusicWithoutDelay.h - Library for playing music at multiple pins without Using Delay. Also uses the Tone library.
MusicWithoutDelay Created by Nathan Ramanathan(nathan6ramanathan@gmail.com), March 21, 2018

MIT License

Copyright (c) 2018 nathanRamaNoodles

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef MusicWithoutDelay_h
#define MusicWithoutDelay_h

#include "Arduino.h"
#include "synthEngine.h"


#define NOTE_C1  24
#define NOTE_CS1 25
#define NOTE_D1  26
#define NOTE_DS1 27
#define NOTE_E1  28
#define NOTE_F1  29
#define NOTE_FS1 30
#define NOTE_G1  31
#define NOTE_GS1 32
#define NOTE_A1  33
#define NOTE_AS1 34
#define NOTE_B1  35
#define NOTE_C2  36
#define NOTE_CS2 37
#define NOTE_D2  38
#define NOTE_DS2 39
#define NOTE_E2  40
#define NOTE_F2  41
#define NOTE_FS2 42
#define NOTE_G2  43
#define NOTE_GS2 44
#define NOTE_A2  45
#define NOTE_AS2 46
#define NOTE_B2  47
#define NOTE_C3  48
#define NOTE_CS3 49
#define NOTE_D3  50
#define NOTE_DS3 51
#define NOTE_E3  52
#define NOTE_F3  53
#define NOTE_FS3 54
#define NOTE_G3  55
#define NOTE_GS3 56
#define NOTE_A3  57 //A3
#define NOTE_AS3 58
#define NOTE_B3  59
#define NOTE_C4  60  //middle C
#define NOTE_CS4 61
#define NOTE_D4  62
#define NOTE_DS4 63
#define NOTE_E4  64
#define NOTE_F4  65
#define NOTE_FS4 66
#define NOTE_G4  67
#define NOTE_GS4 68
#define NOTE_A4  69  //A4 440 Hz
#define NOTE_AS4 70
#define NOTE_B4  71
#define NOTE_C5  72
#define NOTE_CS5 73
#define NOTE_D5  74
#define NOTE_DS5 75
#define NOTE_E5  76
#define NOTE_F5  77
#define NOTE_FS5 78
#define NOTE_G5  79
#define NOTE_GS5 80
#define NOTE_A5  81
#define NOTE_AS5 82
#define NOTE_B5  83
#define NOTE_C6  84
#define NOTE_CS6 85
#define NOTE_D6  86
#define NOTE_DS6 87
#define NOTE_E6  88
#define NOTE_F6  89
#define NOTE_FS6 90
#define NOTE_G6  91
#define NOTE_GS6 92
#define NOTE_A6  93
#define NOTE_AS6 94
#define NOTE_B6  95
#define NOTE_C7  96
#define NOTE_CS7 97
#define NOTE_D7  98
#define NOTE_DS7 99
#define NOTE_E7  100
#define NOTE_F7  101
#define NOTE_FS7 102
#define NOTE_G7  103
#define NOTE_GS7 104
#define NOTE_A7  105
#define NOTE_AS7 106
#define NOTE_B7  107
#define NOTE_C8  108
#define NOTE_CS8 109
#define NOTE_D8  110
#define NOTE_DS8 111
//That took a while to get all the notes


class MusicWithoutDelay
{
public:
  MusicWithoutDelay(const char *p);
  MusicWithoutDelay();
  MusicWithoutDelay& begin(int mode, int waveForm, int envelope, int mod );
  MusicWithoutDelay& begin(int waveForm, int envelope, int mod);
  MusicWithoutDelay& update();
  MusicWithoutDelay& play();
  MusicWithoutDelay& play(int repeat);
  MusicWithoutDelay& mute(bool m);
  MusicWithoutDelay& pause(bool p);
  MusicWithoutDelay& newSong(const char *p);
  MusicWithoutDelay& setBPM(int tempo);
  MusicWithoutDelay& setMod(int percent);
  MusicWithoutDelay& setVolume(int volume);
  MusicWithoutDelay& setSustain(int v);
  MusicWithoutDelay& setWave(int waveShape);
  MusicWithoutDelay& setFrequency(float freq);
  MusicWithoutDelay& setOctave(int oct);
  MusicWithoutDelay& reverse(bool r);
  MusicWithoutDelay& skipTo(long index);
  MusicWithoutDelay& overrideSustain(bool v);
  // void readIt();
  static float getNoteAsFrequency(int n);
  static void setEngine(synthEngine *engine);

  long getTotalTime();
  long getCurrentTime();
  int getBPM();
  int getOctave();
  char* getName();
  bool isRest();
  bool isMuted();
  bool isStart();
  bool isEnd();
  bool isPaused();
  bool isNote();
  bool isBackwards();
  bool isSustainOverrided();
  bool isSingleNote();

private:
  double skipSolver();
  uint32_t pMillis;      //Yup...Thats a 'long' list of variables. Pun intended :D
  uint32_t oneMillis;
  uint32_t timeBar;
  uint32_t placeHolder1;
  uint32_t placeHolder2;
  uint32_t totalTime;
  uint32_t currentTime;
  uint32_t duration;
  uint16_t wholenote;
  uint16_t loc;
  uint16_t pLoc;
  uint16_t bpm;
  uint8_t default_oct;
  uint8_t default_dur;
  uint8_t num;
  uint8_t slurCount;
  uint8_t note;
  uint8_t scale;
  uint8_t myInstrument;
  uint8_t mRepeat;
  int skipCount;
  bool resume, skip, single, reversed, wasPlaying, wasEnd, oneTime,delayer, rest, slur, start, finish, beat, isMute, sustainControl, flagRepeat, playSingleNote;
  char autoFlat[5];  //you can only have 5 of the black keys ;)
  char autoSharp[5];
  char songName[15];  //make this smaller to get more SRAM
  const char *mySong;
  static synthEngine *noodleSynth;
};
#endif
