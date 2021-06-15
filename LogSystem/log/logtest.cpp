#include "logtest.h"
#include <vector>
#include <memory>
#include <iostream>
using namespace std;

namespace logstm {

LogLevelTest::LogLevelTest(){
    cout<<endl<<"\tLogLevel测试."<<endl;
}

void LogLevelTest::toStringTest(){
    vector<LogLevel::Level> levels;
    levels.push_back(LogLevel::UNKOWN);
    levels.push_back(LogLevel::DEBUG);
    levels.push_back(LogLevel::INFO);
    levels.push_back(LogLevel::WARN);
    levels.push_back(LogLevel::ERROR);
    levels.push_back(LogLevel::FATAl);
    cout<<"*****************************************************"<<endl;
    cout<<" void LogLevelTest::toStringTest():"<<endl;
    cout<<" brief:"<<"将日志级别转化为文本"<<endl;
    for(auto& level:levels)
        cout<<" Level:"<<"\t"<<LogLevel::toString(level)<<endl;
    cout<<"*****************************************************"<<endl<<endl;
    return;
}

void LogLevelTest::toLevelTest(){
    vector<string> strLevels{"UNKOWN", "DEBUG", "INFO", "WARN",
                            "ERROR", "FATAl"};
    vector<LogLevel::Level> levels;
    cout<<"*****************************************************"<<endl;
    cout<<" void LogLevelTest::toLevelTest():"<<endl;
    cout<<" brief:"<<"将文本转化为日志级别"<<endl;
    for(auto& strLevel:strLevels)
        levels.push_back(LogLevel::toLevel(strLevel));
    for(auto& level:levels)
        cout<<" Level:"<<"\t"<<LogLevel::toString(level)<<endl;
    cout<<"*****************************************************"<<endl<<endl;
    return;
}

LogAppenderTest::LogAppenderTest(){
    cout<<endl<<"\tLogAppender测试."<<endl;
    logger = make_shared<Logger>("测试日志器");
    event =  make_shared<LogEvent>(__FILE__, __LINE__, 0, 0, 0);
    event->getSStream()<<"Appender测试";
}

void LogAppenderTest::stdAppenderTest(){
    cout<<"*****************************************************"<<endl;
    cout<<" void LogAppenderTest::stdAppenderTest():"<<endl;
    cout<<" brief:"<<"标准Appender"<<endl;
    logger->setAppender(make_shared<StdLogAppender>());
    logger->appendEvent(event);
    logger->log();
    cout<<"*****************************************************"<<endl<<endl;
    return;
}

void LogAppenderTest::fileAppenderTest(){
    cout<<"*****************************************************"<<endl;
    cout<<" void LogAppenderTest::fileAppenderTest():"<<endl;
    cout<<" brief:"<<"文件Appender"<<endl;
    logger->setAppender(make_shared<FileLogAppender>("log.txt"));
    logger->appendEvent(event);
    logger->log();
    cout<<"*****************************************************"<<endl<<endl;
    return;
}

LogEventTest::LogEventTest(){
    cout<<endl<<"\tLogEventTest测试."<<endl;
    logger = make_shared<Logger>("测试事件");
    //logger->setAppender(make_shared<StdLogAppender>());
    event = make_shared<LogEvent>(__FILE__, __LINE__, 0, 0, 0);
}

void LogEventTest::getSStreamTest(){
    cout<<"*****************************************************"<<endl;
    cout<<" void LogEventTest::getSStreamTest():"<<endl;
    cout<<" brief:"<<"流式输入事件内容"<<endl;
    string num = "num:";
    for(int i=0; i<100; i++){
        string insert = num + to_string(i) + "\n";
        event->getSStream()<<insert;
    }
    logger->appendEvent(event);
    logger->appendEvent(event);
    logger->log();
    cout<<"*****************************************************"<<endl<<endl;
    return;
}

LogForMatterTest::LogForMatterTest(){
    cout<<endl<<"\tLogForMatterTest测试."<<endl;
    event =  make_shared<LogEvent>(__FILE__, __LINE__, 0, 0, 0);
    event->getSStream()<<"LogForMatter测试";
    logForMatter = make_shared<LogForMatter>();
}

void LogForMatterTest::setGetContentTest1(){
    cout<<"*****************************************************"<<endl;
    cout<<" void LogForMatterTest::setGetContentTest1():"<<endl;
    cout<<" brief:"<<"设置格式并根据event输出(1)"<<endl;
    logForMatter->setPattern("%m %p %r %c %t  %d %m");
    cout<<"\t"<<logForMatter->getContent(event)<<endl;
    cout<<"*****************************************************"<<endl<<endl;
    return;
}

void LogForMatterTest::setGetContentTest2(){
    cout<<"*****************************************************"<<endl;
    cout<<" void LogForMatterTest::setGetContentTest2():"<<endl;
    cout<<" brief:"<<"设置格式并根据event输出(2)"<<endl;
    logForMatter->setPattern("%m %p %r %c %t  %d %m %N");
    cout<<"\t"<<logForMatter->getContent(event)<<endl;
    cout<<"*****************************************************"<<endl<<endl;
    return;
}

void LogForMatterTest::setGetContentTest3(){
    cout<<"*****************************************************"<<endl;
    cout<<" void LogForMatterTest::setGetContentTest3():"<<endl;
    cout<<" brief:"<<"设置格式并根据event输出(3)"<<endl;
    logForMatter->setPattern("%m %p %r %f %c %t  %d %m");
    cout<<"\t"<<logForMatter->getContent(event)<<endl;
    cout<<"*****************************************************"<<endl<<endl;
    return;
}

}
