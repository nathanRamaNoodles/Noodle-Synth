//documentation found at https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
#include "Arduino.h"
#include "MusicWithoutDelay.h"
int notes[] = { 0,
                NOTE_C1, NOTE_CS1, NOTE_D1, NOTE_DS1, NOTE_E1, NOTE_F1, NOTE_FS1, NOTE_G1, NOTE_GS1, NOTE_A1, NOTE_AS1, NOTE_B1,
                NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2, NOTE_FS2, NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2,
                NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3,
                NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
                NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
                NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
                NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
              };
int powers[] = {1, 10, 100, 1000};
double dur = 0;
bool firstTime;
MusicWithoutDelay::MusicWithoutDelay(char *p) {
  default_dur = 1;  //eqivalent to a wholenote
  default_oct = 4;
  bpm = 100;
  wholenote = (60 * 1000L / bpm) * 4;
  newSong(p);
}
void MusicWithoutDelay:: newSong(char *p) {
  pMillis = 0;
  slurCount = 0;
  num = 0;
  note = 0;
  mySong = p;
  num = 0;
  start = true;
  if (*mySong != ':') {
    memset(songName, 0, 15);
    while (*mySong != ':') {
      if (num < 15) songName[num] = *mySong;
      mySong++;
      num++;
    }
  }
  mySong++;             // skip ':'

  // get default duration
  bool once = true;
  while (*mySong != ':') {
    if (!once)
      mySong++;
    else
      once = false;
    switch (*mySong) {
      case 'd':
        mySong += 2;       // skip "b="
        num = 0;
        while (isDigit(*mySong))
        {
          num = (num * 10) + (*mySong++ - '0');
        }
        if (num > 0) default_dur = num;
        break;
      case 'o':
        mySong += 2;        // skip "o="
        num = *mySong++ - '0';
        if (num >= 1 && num <= 7) default_oct = num;
        break;
      // get BPM
      case 'b':
        mySong += 2;       // skip "b="
        num = 0;
        while (isDigit(*mySong))
        {
          num = (num * 10) + (*mySong++ - '0');
        }
        bpm = num;
        wholenote = (60 * 1000L / bpm) * 4;
        break;
      case 'f':
        mySong += 2;       // skip "f="
        num = 0;
        memset(autoFlat, 0, 5);
        while (isAlpha(*mySong))
        {
          autoFlat[num] = *mySong;
          num++;
          mySong++;
        }
        break;
      case 's':
        mySong += 2;       // skip "s="
        num = 0;
        memset(autoSharp, 0, 5);
        while (isAlpha(*mySong))
        {
          autoSharp[num] = *mySong;
          num++;
          mySong++;
        }
        break;
    }
  }
  loc = strlen(mySong) - 1;
  totalTime = 0;
  skipCount = 0;
  while (skipCount < strlen(mySong)) {
    totalTime += skipSolver();
  }
}
/*void MusicWithoutDelay :: readIt(){
  Serial.print("ddur: "); Serial.println(default_dur, 10);
  Serial.print("doct: "); Serial.println(default_oct, 10);
  Serial.print("bpm: "); Serial.println(bpm, 10);

  for(int i=0; i<strlen(mySong);i++){
    Serial.print(*(mySong+i));
  }
  Serial.println();
  for(int i=strlen(mySong); i>=0;i--){
    Serial.print(*(mySong+i));
  }
  Serial.println();
  Serial.print("Length: ");Serial.println(strlen(mySong));
  }*/
