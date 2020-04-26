#include "cache_column.h"
#include "xcore_log.h"
#include "xcore_str_util.h"
#include <iostream>


COLUMN_DATA_TYPE Cache_column::conv_db_2_cdt(const string& dbType)
{
#define MAKE_PAIR_CONV(x,y) if (0 == dbType.compare(x)) return y;

	MAKE_PAIR_CONV("int",       	CDT_INT);
	MAKE_PAIR_CONV("tinyint",   	CDT_TINYINT);
	MAKE_PAIR_CONV("varchar",   	CDT_VARCHAR);
	MAKE_PAIR_CONV("blob",      	CDT_BLOB);
	MAKE_PAIR_CONV("timestamp", 	CDT_TIMESTAMP);
	MAKE_PAIR_CONV("char",      	CDT_CHAR);
	MAKE_PAIR_CONV("double",    	CDT_DOUBLE);
	MAKE_PAIR_CONV("smallint",  	CDT_SMALLINT);
	MAKE_PAIR_CONV("text",      	CDT_TEXT);
	MAKE_PAIR_CONV("bigint",    	CDT_BIGINT);
	MAKE_PAIR_CONV("mediumblob",    CDT_MEDIUMBLOB);
	MAKE_PAIR_CONV("bit", CDT_BIT);
	MAKE_PAIR_CONV("bool", CDT_BOOL);
	MAKE_PAIR_CONV("mediumint",CDT_MEDIUMINT);
	MAKE_PAIR_CONV("float", CDT_FLOAT);
	MAKE_PAIR_CONV("date", CDT_DATE);
	MAKE_PAIR_CONV("datetime",CDT_DATATIME);
	MAKE_PAIR_CONV("time", CDT_TIME);
	MAKE_PAIR_CONV("year", CDT_YEAR);
	MAKE_PAIR_CONV("binary",CDT_BINARY);
	MAKE_PAIR_CONV("varbinary", CDT_VARBINARY);
	MAKE_PAIR_CONV("tinyblob", CDT_TINYBLOB);
	MAKE_PAIR_CONV("tinytext",CDT_TINYTEXT);
	MAKE_PAIR_CONV("mediumtext", CDT_MEDIUMTEXT);
	MAKE_PAIR_CONV("longblob",CDT_LONGBLOB);
	MAKE_PAIR_CONV("longtext", CDT_LONGTEXT);
	MAKE_PAIR_CONV("enum", CDT_ENUM);
	MAKE_PAIR_CONV("set",CDT_SET);
	XWARNING("[fix] dbtype = %s, is not in convert table;", dbType.c_str());
	return CDT_UNKNOW;
}

bool Cache_column::check_init()
{
#define CHECK_INT(x)    if (x == 0) { printf("%s is 0", #x); VERIFY(false); break;};
#define CHECK_STRING(x) if (x.length() == 0 ){ printf("%s is null", #x); VERIFY(false); break;};

	do 
	{	
		CHECK_INT(ordinal_position);

		CHECK_STRING(table_schema);
		CHECK_STRING(table_name);
		CHECK_STRING(column_name);
		CHECK_STRING(o_date_type);
		CHECK_STRING(column_type);
		
		return prepare_c_conv();
	} while (false);
	return false;
}

bool Cache_column::prepare_c_conv()
{
	bool bret = prepare_c_name();
	VERIFY(bret);
	bret = prepare_c_type();
	VERIFY(bret);
	bret = prepare_c_default();
	VERIFY(bret);
	bret = prepare_c_select_data_getvalue();
	VERIFY(bret);
	bret = prepare_c_use_date_value();
	VERIFY(bret);
	bret = prepare_c_use_vec_getvalue();
	VERIFY(bret);
	return true;
}

bool Cache_column::prepare_c_name()
{
	c_member_name = "m_" + TrimPrefix(column_name);
	return true;
}

bool Cache_column::prepare_c_type()
{
	data_type     = conv_db_2_cdt(o_date_type);
	if (data_type == CDT_UNKNOW)
		return "";

	switch(data_type)
	{
	case CDT_TINYINT:
	case CDT_INT:
	case CDT_SMALLINT:
		{
			//if (column_type.find("unsigned") != string::npos)
			//{
				c_member_type = "uint32";
				c_format = "%u";
				return true;
			//}
			//else
			//{
			//	c_member_type = "int32";
			//	c_format = "%d";
			//	return true;
			//}
		}
		break;
	case CDT_BIGINT:
		{
			//if (column_type.find("unsigned") != string::npos)
			//{
				c_member_type = "uint64";
				c_format = "%llu";
				return true;
			//}
			//else
			//{
			//	c_member_type = "int64";
			//	c_format = "%lld";
			//	return true;
			//}
		}
	case CDT_VARCHAR:
	case CDT_BLOB:
	case CDT_CHAR:
	case CDT_TEXT:
	case CDT_TIMESTAMP:
	case CDT_MEDIUMBLOB:
	case CDT_LONGBLOB:
	case CDT_BINARY:
	case CDT_VARBINARY:
		{
			c_member_type = "string";
			c_format = "%s";
			return true;
		}
		break;
	case CDT_DOUBLE:
		{
			c_member_type = "double";
			c_format = "%f";
			return true;
		}
	default:
		std::cout<< " the case the member type is "<< data_type <<" , not handle this type!!!!"<<std::endl;
		return false;
	}
	return false;

	return (data_type != CDT_UNKNOW);
}

