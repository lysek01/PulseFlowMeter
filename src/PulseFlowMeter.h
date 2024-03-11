#ifndef PulseFlowMeter_h
#define PulseFlowMeter_h

#if !defined(ARDUINO_ARCH_ESP32)
  #error “This library only supports ESP32 boards.”
#endif


#include "Arduino.h"
#include <esp_timer.h>

#define FLOWMETER_FILTER_SIZE 10

class PulseFlowMeter
{
private:
    int _pin;
    double _constant;
    void pulse();
    unsigned long long filter(unsigned long long array[], uint8_t size);

    unsigned long long _buffer[FLOWMETER_FILTER_SIZE];
    int _bufferIndex = 0;

    volatile unsigned long long _pulseCount = 0;
    unsigned long long _lastPulseTime = 0;
    unsigned long long _minTresholdPeriod;
    unsigned long long _maxTresholdPeriod;
    unsigned long long _period = 0;
    bool _flowing = false;

    static PulseFlowMeter *instance;

public:
    PulseFlowMeter();
    void begin(int pin, double constant);
    static void attachInterruptHandler();

    bool setTresholds(float min = 5, float max = 150); // in liters per minute
    bool inRange();

    float getFlowRateLiterMinute();
    float getFlowRateCubicHour();
    float getTotalVolume();
    bool clearTotalVolume();
};

#endif
