// 2013-09-09
// db_mysql.cpp
// guosh
// mysql数据库操作简要封装

#include "db_mysql.h"
#include "xcore_str_util.h"
#include "xcore_parser.h"
#include "xcore_conver.h"
#include "xcore_log.h"
#include "xcore_thread.h"
#include "mysqld_error.h"
#include "errmsg.h"

namespace mysql {

////////////////////////////////////////////////////////////////////////////////
// 支持的数据类型包括: null, bool, int64, double, string, binary, time
// class Value
////////////////////////////////////////////////////////////////////////////////
Value::Value()
	: m_type(0)
	, m_len(0)
{
	// empty
}

Value::Value(bool b)
	: m_type(1)
	, m_len(sizeof(bool))
{
	m_data.m_bool = b;
}

Value::Value(int32 v)
	: m_type(2)
	, m_len(sizeof(int64))
{
	m_data.m_int = v;
}

Value::Value(uint32 v)
	: m_type(2)
	, m_len(sizeof(int64))
{
	m_data.m_int = v;
}

Value::Value(int64 v)
	: m_type(2)
	, m_len(sizeof(int64))
{
	m_data.m_int = v;
}

Value::Value(float v)
	: m_type(3)
	, m_len(sizeof(double))
{
	m_data.m_float = v;
}

Value::Value(double v)
	: m_type(3)
	, m_len(sizeof(double))
{
	m_data.m_float = v;
}

Value::Value(const string& s)
	: m_type(4)
	, m_len(s.size())
{
	m_data.m_ptr = malloc(m_len + 1);
	memcpy(m_data.m_ptr, s.c_str(), m_len + 1);
}

Value::Value(const char* s)
	: m_type(4)
	, m_len(0)
{
	ASSERT(s);
	m_len = strlen(s);
	m_data.m_ptr = malloc(m_len + 1);
	memcpy(m_data.m_ptr, s, m_len + 1);
}

Value::Value(const void* ptr, size_t len)
	: m_type(5)
	, m_len(len)
{
	ASSERT(ptr);
	ASSERT(len >= 0);
	m_data.m_ptr = malloc(m_len + 1);
	memcpy(m_data.m_ptr, ptr, m_len);
}

Value::Value(const XTime& t)
	: m_type(6)
	, m_len(sizeof(time_t))
{
	m_data.m_time = t.sec();
}

Value::Value(const Value& v)
	: m_type(v.m_type)
	, m_len(v.m_len)
{
	switch (m_type)
	{
	case 1:
		m_data.m_bool = v.m_data.m_bool;
		break;
	case 2:
		m_data.m_int = v.m_data.m_int;
		break;
	case 3:
		m_data.m_float = v.m_data.m_float;
		break;
	case 4:
		m_data.m_ptr = malloc(m_len + 1);
		memcpy(m_data.m_ptr, v.m_data.m_ptr, m_len + 1);
		break;
	case 5:
		m_data.m_ptr = malloc(m_len + 1);
		memcpy(m_data.m_ptr, v.m_data.m_ptr, m_len);
		break;
	case 6:
		m_data.m_time = v.m_data.m_time;
		break;
	case 0:
	default:
		m_type = 0;
		m_len = 0;
		break;
	}
}

Value& Value::operator=(const Value& v)
{
	if (this == &v) return *this;

	if (m_type == 4 || m_type == 5)
	{
		free(m_data.m_ptr);
	}
	m_type = v.m_type;
	m_len = v.m_len;

	switch (m_type)
	{
	case 1:
		m_data.m_bool = v.m_data.m_bool;
		break;
	case 2:
		m_data.m_int = v.m_data.m_int;
		break;
	case 3:
		m_data.m_float = v.m_data.m_float;
		break;
	case 4:
		m_data.m_ptr = malloc(m_len + 1);
		memcpy(m_data.m_ptr, v.m_data.m_ptr, m_len + 1);
		break;
	case 5:
		m_data.m_ptr = malloc(m_len + 1);
		memcpy(m_data.m_ptr, v.m_data.m_ptr, m_len);
		break;
	case 6:
		m_data.m_time = v.m_data.m_time;
		break;
	case 0:
	default:
		m_type = 0;
		m_len = 0;
		break;
	}
	return *this;
}

Value::~Value()
{
	if (m_type == 4 || m_type == 5)
	{
		free(m_data.m_ptr);
	}
	m_type = 0;
	m_len = 0;
}


////////////////////////////////////////////////////////////////////////////////
// class Rows
////////////////////////////////////////////////////////////////////////////////
Rows::~Rows()
{
	if (m_res)
	{
		mysql_free_result(m_res);
		m_res = NULL;
	}
}

bool Rows::Next()
{
	if (m_res == NULL) return false;
	m_row = mysql_fetch_row(m_res);
	if (m_row == NULL) return false;
	m_lengths = mysql_fetch_lengths(m_res);
	ASSERT(m_lengths);
	if (m_lengths == NULL) return false;
	return true;
}

bool Rows::GetBool(uint32 idx)
{
	ASSERT(m_row);
	ASSERT(idx < m_count_columns);
	if (m_row[idx] == NULL) return false;
	return string(m_row[idx]) != "0";
}

int64 Rows::GetInt(uint32 idx)
{
	ASSERT(m_row);
	ASSERT(idx < m_count_columns);
	if (m_row[idx] == NULL) return 0;
	XStrParser parser(m_row[idx], strlen(m_row[idx]));
	int64 v = parser.get_integer();
	if (parser.position() == parser.start() || !parser.eof())
	{
		XWARNING("Rows::get_int(%u), Can't covert '%s' to int64.", idx, m_row[idx]);
	}
	return v;
}

double Rows::GetFloat(uint32 idx)
{
	ASSERT(m_row);
	ASSERT(idx < m_count_columns);
	if (m_row[idx] == NULL) return 0;
	double v = 0.0;
	if (!XStrUtil::to_float(m_row[idx], v))
	{
		XWARNING("Rows::get_float(%u), Can't covert '%s' to double.", idx, m_row[idx]);
	}
	return v;
}

string Rows::GetString(uint32 idx)
{
	ASSERT(m_row);
	ASSERT(idx < m_count_columns);
	if (m_row[idx] == NULL) return "";
	return m_row[idx];
}

string Rows::GetGBKString(uint32 idx)
{
	ASSERT(m_row);
	ASSERT(idx < m_count_columns);
	if (m_row[idx] == NULL) return "";
	string strTemp;
	utf8_to_gbk(strTemp, m_row[idx]);
	return strTemp;
}

XTime Rows::GetTime(uint32 idx)
{
	ASSERT(m_row);
	ASSERT(idx < m_count_columns);
	if (m_row[idx] == NULL) return XTime::ZeroTime;
	return XTime::try_parse(m_row[idx]);
}

string Rows::GetBlob(uint32 idx)
{
	ASSERT(m_row);
	ASSERT(idx < m_count_columns);
	if (m_row[idx] == NULL) return "";
	return string(m_row[idx], m_lengths[idx]);
}

bool Rows::GetBool(const string& key)
{
	for (int idx = 0; idx < (int)m_columns.size(); idx++)
	{
		if (XStrUtil::compare(key, m_columns[idx]) == 0)
		{
			if (m_row[idx] == NULL) return false;
			return string(m_row[idx]) != "0";
		}
	}
	ASSERT(false && "Cann't find key of Rows");
	return false;
}

int64 Rows::GetInt(const string& key)
{
	for (int idx = 0; idx < (int)m_columns.size(); idx++)
	{
		if (XStrUtil::compare(key, m_columns[idx]) == 0)
		{
			if (m_row[idx] == NULL) return 0;
			XStrParser parser(m_row[idx], strlen(m_row[idx]));
			int64 v = parser.get_integer();
			if (parser.position() == parser.start() || !parser.eof())
			{
				XWARNING("Rows::get_int(%s), Can't covert '%s' to int64.", key.c_str(), m_row[idx]);
			}
			return v;
		}
	}
	ASSERT(false && "Cann't find key of Rows");
	return 0;
}

double Rows::GetFloat(const string& key)
{
	for (int idx = 0; idx < (int)m_columns.size(); idx++)
	{
		if (XStrUtil::compare(key, m_columns[idx]) == 0)
		{
			if (m_row[idx] == NULL) return 0;
			double v = 0.0;
			if (!XStrUtil::to_float(m_row[idx], v))
			{
				XWARNING("Rows::get_float(%s), Can't covert '%s' to double.", key.c_str(), m_row[idx]);
			}
			return v;
		}
	}
	ASSERT(false && "Cann't find key of Rows");
	return 0.0;
}

string Rows::GetString(const string& key)
{
	for (int idx = 0; idx < (int)m_columns.size(); idx++)
	{
		if (XStrUtil::compare(key, m_columns[idx]) == 0)
		{
			if (m_row[idx] == NULL) return "";
			return m_row[idx];
		}
	}
	ASSERT(false && "Cann't find key of Rows");
	return "";
}

string Rows::GetGBKString(const string& key)
{
	for (int idx = 0; idx < (int)m_columns.size(); idx++)
	{
		if (XStrUtil::compare(key, m_columns[idx]) == 0)
		{
			if (m_row[idx] == NULL) return "";
			string strTemp;
			utf8_to_gbk(strTemp, m_row[idx]);
			return strTemp;
		}
	}
	ASSERT(false && "Cann't find key of Rows");
	return "";
}



XTime Rows::GetTime(const string& key)
{
	for (int idx = 0; idx < (int)m_columns.size(); idx++)
	{
		if (XStrUtil::compare(key, m_columns[idx]) == 0)
		{
			if (m_row[idx] == NULL) return XTime::ZeroTime;
			return XTime::try_parse(m_row[idx]);
		}
	}
	ASSERT(false && "Cann't find key of Rows");
	return XTime::ErrorTime;
}

string Rows::GetBlob(const string& key)
{
	for (int idx = 0; idx < (int)m_columns.size(); idx++)
	{
		if (XStrUtil::compare(key, m_columns[idx]) == 0)
		{
			if (m_row[idx] == NULL) return "";
			return string(m_row[idx], m_lengths[idx]);
		}
	}
	ASSERT(false && "Cann't find key of Rows");
	return "";
}


////////////////////////////////////////////////////////////////////////////////
// class Statement
////////////////////////////////////////////////////////////////////////////////
Statement* Statement::SetParameter(const string& key, const Value& value)
{
	ASSERT(key.size() >= 2 && key.at(0) == '@');
	if (key.size() < 2 || key.at(0) != '@') return this;
	map<string, Value>::iterator it = m_parameters.find(key);
	if (it != m_parameters.end())
		it->second = value;
	else
		m_parameters[key] = value;
	return this;
}

shared_ptr<IXBuffer> Statement::make()
{
	// 计算需要的缓冲区空间
	size_t sz = m_sql.size();
	for (map<string, Value>::iterator it=  m_parameters.begin(); it != m_parameters.end(); ++it)
	{
		sz += it->second.m_len;
	}
	sz = sz * 2 + 1;

	// 申请缓冲区
	shared_ptr<IXBuffer> ptrBuffer = xcore::create_buffer(sz);
	char* pos = (char*)ptrBuffer->data();
	uint32 len = 0;

	// 构造
	uint32 count = 0;
	XStrParser parser(m_sql.c_str(), m_sql.size());
	while (!parser.eof())
	{
		string str = parser.getstr_without("@\'\"");
		memcpy(pos, str.c_str(), str.size());
		pos += (uint32)str.size();
		len += (uint32)str.size();
		if (parser.eof()) break;
		if (*parser.position() == '\'' || *parser.position() == '\"')
		{
			parser.getstr_by_sign(str, *parser.position(), *parser.position(), true);
			memcpy(pos, str.c_str(), str.size());
			pos += (uint32)str.size();
			len += (uint32)str.size();
			continue;
		}
		str = parser.getstr_without(",;. \r\n\t\\()!=<>\'\"%!^[]*+-/`?{}");
		map<string, Value>::iterator it = m_parameters.find(str);
		if (it == m_parameters.end())
		{
			XERROR("Statement make(), cann't match '%s'.", str.c_str());
			return shared_ptr<IXBuffer>();
		}
		switch (it->second.m_type)
		{
		case 1:
			*pos++ = it->second.m_data.m_bool ? '1' : '0';
			len++;
			break;
		case 2:
			str = XStrUtil::to_str(it->second.m_data.m_int, "%lld");
			memcpy(pos, str.c_str(), str.size());
			pos += (uint32)str.size();
			len += (uint32)str.size();
			break;
		case 3:
			str = XStrUtil::to_str(it->second.m_data.m_float);
			memcpy(pos, str.c_str(), str.size());
			pos += (uint32)str.size();
			len += (uint32)str.size();
			break;
		case 4:
		case 5:
			{
				*pos++ = '\'';
				len++;
				count = (uint32)mysql_real_escape_string(m_ptrConn->m_mysql, pos, (const char*)it->second.m_data.m_ptr, (uint32)it->second.m_len);
				pos += count;
				len += count;
				*pos++ = '\'';
				len++;
			}
			break;
		case 6:
			*pos++ = '\'';
			len++;
			str = XTime(it->second.m_data.m_time).to_str();
			memcpy(pos, str.c_str(), str.size());
			pos += str.size();
			len += (uint32)str.size();
			*pos++ = '\'';
			len++;
			break;
		case 0:
		default:
			memcpy(pos, "NULL", 4);
			pos += 4;
			len += 4;
			break;
		}
	}

	*pos = '\0';
	ptrBuffer->resize(len);
	return ptrBuffer;
}

shared_ptr<Result> Statement::Exec()
{
	shared_ptr<IXBuffer> ptrBuffer = make();
	if (ptrBuffer.get() == NULL) return shared_ptr<Result>();
	if (!m_ptrConn->do_query((const char*)ptrBuffer->data(), ptrBuffer->size())) return shared_ptr<Result>();
	shared_ptr<Result> ptrResult = shared_ptr<Result>(new Result);
	ptrResult->m_affected_rows = mysql_affected_rows(m_ptrConn->m_mysql);
	ptrResult->m_last_insertid = mysql_insert_id(m_ptrConn->m_mysql);
	return ptrResult;
}

shared_ptr<Rows> Statement::Query()
{
	shared_ptr<IXBuffer> ptrBuffer = make();
	if (ptrBuffer.get() == NULL) return shared_ptr<Rows>();
	if (!m_ptrConn->do_query((const char*)ptrBuffer->data(), ptrBuffer->size())) return shared_ptr<Rows>();
	shared_ptr<Rows> ptrRows = shared_ptr<Rows>(new Rows);
	ptrRows->m_res = mysql_store_result(m_ptrConn->m_mysql);
	if (ptrRows->m_res == NULL)
	{
		XERROR("In Statement::query() at mysql_store_result() failed.");
		return shared_ptr<Rows>();
	}
	ptrRows->m_count_rows = (uint32)mysql_num_rows(ptrRows->m_res);
	ptrRows->m_count_columns = (uint32)mysql_num_fields(ptrRows->m_res);

	MYSQL_FIELD* pField = NULL;
	while ((pField = mysql_fetch_field(ptrRows->m_res)))
	{
		ptrRows->m_columns.push_back(pField->name);
	}
	return ptrRows;
}


////////////////////////////////////////////////////////////////////////////////
// class Conn
////////////////////////////////////////////////////////////////////////////////
Conn::~Conn(void)
{
	this->do_close();
}

void Conn::do_close()
{
	if (m_mysql)
	{
		mysql_close(m_mysql);
		m_mysql = NULL;
	}
	return;
}

bool Conn::select_scheme(const string& scheme)
{
	if (m_mysql == NULL) return false;
	
	if (0 != mysql_select_db(m_mysql, scheme.c_str()))
	{
		XERROR("mysql(host:%s, user:%s, scheme:%s) select scheme(%s) failed.", 
				m_host.c_str(), m_user.c_str(), m_scheme.c_str(), scheme.c_str());
		return false;
	}

	if (!set_character_set())
	{
		return false;
	}
	
	XDEBUG("mysql(host:%s, user:%s, scheme:%s) select scheme(%s) OK.", 
			m_host.c_str(), m_user.c_str(), m_scheme.c_str(), scheme.c_str());

	m_scheme = scheme;
	return true;
}

bool Conn::get_variable(const string& var, string& value)
{
	value.clear();
	if (m_mysql == NULL) return false;
	string sql = "SHOW VARIABLES LIKE '" + var + "'";
	if (!do_query(sql)) return false;

	MYSQL_RES* res = mysql_store_result(m_mysql);
	if (res && (2 == mysql_num_fields(res)))
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		if (row)
		{
			value = row[1];
			mysql_free_result(res);
			return true;
		}
	}

