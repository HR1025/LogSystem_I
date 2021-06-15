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
    /* ����Ĭ����־׷���� */
    if(appender == nullptr){
        m_appender = make_shared<StdLogAppender>();
    }
    /* ���ø�ʽ�� */
    m_formatter = make_shared<LogForMatter>();
    /* Ϊ��ʽ�����ø�ʽ */
    m_formatter->setPattern(pattern);
}


void Logger::setPattern(string pattern){
    m_pattern = pattern;
    if(m_formatter){
        m_formatter->setPattern(m_pattern);
    }else{
        throw runtime_error("Logger::setPattern:����m_formatterȱʧ");
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
    /* ���������¼��飬������� */
    for(auto event : m_events){
        string content =  m_formatter->getContent(event);
        m_appender->log(content);
    }
    /* ����¼��� */
    m_events.clear();
}

}
