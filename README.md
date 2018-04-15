# MusicWithoutDelay Library

This Arduino Library is makes an Arduino board play music in the background while running your program(Assuming your code doesn't have any delay).

And Yes, **you can play more than one note at the same time**.  Check out the video below for a Legend of Zelda music demostration.  In the video, an Arduino Nano outputs two voices. The arduino uses a vibrating motor as a percussion instrument, and an RGB LED to add some fire to the show.  The delay function is not used at all :)

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
  char * song =  " name of song : settings : notes";
  #### Name(Optional)
  
  * The name of the Song can be set before the first semicolon.
  
  #### The Settings(Optional)
  
  * The settings for the song are set in-between the first and second semicolon(You may use a comma or white space between each setting option) 
    * d= The default duration. So any note(letter) without a number in front will automatically be assigned with the default duration
      * By default, d=1 which is a wholenote
    * o= The default Octave.
      * By default, o=4 which is middle C on the Piano
    * b= the BPM(Beat per Minute) or the Tempo of the Song
      * By default, b=100, which is normal tempo in music
    * s= The sharps
      * Ex: s=aeb, means all a's, e's, and b's in the song are sharps
    * f= The Flats
      * Ex: f=aeb, means all a's, e's, and b's in the song are flats
  
  #### Notes(Required)
  
  * The song's notes are made after the second semicolon.
    * there are only 7 possible letters. a,b,c,d,e,f,g
    * A rest uses the letter "p"
    * the duration of each note is made before the letter
      * ex: 4f is a quarter note
    * put a period for dotted notes
      * ex: 2.f is a dotted half note
    * The duration can be determined by this formula.
      * **Formula: 4/(note value)= duration.  So an eight note would be 4/(1/2) = 8.**
    * To name a letter a flat put an underscore _ right after the letter
      * ex: b_
    * To name a letter a sharp put a # right after the letter
      * ex: c#
    * To raise a note up an Octave, put the number of octaves to be raised after the Sharp/Flats/Letter
      * ex: b1 
      * ex2: c#1
    * To drop a note down an Octave, put a minus sign and then the number of octaves to be dropped after the Sharp/Flats/Letter
      * ex: b-1
      * ex: c#-2
    * Put a comma to indicate the next note
      * ex: b,c  means that the notes will be played one after the other(taking a breath)
    * Put a plus sign to indicate the next note is a slur with the current note
      * ex: b+c  means that the notes will be played like a slur in music(without taking a breath)
    
So using these format rules, we can make this ordered outline
1. duration   (optional)
2. period     (optional)
3. letter     **(required)**
4. sharp/flat (optional)
5. octave     (optional)
6. "," or "+" (required)    
    
Let's use some examples to understand the format of the song file.  

### Examples
1. Question: Lets try to play the letter C    
   * Answer: char * name = "::c";

2. Question: Play the C major scale with wholenotes  
   * Answer: char * name = "::c,d,e,f,g,a,b,c1";

3. Question: Play the C major scale with sixteenth notes
   * Answer: char * name = ":d=16:c,d,e,f,g,a,b,c1";

4. Question: Play the C Major with different durations
   * Answer: char * name = "::c,4d,2e,16f,g,8a,32b,c1";

5. Question: Play the C Major faster
   * Answer: char * name = ":b=160:c,d,e,f,g,a,b,c1";

6. Question: Play some dotted quarter notes
   * Answer: char * name = "::4.c,8.d,2.e";

7. Question: play a note every second
   * Answer: char * name = ":d=4,b=60:c,p";  //bpm=60 means a quarter note = 1 second

8. Question: Slur every three notes
   * Answer: char * name = "::a+b+c,c+d+e";


