/*
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
#include "Arduino.h"
#include "MusicWithoutDelay.h"
#include "Noodles_Settings.h"
static uint8_t globalInstrument=0;
synthEngine * MusicWithoutDelay::noodleSynth = NULL; //This is extremely important for initiating a static private Object

static const int powers[] = {1, 10, 100, 1000};
double dur = 0;
bool firstTime;
MusicWithoutDelay::MusicWithoutDelay(const char *p) {
  default_dur = 1;  //eqivalent to a wholenote
  default_oct = 4;
  bpm = 100;
  wholenote = (60 * 1000L / bpm) * 4;
  myInstrument=globalInstrument;
  globalInstrument++;
  newSong(p);
}
MusicWithoutDelay::MusicWithoutDelay() {
  myInstrument=globalInstrument;
  globalInstrument++;
  playSingleNote=true;
}
MusicWithoutDelay& MusicWithoutDelay:: begin(int mode, int waveForm, int envelope, int mod){

  noodleSynth->begin(myInstrument, mode);
  noodleSynth->setupVoice(myInstrument,waveForm,60,envelope,70,mod+64);
  return *this;
}
MusicWithoutDelay& MusicWithoutDelay:: begin(int waveForm, int envelope, int mod){
  if(myInstrument<1)
  noodleSynth->begin(myInstrument, CHA); //default channel
  else
  noodleSynth->begin(myInstrument);
  noodleSynth->setupVoice(myInstrument,waveForm,60,envelope,70,mod+64);
  return *this;
}
MusicWithoutDelay& MusicWithoutDelay:: newSong(const char *p) {
  playSingleNote=false;
  pMillis = 0;
  slurCount = 0;
  num = 0;
  note = 0;
  mySong = p;
  num = 0;
  loc=0;
  start = true;
  if (pgm_read_byte_near(mySong)!= ':') {
    memset(songName, 0, 15);
    while (pgm_read_byte_near(mySong+loc) != ':') {
      if (num < 15) songName[num] = pgm_read_byte_near(mySong+loc++);
      num++;
    }
  }
  loc++;//skip semicolon
  // get default duration
  bool once = true;
  while (pgm_read_byte_near(mySong+loc) != ':') {
    if (!once)
    loc++;
    else
    once = false;
    switch (pgm_read_byte_near(mySong+loc)) {
      case 'd':
      loc += 2;       // skip "b="
      num = 0;
      while (isDigit(pgm_read_byte_near(mySong+loc)))
      {
        num = (num * 10) + (pgm_read_byte_near(mySong+loc++) - '0');
      }
      if (num > 0) default_dur = num;
      break;
      case 'o':
      loc += 2;        // skip "o="
      num = pgm_read_byte_near(mySong+loc++) - '0';
      if (num >= 1 && num <= 7) default_oct = num;
      break;
      // get BPM
      case 'b':
      loc += 2;       // skip "b="
      num = 0;
      while (isDigit(pgm_read_byte_near(mySong+loc)))
      {
        num = (num * 10) + (pgm_read_byte_near(mySong+loc++) - '0');
      }
      bpm = num;
      wholenote = (60 * 1000L / bpm) * 4;
      break;
      case 'f':
      loc += 2;       // skip "f="
      num = 0;
      memset(autoFlat, 0, 5);
      while (isAlpha(pgm_read_byte_near(mySong+loc)))
      {
        autoFlat[num] = (char)pgm_read_byte_near(mySong+loc++);
        num++;
      }
      break;
      case 's':
      loc += 2;       // skip "s="
      num = 0;
      memset(autoSharp, 0, 5);
      while (isAlpha(pgm_read_byte_near(mySong+loc)))
      {
        autoSharp[num] = (char)pgm_read_byte_near(mySong+loc++);
        num++;
      }
      break;
    }
  }
  pLoc=loc;
  loc = strlen_P(mySong);
  totalTime = 0;
  skipCount = pLoc;
  while (((unsigned)skipCount) < strlen_P(mySong)) {
    totalTime += skipSolver();
  }
  return *this;
}
// void MusicWithoutDelay :: readIt(){
//   Serial.print("TotalTime: "); Serial.println(totalTime);
//   Serial.print("ddur: "); Serial.println(default_dur, 10);
//   Serial.print("doct: "); Serial.println(default_oct, 10);
//   Serial.print("bpm: "); Serial.println(bpm, 10);
//
//   for(int i=0; i<strlen_P(mySong);i++){
//     Serial.print((char)pgm_read_byte_near(mySong+i));
//   }
//   Serial.println();
//   for(int i=strlen_P(mySong); i>=0;i--){
//     Serial.print((char)pgm_read_byte_near(mySong+i));
//   }
//   Serial.println();
//   Serial.print("Length: ");Serial.println(strlen_P(mySong));
// }
MusicWithoutDelay&  MusicWithoutDelay:: update() {
  //Serial.print("Delayer: ");Serial.println(delayer);
  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)

  // #if defined(ESP32)
  // if(myInstrument == 0)
  // noodleSynth->updateDAC();
  // #endif
  uint32_t cM = millis();
  if(playSingleNote){
    if(!resume&&!isMute)
    noodleSynth->trigger(myInstrument);
  }
  else{
    if(flagRepeat){
      if(isEnd()){
        mRepeat--;
        play();
        if(mRepeat<=0){
          flagRepeat=false;
          pause(true);
        }
      }
    }

    firstTime=true;
    if (!resume) {
      beat = false;
      rest = false;
      single = false;
      if (!delayer) {

        if (!reversed) {
          //Serial.print("Play: ");Serial.println(loc);
          // now begin note loop
          if ((finish && !start) || start) {
            loc = pLoc;
            currentTime = 0;
            start = false;
          }
          finish = false;
          if (loc < strlen_P(mySong))
          {
            if (pgm_read_byte_near(mySong+loc) == ':')
            loc++;
            while (isWhitespace(pgm_read_byte_near(mySong+loc))) {
              loc++;
            }
            // first, get note duration, if available
            num = 0;
            while (isdigit(pgm_read_byte_near(mySong+loc)))
            {
              num = (num * 10) + (pgm_read_byte_near(mySong+loc++) - '0');
            }

            if (!skip) {
              if (num)duration = wholenote / num;
              else {
                duration = wholenote / default_dur;
              }
              // now, get optional '.' dotted note
              if (pgm_read_byte_near(mySong+loc) == '.')
              {
                duration += duration / 2;
                loc++;
              }
            }
            else
            skip = false;

            int set = 0;
            if(duration>=1000){
              set=105;
            }
            else if(duration>=400){
              set=87;
            }
            else if(duration>300){
              set=82;
            }
            else if(duration>250){
              set=78;
            }
            else if(duration>200){
              set=73;
            }
            else{
              set=55;
            }
            noodleSynth->setLength(myInstrument,set);

            // now get the note
            note = 0;
            for (int i = 0; i < 5; i++) {
              if (pgm_read_byte_near(mySong+loc) == autoFlat[i]) {
                note--;
                break;
              }
              else if (pgm_read_byte_near(mySong+loc) == autoSharp[i]) {
                note++;
                break;
              }
            }
            switch (pgm_read_byte_near(mySong+loc))
            {
              case 'c':
              note += 1;
              break;
              case 'd':
              note += 3;
              break;
              case 'e':
              note += 5;
              break;
              case 'f':
              note += 6;
              break;
              case 'g':
              note += 8;
              break;
              case 'a':
              note += 10;
              break;
              case 'b':
              note += 12;
              break;
              case 'p':
              default:
              note = 0;
            }
            loc++;
            if (!skip) {
              if (pgm_read_byte_near(mySong+loc) == '.')
              {
                duration += duration / 2;
                loc++;
              }
            }
            // now, get optional '#' sharp
            if (pgm_read_byte_near(mySong+loc) == '#')
            {
              for (int i = 0; i < 5; i++) {
                if (pgm_read_byte_near(mySong+loc-1) == autoSharp[i]) {
                  note--;
                  break;
                }
              }
              note++;
              loc++;
            }
            else if (pgm_read_byte_near(mySong+loc) == '_') //for the flats
            {
              for (int i = 0; i < 5; i++) {
                if (pgm_read_byte_near(mySong+loc-1) == autoFlat[i]) {
                  note++;
                  break;
                }
              }
              note--;
              loc++;
            }
            if (pgm_read_byte_near(mySong+loc) == '-')
            {
              loc++;
              if (isDigit(pgm_read_byte_near(mySong+loc))) {
                scale = pgm_read_byte_near(mySong+loc) - '0';
                scale = default_oct - scale;
                if (scale < 1)
                scale = 1;
                loc++;
              }
              else {
                scale = default_oct;
              }
            }
            // now, get scale
            else if (isdigit(pgm_read_byte_near(mySong+loc)))
            {
              scale = pgm_read_byte_near(mySong+loc) - '0';
              scale = default_oct + scale;
              if (scale > 7)
              scale = 7;
              loc++;
            }
            else
            {
              scale = default_oct;
            }
            while (isWhitespace(pgm_read_byte_near(mySong+loc))) {
              loc++;
            }
            delayer = true;
            if(!sustainControl)
            setSustain(SUSTAIN);
            if (pgm_read_byte_near(mySong+loc) == ',') {
              loc++;     // skip comma for next note (or we may be at the end)
              if (slurCount != 0) {
                beat = true;
                slurCount = 0;
              }
              slur = false;
            } else if (pgm_read_byte_near(mySong+loc) == '+') {
              loc++;
              beat = true;
              if (slurCount == 0) {
                beat = false;
              }
              slurCount++;
              slur = true;
              if(!sustainControl)
              setSustain(NONE);
            } else {
              if (slurCount != 0) {
                beat = true;
              }
            }
            if (note) {
              if(!isMute)
              noodleSynth->mTrigger(myInstrument, (NOTE_C1 - 1)+((scale - 1) * 12 + note));
              beat = !beat;
            }
            else
            rest = true;
            oneMillis = cM;
            pMillis = cM;
            placeHolder1 = cM;
          }
          else {
            delayer = false;
            loc = pLoc;
            beat = false;
            if (start) {
              start = false;
            }
            else
            finish = true;
            // myTone.stop();
          }
        }
        else {
          //Serial.print("Rev: ");Serial.println(loc);
          //Serial.print("Delayer: ");Serial.println(delayer);

          // now begin note loop
          if ((!finish && start) || finish) {
            loc = strlen_P(mySong) - 1;
            currentTime = totalTime;
            finish = false;
          }
          start = false;
          if (loc > pLoc)
          {
            note = 0;
            while (isWhitespace(pgm_read_byte_near(mySong+loc))) {
              loc--;
            }
            // now, get scale
            if (isdigit(pgm_read_byte_near(mySong+loc)))
            {
              scale = pgm_read_byte_near(mySong+loc) - '0';
              if (pgm_read_byte_near(mySong+loc-1) == '-') {
                loc--;
                scale *= -1;
              }
              scale = default_oct + scale;
              if (scale > 7)
              scale = 7;
              else if (scale < 1)
              scale = 1;
              loc--;
            }
            else
            {
              scale = default_oct;
            }


            for (int i = 0; i < 5; i++) {
              if (pgm_read_byte_near(mySong+loc) == autoFlat[i]) {
                note--;
                break;
              }
              else if (pgm_read_byte_near(mySong+loc) == autoSharp[i]) {
                note++;
                break;
              }
            }
            // now, get optional '#' sharp
            if (pgm_read_byte_near(mySong+loc) == '#')
            {
              for (int i = 0; i < 5; i++) {
                if (pgm_read_byte_near(mySong+loc+1) == autoSharp[i]) {
                  note--;
                  break;
                }
              }
              note++;
              loc--;
            }
            else if (pgm_read_byte_near(mySong+loc) == '_')
            {
              for (int i = 0; i < 5; i++) {
                if (pgm_read_byte_near(mySong+loc+1) == autoFlat[i]) {
                  note++;
                  break;
                }
              }
              note--;
              loc--;
            }

            switch (pgm_read_byte_near(mySong+loc))
            {
              case 'c':
              note += 1;
              break;
              case 'd':
              note += 3;
              break;
              case 'e':
              note += 5;
              break;
              case 'f':
              note += 6;
              break;
              case 'g':
              note += 8;
              break;
              case 'a':
              note += 10;
              break;
              case 'b':
              note += 12;
              break;
              case 'p':
              default:
              note = 0;

            }
            loc--;
            bool period = false;
            // now, get optional '.' dotted note
            if (!skip) {
              if (pgm_read_byte_near(mySong+loc) == '.')
              {
                period = true;
                loc--;
              }
              // first, get note duration, if available
              num = 0;
              if (isDigit(pgm_read_byte_near(mySong+loc)))
              {
                int i = 0;
                while (isdigit(pgm_read_byte_near(mySong+loc)))
                {
                  num = ((pgm_read_byte_near(mySong+loc) - '0') * powers[i]) + num ;
                  loc--;
                  i++;
                }
              }
              if (num)duration = wholenote / num;
              else duration = wholenote / default_dur;
              if (period)
              duration += duration / 2;
            }
            else
            skip = false;

            int set = 0;
            if(duration>=1000){
              set=105;
            }
            else if(duration>=400){
              set=87;
            }
            else if(duration>300){
              set=82;
            }
            else if(duration>250){
              set=78;
            }
            else if(duration>200){
              set=73;
            }
            else{
              set=55;
            }
            noodleSynth->setLength(myInstrument,set);


            while (isWhitespace(pgm_read_byte_near(mySong+loc))) {
              loc--;
            }
            delayer = true;
            if(!sustainControl)
            setSustain(SUSTAIN);
            if (pgm_read_byte_near(mySong+loc) == ',') {
              loc--;     // skip comma for next note (or we may be at the end)
              if (slurCount != 0) {
                beat = true;
                slurCount = 0;
              }
              slur = false;
            } else if (pgm_read_byte_near(mySong+loc) == '+') {
              loc--;
              beat = true;
              if (slurCount == 0) {
                beat = false;
              }
              slurCount++;
              slur = true;
              if(!sustainControl)
              setSustain(NONE);
            } else {
              if (slurCount != 0) {
                beat = true;
              }
            }
            if (note) {
              if(!isMute)
              noodleSynth->mTrigger(myInstrument,(NOTE_C1 - 1)+((scale - 1) * 12 + note));
              beat = !beat;
            }
            else
            rest = true;

            pMillis = cM;
            oneMillis = cM;
            placeHolder1 = cM;
          }
          else {
            delayer = false;
            loc = strlen_P(mySong) - 1;
            start = true;
            if (finish) {
              finish = false;
              loc++;
            }
            // stupid minus 1 >:(, took forever to get to the problem
            // myTone.stop();
          }
        }
      }
      else {  //when delayer is true
        if (oneTime) {
          duration = duration - (placeHolder2 - placeHolder1);
          placeHolder1 = cM;
          pMillis = cM;
          oneMillis = cM;
          if (wasPlaying) {
            if(!isMute)
            noodleSynth->mTrigger(myInstrument,(NOTE_C1 - 1)+((scale - 1) * 12 + note));
            wasPlaying = false;
          }
          oneTime = false;
        }
        if (note)
        {
          if (cM - pMillis >= duration) {
            if (slur) {
              // myTone.stop();
            }
            delayer = false;
            pMillis = cM;
          }
        }
        else
        {
          if (cM - pMillis >= duration) {
            delayer = false;
            pMillis = cM;
          }
        }
        if (cM - oneMillis >= 1) {
          if (reversed)
          currentTime--;
          else
          currentTime++;
          oneMillis = cM;
        }
        if (currentTime > totalTime)
        currentTime = totalTime;
      }
    }
    else { //when resume is true
      if (oneTime) {
        placeHolder2 = cM;
        if (note)
        wasPlaying = true;
        // myTone.stop();
        oneTime = false;
      }
    }
  }
  return *this;
}
char* MusicWithoutDelay:: getName() {
  return songName;
}
long MusicWithoutDelay :: getTotalTime() {
  return totalTime; //  returns time in milliseconds
}
long MusicWithoutDelay :: getCurrentTime() {
  return currentTime; //  returns time in milliseconds
}
int MusicWithoutDelay :: getBPM() {
  return bpm;
}
int MusicWithoutDelay :: getOctave() {
  return default_oct;
}
MusicWithoutDelay& MusicWithoutDelay :: setBPM(int tempo) {
  bpm = tempo;
  wholenote = (60 * 1000L / bpm) * 4;
  return *this;
}
MusicWithoutDelay& MusicWithoutDelay :: setOctave(int oct) {
  if (oct >= 1 && oct <= 7) default_oct = oct;
  return *this;
}
MusicWithoutDelay& MusicWithoutDelay :: pause(bool p) {
  resume = p;
  if (start||finish||skip) {
    placeHolder2 = millis();
    if(!skip)
    duration = placeHolder2 - placeHolder1;
    if(start||finish)
    skipTo(0);
  }

  oneTime = true;
  return *this;
}
double MusicWithoutDelay :: skipSolver() {
  num = 0;
  bool period = false;
  while (!isAlpha(pgm_read_byte_near(mySong+skipCount))) {
    if (pgm_read_byte_near(mySong+skipCount) == '.'){
      period = true;
    }
    skipCount++;
  }
  //we are on a letter note
  int back=0;  //keep track how far away from letter note
  while (!isDigit(pgm_read_byte_near(mySong+skipCount))) { //stop on nearest number to the left
    char compare = pgm_read_byte_near(mySong+skipCount);
    if(compare==','||compare=='+'||compare==':')
    break;
    skipCount--;
    back++;
  }
  //we are on a number
  int j = 0;
  while (isdigit(pgm_read_byte_near(mySong+skipCount)))   //decrypt number backwards
  {
    num = ((pgm_read_byte_near(mySong+skipCount--) - '0') * powers[j]) + num ;
    back++;
    j++;
  }
  skipCount+= back;//set skipCount to the letter note
  while(pgm_read_byte_near(mySong+skipCount)!=','&&pgm_read_byte_near(mySong+skipCount)!='+'&& pgm_read_byte_near(mySong+skipCount)!=strlen_P(mySong))
  skipCount++;
  dur = 0;
  //Serial.print(num);
  if (num)
  dur = (wholenote / num);
  else
  dur = (wholenote / default_dur);
  if (period) {
    dur += dur / 2;
    //Serial.print(".");
  }
  //Serial.println();
  return dur;
}
MusicWithoutDelay& MusicWithoutDelay :: skipTo(long index) {
  timeBar = 0;
  int n = 0;
  skipCount = pLoc;
  while (((unsigned)skipCount) < strlen_P(mySong)) {
    if (timeBar < ((unsigned)index)) {
      n = skipCount;
      timeBar += skipSolver();  //adds time
    }
    else {
      while (pgm_read_byte_near(mySong+n) != ',' && pgm_read_byte_near(mySong+n) != '+' && pgm_read_byte_near(mySong+n) != ':' ) {
        n--;
      }
      n++;
      break;
    }
  }
  if (((unsigned)index) >= totalTime || index <= 0) {
    n = 0;
    if (start) {
      start = false;
    }
    else
    finish = true;
    if (index <= 0)
    index = 0;
    else
    index = totalTime;
  }
  if (n != 0)
  skip = true;
  else
  skip = false;

  loc = n;
  if(reversed) duration = index-(timeBar-dur);
  else   duration = timeBar-index;
  currentTime = index;
  delayer = false;
  return *this;
}
bool MusicWithoutDelay :: isStart() {
  if (start && !resume) {
    return true;
  }
  else
  return false;
}
bool MusicWithoutDelay :: isEnd() {
  if (finish && !resume) {
    return true;
  }
  else
  return false;
}
bool MusicWithoutDelay :: isPaused() {
  return resume;
}
bool MusicWithoutDelay :: isRest() {
  return rest;
}
bool MusicWithoutDelay :: isNote() {
  return beat;
}
bool MusicWithoutDelay :: isSingleNote(){
  return playSingleNote;
}
MusicWithoutDelay& MusicWithoutDelay :: reverse(bool r) {
  reversed = r;
  if(firstTime){
    pMillis=millis();
    if (!start && !finish){
      skipTo(currentTime);
    }
    else
    skipTo(0);
    delayer = false;
  }
  return *this;
}
MusicWithoutDelay& MusicWithoutDelay:: mute(bool m){
  isMute=m;
  return *this;
}
MusicWithoutDelay& MusicWithoutDelay::play(){
  skipTo(0);
  pause(false);
  return *this;
}
MusicWithoutDelay& MusicWithoutDelay::play(int i){
  play();
  flagRepeat=true;
  mRepeat = (i<1)?2:i+1;
  return *this;
}
float MusicWithoutDelay::getNoteAsFrequency(int n){
  return 440 * pow(twelveRoot, (n - 69));
}
MusicWithoutDelay& MusicWithoutDelay::setWave( int waveShape){
  noodleSynth->setWave( myInstrument,waveShape);
  return *this;
}
MusicWithoutDelay& MusicWithoutDelay::setSustain(int v){
  noodleSynth->setSustain(myInstrument, v);
  return *this;
}
MusicWithoutDelay& MusicWithoutDelay::overrideSustain(bool v){
  sustainControl = v;
  return *this;
}
bool MusicWithoutDelay::isSustainOverrided(){
  return sustainControl;
}
MusicWithoutDelay& MusicWithoutDelay::setMod( int percent){
  noodleSynth->setMod(myInstrument,percent+64);
  return *this;
}
MusicWithoutDelay& MusicWithoutDelay::setFrequency(float freq){
  playSingleNote=true;
  noodleSynth->setFrequency(myInstrument,freq);
  return *this;
}
MusicWithoutDelay& MusicWithoutDelay::setVolume(int volume){
  noodleSynth->setVolume(myInstrument,volume);
  return *this;
}

bool MusicWithoutDelay::isMuted(){
  return isMute;
}
bool MusicWithoutDelay :: isBackwards() {
  return reversed;
}
void MusicWithoutDelay::setEngine(synthEngine *engine){
  noodleSynth = engine;
}
