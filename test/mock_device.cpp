#include <iostream>
#include <random>
#include <string>
#include <stdexcept>
#include <vector>

#include "com_port.hpp"

class MockDevice {
public:
    MockDevice(const std::string& port_name) : comPort_(port_name) {
        std::cout << "COM port initialized and ready to send data." << std::endl;
    }

    void run() {
        while (true) {
            float temperature = generateRandomTemperature();
            sendTemperatureToHost(temperature);

            Sleep(1000);
        }
    }

private:
    COMPort comPort_;

    float generateRandomTemperature() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dis(-20.0, 50.0);
        return static_cast<float>(dis(gen));
    }

    void sendTemperatureToHost(float temperature) {
        std::string temperature_str = std::to_string(temperature) + "\n";
        try {
            comPort_.writeData(temperature_str);
            std::cout << "Sent temperature: " << temperature_str;
        } catch (const std::exception& e) {
            std::cerr << "Failed to send data via COM port, error: " << e.what() << std::endl;
        }
    }
};

int main() {
    try {
        MockDevice mock_device("COM4");
        mock_device.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