	if (res) mysql_free_result(res);
	return false;
}

bool Conn::get_tables(vector<string>& vTables)
{
	vTables.clear();
	if (m_mysql == NULL) return false;
	MYSQL_RES* res = mysql_list_tables(m_mysql, NULL);
	if (res && (1 == mysql_num_fields(res)))
	{
		MYSQL_ROW row = NULL;
		while ((row = mysql_fetch_row(res)) != NULL)
		{
			if (row[0]) vTables.push_back(row[0]);
		}
		mysql_free_result(res);
		return true;
	}

	if (res) mysql_free_result(res);
	return false;
}

void Conn::last_error(uint32& no, string& error)
{
	no = mysql_errno(m_mysql);
	error = mysql_error(m_mysql);
	return;
}

bool Conn::do_connect()
{
	this->do_close();

	m_mysql = mysql_init(NULL); 
	if (m_mysql == NULL)
	{
		XERROR("mysql(host:%s, user:%s, scheme:%s) init failed.", 
				m_host.c_str(), m_user.c_str(), m_scheme.c_str());
		return false;
	}

	MYSQL * pConnectRet = mysql_real_connect(m_mysql, m_host.c_str(), 
											 m_user.c_str(), m_passwd.c_str(), 
											 m_scheme.empty() ? NULL : m_scheme.c_str(),
											 m_port, NULL, CLIENT_FOUND_ROWS);
	if (pConnectRet != m_mysql)
	{
		XERROR("mysql(host:%s, user:%s, scheme:%s) connect error: %s.", 
				m_host.c_str(), m_user.c_str(), m_scheme.c_str(), mysql_error(m_mysql));
		this->do_close();
		return false;
	}

	if (!set_character_set())
	{
		XERROR("mysql(host:%s, user:%s, scheme:%s) connect error: %s.", 
				m_host.c_str(), m_user.c_str(), m_scheme.c_str(), mysql_error(m_mysql));
		this->do_close();
		return false;
	}

	XINFO("mysql(host:%s, user:%s, scheme:%s) connect ok.", m_host.c_str(), 
			m_user.c_str(), m_scheme.c_str());
	return true;
}

