// 2008-12-03
// xcore_socket.h
// guosh
// socket简单封装


#ifndef _XCORE_SOCKET_H_
#define _XCORE_SOCKET_H_

#include "xcore_define.h"
#include "xcore_noncopyable.h"
#include "xcore_sock_addr.h"

namespace xcore
{

#ifdef __WINDOWS__
struct iovec
{
	void*  iov_base;  // Base address
	size_t iov_len;   // Length
};
#endif//__WINDOWS__

////////////////////////////////////////////////////////////////////////////////
// class XSocket
////////////////////////////////////////////////////////////////////////////////
class XSocket : XNoncopyable
{
public:
	explicit XSocket(SOCKET sock = INVALID_SOCKET) : m_sock(sock) {}
	~XSocket();

	SOCKET get_handle() const { return m_sock; }
	bool   is_open() const;
	void   attach(SOCKET sock);
	SOCKET detach();

	bool open(int type = SOCK_STREAM, uint16 family = AF_INET/* or AF_INET6 */);
	bool bind(const XSockAddr& addr);
	bool connect(const XSockAddr& addr);
	// listen时会自动调用bind
	bool listen(const XSockAddr& addr, int backlog = -1);
	bool accept(XSocket& sock, XSockAddr* remote_addr = NULL);
	bool shutdown();
	bool close(int delay = -1);
	bool abort();

	XSockAddr local_addr() const;
	XSockAddr remote_addr() const;

public:
	bool set_reuse_addr(bool bl = true);
	bool set_keep_alive(bool bl = true);
	bool set_nonblock(bool bl = true);
	bool set_tcp_nodelay(bool bl = true);
	bool set_linger(uint16 delay_sec, bool bl = true);
	bool set_send_bufsize(uint32 nSize = 8192);
	bool set_recv_bufsize(uint32 nSize = 8192);
	bool get_send_bufsize(uint32& nSize) const;
	bool get_recv_bufsize(uint32& nSize) const;

public:
	bool can_recv(int timeout_ms = -1);
	bool can_send(int timeout_ms = -1);

	// return >=0: send/recv number of bytes, -1: error, -2 peer host closed
	int send(const void* buf, int len);
	int recv(void* buf, int len);
	int sendto(const void* buf, int len, const XSockAddr& addr);
	int recvfrom(void* buf, int len, XSockAddr& addr);
	int send_n(const void* buf, int len, int timeout_ms = -1);
	int recv_n(void* buf, int len, int timeout_ms = -1);

	bool send_v(const iovec* iov, int cnt);

private:
	bool _is_can_restore();
	bool _is_already();
	bool _is_would_block();
	bool _is_emfile();

private:
	volatile SOCKET m_sock;
};

} // namespace xcore

using namespace xcore;

#endif//_XCORE_SOCKET_H_
