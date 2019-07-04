#ifndef _DB_TOOL_COMMON_H_
#define _DB_TOOL_COMMON_H_

#include <string>
using namespace std;


void   strTrimRight(string& strSrc, const string& strTrim);
void   strTrimLeft(string& strSrc, const string& strTrim);
string TrimPrefix(string strSrc);

#endif