bool Conn::do_query(const string& sql)
{
	return do_query(sql.c_str(), sql.size());
}

bool Conn::do_query(const char* sql, uint32 len)
{
	ASSERT(sql);
	if (m_mysql == NULL) return false;

	for (int i = 0; i < 5; i++)
	{
		if (0 == mysql_real_query(m_mysql, sql, (unsigned long)len))
		{
			return true;
		}

		int code = mysql_errno(m_mysql);
		if ((code != CR_SERVER_LOST) &&	
			(code != CR_CONN_HOST_ERROR) &&	
			(code != CR_SERVER_GONE_ERROR))
		{
			XERROR("mysql(host:%s, user:%s, scheme:%s) query error(%d)(%s), sql:%s.", 
					m_host.c_str(), m_user.c_str(), m_scheme.c_str(), code, mysql_error(m_mysql), sql);
			return false;
		}

		XERROR("mysql(host:%s, user:%s, scheme:%s) connection lost, try reconnecting...", 
				m_host.c_str(), m_user.c_str(), m_scheme.c_str());

		if (!this->do_connect()) return false;
	}
	
	return false;
}

bool Conn::set_character_set()
{
	// get database character set
	string value;
	if (!get_variable("character_set_database", value))
	{
		XERROR("mysql(host:%s, user:%s, scheme:%s) get_variable 'character_set_database' failed.\n",
				m_host.c_str(), m_user.c_str(), m_scheme.c_str());
		return false;
	}

	// Set default character set for the current connection
	// This function works like the SET NAMES 'charset_name' statement
	if (0 != mysql_set_character_set(m_mysql, value.c_str()))
	{
		XERROR("mysql(host:%s, user:%s, scheme:%s) mysql_set_character_set '%s' failed.",
			   m_host.c_str(), m_user.c_str(), m_scheme.c_str(), value.c_str());
		return false;
	}

	XDEBUG("mysql(host:%s, user:%s, scheme:%s) mysql_set_character_set '%s' ok.",
			m_host.c_str(), m_user.c_str(), m_scheme.c_str(), value.c_str());
	return true;
}

