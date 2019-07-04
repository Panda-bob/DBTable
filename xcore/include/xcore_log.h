// 2008-06-05
// xcore_log.h
// guosh
// ��־�ࣨ���̰߳�ȫ��


#ifndef _XCORE_LOG_H_
#define _XCORE_LOG_H_

#include "xcore_define.h"
#include "xcore_clock.h"

namespace xcore
{
//��־��ɫ
#ifdef __WINDOWS__

#define TRED FOREGROUND_RED | FOREGROUND_INTENSITY
#define TGREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define TYELLOW FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
#define TNORMAL FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE
#define TWHITE TNORMAL | FOREGROUND_INTENSITY
#define TBLUE FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY

#else

#define TRED	1
#define TGREEN	2
#define TYELLOW	3
#define TNORMAL 4
#define TWHITE	5
#define TBLUE	6

#endif


// ��־�������
enum XLOG_LEVEL
{
    XLOG_NONE		=	0,	// ����ӡ��־
    XLOG_CRASH		=   1,  // ���ش���,���³����ܼ�������
    XLOG_ERR		=   2,  // �������,���´���ʧ��(�������ڴ�ʧ�ܵ�)
    XLOG_WARNING	=	3,	// ���򾯸�,���ܵ��´������(�紫�벻�Ϸ�����)
    XLOG_NOTICE		=   4,  // ��������ֵ��ע������
    XLOG_INFO	    =   5,  // ҵ����ص���Ϣ(��Ӱ��������̣�����ʾ�û���½������Ϣ)
    XLOG_DEBUG	    =   6,  // ������Ϣ(��Ӱ��������̣����ӡ��ǰ�ڴ����δʹ���ڴ����Ŀ��)
    XLOG_TRACE		=	7,	// ��ӡ�������й켣
    XLOG_LEVEL_LIMIT
};

// ��־���λ��
enum XLOG_OPTIONS
{
    XLOG_OPTION_FILE		=   0x01,   // log on to file, default
    XLOG_OPTION_STDOUT		=   0x02,   // log on the console if errors in sending
    XLOG_OPTION_STDERR		=   0x04,   // log on the stderr stream
    XLOG_OPTION_TCP			=   0x08,	// log on the tcp stream
};

class XLogParam
{
public:
	uint32			m_options;			// ָ�����λ��, see XLOG_OPTIONS
	XLOG_LEVEL      m_level;			// ָ���������, see XLOG_PRIORITY
	string			m_logdir;			// ��־�ļ���·��
	string			m_ident;			// ��־��������(����������־�����ͬһ�ļ�����ʱ)
	uint32			m_max_line;         // ÿ����־�ļ�����������������ʱ��������־�ļ�
	uint32			m_keep_days;		// ��־�ļ���������
	uint32			m_tcp_port;		    // ���tcp��ʱ�����˿�(0��ʾ����)

	XLogParam()
		: m_options(XLOG_OPTION_STDOUT)
		, m_level(XLOG_DEBUG)
		, m_logdir("log")
		, m_max_line(0)
		, m_keep_days(30)
		, m_tcp_port(0)
	{
		// empty
	}
	static XLogParam Default;
};


class XLog
{
public:
	// ��־ϵͳ��ʼ��
	static bool   initialize(const XLogParam& param = XLogParam::Default);
	static void   uninitialize();

	// ���úͻ�ȡ��־�������
	static void   set_level(uint32 lev/*see XLogLevel*/);
	static uint32 get_level();

	// ���úͻ�ȡ��־���ָ��
	static void   set_options(uint32 opt/*see XLogOption*/);
	static uint32 get_options();

	// ���úͻ�ȡ��־��������
	static void   set_keep_days(uint32 days);
	static uint32 get_keep_days();

	// ���úͻ�ȡTCP��־�������˿�(0��ʾ����)
	static void   set_tcp_port(uint16 port);
	static uint16 get_tcp_port();

	// ���
	static void   printf(XLOG_LEVEL lev/*see XLogLevel*/, const char cszFormat[], ...);

