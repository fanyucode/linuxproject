#include "EnvironmentSensors.h"
#include <fstream>
#include <wiringPi.h>
#include <mcp3004.h>  // 如果使用wiringPi来读取MCP3008

#define BASE 100
#define SPI_CHAN 0
#define SOUND_SENSOR_PIN 0  // 声音传感器连接到MCP3008的第一个通道

int readTemperature() {
    std::ifstream file("/sys/bus/w1/devices/28-。。。。/w1_slave");  // DS18B20的id
    std::string line;
    while (getline(file, line)) {
        if (line.find("t=") != std::string::npos) {
            int tempPos = line.find("t=");
            return std::stoi(line.substr(tempPos + 2)) / 1000;
        }
    }
    return -1;  // 返回-1表示读取失败
}

int readSoundLevel() {
    return analogRead(BASE + SOUND_SENSOR_PIN);
}

void soundThread() {
    while (true) {
        int soundLevel = readSoundLevel();
        if (soundLevel > SOUND_THRESHOLD) {
            std::cout << "Sound level exceeded the threshold!" << std::endl;
            // 发送通知或其他操作
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));  // 每秒检查一次
    }
}

void temperatureThread() {
    while (true) {
        int temperature = readTemperature();
        if (temperature > TEMPERATURE_THRESHOLD) {
            std::cout << "Temperature exceeded the threshold!" << std::endl;
            // 发送通知或其他操作
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));  // 每5秒检查一次
    }
}