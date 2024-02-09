#pragma once

#include<fstream>

void throw_exception(const char* _msg)
{
    throw _msg;
}

//std::fstream log("log.txt",std::ios::out);
#define log std::clog


/*
#define DEBUG_LOG(__msg) do{\
##ifdef DEBUG\
        log << __msg;\
##endif\
}while(false)

#define FINISH_DEBUG_LOG log.close();
*/
