//Just some catchy tune I didn't feel like finishing.  But if you want to, you can finish the rest of the song here: https://musescore.com/jestermusician/undertale_spiderdance
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
const char song[] PROGMEM = {"Spider Dance:b=115,d=16,f=abde,o=5:8f1,8c1,8a,8f,  p,c_1,8b,  b,a,e,f, p,c1,b,a,  b,c1,e,f,a,f,e,  f1,f1,e1,32c1,32b,a,  4f,4g,4a,4b,  8c1,8b,8f1,8c1,  8e#1,8d1,4c1"};
const char song2[] PROGMEM = {"Spider Dance:b=115,d=16,f=abde,o=5:8c1,8a,8f,8g,  p,a,8f,  g,f_,b,d,  p,a,f,e,  g,a,g-1,a-1,c,a-1,g-1,  a,a,8p,16p,   8p,8b-1,8p,8b-1,8p,8b-1,8p,8a-1,  8a,8d,8c1,8f,  8b,8a,4g"};
const char song3[] PROGMEM = {"Spider Dance:b=115,d=16,f=abde,o=3:f,c1,b,c1,f,c1,b,c1,d,c_1,b,c_1,c,c1,g,b,  f,c1,b,c1,  g,c1,b,c1,  a,c1,b,c1,  b,d1,c1,d1,   d,a,g,a, d,b,a,b,  d,c1,b,c1,  d,d1,c1,d1, 4c, 8p,8c,4c,  c1,d#1,e#1"};

#define NUM 3 //number of MusicWithoutDelay objects
MusicWithoutDelay buzzer(song);
MusicWithoutDelay buzzer2(song2);
MusicWithoutDelay buzzer3(song3);
const int motorPin = A2;  //connect a vibrating motor to pin A2
uint32_t motorMillis;

bool save;
int count = 0;

void setup() {
  // put your setup code here, to run once:
  buzzer.begin(CHB, TRIANGLE, ENVELOPE0, 0);   //CHB means pin 3 on Arduino UNO, CHA means pin 11
  buzzer2.begin(TRIANGLE, ENVELOPE0, 0);
  buzzer3.begin(RAMP, ENVELOPE0, 0);
  pinMode(motorPin, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("Type '1','2', or '3' to pause the corresponding instrument.  If you let one of the instruments to finish, the arduino will restart from the beginning."));
  Serial.println(F("Type more than one number to pause multiple instruments"));
  Serial.println(F("Also, type 'r' to modulate"));
  Serial.print(F("TotalTime: ")); Serial.print(buzzer.getTotalTime() / 1E3); Serial.println(F(" seconds"));
}

void loop() {
  // put your main code here, to run repeatedly:
  buzzer.update();
  buzzer2.update();
  buzzer3.update();
  if (millis() - motorMillis >= 50) {
    digitalWrite(motorPin, LOW);
  }
  if (buzzer.isNote() || buzzer2.isNote() || buzzer3.isNote()) {  //this code make the vibrating motor beat to the rhythm
    digitalWrite(motorPin, HIGH);
    motorMillis = millis();
  }
  if (!save)
    proper_Restart();
  if (Serial.available()) {
    char str = Serial.read();
    int r;
    switch (str) {
      case '1':  //type '1' in Serial monitor.
        buzzer.pause(!buzzer.isPaused());
        (buzzer.isPaused()) ? count++ : count--;     //ternary operator is faster than if/else statements.
        (count == NUM) ? save = true : save = false;
        break;
      case '2':  //type '2' in Serial monitor.
        buzzer2.pause(!buzzer2.isPaused());
        (buzzer2.isPaused()) ? count++ : count--;
        (count == NUM) ? save = true : save = false;
        break;
      case '3':  //type '3' in Serial monitor.
        buzzer3.pause(!buzzer3.isPaused());
        (buzzer3.isPaused()) ? count++ : count--;
        (count == NUM) ? save = true : save = false;
        break;
      case 'r':  //type 's' in Serial monitor.
        r = random(-50, 50);
        Serial.print("Mod: "); Serial.println(r);
        buzzer.setMod(r);
        buzzer2.setMod(r);
        buzzer3.setMod(r);
        break;
    }
  }
}

void proper_Restart() { //this method avoids accidental overlapping in songs with different total times
  if (buzzer.isEnd()) {
    buzzer.pause(true); //pause song
  }
  if (buzzer2.isEnd()) {
    buzzer2.pause(true);
  }
  if (buzzer3.isEnd()) {
    buzzer3.pause(true);
  }
  if (buzzer.isPaused() && buzzer2.isPaused() && buzzer3.isPaused()) {
    Serial.println("Reset");
    count = 0;
    skip(0);         //skip to beginning of song(skips in milliseconds)
    buzzer.pause(false);  //resume song
    buzzer2.pause(false);
    buzzer3.pause(false);
  }
}

void skip(long t) {
  buzzer.skipTo(t);
  buzzer2.skipTo(t);
  buzzer3.skipTo(t);
}
