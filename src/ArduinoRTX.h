#ifndef ArduinoRTX_h
#define ArduinoRTX_h

#include "Arduino.h"

enum state { INIT, PLAY, NOTE, PAUSE, END };

class ArduinoRTX {
  private:
    int tonePin;
    enum state state = INIT;
    byte default_dur = 4;
    byte default_oct = 6;
    int bpm = 63;
    int num;
    long wholenote;
    long duration;
    char style = 'N';
    int loops = 0;
    byte note;
    byte scale;
    char *start;
    char *p;
    unsigned long deadline;

  public:
    ArduinoRTX(int pin, char *song);
    void tick();
};

#endif
