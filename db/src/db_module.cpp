

#include "db_module.h"
#include "db_mysql.h"
#include "xcore_log.h"
#include "app_config.h"

namespace db {

string DBModule::m_dbhost;
uint16 DBModule::m_dbport = 3306;
string DBModule::m_dbscheme;
string DBModule::m_dbuser;
string DBModule::m_dbpasswd;
string DBModule::m_dbCharacterSet;
string DBModule::m_dbCharacterNames;

DBModule::DBModule()
{
	// empty
}

DBModule::~DBModule()
{
	// empty
}

bool DBModule::config()
{
	m_dbhost = AppConfig::instance()->get_db_host();
	m_dbport = AppConfig::instance()->get_db_port();
	m_dbscheme = AppConfig::instance()->get_db_scheme();
	m_dbuser = AppConfig::instance()->get_db_user();
	m_dbpasswd = AppConfig::instance()->get_db_password();
	m_dbCharacterSet = AppConfig::instance()->get_db_character_set();
	m_dbCharacterNames = AppConfig::instance()->get_db_character_names();
	if (m_dbhost.empty()) m_dbhost = "localhost";

	return true;
}

bool DBModule::initialize()
{
	if (!mysql::Database::Instance()->Open(m_dbhost, m_dbport, m_dbscheme, m_dbuser, m_dbpasswd))
	{
		XERROR("DB Server open failed.");
		return false;
	}

	if (!create_tables())
	{
		XERROR("DB tables crate failed.");
		return false;
	}
	return true;
}

bool DBModule::start()
{
	return true;
}

bool DBModule::stop()
{
	mysql::Database::Instance()->Close();
	return true;
}

string DBModule::get_name() const
{
	return "DBModule";
}

bool DBModule::is_enable()
{
	return true;
}

bool DBModule::create_tables()
{
	shared_ptr<Conn> ptrConn = mysql::Database::Instance()->Connect();
	if (ptrConn.get() == NULL) return false;

	shared_ptr<Result> ptrResult =
	ptrConn->Prepare("CREATE TABLE `user` ( \
					`UID` int(10) NOT NULL AUTO_INCREMENT, \
					`DeviceID` varchar(128) NOT NULL DEFAULT '', \
					`IsDisable` int(10) NOT NULL DEFAULT '0', \
					`LastTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP, \
					`GenerateTime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00', \
					PRIMARY KEY (`UID`) \
					) DEFAULT CHARSET=utf8;")->Exec();
	if (ptrResult.get() == NULL) return false;

	ptrResult = 
	ptrConn->Prepare("CREATE TABLE `record` ( \
					`ID` int(10) unsigned NOT NULL AUTO_INCREMENT, \
					`UID` int(10) unsigned NOT NULL DEFAULT '0', \
					`AccountType` int(10) unsigned NOT NULL DEFAULT '0', \
					`Account` varchar(128) NOT NULL DEFAULT '', \
					`DeviceID` varchar(128) NOT NULL DEFAULT '', \
					`Local` varchar(128) NOT NULL DEFAULT '', \
					`Language` varchar(128) NOT NULL DEFAULT '', \
					`Model` varchar(128) NOT NULL DEFAULT '', \
					`OS` varchar(128) NOT NULL DEFAULT '', \
					`WidthPixels` int(10) unsigned NOT NULL DEFAULT '0', \
					`HighPixels` int(10) unsigned NOT NULL DEFAULT '0', \
					`AppID` int(10) unsigned NOT NULL DEFAULT '0', \
					`MajorVersion` int(10) unsigned NOT NULL DEFAULT '0', \
					`MinorVersion` int(10) unsigned NOT NULL DEFAULT '0', \
					`VersionInfo` varchar(128) NOT NULL DEFAULT '', \
					`PlatformType` int(10) unsigned NOT NULL DEFAULT '0', \
					`GenerateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP, \
					PRIMARY KEY (`ID`) \
					) DEFAULT CHARSET=utf8;")->Exec();
	if (ptrResult.get() == NULL) return false;

	ptrResult = 
	ptrConn->Prepare("CREATE TABLE `reg_deviceid` ( \
					`DeviceID` varchar(128) NOT NULL DEFAULT '', \
					`UID` int(10) unsigned NOT NULL DEFAULT '0', \
					`GenerateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP, \
					PRIMARY KEY (`DeviceID`), \
					UNIQUE KEY `UID` (`UID`) \
					) DEFAULT CHARSET=utf8;")->Exec();
	if (ptrResult.get() == NULL) return false;

	ptrResult = 
	ptrConn->Prepare("CREATE TABLE `reg_mail` ( \
					`Mail` varchar(64) NOT NULL DEFAULT '', \
					`Passwd` char(32) NOT NULL DEFAULT '', \
					`UID` int(10) unsigned NOT NULL DEFAULT '0', \
					`GenerateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP, \
					PRIMARY KEY (`Mail`), \
					UNIQUE KEY `UID` (`UID`) \
					) DEFAULT CHARSET=utf8;")->Exec();
	if (ptrResult.get() == NULL) return false;

	ptrResult = 
	ptrConn->Prepare("CREATE TABLE `reg_sina` ( \
					`SinaUID` varchar(64) NOT NULL DEFAULT '', \
					`UID` int(10) NOT NULL DEFAULT '0', \
					`GenerateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP, \
					PRIMARY KEY (`SinaUID`), \
					UNIQUE KEY `UID` (`UID`) \
					) DEFAULT CHARSET=utf8;")->Exec();
	if (ptrResult.get() == NULL) return false;

	ptrResult = 
	ptrConn->Prepare("CREATE TABLE `reg_tencent` ( \
					 `TencentUID` varchar(64) NOT NULL DEFAULT '', \
					 `UID` int(10) NOT NULL DEFAULT '0', \
					 `GenerateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP, \
					 PRIMARY KEY (`TencentUID`), \
					 UNIQUE KEY `UID` (`UID`) \
					 ) DEFAULT CHARSET=utf8;")->Exec();
	if (ptrResult.get() == NULL) return false;

	return true;
}

}//namespace db
