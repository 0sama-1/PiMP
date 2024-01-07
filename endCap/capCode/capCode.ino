#include <Wire.h>
#include <MPU6050.h>
#include "MAFMPU.hpp"

constexpr int filterSize = 100;
constexpr int mass = 5;
unsigned long prev_time = 0;

MPU6050 mpu;

MAFMPU<filterSize>* xAccFilter = nullptr;
MAFMPU<filterSize>* yAccFilter = nullptr;
MAFMPU<filterSize>* zAccFilter = nullptr;
MAFMPU<filterSize>* velocityFilter = nullptr;

float xAcc = 0;
float yAcc = 0;
float zAcc = 0;

void setup() {
    Serial.begin(9600);
    Serial.println("Initialize MPU6050");

    while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
        Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
        delay(500);
    }

    xAccFilter = new MAFMPU<filterSize>(mpu);
    yAccFilter = new MAFMPU<filterSize>(mpu);
    zAccFilter = new MAFMPU<filterSize>(mpu);
    velocityFilter = new MAFMPU<filterSize>(mpu);
}

void loop() {

    const uint32_t curr_time = micros();
    const float dt = static_cast<float>(curr_time - prev_time) / 1e6;
    prev_time = curr_time;
    xAcc = mpu.readNormalizeAccel().XAxis;
    yAcc = mpu.readNormalizeAccel().YAxis;
    zAcc = mpu.readNormalizeAccel().ZAxis;

    xAccFilter->sample(xAcc);
    yAccFilter->sample(yAcc);
    zAccFilter->sample(zAcc);

    float X = xAccFilter->value();
    float Y = yAccFilter->value();
    float Z = zAccFilter->value();
    float zAcceleration = -sqrt(X*X + Y*Y + Z*Z);
    velocityFilter->sample(zAcceleration * dt * 100 + 4.1);

    // '>' is used to plot in the vscode extension 'Teleplot'
    Serial.println(dt);
    Serial.print(">Velocity: ");
    Serial.println(velocityFilter->value());
}