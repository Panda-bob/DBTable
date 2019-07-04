#ifndef _DB_TOOL_CACHE_COLUMN_H_
#define _DB_TOOL_CACHE_COLUMN_H_

#include "define.h"
#include "xcore_define.h"

class Cache_column
{
public:
	string           table_schema; //���ֶ������ĸ���
	string           table_name; //���ֶ������ĸ���
	string           column_name;//���ݿ��еı�����
	uint32           ordinal_position; //Ĭ��˳��
	string           column_default;
	bool             is_nullable;	
	string           o_date_type;
	uint32           character_maximum_length;//��󳤶�
	uint32           character_octet_length;//��λ����
	uint32           numeric_precision;//���־���
	uint32           numeric_scale;
	string           character_set_name;//�ַ������ַ������ֶλ��õ���
	string           collation_name;//�ַ�������
	string           column_type;//�ֶ����͵ľ���������
	string           column_key;//�󶨼�ֵ(������Ψһ������)
	string           extra;//�����ֽڣ���������
	string           privileges;//֧�ֵĲ���
	string           column_comment;//�ֶ�ע��

public:
	bool check_init();

	
private:
	bool prepare_c_conv();//�������ݻ����ṹ��׼������C������Ҫ�Ļ����ṹ
	bool prepare_c_name();
	bool prepare_c_type();
	bool prepare_c_default();
	bool prepare_c_select_data_getvalue();
	bool prepare_c_use_date_value();
	bool prepare_c_use_vec_getvalue();

	COLUMN_DATA_TYPE conv_db_2_cdt(const string& dbType);
	
	

public:
	COLUMN_DATA_TYPE data_type;

	string c_member_name;// C++������
	string c_member_type;// C++������
	string c_format;//��ʽ���ַ����е�ռλ��
	string c_construct_default;//���캯���еĳ�ʼ������( m_aa(0) )
	string c_createsql_default;//��������е�Ĭ��ֵ����

	string c_select_one_getvalu;//select����У���ȡ���ֶε�ֵ�����
	string c_use_vec_value;//insert����ַ���������У����ֶε�Valueֵ��ʾ
	string c_use_data_value;//ת����Value�ṹ
	
};



#endif//_DB_TOOL_CACHE_COLUMN_H_