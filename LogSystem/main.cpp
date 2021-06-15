#include <iostream>
#include <ctime>
#include "log/log.h"
#include "log/logtest.h"
using namespace std;
using namespace logstm;
int main()
{
    LogLevelTest logLevelTest;
    logLevelTest.toLevelTest();
    logLevelTest.toStringTest();

    LogAppenderTest logAppenderTest;
    logAppenderTest.stdAppenderTest();
    logAppenderTest.fileAppenderTest();

    LogEventTest logEventTest;
    logEventTest.getSStreamTest();

    LogForMatterTest logForMatterTest;
    logForMatterTest.setGetContentTest1();
    logForMatterTest.setGetContentTest2();
    logForMatterTest.setGetContentTest3();

    return 0;
}
