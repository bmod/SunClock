#pragma once

#include <iostream>




#define FAIL(MSG) \
    std::cerr << MSG << std::endl; \
    std::exit(-1);

#define EXIT_ON_FAIL(SUCCESS, MSG) \
    if (!SUCCESS) { \
        FAIL(MSG) \
    }
