/*
   Noodle Synth was made by Nathan Ramanathan.
   GNU license 2019
*/
/////////////////////////////////////////////////////////----Libraries-------//////////////////////////////////////////////////////////////////
#include <NoodleSynth.h> //contains list of this libraries dependencies
#include <MIDIUSB.h>
#include "settings.h"
#include "MIDI_callbacks.h"
/////////////////////////////////////-------SETUP-------////////////////////////////////////////////////////
void setup()
{
  for (int i = 0; i < NUM; i++) {
    mixer.begin(i, pins[i % (sizeof(pins) / sizeof(pins[0]))]);
    mixer.setupVoice(i, RAMP, 60, ENVELOPE0, 127, 64);
#if defined(SUSTAIN_TYPE)
    mixer.setSustain(i, SUSTAIN_TYPE);
#endif
    mixer.setChannel(i, 1);

  }
#if defined(DEBUG)
  Serial.begin(115200);
  Serial.println("Done Setting up");
#endif

}
/////////////////////////////////////-------LOOP-------////////////////////////////////////////////////////
void loop()
{
  usbMIDI_read();

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

void usbMIDI_read() {
  midiEventPacket_t rx = MidiUSB.read();
  switch (rx.header) {
    case 0:
      break; //No pending events

    case 0x9:
      handleNoteOn(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //pitch
        rx.byte3         //velocity
      );
      break;

    case 0x8:
      handleNoteOff(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //pitch
        rx.byte3         //velocity
      );
      break;

      //    case 0xB:
      //      myControlChange(
      //        rx.byte1 & 0xF,  //channel
      //        rx.byte2,        //control
      //        rx.byte3         //value
      //      );
      //      break;

      //    case 0xC:
      //      instrumentChange(
      //        rx.byte1 & 0xF,  //channel
      //        rx.byte2,        //control
      //        rx.byte3         //value
      //      );
      //break;
      //    case 0xE:
      //      pitchChange(
      //        rx.byte1 & 0xF,  //channel
      //        rx.byte2,        //control
      //        rx.byte3         //value
      //      );
      //      break;
#if defined(DEBUG)
    default:
      Serial.print("Unhandled MIDI message: ");
      Serial.print(rx.header, HEX);
      Serial.print("-");
      Serial.print(rx.byte1, HEX);
      Serial.print("-");
      Serial.print(rx.byte2, HEX);
      Serial.print("-");
      Serial.println(rx.byte3, HEX);
#endif
  }
}
