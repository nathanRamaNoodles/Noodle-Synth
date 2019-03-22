//demonstrates the Volume feature to this library (volume from 0-127)
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
synthEngine mixer(20E3); //https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
const char song[] PROGMEM = {":d=32:c,d,e,f,g,a,b,c1,d1,e1,f1,g1,a1,b1,c2,d2,e2,f2,g2,a2,b2,c3,d3,e3,f3,g3,a3,b3,c4"};
MusicWithoutDelay buzzer(song);
long totalTime;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  buzzer.begin(CHB, SINE, ENVELOPE3, 0);

  buzzer.overrideSustain(true);
  buzzer.setSustain(NONE); // similar to using "+" signs instead of "," commas in song[] array.

  totalTime = buzzer.getTotalTime();
}
void loop() {
  // put your main code here, to run repeatedly:
  buzzer.update();
  long currentTime = buzzer.getCurrentTime();
  int mVolume = map(currentTime, 0, totalTime, 50, 127); // minVolume, maxVolume
  buzzer.setVolume(mVolume);
}
