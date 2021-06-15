#include "log.h"
#include <algorithm>
#include <stdexcept>
using namespace std;

namespace logstm {

Logger::Logger(string name,
   LogAppender::ptr appender,
   LogLevel::Level level,
   string pattern):
m_name(name), m_appender(appender),
m_level(level), m_pattern(pattern)
{
    /* 配置默认日志追加器 */
    if(appender == nullptr){
        m_appender = make_shared<StdLogAppender>();
    }
    /* 配置格式器 */
    m_formatter = make_shared<LogForMatter>();
    /* 为格式器配置格式 */
    m_formatter->setPattern(pattern);
}


void Logger::setPattern(string pattern){
    m_pattern = pattern;
    if(m_formatter){
        m_formatter->setPattern(m_pattern);
    }else{
        throw runtime_error("Logger::setPattern:内置m_formatter缺失");
    }

}

void Logger::appendEvent(LogEvent::ptr event){
    m_events.push_back(event);
}

bool Logger::delEvent(LogEvent::ptr event){
    bool res = false;
    auto itr = find(m_events.begin(), m_events.end(), event);
    if(itr != m_events.end()){
        m_events.erase(itr);
        res = true;
    }
    return res;
}

void Logger::log(){
    /* 遍历整个事件组，依次输出 */
    for(auto event : m_events){
        string content =  m_formatter->getContent(event);
        m_appender->log(content);
    }
    /* 清空事件组 */
    m_events.clear();
}

}
