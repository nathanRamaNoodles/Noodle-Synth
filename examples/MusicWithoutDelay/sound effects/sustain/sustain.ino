//demonstrates the sustain update to this library
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
const char song[] PROGMEM = {"::c,a,b,p"};
int songToggle = 0;
MusicWithoutDelay buzzer(song);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  buzzer.begin(CHB, RAMP, ENVELOPE0, 0);
  buzzer.overrideSustain(true);
  buzzer.setSustain(NONE);
  Serial.println("NONE");
}

void loop() {
  // put your main code here, to run repeatedly:
  buzzer.update();
  if (buzzer.isEnd()) {
    songToggle++;
    switch (songToggle) {
      case NONE:
        Serial.println("NONE");
        buzzer.setSustain(NONE);   //Boring old Arduino tone
        break;
      case SUSTAIN:
        Serial.println("SUSTAIN");
        buzzer.setSustain(SUSTAIN); //Rich Sustain(default)
        break;
      case REV_SUSTAIN:
        Serial.println("REV_SUSTAIN");
        buzzer.setSustain(REV_SUSTAIN); //Fancy, backwards Sustain
        songToggle = -1;
        break;
    }
  }
}
