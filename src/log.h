/*
* File:   Log.h
 * Author: Alberto Lepe <dev@alepe.com>
 *
 * Created on December 1, 2015, 6:00 PM
 */

#pragma once

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
            auto& os = msglevel > WARN ? std::cerr : std::cout;
            os << msg;
            opened = true;
        }
        if (msglevel >= exitLevel)
        {
            std::cerr << "Exiting";
            exit(-1);
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
