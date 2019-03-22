/*
   Note: USB MIDI can handle way more polyphony than MIDI over Serial.
*/
////////////////////////////////////////////////////////-------Settings------//////////////////////////////////////////////////////////////////

/////-----Optional------////////////
//#define DEBUG //Comment out to remove Serial Debugging

#define SPECIFIC_WAVE_ONLY RAMP //comment this out to allow different types of waveforms instead of one waveform.(SINE, TRIANGLE, RAMP, SQUARE)
//#define INFINITE      //comment this out to remove the infinite effect(Only works for Sustain_type = NONE)
//#define pitchWheel   //comment this out to remove pitchWheel.  this can lag the system on the UNO.
//#define pedalSustain  //comment this out to remove pedal sustain(**the pedal you use on your piano to make your notes sound longer**)

//#define VOLUME_potentiometer //**recommended** this allows you to control the volume with a pot
//#define VOLUME_VELOCITY //**This needs improvement** this allows the volume to be determined by your velocity of your keypress. The potentiometer acts as Master Volume
//#define VOLUME_MIDI //comment this out to remove volume control from MIDI interface on your computer or piano


/////-----Required------////////////
#define SUSTAIN_TYPE SUSTAIN //Other sustain types include: NONE, SUSTAIN, REV_SUSTAIN
#define potPin A0 //potentiometer connected to A0

/////-----Polyphony and Synth Sample Rate------////////////
#define NUM 6   //number of polyphonic notes you think the song will have(NOTE: you must change the MAXVOICE settings in synth.cpp to get even more polyphony)
synthEngine mixer(10E3); //sample rate(higher the sample rate, the better the quality. Also, lag will sneak in sooner or later)

/////---Pins-----/////////
int pins[] = {CHB}; //speaker audio pins
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
