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

	/* ģ��������ļ��м��������� */
	virtual bool config() = 0;

	/* ģ����г�ʼ������ */
	virtual bool initialize() = 0;

	/* ����ģ�鹤�� */
	virtual bool start() = 0;

	/* ֹͣģ�鹤�� */
	virtual bool stop() = 0;

	/* ��ȡģ������ */
	virtual string get_name() const = 0;

	/* ģ���Ƿ���� */
	virtual bool is_enable() = 0;
};

}//namespace common

using namespace common;

#endif//_INTERFACE_MODULE_H_
