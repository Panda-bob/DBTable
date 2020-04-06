﻿#ifndef _DB_TOOL_DEFINE_H_
#define _DB_TOOL_DEFINE_H_


#include "common.h"

//字段类型
enum COLUMN_DATA_TYPE 
{
	CDT_UNKNOW,
	CDT_BIT,
	CDT_TINYINT,
	CDT_BOOL,
	CDT_SMALLINT,
	CDT_MEDIUMINT,
	CDT_INT,
	CDT_BIGINT,
	CDT_FLOAT,
	CDT_DOUBLE,

	CDT_DATE,
	CDT_DATATIME,
	CDT_TIMESTAMP,
	CDT_TIME,
	CDT_YEAR,

	CDT_CHAR,
	CDT_VARCHAR,
	CDT_BINARY,
	CDT_VARBINARY,
	CDT_TINYBLOB,
	CDT_TINYTEXT,
	CDT_BLOB,
	CDT_TEXT,
	CDT_MEDIUMBLOB,
	CDT_MEDIUMTEXT,
	CDT_LONGBLOB,
	CDT_LONGTEXT,
	CDT_ENUM,
	CDT_SET,
};

#endif//_DB_TOOL_DEFINE_H_