//https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary
/*Example for the MusicWithoutDelay Library by Nathan Ramanathan. nathan6ramanathan@gmail.com
This sketch demostrates the newSong function's advantage.  It can be used to skip to the beginning of a song or a new song can be chosen.
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
#include <NoodleSynth.h>
synthEngine mixer(20E3);
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4  // GPIO0 for esp8266
Adafruit_SSD1306 display(OLED_RESET);
const char song[] PROGMEM = {":d=12:c,d,e,e,f,g,e,f,g,p"}; //triplets
const char song2[] PROGMEM = {":d=12:e,f,g,g,a,b,g,a,b,p"};
bool songToggle;
MusicWithoutDelay buzzer(song);
MusicWithoutDelay buzzer2(song2);
double totalTime = 0;
void setup() {
  // put your setup code here, to run once:
  buzzer.begin(CHB, TRIANGLE, ENVELOPE0, 0);
  buzzer2.begin(TRIANGLE, ENVELOPE0, 0);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
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
  buzzer.update();
  buzzer2.update();
  if (millis() - oledMillis >= 250) {
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
    oledMillis = millis();
  }
}
