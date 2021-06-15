#include "log.h"
#include <stdexcept>
#include <Qt3DCore/QAbstractAspect>
using  namespace std;

namespace logstm {

string LogLevel::toString(LogLevel::Level level){
    string res;
    switch(level){
        #define XX(level) \
        case level:\
        res = #level; break
        XX(UNKOWN);
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAl);
        #undef XX
    default:
            throw invalid_argument("LogLevel::Level 参数错误!");
            break;
    }
    return res;
}

LogLevel::Level LogLevel::toLevel(string level){
    LogLevel::Level res = LogLevel::UNKOWN;

    if(level == "DEBUG"){
        res = LogLevel::DEBUG;
    }else if(level == "INFO"){
        res = LogLevel::INFO;
    }else if(level == "WARN"){
        res = LogLevel::WARN;
    }else if(level == "ERROR"){
        res = LogLevel::ERROR;
    }else if(level == "FATAl"){
        res = LogLevel::FATAl;
    }else if(level == "UNKOWN"){
        res = LogLevel::UNKOWN;
    }else{
        throw invalid_argument("level 参数错误!");
    }

    return res;
}
}
