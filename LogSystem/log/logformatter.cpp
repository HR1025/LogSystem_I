#include "log.h"
#include <iostream>
#include <ctime>
using namespace std;



namespace logstm {


class StrFormatItem : public LogForMatter::FormatItem{
public:
    StrFormatItem() = default;
    StrFormatItem(string str):m_str(str){}
    std::string format(LogEvent::ptr) override{
        return m_str;
    }
private:
    string m_str;
};

class MsgFormatItem : public LogForMatter::FormatItem{
public:
    MsgFormatItem() = default;
    MsgFormatItem(string){}
    std::string format(LogEvent::ptr event) override{
        return event->getSStream().str();
    }
};

class LevelFormatItem : public LogForMatter::FormatItem{
public:
    LevelFormatItem() = default;
    LevelFormatItem(string){}
    std::string format(LogEvent::ptr event) override{
        return LogLevel::toString(event->getLevel());
    }
};

class EplaseFormatItem : public LogForMatter::FormatItem{
public:
    EplaseFormatItem() = default;
    EplaseFormatItem(string){}
    std::string format(LogEvent::ptr event) override{
        return to_string(event->getEplase());
    }
};

class NameFormatItem : public LogForMatter::FormatItem{
public:
    NameFormatItem() = default;
    NameFormatItem(string){}
    std::string format(LogEvent::ptr event) override{
        return event->getThreadName();
    }
};

class ThredIDFormatItem : public LogForMatter::FormatItem{
public:
    ThredIDFormatItem() = default;
    ThredIDFormatItem(string){}
    std::string format(LogEvent::ptr event) override{
        return to_string(event->getThreadId());
    }
};

class NewLineFormatItem : public LogForMatter::FormatItem{
public:
    NewLineFormatItem() = default;
    NewLineFormatItem(string){}
    std::string format(LogEvent::ptr) override{
        return "\n";
    }
};

class TimeFormatItem : public LogForMatter::FormatItem{
public:
    TimeFormatItem() = default;
    TimeFormatItem(string pattern = "%Y-%m-%d %H:%M:%S")
        :m_pattern(pattern){}
    std::string format(LogEvent::ptr event) override{
        time_t time = (time_t)event->getTime();
        tm * tblock = gmtime(&time);
        stringstream res;
        res<<tblock->tm_year<<"-"<<tblock->tm_mon<<"-"<<tblock->tm_mday
          <<" "<<tblock->tm_hour<<":"<<tblock->tm_min<<":"<<tblock->tm_sec;
        return res.str();
    }
private:
    string m_pattern;
};

class FileNameFormatItem : public LogForMatter::FormatItem{
public:
    FileNameFormatItem() = default;
    FileNameFormatItem(string){}
    std::string format(LogEvent::ptr event) override{
        return event->getFileName();
    }

};

class LineFormatItem : public LogForMatter::FormatItem{
public:
    LineFormatItem() = default;
    LineFormatItem(string){}
    std::string format(LogEvent::ptr event) override{
        return to_string(event->getLine());
    }
};

class TabFormatItem : public LogForMatter::FormatItem{
public:
    TabFormatItem() = default;
    TabFormatItem(string){}
    std::string format(LogEvent::ptr) override{
        return "\t";
    }
};

class FiberldFormatItem : public LogForMatter::FormatItem{
public:
    FiberldFormatItem() = default;
    FiberldFormatItem(string){}
    std::string format(LogEvent::ptr) override{
        return "\t";
    }
};

class ThreadNameFormatItem : public LogForMatter::FormatItem{
public:
    ThreadNameFormatItem() = default;
    ThreadNameFormatItem(string){}
    std::string format(LogEvent::ptr event) override{
        return event->getThreadName();
    }
};

std::string LogForMatter::getContent(LogEvent::ptr event){
    string res;
    for(auto& formatItem : m_formatItems) res += formatItem->format(event);
    return res;
}


bool LogForMatter::parsePattern(){
       bool flag = true;
       // 清空解析格式向量
       m_formatItems.clear();
       for(size_t left = 0, right = 0; right<m_pattern.size(); ){
           // 正常文本
           if(m_pattern[right] != '%' || right+1==m_pattern.size()){
               right++;
               if(right == m_pattern.size())
                   m_formatItems.push_back(make_shared<StrFormatItem>(m_pattern.substr(left, right-left)));
           }else{
               if(right != 0)
                    m_formatItems.push_back(make_shared<StrFormatItem>(m_pattern.substr(left, right-left)));
               char formatItemtype = m_pattern[++right];
               if(!isalpha(formatItemtype)){ flag = false; break; }
               // 转移字符
               if(formatItemtype == '%'){
                   m_formatItems.push_back(make_shared<StrFormatItem>(""));
                   left = right;
                   right++;
                   continue;
               }
               left = ++right;
               // 提取特殊格式的格式(如时间的具体格式)
               string itemPattern("");
               if(m_pattern[right] == '{'){
                   left = ++right;
                   while(right<m_pattern.size()  && m_pattern[right]!='}') right++;
                   itemPattern = m_pattern.substr(left, right-left);
                   right++;
               }
               left = right;
               switch (formatItemtype) {
                    #define XX(format, formatItem)\
                        case format :\
                         m_formatItems.push_back(make_shared<formatItem>(itemPattern)); break;
                        XX('m', MsgFormatItem);
                        XX('p', LevelFormatItem);
                        XX('r', EplaseFormatItem);
                        XX('c', NameFormatItem);
                        XX('t', ThredIDFormatItem);
                        XX('n', NewLineFormatItem);
                        XX('d', TimeFormatItem);
                        XX('f', FileNameFormatItem);
                        XX('l', LineFormatItem);
                        XX('T', TabFormatItem);
                        XX('F', FiberldFormatItem);
                        XX('N', ThreadNameFormatItem);
                    #undef XX
                    default: flag = false;break;
               }

           }
           if(!flag) break;
       }

       return flag;
}

void LogForMatter::setPattern(std::string pattern){
    if(m_pattern != pattern){
        m_pattern = pattern;
        parsePattern();
    }
}

}
