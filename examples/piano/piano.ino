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
//To learn more about this project go to https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary

#include <Button.h>  //https://github.com/JChristensen/Button  acquired by JChristensen's popular button library
#include <MusicWithoutDelay.h>
#include <Tone.h>  //https://github.com/bhagman/Tone
//char *note1 = "C Scale:d=4:c,d,e,f,g,a,b,c1";   //plays c scale,  You can add more notes to get creative. //d=4 means that every note without a number in front of the letter is assumed to be a quarter note.
//char *note2 = ":d=4:e,f,g,a,b,c1,d1,e1";   //plays c scale starting at e
char *note1 = "::c";   //plays c ,  You can add more notes to get creative.
char *note2 = "::e";   //plays e
MusicWithoutDelay pianoKey(note1);
MusicWithoutDelay pianoKey2(note2);
Tone myTone;
Tone myTone2;
#define BUTTON_PIN 2       //Connect a tactile button switch (or something similar) from Arduino pin 2 to ground.
#define BUTTON_PIN2 3       //Connect a tactile button switch (or something similar) from Arduino pin 3 to ground.
#define PULLUP true        //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true        //Since the pullup resistor will keep the pin high
#define DEBOUNCE_MS 70     //A debounce time of 70 milliseconds usually works well for noisy button switches. if not, try 20.
Button firstKey(BUTTON_PIN, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button secondKey(BUTTON_PIN2, PULLUP, INVERT, DEBOUNCE_MS);
void setup() {
  // put your setup code here, to run once:
  myTone.begin(11);        //attach both pins to same speaker with one 1k resistor to pin 11,
  myTone2.begin(10);       //and another 1k resistor to pin 10.
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long cMillis = millis();
  pianoKey.play(cMillis, myTone);
  pianoKey2.play(cMillis, myTone2);
  firstKey.read();
  secondKey.read();
  buttonToPiano(firstKey, pianoKey);
  buttonToPiano(secondKey, pianoKey2);
}

void buttonToPiano(Button b, MusicWithoutDelay &m) {  //Must put & before object, because we must point to the address of the Object in the Arduino Memory
  if (b.isPressed()) {
    if (m.isPaused())    //if note is paused, resume playing
      m.pause();
  }
  else if (!m.isPaused()) {  //if note is playing, pause it
    m.pause();
  }
}

