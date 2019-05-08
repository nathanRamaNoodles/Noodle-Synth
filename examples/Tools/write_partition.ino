/*
   Written by Eric Paquot, 04/2019
   https://github.com/wEPac

   Just a sketch to play some RTTL ringtones on 2 voices (Soprano and Bass),
   and a little helper to compose RTTL string, showing the Music Sheet.

   Playing with the nice library <MusicWithoutDelay.h> by nathanRamaNoodles,
   https://github.com/nathanRamaNoodles/Noodle-Synth, you can really play
   nice little songs to make your arduino alive. However, writing a RTTL file,
   we can easily do few mistakes.

   This sketch is to help our job, to listen our work and to find mistakes,
   but WITHOUT ANY WARRANTY, it must be considered like an example or a draft.
   
*/

////////////////////////////////////////////////////////////////////////////////////
//
//   Run the "Serial Monitor" ("IDE/Tools/Serial Monitor" or "Ctrl+Shift+M")
//   Following instructions from screen, you can :
//      - choose a song #n
//      - play / stop a song
//      - change sound settings, globaly or for a voice
//      - show its partition (music sheet) to check for error when you are composing it
//      - ...
//
//   Enjoy!
//
////////////////////////////////////////////////////////////////////////////////////



#define SERIAL_SPEED        115200
#define SCREEN_SCROLL       80
#define AMOUNT_NOTES_MAX    200         // how many notes to show from a RTTL string (set here depending of the RAM)


//#define USE_AUTOPLAY


#include "Dsongs.h"

//#include <MusicWithoutDelay.h>
#include "MusicWithoutDelay.h"
/*
  // create 2 voices we ll never use, amazingly bad, but this will remove 'clicks'
  MusicWithoutDelay track0a;
  MusicWithoutDelay track0b;
  //*/
MusicWithoutDelay track1;
MusicWithoutDelay track2;
/*
  // create 2 voices we ll never use, amazingly bad, but this will remove 'clicks'
  MusicWithoutDelay track0a;
  MusicWithoutDelay track0b;
  //*/



#define   CHAN            CHA
//#define   CHAN            CHB
// 1 pin is reserved to connect the speaker
//      Board               CHAN A  CHAN B
//      -----------------------------------
//      Uno, Nano            [3]      11
//      Mega                  9       10
//      Teensy 2              A9      -
//      Leonardo, Micro       6       -
#define   PIN_BUZZER      3

#define   PIN_MUTE        4     // optional, for the case we re using an amplifier with mute function



byte      shape[2]        = {0, 0};
const char* SHAPE_NAME[]  = {
  "SINE",     "      ",
  "TRIANGLE", "  ",
  "SQUARE",   "    ",
  "SAW",      "       ",
  "RAMP",     "      ",
  "NOISE",    "     "
};

byte      envelope[2]     = {0, 0};

byte      sustain[2]      = {1, 1};
const char* SUSTAIN_NAME[] = {
  "NONE",     "      ",
  "SUSTAIN",  "   ",
  "REVERSE",  "   "
};


#ifdef USE_AUTOPLAY
bool      autoPlay[2]     = {true, true};
#else
bool      autoPlay[2]     = {false, false};
#endif
bool      playRepeat      = false;
//bool      playRepeat[2]   = {false, false};
bool      playAll         = false;
byte      song2play       = SONG_TABLE_MAX - 1;

byte      playXtimes;     // how many times to repeat a song or songs

word      ledCount;
word      ledTempo;





// =========================================================
// Main
// =========================================================

void setup()
{
  Serial.begin(SERIAL_SPEED);
  while (!Serial);

  pinMode(LED_BUILTIN,    OUTPUT);      // set the led pin
  digitalWrite(PIN_MUTE,  LOW);         // un-mute the amplifier (optional)

  track1.begin(CHAN, shape[0], envelope[0], 0);
  track2.begin(CHAN, shape[1], envelope[1], 0);

  clearScreen();
  printInstructions();

  setSong(0);
  setRepeat();
  setSongPause(3, false);

  //Serial.println(sizeof(SONG_TABLE));
}



