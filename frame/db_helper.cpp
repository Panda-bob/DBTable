#include "db_helper.h"
#include "xcore_log.h"
#include "xcore_str_util.h"
#include "xcore_conver.h"

static int s_portocolstart = 3500;
bool DBConvertProtocol::make(const string& dirPath, const Cache_schema& schema)
{
	string protocalpath = dirPath + "/" + schema.schema_name + ".protocol";
	FILE* pf = fopen(protocalpath.c_str(), "w+");
	if (!pf)
		return false;

	if (!make_protocal(pf, schema))
	{
		fclose(pf);
		return false;
	}

	fclose(pf);
	pf = NULL;
	return true;
}

bool DBConvertProtocol::make_protocal(FILE* pf, const Cache_schema& schema)
{
	for (unsigned int i = 0; i < schema.m_tables.size(); ++i)
	{
		const Cache_Table& table = schema.m_tables[i];
		fprintf(pf, "message %sReqBody = %d\n{\n", table.table_class_name.c_str(), s_portocolstart++);

		for (unsigned int j = 0; j < table.m_columns.size(); ++j)
		{
			const Cache_column& column = table.m_columns[j];
			fprintf(pf, "\t%s %s", column.c_member_type.c_str(), TrimPrefix(column.column_name).c_str());
			fprintf(pf,";\n");
		}

		fprintf(pf, "}\n\n");

		fprintf(pf, "message %sRespBody = %d\n{\n", table.table_class_name.c_str(), s_portocolstart++);
		fprintf(pf, "}\n\n");
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////


bool DBConvertRecordFunc::make(const string& dirPath, const Cache_schema& schema)
{
	string headpath = dirPath + "/" + schema.schema_name + ".h";
	string srcpath = dirPath + "/" + schema.schema_name + ".cpp";

	FILE* pf = fopen(headpath.c_str(), "w+");
	if (!pf)
		return false;

	if(!make_h_func(pf, schema))
	{
		fclose(pf);
		return false;
	}
	fclose(pf);

	pf = fopen(srcpath.c_str(), "w+");
	if (!pf)
		return false;

	if(!make_cpp_func(pf, schema))
	{
		fclose(pf);
		return false;
	}
	fclose(pf);
	return true;
}


bool DBConvertRecordFunc::make_h_func(FILE* pf, const Cache_schema& schema)
{
	for (unsigned int i = 0; i < schema.m_tables.size(); ++i)
	{
		const Cache_Table& table = schema.m_tables[i];
		if(!make_h_table_func(pf, table)){
			printf("make_h_table_func error");
			return false;
		}
	}
	return true;
}

bool DBConvertRecordFunc::make_h_table_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\tvoid Record(%sRequest* request);\n", table.table_class_name.c_str());
	return true;
}

bool DBConvertRecordFunc::make_cpp_func(FILE* pf, const Cache_schema& schema)
{
	for (unsigned int i = 0; i < schema.m_tables.size(); ++i)
	{
		const Cache_Table& table = schema.m_tables[i];
		if(!make_cpp_table_func(pf, table)){
			printf("make_h_table_func error");
			return false;
		}
	}
	return true;
}

bool DBConvertRecordFunc::make_cpp_table_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "void CacheGameServer::Record(%sRequest* request)\n{\n", table.table_class_name.c_str());
	fprintf(pf, "\tif (m_ptrConn.get() == NULL) return;\n");
	for (unsigned int i = 0; i < table.m_columns.size(); ++i){
		const Cache_column& column = table.m_columns[i];
		if (column.c_member_type == "string "){
			fprintf(pf, "\trequest->body().%s = m_ptrConn->escape_string(request->body().%s);\n", column.c_member_name.c_str(), column.c_member_name.c_str() );
		}	
	}
	
	fprintf(pf, "\tXLockGuard<XAtomicLocker> lock(m_lock);\n");
	fprintf(pf, "\tshared_ptr<IXBuffer> ptrBuffer;\n");
	fprintf(pf, "\tif (!m_queueGame.empty()) ptrBuffer = m_queueGame.back();\n");
	
	fprintf(pf, "\tif (ptrBuffer.get() == NULL || ptrBuffer->size() > 600 * 1024)\n\t{\n");
	fprintf(pf, "\t\tptrBuffer = xcore::create_buffer(640 * 1024);\n");
	fprintf(pf, "\t\tm_queueGame.push_back(ptrBuffer);\n");

	fprintf(pf, "\t\t%sBody& body = request->body();\n", table.table_class_name.c_str());
	fprintf(pf, "\t\tchar* pos = (char*)ptrBuffer->data();\n");
	fprintf(pf, "\t\tint ret = sprintf(pos, \"insert into %s values(", table.table_name.c_str());
	for(unsigned int i = 0; i < table.m_columns.size(); ++i)
	{
		const Cache_column& column = table.m_columns[i];
		if (column.extra.find("auto_increment") != string::npos){
			fprintf(pf, "null");
		}else{
			if (column.c_member_type == "string "){
				fprintf(pf, "'%s'", column.c_format.c_str());
			}else{
				fprintf(pf, "%s", column.c_format.c_str());
			}
		}

		if (i != table.m_columns.size() - 1){
			fprintf(pf, ",");
		}else{
			fprintf(pf, ")\",\n");
		}
	}
	fprintf(pf,"\t\t\t\t\t\t\t");
	for(unsigned int i = 0; i < table.m_columns.size(); ++i)
	{
		const Cache_column& column = table.m_columns[i];

		if (column.extra.find("auto_increment") != string::npos)
			continue;

		if (column.c_format == "%s"){
			fprintf(pf, "body.%s.c_str()", column.c_member_name.c_str());
		}else{
			fprintf(pf, "body.%s", column.c_member_name.c_str());
		}
		
		if (i != table.m_columns.size() - 1){
			fprintf(pf, ", ");
		}else{
			fprintf(pf, ");\n");
		}
	}
	fprintf(pf, "\t\tASSERT(ret > 0);\n");
	fprintf(pf, "\t\tptrBuffer->resize((uint32)ret);\n");

	fprintf(pf, "\t}\n\telse\n\t{\n");/////////////////////////////else

	fprintf(pf, "\t\t%sBody& body = request->body();\n", table.table_class_name.c_str());
	fprintf(pf, "\t\tchar* pos = (char*)ptrBuffer->data();\n");
	fprintf(pf, "\t\tint ret = sprintf(pos, \"insert into %s values(", table.table_name.c_str());
	for(unsigned int i = 1; i < table.m_columns.size(); ++i)
	{
		const Cache_column& column = table.m_columns[i];
		if (column.extra.find("auto_increment") != string::npos){
			fprintf(pf, "null");
		}else{
			if (column.c_member_type == "string "){
				fprintf(pf, "'%s'", column.c_format.c_str());
			}else{
				fprintf(pf, "%s", column.c_format.c_str());
			}
		}

		if (i != table.m_columns.size() - 1){
			fprintf(pf, ",");
		}else{
			fprintf(pf, ")\",\n");
		}
	}

	fprintf(pf,"\t\t\t\t\t\t\t");
	for(unsigned int i = 1; i < table.m_columns.size(); ++i)
	{
		const Cache_column& column = table.m_columns[i];

		if (column.extra.find("auto_increment") != string::npos)
			continue;

		if (column.c_format == "%s"){
			fprintf(pf, "body.%s.c_str()", column.c_member_name.c_str());
		}else{
			fprintf(pf, "body.%s", column.c_member_name.c_str());
		}

		if (i != table.m_columns.size() - 1){
			fprintf(pf, ", ");
		}else{
			fprintf(pf, ");\n");
		}
	}
	fprintf(pf, "\t\tASSERT(ret > 0);\n");
	fprintf(pf, "\t\tptrBuffer->resize(ptrBuffer->size() + (uint32)ret);\n");

	fprintf(pf, "\t}\n");
	fprintf(pf, "\tm_event.set();\n");
	fprintf(pf, "}\n\n");
	return true;
}



