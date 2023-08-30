#include "VideoHandler.h"
#include "EnvironmentSensors.h"
#include "ServoControl.h"
#include "SocketServer.h"
#include <thread>
#include <Python.h>

int main() {
    // 初始化所有模块
    VideoHandler videoHandler;
    EnvironmentSensors sensors;
    ServoControl servoControl;
    SocketServer socketServer;

    // 启动辅助线程1：视频捕获和推理
    std::thread videoCaptureThread(&VideoHandler::startCapture, &videoHandler);
    // 读取传感器数据
    std::thread sensorThread([]() {
        while (true) {
            sensors.readTemperature();
            sensors.readSoundLevel();
            std::this_thread::sleep_for(std::chrono::seconds(1));  // 每秒读取一次
        }
    });

    // 处理客户端命令
    std::thread socketThread(&SocketServer::startServer, &socketServer);
    
    // 主线程：显示视频流
    cv::Mat frame;
    while (true) {
        frame = videoHandler.getLatestFrame();  // 假设VideoHandler有这个方法
        if (!frame.empty()) {
            cv::imshow("Video Stream", frame);
            if (cv::waitKey(1) == 'q') {
                break;
            }
        }
    }

    // 停止所有线程和服务
    videoHandler.stopCapture();
    socketServer.stopServer();  

    videoCaptureThread.join();
    detectionThread.join();
    sensorThread.join();
    socketThread.join();

    return 0;
}


