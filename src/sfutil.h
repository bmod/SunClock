#pragma once

#include <filesystem>
#include <iostream>


std::string _res(const std::string& relPath) {
    auto inPath = "res/" + relPath;
    auto absPath = std::filesystem::absolute(inPath);
    if (!exists(absPath)) {
        LOG(FATAL) << "File not found: " << absPath;
        exit(-1);
    }
    return absPath;
}

#define FAIL(MSG) \
    std::cerr << MSG << std::endl; \
    std::exit(-1);

#define EXIT_ON_FAIL(SUCCESS, MSG) \
    if (!SUCCESS) { \
        FAIL(MSG) \
    }
