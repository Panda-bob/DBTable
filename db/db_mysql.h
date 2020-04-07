// 2013-09-09
// db_mysql.h
// guosh
// mysql数据库操作简要封装

#ifndef _DB_MYSQL_H_
#define _DB_MYSQL_H_

#include "xcore_define.h"
#include "xcore_time.h"
#include "xcore_event.h"
#include "xcore_critical.h"
#include "xcore_memory.h"
#include "mysql.h"

namespace mysql {

class Value;
class Result;
class Rows;
class Statement;
class Conn;
class Database;

////////////////////////////////////////////////////////////////////////////////
// 支持的数据类型包括: null, bool, int64, double, string, binary, time
// class Value
////////////////////////////////////////////////////////////////////////////////
class Value
{
	friend class Statement;
	union Data{
		bool   m_bool;
		int64  m_int; // int
		double m_float; // float, double
		time_t m_time; // time
		void*  m_ptr; // string, binary
	}		m_data;
	int		m_type; // 0:null, 1:bool, 2:int64, 3:double, 4:string, 5:binary, 6:time
	size_t	m_len;

public:
	Value();
	Value(bool b);
	Value(int32 v);
	Value(uint32 v);
	Value(int64 v);
	Value(float v);
	Value(double v);
	Value(const string& s);
	Value(const char* s);
	Value(const void* ptr, size_t len);
	Value(const XTime& t);
	Value(const Value& v);
	Value& operator=(const Value& v);
	~Value();
};

////////////////////////////////////////////////////////////////////////////////
// class Result
////////////////////////////////////////////////////////////////////////////////
class Result
{
	friend class Statement;
	int64 m_affected_rows;
	int64 m_last_insertid;

public:
	int64 LastInsertId() { return m_last_insertid; }
	int64 RowsAffected() { return m_affected_rows; }
};

////////////////////////////////////////////////////////////////////////////////
// class Rows
////////////////////////////////////////////////////////////////////////////////
class Rows
{
	friend class Statement;
	MYSQL_RES* m_res;
	uint32 m_count_rows;
	uint32 m_count_columns;
	vector<string> m_columns;
	MYSQL_ROW m_row;
	unsigned long* m_lengths;

public:
	~Rows();
	uint32 CountRows() { return m_count_rows; }
	uint32 CountColumns() { return (uint32)m_count_columns; }
	const vector<string>& Columns() { return m_columns; }

	bool   Next();

	// 根据索引取值
	bool   GetBool(uint32 idx);
	int64  GetInt(uint32 idx);
	double GetFloat(uint32 idx);
	string GetString(uint32 idx);
	string GetGBKString(uint32 idx);
	XTime  GetTime(uint32 idx);
	string GetBlob(uint32 idx); // 二进制数据以string对象返回

	// 根据结果字段名取值(效率稍低,慎用)
	bool   GetBool(const string& key);
	int64  GetInt(const string& key);
	double GetFloat(const string& key);
	string GetString(const string& key);
	string GetGBKString(const string& key);
	XTime  GetTime(const string& key);
	string GetBlob(const string& key); // 二进制数据以string对象返回
};

////////////////////////////////////////////////////////////////////////////////
// class Statement
////////////////////////////////////////////////////////////////////////////////
class Statement
{
	friend class Conn;
	shared_ptr<Conn> m_ptrConn;
	map<string, Value> m_parameters;
	string m_sql;

	shared_ptr<IXBuffer> make();

public:
	// key必须以'@'开头
	Statement* SetParameter(const string& key, const Value& value);

	shared_ptr<Result> Exec();
	shared_ptr<Rows> Query();
};

////////////////////////////////////////////////////////////////////////////////
// class Conn
////////////////////////////////////////////////////////////////////////////////
class Conn : public enable_shared_from_this<Conn>
{
	friend class Statement;
	friend class Database;
	MYSQL*		m_mysql;
	string		m_host;
	uint16		m_port;
	string		m_scheme;
	string		m_user;
	string		m_passwd;
	
public:
	~Conn();

	// 发起一个SQL操作
	shared_ptr<Statement> Prepare(const string& sql);

	// 开始事务(对ISAM和MyISAM数据库无效)
	bool Begin();

	// 提交事务(对ISAM和MyISAM数据库无效)
	bool Commit();

	// 回滚事务(对ISAM和MyISAM数据库无效)
	bool Rollback();
	
private:
	void last_error(uint32& no, string& error);
	bool do_connect();
	bool do_query(const string& sql);
	bool do_query(const char* sql, uint32 len);
	void do_close();
	bool set_character_set();
	bool get_variable(const string& var, string& value);
	bool get_tables(vector<string>& vTables);
	bool select_scheme(const string& scheme);
	bool check_lower_case_table_names();
};


////////////////////////////////////////////////////////////////////////////////
// class Database
////////////////////////////////////////////////////////////////////////////////
class Database
{
	string		m_host;
	uint16		m_port;
	string		m_scheme;
	string		m_user;
	string		m_passwd;
	uint32		m_maxIdleConns;
	uint32		m_maxConns;
	bool		m_isClosed;
	XEvent		m_event;
	XCritical	m_lock;
	set<Conn*>  m_useConns;
	list<shared_ptr<Conn> > m_freeConns;

public:
	static Database* Instance();

	bool Open(const string& host, uint16 port, const string& scheme, const string& user, const string& passwd);

	void Close();

	vector<string> Tables();

	shared_ptr<Conn> Connect(int timeout = -1/*毫秒*/);

	uint32 GetMaxIdleConn();
	void SetMaxIdleConn(uint32 maxIdleConn);

	uint32 GetMaxConn();
	void SetMaxConn(uint32 maxConn);

private:
	Database();
	~Database();
	shared_ptr<Conn> new_conn();
	bool put_conn(Conn* conn);

	friend class Conn_deleter;
	struct Conn_deleter
	{
		void operator()(Conn* conn) const;
	};
};

void test_mymysql();

}//namespace mysql

using namespace mysql;

#endif//_DB_MYSQL_H_

