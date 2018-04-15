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
   
   Triplets= 1/3
   Now, remember these values.
   You will need them in the next step.
 ## The Arduino Code
 
   Now that you know the values of the basic notes in music, lets make some noise!
 The Music file is stored in a char pointer.  For example, in the Basic sketch, char * song is the char pointer/storage.
 
 ![alt text](https://raw.githubusercontent.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary/master/char%20song.PNG "storage Variable")

### Format(RTTL or Ring Tone Transfer Language)
  char * name =  " name of song :d=4,o=5,b=160,f=be,s=a: a,b,c1#,d1_,c-1";
  
  1.The name of the Song can be set before the first semicolon. **(optional)**
  
  2.The settings for the song are set inbetween the first and second semicolon **(optional)**
    * d= The default duration.  So any note(letter) without a number in front will automatically be assigned with the default duration
    * o= The default Ocatve.
    * s= The sharps
      * s=aeb, means all a's, e's, and b's in the song are sharps
    * f= The Flats
      * f=aeb, means all a's, e's, and b's in the song are flats
      
  3.The song's notes are made after the second semicolon. **(required)**
  * there are only 7 possible letters. a,b,c,d,e,f,g
  * the duration of each note is made before the letter
    * ex: 4f is a quarter note
  * The duration can be determined by this formula.
    * **Formula: 4/(note value)= duration.  So an eight note would be 4/(1/2) = 8.**
  * To name a letter a flat put an underscore _ right after the letter
    * ex: b_
  * To name a letter a sharp put a # right after the letter
    * ex: c#
  * To raise a note up an Octave, put the number of octaves to be raised after the Sharp/Flats/Letter
    * ex: b1 
  * To drop a note down an Octave, put a minus sign and then the number of octaves to be dropped after the Sharp/Flats/Letter
    * ex: b-1
Lets talk about some examples to understand the format of the song file.  

### Examples
1. Question: Lets try to play the letter C 
   
   Answer: char * name = "::c";

2. Question: Lets try to play the letter C 
   
   Answer: char * name = "::c";
