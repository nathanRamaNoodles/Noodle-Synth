//demonstrates the Volume feature to this library
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
#define potPin A0

#include <NoodleSynth.h>
synthEngine mixer(20E3); // https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
#include <NewPing.h>  //https://playground.arduino.cc/Code/NewPing
#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

#include <SensorToButton.h>
SensorToButton btn(2, 50);
const char song[] PROGMEM = {":d=4:b,c1,g,a1,p"};
MusicWithoutDelay buzzer(song);
MusicWithoutDelay buzzer2;
MusicWithoutDelay buzzer3;
uint32_t pMillis;
int MIN = 10;
int MAX = 40;
int vol = 0;
float freq;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  int wave = SINE;
  buzzer.begin(CHB, wave, ENVELOPE0, 0).setVolume(vol);
  buzzer2.begin(wave, ENVELOPE0, 0).setVolume(vol);
  buzzer3.begin(wave, ENVELOPE0, 0).setVolume(vol);
  //  buzzer.setVolume(0);
  //  buzzer2.setVolume(0);
  //  buzzer3.setVolume(0);
  freq = buzzer.getNoteAsFrequency(NOTE_A4);
  Serial.print(freq); Serial.println(" hz");
  buzzer.setFrequency(freq);

  freq += random(0, 10) / 10.0;
  buzzer2.setFrequency(freq);

  freq += random(0, 10) / 10.0;
  buzzer3.setFrequency(freq);
  pinMode(potPin, INPUT);
}
void loop() {
  // put your main code here, to run repeatedly:
  btn.read();
  buzzer.update().setVolume(vol);
  buzzer2.update().setVolume(vol);
  buzzer3.update().setVolume(vol);
  //  int vol = analogRead(potPin);
  //  vol = map(vol, 0, 1023, 20, 100);
  if (millis() - pMillis > 50) {
    vol = sonar.ping_cm();
    if (vol < MAX && vol > MIN) {
      vol = map(vol, MIN, MAX, 40, 100);
    }
    else {
      vol = 0;
    }
    buzzer.setVolume(vol);
    pMillis = millis();
  }
  if (btn.wasPressed()) {
    if (buzzer.isSingleNote()) {
      buzzer.newSong(song).setWave(RAMP);
    }
    else {
      buzzer.setFrequency(buzzer.getNoteAsFrequency(NOTE_G3)).setWave(TRIANGLE);
    }
    //    buzzer.mute(!buzzer.isMuted());
    //    buzzer2.mute(!buzzer3.isMuted());
    //    buzzer3.mute(!buzzer3.isMuted());
  }
}
