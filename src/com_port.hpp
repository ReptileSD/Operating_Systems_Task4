#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

class COMPort {
public:
    explicit COMPort(const std::string& port_name) {
#ifdef _WIN32
        hSerial = CreateFile(port_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
        if (hSerial == INVALID_HANDLE_VALUE) {
            DWORD dwError = GetLastError();
            std::cerr << "Failed to open COM port, error code: " << dwError << std::endl;
            throw std::runtime_error("Failed to open COM port");
        }

        DCB dcbSerialParams = { 0 };
        if (!GetCommState(hSerial, &dcbSerialParams)) {
            throw std::runtime_error("Failed to get COM port state");
        }

        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        dcbSerialParams.BaudRate = CBR_9600;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;
        if (!SetCommState(hSerial, &dcbSerialParams)) {
            throw std::runtime_error("Failed to set COM port parameters");
        }

        COMMTIMEOUTS timeouts = { 0 };
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 10;
        timeouts.WriteTotalTimeoutConstant = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;
        SetCommTimeouts(hSerial, &timeouts);
#else
        fd = open(port_name.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
        if (fd < 0) {
            throw std::runtime_error("Failed to open COM port");
        }

        struct termios tty;
        if (tcgetattr(fd, &tty) != 0) {
            throw std::runtime_error("Failed to get COM port attributes");
        }

        cfsetospeed(&tty, B9600);
        cfsetispeed(&tty, B9600);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;  // 8-bit chars
        tty.c_iflag &= ~IGNBRK;                     // disable break processing
        tty.c_lflag = 0;                            // no signaling chars, no echo, no canonical processing
        tty.c_oflag = 0;                            // no remapping, no delays
        tty.c_cc[VMIN]  = 1;                        // read doesn't block
        tty.c_cc[VTIME] = 1;                        // 0.1 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY);     // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);            // ignore modem controls, enable reading
        tty.c_cflag &= ~(PARENB | PARODD);          // shut off parity
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr(fd, TCSANOW, &tty) != 0) {
            throw std::runtime_error("Failed to set COM port attributes");
        }
#endif
    }

    ~COMPort() {
#ifdef _WIN32
        if (hSerial != INVALID_HANDLE_VALUE) {
            CloseHandle(hSerial);
        }
#else
        if (fd >= 0) {
            close(fd);
        }
#endif
    }

    void writeData(const std::string& data) {
#ifdef _WIN32
        DWORD bytes_written;
        if (!WriteFile(hSerial, data.c_str(), data.length(), &bytes_written, nullptr)) {
            throw std::runtime_error("Failed to write data to COM port");
        }
#else
        if (write(fd, data.c_str(), data.length()) < 0) {
            throw std::runtime_error("Failed to write data to COM port");
        }
#endif
    }

    std::string readData() {
#ifdef _WIN32
        char buffer[256];
        DWORD bytes_read;
        if (!ReadFile(hSerial, buffer, sizeof(buffer), &bytes_read, nullptr)) {
            throw std::runtime_error("Failed to read data from COM port");
        }
        return std::string(buffer, bytes_read);
#else
        char buffer[256];
        int bytes_read = read(fd, buffer, sizeof(buffer));
        if (bytes_read < 0) {
            throw std::runtime_error("Failed to read data from COM port");
        }
        return std::string(buffer, bytes_read);
#endif
    }

private:
#ifdef _WIN32
    HANDLE hSerial;
#else
    int fd;
#endif
};
