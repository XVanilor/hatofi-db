//
// Created by vanilor on 14/01/23.
//

#include <string>
#include "Log.h"

void log_info(const std::string& msg)
{
    printf("[INFO] %s\n", msg.c_str());
}

void log_error(const std::string& msg)
{
    printf("[ERROR] %s\n", msg.c_str());
}