//////////////////////////////////////////////////////////////////////////


bool DBConvertCPP::make(const string& dirPath, const Cache_Table& table)
{
	string headpath = dirPath + "/" + table.table_name + ".h";
	string srcpath = dirPath + "/" + table.table_name + ".cpp";

	FILE* pf = fopen(headpath.c_str(), "w+");
	if (!pf)
		return false;

	if(!make_h_file(pf, table))
	{
		fclose(pf);
		return false;
	}
	fclose(pf);

	pf = fopen(srcpath.c_str(), "w+");
	if (!pf)
		return false;

	if(!make_cpp_file(pf, table))
	{
		fclose(pf);
		return false;
	}
	fclose(pf);
	return true;
}


bool DBConvertCPP::make_h_file(FILE* pf, const Cache_Table& table)
{
	if (NULL == pf)
		return false;

	string strTablename = table.table_name;
	XStrUtil::to_upper(strTablename);
	fprintf(pf, "#ifndef _DB_TABLE_%s_\n", strTablename.c_str());
	fprintf(pf, "#define _DB_TABLE_%s_\n\n", strTablename.c_str());

	fprintf(pf, "#include \"xcore_define.h\"\n\n");

	fprintf(pf, "namespace db \n{\n\n");
	fprintf(pf, "class %s\n{\n", table.table_class_name.c_str());
	fprintf(pf,"public:\n");//接口函数

	if (!make_h_construct(pf, table))
		return false;

	if (!make_h_create_table(pf))
		return false;

	if (!make_h_insert_func(pf, table))
		return false;

	if (!make_h_insert_autokey_func(pf, table))
		return false;

	if (!make_h_insert_one_func(pf, table))
		return false;

	if (!make_h_insert_autokey_one_func(pf, table))
		return false;
	
	if (!make_h_replace(pf, table))
		return false;

	if (!make_h_delete_func(pf, table))
		return false;

	if (!make_h_delete_with_keys_func(pf, table))
		return false;
	
	if (!make_h_delete_all_func(pf, table))
		return false;

	if (!make_h_select_func(pf, table))
		return false;

	if (!make_h_select_all_func(pf, table))
		return false;

	if (!make_h_select_ex_func(pf, table))
		return false;
	
	if (!make_h_select_limit_func(pf, table))
		return false;
	
	if (!make_h_select_with_keys_func(pf, table))
		return false;

	if (!make_h_update_func(pf, table))
		return false;

	if (!make_h_update_ex_func(pf, table))
		return false;

	if (!make_h_is_data_exist_func(pf, table))
		return false;

	if (!make_h_get_primary_keys_with_normal_key_func(pf, table))
		return false;

	if (!make_h_get_primary_key_with_unique_key_func(pf, table))
		return false;

	if (!make_h_get_all_primary_keys_func(pf, table))
		return false;

	if (!make_h_get_table_count_func(pf, table))
		return false;
	
	fprintf(pf,"\nprivate:\n");///private 函数

	if (!make_h_create_table_sql(pf, table))
		return false;
	
	if (!make_h_select_private_func(pf, table))
		return false;

	fprintf(pf,"public:\n");//成员变量

	string strColumn;
	for (unsigned int i = 0 ; i < table.m_columns.size(); ++i)
	{
		fprintf(pf, "\t%s %s;\n", table.m_columns[i].c_member_type.c_str(), table.m_columns[i].c_member_name.c_str());		
	}

	fprintf(pf, "\n");

	fprintf(pf,"};//class\n\n");
	fprintf(pf,"};//namespace db\n");
	fprintf(pf, "using namespace db;\n");
	fprintf(pf, "#endif //_DB_TABLE_%s_\n", strTablename.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_file(FILE* pf, const Cache_Table& table)
{
	if (NULL == pf)
		return false;

	fprintf(pf, "#include \"%s.h\"\n", table.table_name.c_str());
	fprintf(pf, "#include \"xcore_str_util.h\"\n");
	fprintf(pf, "#include \"tab_common.h\"\n\n");
	fprintf(pf, "namespace db \n{\n\n");

	if (!make_cpp_construct(pf, table))
		return false;

	if (!make_cpp_create_table_sql(pf, table))
		return false;

	if (!make_cpp_create_table(pf, table))
		return false;

	if (!make_cpp_insert_func(pf, table))
		return false;

	if (!make_cpp_insert_autokey_func(pf, table))
		return false;

	if (!make_cpp_insert_one_func(pf, table))
		return false;

	if (!make_cpp_insert_autokey_one_func(pf, table))
		return false;
	
	if (!make_cpp_replace(pf, table))
		return false;
	
	if (!make_cpp_delete_func(pf, table))
		return false;
	
	if (!make_cpp_delete_with_keys_func(pf, table))
		return false;

	if (!make_cpp_delete_all_func(pf, table))
		return false;
	
	if (!make_cpp_select_private_func(pf, table))
		return false;

	if (!make_cpp_select_func(pf, table))
		return false;

	if (!make_cpp_select_all_func(pf, table))
		return false;

	if (!make_cpp_select_ex_func(pf, table))
		return false;
	
	if (!make_cpp_select_limit_func(pf, table))
		return false;
	
	if (!make_cpp_select_with_keys_func(pf, table))
		return false;

	if (!make_cpp_update_func(pf, table))
		return false;

	if (!make_cpp_update_ex_func(pf, table))
		return false;

	if (!make_cpp_is_data_exist_func(pf, table))
		return false;

	if (!make_cpp_get_primary_keys_with_normal_key_func(pf, table))
		return false;

	if (!make_cpp_get_primary_key_with_unique_key_func(pf, table))
		return false;

	if (!make_cpp_get_all_primary_keys_func(pf, table))
		return false;

	if (!make_cpp_get_table_count_func(pf, table))
		return false;

	fprintf(pf,"};//namespace db\n");
	return true;
}

bool DBConvertCPP::make_h_create_table_sql(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\tstatic string get_create_table_sql();\n");
	return true;
}

bool DBConvertCPP::make_cpp_create_table_sql(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "string %s::get_create_table_sql()\n{\n", table.table_class_name.c_str());
	fprintf(pf, "\tstring strRet;\n");
	string strSql;
	if(make_table_createsql(table, strSql))
		fprintf(pf , "\tstrRet = \"%s\";\n" , strSql.c_str());

	fprintf(pf, "\treturn strRet;\n");
	fprintf(pf, "}\n\n");
	return true;
}

bool DBConvertCPP::make_h_create_table(FILE* pf)
{
	fprintf(pf, "\tstatic bool CreateTable();\n");
	return true;
}


bool DBConvertCPP::make_cpp_create_table(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "bool %s::CreateTable()\n{\n", table.table_class_name.c_str());
	fprintf(pf, "\tstring strSql = get_create_table_sql();\n");
	fprintf(pf, "\treturn TabCommon::CreateTable(strSql);\n");
	fprintf(pf, "}\n\n");
	return true;
}


