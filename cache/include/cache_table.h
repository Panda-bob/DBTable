#ifndef _DB_TOOL_CACHE_TABLE_H_
#define _DB_TOOL_CACHE_TABLE_H_

#include <string>
#include <vector>
#include "xcore_define.h"
#include "cache_column.h"
using namespace std;


class Cache_Table
{
public:
	string table_schema;
	string table_name;
	string table_class_name;
	vector<uint32> table_key_index;
	vector<uint32> m_normal_key_names;
	vector<uint32> m_unique_key_names;
	string engine;
	string auto_increment;
	string table_comment;
	string table_createtime;
	vector<Cache_column> m_columns;
};

class Cache_schema
{
public:
	string schema_name;
	vector <Cache_Table> m_tables;
};

#endif//_DB_TOOL_CACHE_TABLE_H_