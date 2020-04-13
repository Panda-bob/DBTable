#ifndef _MESSAGE_MSG_CONFIG_H_
#define _MESSAGE_MSG_CONFIG_H_

#include "xcore_define.h"
#include "xcore_json.h"

namespace message {

////////////////////////////////////////////////////////////////////////////////
// 日志系统信息
// class LogInfo
////////////////////////////////////////////////////////////////////////////////
class LogInfo 
{
public:
	LogInfo();

	virtual ~LogInfo();

	//virtual int parse_bytes(const byte* src, uint32 len);

	//virtual int to_bytes(byte* dst, uint32 maxlen);

	//virtual uint32 calc_length();

	virtual bool parse_json(XJsonValue& json);
	virtual bool to_json(XJsonValue& json);

public:
	uint32  m_level;  // 日志级别(level 0:none, 1:crash, 2:error, 3:warning, 4:notice, 5:info, 6:debug, 7:trace)
	uint32  m_keep_days;  // 日志保留天数
	uint16  m_port;  // 监听端口(0表示不监听)
};


////////////////////////////////////////////////////////////////////////////////
// mysql数据库信息
// class MysqlInfo
////////////////////////////////////////////////////////////////////////////////
class MysqlInfo 
{
public:
	MysqlInfo();

	virtual ~MysqlInfo();

	//virtual int parse_bytes(const byte* src, uint32 len);

	// int to_bytes(byte* dst, uint32 maxlen);

	//virtual uint32 calc_length();

	virtual bool parse_json(XJsonValue& json);
	virtual bool to_json(XJsonValue& json);

public:
	string  m_host;  // 主机或IP
	uint16  m_port;  // 端口
	string  m_user;  // 用户名
	string  m_password;  // 密码
	string  m_scheme;  // 选择库
	string  m_name;  // 该库名字(库名字不要重复)
	string  m_group;  // 该库所属分组(用于作集群负载)
};

class ConfigServer
{
public:
	ConfigServer();

	virtual ~ConfigServer();

	//virtual int parse_bytes(const byte* src, uint32 len);

	//virtual int to_bytes(byte* dst, uint32 maxlen);

	//virtual uint32 calc_length();

	virtual bool parse_json(XJsonValue& json);
	virtual bool to_json(XJsonValue& json);

public:
	
	LogInfo  m_log_info;  // 日志系统配置
	MysqlInfo  m_mysql_info;  // mysql数据库配置
	
};

////////////////////////////////////////////////////////////////////////////////
// message define
////////////////////////////////////////////////////////////////////////////////


}//namespace message

using namespace message;

#endif//_MESSAGE_MSG_CONFIG_H_