void loop()
{
  if (!autoPlay[0]) track1.update();
  if (!autoPlay[1]) track2.update();

  ledBlink();

  commands();

  if (isSongFinished())
  {
    if (playAll)
    {
      song2play = (++song2play) % SONG_TABLE_MAX;
      setSong(0);
      clearScreen();
      printInstructions();
    }

    if (!playRepeat)
    {
      setSongSkip(0);   // keep synch

      //Serial.print("loop (end): sf="); Serial.print(track1.isEnd()); Serial.print(", fb"); Serial.print(track1.isStart()); Serial.print(", ctime"); Serial.println(track1.getCurrentTime());
      if (!playXtimes)
      {
        if (!track1.isPaused())
        {
          //Serial.print("loop (set pause off): sf="); Serial.print(track1.isEnd()); Serial.print(", fb"); Serial.print(track1.isStart()); Serial.print(", ctime"); Serial.println(track1.getCurrentTime());
          setSongPause(3, true);
          //Serial.print("loop (set pause on): sf="); Serial.print(track1.isEnd()); Serial.print(", fb"); Serial.print(track1.isStart()); Serial.print(", ctime"); Serial.println(track1.getCurrentTime());
          clearScreen();
          printInstructions();
        }
      }
      else
      {
        playXtimes--;
        if (autoPlay[0]) track1.update();
        if (autoPlay[1]) track2.update();
      }
    }
    else
    {
      if (autoPlay[0]) track1.update();
      if (autoPlay[1]) track2.update();
    }
  }
}



// =========================================================
// Music
// =========================================================

void setSong(unsigned long skip)
{
  //track1.newSong((byte*)pgm_read_ptr(&SONG_TABLE[song2play][0])).begin(CHAN, shape[0], envelope[0], 0);
  //track2.newSong((byte*)pgm_read_ptr(&SONG_TABLE[song2play][1])).begin(CHAN, shape[1], envelope[1], 0);
  track1.newSong((byte*)pgm_read_ptr(&SONG_TABLE[song2play][0]));
  track2.newSong((byte*)pgm_read_ptr(&SONG_TABLE[song2play][1]));

  setSongAuto(3);

  if (autoPlay[0]) track1.update();
  if (autoPlay[1]) track2.update();

  if (skip) setSongSkip(skip);

  ledTempo = (track1.getBPM()) * 100;
}

bool isSongFinished()
{
  return ((track1.isEnd() || track1.isStart()) && (track2.isEnd() || track2.isStart()));
}

void setSongAuto(byte chan)
{
#ifdef USE_AUTOPLAY
  if (chan & 0x01) track1.autoPlay(autoPlay[0]);
  if (chan & 0x02) track2.autoPlay(autoPlay[1]);
#endif
}

void setSongSkip(unsigned long skip)
{
  track1.skipTo(skip);
  track2.skipTo(skip);
}

void setSongBackward()
{
  byte isBack = track1.isBackwards();
  track1.reverse(!isBack);
  track2.reverse(!isBack);
}

void setSongPause(byte chan, bool isSongPaused)
{
  if (chan & 0x01) track1.pause(isSongPaused);
  if (chan & 0x02) track2.pause(isSongPaused);
}

void setSongShape(byte chan)
{
  if (chan & 0x01) track1.setWave(shape[0]);
  if (chan & 0x02) track2.setWave(shape[1]);
  //track2.setWave(0);
}

void setSongEnvelope(byte chan)
{
  if (chan & 0x01) track1.setEnvelope(envelope[0]);
  if (chan & 0x02) track2.setEnvelope(envelope[1]);
}

void setSongSustain(byte chan)
{
  track1.overrideSustain(true);
  track2.overrideSustain(true);
  if (chan & 0x01) track1.setSustain(sustain[0]);
  if (chan & 0x02) track2.setSustain(sustain[1]);
}

void ledBlink()
{
  ledCount++;
  if      (ledCount == ledTempo)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if (ledCount >= 2 * ledTempo)
  {
    digitalWrite(LED_BUILTIN, LOW);
    ledCount = 0;
  }
}



