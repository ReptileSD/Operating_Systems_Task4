#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <ctime>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <direct.h> // Для _mkdir
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

class Logger {
public:
    Logger() {
        createLogDirectory("logs");
        createLogFile("logs/all_measurements.log");
        createLogFile("logs/hourly_average.log");
        createLogFile("logs/daily_average.log");
    }

    void logMeasurement(float temperature) {
        std::ostringstream message;
        message << "Temperature: " << temperature;
        writeLog("logs/all_measurements.log", message.str());
        trimLogFile("logs/all_measurements.log", 120); // 3600 * 24 / interval for real conditions
    }

    void logAverageHourly(float hourly_avg) {
        std::ostringstream message;
        message << "Hourly Average: " << hourly_avg;
        writeLog("logs/hourly_average.log", message.str());
        trimLogFile("logs/hourly_average.log", 12); //24 * 30 (last 30 days) for real conditions
    }

    void logAverageDaily(float daily_avg) {
        std::ostringstream message;
        message << "Daily Average: " << daily_avg;
        writeLog("logs/daily_average.log", message.str());
        trimLogFile("logs/daily_average.log", 2); //365 for real conditions
    }

private:
    void createLogDirectory(const std::string& directory) {
#ifdef _WIN32
        if (_mkdir(directory.c_str()) != 0 && errno != EEXIST) {
            std::cerr << "Failed to create log directory: " << directory << std::endl;
        }
#else
        if (mkdir(directory.c_str(), 0777) != 0 && errno != EEXIST) {
            std::cerr << "Failed to create log directory: " << directory << std::endl;
        }
#endif
    }

    void createLogFile(const std::string& filename) {
        std::ofstream log_file(filename, std::ios_base::app);
        if (!log_file.is_open()) {
            std::cerr << "Failed to create log file: " << filename << std::endl;
        }
    }

    void writeLog(const std::string& filename, const std::string& message) {
        std::ofstream log_file(filename, std::ios_base::app);
        if (log_file.is_open()) {
            std::time_t current_time = std::time(nullptr);
            std::tm* time_info = std::localtime(&current_time);
            log_file << "[" 
                     << time_info->tm_hour << ":" 
                     << time_info->tm_min << ":" 
                     << time_info->tm_sec << "] " 
                     << message << std::endl;
        } else {
            std::cerr << "Failed to open log file: " << filename << std::endl;
        }
    }

    void trimLogFile(const std::string& filename, size_t max_entries) {
        std::ifstream log_file(filename);
        if (!log_file.is_open()) {
            std::cerr << "Failed to open log file for trimming: " << filename << std::endl;
            return;
        }

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(log_file, line)) {
            lines.push_back(line);
        }
        log_file.close();

        if (lines.size() > max_entries) {
            std::ofstream log_file_out(filename, std::ios_base::trunc);
            if (!log_file_out.is_open()) {
                std::cerr << "Failed to open log file for writing: " << filename << std::endl;
                return;
            }
            size_t start_index = lines.size() - max_entries;
            for (size_t i = start_index; i < lines.size(); ++i) {
                log_file_out << lines[i] << std::endl;
            }
        }
    }
};

#endif
