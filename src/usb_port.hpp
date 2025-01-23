#pragma once

#include <windows.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

class COMPort {
public:
    COMPort(const std::string& port_name) 
        : hSerial(INVALID_HANDLE_VALUE) {
        // Открываем COM-порт
        hSerial = CreateFile(port_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
        if (hSerial == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Failed to open COM port");
        }

        // Настроим параметры COM порта
        DCB dcbSerialParams = { 0 };
        if (!GetCommState(hSerial, &dcbSerialParams)) {
            throw std::runtime_error("Failed to get COM port state");
        }

        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        dcbSerialParams.BaudRate = CBR_9600;  // Скорость передачи
        dcbSerialParams.ByteSize = 8;          // Количество бит в байте
        dcbSerialParams.StopBits = ONESTOPBIT; // Один стоповый бит
        dcbSerialParams.Parity = NOPARITY;     // Без четности

        if (!SetCommState(hSerial, &dcbSerialParams)) {
            throw std::runtime_error("Failed to set COM port parameters");
        }

        // Настройка таймаутов
        COMMTIMEOUTS timeouts = { 0 };
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 10;
        timeouts.WriteTotalTimeoutConstant = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;
        SetCommTimeouts(hSerial, &timeouts);
    }

    ~COMPort() {
        if (hSerial != INVALID_HANDLE_VALUE) {
            CloseHandle(hSerial);
        }
    }

    void writeData(const std::string& data) {
        DWORD bytes_written;
        if (!WriteFile(hSerial, data.c_str(), data.length(), &bytes_written, nullptr)) {
            throw std::runtime_error("Failed to write data to COM port");
        }
    }

    std::string readData() {
        char buffer[256];
        DWORD bytes_read;
        if (!ReadFile(hSerial, buffer, sizeof(buffer), &bytes_read, nullptr)) {
            throw std::runtime_error("Failed to read data from COM port");
        }
        return std::string(buffer, bytes_read);
    }

private:
    HANDLE hSerial;
};
