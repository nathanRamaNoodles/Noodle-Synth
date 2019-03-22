#include "Arduino.h"
#include "SDpiston.h"

synthEngine * SDpiston::noodleSynth = NULL; //This is extremely important for initiating a static private Object

SDpiston::SDpiston(char *filename){
  wavFile = SD.open(filename);
  pistonNumber = noodleSynth->getAvailablePiston();
  paused = false;
}
void SDpiston::start(){
  paused = false;
  wavFile.seek(44);
}
void SDpiston::pause(){
  // paused != paused;
}
int SDpiston::read(){
  return (int)wavFile.read();
}
bool SDpiston::isPaused(){
  return paused;
}
int SDpiston::getPistonNumber(){
  return pistonNumber;
}
