/*Example for the MusicWithoutDelay Library by Nathan Ramanathan. nathan6ramanathan@gmail.com
   This advanced sketch plays The Legend of Zelda(Main theme).(It can play more than one note at the same time)
   Please consider that the number of notes played at the same time depends on the number of timers on your Arduino.
   In this case, you can play a max of 2 notes at the same time on the Arduino Uno.
   Obviously, you can do other things while the song is being played.
   For example, talk to Serial, read buttons, display images on an OLED screen, etc...
  ////////Directions
   Connect two buttons, one to pin 2 and the other to pin 3.
   Pressing one button pauses te song, and another button skips the song to a specific section in each scene. There are a total of 5 scenes. I made scenes in order to save Arduino Memory.
   Music order: intro->same->middle->same->end (repeat last steps except intro)
*/
//To learn more about this project go to https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
#include <Button.h>      //https://github.com/JChristensen/Button
#include <Tone.h>        //https://github.com/bhagman/Tone
#include <MusicWithoutDelay.h>
//The Max Name length can be 35 characters, but you must include a semicolon after the name to indicate that it is the end of your name
//duration and octave and bpm(tempo or beats per measure) are optional.  Octaves can be 1-7
//example "Nat:d=4,o=5,b=99,f=ab,s=ed: 4a,5f1";
//"Name:duration,octave,bpm,flats,sharps:note1,note2,note3,etc..."  A note's beat equals (number in-front of note). duration=wholeNote/n, n=1 when whole note, n=2 for half note, n=4, for quarter note, n=8, for eight note

//Music order: intro->same->middle->same->end (repeat last steps except intro)
//I had to make this order to save memory from unneccesary repeated rhythms.  But we won't have to do this when I get the SD card version of this library.
char *introSoprano = ":d=12,o=5,b=160,f=aeb:2b,p,p,b,b,b,b,8.b,16a,4b,p,p,b,b,b,b,8.b,16a,4b,p,p,b,b,b,b,8b,16f,16f,8f,16f,16f,8f,16f,16f,8f,8f";
char *introBass    = ":d=12,o=4,b=160,f=aeb:4b,b,b,b,4b,b,b,b,4a,a,a,a,4a,a,a,a,4g_,g_,g_,g_,4g_,g_,g_,g_,4f,4f,4f,8g,8a#";

char *sameSoprano  = ":d=16:4b,4f+8.f,b,b,c1,d1,e1,2f1,8p,8f1,12f1,12g_1,12a1";
char *sameBass     = ":d=16:4d,12d,12d,12c,8.d,d,d,e,f,g,8.a,b,b,c1,d1,e1,4f1,12a,12b,12c1";

char *middleSoprano = "::2b1,12p,12b1,12b1,12b1,12a1,12g_1,8.a1,g_1,2f1,4f1,8e1,e1,f1,2g_1,8f1,8e1,8d_1,d_1,e1,2f1,8e1,8d_1,8c1,c1,d1,2e#1,4g1,8f1,f,f,8f,f,f,8f,f,f,8f,8f";
char *middleBass   =  "::8.d_1,g_,g_,a,b,c1,12d_1,12p,12d_1,12d_1,12c1,12b,8.d_,a,12a,12a,12g_,8.a,a,12a,12g_,12a,8g_,g_,f,8g_,g_,a,4b,8a,8g_,8f,f,e,8f,f,g_,4a,8g_,8f,4e#,8e#,e#,f,8g,g,a#,8b,8c1,8a#,a#-1,a#-1,8a#-1,a#-1,a#-1,8a#-1,a#-1,a#-1,8a#-1,8a#-1";

char *endSoprano   = ":d=4:2b1,p,d_2,c2,a#1,p,f1,2g_1,p,b1,a#1,f1,p,f1,2g_1,p,b_1,a#1,f1,p,d1,2e1,p,g_1,f1,d_1,p,b,8c1,16c1,16d1,e#1,p,g1,8f1,16f,16f,8f,16f,16f,8f,16f,16f,8f,8f";
char *endBass      = ":d=12:2b,4p,4e#1,4e#1,4c1,4p,4a#,e#-2,b-2,d_-1,e#-1,b-1,d_,4e#,4p,4f,f-2,f-2,f-2,4f-2,4p,e#-2,b-2,d_-1,e#-1,b-1,d_,4e#,4p,4f,f-2,f-2,f-2,4f-2,4p,2g_,4p,4c_1,4b,4f,4p,4d_,4e#,8e#,16e#,16f,8g,16g,16a#,8b,8c1,8a#,16a#-1,16a#-1,8a#-1,16a#-1,16a#-1,8a#-1,16a#-1,16a#-1,8a#-1,8a#-1";
char *lastNote     = ":d=1:b,p";
char *lastNote2    = ":d=1:d-1,p";

