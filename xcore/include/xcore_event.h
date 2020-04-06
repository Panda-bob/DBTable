// 2008-02-23
// xcore_event.h
// guosh
// windows下的事件概念


#ifndef _XCORE_EVENT_H_
#define _XCORE_EVENT_H_

#include "xcore_define.h"

namespace xcore
{

///////////////////////////////////////////////////////////////////////////////
// class XEvent
///////////////////////////////////////////////////////////////////////////////
class XEvent
{
public:
	// default is auto reset
	explicit XEvent(bool manual = false);

	~XEvent();

	static XEvent AutoStaticEvent;

	static XEvent ManualStaticEvent;

public:
	void set();

	void reset();

	void wait();

	bool trywait(int32 msec = 0);

private:
	class XEventImpl;
	XEventImpl* m_impl;
};

} // namespace xcore

using namespace xcore;

#endif//_XCORE_EVENT_H_
