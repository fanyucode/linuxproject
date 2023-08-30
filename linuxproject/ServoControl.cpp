#include "ServoControl.h"
#include <wiringPi.h>
#include <mutex>

#define HORIZONTAL_SERVO_PIN 18
#define VERTICAL_SERVO_PIN 19
std::mutex servoMutex;

void initializeServos() {
    wiringPiSetupGpio();
    pinMode(HORIZONTAL_SERVO_PIN, PWM_OUTPUT);
    pinMode(VERTICAL_SERVO_PIN, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(192);
    pwmSetRange(2000);
}

void setHorizontalAngle(int angle) {
    std::lock_guard<std::mutex> lock(servoMutex);
    int duty = (angle * 10) + 500;
    pwmWrite(HORIZONTAL_SERVO_PIN, duty);
}

void setVerticalAngle(int angle) {
    std::lock_guard<std::mutex> lock(servoMutex);
    int duty = (angle * 10) + 500;
    pwmWrite(VERTICAL_SERVO_PIN, duty);
}

