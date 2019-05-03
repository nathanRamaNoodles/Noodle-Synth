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



static uint8_t    globalInstrument  = 0;    // used to count voices we are using
/*
static const int  notes[]           = {       // no need this table, can be easy computed
  0,    // start at '23', then inc by 1
  NOTE_C1, NOTE_CS1, NOTE_D1, NOTE_DS1, NOTE_E1, NOTE_F1, NOTE_FS1, NOTE_G1, NOTE_GS1, NOTE_A1, NOTE_AS1, NOTE_B1,
  NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2, NOTE_FS2, NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2,
  NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3,
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
  NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
  NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
  NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
};
//*/

bool              firstTime;                // can be replaced by 'start' ???????????????????????



void autoUpdate(uint8_t voice)
{
  ;
}


MusicWithoutDelay::MusicWithoutDelay(const char *p)
{
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
  sustain = SUSTAIN;
  
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::begin(byte waveForm, byte envelope, int mod)
{
  if (myInstrument < 1) edgar.begin(myInstrument, CHA); //default channel
  else                  edgar.begin(myInstrument);
  edgar.setupVoice(myInstrument, waveForm, 60, envelope, 70, mod + 64);
  sustain = SUSTAIN;
  
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::newSong(const char *p)
{
  default_dur     = 1;  //equivalent to a wholenote
  default_oct     = 4;
  bpm             = 100;
  
  playSingleNote  = false;
  start           = true;
  mySong          = p;
  loc             = 0;
  //pMillis         = 0;
  memset(songName,  0, SONG_NAME_LENGTH);
  memset(autoFlat,  0, 5);
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

  // duration reference for a whole note
  wholenote = (60 * 1000L / bpm) * 4;

  // pointer to 1st note
  pLoc      = loc + 1;                  

  // pointer to last note
  loc       = strlen_P(mySong) - 1;
  while (!_isNoteDelimiter(loc)) loc--;
  pEndLoc   = loc + 1;              

  // totalTime of this song
  totalTime = 0;
  skipCount = pLoc;
  while (skipCount <= pEndLoc) totalTime += _skipSolver();

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

  if (!skip)    // prevent overlapping of notes and keep synch
  {
    if (num)    duration = wholenote / num;
    else        duration = wholenote / default_dur;
  }
  
  // now, get optional '.' dotted note
  if (pgm_read_byte_near(mySong + loc) == '.')
  {
    if (!skip)  duration += duration / 2;
    loc++;
  }
  skip = false;
  
  //else
  //{
  //  skip = false;
  //}

  // now get the note
  char alphaNote = pgm_read_byte_near(mySong + loc++);
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
  }

  // now, get optional '#' sharp and '_' flat
  int8_t flat_sharp = 0;
  if      (pgm_read_byte_near(mySong + loc) == '#')
  {
    loc++;
    flat_sharp++;
  }
  else if (pgm_read_byte_near(mySong + loc) == '_')
  {
    loc++;
    flat_sharp--;
  }

  // now, get octave
  scale = 1;
  if (pgm_read_byte_near(mySong + loc) == '-')
  {
    loc++;
    scale = -1;
  }
  if (isdigit(pgm_read_byte_near(mySong + loc)))
  {
    scale *= pgm_read_byte_near(mySong + loc++) - '0';
    scale += default_oct;
  }
  else
  {
    scale = default_oct;
  }

  // now, end with global '#' sharp and '_' flat, and adjust octave(scale) if needed
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

    if      (!note)           // we got an abusive flat c 
    {
      note = 12;
      scale--;
    }
    else if (note > 12)       // we got an abusive sharp b
    {
      note = 1;
      scale++;
    }
  }
  if      (scale < 1) scale = 1;
  else if (scale > 7) scale = 7;
}

void MusicWithoutDelay::_setCodeNote(uint16_t slur_loc)
{
  byte note_dur;
  if      (duration >= 1000)  note_dur  = 105;
  else if (duration >=  400)  note_dur  = 87;
  else if (duration >=  300)  note_dur  = 82;
  else if (duration >=  250)  note_dur  = 78;
  else if (duration >=  200)  note_dur  = 73;
  else                        note_dur  = 55;
  
  byte note_sustain;
  if (pgm_read_byte_near(mySong + slur_loc) == '+')   // a slur note, "+"
  {
    note_dur     = 127;                                   // set length at max to prevent a cut
    note_sustain = NONE;                                  // slur note is always NO sustain
  }
  else                                                // a ',' or ':' or NOTHING
  {
    note_sustain = (sustainControl) ? sustain : SUSTAIN;  // user did not define sustain, we use the default sustain
  }
  
  edgar.setLength(myInstrument, note_dur);
  edgar.setSustain(myInstrument, note_sustain);
  
  if (note)
  {
    //if (!isMute) edgar.mTrigger(myInstrument, notes[(scale - 1) * 12 + note]);
    if (!isMute) edgar.mTrigger(myInstrument, 11 + note + 12 * scale);  // start at 'NOTE_C1':24
    beat = true;
  }
  else
  {
    beat = false;
  }
}

