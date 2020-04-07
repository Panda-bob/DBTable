// 2014-11-24
// app_config.cpp
// 系统配置文件

#include "app_config.h"
#include "xcore_file_util.h"
#include "xcore_log.h"
#include "xcore_utf8.h"

namespace frame
{

////////////////////////////////////////////////////////////////////////////////
// class AppConfig
////////////////////////////////////////////////////////////////////////////////
Config_GameServer AppConfig::m_config;

AppConfig::AppConfig()
{
	// empty
}

AppConfig::~AppConfig()
{
	// empty
}

bool AppConfig::load(const string& path)
{
	// 加载本地配置信息
	string content;
	string format;
	if (!XUTF8::readfile_to_utf8(content, format, path))
	{
		XERROR("Read file(%s) failed.", path.c_str());
		return false;
	}
	XJsonValue root;
	if (!XJsonReader().parse(content, root, false))
	{
		XERROR("Read file(%s), invalid json file1.", path.c_str());
		return false;
	}
	if (!m_config.parse_json(root))
	{
		XERROR("Read file(%s), invalid json file2.", path.c_str());
		return false;
	}

	XLog::set_level(m_config.m_log_info.m_level);
	XLog::set_keep_days(m_config.m_log_info.m_keep_days);
	XLog::set_tcp_port(m_config.m_log_info.m_port);
	return true;
}

const Config_GameServer& AppConfig::config()
{
	return m_config;
}

}//namespace frame

