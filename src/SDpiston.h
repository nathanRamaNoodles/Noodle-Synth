#ifndef SDpiston_h
#define SDpiston_h
#include <SD.h>
#include "synthEngine.h"
class SDpiston
{
public:
  SDpiston(char *filename);
  void start();
  void pause();
  int read();
  bool isPaused();
  int getPistonNumber();
private:
  static synthEngine *noodleSynth;
  uint8_t pistonNumber;
  File wavFile;
  bool paused;
};
#endif
