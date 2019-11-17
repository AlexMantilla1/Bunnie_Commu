// ArduinoLike.h

#ifndef _ArduinoLike_H_
#define _ArduinoLike_H_

#define INPUT 1
#define OUTPUT 0
#define HIGH 1
#define LOW 0

void delayMicroseconds(int time);
void delayMilliseconds(int time);
void pinMode(int Pin, int Mode);
void digitalWrite(int Pin, int newState);
int digitalRead(int Pin);
uint32_t millis2();
unsigned long micros(void);
unsigned long millis(void);

#endif