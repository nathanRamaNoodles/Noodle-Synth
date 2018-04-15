/*Example for the MusicWithoutDelay Library by Nathan Ramanathan. nathan6ramanathan@gmail.com
   This sketch demostrates the newSong function's advantage.  It can be used to skip to the beginning of a song or a new song can be chosen.
   Obviously, you can do other things while the song is being played.
   For example, talk to Serial, read buttons, display images on an OLED screen, etc...
*/
//more info at https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
#include <MusicWithoutDelay.h>
#include <Tone.h>                //https://github.com/bhagman/Tone
Tone myTone;
char *song = ":d=16:c,a,b,p";
char *song2 = ":d=12:c1,d1,e1,p";
bool songToggle;
MusicWithoutDelay buzzer(song);
void setup() {
  // put your setup code here, to run once:
  myTone.begin(11);
}

void loop() {
  // put your main code here, to run repeatedly:
  buzzer.play(millis(), myTone);
  if (buzzer.isEnd()) {
    songToggle = !songToggle;
    if (songToggle)
      buzzer.newSong(song2);      //If you change song2 ->song, the arduino will treat newSong() like skipTo(0); and skip to the beginning of song
    else
      buzzer.newSong(song);
  }
}
