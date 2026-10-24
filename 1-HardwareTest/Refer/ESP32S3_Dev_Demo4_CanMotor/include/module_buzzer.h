#ifndef MODULE_BUZZER_H
#define MODULE_BUZZER_H

#include <map_note.h>

void buzzer_init(void);
void buzzer_beep(int duration_ms);
void buzzer_info(void);
void buzzer_warn(void);
void buzzer_error(void);
void buzzer_ready(void);

#endif