bool DBConvertCPP::make_h_construct(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\t%s();\n\n", table.table_class_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_construct(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "%s::%s()", table.table_class_name.c_str(), table.table_class_name.c_str());
	string strTemp;
	vector<Cache_column> constrVec;
	for(unsigned int i = 0; i < table.m_columns.size(); ++i)
	{
		if (table.m_columns[i].c_construct_default.size() != 0)
		{
			constrVec.push_back(table.m_columns[i]);
		}
	}

	for (uint32 i = 0 ; i < constrVec.size(); ++i)
	{
		if (i == 0)
			fprintf(pf, "\n\t: ");
		else 
			fprintf(pf, "\n\t, ");
		
		fprintf(pf, "%s", constrVec[i].c_construct_default.c_str());
	}	

	fprintf(pf, "\n{\n\t//empty\n}\n\n");
	return true;
}

bool DBConvertCPP::make_h_insert_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\n\tstatic bool Insert(const vector<%s>& vecData);\n", table.table_class_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_insert_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "bool %s::Insert(const vector<%s>& vecData)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str());
	fprintf(pf, "\tConnPtr ptrConn = TabCommon::Connect();\n");
	fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn false;\n\t}\n\n");
	fprintf(pf, "\tptrConn->Begin();\n");
	fprintf(pf, "\tfor (unsigned int i = 0; i < vecData.size(); ++i)\n\t{\n");
	fprintf(pf, "\t\tResultPtr ptrResult = ptrConn->Prepare(\"insert into %s values(", table.table_name.c_str());

	for (unsigned int i = 1 ; i <= table.m_columns.size(); ++i)
	{
		unsigned int j;
		for ( j = 0; j < table.m_columns.size(); ++j)
		{
			if (table.m_columns[j].ordinal_position == i)
				break;
		}
		if (j == table.m_columns.size())
			return false;		

		fprintf(pf, "@p_%s", table.m_columns[j].column_name.c_str());

		if (i != table.m_columns.size())
			fprintf(pf, ", ");
		else
			fprintf(pf, ")\")\n");
	}

	for (unsigned int i = 1 ; i <= table.m_columns.size(); ++i)
	{
		unsigned int j;
		for ( j = 0; j < table.m_columns.size(); ++j)
		{
			if (table.m_columns[j].ordinal_position == i)
				break;
		}
		if (j == table.m_columns.size())
			return false;		

		fprintf(pf, "\t\t\t->SetParameter(\"@p_%s\", %s)\n", table.m_columns[j].column_name.c_str(), table.m_columns[j].c_use_vec_value.c_str());
	}
	fprintf(pf, "\t\t\t->Exec();\n");
	fprintf(pf, "\t\tif (NULL == ptrResult.get())\n\t\t{\n\t\t\treturn false;\n\t\t}\n\n");
	fprintf(pf, "\t}\n");
	fprintf(pf, "\tptrConn->Commit();\n");
	fprintf(pf, "\treturn true;\n");
	fprintf(pf,"}\n\n");
	return true;
}

bool DBConvertCPP::make_h_insert_one_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\n\tstatic bool Insert(const %s& Data);\n", table.table_class_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_insert_one_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "bool %s::Insert(const %s& Data)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str());
	fprintf(pf, "\tConnPtr ptrConn = TabCommon::Connect();\n");
	fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn false;\n\t}\n\n");
	fprintf(pf, "\tResultPtr ptrResult = ptrConn->Prepare(\"insert into %s values(", table.table_name.c_str());

	for (unsigned int i = 1 ; i <= table.m_columns.size(); ++i)
	{
		unsigned int j;
		for ( j = 0; j < table.m_columns.size(); ++j)
		{
			if (table.m_columns[j].ordinal_position == i)
				break;
		}
		if (j == table.m_columns.size())
			return false;		

		fprintf(pf, "@p_%s", table.m_columns[j].column_name.c_str());

		if (i != table.m_columns.size())
			fprintf(pf, ", ");
		else
			fprintf(pf, ")\")\n");
	}

	for (unsigned int i = 1 ; i <= table.m_columns.size(); ++i)
	{
		unsigned int j;
		for ( j = 0; j < table.m_columns.size(); ++j)
		{
			if (table.m_columns[j].ordinal_position == i)
				break;
		}
		if (j == table.m_columns.size())
			return false;		

		fprintf(pf, "\t\t->SetParameter(\"@p_%s\", %s)\n", table.m_columns[j].column_name.c_str(), table.m_columns[j].c_use_data_value.c_str());
	}
	fprintf(pf, "\t\t->Exec();\n");
	fprintf(pf, "\tif (NULL == ptrResult.get())\n\t{\n\t\treturn false;\n\t}\n\n");
	fprintf(pf, "\treturn true;\n");
	fprintf(pf,"}\n\n");
	return true;
}

bool DBConvertCPP::make_h_replace(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\n\tstatic bool Replace(const %s& Data);\n", table.table_class_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_replace(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "bool %s::Replace(const %s& Data)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str());
	fprintf(pf, "\tConnPtr ptrConn = TabCommon::Connect();\n");
	fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn false;\n\t}\n\n");
	fprintf(pf, "\tResultPtr ptrResult = ptrConn->Prepare(\"replace into %s values(", table.table_name.c_str());

	for (unsigned int i = 1; i <= table.m_columns.size(); ++i)
	{
		unsigned int j;
		for (j = 0; j < table.m_columns.size(); ++j)
		{
			if (table.m_columns[j].ordinal_position == i)
				break;
		}
		if (j == table.m_columns.size())
			return false;

		fprintf(pf, "@p_%s", table.m_columns[j].column_name.c_str());

		if (i != table.m_columns.size())
			fprintf(pf, ", ");
		else
			fprintf(pf, ")\")\n");
	}

	for (unsigned int i = 1; i <= table.m_columns.size(); ++i)
	{
		unsigned int j;
		for (j = 0; j < table.m_columns.size(); ++j)
		{
			if (table.m_columns[j].ordinal_position == i)
				break;
		}
		if (j == table.m_columns.size())
			return false;

		fprintf(pf, "\t\t->SetParameter(\"@p_%s\", %s)\n", table.m_columns[j].column_name.c_str(), table.m_columns[j].c_use_data_value.c_str());
	}
	fprintf(pf, "\t\t->Exec();\n");
	fprintf(pf, "\tif (NULL == ptrResult.get())\n\t{\n\t\treturn false;\n\t}\n\n");
	fprintf(pf, "\treturn true;\n");
	fprintf(pf, "}\n\n");
	return true;

}


