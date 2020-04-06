
#include "db_mysql.h"
#include "cache_table.h"
#include "xcore_time.h"
#include "xcore_str_util.h"
#include "math.h"
#include "db_helper.h"
//#include "Table_activitys.h"
//mysql 查询库中表的信息
//SELECT * from INFORMATION_SCHEMA.TABLES  where TABLE_SCHEMA = 'honour';

//mysql 查询表中的列信息
//select * from INFORMATION_SCHEMA.COLUMNS where TABLE_SCHEMA = 'honour' and TABLE_NAME = 'role_info_blob';

string table_to_class_name(string strTable)
{	
	if (strTable.length() == 0)
		return strTable;

	strTable[0] = toupper(strTable[0]);

	for (unsigned int i = 1; i < strTable.length(); ++i)
	{	
		if (strTable[i] == '_' && (i + 1) < strTable.length())
		{
			strTable[i + 1] = toupper(strTable[i + 1]);
			strTable.erase(strTable.begin() + i);
		}		
	}
	return strTable;
	
}

bool ReadTableStruct(Cache_schema& schema, 
					 const string& host, 
					 const string& port,
					 const string& user,
					 const string& pwd,
					 const string& schema_name)
{
	if(!Database::Instance()->Open(host, XStrUtil::to_int_def(port, 3306), schema_name, user, pwd))
	{
		printf("db open error");
		return false;
	}
	shared_ptr<Conn> ptrConn = Database::Instance()->Connect();
	if (!ptrConn.get())
	{
		printf("get conn error");
		return false;
	}
	shared_ptr<Rows> ptrRows =
		ptrConn->Prepare("SELECT * from INFORMATION_SCHEMA.TABLES  where TABLE_SCHEMA = @schema")
		->SetParameter("@schema", schema_name)
		->Query();

	VERIFY(ptrRows.get());

	schema.schema_name = schema_name;
	while(ptrRows.get() && ptrRows->Next())
	{
		Cache_Table table;
		table.table_name       = ptrRows->GetGBKString("TABLE_NAME");
		table.table_class_name = table_to_class_name(table.table_name);
		table.table_createtime = ptrRows->GetTime("CREATE_TIME").to_str();
		table.engine           = ptrRows->GetGBKString("ENGINE");
		table.auto_increment   = ptrRows->GetGBKString("AUTO_INCREMENT");
		table.table_comment    = ptrRows->GetGBKString("TABLE_COMMENT");
		schema.m_tables.push_back(table);
	}

	for (uint32 i = 0; i < schema.m_tables.size(); ++i)
	{
		shared_ptr<Rows> ptrRows =
			ptrConn->Prepare("SELECT * from INFORMATION_SCHEMA.COLUMNS where TABLE_SCHEMA = @schema and TABLE_NAME = @tablename")
			->SetParameter("@schema", schema_name)
			->SetParameter("@tablename", schema.m_tables[i].table_name)
			->Query();

		VERIFY(ptrRows.get());

		while(ptrRows.get() && ptrRows->Next())
		{
			Cache_column column;
			column.table_schema             = schema_name;
			column.table_name               = schema.m_tables[i].table_name;
			column.column_name              = ptrRows->GetGBKString("COLUMN_NAME");
			column.ordinal_position         = (uint32)ptrRows->GetInt("ORDINAL_POSITION");
			column.column_default           = ptrRows->GetGBKString("COLUMN_DEFAULT");
			column.is_nullable              = (ptrRows->GetGBKString("IS_NULLABLE") == "YSE");
			column.o_date_type              = ptrRows->GetGBKString("DATA_TYPE");
			column.character_maximum_length = (uint32)ptrRows->GetInt("CHARACTER_MAXIMUM_LENGTH");
			column.character_octet_length   = (uint32)ptrRows->GetInt("CHARACTER_OCTET_LENGTH");			
			column.numeric_precision        = (uint32)ptrRows->GetInt("NUMERIC_PRECISION");
			column.numeric_scale            = (uint32)ptrRows->GetInt("NUMERIC_SCALE");
			column.character_set_name       = ptrRows->GetGBKString("CHARACTER_SET_NAME");
			column.collation_name           = ptrRows->GetGBKString("COLLATION_NAME");
			column.column_type              = ptrRows->GetGBKString("COLUMN_TYPE");
			column.column_key               = ptrRows->GetGBKString("COLUMN_KEY");
			column.extra                    = ptrRows->GetGBKString("EXTRA");
			column.privileges               = ptrRows->GetGBKString("PRIVILEGES");
			column.column_comment           = ptrRows->GetGBKString("COLUMN_COMMENT");

			VERIFY(column.check_init());
			schema.m_tables[i].m_columns.push_back(column);
			if (column.column_key.find("PRI") != string::npos)
			{
				schema.m_tables[i].table_key_index.push_back(schema.m_tables[i].m_columns.size() - 1);
			}
			else if (column.column_key.find("UNI") != string::npos)
			{
				schema.m_tables[i].m_unique_key_names.push_back(schema.m_tables[i].m_columns.size() - 1);
			}
			else if (column.column_key.find("MUL") != string::npos)
			{
				schema.m_tables[i].m_normal_key_names.push_back(schema.m_tables[i].m_columns.size() - 1);
			}
		}
	}
	return true;
}

