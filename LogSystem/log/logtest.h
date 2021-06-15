#ifndef LOGTEST_H
#define LOGTEST_H

#include "log.h"

namespace logstm {

class LogLevelTest{
public:
    LogLevelTest();
    void toStringTest();
    void toLevelTest();
};

class LogAppenderTest{
public:
    LogAppenderTest();
    void stdAppenderTest();
    void fileAppenderTest();
private:
    Logger::ptr logger;
    LogEvent::ptr event;
};

class LogEventTest{
public:
    LogEventTest();
    void getSStreamTest();
private:
    Logger::ptr logger;
    LogEvent::ptr event;
};

class LogForMatterTest{
public:
    LogForMatterTest();
    void setGetContentTest1();
    void setGetContentTest2();
    void setGetContentTest3();
private:
    LogEvent::ptr event;
    LogForMatter::ptr logForMatter;
};

}

#endif // LOGTEST_H