bool DBConvertCPP::make_h_insert_autokey_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\n\tstatic bool InsertAutoKey(vector<%s>& vecData);\n", table.table_class_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_insert_autokey_func(FILE* pf, const Cache_Table& table)
{
	uint32 nauto = 0;
	for (unsigned int iIndex = 0 ; iIndex < table.m_columns.size(); ++iIndex)
	{
		if (table.m_columns[iIndex].extra.find("auto_increment") != string::npos)
			++nauto;
	}

	if (table.table_key_index.size() != 1 || nauto != 1)
	{//非单key且有自增长的表该函数置空
		fprintf(pf, "bool %s::InsertAutoKey(vector<%s>& vecData)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str());
		fprintf(pf, "\treturn false;\n");
		fprintf(pf,"}\n\n");
		return true;
	}

	//如果有单个key
	fprintf(pf, "bool %s::InsertAutoKey(vector<%s>& vecData)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str());
	fprintf(pf, "\tConnPtr ptrConn = TabCommon::Connect();\n");
	fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn false;\n\t}\n\n");
	fprintf(pf, "\tptrConn->Begin();\n");
	fprintf(pf, "\tfor (unsigned int i = 0; i < vecData.size(); ++i)\n\t{\n");
	fprintf(pf, "\t\tResultPtr ptrResult = ptrConn->Prepare(\"insert into %s values(", table.table_name.c_str());

	for (unsigned int i = 1 ; i <= table.m_columns.size(); ++i)
	{
		unsigned int j;
		for ( j = 0; j < table.m_columns.size(); ++j)
		{
			if (table.m_columns[j].ordinal_position == i)
				break;
		}
		if (j == table.m_columns.size())
			return false;		

		if (table.m_columns[j].extra.find("auto_increment") == string::npos)
		{
			fprintf(pf, "@p_%s", table.m_columns[j].column_name.c_str());
		}
		else
		{
			fprintf(pf, "null");
		}
		if (i != table.m_columns.size())
			fprintf(pf, ", ");
		else
			fprintf(pf, ")\")\n");
	}

	for (unsigned int i = 1 ; i <= table.m_columns.size(); ++i)
	{
		unsigned int j;
		for ( j = 0; j < table.m_columns.size(); ++j)
		{
			if (table.m_columns[j].ordinal_position == i)
				break;
		}
		if (j == table.m_columns.size())
			return false;		

		if (table.m_columns[j].extra.find("auto_increment") == string::npos)
		{
			fprintf(pf, "\t\t\t->SetParameter(\"@p_%s\", %s)\n", table.m_columns[j].column_name.c_str(), table.m_columns[j].c_use_vec_value.c_str());
		}		
	}
	fprintf(pf, "\t\t\t->Exec();\n");
	fprintf(pf, "\t\tif (NULL == ptrResult.get())\n\t\t{\n\t\t\treturn false;\n\t\t}\n\n");
	fprintf(pf, "\t\t vecData[i].%s = (uint32)ptrResult->LastInsertId();\n", table.m_columns[table.table_key_index[0]].c_member_name.c_str());
	fprintf(pf, "\t}\n");
	fprintf(pf, "\tptrConn->Commit();\n");
	fprintf(pf, "\treturn true;\n");
	fprintf(pf,"}\n\n");
	return true;
}

bool DBConvertCPP::make_h_insert_autokey_one_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\n\tstatic bool InsertAutoKey(%s& Data);\n", table.table_class_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_insert_autokey_one_func(FILE* pf, const Cache_Table& table)
{
	uint32 nauto = 0;
	for (unsigned int iIndex = 0 ; iIndex < table.m_columns.size(); ++iIndex)
	{
		if (table.m_columns[iIndex].extra.find("auto_increment") != string::npos)
			++nauto;
	}

	if (table.table_key_index.size() != 1 || nauto != 1)
	{//非单key且有自增长的表该函数置空
		fprintf(pf, "bool %s::InsertAutoKey(%s& Data)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str());
		fprintf(pf, "\treturn false;\n");
		fprintf(pf,"}\n\n");
		return true;
	}

	fprintf(pf, "bool %s::InsertAutoKey(%s& Data)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str());
	fprintf(pf, "\tConnPtr ptrConn = TabCommon::Connect();\n");
	fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn false;\n\t}\n\n");
	fprintf(pf, "\tResultPtr ptrResult = ptrConn->Prepare(\"insert into %s values(", table.table_name.c_str());

	for (unsigned int i = 1 ; i <= table.m_columns.size(); ++i)
	{
		unsigned int j;
		for ( j = 0; j < table.m_columns.size(); ++j)
		{
			if (table.m_columns[j].ordinal_position == i)
				break;
		}
		if (j == table.m_columns.size())
			return false;		

		if (table.m_columns[j].extra.find("auto_increment") == string::npos)
		{
			fprintf(pf, "@p_%s", table.m_columns[j].column_name.c_str());
		}
		else
		{
			fprintf(pf, "null");
		}
		if (i != table.m_columns.size())
			fprintf(pf, ", ");
		else
			fprintf(pf, ")\")\n");
	}

	for (unsigned int i = 1 ; i <= table.m_columns.size(); ++i)
	{
		unsigned int j;	
		for ( j = 0; j < table.m_columns.size(); ++j)
		{
			if (table.m_columns[j].ordinal_position == i)
				break;
		}
		if (j == table.m_columns.size())
			return false;		

		if (table.m_columns[j].extra.find("auto_increment") == string::npos)
		{
			fprintf(pf, "\t\t\t->SetParameter(\"@p_%s\", %s)\n", table.m_columns[j].column_name.c_str(), table.m_columns[j].c_use_data_value.c_str());
		}		
	}
	fprintf(pf, "\t\t\t->Exec();\n");
	fprintf(pf, "\tif (NULL == ptrResult.get())\n\t{\n\t\treturn false;\n\t}\n\n");
	fprintf(pf, "\tuint32 last_insertid = (uint32)ptrResult->LastInsertId();\n");
	fprintf(pf, "\tData.%s = last_insertid;\n", table.m_columns[table.table_key_index[0]].c_member_name.c_str());
	fprintf(pf, "\treturn true;\n");
	fprintf(pf,"}\n\n");
	return true;
}

bool DBConvertCPP::make_h_update_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\n\tstatic bool Update(const %s& Data);\n", table.table_class_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_update_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "bool %s::Update(const %s& Data)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str());
	fprintf(pf, "\tConnPtr ptrConn = TabCommon::Connect();\n");
	fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn false;\n\t}\n\n");

	fprintf(pf, "\tResultPtr ptrResult = ptrConn->Prepare(\"update %s set", table.table_name.c_str());

	for (unsigned int i = 0 ; i < table.m_columns.size(); ++i)
	{
		if (table.table_key_index.end() != find(table.table_key_index.begin(), table.table_key_index.end(), i) )
			continue;

		fprintf(pf, " %s = @p_%s", table.m_columns[i].column_name.c_str(), table.m_columns[i].column_name.c_str());

		if (i != table.m_columns.size() - 1)
			fprintf(pf, ",");
	}

	fprintf(pf, " where ");

	for (unsigned int i = 0; i < table.table_key_index.size(); ++i)
	{
		const Cache_column &oneColumen = table.m_columns[table.table_key_index[i]];
		fprintf(pf,"%s = @p_%s", oneColumen.column_name.c_str(), oneColumen.column_name.c_str());
		if (i != table.table_key_index.size() - 1)
		{
			fprintf(pf, ", ");
		}
	}
	fprintf(pf, "\")\n");

	for (unsigned int i = 0 ; i < table.m_columns.size(); ++i)
	{
		fprintf(pf, "\t\t\t->SetParameter(\"@p_%s\", %s)\n", table.m_columns[i].column_name.c_str(), table.m_columns[i].c_use_data_value.c_str());			
	}
	fprintf(pf, "\t\t\t->Exec();\n");
	fprintf(pf, "\tif (NULL == ptrResult.get())\n\t{\n\t\treturn false;\n\t}\n\n");
	fprintf(pf, "\treturn (ptrResult->RowsAffected() >= 0);\n");
	fprintf(pf,"}\n\n");
	return true;
}

