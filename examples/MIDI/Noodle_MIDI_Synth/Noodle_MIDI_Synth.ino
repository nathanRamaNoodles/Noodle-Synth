/*
   Noodle Synth was made by Nathan Ramanathan.
   GNU license 2019
*/
/////////////////////////////////////////////////////////----Libraries-------//////////////////////////////////////////////////////////////////
#include <NoodleSynth.h> //contains list of this libraries dependencies
#include <MIDI.h>
#include "settings.h"
#include "MIDI_callbacks.h"
/////////////////////////////////////-------SETUP-------////////////////////////////////////////////////////
void setup()
{
  pinMode(LED, OUTPUT);

#if defined(__arm__) && defined(TEENSYDUINO) && defined(USB_MIDI_MODE)
  usbMIDI.setHandleNoteOff(handleNoteOff);
  usbMIDI.setHandleNoteOn(handleNoteOn);
  usbMIDI.setHandleControlChange(myControlChange);
  usbMIDI.setHandleProgramChange(instrumentChange);
  usbMIDI.setHandlePitchChange(pitchChange);
#endif

  midibench.setHandleNoteOff(handleNoteOff);
  midibench.setHandleNoteOn(handleNoteOn);
  midibench.setHandleControlChange(myControlChange);
  midibench.setHandleProgramChange(instrumentChange);
  midibench.setHandlePitchBend(pitchChange);
  midibench.begin(MIDI_CHANNEL_OMNI);

  for (int i = 0; i < NUM; i++) {
    mixer.begin(i, pins[i % (sizeof(pins) / sizeof(pins[0]))]);
    mixer.setupVoice(i, RAMP, 60, ENVELOPE0, 127, 64);
#if defined(SUSTAIN_TYPE)
    mixer.setSustain(i, SUSTAIN_TYPE);
#endif
    mixer.setChannel(i, 1);

  }
#if defined(__AVR_ATmega2560__)||defined(__arm__) && defined(TEENSYDUINO)
  Serial1.begin(31250);
  Serial.begin(115200);
#if defined(DEBUG)
  Serial.println("Done Setting up");
#endif
#else
#   if defined(HAIRLESS_MIDI)
  Serial.begin(38400);
#   else
  Serial.begin(31250);
#   endif
#endif

}
/////////////////////////////////////-------LOOP-------////////////////////////////////////////////////////
void loop()
{
#if defined(__arm__) && defined(TEENSYDUINO) && defined(USB_MIDI_MODE)
  usbMIDI.read();
#endif

  midibench.read();
  for (int i = 0; i < NUM; i++) {
#if defined(VOLUME_VELOCITY) || defined(VOLUME_MIDI) || defined(VOLUME_potentiometer)
    int val = analogRead(potPin);
    val = map(val, 0, 1023, 0, 127); //some pianos don't offer indiviual volume control, so you might need to use a potentionmeter to adjest the volume from here.
#   if defined(VOLUME_VELOCITY)
    masterVolume = val;
#   else
#       if !defined(SPECIFIC_WAVE_ONLY)
    switch (mixer.getWave(i)) { //RAMP and SQUARE are naturally louder than SINE and TRIANGLE.  So this piece of code should even out the volumes.
      case TRIANGLE:
        val -= 5;
        break;
      case RAMP:
        val -= 5;
        break;
      case SQUARE:
        val -= 5;
        break;
    }
#       endif
    mixer.setVolume(i, val);
#   endif
#elif !defined(VOLUME_MIDI)
    mixer.setVolume(i, 127);
#endif
  }
}