bool Conn::check_lower_case_table_names()
{
	// check 'lower_case_table_names'
	string value;
	if (!get_variable("lower_case_table_names", value))
	{
		XERROR("get_variable 'lower_case_table_names' failed.\n");
		do_close();
		return false;
	}
	if (value != "1")
	{
		XERROR("Please set mysql variables: lower_case_table_names = 1.");
		do_close();
		return false;
	}
	return true;
}

shared_ptr<Statement> Conn::Prepare(const string& sql)
{
	ASSERT(m_mysql);
	shared_ptr<Statement> ptrStmt(new Statement());
	ptrStmt->m_ptrConn = this->shared_from_this();
	ptrStmt->m_sql = sql;
	return ptrStmt;
}

bool Conn::Begin()
{
	ASSERT(m_mysql);
	return do_query("START TRANSACTION");
}

bool Conn::Commit()
{
	ASSERT(m_mysql);
	return do_query("COMMIT");
}

bool Conn::Rollback()
{
	ASSERT(m_mysql);
	return do_query("ROLLBACK");
}


///////////////////////////////////////////////////////////////////////////////
// class Database
////////////////////////////////////////////////////////////////////////////////
Database::Database()
	: m_port(0)
	, m_maxIdleConns(1)
	, m_maxConns(1)
	, m_isClosed(false)
{
	// empty
}

