#ifndef __GPIO_H
#define __GPIO_H
#include "ebox_core.h"

void gpio_pd0_init();
void gpio_pd0_write(uint8_t GPIO_BitVal);
void gpio_pb0_init();
void gpio_pb0_write(uint8_t GPIO_BitVal);

#endif