//char *oneSecond    = ":b=60:4a,4p";  //note every second
Tone myTone;
Tone myTone2;
MusicWithoutDelay buzzer(introSoprano);
MusicWithoutDelay buzzer2(introBass);

#define BUTTON_PIN 2       //Connect a tactile button switch (or something similar) from Arduino pin 2 to ground.
#define BUTTON_PIN2 3       //Connect a tactile button switch (or something similar) from Arduino pin 3 to ground.
#define PULLUP true        //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true        //Since the pullup resistor will keep the pin high unless the
//switch is closed, this is negative logic, i.e. a high state
//means the button is NOT pressed. (Assuming a normally open switch.)
#define DEBOUNCE_MS 70     //A debounce time of 70 milliseconds usually works well for noisy button switches.
Button pauseButton(BUTTON_PIN, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button reverseButton(BUTTON_PIN2, PULLUP, INVERT, DEBOUNCE_MS);

#define motor A0
#define potPin A1
int rgbLeds[] = {A3, A4, A5};
int led = 0;
long totalTime = 0;
void setup()
{
  //Serial.begin(9600);  //Serial takes too much memory
  //Serial.print("Playing: "); Serial.println(buzzer.getName());
  pinMode(motor, OUTPUT);  //percussion instrument
  for (int i = 0; i < 3; i++) {
    pinMode(rgbLeds[i], OUTPUT);
  }
  pinMode(13, OUTPUT);
  totalTime = buzzer.getTotalTime();
  myTone.begin(11);
  myTone2.begin(10);
}

bool repeat = true;
bool pState;
bool state;
unsigned long motorMillis = 0;
int count = 0;
int sign = 1;
void loop()
{
  unsigned long cMillis = millis();
  pauseButton.read();
  reverseButton.read();
  buzzer.play(cMillis, myTone);
  buzzer2.play(cMillis, myTone2);
  if (!buzzer.isPaused() && buzzer.isEnd()) {
    switch (count) {
      case 0:
        count++;
        buzzer.newSong(sameSoprano);
        buzzer2.newSong(sameBass);
        break;
      case 1:
        buzzer.newSong(middleSoprano);
        buzzer2.newSong(middleBass);
        count++;
        break;
      case 2:
        buzzer.newSong(sameSoprano);
        buzzer2.newSong(sameBass);
        count++;
        break;
      case 3:
        buzzer.newSong(endSoprano);
        buzzer2.newSong(endBass);
        count++;
        break;
      case 4:
        if (repeat) {
          count = 1;
          buzzer.newSong(sameSoprano);
          buzzer2.newSong(sameBass);
          repeat = false;
        }
        else {
          buzzer.newSong(lastNote);
          buzzer2.newSong(lastNote2);
          digitalWrite(motor, LOW);
          for (int i = 0; i < 3; i++) {
            digitalWrite(rgbLeds[i], HIGH);
          }
          repeat = true;
          count++;
        }
        break;
      case 5:
        for (int i = 0; i < 3; i++) {
          digitalWrite(rgbLeds[i], LOW);
        }
        Pause();
        buzzer.newSong(introSoprano);
        buzzer2.newSong(introBass);
        count = 0;
        break;
    }
  }
  if (pauseButton.wasReleased()) {
    Pause();
  }
  if (reverseButton.wasReleased()) {
    buzzer.skipTo(buzzer.getTotalTime() / 2);
    buzzer2.skipTo(buzzer2.getTotalTime() / 2);
  }
  if (cMillis - motorMillis >= 50) {
    digitalWrite(motor, LOW);
  }
  if (buzzer2.isNote()) {
    digitalWrite(motor, HIGH);
    motorMillis = cMillis;
  }
  state = buzzer.isNote();
  if (state != pState) {
    pState = state;
    if (pState) {
      digitalWrite(rgbLeds[led], LOW);
    }
    else {
      digitalWrite(rgbLeds[led], LOW);
      led++;
      if (led == 3) {
        led = 0;
      }
      digitalWrite(rgbLeds[led], HIGH);
    }
  }
  if (buzzer.isRest()) {
    digitalWrite(rgbLeds[led], LOW);
  }
}
void Pause() {
  buzzer.pause();
  buzzer2.pause();
}
void backwards() {
  buzzer.reverse();
  buzzer2.reverse();
}

