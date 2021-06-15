#include "log.h"
#include <iostream>
#include <stdexcept>
using namespace std;

namespace logstm {
bool StdLogAppender::log(std::string& content){
    cout<<content<<endl;
    return cout.bad();
}

FileLogAppender::FileLogAppender(std::string file):m_file(file){
    m_ostream.open(m_file, ios_base::out |  ios_base::app);
    if(!m_ostream.is_open()){
        throw runtime_error("FileLogAppender初始化ofstream出现未知错误!");
    }
}

FileLogAppender::~FileLogAppender(){
    if(m_ostream.is_open()){
        m_ostream.close();
    }
}

bool FileLogAppender::log(std::string& content){
    bool res;
    m_ostream<<content;
    res = m_ostream.bad();
    m_ostream.flush(); // 刷新流
    return res;
}

}
