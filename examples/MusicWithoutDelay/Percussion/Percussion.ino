//https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
/*Example for the MusicWithoutDelay Library by Nathan Ramanathan. nathan6ramanathan@gmail.com
   This sketch demostrates the skipTo function's advantage.  It can skip to any point of the song :D
   It plays a small section of the legend of Zelda Song.
   Obviously, you can do other things while the song is being played.
   For example, talk to Serial, read buttons, display images on an OLED screen, etc...

   Pressing either button on pin 2 or 3 causes the song to skip to the beginning or anywhere.

   This is by far the most complex, beautiful, mathematical function I have created.  :')
   Use skipTo(long) wisely
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
const char song[] PROGMEM = {"Entertainer:d=4,o=5,b=140:8d,8d#,8e,c1,8e,c1,8e,2.c1,8c1,8d1,8d#1,8e1,8c1,8d1,e1,8b,d1,2c1,p,8d,8d#,8e,c1,8e,c1,8e,2.c1,8p,8a,8g,8f#,8a,8c1,e1,8d1,8c1,8a,2d1"};
MusicWithoutDelay drum(song);
#define motor A2        //vibrating motor attached to AnalogPin 0
unsigned long motorMillis = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(motor, OUTPUT);  //percussion instrument
  drum.begin(CHB, TRIANGLE, ENVELOPE0, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long cMillis = millis();
  drum.update();
  if (cMillis - motorMillis >= 50) {
    digitalWrite(motor, LOW);
  }
  if (drum.isNote()) {
    digitalWrite(motor, HIGH);
    motorMillis = cMillis;
  }
}