// =========================================================
// Commands
// =========================================================

byte checkMulti(char str)
{
  byte num = byte(str - '0');
  
  if (byte(str) != 10)
  {
    if (num != 2 && num != 1) num = 0x13;
  }
  else                        num = 0x03;

  return num;
}

void commands()
{
  if (!Serial.available()) return;

  char str        = Serial.read();
  bool multi_com  = true;
  while (multi_com)
  {
    byte num  = byte(str - '0');
    //Serial.print(str, DEC); Serial.print(','); Serial.print(str); Serial.print(','); Serial.println(num);
    delay(50);

    if (num > 0 && num <= 9)
    {
      byte total = num;
      str        = Serial.read();
      num        = byte(str - '0');
      if (str != 10)
      {
        if (num >= 0 || num <= 9)
        {
          total      = total * 10 + num;
          multi_com  = false;
        }
      }
      else multi_com  = false;

      if (total > 0 && total <= SONG_TABLE_MAX)
      {
        song2play = total - 1;
        setSong(0);
        setRepeat();
      }
    }
    else
    {
      switch (str)
      {
        case 'p':
          str        = Serial.read();
          num        = checkMulti(str);
          multi_com  = (num & 0xF0);
          num        = (num & 0x0F);
          
          if (num & 0x01)       setSongPause(num, !track1.isPaused());
          if      (num == 3)    setSongPause(num,  track1.isPaused());
          else if (num & 0x02)  setSongPause(num, !track2.isPaused());
          
          if (isSongFinished()) setRepeat();

          if (autoPlay[0] || autoPlay[1]) setRepeat();
          if (autoPlay[0]) track1.update();
          if (autoPlay[1]) track2.update();

          break;

        case 'o':
          str        = Serial.read();
          num        = checkMulti(str);
          multi_com  = (num & 0xF0);
          num        = (num & 0x0F);

          if (num) num = 3;

#ifdef USE_AUTOPLAY
          if (num & 0x01)       autoPlay[0] = !autoPlay[0];
          if      (num == 3)    autoPlay[1] =  autoPlay[0];
          else if (num & 0x02)  autoPlay[1] = !autoPlay[1];
#endif
          setSongAuto(num);
          break;

        case 'f':
          setSongSkip(track1.getCurrentTime() + ((track1.isBackwards()) ? - 10000L : 10000L));
          multi_com  = false;
          break;

        case 'b':
          setSongBackward();
          multi_com  = false;
          break;

        case 'r':
          playRepeat = !playRepeat;
          setRepeat();
          multi_com  = false;
          break;

        case 'a':
          playAll    = !playAll;
          setRepeat();
          multi_com  = false;
          break;

        case 'w':
          str        = Serial.read();
          num        = checkMulti(str);
          multi_com  = (num & 0xF0);
          num        = (num & 0x0F);
          
          if (num & 0x01)       shape[0] = (++shape[0]) % WAVES_MAX;
          if      (num == 3)    shape[1] = shape[0];
          else if (num & 0x02)  shape[1] = (++shape[1]) % WAVES_MAX;
          setSongShape(num);
          break;

        case 'e':
          str        = Serial.read();
          num        = checkMulti(str);
          multi_com  = (num & 0xF0);
          num        = (num & 0x0F);
          
          if (num & 0x01)       envelope[0] = (++envelope[0]) % ENVELOPES_MAX;
          if      (num == 3)    envelope[1] = envelope[0];
          else if (num & 0x02)  envelope[1] = (++envelope[1]) % ENVELOPES_MAX;
          setSongEnvelope(num);
          break;

        case 's':
          str        = Serial.read();
          num        = checkMulti(str);
          multi_com  = (num & 0xF0);
          num        = (num & 0x0F);
          
          if (num & 0x01)       sustain[0] = (++sustain[0]) % SUSTAINS_MAX;
          if      (num  == 3)   sustain[1] = sustain[0];
          else if (num & 0x02)  sustain[1] = (++sustain[1]) % SUSTAINS_MAX;
          setSongSustain(num);
          break;

        case 'x':
          while (Serial.read() != 10) delay(50);
          showPartition(true, false);
          printInstructions();
          return;
          break;

        case 'y':
          while (Serial.read() != 10) delay(50);
          showPartition(false, false);
          printInstructions();
          return;
          break;

        default:
          return;
      }
    }
  }

  clearScreen();
  printInstructions();
}

