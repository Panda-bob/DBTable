// 2009-02-04
// xcore_str_util.h
// guosh
// ��װһЩ�����ַ��������ĺ���


#ifndef _XCORE_STR_UTIL_H_
#define _XCORE_STR_UTIL_H_

#include "xcore_define.h"

namespace xcore
{

///////////////////////////////////////////////////////////////////////////////
// class XStrUtil
///////////////////////////////////////////////////////////////////////////////
class XStrUtil
{
public:
	// ȥ���ַ���ͷ(��β)�����ַ�����ָ�����ַ�
	static string& chop_head(string& strSrc, const char* pcszCharSet = " \t\r\n");
	static string& chop_tail(string& strSrc, const char* pcszCharSet = " \t\r\n");
	static string& chop(string& strSrc, const char* pcszCharSet = " \t\r\n"); // ȥ��ͷ��β��ָ���ַ�
	static string& chop_all(string& strSrc, const char* pcszCharSet = " \t\r\n"); // ȥ���ַ���������ָ���ַ�

	// �ַ���ת��д(��Сд)
	static void to_upper(char* pszSrc);
	static void to_lower(char* pszSrc);
	static void to_upper(string& strSrc);
	static void to_lower(string& strSrc);

	// �滻
	static void replace(char* str, char oldch, char newch);
	static void replace(char* str, const char* oldCharSet, char newch);
	static void replace(string& str, char oldch, char newch);
	static void replace(string& str, const char* oldCharSet, char newch); // �ҵ�ָ�����ַ����Ͼ���Ϊָ���ַ�

	// ƴ��
	static string join(const vector<string>& a, const string& sep);

	// �ظ�
	static string repeat(const string& str, uint32 count);

	// ���ִ�Сд�Ƚ�
	static int compare(const char* pszSrc1, const char* pszSrc2, int length = -1);
	static int compare(const string& str1, const string& str2, int length = -1);

	// �����ִ�Сд�Ƚ�
	static int compare_nocase(const char* pszSrc1, const char* pszSrc2, int length = -1);
	static int compare_nocase(const string& str1, const string& str2, int length = -1);

	// �����ַ�����ָ���ķָ��ַ��ֽ�Դ�ַ���,�����õ�vector��
	// nMaxCountָ�������õ�������,������maxCount����ֹ������,�����������;��Ϊ-1��ʾ��������
	static uint32 split(const string& strSrc, vector<string>& vItems, const char* pcszCharSet = " \r\n\t",
	                    int nMaxCount = -1);

	// �ַ���ת����
	static bool	  to_int(const string& strSrc, int& nValue, int radix = 10);
	static int	  to_int_def(const string& strSrc, int def = -1, int radix = 10);
	static int	  try_to_int_def(const string& strSrc, int def = -1, int radix = 10);
	static bool	  to_uint(const string& strSrc, uint32& uValue, int radix = 10);
	static uint32 to_uint_def(const string& strSrc, uint32 def = 0, int radix = 10);
	static uint32 try_to_uint_def(const string& strSrc, uint32 def = 0, int radix = 10);
	static bool	  to_uint64(const string& strSrc, uint64& uValue, int radix = 10);
	static uint64 to_uint64_def(const string& strSrc, uint64 def = 0, int radix = 10);
	static uint64 try_to_uint64_def(const string& strSrc, uint64 def = 0, int radix = 10);

	// �ַ���ת��������
	static bool   to_float(const string& strSrc, double& value);
	static double to_float_def(const string& strSrc, double def = 0.0);
	static double try_to_float_def(const string& strSrc, double def = 0.0);

	// ��ֵת�ַ���
	static string to_str(int nVal, const char* cpszFormat = NULL/*"%d"*/);
	static string to_str(uint32 uVal, const char* cpszFormat = NULL/*"%u"*/);
	static string to_str(int64 nlVal, const char* cpszFormat = NULL/*"%lld"*/);
	static string to_str(uint64 ulVal, const char* cpszFormat = NULL/*"%llu"*/);
	static string to_str(double fVal, const char* cpszFormat = NULL/*"%f"*/);

	// ��ʽ�����(������10KB)
	static string sprintf(const char cszFormat[], ...);

	// string hash
	static uint32 hash_code(const char* str);
	static uint32 murmur_hash(const void* key, uint32 len);

	// dump data
	static void   dump(string& result, const void* pdata, uint32 length);
	static string dump(const void* pdata, uint32 length);
};

} // namespace xcore

using namespace xcore;

#endif//_XCORE_STR_UTIL_H_
