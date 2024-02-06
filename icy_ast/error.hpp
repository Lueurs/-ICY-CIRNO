#pragma once

#include<fstream>

void throw_exception(const char* _msg)
{
    throw _msg;
}

std::fstream error_log("log.txt",std::ios::out);
