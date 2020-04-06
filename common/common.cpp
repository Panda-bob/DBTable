#include "common.h"

void strTrimLeft(string& strSrc, const string& strTrim)
{
	int nPos = strSrc.find(strTrim);
	if (nPos == string::npos || nPos != 0)
		return;

	strSrc = string(strSrc.c_str() + strTrim.length(), strSrc.length() - strTrim.length());
	return ;
};

void strTrimRight(string& strSrc, const string& strTrim)
{
	int nPos = strSrc.find(strTrim);
	if (nPos == string::npos || nPos != strSrc.length() - strTrim.length())
		return ;

	strSrc = string(strSrc.c_str(), strSrc.length() - strTrim.length());
	return ;
};

string TrimPrefix(string strSrc)
{
	strTrimLeft(strSrc, "n_");
	strTrimLeft(strSrc, "s_");
	strTrimLeft(strSrc, "t_");
	strTrimLeft(strSrc, "b_");
	return strSrc;
};