#include <Wire.h>
#include <MPU6050.h>
#include "MAFMPU.hpp"

constexpr int filterSize = 50;
constexpr int mass = 5;

MPU6050 mpu;

MAFMPU<filterSize>* xAccFilter = nullptr;
MAFMPU<filterSize>* yAccFilter = nullptr;
MAFMPU<filterSize>* zAccFilter = nullptr;

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
}

void loop() {

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

    // '>' is used to plot in the vscode extension 'Teleplot'
    Serial.print(">Force: ");
    Serial.println(mass * zAcceleration);
    
    delay(1); // Adjust as needed for your application
}