void MusicWithoutDelay:: play(long cM, Tone tone1) {
  //Serial.print("Delayer: ");Serial.println(delayer);
  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)
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
          loc = 0;
          currentTime = 0;
          start = false;
        }
        finish = false;
        if (loc < strlen(mySong))
        {
          if (*(mySong + loc) == ':')
            loc++;
          while (isWhitespace(*(mySong + loc))) {
            loc++;
          }
          // first, get note duration, if available
          num = 0;
          while (isdigit(*(mySong + loc)))
          {
            num = (num * 10) + (*(mySong + loc) - '0');
            loc++;
          }
          if (!skip) {
            if (num)duration = wholenote / num;
            else {
              duration = wholenote / default_dur;
            }
            // now, get optional '.' dotted note
            if (*(mySong + loc) == '.')
            {
              duration += duration / 2;
              loc++;
            }
          }
          else
            skip = false;
          // now get the note
          note = 0;
          for (int i = 0; i < 5; i++) {
            if (*(mySong + loc) == autoFlat[i]) {
              note--;
              break;
            }
            else if (*(mySong + loc) == autoSharp[i]) {
              note++;
              break;
            }
          }
          switch (*(mySong + loc))
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

          // now, get optional '#' sharp
          if (*(mySong + loc) == '#')
          {
            for (int i = 0; i < 5; i++) {
              if (*(mySong + loc - 1) == autoSharp[i]) {
                note--;
                break;
              }
            }
            note++;
            loc++;
          }
          else if (*(mySong + loc) == '_') //for the flats
          {
            for (int i = 0; i < 5; i++) {
              if (*(mySong + loc - 1) == autoFlat[i]) {
                note++;
                break;
              }
            }
            note--;
            loc++;
          }
          if (*(mySong + loc) == '-')
          {
            loc++;
            if (isDigit(*(mySong + loc))) {
              scale = *(mySong + loc) - '0';
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
          else if (isdigit(*(mySong + loc)))
          {
            scale = *(mySong + loc) - '0';
            scale = default_oct + scale;
            if (scale > 7)
              scale = 7;
            loc++;
          }
          else
          {
            scale = default_oct;
          }
          while (isWhitespace(*(mySong + loc))) {
            loc++;
          }
          delayer = true;
          if (*(mySong + loc) == ',') {
            loc++;     // skip comma for next note (or we may be at the end)
            if (slurCount != 0) {
              beat = true;
              slurCount = 0;
            }
            slur = false;
          } else if (*(mySong + loc) == '+') {
            loc++;
            beat = true;
            if (slurCount == 0) {
              beat = false;
            }
            slurCount++;
            slur = true;
          } else {
            if (slurCount != 0) {
              beat = true;
            }
          }
          if (note) {
            tone1.play(notes[(scale - 1) * 12 + note]);
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
          loc = 0;
          beat = false;
          if (start) {
            start = false;
          }
          else
            finish = true;
          tone1.stop();
        }
      }
      else {
        //Serial.print("Rev: ");Serial.println(loc);
        //Serial.print("Delayer: ");Serial.println(delayer);

        // now begin note loop
        if ((!finish && start) || finish) {
          loc = strlen(mySong) - 1;
          currentTime = totalTime;
          finish = false;
        }
        start = false;
        if (loc > 0)
        {
          note = 0;
          while (isWhitespace(*(mySong + loc))) {
            loc--;
          }
          // now, get scale
          if (isdigit(*(mySong + loc)))
          {
            scale = *(mySong + loc) - '0';
            if (*(mySong + loc - 1) == '-') {
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
            if (*(mySong + loc) == autoFlat[i]) {
              note--;
              break;
            }
            else if (*(mySong + loc) == autoSharp[i]) {
              note++;
              break;
            }
          }
          // now, get optional '#' sharp
          if (*(mySong + loc) == '#')
          {
            for (int i = 0; i < 5; i++) {
              if (*(mySong + loc + 1) == autoSharp[i]) {
                note--;
                break;
              }
            }
            note++;
            loc--;
          }
          else if (*(mySong + loc) == '_')
          {
            for (int i = 0; i < 5; i++) {
              if (*(mySong + loc + 1) == autoFlat[i]) {
                note++;
                break;
              }
            }
            note--;
            loc--;
          }

          switch (*(mySong + loc))
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
            if (*(mySong + loc) == '.')
            {
              period = true;
              loc--;
            }
            // first, get note duration, if available
            num = 0;
            if (isDigit(*(mySong + loc)))
            {
              int i = 0;
              while (isdigit(*(mySong + loc)))
              {
                num = ((*(mySong + loc) - '0') * powers[i]) + num ;
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
          while (isWhitespace(*(mySong + loc))) {
            loc--;
          }
          delayer = true;
          if (*(mySong + loc) == ',') {
            loc--;     // skip comma for next note (or we may be at the end)
            if (slurCount != 0) {
              beat = true;
              slurCount = 0;
            }
            slur = false;
          } else if (*(mySong + loc) == '+') {
            loc--;
            beat = true;
            if (slurCount == 0) {
              beat = false;
            }
            slurCount++;
            slur = true;
          } else {
            if (slurCount != 0) {
              beat = true;
            }
          }
          if (note) {
            tone1.play(notes[(scale - 1) * 12 + note]);
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
          loc = strlen(mySong) - 1;
          start = true;
          if (finish) {
            finish = false;
            loc++;
          }
          // stupid minus 1 >:(, took forever to get to the problem
          tone1.stop();
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
          tone1.play(notes[(scale - 1) * 12 + note]);
          wasPlaying = false;
        }
        oneTime = false;
      }
      if (note)
      {
        if (cM - pMillis >= duration) {
          if (!slur) {
            tone1.stop();
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
      if (currentTime < 0)
        currentTime = 0;
    }
  }
  else { //when resume is true
    if (oneTime) {
      placeHolder2 = cM;
      if (tone1.isPlaying())
        wasPlaying = true;
      tone1.stop();
      oneTime = false;
    }
  }
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
void MusicWithoutDelay :: setBPM(int tempo) {
  bpm = tempo;
  if (bpm < 0)
    bpm = 0;
  wholenote = (60 * 1000L / bpm) * 4;
}
void MusicWithoutDelay :: setOctave(int oct) {
  if (oct >= 1 && oct <= 7) default_oct = oct;
}
void MusicWithoutDelay :: pause() {
  resume = !resume;
   if (start||finish||skip) {	  
	placeHolder2 = millis();
	if(!skip)
		duration = placeHolder2 - placeHolder1;
    if(start||finish)
		skipTo(0);
	}

  oneTime = true;
}
double MusicWithoutDelay :: skipSolver() {
  num = 0;
  bool period = false;
  while (!isAlpha(*(mySong + skipCount))) {
	if (*(mySong + skipCount) == '.'){
		period = true;
	}
    skipCount++;
  }
  //we are on a letter note
  int back=0;  //keep track how far away from letter note
  while (!isDigit(*(mySong + skipCount))) { //stop on nearest number to the left
	 if(*(mySong+skipCount)==','||*(mySong+skipCount)=='+'||*(mySong+skipCount)==':')
	  break;
	skipCount--;
	back++;
  }
  //we are on a number
  int j = 0;
  while (isdigit(*(mySong + skipCount)))   //decrypt number backwards
  {
	num = ((*(mySong + skipCount) - '0') * powers[j]) + num ;
	skipCount--;
	back++;
	j++;
  }
  skipCount+= back;//set skipCount to the letter note 
  while(*(mySong+skipCount)!=','&&*(mySong+skipCount)!='+'&& skipCount!=strlen(mySong))
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
void MusicWithoutDelay :: skipTo(long index) {
  timeBar = 0;
  int n = 0;
  skipCount = 0;
  while (skipCount < strlen(mySong)) {
    if (timeBar < index) {
      n = skipCount;
      timeBar += skipSolver();  //adds time
    }
    else {
        while (*(mySong + n) != ',' && *(mySong + n) != '+' && *(mySong + n) != ':' ) {
          n--;
        }        
        n++;
      break;
    }
  }
  if (index >= totalTime || index <= 0) {
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
  if (!resume) {
    return false;
  }
  else
    return true;
}
bool MusicWithoutDelay :: isRest() {
  if (rest) {
    return true;
  }
  else {
    return false;
  }
}
bool MusicWithoutDelay :: isNote() {
  if (beat) {
    return true;
  }
  else {
    return false;
  }
}
void MusicWithoutDelay :: reverse() {
	reversed = !reversed;
	if(firstTime){
		pMillis=millis();
		if (!start && !finish){
			skipTo(currentTime);
		}
		else
			skipTo(0);
		delayer = false;
	}
}
bool MusicWithoutDelay :: isBackwards() {
  if (reversed)
    return true;
  else
    return false;
}
