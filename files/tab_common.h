#ifndef _TAB_COMMON_H_
#define _TAB_COMMON_H_

#include "xcore_define.h"

namespace db 
{

class TabCommon
{
public:
	TabCommon();

	static bool CreateTable(string& strSql);
	
	static shared_ptr<Conn> Connect();
	
	static bool Delete(const string& table,const string& key);
	
	static bool Delete(const string& table);

};//class

};//namespace db
using namespace db;
#endif //_TAB_COMMON_H_
