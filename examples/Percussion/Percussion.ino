/*Example for the MusicWithoutDelay Library by Nathan Ramanathan. nathan6ramanathan@gmail.com
   This sketch demostrates the isNote function's advantage. It can be used for percussion :D
   It plays a small section of the Entertainer Song.
   Obviously, you can do other things while the song is being played.
   For example, talk to Serial, read buttons, display images on an OLED screen, etc...
*/
// More info at https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
#include <MusicWithoutDelay.h>
#include <Tone.h>                //https://github.com/bhagman/Tone
Tone myTone;
char *song = "Entertainer:d=4,o=5,b=140:8d,8d#,8e,c1,8e,c1,8e,2.c1,8c1,8d1,8d#1,8e1,8c1,8d1,e1,8b,d1,2c1,p,8d,8d#,8e,c1,8e,c1,8e,2.c1,8p,8a,8g,8f#,8a,8c1,e1,8d1,8c1,8a,2d1";
MusicWithoutDelay drum(song);
#define motor A0        //vibrating motor attached to AnalogPin 0
unsigned long motorMillis = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(motor, OUTPUT);  //percussion instrument
  myTone.begin(11);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long cMillis = millis();
  drum.play(cMillis, myTone);
  if (cMillis - motorMillis >= 50) {
    digitalWrite(motor, LOW);
  }
  if (drum.isNote()) {
    digitalWrite(motor, HIGH);
    motorMillis = cMillis;
  }
}
