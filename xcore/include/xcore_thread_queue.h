// 2012-04-05
// xcore_thread_queue.h
// guosh
// 任务队列


#ifndef _XCORE_THREAD_QUEUE_H_
#define _XCORE_THREAD_QUEUE_H_

#include "xcore_define.h"
#include "xcore_noncopyable.h"
#include "xcore_critical.h"
#include "xcore_event.h"

namespace xcore
{

///////////////////////////////////////////////////////////////////////////////
// class XThreadQueue
///////////////////////////////////////////////////////////////////////////////
template<typename TaskT_>
class XThreadQueue : public XNoncopyable
{
public:
	XThreadQueue(uint32 capacity_ = 0XFFFFFFFF)
		: m_capacity(capacity_)
		, m_exit(false)
		, m_count(0)
	{
		ASSERT(m_capacity > 0);
	}

	virtual ~XThreadQueue()
	{
		// empty
	}

	bool put(const TaskT_& task_, bool isFront = false, int timeout_ms = -1)
	{
		m_lock.lock();
		while (m_count >= m_capacity)
		{
			m_lock.unlock();
			if (m_exit || !m_notFull.trywait(timeout_ms) || m_exit) return false;
			m_lock.lock();
		}
		if (isFront)
			m_tasks.push_front(task_);
		else
			m_tasks.push_back(task_);
		m_count++;
		m_lock.unlock();
		m_notEmpty.set();
		return true;
	}

	bool take(TaskT_& task_, int timeout_ms = -1)
	{
		m_lock.lock();
		while (m_tasks.empty())
		{
			m_lock.unlock();
			if (m_exit || !m_notEmpty.trywait(timeout_ms) || m_exit) return false;
			m_lock.lock();
		}
		task_ = m_tasks.front();
		m_tasks.pop_front();
		if (m_count > 0) m_count--;
		m_lock.unlock();
		m_notFull.set();
		return true;
	}

	void notify_exit()
	{
		m_exit = true;
		for (int i = 0; i < 100; i++)
		{
			m_notFull.set();
			m_notEmpty.set();
		}
		return;
	}

	bool is_empty()
	{
		XLockGuard<XCritical> lock(m_lock);
		return m_tasks.empty();
	}

	bool is_full()
	{
		XLockGuard<XCritical> lock(m_lock);
		return m_count >= m_capacity;
	}

	uint32 size()
	{
		XLockGuard<XCritical> lock(m_lock);
		return m_count;
	}

	uint32 capacity() const
	{
		return m_capacity;
	}

private:
	const uint32  m_capacity;
	volatile bool m_exit;
	XCritical     m_lock;
	XEvent        m_notEmpty;
	XEvent        m_notFull;
	list<TaskT_>  m_tasks;
	uint32        m_count;
};

} // namespace xcore

using namespace xcore;

#endif//_XCORE_THREAD_QUEUE_H_
