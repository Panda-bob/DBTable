#ifndef _DB_TOOL_CACHE_DB_HELPER_H_
#define _DB_TOOL_CACHE_DB_HELPER_H_

#include "cache_column.h"
#include "cache_table.h"


class DBConvertProtocol
{
public:
	
	static bool make(const string& dirPath, const Cache_schema& schema);
	static bool make_protocal(FILE* pf, const Cache_schema& schema);
};

class DBConvertRecordFunc
{
public:
	static bool make(const string& dirPath, const Cache_schema& schema);
	static bool make_h_func(FILE* pf, const Cache_schema& schema);
	static bool make_cpp_func(FILE* pf, const Cache_schema& schema);
	
	static bool make_h_table_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_table_func(FILE* pf, const Cache_Table& table);

};


class DBConvertCPP
{
public:
	static bool make(const string& dirPath, const Cache_Table& table);
	static bool make_table_createsql(const Cache_Table& table, string& strCppFunc);

	static bool make_h_file(FILE* pf, const Cache_Table& table);
	static bool make_h_create_table_sql(FILE* pf, const Cache_Table& table);
	static bool make_h_create_table(FILE* pf);
	static bool make_h_construct(FILE* pf, const Cache_Table& table);

	static bool make_h_insert_func(FILE* pf, const Cache_Table& table);
	static bool make_h_insert_autokey_func(FILE* pf, const Cache_Table& table);
	static bool make_h_insert_autokey_one_func(FILE* pf, const Cache_Table& table);
	static bool make_h_insert_one_func(FILE* pf, const Cache_Table& table);

	static bool make_h_replace(FILE* pf, const Cache_Table& table);

	static bool make_h_update_func(FILE* pf, const Cache_Table& table);
	static bool make_h_update_ex_func(FILE* pf, const Cache_Table& table);

	static bool make_h_delete_func(FILE* pf, const Cache_Table& table);
	static bool make_h_delete_all_func(FILE* pf, const Cache_Table& table);
	static bool make_h_delete_with_keys_func(FILE* pf, const Cache_Table& table);

	static bool make_h_select_ex_func(FILE* pf, const Cache_Table& table);
	static bool make_h_select_all_func(FILE* pf, const Cache_Table& table);
	static bool make_h_select_func(FILE* pf, const Cache_Table& table);	
	static bool make_h_select_limit_func(FILE* pf, const Cache_Table& table);
	static bool make_h_select_with_keys_func(FILE* pf, const Cache_Table& table);
	static bool make_h_select_private_func(FILE* pf, const Cache_Table& table);

	static bool make_h_is_data_exist_func(FILE* pf, const Cache_Table& table);
	static bool make_h_get_primary_keys_with_normal_key_func(FILE* pf, const Cache_Table& table);
	static bool make_h_get_primary_key_with_unique_key_func(FILE* pf, const Cache_Table& table);
	static bool make_h_get_all_primary_keys_func(FILE* pf, const Cache_Table& table);
	static bool make_h_get_table_count_func(FILE* pf, const Cache_Table& table);

	static bool make_cpp_file(FILE* pf, const Cache_Table& table);
	static bool make_cpp_create_table_sql(FILE* pf, const Cache_Table& table);
	static bool make_cpp_create_table(FILE* pf, const Cache_Table& table);
	static bool make_cpp_construct(FILE* pf, const Cache_Table& table);	

	static bool make_cpp_insert_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_insert_autokey_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_insert_autokey_one_func(FILE* pf, const Cache_Table& table);	
	static bool make_cpp_insert_one_func(FILE* pf, const Cache_Table& table);
	
	static bool make_cpp_replace(FILE* pf, const Cache_Table& table);
	
	static bool make_cpp_update_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_update_ex_func(FILE* pf, const Cache_Table& table);

	static bool make_cpp_delete_all_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_delete_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_delete_with_keys_func(FILE* pf, const Cache_Table& table);

	static bool make_cpp_select_all_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_select_func(FILE* pf, const Cache_Table& table);	
	static bool make_cpp_select_ex_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_select_limit_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_select_with_keys_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_select_private_func(FILE* pf, const Cache_Table& table);	

	static bool make_cpp_is_data_exist_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_get_primary_keys_with_normal_key_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_get_primary_key_with_unique_key_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_get_all_primary_keys_func(FILE* pf, const Cache_Table& table);
	static bool make_cpp_get_table_count_func(FILE* pf, const Cache_Table& table);

private:
	static string make_fun_name_use_column_name(const string& column_name);
};

#endif//_DB_TOOL_CACHE_DB_HELPER_H_