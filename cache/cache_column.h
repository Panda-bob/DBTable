#ifndef _DB_TOOL_CACHE_COLUMN_H_
#define _DB_TOOL_CACHE_COLUMN_H_

#include "define.h"
#include "xcore_define.h"

class Cache_column
{
public:
	string           table_schema; //该字段属于哪个库
	string           table_name; //该字段属于哪个表
	string           column_name;//数据库中的变量名
	uint32           ordinal_position; //默认顺序
	string           column_default;
	bool             is_nullable;	
	string           o_date_type;
	uint32           character_maximum_length;//最大长度
	uint32           character_octet_length;//八位长度
	uint32           numeric_precision;//数字精度
	uint32           numeric_scale;
	string           character_set_name;//字符集（字符串类字段会用到）
	string           collation_name;//字符集名称
	string           column_type;//字段类型的具体描述，
	string           column_key;//绑定键值(主键，唯一索引等)
	string           extra;//额外字节，自增长等
	string           privileges;//支持的操作
	string           column_comment;//字段注释

public:
	bool check_init();

	
private:
	bool prepare_c_conv();//根据数据基础结构，准备生成C编码需要的基础结构
	bool prepare_c_name();
	bool prepare_c_type();
	bool prepare_c_default();
	bool prepare_c_select_data_getvalue();
	bool prepare_c_use_date_value();
	bool prepare_c_use_vec_getvalue();

	COLUMN_DATA_TYPE conv_db_2_cdt(const string& dbType);
	
	

public:
	COLUMN_DATA_TYPE data_type;

	string c_member_name;// C++变量名
	string c_member_type;// C++类型名
	string c_format;//格式化字符串中的占位符
	string c_construct_default;//构造函数中的初始化部分( m_aa(0) )
	string c_createsql_default;//建表语句中的默认值部分

	string c_select_one_getvalu;//select语句中，获取该字段的值的语句
	string c_use_vec_value;//insert多个字符串的语句中，该字段的Value值表示
	string c_use_data_value;//转换成Value结构
	
};



#endif//_DB_TOOL_CACHE_COLUMN_H_