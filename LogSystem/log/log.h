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
 * @brief ��־����
 */
class LogLevel{
public:
    enum Level{
        UNKOWN = 0,  // δ֪
        DEBUG = 1,   // ����
        INFO = 2,    // ��Ϣ���
        WARN = 3,    // ����
        ERROR = 4,   // ����
        FATAl = 5    // Ӳ������
    };
public:
    /**
     *  @brief        ����־����ת��Ϊ�ı�
     *  @param[in]    level ��־����
     *  @return       std::string ��־�����ı�
     */
    static std::string toString(Level level);
    /**
     *  @brief        ���ı�ת��Ϊ��־����
     *  @param[in]    level ��־�����ı�
     *  @return       Level ��־����
     */
    static LogLevel::Level toLevel(std::string level);
};


/**
 * @brief ��־׷����
 * @note  ����һ�������
 */
class LogAppender{
public:
    using ptr = std::shared_ptr<LogAppender>;
    /**
     * @brief     ����־�����������ӦĿ��
     * @param[in] content ��־����
     * @return    bool    �Ƿ�����ɹ�
     */
    virtual bool log(std::string& content) = 0;
private:
};

/**
 * @brief ����̨��־׷����
 * @note  ����һ�������
 */
class StdLogAppender : public LogAppender{
public:
    /**
     * @brief     ����־�����������ӦĿ��
     * @param[in] content ��־����
     * @return    bool    �Ƿ�����ɹ�
     */
    virtual bool log(std::string& content) override;
};

/**
 * @brief �ļ���־׷����
 * @note  ����һ�������
 */
class FileLogAppender : public LogAppender{
public:
    /**
     * @brief     ���캯��
     * @param[in] file    ��Ҫ������ļ�
     * @note      �ڹ��캯���ھͽ�Ҫ������ļ������ʼ��
     */
    FileLogAppender(std::string file);
    ~FileLogAppender();
    /**
     * @brief     ����־�����������ӦĿ��
     * @param[in] content ��־����
     * @return    bool    �Ƿ�����ɹ�
     */
    virtual bool log(std::string& content) override;
private:
    std::string m_file;                  // ����ļ�
    std::ofstream m_ostream;             // ����ļ���
};


/**
 * @brief ��־�¼���
 */
class LogEvent{
public:
    using ptr = std::shared_ptr<LogEvent>;

    /**
     * @brief     ���캯��
     * @param[in] file    �ļ���
     * @param[in] line    �к�
     * @param[in] eplase  �������е�ʱ��
     * @param[in] fiberld Э�̺�
     * @param[in] time    ʱ���
     * @param[in] level   �¼��ȼ�
     * @note      �ڹ��캯����Ĭ�ϳ�ʼ���߳������߳�ID��
     *            ���Ե���setThreadName�޸��߳���
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
    const std::string m_file; // �ļ���
    uint32_t m_line;          // �к�
    uint32_t m_eplase;        // ��������ʱ�䣨ms��
    uint32_t m_threadId;      // �߳�ID
    uint32_t m_fiberId;       // Э��ID
    uint32_t m_time;          // ʱ���
    std::string m_threadName; // �߳���
    std::stringstream m_ss;   // ��־������
    LogLevel::Level m_level;  // ��־�ȼ�
};

// ��־��ʽ��
class LogForMatter{
public:
    using ptr = std::shared_ptr<LogForMatter>;
    LogForMatter() = default;
    std::string getPattern() const { return m_pattern; }
    /**
     * @brief
     * @param
     * @details
     *  %m ��Ϣ
     *  %p ��־����
     *  %r �ۼƺ�����
     *  %c ��־����
     *  %t �߳�id
     *  %n ����
     *  %d ʱ��
     *  %f �ļ���
     *  %l �к�
     *  %T �Ʊ���
     *  %F Э��id
     *  %N �߳�����
     *
     *  Ĭ�ϸ�ʽ "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
     */
    void setPattern(std::string pattern);

    // ����־��Ϣת��Ϊ�ַ�
    std::string getContent(LogEvent::ptr event);

public:
    class FormatItem {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        // ���ղ�ͬ�ĸ�ʽ�����event���ݽ��и�ʽ�����
        virtual std::string format(LogEvent::ptr event) = 0;
    };

private:
    std::string m_pattern;                       // ��־��ʽ
    std::vector<FormatItem::ptr> m_formatItems;  // ������ʽ����
    // ������־��ʽ
    bool parsePattern();
};

// ��־��
class Logger{
public:
    using ptr = std::shared_ptr<Logger>;

    /**
     * @brief     ���캯��
     * @param[in] name      ����
     * @param[in] appender  ׷����
     * @param[in] level     ��־���ȼ�
     * @param[in] pattern   ��־��ʽ
     * @note      ��δ��ʼ��׷�������ȼ�����־��ʽ�����ȡĬ��
     *            ׷�ӵ�����̨���
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
     * @brief     �����¼�
     * @param[in] event     �¼�
     * @note      �����ظ������¼�
     */
    void appendEvent(LogEvent::ptr event);
    /**
     * @brief     ɾ���¼�
     * @param[in] event     �¼�
     * @param[in] bool      �Ƿ�ɾ���ɹ�
     * @note      ɾ���������ӵ��¼�
     */
    bool delEvent(LogEvent::ptr event);
    /**
     * @brief     �����־
     * @note      �˲�����m_events�������
     */
    void log();

private:
    std::string                     m_name;       // ����
    LogAppender::ptr                m_appender;   // ��־Ŀ��
    LogLevel::Level                 m_level;      // ���εȼ�
    std::string                     m_pattern;    // ��־��ʽ
    LogForMatter::ptr               m_formatter;  // ��־��ʽ������
    std::list<LogEvent::ptr>        m_events;     // ��־�¼�����
};



}

#endif // LOG_H