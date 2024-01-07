#include <Wire.h>
#include <MPU6050.h>
#include "MAFMPU.hpp"

constexpr int filterSize = 50;

MPU6050 mpu;

MAFMPU<filterSize>* pitchFilter = nullptr;
MAFMPU<filterSize>* rollFilter = nullptr;
MAFMPU<filterSize>* yawFilter = nullptr;

MAFMPU<filterSize>* xAccFilter = nullptr;
MAFMPU<filterSize>* yAccFilter = nullptr;
MAFMPU<filterSize>* zAccFilter = nullptr;

float xAng = 0;
float yAng = 0;
float zAng = 0;

float xAcc = 0;
float yAcc = 0;
float zAcc = 0;

bool foundMPU = false;
void calibrateGyro();

void setup() {
    Serial.begin(9600);
    Serial.println("Initialize MPU6050");

    while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
        Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
        delay(500);
    }
    foundMPU = true;

    pitchFilter = new MAFMPU<filterSize>(mpu);
    rollFilter = new MAFMPU<filterSize>(mpu);
    yawFilter = new MAFMPU<filterSize>(mpu);

    xAccFilter = new MAFMPU<filterSize>(mpu);
    yAccFilter = new MAFMPU<filterSize>(mpu);
    zAccFilter = new MAFMPU<filterSize>(mpu);
    calibrateGyro();
}

void loop() {
    if (foundMPU) {
        xAng += mpu.readNormalizeGyro().XAxis - pitchFilter->getBias();
        yAng += mpu.readNormalizeGyro().YAxis - rollFilter->getBias();
        zAng += mpu.readNormalizeGyro().ZAxis - yawFilter->getBias();

        xAcc = mpu.readNormalizeAccel().XAxis;
        yAcc = mpu.readNormalizeAccel().YAxis;
        zAcc = mpu.readNormalizeAccel().ZAxis;


        pitchFilter->sample(xAng);
        rollFilter->sample(yAng);
        yawFilter->sample(zAng);

        xAccFilter->sample(xAcc);
        yAccFilter->sample(yAcc);
        zAccFilter->sample(zAcc);
/*
        int pitch = map(pitchFilter->value() * 0.001, 0, 100, 0, 360);
        int roll = map(rollFilter->value() * 0.001, 0, 100, 0, 360);
        int yaw = map(yawFilter->value() * 0.001, 0, 100, 0, 360);
*/

        float pitch = pitchFilter->value();
        float roll = rollFilter->value();
        float yaw = yawFilter->value();

        float X = xAccFilter->value();
        float Y = -yAccFilter->value();
        float Z = zAccFilter->value();
        float R_2 = X*X + Y*Y + Z*Z;
/*
        Serial.print("Pitch:");
        Serial.print(xAng);
        Serial.print(",");
        Serial.print("Roll:");
        Serial.print(yAng);
        Serial.print(",");
        Serial.print("Yaw:");
        Serial.print(zAng);
        Serial.print(",");
        Serial.print("Zaverage: ");
        Serial.println(Z*cos(roll));
*/

        Serial.print(",NetZAcceleration: ");
        Serial.println(-sqrt(R_2));
        
        delay(1); // Adjust as needed for your application
    }
}

void calibrateGyro() {
    //Serial.println("Calibrating Gyro:");
    const int numSamples = 2000; // Number of samples for calibration
    float gyroXSum = 0.0;
    float gyroYSum = 0.0;
    float gyroZSum = 0.0;

    // Collect samples to calculate the average gyro readings (bias)
    for (int i = 0; i < numSamples; ++i) {
        gyroXSum += mpu.readNormalizeGyro().XAxis;
        gyroYSum += mpu.readNormalizeGyro().YAxis;
        gyroZSum += mpu.readNormalizeGyro().ZAxis;
        delay(1); // Adjust delay as needed between samples
    }

    // Calculate average gyro readings
    float gyroXBias = gyroXSum / numSamples;
    float gyroYBias = gyroYSum / numSamples;
    float gyroZBias = gyroZSum / numSamples;

    // Apply bias compensation to subsequent gyro readings
    pitchFilter->setBias(gyroXBias);
    rollFilter->setBias(gyroYBias);
    yawFilter->setBias(gyroZBias);
}