bool DBConvertCPP::make_h_update_ex_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\n\t//mapUpdate : key - column name, value - column data, strWhere eg : roleid = '100000'\n");
	fprintf(pf, "\tstatic bool UpdateEx(const map<string, string>& mapUpdate, const string& strWhere);\n");
	return true;
}


bool DBConvertCPP::make_cpp_update_ex_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "//mapUpdate : key - column name, value - column data, strWhere eg : roleid = '100000'\n");
	fprintf(pf, "bool %s::UpdateEx(const map<string, string>& mapUpdate, const string& strWhere)\n{\n", table.table_class_name.c_str());
	fprintf(pf, "\treturn TabCommon::UpdateEx(\"%s\", mapUpdate, strWhere);\n", table.table_name.c_str());
	fprintf(pf,"}\n\n");
	return true;
}

bool DBConvertCPP::make_h_delete_all_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\n\tstatic bool Delete();\n");
	return true;
}

bool DBConvertCPP::make_cpp_delete_all_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "bool %s::Delete()\n{\n", table.table_class_name.c_str());
	fprintf(pf, "\treturn TabCommon::Delete(\"%s\");\n", table.table_name.c_str());
	fprintf(pf,"}\n\n");
	return true;
}

bool DBConvertCPP::make_h_delete_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.size() == 0)
	{
		return true;
	}

	fprintf(pf, "\n\tstatic bool Delete(");

	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];
	if (oneColumen.c_member_type == "string"){
		fprintf(pf,"const string& %s", oneColumen.column_name.c_str());
	}else{
		fprintf(pf,"%s %s", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	}

	fprintf(pf, ");\n");
	return true;
}

bool DBConvertCPP::make_cpp_delete_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.size() == 0)
	{
		return true;
	}

	fprintf(pf, "bool %s::Delete(", table.table_class_name.c_str());

	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];
	if (oneColumen.c_member_type == "string"){
		fprintf(pf,"const string& %s", oneColumen.column_name.c_str());
	}else{
		fprintf(pf,"%s %s", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	}

	fprintf(pf, ")\n{\n");
	if (oneColumen.c_member_type == "string"){
		fprintf(pf, "\tstring str_where = XStrUtil::sprintf(\"%s = %s\", %s.c_str());\n", oneColumen.column_name.c_str(), oneColumen.c_format.c_str(), oneColumen.column_name.c_str());
	}
	else{
		fprintf(pf, "\tstring str_where = XStrUtil::sprintf(\"%s = %s\", %s);\n", oneColumen.column_name.c_str(), oneColumen.c_format.c_str(), oneColumen.column_name.c_str());
	}
	fprintf(pf, "\treturn TabCommon::Delete(\"%s\", str_where);\n", table.table_name.c_str());
	fprintf(pf,"}\n\n");

	return true;
}


bool DBConvertCPP::make_h_delete_with_keys_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.size() == 0)
	{
		return true;
	}
	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];

	fprintf(pf, "\n\tstatic bool Delete(const vector<");
	if (oneColumen.c_member_type == "string"){
		fprintf(pf,"string>& %ss", oneColumen.column_name.c_str());
	}else{
		fprintf(pf,"%s>& %ss", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	}
	fprintf(pf, ");\n");
	return true;
}

bool DBConvertCPP::make_cpp_delete_with_keys_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.size() == 0)
	{
		return true;
	}
	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];

	fprintf(pf, "bool %s::Delete(const vector<", table.table_class_name.c_str());
	if (oneColumen.c_member_type == "string"){
		fprintf(pf,"string>& %ss", oneColumen.column_name.c_str());
	}else{
		fprintf(pf,"%s>& %ss", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	}
	fprintf(pf, ")\n{\n");
	
	fprintf(pf, "\tstring str_where = \"%s in(\";\n", oneColumen.column_name.c_str());
	fprintf(pf, "\tvector<%s>::const_iterator it = %ss.begin();\n", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\twhile (it != %ss.end())\n\t{\n", oneColumen.column_name.c_str());
	fprintf(pf, "\t\t%s %s = *it;\n\t\tit++;\n", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\t\tif (it == %ss.end())\n\t\t{\n", oneColumen.column_name.c_str());
	fprintf(pf, "\t\t\tstr_where += XStrUtil::sprintf(\" %s)\", %s);\n", oneColumen.c_format.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\t\t\tbreak;\n\t\t}\n\t\telse\n\t\t{\n");
	fprintf(pf, "\t\t\tstr_where += XStrUtil::sprintf(\" %s,\", %s);\n", oneColumen.c_format.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\t\t}\n\t}\n");
	
	fprintf(pf, "\treturn TabCommon::Delete(\"%s\", str_where);\n", table.table_name.c_str());
	fprintf(pf,"}\n\n");
	
	return true;
}


bool DBConvertCPP::make_h_select_private_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\n\tstatic int Select(vector<%s>& vecData, string& sql_str);\n\n", table.table_class_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_select_private_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "int %s::Select(vector<%s>& vecData, string& sql_str)\n{\n", table.table_class_name.c_str(),  table.table_class_name.c_str());
	fprintf(pf, "\tConnPtr ptrConn = TabCommon::Connect();\n");
	fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn -1;\n\t}\n\n");
	fprintf(pf, "\tRowsPtr ptrRows = ptrConn->Prepare(sql_str.c_str())\n");
	fprintf(pf, "\t\t\t->Query();\n");
	fprintf(pf, "\tif (NULL == ptrRows.get())\n\t{\n\t\treturn -1;\n\t}\n\n");

	fprintf(pf, "\tint nRet = 0;\n");
	fprintf(pf, "\t%s Data;\n", table.table_class_name.c_str());
	fprintf(pf, "\twhile(ptrRows.get() && ptrRows->Next())\n\t{\n");
	fprintf(pf, "\t\tuint32 index = 0;\n");
	for (unsigned int i = 0 ; i < table.m_columns.size(); ++i)
	{
		fprintf(pf, "\t\t%s\n", table.m_columns[i].c_select_one_getvalu.c_str());
	}

	fprintf(pf, "\t\tvecData.push_back(Data);\n");
	fprintf(pf, "\t\t++nRet;\n");
	fprintf(pf, "\t}\n");
	fprintf(pf, "\treturn nRet;\n");
	fprintf(pf,"}\n\n");
	return true;
}


bool DBConvertCPP::make_h_select_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.empty())
	{
		return true;
	}

	fprintf(pf, "\n\tstatic int Select(%s& Data);\n", table.table_class_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_select_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.empty())
	{
		return true;
	}
	
	fprintf(pf, "int %s::Select(%s& Data)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str());
	fprintf(pf, "\tvector<%s> vecData;\n", table.table_class_name.c_str());
	fprintf(pf, "\tstring sql_str = XStrUtil::sprintf(\"select * from %s where ", table.table_name.c_str());
	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];
	if (oneColumen.c_member_type == "string"){
		fprintf(pf, "%s = %s\", Data.%s.c_str());\n", oneColumen.column_name.c_str(), oneColumen.c_format.c_str(), oneColumen.c_member_name.c_str());
	}
	else{
		fprintf(pf, "%s = %s\", Data.%s);\n", oneColumen.column_name.c_str(), oneColumen.c_format.c_str(), oneColumen.c_member_name.c_str());
	}
	
	fprintf(pf, "\tint nRet = Select(vecData, sql_str);\n");
	fprintf(pf, "\tif (nRet > 0 && !vecData.empty())\n\t{\n");
	fprintf(pf, "\t\tData = vecData.back();\n\t}\n");
	fprintf(pf, "\treturn nRet;\n");
	fprintf(pf,"}\n\n");
	return true;
}


bool DBConvertCPP::make_h_select_all_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\n\tstatic int SelectAll(vector<%s>& vecData);\n", table.table_class_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_select_all_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "int %s::SelectAll(vector<%s>& vecData)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str());
	fprintf(pf, "\tstring sql_str = \"select * from %s \";\n", table.table_name.c_str());
	fprintf(pf, "\treturn Select(vecData, sql_str);\n");
	fprintf(pf,"}\n\n");
	return true;
}