Database::~Database()
{
	// empty
}

Database* Database::Instance()
{
	static Database instance_;
	return &instance_;
}

bool Database::Open(const string& host, uint16 port, const string& scheme, const string& user, const string& passwd)
{
	ASSERT(m_host.empty());

	m_host = host;
	m_scheme = scheme;
	m_user = user;
	m_passwd = passwd;
	m_port = port;

	shared_ptr<Conn> ptrConn = new_conn();
	if (ptrConn.get() == NULL) return false;
	if (!ptrConn->check_lower_case_table_names()) return false;
	m_freeConns.push_back(ptrConn);
	return true;
}

void Database::Close()
{
	m_isClosed = true;
	XLockGuard<XCritical> lock(m_lock);
	m_freeConns.clear();
}

vector<string> Database::Tables()
{
	vector<string> tables;
	shared_ptr<Conn> ptrConn = this->Connect();
	if (ptrConn.get() == NULL) return tables;
	ptrConn->get_tables(tables);
	return tables;
}

shared_ptr<Conn> Database::Connect(int timeout)
{
	if (m_isClosed) return shared_ptr<Conn>();

	shared_ptr<Conn> ptrConn;
	uint32 connCount = 0;
	do
	{
		m_lock.lock();
		if (!m_freeConns.empty())
		{
			ptrConn = m_freeConns.front();
			m_freeConns.pop_front();
			m_useConns.insert(ptrConn.get());
			m_lock.unlock();
			return ptrConn;
		}
		connCount += (uint32)m_useConns.size();
		connCount += (uint32)m_freeConns.size();
		m_lock.unlock();

		if (connCount < m_maxConns)
		{
			ptrConn = new_conn();
			if (ptrConn.get())
			{
				m_lock.lock();
				m_useConns.insert(ptrConn.get());
				m_lock.unlock();
				return ptrConn;
			}
			else
			{
				xcore::sleep(1);
				continue;
			}
		}

		// wait
		if (m_event.trywait(timeout)) continue;

	} while(false);
	
	return shared_ptr<Conn>();
}

