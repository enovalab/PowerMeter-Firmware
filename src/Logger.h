#ifndef LOGGER_H
#define LOGGER_H

#define ALLOW_SERIAL_LOG true
#define ALLOW_FILE_LOG true

#define ALLOW_LOG_ERROR true
#define ALLOW_LOG_WARNING true
#define ALLOW_LOG_DEBUG true
#define ALLOW_LOG_INFO true

#if ALLOW_SERIAL_LOG
    #include <Arduino.h>
    #define SERIAL_LOG(...) {                   \
        if(Serial)                              \
        {                                       \
            Serial.printf(__VA_ARGS__);         \
        }                                       \
        else                                    \
        {                                       \
            Serial.begin(115200);               \
            Serial.printf(__VA_ARGS__);         \
        }                                       \
    }
#else
    #define SERIAL_LOG(...)
#endif


#if ALLOW_LOG_ERROR
    #define SERIAL_LOG_ERROR(...) SERIAL_LOG(__VA_ARGS__)
#else
    #define SERIAL_LOG_ERROR(...)
#endif


#if ALLOW_LOG_WARNING
    #define SERIAL_LOG_WARNING(...) SERIAL_LOG(__VA_ARGS__)
#else
    #define SERIAL_LOG_WARNING(...)
#endif


#if ALLOW_LOG_DEBUG
    #define SERIAL_LOG_DEBUG(...) SERIAL_LOG(__VA_ARGS__)
#else
    #define SERIAL_LOG_DEBUG(...)
#endif


#if ALLOW_LOG_INFO
    #define SERIAL_LOG_INFO(...) SERIAL_LOG(__VA_ARGS__)
#else
    #define SERIAL_LOG_INFO(...)
#endif



#if ALLOW_FILE_LOG
    #include <stdio.h>
    #include <SPIFFS.h>
    #include <fstream>
    #define FILE_LOG(...) {                                     \
        FILE* logFile;                                          \
        logFile = fopen("/spiffs/log/log.log", "a");            \
        fprintf(logFile, __VA_ARGS__);                          \
        fclose(logFile);                                        \
    }
#else
    #define FILE_LOG(...)
#endif


#if ALLOW_LOG_ERROR
    #define FILE_LOG_ERROR(...) FILE_LOG(__VA_ARGS__)
#else
    #define FILE_LOG_ERROR(...)
#endif


#if ALLOW_LOG_WARNING
    #define FILE_LOG_WARNING(...) FILE_LOG(__VA_ARGS__)
#else
    #define FILE_LOG_WARNING(...)
#endif


#if ALLOW_LOG_DEBUG
    #define FILE_LOG_DEBUG(...) FILE_LOG(__VA_ARGS__)
#else
    #define FILE_LOG_DEBUG(...)
#endif


#if ALLOW_LOG_INFO
    #define FILE_LOG_INFO(...) FILE_LOG(__VA_ARGS__)
#else
    #define FILE_LOG_INFO(...)
#endif

#endif