void setRepeat()
{
  if (playAll) playXtimes = SONG_TABLE_MAX;
  else         playXtimes = 2;
}



// =========================================================
// Instructions
// =========================================================

void printSettings()
{
  printLine();
  Serial.println(F("Basic settings for instruments :"));
  printInstrName(); Serial.print(F("Instrument     ")); printSetSpace(); Serial.print(F("Wave Shape")); printSetSpace(); Serial.println(F("Envelope"));
  printInstrItem(); Serial.print(F("organ]      ")); printSetSpace(); Serial.print(F("SINE      ")); printSetSpace(); Serial.println(F("0"));
  printInstrItem(); Serial.print(F("piano]      ")); printSetSpace(); Serial.print(F("TRIANGLE  ")); printSetSpace(); Serial.println(F("3"));
  printInstrItem(); Serial.print(F("harpsichord]")); printSetSpace(); Serial.print(F("SQUARE    ")); printSetSpace(); Serial.println(F("0"));
  printInstrItem(); Serial.print(F("trumpet]    ")); printSetSpace(); Serial.print(F("RAMP/SAW  ")); printSetSpace(); Serial.println(F("2"));
  printInstrItem(); Serial.print(F("clarinet]   ")); printSetSpace(); Serial.print(F("SQUARE    ")); printSetSpace(); Serial.println(F("1"));
  printInstrItem(); Serial.print(F("flute]      ")); printSetSpace(); Serial.print(F("TRIANGLE  ")); printSetSpace(); Serial.println(F("1"));
}

void printInstructions()
{
  printSettings();
  printLine();
  Serial.print(F("Selected song : ======= ")); Serial.print(song2play + 1); Serial.println(F(" ======="));
  Serial.println(F("Type :"));
  printInstrItem(); Serial.print(F("a number] to select a song from 1 to ")); Serial.println(SONG_TABLE_MAX);
  printInstrItem(); Serial.print(F("p*] to Play/Pause")); printInstrParam(); Serial.print((track1.isPaused()) ? F("PAUSE1, ") : F("PLAY1,  ")); Serial.println((track2.isPaused()) ? F("PAUSE2") : F("PLAY2"));
#ifdef USE_AUTOPLAY
  printInstrItem(); Serial.print(F("o*] for  AutoPlay")); printInstrParam(); Serial.print((autoPlay[0]) ? F("AUTO1,  ") : F("LOOP1,  ")); Serial.println((autoPlay[1]) ? F("AUTO2") : F("LOOP2"));
#endif
  printInstrItem(); Serial.print(F("f]  to Skip  FORWARD (+10s).")); Serial.println();
  printInstrItem(); Serial.print(F("b]  to Play  BACKWARD")); printInstrValid(track1.isBackwards());
  printInstrItem(); Serial.print(F("r]  to REPEAT song(s)")); printInstrValid(playRepeat);
  printInstrItem(); Serial.print(F("a]  to play ALL songs")); printInstrValid(playAll);
  printInstrItem(); Serial.print(F("w*] to cycle the WAVE shapes (6)")); printInstrParam(); Serial.print(SHAPE_NAME[2 * shape[0]]); Serial.print(F("1, ")); Serial.print(SHAPE_NAME[2 * shape[0] + 1]); Serial.print(SHAPE_NAME[2 * shape[1]]); Serial.println(F("2"));
  printInstrItem(); Serial.print(F("e*] to cycle the ENVELOPES   (4)")); printInstrParam(); Serial.print(F("ENVELOPE1-")); Serial.print(envelope[0]); Serial.print(F(", ENVELOPE2-")); Serial.println(envelope[1]);
  printInstrItem(); Serial.print(F("s*] to cycle the SUSTAINS    (3)")); printInstrParam(); Serial.print(SUSTAIN_NAME[2 * sustain[0]]); Serial.print(F("1, ")); Serial.print(SUSTAIN_NAME[2 * sustain[0] + 1]); Serial.print(SUSTAIN_NAME[2 * sustain[1]]); Serial.println(F("2"));
  printInstrItem(); Serial.print(F("x]  to show Music SHEET of the song")); Serial.println();
  printInstrItem(); Serial.print(F("y]  to show NOTES from the song")); Serial.println();
  printInstrName(); Serial.print(F("(*) add 1 or 2 to select a specific voice")); Serial.println();
  printLine();
}

