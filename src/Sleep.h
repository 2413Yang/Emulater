#ifndef SLEEP_H
#define SLEEP_H
#include <stdint.h>

int64_t getCurrentMicroseconds();

void microSleep(int64_t usInterval);

#endif