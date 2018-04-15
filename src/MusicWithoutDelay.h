/*
  MusicWithoutDelay.h - Library for playing music at multiple pins without Using Delay. Also uses the Tone library.
  https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
  MusicWithoutDelay Created by Nathan Ramanathan(nathan6ramanathan@gmail.com), March 21, 2018
  The Tone Library was created by David A. Mellis on November 2, 2007.
  Released into the public domain.
*/

#ifndef MusicWithoutDelay_h
#define MusicWithoutDelay_h

#include "Arduino.h"
#include <Tone.h>
              
class MusicWithoutDelay
{
  public:
    MusicWithoutDelay(char *p);
	void play(long cM, Tone tone1);
	void pause();
	void newSong(char *p);
	void setBPM(int tempo);
	void setOctave(int oct);
	void reverse();
	void skipTo(long index);
	//void readIt();
	long getTotalTime();
	long getCurrentTime();
	int getBPM();
	int getOctave();
	char* getName();
	bool isRest();
	bool isStart();
	bool isEnd();
	bool isPaused();
	bool isNote();
	bool isBackwards();
	
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
    uint16_t bpm;
    uint8_t default_oct;
    uint8_t default_dur;
    uint8_t num;
    uint8_t slurCount;
    uint8_t note;
    uint8_t scale; 
    int skipCount;  
    bool resume;
    bool skip;
    bool single;
    bool reversed;
    bool wasPlaying;
    bool wasEnd;
    bool oneTime; 
    bool delayer;
    bool rest;
    bool slur;
    bool start;
    bool finish;
    bool beat;
    char autoFlat[5];  //you can only have 5 of the black keys ;)
    char autoSharp[5];
    char songName[15];  //make this smaller to get more SRAM
    char *mySong;
};
#endif
