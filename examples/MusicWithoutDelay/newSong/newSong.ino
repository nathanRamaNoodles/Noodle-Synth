//https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
/*Example for the MusicWithoutDelay Library by Nathan Ramanathan. nathan6ramanathan@gmail.com
   This sketch demostrates the newSong function's advantage.  It can be used to skip to the beginning of a song or a new song can be chosen.
   Obviously, you can do other things while the song is being played.
   For example, talk to Serial, read buttons, display images on an OLED screen, etc...
*/
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
#include <NoodleSynth.h>
synthEngine mixer(20E3);
const char song[] PROGMEM = {":d=16:c,a,b,p"};
const char song2[] PROGMEM= {":d=12:c1,d1,e1,p"};
bool songToggle;
MusicWithoutDelay buzzer(song);
void setup() {
  // put your setup code here, to run once:
  buzzer.begin(CHB, TRIANGLE, ENVELOPE0, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  buzzer.update();
  if (buzzer.isEnd()) {
    songToggle = !songToggle;
    if (songToggle)
      buzzer.newSong(song2);      //If you change song2 ->song, the arduino will treat newSong() like skipTo(0); and skip to the beginning of song
    else
      buzzer.newSong(song);
  }
}
