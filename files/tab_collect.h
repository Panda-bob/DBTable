#ifndef _DB_TABLE_TAB_COLLECT_
#define _DB_TABLE_TAB_COLLECT_

#include "xcore_define.h"

namespace db 
{

class TabCollect
{
public:
	TabCollect();

	static bool CreateTable();

	static bool Insert(const vector<TabCollect>& vecData);

	static bool InsertAutoKey(vector<TabCollect>& vecData);

	static bool Insert(const TabCollect& Data);

	static bool InsertAutoKey(TabCollect& Data);

	static bool Replace(const TabCollect& Data);

	static bool Delete(uint64 collect_id);

	static bool Delete(const vector<uint64>& collect_ids);

	static bool Delete();

	static int Select(TabCollect& Data);

	static int SelectAll(vector<TabCollect>& vecData);

	static int SelectEx(vector<TabCollect>& vecData, const string& strWhere);

	static int SelectLimit(vector<TabCollect>& vecData, uint32 start_index, uint32 data_num);

	static int SelectWithKeys(vector<TabCollect>& vecData, const set<uint64>& collect_ids);

	static int SelectWithKeys(vector<TabCollect>& vecData, const vector<uint64>& collect_ids);

	static bool Update(const TabCollect& Data);

	//mapUpdate : key - column name, value - column data, strWhere eg : roleid = '100000'
	static bool UpdateEx(const map<string, string>& mapUpdate, const string& strWhere);

	static int IsDataExist(uint64 collect_id);

	static bool GetAllPrimaryKeys(vector<uint64>& collect_ids);

	static uint32 GetTableCount();

private:
	static string get_create_table_sql();

	static int Select(vector<TabCollect>& vecData, string& sql_str);

public:
	uint64 m_collect_id;
	string m_collect_infos;

};//class

};//namespace db
using namespace db;
#endif //_DB_TABLE_TAB_COLLECT_
