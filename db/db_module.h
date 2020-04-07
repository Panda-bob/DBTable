

#pragma once

#ifndef _DB_MODULE_H_
#define _DB_MODULE_H_

#include "xcore_define.h"
//#include "interface_module.h"

namespace db {

class DBModule : public IModule
{
public:
	DBModule();
	virtual ~DBModule();
	virtual bool  config();
	virtual bool  initialize();
	virtual bool  start();
	virtual bool  stop();
	virtual string get_name() const;
	virtual bool   is_enable();

	static string	DBHost() { return m_dbhost; }
	static uint16	DBPort() { return m_dbport; }
	static string	DBScheme() { return m_dbscheme; }
	static string	DBUser() { return m_dbuser; }
	static string	DBPasswd() { return m_dbpasswd; }
	static string	DBCharacterSet() { return m_dbCharacterSet; }
	static string	DBCharacterNames() { return m_dbCharacterNames; }

protected:
	bool create_tables();

private:
	static string	m_dbhost;
	static uint16   m_dbport;
	static string	m_dbscheme;
	static string	m_dbuser;
	static string	m_dbpasswd;
	static string	m_dbCharacterSet;
	static string	m_dbCharacterNames;
};

}//namespace db

using namespace db;

#endif//_DB_MODULE_H_

