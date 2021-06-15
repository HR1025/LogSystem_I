#include "log.h"
#include <thread>

using namespace std;

namespace logstm {
    LogEvent::LogEvent(string file,
                       uint32_t line,
                       uint32_t eplase,
                       uint32_t fiberId,
                       uint32_t time,
                       LogLevel::Level level):
    m_file(file), m_line(line), m_eplase(eplase),
    m_fiberId(fiberId), m_time(time), m_level(level)
    {
        /* Ϊ�߳�ID����ֵ */
        stringstream ss;
        ss << this_thread::get_id();
        m_threadId = stoul(ss.str());
        /* Ϊ�߳�������ֵ */
        m_threadName = "thread" + ss.str();
        /* �����־������ */
        m_ss.clear();
    }
}