const int FLAG_DB_MAKE_CPP         = 0x1;
const int FLAG_DB_MAKE_PROTOCAL    = 0x2;
const int FLAG_DB_MAKE_RECORD_FUNC = 0x4;

int main(int argc, char** argv)
{
	int MAKE_FLAG = 0x0;

	string host, port, schema_name, user, pwd, dir;

	if (argc != 7 && argc != 8)
	{
		printf("please check the agrc count, must more then 7!");
		getchar();
		return 1;
	}

	if (*argv[1] == '-'){
		int nlen = strlen(argv[1]);
		char* pFlag = argv[1];
		while (((--nlen) > 0) && ++pFlag){
			if (*pFlag == 'n'){
				MAKE_FLAG |= FLAG_DB_MAKE_CPP;
			}
			if (*pFlag == 'p'){
				MAKE_FLAG |= FLAG_DB_MAKE_PROTOCAL;
			}
			if (*pFlag == 'r'){
				MAKE_FLAG |= FLAG_DB_MAKE_RECORD_FUNC;
			}
		}
	}
	else{
		MAKE_FLAG = FLAG_DB_MAKE_CPP;
	}

	if (argc == 7){
		host = argv[1];
		port = argv[2];
		schema_name = argv[3];
		user = argv[4];
		pwd = argv[5];
		dir = argv[6];
	}

	if (argc == 8){
		host = argv[2];
		port = argv[3];
		schema_name = argv[4];
		user = argv[5];
		pwd = argv[6];
		dir = argv[7];
	}

	Cache_schema schema;
	if (!ReadTableStruct(schema, host, port, user, pwd, schema_name))
	{
		printf("ReadTableStruct failed");
		getchar();
		return 1;
	}
	
	if (MAKE_FLAG & FLAG_DB_MAKE_CPP)
	{
		printf("now make tab*.h tab*.cpp\n");
		for (uint32 i = 0; i < schema.m_tables.size(); ++i)
		{
			if(!DBConvertCPP::make(dir, schema.m_tables[i]))
				printf("make %s failed\n", schema.m_tables[i].table_name.c_str());
			else
				printf("make %s OK!\n", schema.m_tables[i].table_name.c_str());
		}
	}

	if (MAKE_FLAG & FLAG_DB_MAKE_PROTOCAL)
	{
		printf("now make protocol");
		if(!DBConvertProtocol::make(dir, schema))
			printf("make protocol %s failed\n", schema.schema_name.c_str());
		else
			printf("make protocol %s OK!\n", schema.schema_name.c_str());
	}

	if (MAKE_FLAG & FLAG_DB_MAKE_RECORD_FUNC)
	{
		printf("now make func");
		if(!DBConvertRecordFunc::make(dir, schema))
			printf("make func %s failed\n", schema.schema_name.c_str());
		else
			printf("make func %s OK!\n", schema.schema_name.c_str());
	}

	printf("make ok!");
	getchar();
	
	return 0;
}