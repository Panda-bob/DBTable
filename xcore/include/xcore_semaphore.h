// 2008-02-27
// xcore_semaphore.h
// guosh
// 信号量类


#ifndef _XCORE_SEMAPHORE_H_
#define _XCORE_SEMAPHORE_H_

#include "xcore_define.h"

namespace xcore
{

////////////////////////////////////////////////////////////////////////////////
// class XSemaphore
////////////////////////////////////////////////////////////////////////////////
class XSemaphore
{
public:
	explicit XSemaphore(uint32 init_count = 0);

	~XSemaphore();

	void wait();

	bool trywait(uint32 msec = 0);

	bool post(uint32 count = 1);

	uint32 get_value();

private:
	class XSemaphoreImpl;
	XSemaphoreImpl* m_impl;
};

} // namespace xcore

using namespace xcore;

#endif//_XCORE_SEMAPHORE_H_