//初始化c_createsql_default 和 c_construct_default 字段
//要求 在此之前必须已经调用过prepare_C_Name() 和 prepare_C_Type()
bool Cache_column::prepare_c_default()
{
	if ( column_default.length() == 0)
	{
		c_createsql_default = " ";
		c_construct_default = "";
		return true;
	}

	c_createsql_default = " ";
	c_construct_default = "";

	if (data_type == CDT_UNKNOW)
		return false;

	switch(data_type)
	{
	case CDT_BIGINT:
	case CDT_TINYINT:
	case CDT_INT:
	case CDT_SMALLINT:
	case CDT_DOUBLE:
		{		
			if (column_default.length() > 0)
				c_construct_default = c_member_name + "(" + column_default + ")";
			else
				c_construct_default = c_member_name + "(0)";
		
			c_createsql_default = "DEFAULT " + column_default + " ";
		}
		break;
	case CDT_VARCHAR:
	case CDT_BLOB:
	case CDT_CHAR:
	case CDT_TEXT:
	case CDT_MEDIUMBLOB:
		{
			if (column_default.length() > 0)
				c_construct_default = c_member_name + "(\"" + column_default + "\")";
			else
				c_construct_default = c_member_name + "(\"\")";
			
			c_createsql_default = "DEFAULT '" + column_default + "' ";
		}
		break;
	case CDT_TIMESTAMP:
		{	
			if (column_default == "CURRENT_TIMESTAMP")
				c_construct_default = c_member_name + "(XTime().to_str())";
			else
				c_construct_default = c_member_name + "(\"" + column_default + "\")";

			if (column_default == "CURRENT_TIMESTAMP")
				c_createsql_default = "DEFAULT " + column_default + " ";
			else
				c_createsql_default = "DEFAULT '" + column_default + "' ";
		}
		break;
	default:
		return false;
	}
	return true;
}

bool Cache_column::prepare_c_select_data_getvalue()
{
	if (data_type == CDT_UNKNOW)
		return false;
	
	switch(data_type)
	{
	case CDT_BLOB:
	case CDT_MEDIUMBLOB:
		c_select_one_getvalu = "Data." + c_member_name + " = ptrRows->GetBlob(index++);";
		break;
	case CDT_BIGINT:
	case CDT_TINYINT:
	case CDT_INT:
	case CDT_SMALLINT:
		{
			c_select_one_getvalu  = "Data." + c_member_name + " = (" + c_member_type + ") ptrRows->GetInt(index++);";
		}
		break;
	case CDT_VARCHAR:	
	case CDT_CHAR:
	case CDT_TEXT:		
	case CDT_TIMESTAMP:	
		c_select_one_getvalu = "Data." + c_member_name + " = ptrRows->GetString(index++);";
		break;
	case CDT_DOUBLE:
		c_select_one_getvalu = "Data." + c_member_name + " = ptrRows->GetFloat(index++);";
		break;
	default:
		return false;
	}
	return true;
}

bool Cache_column::prepare_c_use_date_value()
{
	if (data_type == CDT_UNKNOW)
		return false;
	
	switch(data_type)
	{
	case CDT_BLOB:
	case CDT_MEDIUMBLOB:
		c_use_data_value = "Value(Data." + c_member_name + ".c_str(), Data." + c_member_name + ".length())";
		break;
	case CDT_BIGINT:
	case CDT_TINYINT:
	case CDT_INT:
	case CDT_SMALLINT:		
	case CDT_VARCHAR:	
	case CDT_CHAR:
	case CDT_TEXT:
	case CDT_TIMESTAMP:		
	case CDT_DOUBLE:
		c_use_data_value = "Data." + c_member_name;
		break;
	default:
		return false;
	}
	return true;
}


bool Cache_column::prepare_c_use_vec_getvalue()
{
	if (data_type == CDT_UNKNOW)
		return false;
	switch(data_type)
	{
	case CDT_BLOB:
	case CDT_MEDIUMBLOB:
		c_use_vec_value = "Value(vecData[i]." + c_member_name + ".c_str(), vecData[i]." + c_member_name + ".length())";
		break;
	case CDT_BIGINT:
	case CDT_TINYINT:
	case CDT_INT:
	case CDT_SMALLINT:		
	case CDT_VARCHAR:	
	case CDT_CHAR:
	case CDT_TEXT:
	case CDT_TIMESTAMP:		
	case CDT_DOUBLE:
		c_use_vec_value = "vecData[i]." + c_member_name;
		break;
	default:
		return false;
	}
	return true;
}

