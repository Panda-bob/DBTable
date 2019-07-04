// 2009-07-27
// xcore_atomic.h
// guosh
// ԭ�Ӳ�����


#ifndef _XCORE_ATOMIC_H_
#define _XCORE_ATOMIC_H_

#include "xcore_define.h"

namespace xcore
{

///////////////////////////////////////////////////////////////////////////////
// class XAtomic32
///////////////////////////////////////////////////////////////////////////////
class XAtomic32
{
public:
	XAtomic32(int32 i = 0);
	XAtomic32(const XAtomic32& from);
	~XAtomic32();

	int32 get_value() const;  // ���ص�ǰֵ
	int32 set_value(int32 i); // ����ԭֵ
	int32 test_zero_inc(); // �����������ֵ

	operator int32();
	XAtomic32& operator= (int32 i);
	XAtomic32& operator= (const XAtomic32& from);

	int32 operator+= (int32 i); // �����������ֵ
	int32 operator-= (int32 i); // ���ؼ������ֵ
	int32 operator++ (int); // ����ԭֵ
	int32 operator-- (int); // ����ԭֵ
	int32 operator++ (); // �����������ֵ
	int32 operator-- (); // ���ؼ������ֵ

private:
	volatile int32 m_counter;
};


///////////////////////////////////////////////////////////////////////////////
// class XAtomic64
///////////////////////////////////////////////////////////////////////////////
class XAtomic64
{
public:
	XAtomic64(int64 i = 0);
	XAtomic64(const XAtomic64& from);
	~XAtomic64();

	int64 get_value() const;  // ���ص�ǰֵ
	int64 set_value(int64 i); // ����ԭֵ
	int64 test_zero_inc(); // �����������ֵ

	operator int64();
	XAtomic64& operator= (int64 i);
	XAtomic64& operator= (const XAtomic64& from);

	int64 operator+= (int64 i); // �����������ֵ
	int64 operator-= (int64 i); // ���ؼ������ֵ
	int64 operator++ (int); // ����ԭֵ
	int64 operator-- (int); // ����ԭֵ
	int64 operator++ (); // �����������ֵ
	int64 operator-- (); // ���ؼ������ֵ

private:
	volatile int64 m_counter;
};


///////////////////////////////////////////////////////////////////////////////
// class XAtomicLocker
///////////////////////////////////////////////////////////////////////////////
class XAtomicLocker
{
public:
	XAtomicLocker();

	~XAtomicLocker();

	void lock();

	bool trylock();

	void unlock();

private:
	XAtomic32  m_counter;
	XAtomic32  m_ownercount;
	pthread_t  m_ownerid;
};

}//namespace xcore

using namespace xcore;

#endif//_XCORE_ATOMIC_H_