bool DBConvertCPP::make_h_select_ex_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\n\tstatic int SelectEx(vector<%s>& vecData, const string& strWhere);\n", table.table_class_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_select_ex_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "int %s::SelectEx(vector<%s>& vecData, const string& strWhere)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str());
	fprintf(pf, "\tstring sql_str = \"select * from %s where \" + strWhere + \" ;\";\n", table.table_name.c_str());
	fprintf(pf, "\treturn Select(vecData, sql_str);\n");
	fprintf(pf,"}\n\n");
	return true;
}


bool DBConvertCPP::make_h_select_limit_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\n\tstatic int SelectLimit(vector<%s>& vecData, uint32 start_index, uint32 data_num);\n", table.table_class_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_select_limit_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "int %s::SelectLimit(vector<%s>& vecData, uint32 start_index, uint32 data_num)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str());
	fprintf(pf, "\tstring sql_str = XStrUtil::sprintf(\"select * from %s limit %%u, %%u\", start_index, data_num);\n", table.table_name.c_str());
	fprintf(pf, "\treturn Select(vecData, sql_str);\n");
	fprintf(pf,"}\n\n");
	return true;
}


bool DBConvertCPP::make_h_select_with_keys_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.empty())
	{
		return true;
	}
	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];

	fprintf(pf, "\n\tstatic int SelectWithKeys(vector<%s>& vecData, const set<%s>& %ss);\n", table.table_class_name.c_str(), oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\n\tstatic int SelectWithKeys(vector<%s>& vecData, const vector<%s>& %ss);\n", table.table_class_name.c_str(), oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_select_with_keys_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.empty())
	{
		return true;
	}
	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];

	fprintf(pf, "int %s::SelectWithKeys(vector<%s>& vecData, const set<%s>& %ss)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str(), oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	
	fprintf(pf, "\tstring sql_str = \"select * from %s where %s in(\";\n", table.table_name.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\tset<%s>::const_iterator it = %ss.begin();\n", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\twhile (it != %ss.end())\n\t{\n", oneColumen.column_name.c_str());
	fprintf(pf, "\t\t%s %s = *it;\n\t\tit++;\n", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\t\tif (it == %ss.end())\n\t\t{\n", oneColumen.column_name.c_str());
	fprintf(pf, "\t\t\tsql_str += XStrUtil::sprintf(\" %s)\", %s);\n", oneColumen.c_format.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\t\t\tbreak;\n\t\t}\n\t\telse\n\t\t{\n");
	fprintf(pf, "\t\t\tsql_str += XStrUtil::sprintf(\" %s,\", %s);\n", oneColumen.c_format.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\t\t}\n\t}\n");
	
	fprintf(pf, "\treturn Select(vecData, sql_str);\n");
	fprintf(pf,"}\n\n");

	fprintf(pf, "int %s::SelectWithKeys(vector<%s>& vecData, const vector<%s>& %ss)\n{\n", table.table_class_name.c_str(), table.table_class_name.c_str(), oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());

	fprintf(pf, "\tstring sql_str = \"select * from %s where %s in(\";\n", table.table_name.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\tvector<%s>::const_iterator it = %ss.begin();\n", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\twhile (it != %ss.end())\n\t{\n", oneColumen.column_name.c_str());
	fprintf(pf, "\t\t%s %s = *it;\n\t\tit++;\n", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\t\tif (it == %ss.end())\n\t\t{\n", oneColumen.column_name.c_str());
	fprintf(pf, "\t\t\tsql_str += XStrUtil::sprintf(\" %s)\", %s);\n", oneColumen.c_format.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\t\t\tbreak;\n\t\t}\n\t\telse\n\t\t{\n");
	fprintf(pf, "\t\t\tsql_str += XStrUtil::sprintf(\" %s,\", %s);\n", oneColumen.c_format.c_str(), oneColumen.column_name.c_str());
	fprintf(pf, "\t\t}\n\t}\n");

	fprintf(pf, "\treturn Select(vecData, sql_str);\n");
	fprintf(pf, "}\n\n");
	return true;
}


bool DBConvertCPP::make_h_is_data_exist_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.empty())
	{
		return true;
	}
	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];

	if (oneColumen.c_member_type == "string"){
		fprintf(pf, "\n\tstatic int IsDataExist(const string& %s);\n", oneColumen.column_name.c_str());
	}
	else{
		fprintf(pf, "\n\tstatic int IsDataExist(%s %s);\n", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	}
	
	return true;
}

bool DBConvertCPP::make_cpp_is_data_exist_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.empty())
	{
		return true;
	}
	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];

	if (oneColumen.c_member_type == "string"){
		fprintf(pf, "int %s::IsDataExist(const string& %s)\n{\n", table.table_class_name.c_str(), oneColumen.column_name.c_str());
	}
	else{
		fprintf(pf, "int %s::IsDataExist(%s %s)\n{\n", table.table_class_name.c_str(), oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	}

	fprintf(pf, "\tConnPtr ptrConn = TabCommon::Connect();\n");
	fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn -1;\n\t}\n\n");

	if (oneColumen.c_member_type == "string"){
		fprintf(pf, "\tstring sql_str = XStrUtil::sprintf(\"select count(*) from %s where %s = %s;\", %s.c_str()); \n", table.table_name.c_str(), oneColumen.column_name.c_str(), oneColumen.c_format.c_str(), oneColumen.column_name.c_str());
	}
	else{
		fprintf(pf, "\tstring sql_str = XStrUtil::sprintf(\"select count(*) from %s where %s = %s;\", %s); \n", table.table_name.c_str(), oneColumen.column_name.c_str(), oneColumen.c_format.c_str(), oneColumen.column_name.c_str());
	}
	fprintf(pf, "\tRowsPtr ptrRows = ptrConn->Prepare(sql_str.c_str())\n\t\t->Query();\n");
	fprintf(pf, "\tif (NULL == ptrRows.get())\n\t{\n\t\treturn -1;\n\t}\n\n");
	fprintf(pf, "\tint nRet = 0;\n");
	fprintf(pf, "\tif (ptrRows.get() && ptrRows->Next())\n\t{\n");
	fprintf(pf, "\t\tnRet = (int) ptrRows->GetInt(0);\n\t}\n");

	fprintf(pf, "\treturn nRet;\n");
	fprintf(pf, "}\n\n");
	return true;
}


