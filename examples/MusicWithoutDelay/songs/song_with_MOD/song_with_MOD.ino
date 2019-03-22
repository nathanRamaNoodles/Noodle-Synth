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

#include <SensorToButton.h>  //https://github.com/nathanRamaNoodles/SensorToButton
#define buttonPin 2
SensorToButton btn(buttonPin, 50);
#define potPin A0
#include <NoodleSynth.h>
synthEngine mixer(20E3); //https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
const char song[] PROGMEM = {"Spider Dance:b=115,d=16,f=abde,o=4:8f1,8c1,8a,8f,  p,c_1,8b,  b,a,e,f, p,c1,b,a,  b,c1,e,f,a,f,e,  f1,f1,e1,32c1,32b,a,  4f,4g,4a,4b,  8c1,8b,8f1,8c1,  8e#1,8d1,4c1"};
const char song2[] PROGMEM = {"Spider Dance:b=115,d=16,f=abde,o=4:8c1,8a,8f,8g,  p,a,8f,  g,f_,b,d,  p,a,f,e,  g,a,g-1,a-1,c,a-1,g-1,  a,a,8p,16p,   8p,8b-1,8p,8b-1,8p,8b-1,8p,8a-1,  8a,8d,8c1,8f,  8b,8a,4g"};
const char song3[] PROGMEM = {"Spider Dance:b=115,d=16,f=abde,o=3:f,c1,b,c1,f,c1,b,c1,d,c_1,b,c_1,c,c1,g,b,  f,c1,b,c1,  g,c1,b,c1,  a,c1,b,c1,  b,d1,c1,d1,   d,a,g,a, d,b,a,b,  d,c1,b,c1,  d,d1,c1,d1, 4c, 8p,8c,4c,  c1,d#1,e#1"};
const char coinEffect[] PROGMEM = {"Coin Effect:o=5:32b,32e1"};
#define NUM 3 //number of MusicWithoutDelay objects
MusicWithoutDelay buzzer(song);
MusicWithoutDelay buzzer2(song2);
MusicWithoutDelay buzzer3(song3);
MusicWithoutDelay buzzer4(coinEffect);

bool save;
int count = 0;
uint32_t isHeldMillis = 0;
const int heldAmount = 300;
bool recursion, isHeld;
int sustain[3] = {1, 1, 1};

void setup() {
  Serial.begin(9600);
  pinMode(potPin, INPUT);
  buzzer.begin(CHA,RAMP, ENVELOPE0, 0);   //CHB means pin 3 on Arduino UNO, CHA means pin 11, DIFF means both
  buzzer2.begin(RAMP, ENVELOPE0, 0);
  buzzer3.begin(RAMP, ENVELOPE0, 0);
  buzzer4.begin(SQUARE, ENVELOPE0, 0);//coin effect
  buzzer4.pause(true);
  //This part is necessary if you want to mess with the sustains of the music
  buzzer.overrideSustain(true);//must override built-in sustains to modify song's sustains at runtime
  buzzer2.overrideSustain(true);
  buzzer3.overrideSustain(true);
  Serial.println(F("Type '1','2', or '3' to pause the corresponding instrument.  If you let one of the instruments to finish, the arduino will restart from the beginning."));
  Serial.println(F("Type more than one number to pause multiple instruments"));
  Serial.println(F("Also, type 'b' to play the song backwards, and press `z`, `x`, and `c` to hear the sustains corresponding to each instrument"));
  Serial.print(F("TotalTime: ")); Serial.print(buzzer.getTotalTime() / 1E3); Serial.println(F(" seconds"));
}
void loop() {
  btn.read();
  if (btn.wasPressed()) {
    isHeldMillis = millis();
    isHeld = true;
    buzzer4.play();
  }
  else if (btn.wasReleased()) {
    isHeld = false;
    buzzer4.setBPM(100);
    recursion = false;
  }
  if ((millis() - isHeldMillis > heldAmount) && isHeld && !recursion) {  //this part handles when the button is held for more than 300 ms
    buzzer4.pause(false);//unpause
    buzzer4.setBPM(125);
    recursion = true;
  }
  int reading = analogRead(potPin);  //read the Potentiometer
  reading = map(reading, 0, 1023, 0, 100);
  //  reading = map(reading, 0, 1023, -50, 50);
  set_Volume(reading);
  //    modulate(reading); //modulation engine

  if (-3 < reading && reading < 3) { //give a little probabilty for your potPin to land here
    reading = 0;
  }

  buzzer.update(); //synth engine
  buzzer2.update();
  buzzer3.update();
  buzzer4.update();
  if (buzzer4.isEnd() && !recursion) {
    buzzer4.pause(true);
  }
  if (!save)
    proper_Restart(); //corrects overlapping at end of song
  if (Serial.available()) {
    char str = Serial.read();
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
      case 'b':
        backwards();
        break;
      case 'z':
        sustain[0] = !sustain[0];
        buzzer.setSustain(sustain[0]);
        break;
      case 'x':
        sustain[1] = !sustain[1];
        buzzer2.setSustain(sustain[1]);
        break;
      case 'c':
        sustain[2] = !sustain[2];
        buzzer3.setSustain(sustain[2]);
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
//    Serial.println("Reset");
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
void backwards() {
  buzzer.reverse(!buzzer.isBackwards());
  buzzer2.reverse(!buzzer2.isBackwards());
  buzzer3.reverse(!buzzer3.isBackwards());
  if (buzzer.isBackwards()) {
    set_Sustain(REV_SUSTAIN);
  }
  else {
    set_Sustain(SUSTAIN);
  }
}
void modulate(int m) {
  buzzer.setMod(m);
  buzzer2.setMod(m);
  buzzer3.setMod(m);
  buzzer4.setMod(m);
}
void set_Sustain(int m) {
  buzzer.setSustain(m);
  buzzer2.setSustain(m);
  buzzer3.setSustain(m);
}
void set_Volume(int v) {
  buzzer.setVolume(v);
  buzzer2.setVolume(v);
  buzzer3.setVolume(v);
  buzzer4.setVolume(v);
}