	// ������ɫ
	static void	set_color(unsigned int color)
	{
		#ifndef __WINDOWS__
			static const char* color_strings[TBLUE+1] = {
			"",
			"\033[22;31m",
			"\033[22;32m",
			"\033[01;33m",
			"\033[0m",
			"\033[01;37m",
			"\033[1;34m",
			};
			fputs(color_strings[color],stdout);
		#else
			//SetConsoleTextAttribute(stdout_handle,(WORD)color);

			HANDLE stdout_handle, stderr_handle;
			stderr_handle = (HANDLE)GetStdHandle(STD_ERROR_HANDLE);
			stdout_handle = (HANDLE)GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(stdout_handle, (WORD)color);
		#endif
	}
};

#define XCRASH(format, ...)	    XLog::printf(XLOG_CRASH, (const char*)format, ##__VA_ARGS__)
#define XERROR(format, ...)    XLog::printf(XLOG_ERR, (const char*)format, ##__VA_ARGS__)
#define XWARNING(format, ...)  XLog::printf(XLOG_WARNING, (const char*)format, ##__VA_ARGS__)
#define XNOTICE(format, ...)   XLog::printf(XLOG_NOTICE, (const char*)format, ##__VA_ARGS__)
#define XINFO(format, ...)     XLog::printf(XLOG_INFO, (const char*)format, ##__VA_ARGS__)
#define XDEBUG(format, ...)    XLog::printf(XLOG_DEBUG, (const char*)format, ##__VA_ARGS__)

#ifdef __DEBUG__
class XLogTrace
{
public:
	XLogTrace(const char* msg, int line)
		: m_msg(msg)
		, m_line(line)
	{
		XLog::printf(XLOG_TRACE, "=> Entry %s():%d", m_msg, m_line);
	}

	~XLogTrace()
	{
		XLog::printf(XLOG_TRACE, "<= Leave %s():%d, Elapsed %llu usecs.", m_msg, m_line, m_clock.peek().to_usecs());
	}
private:
	XClock      m_clock;
	const char* m_msg;
	int         m_line;
};
#define XTRACE_FUNCTION() XLogTrace tmplogtrace_(__FUNCTION__, __LINE__)
#define XTRACE(format, ...) XLog::printf(XLOG_TRACE, (const char*)format, ##__VA_ARGS__)
#else //__DEBUG__
#define XTRACE_FUNCTION()
#define XTRACE(format, ...)
#endif //__DEBUG__


#define XLOG_PROCESS_ERROR(Condition) \
	do  \
	{   \
		if (!(Condition))       \
		{                       \
			XERROR("XLOG_PROCESS_ERROR(%s) at %s in %s:%d.", #Condition, __FUNCTION__, __FILE__, __LINE__); \
			goto Exit0;         \
		}                       \
	} while (false)

#define XLOG_PROCESS_SUCCESS(Condition) \
	do  \
	{   \
		if (Condition)          \
		{                       \
			XNOTICE("XLOG_PROCESS_SUCCESS(%s) at %s in %s:%d.", #Condition, __FUNCTION__, __FILE__, __LINE__); \
			goto Exit1;         \
		}                       \
	} while (false)

#define XLOG_CHECK_ERROR(Condition) \
	do  \
	{   \
		if (!(Condition))       \
		{                       \
			XNOTICE("XLOG_CHECK_ERROR(%s) at %s in %s:%d.", #Condition, __FUNCTION__, __FILE__, __LINE__); \
		}                       \
	} while (false)

#define XLOG_PROCESS_ERROR_RET(Condition, Code) \
	do  \
	{   \
		if (!(Condition))       \
		{                       \
			XERROR("XLOG_PROCESS_ERROR_RET_CODE(%s, %d) at %s in %s:%d.", #Condition, Code, __FUNCTION__, __FILE__, __LINE__); \
			nResult = Code;     \
			goto Exit0;         \
		}                       \
	} while (false)

#define XLOG_COM_PROCESS_ERROR(Condition) \
	do  \
	{   \
		if (FAILED(Condition))  \
		{                       \
			XERROR("XLOG_COM_PROCESS_ERROR(%s) at %s in %s:%d.", #Condition, __FUNCTION__, __FILE__, __LINE__); \
			goto Exit0;         \
		}                       \
	} while (false)


#define XLOG_COM_PROCESS_SUCCESS(Condition)   \
	do  \
	{   \
		if (SUCCEEDED(Condition))   \
		{                           \
			XNOTICE("XLOG_COM_PROCESS_SUCCESS(%s) at %s in %s:%d.", #Condition, __FUNCTION__, __FILE__, __LINE__); \
			goto Exit1;             \
		}                           \
	} while (false)


#define XLOG_COM_PROCESS_ERROR_RET(Condition, Code)     \
	do  \
	{   \
		if (FAILED(Condition))      \
		{                           \
			XERROR("XLOG_COM_PROC_ERROR_RET_CODE(%s, %p) at %s in %s:%d.", #Condition, Code, __FUNCTION__, __FILE__, __LINE__); \
			hrResult = Code;        \
			goto Exit0;             \
		}                           \
	} while (false)

#define XLOG_COM_CHECK_ERROR(Condition) \
	do  \
	{   \
		if (FAILED(Condition))       \
		{                       \
			XNOTICE("XLOG_COM_CHECK_ERROR(%s) at %s in %s:%d.", #Condition, __FUNCTION__, __FILE__, __LINE__); \
		}                       \
	} while (false)

} // namespace xcore

using namespace xcore;

#endif//_XCORE_LOG_H_
