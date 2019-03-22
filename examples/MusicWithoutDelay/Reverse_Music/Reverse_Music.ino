// A fun sketch to demonstrate the use of the ToneWithoutDelay library's reverse() function.

//connect a vibrating motor to pin A0 to hear a percussion instrument
/*Example for the MusicWithoutDelay Library by Nathan Ramanathan. nathan6ramanathan@gmail.com
   This sketch demostrates the reverse function's advantage.  It can reverse the song at any moment(without delay, obviously)
   It plays a small section of the Mision Impossible song.
   Obviously, you can do other things while the song is being played.
   For example, talk to Serial, read buttons, display images on an OLED screen, etc...

   Pressing button 2 causes the song to pause.
   Pressing button 3 causes the song to switch directions. So it can play backwards and forwards.

   This is one of my funny and beautiful functions I made.  XD
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

#include <SensorToButton.h>  //https://github.com/nathanRamaNoodles/SensorToButton
#include <NoodleSynth.h>
synthEngine mixer(20E3);
//Too many chars can cause Memory problems.  Be cautious ;)
const char song[] PROGMEM = {"MissionImp:d=16,o=6,b=150:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,16g,8p,16g,8p,16a#,16p,16c1,16p,16g,8p,16g,8p,16f,16p,16f#,16p,16g,8p,16g,8p,16a#,16p,16c1,16p,16g,8p,16g,8p,16f,16p,16f#,16p,16a#,16g,2d,32p,16a#,16g,2c#,32p,16a#,16g,2c,16a#-1,8c,2p,32p,16a#-1,16g-1,2f#,32p,16a#-1,16g-1,2f,32p,16a#-1,16g-1,2e,16d#,8d"};
const char song2[] PROGMEM = {":d=16,o=4,b=150:8d,16d#,2e,16g-1,16a#-1,32p,2f,16g-1,16a#-1,32p,2f#,16g-1,16a#-1,32p,2p,8c,16a#-1,2c,16g,16a#,32p,2c#,16g,16a#,32p,2d,16g,16a#,16p,16f#,16p,16f,8p,16g,8p,16g,16p,16c1,16p,16a#,8p,16g,8p,16g,16p,16f#,16p,16f,8p,16g,8p,16g,16p,16c1,16p,16a#,8p,16g,8p,16g,32g,32f#,32f,32e,32d#,32d,32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d"};
#define BUTTON_PIN 7       //Connect a tactile button switch (or something similar) from Arduino pin 2 to ground.
#define BUTTON_PIN2 6       //Connect a tactile button switch (or something similar) from Arduino pin 3 to ground.
#define DEBOUNCE_MS 70     //A debounce time of 70 milliseconds usually works well for noisy button switches.
SensorToButton pauseButton(BUTTON_PIN, DEBOUNCE_MS);    //Declare the button
SensorToButton reverseButton(BUTTON_PIN2, DEBOUNCE_MS);
#define motor A0
unsigned long motorMillis = 0;
MusicWithoutDelay buzzer(song);
MusicWithoutDelay buzzer2(song2);
void setup()
{
  Serial.begin(9600);
  Serial.println(F("Let's play some music!  Type 'p' to pause/play, and 'b' to reverse/forward the song! "));
  Serial.print(F("Buzzer is playing: ")); Serial.println(buzzer.getName());
  pinMode(motor, OUTPUT);  //percussion instrument
  buzzer.begin(CHB, TRIANGLE, ENVELOPE0, 0);
  buzzer2.begin(TRIANGLE, ENVELOPE0, 0);

  buzzer.overrideSustain(true); //let's override sustain and make some cool effects :)
  buzzer2.overrideSustain(true);

  buzzer2.reverse(!buzzer2.isBackwards());  //plays the song backwards(since it was already backwards to begin with, this function reassures you that the song will sound as if it was playing forwards)
  //if you comment this out, the speaker will play the song in reverse and forwards...AT the SAME TIME!!! >:)  (it sounds bad)
}
void loop()
{
  unsigned long cMillis = millis();
  pauseButton.read();
  reverseButton.read();
  buzzer.update();
  buzzer2.update();
  if (Serial.available()) {
    char str = Serial.read();
    switch (str) {
      case 'p':
        Pause();
        if (!buzzer.isPaused()) {
          Serial.println(F("Resuming"));
        }
        else {
          Serial.println(F("Paused"));
        }
        break;
      case 'b':
        backwards();
        if (buzzer.isBackwards())
          Serial.println(F("Playing Backwards"));
        else
          Serial.println(F("Playing Forwards"));
        break;
    }
  }
  if (pauseButton.wasReleased()) {
    Pause();
  }
  if (reverseButton.wasReleased()) {
    backwards(); //backwards
  }
  if (cMillis - motorMillis >= 35) {
    digitalWrite(motor, LOW);
  }
  if (buzzer.isNote()) {
    digitalWrite(motor, HIGH);
    motorMillis = cMillis;
  }
}
void Pause() {
  buzzer.pause(!buzzer.isPaused());
  buzzer2.pause(!buzzer2.isPaused());
}
void set_Sustain(int m) {
  buzzer.setSustain(m);
  buzzer2.setSustain(m);
}
void backwards() {
  buzzer.reverse(!buzzer.isBackwards());
  buzzer2.reverse(!buzzer2.isBackwards());
  if (buzzer.isBackwards()) {
    set_Sustain(REV_SUSTAIN);
  }
  else {
    set_Sustain(SUSTAIN);
  }
}
