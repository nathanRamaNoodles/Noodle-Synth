/*
   Note: USB MIDI can handle way more polyphony than MIDI over Serial.
*/
////////////////////////////////////////////////////////-------Settings------//////////////////////////////////////////////////////////////////

/////-----Optional------////////////
//#define DEBUG //Comment out to remove Serial Debugging

//#define SPECIFIC_WAVE_ONLY RAMP //comment this out to allow different types of waveforms instead of one waveform.(SINE, TRIANGLE, RAMP, SQUARE)
//#define INFINITE      //comment this out to remove the infinite effect(Only works for Sustain_type = NONE)
#define pitchWheel   //comment this out to remove pitchWheel.  this can lag the system on the UNO.
#define pedalSustain  //comment this out to remove pedal sustain(**the pedal you use on your piano to make your notes sound longer**)

#define VOLUME_potentiometer //**recommended** this allows you to control the volume with a pot
//#define VOLUME_VELOCITY //**This needs improvement** this allows the volume to be determined by your velocity of your keypress. The potentiometer acts as Master Volume
//#define VOLUME_MIDI //comment this out to remove volume control from MIDI interface on your computer or piano

//#define HAIRLESS_MIDI //comment this out to use standard MIDI   http://projectgus.github.io/hairless-midiserial/  //(this software is used for devices with no USB support like the UNO)
#define USB_MIDI_MODE //comment out to remove USB MIDI capabilites


/////-----Required------////////////
#define SUSTAIN_TYPE SUSTAIN //Other sustain types include: NONE, SUSTAIN, REV_SUSTAIN
#define potPin A0 //potentiometer connected to A0

/////-----Polyphony and Synth Sample Rate------////////////
#if defined(__arm__) && defined(TEENSYDUINO) //TEENSY
#define NUM 36   //number of polyphonic notes you think the song will have(NOTE: you must change the MAXVOICE settings in synth.cpp to get even more polyphony)
synthEngine mixer(20E3); //sample rate(higher the sample rate, the better the quality. Also, lag will sneak in sooner or later)
#elif defined(__AVR_ATmega2560__) //MEGA2560
#define NUM 4
synthEngine mixer(20E3);
#else     //UNO, NANO, Pro Micro, etc
#define NUM 8 //12
synthEngine mixer(13E3); //10E3
#endif

/////-----MIDI Serial------////////////
#if defined(__AVR_ATmega2560__)//Arduino Mega
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midibench); //Connect your MIDI's RX,TX to the assigned Serial1.
#elif defined(AVR)  //Arduino Uno/Mini etc
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, midibench);
#else  //Teensy or others
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midibench);
#endif

/////---Pins-----/////////
#if defined(__arm__) && defined(TEENSYDUINO) //TEENSY
int pins[] = {5, 6, 9}; //speaker audio pins
#else //Arduino UNO/MEGA/NANO
int pins[] = {CHB}; //speaker audio pins
#endif
const int LED = 13; // LED pin
///////////////////////////////---------------------------End of Settings-------------------------------------/////////////////////////////////


///////////////////////////////----------------------------Don't touch the stuff below---------------------------////////////////////////////
const int NumChannels = (NUM > 16) ? 16 : NUM; //number of channels used in your song, max is usually 16.  Keeping this small is best when you want more polyphony per channel
int masterVolume = 127;
int lastUsedVoice = 0;
#define percussionChannel 10
struct storedMIDI {
bool isPedalSustain = false;
int bendFactor = 0;
int currentVolume = 127;
#if defined(SPECIFIC_WAVE_ONLY)
int waveForm = SPECIFIC_WAVE_ONLY;
#else
int waveForm = SINE;
#endif
};
storedMIDI tempData[NumChannels];
int waveForms[4] = {SINE, TRIANGLE, RAMP, SQUARE};
///////////////////////////////----------------------------Don't touch the stuff above---------------------------////////////////////////////
