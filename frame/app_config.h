#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#include "xcore_define.h"
#include "msg_config.h"

namespace frame
{

////////////////////////////////////////////////////////////////////////////////
// class AppConfig
////////////////////////////////////////////////////////////////////////////////
class AppConfig
{
	AppConfig();
	~AppConfig();

public:
	static bool load(const string& path);

	static const ConfigServer& config();

private:
	static ConfigServer m_config;
};

}//namespace frame

using namespace frame;

#endif//_APP_CONFIG_H_
