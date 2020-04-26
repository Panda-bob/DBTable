#include "xcore_str_util.h"
#include "tab_common.h"

namespace db 
{

TabCommon::TabCommon()
{
	//empty
}
bool TabCommon::CreateTable(string& strSql)
{
	string strSqlUtf8;
	VERIFY(gbk_to_utf8(strSqlUtf8, strSql));
	if ( strSqlUtf8.length() == 0 ) 
		return false;

	shared_ptr<Conn> ptrConn = Database::Instance()->Connect();
	shared_ptr<Result> ptrResult = ptrConn->Prepare(strSqlUtf8.c_str())->Exec();
	if (NULL == ptrResult.get())
		return false;

	return true;
}
shared_ptr<Conn> TabCommon::Connect()
{
	shared_ptr<Conn> ptrConn = Database::Instance()->Connect();
	return ptrConn;
}
bool TabCommon::Delete(const string& table,const string& key)
{
	shared_ptr<Conn> ptrConn = Database::Instance()->Connect();
	if (NULL == ptrConn.get())
		return false;

	shared_ptr<Result> ptrResult = ptrConn->Prepare("delete from @p_table where `key` = @p_key")
			->SetParameter("@p_table", table)
			->SetParameter("@p_key", key)
			->Exec();
		if (NULL == ptrResult.get())
			return false;

	return true;
}
bool TabCommon::Delete(const string& table)
{
	shared_ptr<Conn> ptrConn = Database::Instance()->Connect();
	if (NULL == ptrConn.get())
		return false;

	shared_ptr<Result> ptrResult = ptrConn->Prepare("delete from @p_table ")
			->SetParameter("@p_table", table)
			->Exec();
	if (NULL == ptrResult.get())
		return false;

	return true;
}

};//namespace db
