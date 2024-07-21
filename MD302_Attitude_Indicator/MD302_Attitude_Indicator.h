#pragma once

#include "Arduino.h"

class MD302_Attitude_Indicator
{
public:
    MD302_Attitude_Indicator();
    void begin();
    void attach();
    void detach();
    void set(int16_t messageID, char *setPoint);
    void update();

private:
    bool    _initialised;
    uint8_t _pin1, _pin2, _pin3;
    float pitch = 180;
    float roll = 0;
    float newRoll = 0;
    float pitchPosition = 0;
    float newPitch = 0;
    float slip = -8; // slip angle value from sim

    // Function declarations
    float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
    void setPitchAngle(float pitchAngle);
    void setRollAngle(float rollAngle);
    void setSlipDegree(float slipDegree);
    void drawPitchScale();
    void drawBall();
    void drawAll();
};