bool MusicWithoutDelay::_isNoteDelimiter(uint16_t pointer)
{
  char test = pgm_read_byte_near(mySong + pointer);
  if (test != ',' && test != '+' && test != ':') return false;
  
  return true;
}

MusicWithoutDelay&  MusicWithoutDelay::update()
{
  // !!!!!! here we will fix a rule about note pointer, pointer will alway be
  //    at the begining of the next note to play (for playing reverse as well)
  //    so, just after a note delimiter.
  
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

    firstTime = true;   // use start is better, here ????????????????????????????????????????????????????????????????????
    
    if (resume) // ================> in pause, resume is true
    {
      if (oneTime)                      // resume, is ON for the 1st time
      {
        placeHolder2          = cM;
        if (note) wasPlaying  = true;
        oneTime               = false;
      }
    }
    else        // ================> not in pause, playing
    {
      if (delayer)        // ================> still playing a note, delayer is true
      {
        if (oneTime)                    // play/pause, not resume, is ON for the 1st time
        {
          duration      = duration - (placeHolder2 - placeHolder1);
          placeHolder1  = cM;
          pMillis       = cM;
          oneMillis     = cM;
          
          if (wasPlaying)
          {
            //if (!isMute) edgar.mTrigger(myInstrument, notes[(scale - 1) * 12 + note]);
            if (!isMute) edgar.mTrigger(myInstrument, 11 + note + 12 * scale);  // start at 'NOTE_C1':24
            wasPlaying  = false;
          }
          oneTime     = false;
        }
        
        if (cM - pMillis >= duration)   // note has been played, delayer => false
        {
          delayer     = false;
          pMillis     = cM;
        }

        if (cM - oneMillis >= 1)        // keep currentTime accurate
        {
          if      ( reversed && currentTime)             currentTime--;
          else if (!reversed && currentTime < totalTime) currentTime++;
          oneMillis   = cM;
        }
      }
      else                // ================> waiting to play a new note, delayer is false
      {
        delayer = true;           // new note to be play, delayer will stay true as long as to play it
        
        if (finish || start)      // song start again, reset values
        {
          finish        = true;
          start         = false;
          
          skip          = false;
          pMillis       = 0;
          setSustain(sustain);
        }
        
        if (reversed)             // ================> playing backward
        {
          // ====== now begin the note reversed loop
          // reset pointers
          if (finish)
          {
            finish        = false;
            loc           = pEndLoc;                // must point at the begining of final note
            currentTime   = totalTime;
          }

          // ====== look further for next note to play
          if (loc >= pLoc)
          {
            skipCount     = loc - 1;                // pointer at the precedent delimiter ':', ',' or '+', save this
            _getCodeNote();

            if (sustainControl && sustain == REV_SUSTAIN)
            {                                       // pointer at the current delimiter ':', ',' or '+', save this
              while (isWhitespace(pgm_read_byte_near(mySong + loc))) loc++;
            }
            else
            {
              loc           = skipCount;
            }
            _setCodeNote(loc);
            
            loc           = --skipCount;            // set pointer at the end of the precedent note
            while (!_isNoteDelimiter(loc)) loc--;   // look for the start of precedent note
            loc++;                                  // skip delimiter
          }
          else      // ====== reach start of the song, set flags, next update will reset pointers
          {
            finish = true;
          }
        }
        else                      // ================> playing forward
        {
          // ====== now begin note loop
          // reset pointers
          if (finish)
          {
            loc           = pLoc;
            currentTime   = 0;
            finish        = false;
          }

          // ====== look further for next note to play
          if (loc <= pEndLoc)
          { 
            skipCount     = loc - 1;                // pointer at the precedent delimiter ':', ',' or '+', save this
            _getCodeNote();

            while (isWhitespace(pgm_read_byte_near(mySong + loc))) loc++;
            if (!sustainControl || !sustain == REV_SUSTAIN)
            {
              skipCount     = loc;                  // pointer at the current delimiter ':', ',' or '+', save this
            }
            _setCodeNote(skipCount);
            
            loc++;                                  // look for the start of next note
          }
          else    // ====== reach end of the song, set flags, next update will reset pointers
          {
            finish        = true;
          }

          if (!finish)
          {
            oneMillis     = cM;
            pMillis       = cM;
            placeHolder1  = cM;
          }
        }
      }
    }
  }
  
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::mute(bool m)
{
  isMute = m;
  
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::pause(bool p)
{
  resume = p;
  
  if (start || finish || skip)
  {
    placeHolder2 = millis();
    if (!skip)            duration = placeHolder2 - placeHolder1;
    if (start || finish)  skipTo(0);
  }

  oneTime = true;

  return *this;
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



double MusicWithoutDelay::_skipSolver()
{
  // just a way to read duration for the note than 'skipCount' will point
  // 'skipCount' will be incremented then, to the next note

  double dur;
  while (isWhitespace(pgm_read_byte_near(mySong + skipCount))) skipCount++;
  
  num  = 0;
  while (isDigit(pgm_read_byte_near(mySong + skipCount)))
  {
    num = (num * 10) + (pgm_read_byte_near(mySong + skipCount++) - '0');
  }
  
  bool period = false;
  if (pgm_read_byte_near(mySong + skipCount) == '.')  period = true;

  while (!_isNoteDelimiter(skipCount) && skipCount <= pEndLoc) skipCount++;
  skipCount++;    // skip delimiter
  
  if (num)    dur = (wholenote / num);
  else        dur = (wholenote / default_dur);

  if (period) dur += dur / 2;
  
  return dur;
}



MusicWithoutDelay& MusicWithoutDelay::skipTo(long index)
{
  double    timeBar = 0;
  double    dur;
  
  if (index <= 0 || unsigned(index) >= totalTime) // out of range
  {
    if (!start)     finish  = true;
    
    if (index <= 0) index   = 0;
    else            index   = totalTime;

    //if (reversed)   loc     = pEndLoc;
    //else            loc     = pLoc;
  }
  else                                            // in range, check for the nearest note
  {
    skipCount = pLoc;
    while (skipCount <= pEndLoc)
    {
      dur      = _skipSolver();
      timeBar += dur;                   //adds time
        
      if (timeBar >= unsigned(index))   // reached time, now, point to the begining of the note
      {
        skipCount -= 2;   // skip delimiter, go back to the end of concerned note
        while (!_isNoteDelimiter(skipCount)) skipCount--;
        skipCount++;      // skip delimiter, point at the begining of concerned note
        
        timeBar -= dur;
        loc      = skipCount;
        break;
      }
    }
  }

  if (reversed) duration = index - timeBar;
  else          duration = timeBar - index;
    
  if (timeBar)  skip = true;
  else          skip = false;

  currentTime = index;
  delayer     = false;
  
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



MusicWithoutDelay& MusicWithoutDelay::overrideSustain(bool value)
{
  sustainControl = value;
  
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::setSustain(byte value)
{
  sustain         = value;
  edgar.setSustain(myInstrument, value);
  
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::setOctave(byte oct)
{
  if (oct >= 1 && oct <= 7) default_oct = oct;
  
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::setBPM(int tempo)
{
  bpm       = tempo;
  wholenote = (60 * 1000L / bpm) * 4;
  
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::setMod(int percent)
{
  edgar.setMod(myInstrument, percent + 64);
  
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::setVolume(byte volume)
{
  edgar.setVolume(myInstrument, volume);
  
  return *this;
}



MusicWithoutDelay& MusicWithoutDelay::setFrequency(float freq)
{
  playSingleNote = true;
  edgar.setFrequency(myInstrument, freq);
  
  return *this;
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



float MusicWithoutDelay::getNoteAsFrequency(byte n)
{
  return 440 * pow(twelveRoot, (n - 69));
}



char* MusicWithoutDelay::getName()
{
  return songName;
}



long MusicWithoutDelay::getTotalTime()
{
  return totalTime;     // returns time in milliseconds
}



long MusicWithoutDelay::getCurrentTime()
{
  return currentTime;   // returns time in milliseconds
}



byte MusicWithoutDelay::getOctave()
{
  return default_oct;
}



int MusicWithoutDelay::getBPM()
{
  return bpm;
}



byte MusicWithoutDelay::getSustain()
{
  return sustain;
}



bool MusicWithoutDelay::isSustainOverrided()
{
  return sustainControl;
}



bool MusicWithoutDelay::isSingleNote()
{
  return playSingleNote;
}



bool MusicWithoutDelay::isMuted()
{
  return isMute;
}



bool MusicWithoutDelay::isPaused()
{
  return resume;
}



bool MusicWithoutDelay::isBackwards()
{
  return reversed;
}



bool MusicWithoutDelay::isNote()
{
  return (beat && !resume);
}



bool MusicWithoutDelay::isRest()
{
  return (!beat && !resume);
}



bool MusicWithoutDelay::isStart()
{
  //if (start && !resume)  return true;
  if (reversed && finish && !resume)  return true;
  else                                return false;
}



bool MusicWithoutDelay::isEnd()
{
  //if (finish && !resume) return true;
  if (!reversed && finish && !resume) return true;
  else                                return false;
}