void clearScreen()     {
  byte num = SCREEN_SCROLL;
  while (num--) Serial.println();
}
void printLine()       {
  Serial.println();
  Serial.println(F("====================================================================="));
}
void printSetSpace()   {
  Serial.print(F("    "));
}
void printInstrName()  {
  Serial.print(F("   "));
}
void printInstrItem()  {
  Serial.print(F("   - ["));
}
void printInstrParam() {
  Serial.print(F(". Current is : "));
}
void printInstrValid(byte isValid) {
  printInstrParam();
  Serial.println((isValid) ? F("ON") : F("OFF"));
}



// =========================================================
// here the big part to show a partition
// ...
// I hate VERBOSE!!!! :-P
// =========================================================

#define     PARTITION_TEXT_LINE_AMOUNT    7

//                            0     1     2     3     4     5     6     7     8     9     10    11    12
const char* alphaNote[]   = {"R ", "c ", "c#", "d ", "d#", "e ", "f ", "f#", "g ", "g#", "a ", "a#", "b "};

//                            1    2    4    8    16    32    64
const char* charNote[]    = {"o", "d", "@", "{", "{{", "{3", "{4"};
#define     CHAR_NOTE_MAX   7

const char*         mySong;
uint16_t            loc;
uint16_t            pLoc;
uint8_t             default_oct;
uint8_t             default_dur;
int8_t              autoFlat[5];   // you can only have 5 of the black keys ;)
int8_t              autoSharp[5];  //
uint32_t            duration;
uint8_t             note;
int8_t              scale;
uint8_t             num;
static byte         notes[AMOUNT_NOTES_MAX][3];
int                 songLen;

void _getCodeNote()
{
  while (isWhitespace(pgm_read_byte_near(mySong + loc))) loc++;

  // first, get note duration, if available
  num = 0;
  while (isdigit(pgm_read_byte_near(mySong + loc)))
  {
    num = (num * 10) + (pgm_read_byte_near(mySong + loc++) - '0');
  }

  if (num)    duration = num << 1;
  else        duration = default_dur;

  // now, get optional '.' dotted note
  if (pgm_read_byte_near(mySong + loc) == '.')
  {
    duration++;
    loc++;
  }


  // now get the notes
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

  // now, end with global '#' sharp and '_' flat
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

    if      (!note)           // take care about fake flat c
    {
      note = 12;
      scale--;
    }
    else if (note > 12)       // take care about fake sharp b
    {
      note = 1;
      scale++;
    }
  }

  if      (scale < 1) scale = 1;
  else if (scale > 7) scale = 7;
}

