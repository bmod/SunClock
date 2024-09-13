/*
* File:   Log.h
 * Author: Alberto Lepe <dev@alepe.com>
 *
 * Created on December 1, 2015, 6:00 PM
 */

#pragma once

#include <chrono>
#include <iostream>



enum typelog : int
{
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
    FATAL
};


class LOG
{
public:
    LOG();

    LOG(typelog type);

    ~LOG();

    template <class T>
    LOG& operator<<(const T& msg)
    {

        if (msglevel >= level)
        {
            std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            auto& os = msglevel > WARN ? std::cerr : std::cout;
            os << std::ctime(&end_time) << msg;
            opened = true;
        }
        return *this;
    }


private:
    typelog exitLevel = FATAL;
    typelog level = DEBUG;
    bool opened = false;
    typelog msglevel = DEBUG;

    std::string getLabel(typelog type);
};
