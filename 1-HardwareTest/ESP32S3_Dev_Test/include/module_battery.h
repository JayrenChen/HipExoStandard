#ifndef MODULE_BATTERY_H
#define MODULE_BATTERY_H

#include <Arduino.h>

void battery_init(void);
float_t battery_read(void);


#endif // MODULE_BATTERY_H