bool DBConvertCPP::make_h_get_primary_keys_with_normal_key_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.empty() || table.m_normal_key_names.empty())
	{
		return true;
	}
	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];

	for (unsigned int i = 0; i < table.m_normal_key_names.size(); ++i)
	{
		const Cache_column &oneColumen_normal = table.m_columns[table.m_normal_key_names[i]];
		string name_str = make_fun_name_use_column_name(oneColumen_normal.column_name);
		if (oneColumen_normal.c_member_type == "string"){
			fprintf(pf, "\n\tstatic bool GetPrimaryKeysWith%s(string& %s, set<%s>& %ss);\n", name_str.c_str(), oneColumen_normal.column_name.c_str(), oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
		}
		else{
			fprintf(pf, "\n\tstatic bool GetPrimaryKeysWith%s(%s %s, set<%s>& %ss);\n", name_str.c_str(), oneColumen_normal.c_member_type.c_str(), oneColumen_normal.column_name.c_str(), oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
		}
	}

	return true;
}

bool DBConvertCPP::make_cpp_get_primary_keys_with_normal_key_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.empty() || table.m_normal_key_names.empty())
	{
		return true;
	}
	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];

	for (unsigned int i = 0; i < table.m_normal_key_names.size(); ++i)
	{
		const Cache_column &oneColumen_normal = table.m_columns[table.m_normal_key_names[i]];
		string name_str = make_fun_name_use_column_name(oneColumen_normal.column_name);
		if (oneColumen_normal.c_member_type == "string"){
			fprintf(pf, "bool %s::GetPrimaryKeysWith%s(string& %s, set<%s>& %ss)\n{\n", table.table_class_name.c_str(), name_str.c_str(), oneColumen_normal.column_name.c_str(), oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
		}
		else{
			fprintf(pf, "bool %s::GetPrimaryKeysWith%s(%s %s, set<%s>& %ss)\n{\n", table.table_class_name.c_str(), name_str.c_str(), oneColumen_normal.c_member_type.c_str(), oneColumen_normal.column_name.c_str(), oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
		}

		fprintf(pf, "\tConnPtr ptrConn = TabCommon::Connect();\n");
		fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn false;\n\t}\n\n");

		if (oneColumen_normal.c_member_type == "string"){
			fprintf(pf, "\tstring sql_str = XStrUtil::sprintf(\"select %s from %s where %s = %s;\", %s.c_str()); \n", oneColumen.column_name.c_str(), table.table_name.c_str(), oneColumen_normal.column_name.c_str(), oneColumen_normal.c_format.c_str(), oneColumen_normal.column_name.c_str());
		}
		else{
			fprintf(pf, "\tstring sql_str = XStrUtil::sprintf(\"select %s from %s where %s = %s;\", %s); \n", oneColumen.column_name.c_str(), table.table_name.c_str(), oneColumen_normal.column_name.c_str(), oneColumen_normal.c_format.c_str(), oneColumen_normal.column_name.c_str());
		}
		fprintf(pf, "\tRowsPtr ptrRows = ptrConn->Prepare(sql_str.c_str())\n\t\t->Query();\n");
		fprintf(pf, "\tif (NULL == ptrRows.get())\n\t{\n\t\treturn false;\n\t}\n\n");
		fprintf(pf, "\twhile (ptrRows.get() && ptrRows->Next())\n\t{\n");
		if (oneColumen_normal.c_member_type == "string"){
			fprintf(pf, "\t\tstring %s = ptrRows->GetString(0);\n", oneColumen.column_name.c_str());
		}
		else if (oneColumen_normal.c_member_type == "double"){
			fprintf(pf, "\t\tdouble %s = ptrRows->GetFloat(0);\n", oneColumen.column_name.c_str());
		}
		else{
			fprintf(pf, "\t\t%s %s = (%s) ptrRows->GetInt(0);\n", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str(), oneColumen.c_member_type.c_str());
		}
		fprintf(pf, "\t\t%ss.insert(%s);\n\t}\n", oneColumen.column_name.c_str(), oneColumen.column_name.c_str());

		fprintf(pf, "\treturn true;\n");
		fprintf(pf, "}\n\n");
	}
	return true;
}


bool DBConvertCPP::make_h_get_primary_key_with_unique_key_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.empty() || table.m_unique_key_names.empty())
	{
		return true;
	}
	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];

	for (unsigned int i = 0; i < table.m_unique_key_names.size(); ++i)
	{
		const Cache_column &oneColumen_unique = table.m_columns[table.m_unique_key_names[i]];
		string name_str = make_fun_name_use_column_name(oneColumen_unique.column_name);
		if (oneColumen_unique.c_member_type == "string"){
			fprintf(pf, "\n\tstatic %s GetPrimaryKeyWith%s(string& %s);\n", oneColumen.c_member_type.c_str(), name_str.c_str(), oneColumen_unique.column_name.c_str());
		}
		else{
			fprintf(pf, "\n\tstatic %s GetPrimaryKeyWith%s(%s %s);\n", oneColumen.c_member_type.c_str(), name_str.c_str(), oneColumen_unique.c_member_type.c_str(), oneColumen_unique.column_name.c_str());
		}
	}

	return true;
}

bool DBConvertCPP::make_cpp_get_primary_key_with_unique_key_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.empty() || table.m_unique_key_names.empty())
	{
		return true;
	}
	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];

	for (unsigned int i = 0; i < table.m_unique_key_names.size(); ++i)
	{
		const Cache_column &oneColumen_unique = table.m_columns[table.m_unique_key_names[i]];
		string name_str = make_fun_name_use_column_name(oneColumen_unique.column_name);
		if (oneColumen_unique.c_member_type == "string"){
			fprintf(pf, "%s %s::GetPrimaryKeyWith%s(string& %s)\n{\n", oneColumen.c_member_type.c_str(), table.table_class_name.c_str(), name_str.c_str(), oneColumen_unique.column_name.c_str());
		}
		else{
			fprintf(pf, "%s %s::GetPrimaryKeyWith%s(%s %s)\n{\n", oneColumen.c_member_type.c_str(), table.table_class_name.c_str(), name_str.c_str(), oneColumen_unique.c_member_type.c_str(), oneColumen_unique.column_name.c_str());
		}

		fprintf(pf, "\tConnPtr ptrConn = TabCommon::Connect();\n");
		fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn false;\n\t}\n\n");
		if (oneColumen.c_member_type == "string"){
			fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn \"\";\n\t}\n\n");
		}
		else{
			fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn 0;\n\t}\n\n");
		}

		if (oneColumen_unique.c_member_type == "string"){
			fprintf(pf, "\tstring sql_str = XStrUtil::sprintf(\"select %s from %s where %s = %s;\", %s.c_str()); \n", oneColumen.column_name.c_str(), table.table_name.c_str(), oneColumen_unique.column_name.c_str(), oneColumen_unique.c_format.c_str(), oneColumen_unique.column_name.c_str());
		}
		else{
			fprintf(pf, "\tstring sql_str = XStrUtil::sprintf(\"select %s from %s where %s = %s;\", %s); \n", oneColumen.column_name.c_str(), table.table_name.c_str(), oneColumen_unique.column_name.c_str(), oneColumen_unique.c_format.c_str(), oneColumen_unique.column_name.c_str());
		}
		
		fprintf(pf, "\tRowsPtr ptrRows = ptrConn->Prepare(sql_str.c_str())\n\t\t->Query();\n");
		if (oneColumen.c_member_type == "string"){
			fprintf(pf, "\tif (NULL == ptrRows.get())\n\t{\n\t\treturn \"\";\n\t}\n\n");
		}
		else{
			fprintf(pf, "\tif (NULL == ptrRows.get())\n\t{\n\t\treturn 0;\n\t}\n\n");
		}

		if (oneColumen.c_member_type == "string"){
			fprintf(pf, "\tstring %s = "";\n", oneColumen.column_name.c_str());
		}
		else{
			fprintf(pf, "\t%s %s = 0;\n", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
		}
		fprintf(pf, "\tif (ptrRows.get() && ptrRows->Next())\n\t{\n");
		if (oneColumen.c_member_type == "string"){
			fprintf(pf, "\t\t%s = ptrRows->GetString(0);\n\t}\n", oneColumen.column_name.c_str());
		}
		else if (oneColumen.c_member_type == "double"){
			fprintf(pf, "\t\t%s = ptrRows->GetFloat(0);\n\t}\n", oneColumen.column_name.c_str());
		}
		else{
			fprintf(pf, "\t\t%s = (%s) ptrRows->GetInt(0);\n\t}\n", oneColumen.column_name.c_str(), oneColumen.c_member_type.c_str());
		}

		fprintf(pf, "\treturn %s;\n", oneColumen.column_name.c_str());
		fprintf(pf, "}\n\n");
	}

	return true;
}


