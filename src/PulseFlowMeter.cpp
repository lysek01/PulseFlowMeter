#include "PulseFlowMeter.h"

PulseFlowMeter *PulseFlowMeter::instance = nullptr;

PulseFlowMeter::PulseFlowMeter()
{
}

void PulseFlowMeter::begin(int pin, double constant)
{
    _pin = pin;
    _constant = constant;
    instance = this;
    pinMode(_pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(_pin), attachInterruptHandler, FALLING);
}

void PulseFlowMeter::attachInterruptHandler()
{
    instance->pulse();
}

void PulseFlowMeter::pulse()
{
    if (esp_timer_get_time() < _lastPulseTime)
    {
        _lastPulseTime = esp_timer_get_time();
    }

    unsigned long long period = esp_timer_get_time() - _lastPulseTime;
    _lastPulseTime = esp_timer_get_time();

    if (period >= _maxTresholdPeriod && period <= _minTresholdPeriod)
    {
        _flowing = true;
        if (inRange())
        {
            _pulseCount++;
            _buffer[_bufferIndex] = period;
            _bufferIndex = (_bufferIndex + 1) % FLOWMETER_FILTER_SIZE;

            if (_bufferIndex == FLOWMETER_FILTER_SIZE - 1)
            {
                _period = filter(_buffer, FLOWMETER_FILTER_SIZE);
            }
        }
    }
    else
    {
        _flowing = false;
    }
}

bool PulseFlowMeter::setTresholds(float min, float max)
{
    if (min > 0 && max > min)
    {
        _minTresholdPeriod = (60 * 1000000) / (_constant * min);
        _maxTresholdPeriod = (60 * 1000000) / (_constant * max);
        return true;
    }
    return false;
}

bool PulseFlowMeter::inRange()
{
    return (_lastPulseTime + _minTresholdPeriod > esp_timer_get_time() && _flowing);
}

float PulseFlowMeter::getFlowRateLiterMinute()
{
    if (inRange() && _period > 0)
    {
        return (60 * 1000000) / (_constant * _period);
    }
    return 0;
}

float PulseFlowMeter::getFlowRateCubicHour()
{
    return getFlowRateLiterMinute() * 0.06;
}

float PulseFlowMeter::getTotalVolume()
{
    if (_pulseCount != 0)
    {
        return _pulseCount / _constant;
    }
    return 0;
}

bool PulseFlowMeter::clearTotalVolume()
{
    return _pulseCount = 0;
}

unsigned long long PulseFlowMeter::filter(unsigned long long array[], uint8_t size)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            if (array[j] > array[j + 1])
            {
                unsigned long long temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
    if (size % 2 == 1)
    {
        return array[size / 2];
    }
    else
    {
        return (array[size / 2 - 1] + array[size / 2]) / 2;
    }
}