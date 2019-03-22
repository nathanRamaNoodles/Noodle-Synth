/*Example for the MusicWithoutDelay Library by Nathan Ramanathan. nathan6ramanathan@gmail.com
   This simple sketch demostrates polyphonic sounds.(It can play more than one note at the same time)
   Please consider that the number of notes played at the same time depends on the number of timers on your Arduino.
   In this case, you can play a max of 2 notes at the same time on the Arduino Uno.
   Obviously, you can do other things while the song is being played.
   For example, talk to Serial, read buttons, display images on an OLED screen, etc...
  ////////Directions
   Connect two buttons, one to pin 2 and the other to pin 3.
   When you press any button, a note will be played according to the char*.
   **You can play more than one note by pressing both buttons!!
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
//To learn more about this project go to https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary

#include <SensorToButton.h>  //https://github.com/nathanRamaNoodles/SensorToButton  acquired by JChristensen's popular button library
#include <NoodleSynth.h>
synthEngine mixer(20E3);
//const char note1[] PROGMEM = "C Scale:d=4:c,d,e,f,g,a,b,c1";   //plays c scale,  You can add more notes to get creative. //d=4 means that every note without a number in front of the letter is assumed to be a quarter note.
//const char note2[] PROGMEM = ":d=4:e,f,g,a,b,c1,d1,e1";   //plays c scale starting at e
const char note1[] PROGMEM = {"::c"};   //plays c ,  You can add more notes to get creative.
const char note2[] PROGMEM = {"::e"};  //plays e
const char note3[] PROGMEM = {"::f"};  //plays f
const char note4[] PROGMEM = {"::g"};  //plays g
MusicWithoutDelay pianoKey(note1);
MusicWithoutDelay pianoKey2(note2);
MusicWithoutDelay pianoKey3(note3);
MusicWithoutDelay pianoKey4(note4);
#define BUTTON_PIN 7       //Connect a tactile button switch (or something similar) from Arduino pin 2 to ground.
#define BUTTON_PIN2 6       //Connect a tactile button switch (or something similar) from Arduino pin 3 to ground.
#define BUTTON_PIN3 5
#define BUTTON_PIN4 2

#define DEBOUNCE_MS 70     //A debounce time of 70 milliseconds usually works well for noisy button switches. if not, try 20.
SensorToButton firstKey(BUTTON_PIN, DEBOUNCE_MS);    //Declare the button
SensorToButton secondKey(BUTTON_PIN2, DEBOUNCE_MS);
SensorToButton thirdKey(BUTTON_PIN3, DEBOUNCE_MS);
SensorToButton fourthKey(BUTTON_PIN4, DEBOUNCE_MS);
void setup() {
  // put your setup code here, to run once:
  pianoKey.begin(CHB, TRIANGLE, ENVELOPE0, 0);
  pianoKey2.begin(TRIANGLE, ENVELOPE0, 0);
  pianoKey3.begin(TRIANGLE, ENVELOPE0, 0);
  pianoKey4.begin(TRIANGLE, ENVELOPE0, 0);

  pianoKey.pause(true); pianoKey2.pause(true); pianoKey3.pause(true); pianoKey4.pause(true);
}

void loop() {
  // put your main code here, to run repeatedly:
  pianoKey.update();
  pianoKey2.update();
  pianoKey3.update();
  pianoKey4.update();

  firstKey.read();
  secondKey.read();
  thirdKey.read();
  fourthKey.read();

  buttonToPiano(firstKey, pianoKey);
  buttonToPiano(secondKey, pianoKey2);
  buttonToPiano(thirdKey, pianoKey3);
  buttonToPiano(fourthKey, pianoKey4);
}

void buttonToPiano(SensorToButton b, MusicWithoutDelay &m) {  //Must put & before object, because we must point to the address of the Object in the Arduino Memory
  if (b.isPressed()) {
    m.play(1);    //play one time
  }
}