bool DBConvertCPP::make_h_get_all_primary_keys_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.empty())
	{
		return true;
	}
	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];

	fprintf(pf, "\n\tstatic bool GetAllPrimaryKeys(vector<%s>& %ss);\n", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	return true;
}

bool DBConvertCPP::make_cpp_get_all_primary_keys_func(FILE* pf, const Cache_Table& table)
{
	if (table.table_key_index.empty())
	{
		return true;
	}
	const Cache_column &oneColumen = table.m_columns[table.table_key_index[0]];

	fprintf(pf, "bool %s::GetAllPrimaryKeys(vector<%s>& %ss)\n{\n", table.table_class_name.c_str(), oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());

	fprintf(pf, "\tConnPtr ptrConn = TabCommon::Connect();\n");
	fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn false;\n\t}\n\n");

	fprintf(pf, "\tstring sql_str = XStrUtil::sprintf(\"select %s from %s\");\n", oneColumen.column_name.c_str(), table.table_name.c_str());
	fprintf(pf, "\tRowsPtr ptrRows = ptrConn->Prepare(sql_str.c_str())\n\t\t->Query();\n");
	fprintf(pf, "\tif (NULL == ptrRows.get())\n\t{\n\t\treturn false;\n\t}\n\n");
	fprintf(pf, "\twhile (ptrRows.get() && ptrRows->Next())\n\t{\n");
	if (oneColumen.c_member_type == "string"){
		fprintf(pf, "\t\t%s %s = ptrRows->GetString(0);\n", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	}
	else if (oneColumen.c_member_type == "double"){
		fprintf(pf, "\t\t%s %s = ptrRows->GetFloat(0);\n", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str());
	}
	else{
		fprintf(pf, "\t\t%s %s = (%s) ptrRows->GetInt(0);\n", oneColumen.c_member_type.c_str(), oneColumen.column_name.c_str(), oneColumen.c_member_type.c_str());
	}
	fprintf(pf, "\t\t%ss.push_back(%s);\n\t}\n", oneColumen.column_name.c_str(), oneColumen.column_name.c_str());

	fprintf(pf, "\treturn true;\n");
	fprintf(pf, "}\n\n");
	return true;
}


bool DBConvertCPP::make_h_get_table_count_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "\n\tstatic uint32 GetTableCount();\n");
	return true;
}

bool DBConvertCPP::make_cpp_get_table_count_func(FILE* pf, const Cache_Table& table)
{
	fprintf(pf, "uint32 %s::GetTableCount()\n{\n", table.table_class_name.c_str());

	fprintf(pf, "\tConnPtr ptrConn = TabCommon::Connect();\n");
	fprintf(pf, "\tif (NULL == ptrConn.get())\n\t{\n\t\treturn 0;\n\t}\n\n");

	fprintf(pf, "\tstring sql_str = XStrUtil::sprintf(\"select count(*) from %s\");\n", table.table_name.c_str());
	fprintf(pf, "\tRowsPtr ptrRows = ptrConn->Prepare(sql_str.c_str())\n\t\t->Query();\n");
	fprintf(pf, "\tif (NULL == ptrRows.get())\n\t{\n\t\treturn 0;\n\t}\n\n");
	fprintf(pf, "\tuint32 table_count = 0;\n");
	fprintf(pf, "\tif (ptrRows.get() && ptrRows->Next())\n\t{\n");
	fprintf(pf, "\t\ttable_count = (uint32) ptrRows->GetInt(0);\n\t}\n");

	fprintf(pf, "\treturn table_count;\n");
	fprintf(pf, "}\n\n");
	return true;
}


bool DBConvertCPP::make_table_createsql(const Cache_Table& table, string& strCppFunc)
{
	string strTemp;
	int nKeyIndex = -1;
	strCppFunc =" create table IF NOT EXISTS " + table.table_name + "(";
	for (unsigned int i = 0; i < table.m_columns.size(); ++i)
	{
		strCppFunc += "`" + table.m_columns[i].column_name + "` " ;
		strCppFunc += table.m_columns[i].column_type + " ";
		strCppFunc += (table.m_columns[i].is_nullable ? "NULL " : "NOT NULL ");
		strCppFunc += table.m_columns[i].c_createsql_default;
		strCppFunc += table.m_columns[i].extra;
		strCppFunc += ",\"\n\t\t\t\t\"";
	}

	for (unsigned int i = 0; i < table.table_key_index.size(); ++i)
	{
		if (i == 0)
		{
			strCppFunc += "PRIMARY KEY(";
		}
		const Cache_column &oneColumen = table.m_columns[table.table_key_index[i]];
		strCppFunc += "`" + oneColumen.column_name;
		if (i != table.table_key_index.size() - 1)
		{
			strCppFunc += "`,";
		}
		else
		{
			strCppFunc += "`)";
		}
	}

	for (unsigned int i = 0; i < table.m_normal_key_names.size(); ++i)
	{
		const Cache_column &oneColumen_normal = table.m_columns[table.m_normal_key_names[i]];
		strCppFunc += ", KEY `" + oneColumen_normal.column_name + "`(`" + oneColumen_normal.column_name + "`)";
	}

	for (unsigned int i = 0; i < table.m_unique_key_names.size(); ++i)
	{
		const Cache_column &oneColumen_unique = table.m_columns[table.m_unique_key_names[i]];
		strCppFunc += ", UNIQUE KEY `" + oneColumen_unique.column_name + "`(`" + oneColumen_unique.column_name + "`)";
	}
	
	strCppFunc += ") ";

	if (table.auto_increment.length() > 0)
	{
		strCppFunc += "AUTO_INCREMENT=" + table.auto_increment + " ";
	}

	strCppFunc += "ENGINE=INNODB DEFAULT CHARSET=utf8" ;

	return true;
}

string DBConvertCPP::make_fun_name_use_column_name(const string& column_name)
{
	vector<string> temp_vec;
	vector<string> temp_vec_use;
	XStrUtil::split(column_name, temp_vec, "_");
	for (unsigned int i = 0; i < temp_vec.size(); ++i)
	{
		XStrUtil::to_lower(temp_vec[i]);
	}
	for (unsigned int i = 0; i < temp_vec.size(); ++i)
	{
		string temp_str = temp_vec[i].substr(0, 1);
		XStrUtil::to_upper(temp_str);
		temp_vec_use.push_back(temp_str);
		temp_str = temp_vec[i].substr(1);
		temp_vec_use.push_back(temp_str);
	}
	string str_func_name;
	for (unsigned int i = 0; i < temp_vec_use.size(); ++i)
	{
		str_func_name += temp_vec_use[i];
	}
	return str_func_name;
}
