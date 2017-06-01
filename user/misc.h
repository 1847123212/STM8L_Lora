#ifndef __MISC_H
#define __MISC_H
#include "ebox_core.h"
extern char* ultoa( unsigned long value, char *string, int radix );
uint32_t ATOI32(char *str, int base);
char D2C(uint8_t val);
#endif
