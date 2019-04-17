#include "Arduino.h"
#include "ArduinoRTX.h"

#define OCTAVE_OFFSET 0

#define isdigit(n) (n >= '0' && n <= '9')
#define issep(n) (n == ':' || n == ',')

// These values can also be found as constants in the Tone library (Tone.h)
int notes[] = { 0,
  262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
  523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
  1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
  2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,
};

ArduinoRTX::ArduinoRTX(int pin, char *song) {
  tonePin = pin;
  p = song;
  pinMode(tonePin, OUTPUT);
}

void ArduinoRTX::tick() {
  // Absolutely no error checking in here

  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)

  if (state == INIT) {
    // ignore name
    while (*p != ':') {
      p++;
    }

    p++; // skip ':'

    while (*p != ':') {
      // get default duration
      if (*p == 'd') {
        p += 2; // skip "d="
        num = 0;
        while (isdigit(*p)) {
          num = (num * 10) + (*p++ - '0');
        }
        if (issep(*p) && num > 0) {
          default_dur = num;
        }
      } else if (*p == 'o') { // get default octave
        p += 2; // skip "o="
        num = *p++ - '0';
        if (issep(*p) && num >= 3 && num <= 7) {
          default_oct = num;
        }
      } else if (*p == 's') { // get style
        p += 2; // skip "s="
        style = *p++;
      } else if (*p == 'b') { // get BPM
        p += 2; // skip "b="
        num = 0;
        while (isdigit(*p)) {
          num = (num * 10) + (*p++ - '0');
        }
        if (issep(*p)) {
          bpm = num;
        }
      } else if (*p == 'l') { // get loops
        p += 2; // skip "l="
        num = 0;
        while (isdigit(*p)) {
          num = (num * 10) + (*p++ - '0');
        }
        if (issep(*p) && num < 16) {
          loops = num;
        }
      }

      while (!issep(*p)) {
        p++;
      }

      if (*p == ',') {
        p++;
      }
    }

    p++; // skip ':'

    // BPM usually expresses the number of quarter notes per minute
    wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)

    // Serial.print("ddur: ");
    // Serial.println(default_dur, 10);
    // Serial.print("doct: ");
    // Serial.println(default_oct, 10);
    // Serial.print("style: ");
    // Serial.println(style);
    // Serial.print("bpm: ");
    // Serial.println(bpm, 10);
    // Serial.print("wn: ");
    // Serial.println(wholenote, 10);
    // Serial.print("loops: ");
    // Serial.println(loops, 10);

    start = p;
    state = PLAY;
  }

  if (state == PLAY) {
    // now begin note loop
    if (!*p) {
      if (loops > 0) {
        p = start;
        if (loops != 15) {
          loops--;
        }
      } else {
        noTone(tonePin);
        state = END;
        return;
      }
    }

    // first, get note duration, if available
    num = 0;
    while (isdigit(*p)) {
      num = (num * 10) + (*p++ - '0');
    }

    if (num) {
      duration = wholenote / num;
    } else {
      duration = wholenote / default_dur;  // we will need to check if we are a dotted note after
    }

    // now get the note
    note = 0;

    switch (*p) {
      case 'c':
        note = 1;
        break;
      case 'd':
        note = 3;
        break;
      case 'e':
        note = 5;
        break;
      case 'f':
        note = 6;
        break;
      case 'g':
        note = 8;
        break;
      case 'a':
        note = 10;
        break;
      case 'b':
        note = 12;
        break;
      case 'p':
      default:
        note = 0;
    }
    p++;

    // now, get optional '#' sharp
    if (*p == '#') {
      note++;
      p++;
    }

    // now, get optional '.' dotted note
    if (*p == '.') {
      duration += duration / 2;
      p++;
    }

    // now, get scale
    if (isdigit(*p)) {
      scale = *p - '0';
      p++;
    }
    else
    {
      scale = default_oct;
    }

    scale += OCTAVE_OFFSET;

    if (*p == ',') {
      p++; // skip comma for next note (or we may be at the end)
    }

    // now play the note

    if (note) {
      // Serial.print("Playing: ");
      // Serial.print(scale, 10); Serial.print(' ');
      // Serial.print(note, 10); Serial.print(" (");
      // Serial.print(notes[(scale - 4) * 12 + note], 10);
      // Serial.print(") ");
      // Serial.println(duration, 10);
      tone(tonePin, notes[(scale - 4) * 12 + note]);
      state = NOTE;
      deadline = millis() + (style == 'S' ? duration / 2 : style == 'C' ? duration : (duration - duration / 20 /*wholenote / 100*/));
    } else {
      // Serial.print("Pausing: ");
      // Serial.println(duration, 10);
      state = PAUSE;
      deadline = millis() + duration;
      noTone(tonePin);
    }
  } else if (state == NOTE && deadline < millis()) {
    if (style != 'C') {
      state = PAUSE;
      deadline = millis() + (style == 'S' ? duration / 2 : duration / 20 /*wholenote / 100*/);
      noTone(tonePin);
    } else {
      state = PLAY;
    }
  } else if (state == PAUSE && deadline < millis()) {
    state = PLAY;
  }
}
