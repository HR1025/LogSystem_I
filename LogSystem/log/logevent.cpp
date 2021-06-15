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
        /* 为线程ID赋初值 */
        stringstream ss;
        ss << this_thread::get_id();
        m_threadId = stoul(ss.str());
        /* 为线程名赋初值 */
        m_threadName = "thread" + ss.str();
        /* 清空日志流内容 */
        m_ss.clear();
    }
}
