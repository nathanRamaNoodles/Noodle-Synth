/*Example for the MusicWithoutDelay Library by Nathan Ramanathan. nathan6ramanathan@gmail.com
   This sketch demostrates the skipTo function's advantage.  It can skip to any point of the song :D
   It plays a small section of the legend of Zelda Song.
   Obviously, you can do other things while the song is being played.
   For example, talk to Serial, read buttons, display images on an OLED screen, etc...

   Pressing either button on pin 2 or 3 causes the song to skip to the beginning or anywhere.

   This is by far the most complex, beautiful, mathematical function I have created.  :')
   Use skipTo(long) wisely
*/
//To learn more about this project go to https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
#include <MusicWithoutDelay.h>
#include <Button.h>  //https://github.com/JChristensen/Button  acquired by JChristensen's popular button library
#include <Tone.h>                //https://github.com/bhagman/Tone
char *Soprano  = "Zelda:o=5,b=160,f=aeb:4b,4f+8.f,16b,16b,16c1,16d1,16e1,2f1,8p,8f1,12f1,12g_1,12a1,2b1,12p,12b1,12b1,12b1,12a1,12g_1,8.a1,16g_1,2f1,4f1,8e1,16e1,16f1,2g_1,8f1,8e1,8d_1,16d_1,16e1,2f1,8e1,8d_1,8c1,16c1,16d1,2e#1,4g1,8f1,16f,16f,8f,16f,16f,8f,16f,16f,8f,8f";
char *Bass     = ":b=160,f=aeb:4d,12d,12d,12c,8.d,16d,16d,16e,16f,16g,8.a,16b,16b,16c1,16d1,16e1,4f1,12a,12b,12c1,8.d_1,16g_,16g_,16a,16b,16c1,12d_1,12p,12d_1,12d_1,12c1,12b,8.d_,16a,12a,12a,12g_,8.a,16a,12a,12g_,12a,8g_,16g_,16f,8g_,16g_,16a,4b,8a,8g_,8f,16f,16e,8f,16f,16g_,4a,8g_,8f,4e#,8e#,16e#,16f,8g,16g,16a#,8b,8c1,8a#,16a#-1,16a#-1,8a#-1,16a#-1,16a#-1,8a#-1,16a#-1,16a#-1,8a#-1,8a#-1";
MusicWithoutDelay instrument(Soprano);          //o=5 means that the instrument will play at the 5th Octave. o=4 is default
MusicWithoutDelay instrument2(Bass);          //f=aeb, means all a's, e's, and b's are flats
Tone myTone;                                    //b=160, means the bpm(tempo) is 160, default is 100
Tone myTone2;
#define BUTTON_PIN 2       //Connect a tactile button switch (or something similar) from Arduino pin 2 to ground.
#define BUTTON_PIN2 3       //Connect a tactile button switch (or something similar) from Arduino pin 3 to ground.
#define PULLUP true        //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true        //Since the pullup resistor will keep the pin high
#define DEBOUNCE_MS 70     //A debounce time of 70 milliseconds usually works well for noisy button switches. if not, try 20.
Button beginningButton(BUTTON_PIN, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button randomButton(BUTTON_PIN2, PULLUP, INVERT, DEBOUNCE_MS);

void setup() {
  // put your setup code here, to run once:
  myTone.begin(11);        //attach both pins to same speaker with one 1k resistor to pin 11,
  myTone2.begin(10);       //and another 1k resistor to pin 10.
  Serial.begin(9600);
  Serial.print(F("Song's Name: ")); Serial.println(instrument.getName());  //prints name of song
  Serial.print(F("Total Time: ")); Serial.print(instrument.getTotalTime() / 1E3); Serial.println(F(" seconds")); //prints total Time of song
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long cMillis = millis();
  instrument.play(cMillis, myTone);
  instrument2.play(cMillis, myTone2);
  beginningButton.read();
  randomButton.read();
  if (beginningButton.wasPressed()) {
    Serial.println(F("Skipping to beginning"));
    instrument.skipTo(0);         //skips to 0 seconds(beginning)
    instrument2.skipTo(0);
    // instrument.skipTo(6*1E3);  //skips to 6 seconds
    //instrument2.skipTo(6*1E3);
  }
  if (randomButton.wasPressed()) {
    int r = random(0, 100);
    instrument.skipTo(instrument.getTotalTime() * (r / 100.0));    //random skip
    instrument2.skipTo(instrument2.getTotalTime() * (r / 100.0));
    Serial.print(F("Skipping to: ")); Serial.print(instrument.getCurrentTime() / 1E3); Serial.println(F(" seconds"));
  }
}

