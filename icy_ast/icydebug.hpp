#pragma once

#include<fstream>

void throw_exception(const char* _msg);

//std::fstream log("log.txt",std::ios::out);

/*
#define DEBUG_LOG(__msg) do{\
##ifdef DEBUG\
        log << __msg;\
##endif\
}while(false)c

#define FINISH_DEBUG_LOG log.close();
*/
