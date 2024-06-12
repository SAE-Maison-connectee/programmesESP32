#ifndef HEXTOINT_H
#define HEXTOINT_H
#include "Arduino.h"
int HEXTOINT(const char *s) 
{
  int x = 0;
  for(;;) {
    char c = *s;
    if (c >= '0' && c <= '9') {
      x *= 16;
      x += c - '0'; 
    }
    else if (c >= 'a' && c <= 'f') {
      x *= 16;
      x += (c - 'a') + 10; 
    }
    else break;
    s++;
  }
  return x;
}
String INTTOHEX(uint32_t i)
{
    return String(i,HEX);
}
#endif
