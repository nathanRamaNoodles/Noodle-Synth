# MusicWithoutDelay Library

This Arduino Library is makes an Arduino board play music in the background while running your program(Assuming your code doesn't have any delay).

## Quick Start(For First Time Explorers)
   1. Install this library by downloading the zip folder.  Read [this Arduino Library Tutorial](https://www.arduino.cc/en/Guide/Libraries) to install the library.
   2. Open the examples folder after installing the library.  
   3. Choose the "Basics" sketch.
   4. Attach a speaker with a 200 ohm resistor to pin 11, then attach the other end of the speaker to ground.
   5. Upload the code
   
![alt text](https://raw.githubusercontent.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary/master/MusicWithoutDelay.png "Schematic")

## How to write Music

   You may be wondering how one can write songs for the Arduino without any knowledge of Music!  Its actually quite interesting and fun.  First you need to know the value of the notes in Music.
   ![alt text](http://ezstrummer.com/ezriffs/demo/notes_rests.gif "Note Values")
   
   Now, remember these values.
   
 ### The Arduino Code
 
   Now that you know the values of the basic notes in music, lets make some noise!
 The Music file is stored in a char pointer.  For example, in the Basics sketch, char * song is te char pointer or storage.
 
