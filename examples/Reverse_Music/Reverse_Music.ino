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
//To learn more about this project go to https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary

#include <Tone.h>    //https://github.com/bhagman/Tone
#include <Button.h>  //https://github.com/JChristensen/Button
#include <MusicWithoutDelay.h>
//Too many chars can cause Memory problems.  Be cautious ;)
char *song = ":d=16,o=6,b=150:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,16g,8p,16g,8p,16a#,16p,16c1,16p,16g,8p,16g,8p,16f,16p,16f#,16p,16g,8p,16g,8p,16a#,16p,16c1,16p,16g,8p,16g,8p,16f,16p,16f#,16p,16a#,16g,2d,32p,16a#,16g,2c#,32p,16a#,16g,2c,16a#-1,8c,2p,32p,16a#-1,16g-1,2f#,32p,16a#-1,16g-1,2f,32p,16a#-1,16g-1,2e,16d#,8d";
char *song2 = ":d=16,o=4,b=150:8d,16d#,2e,16g-1,16a#-1,32p,2f,16g-1,16a#-1,32p,2f#,16g-1,16a#-1,32p,2p,8c,16a#-1,2c,16g,16a#,32p,2c#,16g,16a#,32p,2d,16g,16a#,16p,16f#,16p,16f,8p,16g,8p,16g,16p,16c1,16p,16a#,8p,16g,8p,16g,16p,16f#,16p,16f,8p,16g,8p,16g,16p,16c1,16p,16a#,8p,16g,8p,16g,32g,32f#,32f,32e,32d#,32d,32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d";
#define BUTTON_PIN 2       //Connect a tactile button switch (or something similar) from Arduino pin 2 to ground.
#define BUTTON_PIN2 3       //Connect a tactile button switch (or something similar) from Arduino pin 3 to ground.
#define PULLUP true        //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true        //Since the pullup resistor will keep the pin high unless the
//switch is closed, this is negative logic, i.e. a high state
//means the button is NOT pressed. (Assuming a normally open switch.)
#define DEBOUNCE_MS 70     //A debounce time of 70 milliseconds usually works well for noisy button switches.
Button pauseButton(BUTTON_PIN, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button reverseButton(BUTTON_PIN2, PULLUP, INVERT, DEBOUNCE_MS);
Tone myTone;
Tone myTone2;
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
  myTone.begin(11);        //attach both pins to same speaker with one 1k resistor to pin 11,
  myTone2.begin(10);       //and another 1k resistor to pin 10.
  buzzer2.reverse();  //plays the song backwards(since it was already backwards to begin with, this function reassures you that the song will sound as if it was playing forwards)
  //if you comment this out, the speaker will play the song in reverse and forwards...AT the SAME TIME!!! >:)  (it sounds bad)
}
void loop()
{
  unsigned long cMillis = millis();
  pauseButton.read();
  reverseButton.read();
  buzzer.play(cMillis, myTone);
  buzzer2.play(cMillis, myTone2);
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
  buzzer.pause();
  buzzer2.pause();
}
void backwards() {
  buzzer.reverse();
  buzzer2.reverse();
}
