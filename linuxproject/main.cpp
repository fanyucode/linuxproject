#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include "SocketServer.h"
#include "ServoControl.h"
#include "EnvironmentSensors.h"

// 假设以下函数已经在其他文件中定义
void videoThread(cv::VideoCapture& cap);
void soundThread();
void temperatureThread();
void servoControlThread();

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error opening camera!" << std::endl;
        return -1;
    }

    initializeServos();

    // 创建并启动线程
    std::thread camThread(videoThread, std::ref(cap));
    std::thread sndThread(soundThread);
    std::thread tempThread(temperatureThread);
    std::thread servoThread(servoControlThread);

    // 主线程可以继续执行其他任务，或者只是等待其他线程完成
    camThread.join();
    sndThread.join();
    tempThread.join();
    servoThread.join();

    return 0;
}

