// 2013-09-26
// xcore_utf8.h
// guosh
// UTF-8����֧����

#ifndef _XCORE_UTF8_H_
#define _XCORE_UTF8_H_

#include "xcore_define.h"

namespace xcore
{

///////////////////////////////////////////////////////////////////////////////
// UCS2ȡֵ��Χ: 0X0 - 0XFFFF
// UCS4ȡֵ��Χ: 0X0 - 0X7FFFFFFF
// UTF8ȡֵ��Χ: 0X0 - 0XFDBFBFBFBFBF
// class XUTF8
///////////////////////////////////////////////////////////////////////////////
class XUTF8
{
public:
	// ����utf8�ַ�, < 0:��ʾ����
	static int64 ucs4_to_utf8(int32 ucs4);
	static int64 ucs2_to_utf8(uint16 ucs2);

	// ����ucs4/ucs2�ַ�, < 0:��ʾ����
	static int32 utf8_to_ucs4(int64 utf8);
	static int32 utf8_to_ucs2(int64 utf8);

	// �Ƿ�Ϊutf8�ַ�
	static bool is_utf8(int64 utf8);

	// ���ַ����м��һ��utf8�ַ�,���ظ��ַ�����,0��ʾ����
	static uint32 checkout_utf8_char(const char* szStr);

	// ���ַ����м��utf8�ַ���,���ؼ�����ַ���(szStr���ܲ���һ��������utf8�ַ���)
	static string checkout_utf8_string(const char* szStr);

	// �Ƿ�Ϊһ��������utf8�ַ���
	static bool is_utf8_string(const char* szStr, uint32 length);

	// ��һ��ucs2/ucs4����Ϊutf8��ʽ��ƴ�ӵ��ַ�����
	static void append_to_utf8_string(string& utf8, uint16 ucs2);
	static bool append_to_utf8_string(string& utf8, int32 ucs4);

	// ���ļ�����תΪutf8�����ʽ
	// src_format: Դ�ļ������ʽ=>ANSI/ASCII,UTF-16,UTF-16 - Big Endian,UTF-8,UTF-8 NO BOM,UTF-16 - NO BOM,UTF-16 - Big Endian - NO BOM
	static bool readfile_to_utf8(string& utf8, string& src_format, const string& filepath);
};

}//namespace xcore

using namespace xcore;

#endif//_XCORE_UTF8_H_
