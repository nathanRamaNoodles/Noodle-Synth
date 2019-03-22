#ifndef NoodleSettings_h
#define NoodleSettings_h

#define twelveRoot 1.059463
#define pitchBendRoot 1.0001693  // after many tests, this number is what I found most accurate

//default sample rate(this is used when user doesn't specify the sample rate)
#define default_sample_rate 20E3 //20,000 Hz
/*****maxVOICES settings (These settings are used when the user doesn't specify how many note polyphony they want)***/
#if defined(__AVR_ATmega2560__)
#define maxVOICES 16  //Arduino Mega2560; best performance would be 8
#elif defined(__AVR__)
#define maxVOICES 8  //Arduino Uno, Mini, Micro, Pro Micro, Nano, and Teensy 2.0; best performance would be 4, but change this to your microcontroller's processing power :)
#elif defined(__arm__) && defined(TEENSYDUINO)
#define maxVOICES 64 //Teensy 3.++; Teensy boards can handle a whooping 30+ simulentaneous voices(Teensy is a powerful 32-bit chip)
#elif defined(ESP8266)
#define maxVOICES 2 //ESP8266; best performance would be 6
#elif defined(ESP32)
#define maxVOICES 8 //ESP32; best performance would be 10
#else
#error "Hey! This library isn't compatible with this board" //other unknown boards
#endif
/***end of MaxVoice settings****/
#define pitchOffset 1
/*-----------------------------------------------------------------------------------------------------------------*/
/*****ATmega328p only*****
* For some boards, if your SD card's pins take the audio pin, then uncomment the "USE_TIMER1".
* Timers exist on different pins, by default timer2 is in use for Arduino UNO.  CHA: 11,  CHB: 3.
* So Timer 1 on Arduino Uno will output at pins CHA: 9,  CHB: 10.
*/

// #define USE_TIMER1

/*-----------------------------------------------------------------------------------------------------------------*/
#endif
