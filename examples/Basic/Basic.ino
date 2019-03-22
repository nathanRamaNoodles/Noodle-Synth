//Type 's' in serial monitor, you should see something :D  (This proves that my library doesn't use delay)
/*Example for the MusicWithoutDelay Library by Nathan Ramanathan. nathan6ramanathan@gmail.com
   This simple sketch is the most basic thing my library does effectively.
   It plays the C Major scale, with each note being a quarter note.
   The song stops playing when the instrument is done playing forward and backward once.
   Obviously, you can do other things while the song is being played.
   For example, talk to Serial, read buttons, display images on an OLED screen, etc...
*/

/*MIT License

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
  SOFTWARE.*/
//To learn more about this project go to https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
#include <NoodleSynth.h>
synthEngine mixer(20E3);
const char song[] PROGMEM = "::e,8f,8g,8a,8b,8c1,8d1,8e1";  //the C major scale. This format is known as RingTone Transfer Language or RTTL(It was used by Nokia's phone company).
const char song2[] PROGMEM = ":d=4:c,d,e,f";
const char song3[] PROGMEM = ":o=6,d=2:16a,16b,8a,4b";
const char song4[] PROGMEM = ":o=5,d=2:32g,32a,32b,32c";
MusicWithoutDelay instrument(song);          //d=4 means that every note without a number in front of the letter is assumed to be a quarter note.
MusicWithoutDelay instrument2(song2);
MusicWithoutDelay instrument3(song3);
MusicWithoutDelay instrument4(song4);
void setup() {                               //For details on the RTTL format, look at https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary documentation
  // put your setup code here, to run once:
  instrument.begin(CHB, TRIANGLE, ENVELOPE0, 0);
  instrument2.begin(TRIANGLE, ENVELOPE0, 0);
  instrument3.begin(TRIANGLE, ENVELOPE0, 0);
  instrument4.begin(TRIANGLE, ENVELOPE0, 0);
  Serial.begin(115200);
  unsigned long t = instrument.getTotalTime();  //spits out total time in milliseconds
  Serial.println(F("Type '1','2','3', or '4' to mute the corresponding instrument."));

  Serial.print("Total Time: "); Serial.println(t / 1E3);
}

void loop() {
  // put your main code here, to run repeatedly:
  instrument.update();  //but its important to recognize that this library depends
  instrument2.update();
  instrument3.update();
  instrument4.update();
  if (instrument.isEnd()) {          //on your main code not having delay().  Also, it increases stability, so your welcome :D
    //    instrument.pause();     //pauses song
    //    instrument2.pause();
    //    instrument3.pause();
    //    instrument4.pause();
  }
  if (Serial.available()) {
    char str = Serial.read();
    switch (str) {
      case 's':  //type 's' in Serial monitor.
        //instrument.skipTo(instrument.getTotalTime() * (0.50)); //skip to halfway point
        Serial.println("What's it to ya?  I don't use delay, and you shouldn't either.\n Star this project on Github, and spread this good news to others.");
        break;
      case '1':  //type 's' in Serial monitor.
        instrument.mute(!instrument.isMuted());
        break;
      case '2':  //type 's' in Serial monitor.
        instrument2.mute(!instrument2.isMuted());
        break;
      case '3':  //type 's' in Serial monitor.
        instrument3.mute(!instrument3.isMuted());
        break;
      case '4':  //type 's' in Serial monitor.
        instrument4.mute(!instrument4.isMuted());
        break;
    }
  }
}
