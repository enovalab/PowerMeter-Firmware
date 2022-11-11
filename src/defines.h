#ifndef DEFINES_H
#define DEFINES_H

#define LOG_VALUES false
#if LOG_VALUES
#define CURRENT_FACTOR 500
#else
#define CURRENT_FACTOR 1
#endif


#define CAL_U 0.4908f
#define CAL_I 0.01596f
#define CAL_PHASE 12

#define PIN_U 33
#define PIN_I 32
#define PIN_RELAY 2

#endif