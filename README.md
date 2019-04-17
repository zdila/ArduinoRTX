# ArtuinoRTX

ArtuinoRTX is a small library playing RTTTL/RTX ringtones.

It is based on the code originally written by Brett Hagman updated to be non-blocking and support advanced features of RTX format.

## Usage

```c
int tonePin = 10;

char *song = "Twinkle:d=4,o=5,b=80:32p,8c,8c,8g,8g,8a,8a,g,8f,8f,8e,8e,8d,8d,c,8g,8g,8f,8f,8e,8e,d,8g,8g,8f,8f,8e,8e,d,8c,8c,8g,8g,8a,8a,g,8f,8f,8e,8e,8d,8d,c";

ArduinoRTX rtx(tonePin, song);

void setup(void) {
  // do your initialization
}

void loop(void) {
  rtx.tick();
  // do something else non-blocking
}
```
