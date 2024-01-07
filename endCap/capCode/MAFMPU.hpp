#pragma once

#include "MovingAverageFilter.hpp"
#include <MPU6050.h>

template <size_t N>
class MAFMPU : public MPU6050 {
public:
    MAFMPU(MPU6050& mpu) : MPU6050(mpu) {}

    // The sensor is ready to be used if the filter is full.
    bool isReady() const { return filter.isFull(); }

    // Take a sample from the sensor and put it into the filter.
    void sample(float quantity) { filter.sample(quantity); }

    // Returns the average of the filter.
    float value() const { return filter.average(); }

    // Returns the capacity of the filter.
    size_t capacity() const { return filter.capacity(); }

    void setBias(float bias) { mBias = bias; }

    float getBias() { return mBias; }

private:
    MovingAverageFilter<float, N> filter;
    float mBias;
};
