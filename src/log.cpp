#include "log.h"

#include <unordered_map>

LOG::LOG() {}

LOG::LOG(const typelog type) {
    msglevel = type;
    *this << ("[" + getLabel(type) + "] ");
}

LOG::~LOG() {
    if (opened) {
        std::cout << std::endl;
    }
    opened = false;
}

std::string LOG::getLabel(typelog type) {
    switch (type) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARN:
            return "WARN";
        case ERROR:
            return "ERROR";
        case FATAL:
            return "FATAL";
    }
    return "NOTHING";
}