void showPartition(bool showPartition, bool showAll)
{
  byte start_song = 0;
  byte end_song   = SONG_TABLE_MAX;
  if (!showAll)
  {
    start_song = song2play;
    end_song   = start_song + 1;
  }

  for (byte song_num = start_song; song_num < end_song; song_num++)
  {
    for (byte track_num = 0; track_num < 2; track_num++)
    {
      printLine();
      Serial.print(F("song: ")); Serial.print(song_num + 1);
      Serial.print(F(" / track: ")); Serial.print(track_num + 1); Serial.print(", "); Serial.print((track_num) ? F("Bass") : F("Soprano"));
      if (!showPartition) printLine();


      default_oct     = 4;
      default_dur     = 1 << 1;
      note            = 0;
      mySong          = (byte*)pgm_read_ptr(&SONG_TABLE[song_num][track_num]);
      num             = 0;
      loc             = 0;
      songLen         = strlen_P(mySong);

      //memset(songName, 0, SONG_NAME_LENGTH);
      memset(autoFlat, 0, 5);
      memset(autoSharp, 0, 5);


      // get the name
      if (pgm_read_byte_near(mySong) != ':')
      {
        while (pgm_read_byte_near(mySong + loc) != ':')
        {
          //if (num < SONG_NAME_LENGTH) songName[num] = pgm_read_byte_near(mySong + loc);
          //num++;
          loc++;
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
            loc += 2;       // skip "d="
            num  = 0;
            while (isDigit(pgm_read_byte_near(mySong + loc)))
            {
              num = (num * 10) + (pgm_read_byte_near(mySong + loc++) - '0');
            }
            if (num > 0) default_dur = num << 1;
            break;
          case 'o':
            loc += 2;        // skip "o="
            num  = pgm_read_byte_near(mySong + loc++) - '0';
            if (num >= 1 && num <= 7) default_oct = num;
            break;
          case 'b':
            loc += 2;       // skip "b=" get BPM
            num  = 0;
            while (isDigit(pgm_read_byte_near(mySong + loc)))
            {
              num = (num * 10) + (pgm_read_byte_near(mySong + loc++) - '0');
            }
            //bpm  = num;
            //wholenote = (60 * 1000L / bpm) * 4;
            break;
          case 'f':
            loc += 2;       // skip "f="
            num  = 0;
            while (isAlpha(pgm_read_byte_near(mySong + loc)) && num < 5)
            {
              autoFlat[num++] = (char)pgm_read_byte_near(mySong + loc++);
            }
            break;
          case 's':
            loc += 2;       // skip "s="
            num  = 0;
            while (isAlpha(pgm_read_byte_near(mySong + loc)) && num < 5)
            {
              autoSharp[num++] = (char)pgm_read_byte_near(mySong + loc++);
            }
            break;
        }
      }

      // get all notes
      byte notes_idx  = 0;
      while (loc < strlen_P(mySong) && notes_idx < AMOUNT_NOTES_MAX)
      {
        if (pgm_read_byte_near(mySong + loc) == ':') loc++;



        _getCodeNote();
        while (isWhitespace(pgm_read_byte_near(mySong + loc))) loc++;
        if (pgm_read_byte_near(mySong + loc) == ',') loc++;
        if (pgm_read_byte_near(mySong + loc) == '+') loc++;



        notes[notes_idx][0] = duration;
        notes[notes_idx][1] = note;
        notes[notes_idx][2] = scale;

        if (!showPartition)   // we only show notes
        {
          Serial.print(' ');
          if (notes_idx + 1 < 1000) Serial.print(' ');
          if (notes_idx + 1 <  100) Serial.print(' ');
          if (notes_idx + 1 <   10) Serial.print(' ');
          //Serial.print(F("n"));
          Serial.print(notes_idx + 1);
          Serial.print(F("     oct= ")); Serial.print(notes[notes_idx][2]);
          Serial.print(F("     note= ")); Serial.print(alphaNote[notes[notes_idx][1]]);
          Serial.print(F("     dur= ")); Serial.print(notes[notes_idx][0] >> 1);
          if (notes[notes_idx][0] & 0x01) Serial.print('.');
          Serial.println();
        }
        notes_idx++;
      }


      if (!showPartition) goto endPrint;

      // now show music sheet
      Serial.print(F(", ")); Serial.print(notes_idx); Serial.println(F(" notes"));
      Serial.print(F("R = rest, "));
      for (byte s = 0; s < CHAR_NOTE_MAX; s++)
      {
        Serial.print(charNote[s]);
        Serial.print(F(" = 1/"));
        Serial.print(0x01 << s);
        Serial.print(F(",  "));
        if (s == 2) Serial.println();
      }
      printLine();

      byte l          = PARTITION_TEXT_LINE_AMOUNT * 4;
      byte line_mid   = PARTITION_TEXT_LINE_AMOUNT * 2 + 2;
      byte line_min   = line_mid - 5;
      byte line_max   = line_mid + 5;
      while (l--)
      {
        byte timeLine = 0;
        for (int k = 0; k < notes_idx; k++)
        {
          char noteg      = notes[k][1];
          byte flat_sharp = 0;
          switch (noteg)
          {
            case 2:
              noteg -= 1;
              flat_sharp++;
              break;
            case 3:
              noteg -= 1;
              break;
            case 4:
              noteg -= 2;
              flat_sharp++;
              break;
            case 5:
              noteg -= 2;
              break;
            case 6:
              noteg -= 2;
              break;
            case 7:
              noteg -= 3;
              flat_sharp++;
              break;
            case 8:
              noteg -= 3;
              break;
            case 9:
              noteg -= 4;
              flat_sharp++;
              break;
            case 10:
              noteg -= 4;
              break;
            case 11:
              noteg -= 5;
              flat_sharp++;
              break;
            case 12:
              noteg -= 5;
              break;
          }
          noteg--;

          byte dur        = notes[k][0];
          byte oct        = notes[k][2];
          bool isdrown    = ((l % 2) ? false : true);

          // on which line the note will be
          byte note_line;
          if      (noteg < 0)   // a rest
          {
            note_line = line_mid;
          }
          else if (track_num)   // Bass
          {
            note_line = (noteg + line_mid - 8) + 7 * (1 + oct - default_oct);
          }
          else                  // Soprano
          {
            note_line = (noteg + line_mid - 6) + 7 * (oct - default_oct);
          }

          // draw what will must be for this column
          if (l == note_line)
          {
            drawNote(dur, flat_sharp, noteg, isdrown);
          }
          else
          {
            if      (l < line_min && l < note_line) isdrown = false;
            else if (l > line_max && l > note_line) isdrown = false;
            drawLine(isdrown);
          }

          if (l < line_min || l > line_max) isdrown = false;
          dur = (dur >> 1);
          if (dur < 8) drawLine(isdrown);
          if (dur < 4) drawLine(isdrown);
          Serial.print((isdrown) ? '-' : ' ');


          timeLine += 64 / (dur >> 1);                    // duration is times 2, first bit is for dot
          if (dur & 0x01) timeLine += 64 / (dur & 0xFE);  // doted duration
          if (timeLine >= 2 * 64)
          {
            timeLine = 0;
            drawTimeLine(((l % 2) ? false : true), (l > line_min && l < line_max) ? true : false);
          }

        }
        Serial.println();
      }
endPrint:
      ;
    }
  }
}

