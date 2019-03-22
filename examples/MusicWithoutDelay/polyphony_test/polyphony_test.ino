//Here, I try to find the true potentional of polyphony for the Arduino Teensy.  Arduino Uno is pretty weak at 7 voices, but the Teensy is very powerful at 16+ voices.

//A potentiometer controls the volume, and a button can oscilate the frequencies.(Note: the more note-polyphony,
//the lower the volume you need to set each instrument; otherwise the sound will get cracky) :D
//Find out more here: https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
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

//https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
#include <NoodleSynth.h>
synthEngine mixer(20E3);
#include <SensorToButton.h> //https://github.com/nathanRamaNoodles/SensorToButton
SensorToButton btn(2, 50);  //pin, debounce time(ms)
#define NUM 4   //Num of voices, you may change this to your microcontroller's processor speed,
//but I have the max number of voices listed on my github: https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
MusicWithoutDelay instruments[NUM];
int vol = 0;
uint16_t INTERVAL = 15;
bool mode = true;
int flip = 1;
// int pins[6] = {CHA,CHB,6,9,22,23}; //for Teensy 3+
int pins[2] = {CHA,CHB};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //  while(!Serial);  //For USB devices
  for (byte i = 0; i < NUM; i++) {
    MusicWithoutDelay *myInstrument = &instruments[i];
    float freq = MusicWithoutDelay::getNoteAsFrequency(NOTE_A3 + i * 4);               //Function gives Frequceny using a Formula.  Every other 4 notes is a harmonic major(something like that)
    Serial.print(freq); Serial.println(" hz");//Print Frequency to Serial

    myInstrument->begin(pins[i%2], SINE, ENVELOPE0, 0).setVolume(0).setFrequency(freq);  //set up instruments and their frequencies

  }
  pinMode(potPin, INPUT);
}
uint32_t pMillis, serialMillis;
void loop() {
  // put your main code here, to run repeatedly:
  uint32_t cMillis = millis();
  btn.read();
  //  if (cMillis - serialMillis > 500) {
  for (byte i = 0; i < NUM; i++) {
    MusicWithoutDelay *myInstrument = &instruments[i];
    myInstrument->update().setVolume(vol);             //update instruments
  }
  //    serialMillis = cMillis;
  //  }
  if (mode) {
    vol = analogRead(potPin);
    vol = map(vol, 0, 1023, 0, 127);
  }
  else {
    if (cMillis - pMillis >= INTERVAL) {   //Vibrato
      vol += flip;
      if (vol < 90 || vol > 126) {
        flip *= -1;
      }
      pMillis = cMillis;
    }
  }
  if (btn.wasPressed()) {
    vol = 95;
    mode = !mode;
  }
}
