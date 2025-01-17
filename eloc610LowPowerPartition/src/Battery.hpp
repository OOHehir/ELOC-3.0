/*
 * Created on Sun Apr 23 2023
 *
 * Project: International Elephant Project (Wildlife Conservation International)
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 Fabian Lindner
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef BATTERY_HPP_
#define BATTERY_HPP_

#include <vector>
#include <map>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "CPPANALOG/analogio.h"
#include "ElocSystem.hpp"
#include "esp_err.h"

typedef struct {
    float Voff;
    float Vlow;
    float Vfull;
}bat_limits_t;

typedef struct { double volt; double soc; } socLUT_t;

class Battery
{
private:
    SemaphoreHandle_t mSemaphore;
    StaticSemaphore_t _mSemaphoreBuffer;
    typedef enum {
        BAT_NONE = 0,
        BAT_LiFePo,
        BAT_LiPo,
    }batType_t;
    bool mChargingEnabled;
    float mVoltageOffset;
    ElocSystem& mSys;
    CPPANALOG::CppAdc1 mAdc;
    float mVoltage;
    batType_t mBatteryType;
    int64_t mLastReadingMs;
    bool mCalibrationValid;
    std::map<float, float> mCalData;

    const bool mHasIoExpander;
    const uint32_t AVG_WINDOW;
    const uint32_t UPDATE_INTERVAL_MS;
    const uint32_t avgIntervalMs;
    static const char* CAL_FILE;
    
    void updateOutdatedVoltage();
    esp_err_t readRawVoltage(float& voltage);
    virtual esp_err_t setChargingEnable(bool enable); 
    const bat_limits_t& getLimits() const;
    const std::vector<socLUT_t>&  getSocLUT() const;
    Battery();
    esp_err_t init();
    esp_err_t loadCalFile();

public:
    virtual ~Battery();
    static Battery& GetInstance() {
        static Battery battery;
        return battery;
    }

    virtual float getVoltage();
    virtual float getSoC();
    const char* getState();
    const char* getBatType() const;

    bool isLow();
    bool isFull();
    bool isEmpty();

    void updateVoltage(bool forceUpdate = false);

    virtual esp_err_t setDefaultChargeEn(bool enable); 

    bool isCalibrationDone() const;
    esp_err_t clearCal();
    esp_err_t printCal(String& buf) const;
    esp_err_t updateCal(const char* buf);

    esp_err_t getRawVoltage(float& voltage);
};


#endif // BATTERY_HPP_