uint32 Database::GetMaxIdleConn()
{
	return m_maxIdleConns;
}

void Database::SetMaxIdleConn(uint32 maxIdleConn)
{
	list<shared_ptr<Conn> > freeConns; // 自动析构关闭连接
	m_maxIdleConns = maxIdleConn;

	m_lock.lock();
	while ((uint32)m_freeConns.size() > maxIdleConn)
	{
		shared_ptr<Conn> ptrConn = m_freeConns.front();
		m_freeConns.pop_front();
		freeConns.push_back(ptrConn);
	}
	m_lock.unlock();
}

uint32 Database::GetMaxConn()
{
	return m_maxConns;
}

void Database::SetMaxConn(uint32 maxConn)
{
	list<shared_ptr<Conn> > freeConns; // 自动析构关闭连接
	m_maxConns = maxConn;

	m_lock.lock();
	if (maxConn >= (uint32)m_useConns.size())
		maxConn -= (uint32)m_useConns.size();
	else
		maxConn = 0;
	while ((uint32)m_freeConns.size() > maxConn)
	{
		shared_ptr<Conn> ptrConn = m_freeConns.front();
		m_freeConns.pop_front();
		freeConns.push_back(ptrConn);
	}
	m_lock.unlock();
}

shared_ptr<Conn> Database::new_conn()
{
	shared_ptr<Conn> ptrConn = shared_ptr<Conn> (new Conn, Conn_deleter());
	ptrConn->m_mysql = NULL;
	ptrConn->m_host = m_host;
	ptrConn->m_port = m_port;
	ptrConn->m_scheme = m_scheme;
	ptrConn->m_user = m_user;
	ptrConn->m_passwd = m_passwd;

	if (!ptrConn->do_connect()) return shared_ptr<Conn>();
	return ptrConn;
}

