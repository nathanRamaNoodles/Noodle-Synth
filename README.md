# MusicWithoutDelay Library(v2.0.0)
* [Quick start here](https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary#quick-start)
* This library was inspired by [Bhagman's RTTL Arduino sketch](https://github.com/bhagman/Tone/blob/master/examples/RTTTL/RTTTL.pde).

## Updates
* This library has been upgraded to version 2.0.0
  * previous versions [found here](https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary/releases).
* Improvements
  * 4-note polyphony using only one timer
  * velocity triggered notes
  * Uses PROGMEM instead of char arrays **(Decreases SRAM usage)**
  * Different types of waves
    * Square
    * Triangle
    * Ramp
    * Saw
    * Sine
    * Noise
  * Modulate instruments for funny musical effects

## Demonstration

Check out [this video](https://youtu.be/uoHhlrqZYDI) for a Legend of Zelda music demostration.  In the video, an Arduino Nano outputs two voices. The arduino uses a vibrating motor as a percussion instrument, and an RGB LED to add some fire to the show.  The delay function is not used at all :)

## Advantages
* This Arduino Library makes an Arduino board play music in the background while running your program(Assuming your code doesn't have any delay).  
  * **So you can view the Serial Monitor, display stuff on an OLED, and read buttons while this library plays your music in the background**
  * This library doesn't use the delay function; it uses a similar technique to [Arduino's BlinkWithoutDelay sketch](https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay).
* You do not need any shield or extra hardware for this library.
* **You can play 4 notes at the same time**
* Modulation effects added
* Pause/resume Song
* Play song forward and backwards!! :D
* skipTo a favorite part in the song(The hardest to program, but it was worth it)

## Disadvantages(Soon to be fixed)
* ~~Can't play more than two notes on many arduino boards.~~
* ~~Tone library creates timer conflicts with Servo library~~
* ~~I am trying to move away from the Tone library because many common libraries hate it.~~ :D
* Can play four notes on Arduino Uno.
  * Not working on Arduino Mega 2560.
* No volume control, yet.

## Quick Start
1. Install this library by downloading the zip folder.  Read [this Arduino Library Tutorial](https://www.arduino.cc/en/Guide/Libraries) to install the library.
2. Open the examples folder after installing the library.  
3. Choose the "Basics" sketch.
4. Attach a speaker as per the schematic below.
5. Upload the code.
6. The comments in the code should be helpful.  And try the other examples included with this library.

![alt text](https://raw.githubusercontent.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary/master/MusicWithoutDelay.png "Schematic")

# How to write Music

You may be wondering how one can write songs for the Arduino without any knowledge of Music!  Its actually quite interesting and fun.  First you need to know the value of the notes in Music.
![alt text](http://ezstrummer.com/ezriffs/demo/notes_rests.gif "Note Values")

Triplets= 1/3

Now, remember these values.
You will need them in the next step.

## The Arduino Code

Now that you know the values of the basic notes in music, let's make some noise!
The Music file is stored in the `PROGMEM`.

![alt text](https://raw.githubusercontent.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary/master/char%20song.PNG "storage Variable")

### Format(RTTL or Ring Tone Transfer Language)
`const char song[] PROGMEM =  " name of song : settings : notes";`
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

So using these format rules, we can make this ordered outline for the definition of a Note.
1. duration   (optional)

2. period     (optional)

3. letter     **(required)**

4. sharp/flat (optional)

5. octave     (optional)

6. "," or "+" **(required)**    

Let's use some examples to understand the format of the song file.  

## Examples
Assume `const char name[] PROGMEM`

1. Question: Lets try to play the letter C    
   * Answer:  name = "::c";

2. Question: Play the C major scale with wholenotes  
   * Answer:  name = "::c,d,e,f,g,a,b,c1";

3. Question: Play the C major scale with sixteenth notes
   * Answer:  name = ":d=16:c,d,e,f,g,a,b,c1";

4. Question: Play the C Major with different durations
   * Answer:  name = "::c,4d,2e,16f,g,8a,32b,c1";

5. Question: Play the C Major faster
   * Answer:  name = ":b=160:c,d,e,f,g,a,b,c1";

6. Question: Play some dotted quarter notes
   * Answer:  name = "::4.c,8.d,2.e";

7. Question: play a note every second
   * Answer:  name = ":d=4,b=60:c,p";  //bpm=60 means a quarter note = 1 second

8. Question: Slur every three notes
   * Answer:  name = "::a+b+c,c+d+e";

9. Question: Combine note with all possible options
   * Answer:  name = "::4.a_-1";

10. Question: Play notes with whitespaces(whitespaces make it easier to write music since it's easier on the eyes)
    * Answer:  name = ":: 12c , 12b , 12a , 4a , 4b , 4c , 2g , 2a , d";  //The library is user-friendly, it ignores spaces **(but don't put spaces in between the definitions of the notes)** So don't do this ":: 12 c # ,".

# Starting the speaker
In order to start our speaker, we need to call the `begin()` function.
You can call it two ways.
```
begin(int mode, int waveForm, int envelope, int mod)
```  
Where **mode** is the pin connected to the speaker.  It must be `CHA` or `CHB`.  **CHA is pin 11** on UNO, and **CHB is pin 3** on UNO.  
* `CHB` is more preferred

The **waveForm** can be `SINE`, `SQUARE`, `TRIANGLE`, `SAW`, `RAMP`, or `NOISE`
* `TRIANGLE` is recommended

The **envelope** can be `ENVELOPE0`, `ENVELOPE1`,`ENVELOPE2`, or `ENVELOPE3`.
* Default is `ENVELOPE0`


The **mod** can be any positive or negative number to create fancy musical effects.  **The further away you are from `0`, the fancier the song will sound.**
* Default is `0`.

```
begin(int waveForm, int envelope, int mod)
```
Use this format for more than one instrument.
# Functions
```
begin(int mode, int waveForm, int envelope, int mod)//mode can be CHA or CHB  
begin(int waveForm, int envelope, int mod)
--------------------------------------------------------
play()            // plays the song  
pause()           //pauses and resumes song
reverse()         //reverse direction of song being played
newSong(const char p[] PROGMEM)  //used to play a new song
skipTo(long index)//skips song to time suggested in milliseconds
mute()            // mutes instrument
--------------------------------------------------------
setBPM(int tempo)  //set speed of song
setOctave(int oct) //set octave
setWave(int waveShape) //set waveShape of instrument
setMod(int percent)    //modulate instrument for cool effects
--------------------------------------------------------
getTotalTime()    //gets totalTime of song in milliseconds
getCurrentTime()  //gets currentTime of song in milliseconds
getBPM()          //gets tempo of song
getOctave()       //gets the song's octave
getName()         //get name of song
isRest()          //returns true if song is playing a rest
isMuted()         //returns true if muted
isStart()         //returns true if song has reached the beginning
isEnd()           //returns true if song has reached the end
isPaused()        //returns true if song is paused
isNote()          //returns true if song is playing a note
isBackwards()     //returns true if song is playing backwards
```
# Constants:
```
//Use these constants for setting wave shapes
SINE
SQUARE
SAW
RAMP
TRIANGLE
NOISE
--------------------------------------------------------
//Use these constants for setting the speaker pin
CHA //pin 11 on Uno
CHB //pin 3 on Uno
--------------------------------------------------------
//Use these constants for setting the type of Envelope
ENVELOPE0
ENVELOPE1
ENVELOPE2
ENVELOPE3
```
