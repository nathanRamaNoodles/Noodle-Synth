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
const char song[] PROGMEM = {":d=16,o=5,b=160,f=aeb:2b,12p,12p,12b,12b,12b,12b,8.b,16a,4b,12p,12p,12b,12b,12b,12b,8.b,16a,4b,12p,12p,12b,12b,12b,12b,8b,16f,16f,8f,16f,16f,8f,16f,16f,8f,8f,   4b,4f+8.f,b,b,c1,d1,e1,2f1,8p,8f1,12f1,12g_1,12a1,   2b1,12p,12b1,12b1,12b1,12a1,12g_1,8.a1,g_1,2f1,4f1,8e1,e1,f1,2g_1,8f1,8e1,8d_1,d_1,e1,2f1,8e1,8d_1,8c1,c1,d1,2e#1,4g1,8f1,f,f,8f,f,f,8f,f,f,8f,8f,  4b,4f+8.f,b,b,c1,d1,e1,2f1,8p,8f1,12f1,12g_1,12a1,  2b1,4p,4d_2,4c2,4a#1,4p,4f1,2g_1,4p,4b1,4a#1,4f1,4p,4f1,2g_1,4p,4b_1,4a#1,4f1,4p,4d1,2e1,4p,4g_1,4f1,4d_1,4p,4b,8c1,16c1,16d1,4e#1,4p,4g1,8f1,16f,16f,8f,16f,16f,8f,16f,16f,8f,8f,   1b,1p"};
const char song2[] PROGMEM = {":d=16,o=4,b=160,f=aeb:4b,12b,12b,12b,4b,12b,12b,12b,4a,12a,12a,12a,4a,12a,12a,12a,4g_,12g_,12g_,12g_,4g_,12g_,12g_,12g_,4f,4f,4f,8g,8a#,   4d,12d,12d,12c,8.d,d,d,e,f,g,8.a,b,b,c1,d1,e1,4f1,12a,12b,12c1,   8.d_1,g_,g_,a,b,c1,12d_1,12p,12d_1,12d_1,12c1,12b,8.d_,a,12a,12a,12g_,8.a,a,12a,12g_,12a,8g_,g_,f,8g_,g_,a,4b,8a,8g_,8f,f,e,8f,f,g_,4a,8g_,8f,4e#,8e#,e#,f,8g,g,a#,8b,8c1,8a#,a#-1,a#-1,8a#-1,a#-1,a#-1,8a#-1,a#-1,a#-1,8a#-1,8a#-1,  4d,12d,12d,12c,8.d,d,d,e,f,g,8.a,b,b,c1,d1,e1,4f1,12a,12b,12c1,  2b,4p,4e#1,4e#1,4c1,4p,4a#,12e#-2,12b-2,12d_-1,12e#-1,12b-1,12d_,4e#,4p,4f,12f-2,12f-2,12f-2,4f-2,4p,12e#-2,12b-2,12d_-1,12e#-1,12b-1,12d_,4e#,4p,4f,12f-2,12f-2,12f-2,4f-2,4p,2g_,4p,4c_1,4b,4f,4p,4d_,4e#,8e#,16e#,16f,8g,16g,16a#,8b,8c1,8a#,16a#-1,16a#-1,8a#-1,16a#-1,16a#-1,8a#-1,16a#-1,16a#-1,8a#-1,8a#-1,  1d-1,1p"};
MusicWithoutDelay buzzer(song);
MusicWithoutDelay buzzer2(song2);
const int motorPin = A2;
const int potPin = A0;
uint32_t motorMillis;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  buzzer.begin(CHB, TRIANGLE, ENVELOPE0, 0);
  buzzer2.begin(TRIANGLE, ENVELOPE0, 0);
  pinMode(motorPin, OUTPUT);
  pinMode(potPin, INPUT);
  unsigned long totalTime = buzzer.getTotalTime();
  Serial.println(F("Type 'm' to modulate,\n's' to randomly skip within song,\n'p' to pause/play\n'b' to play in reverse/forwards"));
  Serial.print("TotalTime: "); Serial.print(totalTime / 1E3); Serial.println(" seconds!");
}
int r, j;
void loop() {
  // put your main code here, to run repeatedly:
  buzzer.update();
  buzzer2.update();
  //  int val = map(analogRead(potPin), 0, 1023, -50, 50);
  //  buzzer.setMod(val);
  //  buzzer2.setMod(val);
  if (millis() - motorMillis >= 50) {
    digitalWrite(motorPin, LOW);
  }
  if (buzzer.isNote() || buzzer2.isNote()) {
    digitalWrite(motorPin, HIGH);
    motorMillis = millis();
  }
  if (Serial.available()) {
    char str = Serial.read();
    switch (str) {
      case 'p':
        Pause();
        if (!buzzer.isPaused()) {
          Serial.println(F("Resuming"));
        }
        else {
          Serial.print(F("Paused.\tCurrent-Time: ")); Serial.print(buzzer.getCurrentTime() / 1E3); Serial.println(" seconds.");
        }
        break;
      case 'b':
        backwards();
        if (buzzer.isBackwards())
          Serial.println(F("Playing Backwards"));
        else
          Serial.println(F("Playing Forwards"));
        break;
      case 's':
        r = random(0, 100);
        buzzer.skipTo(buzzer.getTotalTime() * (r / 100.0));    //random skip
        buzzer2.skipTo(buzzer2.getTotalTime() * (r / 100.0));
        Serial.print(F("Skipping to: ")); Serial.print(buzzer.getCurrentTime() / 1E3); Serial.println(F(" seconds"));
        break;
      case 'm':
        j = random(-50, 50);
        buzzer.setMod(j);
        buzzer2.setMod(j);
        break;
    }
  }
}

void Pause() {
  buzzer.pause(!buzzer.isPaused());
  buzzer2.pause(!buzzer2.isPaused());
}
void backwards() {
  buzzer.reverse(!buzzer.isBackwards());
  buzzer2.reverse(!buzzer2.isBackwards());
}