void drawNote(byte dur, byte flat_sharp, char noteg, bool isdrown)
{
  //                            1    2    4    8    16    32    64
  //const char* charNote[]  = {"o", "d", "@", "|", "||", "|3", "|4"};
  Serial.print((isdrown) ? '-' : ' ');

  if ((dur >> 1) < 16)  Serial.print((isdrown) ? '-' : ' ');

  if (noteg < 0)
  {
    Serial.print('R');
  }
  else
  {
    if (flat_sharp)       Serial.print((flat_sharp > 0) ? '#' : 'b');
    else                  Serial.print((isdrown) ? '-' : ' ');
  }

  byte i = CHAR_NOTE_MAX;
  byte durb;
  while (i--)
  {
    if ((dur >> (i + 1)) & 0x01)
    {
      Serial.print(charNote[i]);
      durb = dur - (0x01 << (i + 1));
      break;
    }
  }
  //Serial.print(((dur & 0x01) || (dur & 0x01)) ? '.' : ((isdrown) ? '-' : ' '));
  Serial.print((durb) ? '.' : ((isdrown) ? '-' : ' '));

  Serial.print((isdrown) ? '-' : ' ');
}

void drawLine(bool isdrown)
{
  byte i = 6;
  while (i--) Serial.print((isdrown) ? '-' : ' ');
}

void drawTimeLine(bool isdrown, bool isVisible)
{
  Serial.print((isdrown && isVisible) ? '-' : ' ');
  Serial.print(           (isVisible) ? '|' : ' ');
  Serial.print((isdrown && isVisible) ? '-' : ' ');
}





