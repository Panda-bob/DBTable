#include "tab_collect.h"
#include "xcore_str_util.h"
#include "tab_common.h"

namespace db 
{

TabCollect::TabCollect()
{
	//empty
}

string TabCollect::get_create_table_sql()
{
	string strRet;
	strRet = " create table IF NOT EXISTS tab_collect(`collect_id` bigint(64) NOT NULL  ,"
				"`collect_infos` blob NOT NULL  ,"
				"PRIMARY KEY(`collect_id`)) ENGINE=INNODB DEFAULT CHARSET=utf8";
	return strRet;
}

bool TabCollect::CreateTable()
{
	string strSql = get_create_table_sql();
	return TabCommon::CreateTable(strSql);
}

bool TabCollect::Insert(const vector<TabCollect>& vecData)
{
	ConnPtr ptrConn = TabCommon::Connect();
	if (NULL == ptrConn.get())
	{
		return false;
	}

	ptrConn->Begin();
	for (unsigned int i = 0; i < vecData.size(); ++i)
	{
		ResultPtr ptrResult = ptrConn->Prepare("insert into tab_collect values(@p_collect_id, @p_collect_infos)")
			->SetParameter("@p_collect_id", vecData[i].m_collect_id)
			->SetParameter("@p_collect_infos", Value(vecData[i].m_collect_infos.c_str(), vecData[i].m_collect_infos.length()))
			->Exec();
		if (NULL == ptrResult.get())
		{
			return false;
		}

	}
	ptrConn->Commit();
	return true;
}

bool TabCollect::InsertAutoKey(vector<TabCollect>& vecData)
{
	return false;
}

bool TabCollect::Insert(const TabCollect& Data)
{
	ConnPtr ptrConn = TabCommon::Connect();
	if (NULL == ptrConn.get())
	{
		return false;
	}

	ResultPtr ptrResult = ptrConn->Prepare("insert into tab_collect values(@p_collect_id, @p_collect_infos)")
		->SetParameter("@p_collect_id", Data.m_collect_id)
		->SetParameter("@p_collect_infos", Value(Data.m_collect_infos.c_str(), Data.m_collect_infos.length()))
		->Exec();
	if (NULL == ptrResult.get())
	{
		return false;
	}

	return true;
}

bool TabCollect::InsertAutoKey(TabCollect& Data)
{
	return false;
}

bool TabCollect::Replace(const TabCollect& Data)
{
	ConnPtr ptrConn = TabCommon::Connect();
	if (NULL == ptrConn.get())
	{
		return false;
	}

	ResultPtr ptrResult = ptrConn->Prepare("replace into tab_collect values(@p_collect_id, @p_collect_infos)")
		->SetParameter("@p_collect_id", Data.m_collect_id)
		->SetParameter("@p_collect_infos", Value(Data.m_collect_infos.c_str(), Data.m_collect_infos.length()))
		->Exec();
	if (NULL == ptrResult.get())
	{
		return false;
	}

	return true;
}

bool TabCollect::Delete(uint64 collect_id)
{
	string str_where = XStrUtil::sprintf("collect_id = %llu", collect_id);
	return TabCommon::Delete("tab_collect", str_where);
}

bool TabCollect::Delete(const vector<uint64>& collect_ids)
{
	string str_where = "collect_id in(";
	vector<uint64>::const_iterator it = collect_ids.begin();
	while (it != collect_ids.end())
	{
		uint64 collect_id = *it;
		it++;
		if (it == collect_ids.end())
		{
			str_where += XStrUtil::sprintf(" %llu)", collect_id);
			break;
		}
		else
		{
			str_where += XStrUtil::sprintf(" %llu,", collect_id);
		}
	}
	return TabCommon::Delete("tab_collect", str_where);
}

bool TabCollect::Delete()
{
	return TabCommon::Delete("tab_collect");
}

int TabCollect::Select(vector<TabCollect>& vecData, string& sql_str)
{
	ConnPtr ptrConn = TabCommon::Connect();
	if (NULL == ptrConn.get())
	{
		return -1;
	}

	RowsPtr ptrRows = ptrConn->Prepare(sql_str.c_str())
			->Query();
	if (NULL == ptrRows.get())
	{
		return -1;
	}

	int nRet = 0;
	TabCollect Data;
	while(ptrRows.get() && ptrRows->Next())
	{
		uint32 index = 0;
		Data.m_collect_id = (uint64) ptrRows->GetInt(index++);
		Data.m_collect_infos = ptrRows->GetBlob(index++);
		vecData.push_back(Data);
		++nRet;
	}
	return nRet;
}

int TabCollect::Select(TabCollect& Data)
{
	vector<TabCollect> vecData;
	string sql_str = XStrUtil::sprintf("select * from tab_collect where collect_id = %llu", Data.m_collect_id);
	int nRet = Select(vecData, sql_str);
	if (nRet > 0 && !vecData.empty())
	{
		Data = vecData.back();
	}
	return nRet;
}

int TabCollect::SelectAll(vector<TabCollect>& vecData)
{
	string sql_str = "select * from tab_collect ";
	return Select(vecData, sql_str);
}

int TabCollect::SelectEx(vector<TabCollect>& vecData, const string& strWhere)
{
	string sql_str = "select * from tab_collect where " + strWhere + " ;";
	return Select(vecData, sql_str);
}

int TabCollect::SelectLimit(vector<TabCollect>& vecData, uint32 start_index, uint32 data_num)
{
	string sql_str = XStrUtil::sprintf("select * from tab_collect limit %u, %u", start_index, data_num);
	return Select(vecData, sql_str);
}

int TabCollect::SelectWithKeys(vector<TabCollect>& vecData, const set<uint64>& collect_ids)
{
	string sql_str = "select * from tab_collect where collect_id in(";
	set<uint64>::const_iterator it = collect_ids.begin();
	while (it != collect_ids.end())
	{
		uint64 collect_id = *it;
		it++;
		if (it == collect_ids.end())
		{
			sql_str += XStrUtil::sprintf(" %llu)", collect_id);
			break;
		}
		else
		{
			sql_str += XStrUtil::sprintf(" %llu,", collect_id);
		}
	}
	return Select(vecData, sql_str);
}

int TabCollect::SelectWithKeys(vector<TabCollect>& vecData, const vector<uint64>& collect_ids)
{
	string sql_str = "select * from tab_collect where collect_id in(";
	vector<uint64>::const_iterator it = collect_ids.begin();
	while (it != collect_ids.end())
	{
		uint64 collect_id = *it;
		it++;
		if (it == collect_ids.end())
		{
			sql_str += XStrUtil::sprintf(" %llu)", collect_id);
			break;
		}
		else
		{
			sql_str += XStrUtil::sprintf(" %llu,", collect_id);
		}
	}
	return Select(vecData, sql_str);
}

bool TabCollect::Update(const TabCollect& Data)
{
	ConnPtr ptrConn = TabCommon::Connect();
	if (NULL == ptrConn.get())
	{
		return false;
	}

	ResultPtr ptrResult = ptrConn->Prepare("update tab_collect set collect_infos = @p_collect_infos where collect_id = @p_collect_id")
			->SetParameter("@p_collect_id", Data.m_collect_id)
			->SetParameter("@p_collect_infos", Value(Data.m_collect_infos.c_str(), Data.m_collect_infos.length()))
			->Exec();
	if (NULL == ptrResult.get())
	{
		return false;
	}

	return (ptrResult->RowsAffected() >= 0);
}

//mapUpdate : key - column name, value - column data, strWhere eg : roleid = '100000'
bool TabCollect::UpdateEx(const map<string, string>& mapUpdate, const string& strWhere)
{
	return TabCommon::UpdateEx("tab_collect", mapUpdate, strWhere);
}

int TabCollect::IsDataExist(uint64 collect_id)
{
	ConnPtr ptrConn = TabCommon::Connect();
	if (NULL == ptrConn.get())
	{
		return -1;
	}

	string sql_str = XStrUtil::sprintf("select count(*) from tab_collect where collect_id = %llu;", collect_id); 
	RowsPtr ptrRows = ptrConn->Prepare(sql_str.c_str())
		->Query();
	if (NULL == ptrRows.get())
	{
		return -1;
	}

	int nRet = 0;
	if (ptrRows.get() && ptrRows->Next())
	{
		nRet = (int) ptrRows->GetInt(0);
	}
	return nRet;
}

bool TabCollect::GetAllPrimaryKeys(vector<uint64>& collect_ids)
{
	ConnPtr ptrConn = TabCommon::Connect();
	if (NULL == ptrConn.get())
	{
		return false;
	}

	string sql_str = XStrUtil::sprintf("select collect_id from tab_collect");
	RowsPtr ptrRows = ptrConn->Prepare(sql_str.c_str())
		->Query();
	if (NULL == ptrRows.get())
	{
		return false;
	}

	while (ptrRows.get() && ptrRows->Next())
	{
		uint64 collect_id = (uint64) ptrRows->GetInt(0);
		collect_ids.push_back(collect_id);
	}
	return true;
}

uint32 TabCollect::GetTableCount()
{
	ConnPtr ptrConn = TabCommon::Connect();
	if (NULL == ptrConn.get())
	{
		return 0;
	}

	string sql_str = XStrUtil::sprintf("select count(*) from tab_collect");
	RowsPtr ptrRows = ptrConn->Prepare(sql_str.c_str())
		->Query();
	if (NULL == ptrRows.get())
	{
		return 0;
	}

	uint32 table_count = 0;
	if (ptrRows.get() && ptrRows->Next())
	{
		table_count = (uint32) ptrRows->GetInt(0);
	}
	return table_count;
}

};//namespace db
