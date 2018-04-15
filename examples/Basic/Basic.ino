//Type 's' in serial monitor, you should see something :D  (This proves that my library doesn't use delay)
/*Example for the MusicWithoutDelay Library by Nathan Ramanathan. nathan6ramanathan@gmail.com
   This simple sketch is the most basic thing my library does effectively.
   It plays the C Major scale, with each note being a quarter note.
   The song stops playing when the instrument is done playing forward and backward once.
   Obviously, you can do other things while the song is being played.
   For example, talk to Serial, read buttons, display images on an OLED screen, etc...
*/
//To learn more about this project go to https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
#include <MusicWithoutDelay.h>
#include <Tone.h>                //https://github.com/bhagman/Tone
char *song = ":d=4:c,d,e,f,g,a,b,c1";  //the C major scale. This format is known as RingTone Transfer Language or RTTL(It was used by Nokia's phone company).
MusicWithoutDelay instrument(song);          //d=4 means that every note without a number in front of the letter is assumed to be a quarter note.
Tone myTone;
void setup() {                               //For details on the RTTL format, look at https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary documentation
  // put your setup code here, to run once:
  myTone.begin(11);  //connect speaker with 200 ohm resistor to pin 11.
  Serial.begin(9600);
  unsigned long t = instrument.getTotalTime();  //spits out total time in milliseconds

  Serial.print("Total Time: "); Serial.println(t / 1E3);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long cMillis = millis();  //I know millis() can be implemented in my library,
  instrument.play(cMillis, myTone);  //but its important to recognize that this library depends
  if (instrument.isEnd()) {          //on your main code not having delay().  Also, it increases stability, so your welcome :D
    instrument.reverse();     //reverses song when end is reached
  }
  if (instrument.isStart()) {
    instrument.pause();  //stops the song when beginning is reached while song was played backwards
  }
  if (Serial.available()) {
    char str = Serial.read();
    switch (str) {
      case 's':  //type 's' in Serial monitor.
        //instrument.skipTo(instrument.getTotalTime() * (0.50)); //skip to halfway point
        Serial.println("What's it to ya?  I don't use delay, and you shouldn't either.\n Star this project on Github, and spread this good news to others.");
        break;
    }
  }
}
