//
// Created by vanilor on 14/01/23.
//

#include <string>
#include <iostream>
#include "Log.h"

void log_info(const std::string& msg)
{
    printf("[INFO] %s\n", msg.c_str());
}

void log_warn(const std::string& msg)
{
    std::cerr << "[WARN] " << msg << "\n";
}

void log_error(const std::string& msg)
{
    std::cerr << "[ERROR] " << msg << "\n";
}