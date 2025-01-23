#include "com_port.hpp"
#include "logger.hpp"
#include <vector>
#include <numeric>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <thread>
#include <chrono>
#endif

int main() {
    try {
#ifdef _WIN32
        COMPort com_port("COM3");
#else
        COMPort com_port("/dev/ttyS2");
#endif
        Logger logger;
        int interval = 1;
        std::vector<float> measurementsAll;
        std::vector<float> measurementsHourly;

        while (true) {
            std::string data = com_port.readData();
            float temperature = std::stof(data);

            logger.logMeasurement(temperature);

            measurementsAll.push_back(temperature);

            if (measurementsAll.size() == 10 / interval) { // (3600 / interval) for real conditions
                float hourly_sum = std::accumulate(measurementsAll.begin(), measurementsAll.end(), 0.0f);
                float hourly_avg = hourly_sum / measurementsAll.size();

                measurementsHourly.push_back(hourly_avg);
                logger.logAverageHourly(hourly_avg);

                measurementsAll.clear();
            }

            if (measurementsHourly.size() == 6) { // 24 for real conditions
                float daily_sum = std::accumulate(measurementsHourly.begin(), measurementsHourly.end(), 0.0f);
                float daily_avg = daily_sum / measurementsHourly.size();

                logger.logAverageDaily(daily_avg);

                measurementsHourly.clear();
            }

#ifdef _WIN32
            Sleep(interval * 1000);
#else
            std::this_thread::sleep_for(std::chrono::seconds(interval));
#endif
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
