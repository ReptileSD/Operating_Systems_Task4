#pragma once

#include <iostream>
#include <string>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#endif

class SerialPort {
public:
    explicit SerialPort(const std::string& port_name) : port_name_(port_name) {
#ifdef _WIN32
        h_serial_ = CreateFileA(port_name.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (h_serial_ == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Failed to connect to serial port: " + port_name);
        }
#else
        fd_ = open(port_name.c_str(), O_RDONLY | O_NOCTTY);
        if (fd_ < 0) {
            throw std::runtime_error("Failed to connect to serial port: " + port_name);
        }
#endif
    }

    ~SerialPort() {
#ifdef _WIN32
        if (h_serial_ != INVALID_HANDLE_VALUE) {
            CloseHandle(h_serial_);
        }
#else
        if (fd_ >= 0) {
            close(fd_);
        }
#endif
    }

    float readTemperature() {
        char buffer[32] = {0};

#ifdef _WIN32
        DWORD bytes_read;
        if (!ReadFile(h_serial_, buffer, sizeof(buffer) - 1, &bytes_read, nullptr)) {
            throw std::runtime_error("Failed to read from serial port");
        }
#else
        int bytes_read = read(fd_, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0) {
            throw std::runtime_error("Failed to read from serial port");
        }
#endif

        try {
            return std::stof(buffer);
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid data received from serial port");
        }
    }

private:
    std::string port_name_;
#ifdef _WIN32
    HANDLE h_serial_;
#else
    int fd_;
#endif
};
