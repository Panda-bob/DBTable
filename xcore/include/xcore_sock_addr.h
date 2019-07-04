// 20010-09-29
// xcore_sock_addr.h
// guosh
// Sock地址类


#ifndef _XCORE_SOCK_ADDR_H_
#define _XCORE_SOCK_ADDR_H_

#include "xcore_define.h"

namespace xcore
{

////////////////////////////////////////////////////////////////////////////////
// class XSockAddr
////////////////////////////////////////////////////////////////////////////////
class XSockAddr
{
public:
	XSockAddr(uint16 family = AF_INET/* or AF_INET6*/);
	XSockAddr(const XSockAddr& addr);
	XSockAddr(const sockaddr_in& addr);
	XSockAddr(const sockaddr_in6& addr);
	XSockAddr(const string& host, uint16 port); // host: ip或域名
	~XSockAddr(void);

	XSockAddr& operator = (const XSockAddr& addr);
	XSockAddr& operator = (const sockaddr_in& addr);
	XSockAddr& operator = (const sockaddr_in6& addr);
	operator const sockaddr* () const;
	int size() const;
	uint16 family() const;

	void   set_port(uint16 port);
	uint16 get_port() const;

	// host such as "192.168.4.12" or "www.sina.com.cn"
	// 任意的ipv4地址为"0.0.0.0"
	// 任意的ipv6地址为"::"或"0:0:0:0:0:0:0:0"
	bool   set_host(const string& host);
	string get_hostaddr() const;

	string to_str() const;
	void   reset();

	bool is_any() const; // 任意
	bool is_none() const; // 无效
	bool is_loop() const; // 环路

	static bool local_addrs(vector<XSockAddr>& addrs);  // 获取本机所有IP地址
	static bool resolve_dns(vector<XSockAddr>& addrs, const string& domain);  // 解析域名

	friend bool operator == (const XSockAddr& addr1, const XSockAddr& addr2);
	friend bool operator != (const XSockAddr& addr1, const XSockAddr& addr2);

private:
	union Data
	{
		sockaddr_in  m_inaddr;
		sockaddr_in6  m_inaddr6;
	} m_data;
};

}//namespace xcore

using namespace xcore;

#endif//_XCORE_SOCK_ADDR_H_
