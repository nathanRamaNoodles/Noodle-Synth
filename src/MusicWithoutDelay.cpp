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
static uint8_t    globalInstrument  = 0;
static const int  notes[]           = {
  0,
  NOTE_C1, NOTE_CS1, NOTE_D1, NOTE_DS1, NOTE_E1, NOTE_F1, NOTE_FS1, NOTE_G1, NOTE_GS1, NOTE_A1, NOTE_AS1, NOTE_B1,
  NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2, NOTE_FS2, NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2,
  NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3,
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
  NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
  NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
  NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
};

double            dur               = 0;
bool              firstTime;



MusicWithoutDelay::MusicWithoutDelay(const char *p)
{
  /*
  default_dur     = 1;  //eqivalent to a wholenote    =====> placed in 'newSong(const char *p)'
  default_oct     = 4;
  bpm             = 100;
  wholenote       = (60 * 1000L / bpm) * 4;
  //*/
  myInstrument    = globalInstrument;
  globalInstrument++;
  edgar.setNumVoices(globalInstrument);
  newSong(p);
}



MusicWithoutDelay::MusicWithoutDelay()
{
  myInstrument    = globalInstrument;
  globalInstrument++;
  edgar.setNumVoices(globalInstrument);
  playSingleNote  = true;
}



MusicWithoutDelay& MusicWithoutDelay::begin(byte mode, byte waveForm, byte envelope, int mod)
{
  edgar.begin(myInstrument, mode);
  edgar.setupVoice(myInstrument, waveForm, 60, envelope, 70, mod + 64);
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::begin(byte waveForm, byte envelope, int mod)
{
  if (myInstrument < 1) edgar.begin(myInstrument, CHA); //default channel
  else                  edgar.begin(myInstrument);
  edgar.setupVoice(myInstrument, waveForm, 60, envelope, 70, mod + 64);
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::newSong(const char *p)
{
  default_dur     = 1;  //equivalent to a wholenote
  default_oct     = 4;
  bpm             = 100;
  wholenote       = (60 * 1000L / bpm) * 4;
  
  playSingleNote  = false;
  start           = true;
  mySong          = p;
  loc             = 0;
  pMillis         = 0;
  slurCount       = 0;
  memset(songName, 0, SONG_NAME_LENGTH);
  memset(autoFlat, 0, 5);
  memset(autoSharp, 0, 5);
  

  // get the name
  if (pgm_read_byte_near(mySong) != ':')
  {
    num = 0;
    while (pgm_read_byte_near(mySong + loc) != ':')
    {
      if (num < SONG_NAME_LENGTH) songName[num] = pgm_read_byte_near(mySong + loc++);
      num++;
    }
  }
  
  loc++;    //skip semicolon

  // get header default values
  while (pgm_read_byte_near(mySong + loc) != ':')
  {
    switch (pgm_read_byte_near(mySong + loc))
    {
      case ',':
        loc++;
        break;
      case 'd':
        loc += 2;       // skip "d="     default duration
        num  = 0;
        while (isDigit(pgm_read_byte_near(mySong + loc)))
        {
          num = (num * 10) + (pgm_read_byte_near(mySong + loc++) - '0');
        }
        if (num > 0) default_dur = num;
        break;
      case 'o':
        loc += 2;       // skip "o="    default octave
        num  = pgm_read_byte_near(mySong + loc++) - '0';
        if (num >= 1 && num <= 7) default_oct = num;
        break;
      case 'b':
        loc += 2;       // skip "b="    default Beat Per Minute
        num  = 0;
        while (isDigit(pgm_read_byte_near(mySong + loc)))
        {
          num = (num * 10) + (pgm_read_byte_near(mySong + loc++) - '0');
        }
        bpm       = num;
        wholenote = (60 * 1000L / bpm) * 4;
        break;
      case 'f':
        loc += 2;       // skip "f="    default flat notes
        num  = 0;
        while (isAlpha(pgm_read_byte_near(mySong + loc)) && num < 5)
        {
          autoFlat[num++] = (char)pgm_read_byte_near(mySong + loc++);
        }
        break;
      case 's':
        loc += 2;       // skip "s="    default sharp notes
        num  = 0;
        while (isAlpha(pgm_read_byte_near(mySong + loc)) && num < 5)
        {
          autoSharp[num++] = (char)pgm_read_byte_near(mySong + loc++);
        }
        break;
    }
  }
  
  loc++;    //skip semicolon
  
  pLoc      = loc;                  // this will point now at the 1st note
  loc       = strlen_P(mySong);
  totalTime = 0;
  skipCount = pLoc;
  while (((unsigned)skipCount) < strlen_P(mySong))
  {
    totalTime += _skipSolver();
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



void MusicWithoutDelay::_getCodeNote()
{
  while (isWhitespace(pgm_read_byte_near(mySong + loc))) loc++;

  // first, get note duration, if available
  num = 0;
  while (isdigit(pgm_read_byte_near(mySong + loc)))
  {
    num = (num * 10) + (pgm_read_byte_near(mySong + loc++) - '0');
  }

  //if (!skip)
  //{
    if (num)    duration = wholenote / num;
    else        duration = wholenote / default_dur;

    // now, get optional '.' dotted note
    if (pgm_read_byte_near(mySong + loc) == '.')
    {
      duration += duration / 2;
      loc++;
    }
  //}
  //else
  //{
  //  skip = false;
  //}

  int set = 0;
  if      (duration >= 1000)  set = 105;
  else if (duration >=  400)  set = 87;
  else if (duration >   300)  set = 82;
  else if (duration >   250)  set = 78;
  else if (duration >   200)  set = 73;
  else                        set = 55;
  edgar.setLength(myInstrument, set);

  // now get the note
  char alphaNote = pgm_read_byte_near(mySong + loc++); // ========> MOD:
  note = 0;
  switch (alphaNote)
  {
    case 'c':
      note = 1;
      break;
    case 'd':
      note = 3;
      break;
    case 'e':
      note = 5;
      break;
    case 'f':
      note = 6;
      break;
    case 'g':
      note = 8;
      break;
    case 'a':
      note = 10;
      break;
    case 'b':
      note = 12;
      break;
  }                                                     // ========> MOD: end

  /*
  if (!skip)
  {
    if (pgm_read_byte_near(mySong + loc) == '.')
    {
      duration += duration / 2;
      loc++;
    }
  }
  //*/

  // now, get optional and default '#' sharp and '_' flat
  int8_t flat_sharp = 0;                    // ========> MOD:
  if      (pgm_read_byte_near(mySong + loc) == '#')
  {
    flat_sharp++;
    loc++;
  }
  else if (pgm_read_byte_near(mySong + loc) == '_')
  {
    flat_sharp--;
    loc++;
  }
  /*
  if (note)
  {
    byte i = 5;
    while (i--)
    {
      if      (alphaNote == autoSharp[i])
      {
        flat_sharp++;
        break;
      }
      else if (alphaNote == autoFlat[i])
      {
        flat_sharp--;
        break;
      }
    }
    note += flat_sharp % 2;   // be sure values are from {-1, 0, 1} 
  }
  //*/

  // now, get octave
  scale = 1;                              // ========> MOD:
  if (pgm_read_byte_near(mySong + loc) == '-')
  {
    loc++;
    scale = -1;
  }
  if (isdigit(pgm_read_byte_near(mySong + loc)))
  {
    scale *= pgm_read_byte_near(mySong + loc++) - '0';
    scale += default_oct;
    //if      (scale < 1) scale = 1;
    //else if (scale > 7) scale = 7;
  }
  else
  {
    scale = default_oct;
  }
  
  if (note)
  {
    byte i = 5;
    while (i--)
    {
      if      (alphaNote == autoSharp[i])
      {
        flat_sharp++;
        break;
      }
      else if (alphaNote == autoFlat[i])
      {
        flat_sharp--;
        break;
      }
    }
    note += flat_sharp % 2;   // be sure values are from {-1, 0, 1} 

    if      (!note)
    {
      note = 12;
      scale--;
    }
    else if (note > 12)
    {
      note = 1;
      scale++;
    }
  }
  if      (scale < 1) scale = 1;
  else if (scale > 7) scale = 7;
}

void MusicWithoutDelay::_setCodeNote()
{
  delayer = true;
  if (!sustainControl) setSustain(SUSTAIN);

  if      (pgm_read_byte_near(mySong + loc) == ',')
  {
    //loc += increment;     // skip comma for next note (or we may be at the end)
    if (slurCount != 0)
    {
      beat      = true;
      slurCount = 0;
    }
    
    slur = false;
  }
  else if (pgm_read_byte_near(mySong + loc) == '+')
  {
    //loc += increment;
    beat = ((slurCount == 0) ? false : true);
    slurCount++;
    slur = true;
    if (!sustainControl) setSustain(NONE);
  }
  else
  {
    if (slurCount != 0) beat = true;
  }

  if (note)
  {
    if (!isMute) edgar.mTrigger(myInstrument, notes[(scale - 1) * 12 + note]);
    beat = !beat;
  }
  else
  {
    rest = true;
  }
}

bool MusicWithoutDelay::_isStartNote(uint16_t pointer)
{
  char test = pgm_read_byte_near(mySong + pointer);
  if (test != ',' && test != '+' && test != ':') return false;
  return true;
}

MusicWithoutDelay&  MusicWithoutDelay::update()
{
  //Serial.print("Delayer: ");Serial.println(delayer);
  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)

  uint32_t cM = millis();
  if (playSingleNote)
  {
    if (!resume && !isMute) edgar.trigger(myInstrument);
  }
  else
  {
    if (flagRepeat)
    {
      if (isEnd())
      {
        mRepeat--;
        play();
        if (mRepeat <= 0)
        {
          flagRepeat = false;
          pause(true);
        }
      }
    }

    firstTime = true;
    if (!resume)
    {
      beat   = false;
      rest   = false;
      single = false;
      if (!delayer)
      {
        if (!reversed)
        {
          // ====== now begin note loop
          if ((finish && !start) || start)
          {
            loc         = pLoc;
            currentTime = 0;
            start       = false;
          }
          finish = false;

          
          if (loc < strlen_P(mySong))
          {
            //if (pgm_read_byte_near(mySong + loc) == ':') loc++; /// no need this, loc now always point at the begining of the note

            _getCodeNote();
            while (isWhitespace(pgm_read_byte_near(mySong + loc))) loc++;

            _setCodeNote();
            loc++;                    // skip comma or + for next note (or we may be at the end)
            
            oneMillis    = cM;
            pMillis      = cM;
            placeHolder1 = cM;
          }
          else
          {
            delayer = false;
            loc     = pLoc;
            beat    = false;
            if (start) start  = false;
            else       finish = true;
            // myTone.stop();
          }
        }
        else
        {
          //Serial.print("Rev: ");Serial.println(loc);
          //Serial.print("Delayer: ");Serial.println(delayer);

          // now begin the note reversed loop
          if ((!finish && start) || finish)
          {
            loc         = strlen_P(mySong) - 1;     // !!!!!!!!!!!!!!!!!! must point to beginin of note
            while (!_isStartNote(loc)) loc--;
            loc++;                  // skip delimiter
            currentTime = totalTime;
            finish      = false;
          }
          start = false;
          
          if (loc > pLoc)
          {
            loc -= 2;               // skip at the end of precedent note

            while (!_isStartNote(loc)) loc--;       // look for the start of precedent note
            loc++;                  // skip delimiter
            
            uint16_t cur_loc = loc; // point at the start of precedent note
            _getCodeNote();
            loc = cur_loc - 1;      // point at ',' or '+' from precedent note

            _setCodeNote();
            
            oneMillis    = cM;
            pMillis      = cM;
            placeHolder1 = cM;
          }
          else
          {
            delayer = false;
            loc     = strlen_P(mySong) - 1;
            start   = true;
            if (finish)
            {
              finish = false;
              loc++;
            }
            // stupid minus 1 >:(, took forever to get to the problem
            // myTone.stop();
          }
        }
      }
      else
      { //when delayer is true
        if (oneTime)
        {
          duration     = duration - (placeHolder2 - placeHolder1);
          placeHolder1 = cM;
          pMillis      = cM;
          oneMillis    = cM;
          if (wasPlaying)
          {
            if (!isMute) edgar.mTrigger(myInstrument, notes[(scale - 1) * 12 + note]);
            wasPlaying = false;
          }
          oneTime = false;
        }
        if (note)
        {
          if (cM - pMillis >= duration)
          {
            if (slur)
            {
              // myTone.stop();
            }
            delayer = false;
            pMillis = cM;
          }
        }
        else
        {
          if (cM - pMillis >= duration)
          {
            delayer = false;
            pMillis = cM;
          }
        }

        if (cM - oneMillis >= 1)
        {
          if (reversed) currentTime--;
          else          currentTime++;
          oneMillis = cM;
        }
        if (currentTime > totalTime) currentTime = totalTime;
      }
    }
    else
    { //when resume is true
      if (oneTime)
      {
        placeHolder2 = cM;
        if (note) wasPlaying = true;
        // myTone.stop();
        oneTime = false;
      }
    }
  }
  return *this;
}



char* MusicWithoutDelay::getName()
{
  return songName;
}



long MusicWithoutDelay::getTotalTime()
{
  return totalTime;   //  returns time in milliseconds
}



long MusicWithoutDelay::getCurrentTime()
{
  return currentTime; //  returns time in milliseconds
}



int MusicWithoutDelay::getBPM()
{
  return bpm;
}



byte MusicWithoutDelay::getOctave()
{
  return default_oct;
}



MusicWithoutDelay& MusicWithoutDelay::setBPM(int tempo)
{
  bpm       = tempo;
  wholenote = (60 * 1000L / bpm) * 4;
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::setOctave(byte oct)
{
  if (oct >= 1 && oct <= 7) default_oct = oct;
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::pause(bool p)
{
  resume = p;
  if (start || finish || skip)
  {
    placeHolder2 = millis();
    if (!skip) duration = placeHolder2 - placeHolder1;
    if (start || finish) skipTo(0);
  }

  oneTime = true;
  return *this;
}



double MusicWithoutDelay::_skipSolver()
{
  while (isWhitespace(pgm_read_byte_near(mySong + skipCount))) skipCount++;
  
  num  = 0;
  while (isDigit(pgm_read_byte_near(mySong + loc)))
  {
    num = (num * 10) + (pgm_read_byte_near(mySong + loc++) - '0');
  }
  
  bool period = false;
  if (pgm_read_byte_near(mySong + skipCount) == '.')  period = true;

  while (!_isStartNote(skipCount) && skipCount < strlen_P(mySong)) skipCount++;
  skipCount++;    // skip delimiter
  
  if (num)    dur = (wholenote / num);
  else        dur = (wholenote / default_dur);

  if (period) dur += dur / 2;
  
  return dur;
}



MusicWithoutDelay& MusicWithoutDelay::skipTo(long index)
{
  timeBar   = 0;
  //int n     = 0;
  //skipCount = pLoc;
  
  if (unsigned(index) >= totalTime || index <= 0)
  {
    //n = 0;
    if (start)      start   = false;
    else            finish  = true;
    
    if (index <= 0) index   = 0;
    else            index   = totalTime;

    loc     = pLoc;
  }
  else
  {
    skipCount = pLoc;
    while (skipCount < strlen_P(mySong))
    {
      //n        = skipCount;
      timeBar += _skipSolver();  //adds time
        
      if (timeBar >= unsigned(index))   // reached time, now, point to the beginin of the note
      {
        skipCount -= 2;   // skip delimiter
        while (!_isStartNote(skipCount)) skipCount--;
        skipCount++;      // skip delimiter
        
        loc = skipCount;
        break;
      }
    }
  }
  
  if (timeBar)  skip = true;
  else          skip = false;

  if (reversed) duration = index - (timeBar - dur);
  else          duration = timeBar - index;
  currentTime = index;
  delayer     = false;
  
  return *this;
}



bool MusicWithoutDelay::isStart()
{
  if (start && !resume)  return true;
  else                   return false;
}



bool MusicWithoutDelay::isEnd()
{
  if (finish && !resume) return true;
  else                   return false;
}



bool MusicWithoutDelay::isPaused()
{
  return resume;
}



bool MusicWithoutDelay::isRest()
{
  return rest;
}



bool MusicWithoutDelay::isNote()
{
  return beat;
}



bool MusicWithoutDelay::isSingleNote()
{
  return playSingleNote;
}



MusicWithoutDelay& MusicWithoutDelay::reverse(bool r)
{
  reversed = r;
  if (firstTime)
  {
    pMillis = millis();
    if (!start && !finish) skipTo(currentTime);
    else                   skipTo(0);
    delayer = false;
  }
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::mute(bool m)
{
  isMute = m;
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::play()
{
  skipTo(0);
  pause(false);
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::play(int i)
{
  play();
  flagRepeat = true;
  mRepeat    = (i < 1) ? 2 : i + 1;
  return *this;
}



float MusicWithoutDelay::getNoteAsFrequency(int n)
{
  return 440 * pow(twelveRoot, (n - 69));
}



MusicWithoutDelay& MusicWithoutDelay::setWave(byte wave)
{
  edgar.setWave(myInstrument, wave);
  return *this;
}



MusicWithoutDelay&  MusicWithoutDelay::setEnvelope(byte env)
{
  edgar.setEnvelope(myInstrument, env);
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::setSustain(int v)
{
  edgar.setSustain(myInstrument, v);
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::overrideSustain(bool v)
{
  sustainControl = v;
  return *this;
}



bool MusicWithoutDelay::isSustainOverrided()
{
  return sustainControl;
}



MusicWithoutDelay& MusicWithoutDelay::setMod( int percent)
{
  edgar.setMod(myInstrument, percent + 64);
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::setFrequency(float freq)
{
  playSingleNote = true;
  edgar.setFrequency(myInstrument, freq);
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::setVolume(int volume)
{
  edgar.setVolume(myInstrument, volume);
  return *this;
}



bool MusicWithoutDelay::isMuted()
{
  return isMute;
}



bool MusicWithoutDelay::isBackwards()
{
  return reversed;
}



