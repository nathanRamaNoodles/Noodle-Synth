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

#include <MusicWithoutDelay.h>
const char song[] PROGMEM = {"Spider Dance:b=115,d=16,f=abde,o=5:8f1,8c1,8a,8f,  p,c_,8b,  16b,16a,16e,16f"};
const char song2[] PROGMEM = {"Spider Dance:b=115,d=16,f=abde,o=5:8c1,8a,8f,8g,  p,a,8f,  16g,16f_,16b,16d"};
const char song3[] PROGMEM = {"Spider Dance:b=115,d=16,f=abde,o=3:f,c,b,c,f,c,b,c,d,c_,b,c_,c#-1,c,g,b"};
MusicWithoutDelay buzzer(song);
MusicWithoutDelay buzzer2(song2);
MusicWithoutDelay buzzer3(song3);
const int motorPin = A2;
uint32_t motorMillis;
void setup() {
  // put your setup code here, to run once:
  buzzer.begin(CHB, TRIANGLE, ENVELOPE0, 0);
  buzzer2.begin(TRIANGLE, ENVELOPE0, 0);
  buzzer3.begin(RAMP, ENVELOPE0, 0);
  pinMode(motorPin, OUTPUT);
  Serial.begin(9600);
}
void loop() {
  // put your main code here, to run repeatedly:
  buzzer.play();
  buzzer2.play();
  buzzer3.play();
  if (millis() - motorMillis >= 50) {
    digitalWrite(motorPin, LOW);
  }
  if (buzzer.isNote() || buzzer2.isNote() || buzzer3.isNote()) {
    digitalWrite(motorPin, HIGH);
    motorMillis = millis();
  }
  if (buzzer.isEnd()) {
    buzzer.pause();
  }
  if (buzzer2.isEnd()) {
    buzzer2.pause();
  }
  if (buzzer3.isEnd()) {
    buzzer3.pause();
  }
  if (Serial.available()) {
    char str = Serial.read();
    switch (str) {
      case '1':  //type 's' in Serial monitor.
        buzzer.pause();
        break;
      case '2':  //type 's' in Serial monitor.
        buzzer2.pause();
        break;
      case '3':  //type 's' in Serial monitor.
        buzzer3.pause();
        break;
      case '4':  //type 's' in Serial monitor.
        buzzer.mute();
        buzzer2.mute();
        buzzer3.mute();
        break;
    }
  }
}

