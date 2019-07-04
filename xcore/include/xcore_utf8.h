// 2013-09-26
// xcore_utf8.h
// guosh
// UTF-8编码支持类

#ifndef _XCORE_UTF8_H_
#define _XCORE_UTF8_H_

#include "xcore_define.h"

namespace xcore
{

///////////////////////////////////////////////////////////////////////////////
// UCS2取值范围: 0X0 - 0XFFFF
// UCS4取值范围: 0X0 - 0X7FFFFFFF
// UTF8取值范围: 0X0 - 0XFDBFBFBFBFBF
// class XUTF8
///////////////////////////////////////////////////////////////////////////////
class XUTF8
{
public:
	// 返回utf8字符, < 0:表示出错
	static int64 ucs4_to_utf8(int32 ucs4);
	static int64 ucs2_to_utf8(uint16 ucs2);

	// 返回ucs4/ucs2字符, < 0:表示出错
	static int32 utf8_to_ucs4(int64 utf8);
	static int32 utf8_to_ucs2(int64 utf8);

	// 是否为utf8字符
	static bool is_utf8(int64 utf8);

	// 从字符流中检出一个utf8字符,返回该字符长度,0表示错误
	static uint32 checkout_utf8_char(const char* szStr);

	// 从字符流中检出utf8字符串,返回检出的字符串(szStr可能不是一个完整的utf8字符串)
	static string checkout_utf8_string(const char* szStr);

	// 是否为一个完整的utf8字符串
	static bool is_utf8_string(const char* szStr, uint32 length);

	// 把一个ucs2/ucs4编码为utf8格式并拼接到字符串后
	static void append_to_utf8_string(string& utf8, uint16 ucs2);
	static bool append_to_utf8_string(string& utf8, int32 ucs4);

	// 读文件内容转为utf8编码格式
	// src_format: 源文件编码格式=>ANSI/ASCII,UTF-16,UTF-16 - Big Endian,UTF-8,UTF-8 NO BOM,UTF-16 - NO BOM,UTF-16 - Big Endian - NO BOM
	static bool readfile_to_utf8(string& utf8, string& src_format, const string& filepath);
};

}//namespace xcore

using namespace xcore;

#endif//_XCORE_UTF8_H_
