/*Example for the MusicWithoutDelay Library by Nathan Ramanathan. nathan6ramanathan@gmail.com
   This sketch demostrates the newSong function's advantage.  It can be used to skip to the beginning of a song or a new song can be chosen.
   Obviously, you can do other things while the song is being played.
   For example, talk to Serial, read buttons, display images on an OLED screen, etc...
*/
//More info at https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
#include <MusicWithoutDelay.h>
#include <Tone.h>                //https://github.com/bhagman/Tone
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4  // GPIO0 for esp8266
Adafruit_SSD1306 display(OLED_RESET);
Tone myTone;
Tone myTone2;
char *song = ":d=12:c,d,e,e,f,g,e,f,g,p";  //triplets
char *song2 = ":d=12:e,f,g,g,a,b,g,a,b,p";
bool songToggle;
MusicWithoutDelay buzzer(song);
MusicWithoutDelay buzzer2(song2);
double totalTime = 0;
void setup() {
  // put your setup code here, to run once:
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  myTone.begin(11);
  myTone2.begin(10);
  totalTime =  buzzer.getTotalTime();
  display.clearDisplay();
  // text display tests
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("TotalTime: ");
  display.setCursor(0, 17);
  display.print(totalTime / 1E3); display.print(" sec");
  display.setTextSize(2);
  display.setCursor(67, 39);
  display.println(" v1.0");
  display.display();
  delay(2000);
  display.clearDisplay();
}
unsigned long oledMillis = 0;
void loop() {
  // put your main code here, to run repeatedly:
  unsigned long cMillis = millis();
  buzzer.play(cMillis, myTone);
  buzzer2.play(cMillis, myTone2);
  if (cMillis - oledMillis >= 250) {
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    double currentTime = buzzer.getCurrentTime() / 1E3;
    display.println(currentTime); display.print("sec");
    display.setTextSize(2);
    display.setCursor(67, 39);
    display.println(" v1.0");
    display.display();
    oledMillis = cMillis;
  }
}