bool Database::put_conn(Conn* conn)
{
	ASSERT(conn);
	XLockGuard<XCritical> lock(m_lock);
	if (m_useConns.erase(conn) <= 0) return false;
	if (m_isClosed) return false; // 已关闭,不再往队列放
	if ((uint32)(m_freeConns.size() + m_useConns.size()) < m_maxConns && 
		(uint32) m_freeConns.size() < m_maxIdleConns)
	{
		m_freeConns.push_back(shared_ptr<Conn> (conn, Conn_deleter()));
		m_event.set(); // 通知
		return true;
	}
	return false;
}

void Database::Conn_deleter::operator()(Conn* conn) const
{
	if (conn == NULL) return;
	if (!Database::Instance()->put_conn(conn))
	{
		conn->do_close();
		delete conn;
	}
}


////////////////////////////////////////////////////////////////////////////////
// test_mymysql
////////////////////////////////////////////////////////////////////////////////
void test_mymysql()
{
	ASSERT(Database::Instance()->Open("localhost", 3306, "test", "root", "123456"));
	for (int i = 0; i < 10; i++)
	{
		shared_ptr<Conn> ptrConn2 = Database::Instance()->Connect();
	}
	shared_ptr<Conn> ptrConn = Database::Instance()->Connect();

	shared_ptr<Result> ptrResult0 =
	ptrConn->Prepare("CREATE TABLE IF NOT EXISTS `test` ( \
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT, \
		`MyInt` int(11) NOT NULL DEFAULT '0', \
		`MyStr` varchar(32) NOT NULL DEFAULT '', \
		`MyBlob` blob NOT NULL, \
		`MySmallInt` tinyint(3) unsigned NOT NULL DEFAULT '0',\
		`MyChars` char(12) NOT NULL DEFAULT '',\
		`MyNilInt` int(11) DEFAULT NULL, \
		`MyNilStr` varchar(255) DEFAULT NULL, \
		`MyTime` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP, \
		`MyDateTime` datetime DEFAULT NULL, \
		PRIMARY KEY (`ID`) \
		) ENGINE=InnoDB DEFAULT CHARSET=utf8")
		->Exec();
	ASSERT(ptrResult0.get());
	int64 affected_rows0 = ptrResult0->RowsAffected();
	int64 last_insertid0 = ptrResult0->LastInsertId();
	printf("affected_rows0=>%lld, last_insertid0=>%lld\n", affected_rows0, last_insertid0);

	ASSERT(ptrConn->Begin()); // 事务开始

	char buf[256];
	for (int i = 0; i < 256; i++) buf[i] = (char)i;
	shared_ptr<Result> ptrResult =
	ptrConn->Prepare("insert into test values(NULL, @MyInt, @MyStr, @MyBlob, @MySmallInt, @MyChars, @MyNilInt, @MyNilStr, @MyTime, @MyDateTime)")
		->SetParameter("@MyInt", 123)
		->SetParameter("@MyStr", "hello我们bbbb")
		->SetParameter("@MyBlob", Value(buf, 256))
		->SetParameter("@MySmallInt", 12)
		->SetParameter("@MyChars", "abcdbcdbcdbcdbcd")
		->SetParameter("@MyNilInt", Value()) // NULL
		->SetParameter("@MyNilStr", Value())
		->SetParameter("@MyTime", XTime())
		->SetParameter("@MyDateTime", Value())
		->Exec();
	ASSERT(ptrResult.get());
	int64 affected_rows = ptrResult->RowsAffected();
	int64 last_insertid = ptrResult->LastInsertId();
	printf("affected_rows=>%lld, last_insertid=>%lld\n", affected_rows, last_insertid);

	ASSERT(ptrConn->Commit()); // 事务提交
	//ASSERT(ptrConn->Rollback()); // 回滚

	shared_ptr<Rows> ptrRows =
	ptrConn->Prepare("select * from test")
		->Query();
	ASSERT(ptrRows.get());
	while(ptrRows.get() && ptrRows->Next())
	{
		int64 id = ptrRows->GetInt(0); printf("%lld\n", id);
		int64 myint = ptrRows->GetInt(1); printf("%lld\n", myint);
		string mystr = ptrRows->GetString(2); printf("%s\n", mystr.c_str());
		string myblob = ptrRows->GetBlob(3); string s; XStrUtil::dump(s, myblob.c_str(), myblob.size()); printf("%s", s.c_str());
		int64 mysamllint = ptrRows->GetInt("MySmallInt"); printf("%lld\n", mysamllint);
		string mychars = ptrRows->GetString("MyChars"); printf("%s\n", mychars.c_str());
		int64 mynilint = ptrRows->GetInt(6); printf("%lld\n", mynilint);
		string mynilstr = ptrRows->GetString(7); printf("%s\n", mynilstr.c_str());
		string mytime = ptrRows->GetString(8); printf("%s\n", mytime.c_str());
		XTime mydatetime = ptrRows->GetTime(9); printf("%s\n", mydatetime.to_str().c_str());
		printf("\n");
	}

	shared_ptr<Result> ptrResult2 =
	ptrConn->Prepare("update test set MyNilStr=@MyNilStr,MyNilInt=@MyNilInt where ID=@ID")
		->SetParameter("@MyNilStr", "wowow")
		->SetParameter("@MyNilInt", 444)
		->SetParameter("@ID", 23)
		->Exec();
	ASSERT(ptrResult2.get());
	int64 affected_rows2 = ptrResult2->RowsAffected();
	int64 last_insertid2 = ptrResult2->LastInsertId();
	printf("affected_rows2=>%lld, last_insertid2=>%lld\n", affected_rows2, last_insertid2);

	shared_ptr<Result> ptrResult3 =
		ptrConn->Prepare("delete from test where ID=@ID")
		->SetParameter("@ID", 23)
		->Exec();
	ASSERT(ptrResult3.get());
	int64 affected_rows3 = ptrResult3->RowsAffected();
	int64 last_insertid3 = ptrResult3->LastInsertId();
	printf("affected_rows3=>%lld, last_insertid3=>%lld\n", affected_rows3, last_insertid3);

	shared_ptr<Result> ptrResult4 =
		ptrConn->Prepare("drop table test")
					   ->Exec();
	ASSERT(ptrResult4.get());
	int64 affected_rows4 = ptrResult4->RowsAffected();
	int64 last_insertid4 = ptrResult4->LastInsertId();
	printf("affected_rows4=>%lld, last_insertid4=>%lld\n", affected_rows4, last_insertid4);

	Database::Instance()->Close();
	return ;
}

}// namespace mysql
