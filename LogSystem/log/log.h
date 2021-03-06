#ifndef LOG_H
#define LOG_H

#include <string>
#include <list>
#include <memory>
#include <stdint.h>
#include <sstream>
#include <vector>
#include <fstream>

namespace logstm {

/**
 * @brief 日志级别
 */
class LogLevel{
public:
    enum Level{
        UNKOWN = 0,  // 未知
        DEBUG = 1,   // 调试
        INFO = 2,    // 信息输出
        WARN = 3,    // 警告
        ERROR = 4,   // 错误
        FATAl = 5    // 硬件错误
    };
public:
    /**
     *  @brief        将日志级别转化为文本
     *  @param[in]    level 日志级别
     *  @return       std::string 日志级别文本
     */
    static std::string toString(Level level);
    /**
     *  @brief        将文本转化为日志级别
     *  @param[in]    level 日志级别文本
     *  @return       Level 日志级别
     */
    static LogLevel::Level toLevel(std::string level);
};


/**
 * @brief 日志追加器
 * @note  这是一个虚基类
 */
class LogAppender{
public:
    using ptr = std::shared_ptr<LogAppender>;
    /**
     * @brief     将日志内容输出到对应目标
     * @param[in] content 日志内容
     * @return    bool    是否输出成功
     */
    virtual bool log(std::string& content) = 0;
private:
};

/**
 * @brief 控制台日志追加器
 * @note  这是一个虚基类
 */
class StdLogAppender : public LogAppender{
public:
    /**
     * @brief     将日志内容输出到对应目标
     * @param[in] content 日志内容
     * @return    bool    是否输出成功
     */
    virtual bool log(std::string& content) override;
};

/**
 * @brief 文件日志追加器
 * @note  这是一个虚基类
 */
class FileLogAppender : public LogAppender{
public:
    /**
     * @brief     构造函数
     * @param[in] file    将要输出的文件
     * @note      在构造函数内就将要输出的文件对象初始化
     */
    FileLogAppender(std::string file);
    ~FileLogAppender();
    /**
     * @brief     将日志内容输出到对应目标
     * @param[in] content 日志内容
     * @return    bool    是否输出成功
     */
    virtual bool log(std::string& content) override;
private:
    std::string m_file;                  // 输出文件
    std::ofstream m_ostream;             // 输出文件流
};


/**
 * @brief 日志事件器
 */
class LogEvent{
public:
    using ptr = std::shared_ptr<LogEvent>;

    /**
     * @brief     构造函数
     * @param[in] file    文件名
     * @param[in] line    行号
     * @param[in] eplase  程序运行的时间
     * @param[in] fiberld 协程号
     * @param[in] time    时间戳
     * @param[in] level   事件等级
     * @note      在构造函数内默认初始化线程名及线程ID，
     *            可以调用setThreadName修改线程名
     */
    LogEvent(std::string file,
             uint32_t line,
             uint32_t eplase,
             uint32_t fiberld,
             uint32_t time,
             LogLevel::Level level = LogLevel::DEBUG);

    std::string getFileName() const { return m_file; }
    uint32_t getLine() const { return m_line; }
    void setEplase(uint32_t eplase) { m_eplase = eplase; }
    uint32_t getEplase() const { return m_eplase; }
    void setThreadId(uint32_t threadId) { m_threadId = threadId; }
    uint32_t getThreadId() const { return m_threadId; }
    void setFiberld(uint32_t fiberId) { m_fiberId = fiberId; }
    uint32_t getFiberld() const { return m_fiberId; }
    void setTime(uint32_t time) { m_time = time; }
    uint32_t getTime() const { return m_time; }
    void setThreadName(std::string threadName)
    { m_threadName = threadName; }
    std::string getThreadName() const { return m_threadName; }
    void setLevel(LogLevel::Level level) { m_level = level; }
    LogLevel::Level getLevel() const { return m_level; }

    std::stringstream& getSStream() { return m_ss; }

private:
    const std::string m_file; // 文件名
    uint32_t m_line;          // 行号
    uint32_t m_eplase;        // 程序运行时间（ms）
    uint32_t m_threadId;      // 线程ID
    uint32_t m_fiberId;       // 协程ID
    uint32_t m_time;          // 时间戳
    std::string m_threadName; // 线程名
    std::stringstream m_ss;   // 日志内容流
    LogLevel::Level m_level;  // 日志等级
};

// 日志格式器
class LogForMatter{
public:
    using ptr = std::shared_ptr<LogForMatter>;
    LogForMatter() = default;
    std::string getPattern() const { return m_pattern; }
    /**
     * @brief
     * @param
     * @details
     *  %m 消息
     *  %p 日志级别
     *  %r 累计毫秒数
     *  %c 日志名称
     *  %t 线程id
     *  %n 换行
     *  %d 时间
     *  %f 文件名
     *  %l 行号
     *  %T 制表符
     *  %F 协程id
     *  %N 线程名称
     *
     *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
     */
    void setPattern(std::string pattern);

    // 将日志信息转化为字符
    std::string getContent(LogEvent::ptr event);

public:
    class FormatItem {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        // 按照不同的格式需求对event内容进行格式化输出
        virtual std::string format(LogEvent::ptr event) = 0;
    };

private:
    std::string m_pattern;                       // 日志格式
    std::vector<FormatItem::ptr> m_formatItems;  // 解析格式向量
    // 解析日志格式
    bool parsePattern();
};

// 日志器
class Logger{
public:
    using ptr = std::shared_ptr<Logger>;

    /**
     * @brief     构造函数
     * @param[in] name      名称
     * @param[in] appender  追加器
     * @param[in] level     日志器等级
     * @param[in] pattern   日志格式
     * @note      若未初始化追加器、等级、日志格式，则采取默认
     *            追加到控制台输出
     */
    Logger(std::string name,
           LogAppender::ptr appender=nullptr,
           LogLevel::Level level = LogLevel::DEBUG,
           std::string pattern="%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");

    void setName(std::string name) { m_name = name; }
    std::string getName() const     { return m_name; }
    void setPattern(std::string pattern);
    std::string getPattern() const { return m_pattern; }
    void setLevel(LogLevel::Level level) { m_level = level; }
    LogLevel::Level getLevel() const { return m_level; }
    void setAppender(LogAppender::ptr appender) { m_appender = appender; }
    LogAppender::ptr getAppender() const { return m_appender; }

    /**
     * @brief     添加事件
     * @param[in] event     事件
     * @note      可以重复添加事件
     */
    void appendEvent(LogEvent::ptr event);
    /**
     * @brief     删除事件
     * @param[in] event     事件
     * @param[in] bool      是否删除成功
     * @note      删除最先添加的事件
     */
    bool delEvent(LogEvent::ptr event);
    /**
     * @brief     输出日志
     * @note      此操作后m_events将被清空
     */
    void log();

private:
    std::string                     m_name;       // 名称
    LogAppender::ptr                m_appender;   // 日志目标
    LogLevel::Level                 m_level;      // 屏蔽等级
    std::string                     m_pattern;    // 日志格式
    LogForMatter::ptr               m_formatter;  // 日志格式解析器
    std::list<LogEvent::ptr>        m_events;     // 日志事件器组
};



}

#endif // LOG_H
