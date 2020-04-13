// 2009-07-15
// IModule.h
// user

#pragma once

#ifndef _INTERFACE_MODULE_H_
#define _INTERFACE_MODULE_H_

#include <string>
using namespace std;

namespace common {

////////////////////////////////////////////////////////////////////////////////
// ModuleInterface
////////////////////////////////////////////////////////////////////////////////
class IModule
{
public :
	virtual ~IModule()                                   { }

	/* 模块从配置文件中加载配置项 */
	virtual bool config() = 0;

	/* 模块进行初始化工作 */
	virtual bool initialize() = 0;

	/* 启动模块工作 */
	virtual bool start() = 0;

	/* 停止模块工作 */
	virtual bool stop() = 0;

	/* 获取模块名称 */
	virtual string get_name() const = 0;

	/* 模块是否可用 */
	virtual bool is_enable() = 0;
};

}//namespace common

using namespace common;

#endif//_INTERFACE_MODULE_H_
