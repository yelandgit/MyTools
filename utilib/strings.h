#pragma once

#include <windows.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <list>


std::string unistr(LPCWSTR s, int cp);
inline std::string unistr(const std::wstring& ws, int cp) { return unistr(ws.c_str(),cp); }
std::wstring uniwstr(LPCSTR s, int cp);
inline std::wstring uniwstr(const std::string& ws, int cp) { return uniwstr(ws.c_str(),cp); }

inline std::string utf8c(LPCWSTR s) { return unistr(s, 65001); }
inline std::string utf8c(const std::wstring& ws) { return unistr(ws.c_str(), 65001); }
inline std::wstring utf8wc(LPCSTR s) { return uniwstr(s, 65001); }
inline std::wstring utf8wc(const std::string& s) { return uniwstr(s.c_str(), 65001); }

inline std::string tolower(std::string s) { for (std::string::iterator i=s.begin(); i!=s.end();) *i++ = tolower(*i); return s; }
inline std::string toupper(std::string s) { for (std::string::iterator i=s.begin(); i!=s.end(); ++i) *i = toupper(*i); return s; }
inline std::wstring towlower(std::wstring s) { for (std::wstring::iterator i=s.begin(); i!=s.end();) *i++ = towlower(*i); return s; }
inline std::wstring towupper(std::wstring s) { for (std::wstring::iterator i=s.begin(); i!=s.end();) *i++ = towupper(*i); return s; }

inline size_t replace(std::string& s, char c1, char c2) { size_t n=0; for (std::string::iterator i=s.begin(); i!=s.end(); ++i) if (*i==c1) { *i = c2; ++n; } return n; }
inline size_t replace(std::wstring& s, wchar_t c1, wchar_t c2) { size_t n=0; for (std::wstring::iterator i=s.begin(); i!=s.end(); ++i) if (*i==c1) { *i = c2; ++n; } return n; }

inline std::string operator + (const std::string& s1, char c) { std::string s(s1); s += c; return s; }
inline std::string operator + (const std::string& s1, LPCSTR sz) { std::string s(s1); s += sz; return s; }
inline std::string operator + (const std::string& s1, const std::string& s2) { std::string s(s1); s += s2; return s; }
inline std::string operator + (const std::string& s, int d) { char sz[32]; sprintf_s(sz,sizeof(sz),"%d",d); return s + sz; }
inline std::string operator + (const std::string& s, long d) { char sz[32]; sprintf_s(sz,sizeof(sz),"%ld",d); return s + sz; }
inline std::string operator + (const std::string& s, __int64 d) { char sz[32]; sprintf_s(sz,sizeof(sz),"%lld",d); return s + sz; }

inline std::ostream& operator << (std::ostream& os, const std::string& s) { os << s.c_str(); return os; }
inline std::ostream& operator << (std::ostream& os, LPCWSTR s) { if (s[0]) os << utf8c(s).c_str(); return os; }
inline std::ostream& operator << (std::ostream& os, const std::wstring& s) { if (s.size()>0) os << utf8c(s).c_str(); return os; }

typedef std::vector<std::string> strvec;
typedef std::vector<std::wstring> wstrvec;
typedef std::list<std::string> strlist;
typedef std::list<std::wstring> wstrlist;

inline std::ostream& operator << (std::ostream& os, const strvec& v)
{
	for (strvec::const_iterator i=v.begin(); i!=v.end(); ++i)
		os << (*i).c_str() << std::endl;
	return os;
}
inline std::ostream& operator << (std::ostream& os, const wstrvec& v)
{
	for (wstrvec::const_iterator i=v.begin(); i!=v.end(); ++i)
		os << utf8c(*i).c_str() << std::endl;
	return os;
}

size_t tokens(strvec& v, const std::string& s, LPCSTR sep=" \t");

inline std::string hex(short d) { char sz[40]; sprintf_s(sz, sizeof(sz), "%02x", d); return std::string(sz); }
inline std::string hex(unsigned short d) { char sz[40]; sprintf_s(sz, sizeof(sz), "%02x", d); return std::string(sz); }
inline std::string hex(int d) { char sz[40]; sprintf_s(sz, sizeof(sz), "%04x", d); return std::string(sz); }
inline std::string hex(unsigned int d) { char sz[40]; sprintf_s(sz, sizeof(sz), "%04x", d); return std::string(sz); }
inline std::string hex(long d) { char sz[40]; sprintf_s(sz, sizeof(sz), "%08lx", d); return std::string(sz); }
inline std::string hex(unsigned long d) { char sz[40]; sprintf_s(sz, sizeof(sz), "%08lx", d); return std::string(sz); }
inline std::string hex(__int64 d) { char sz[40]; sprintf_s(sz, sizeof(sz), "%016llx", d); return std::string(sz); }
inline std::string hex(unsigned __int64 d) { char sz[40]; sprintf_s(sz, sizeof(sz), "%016llx", d); return std::string(sz); }
inline std::string hex(const void* p) { char sz[40]; sprintf_s(sz, sizeof(sz), "%016llx", (__int64)p); return std::string(sz); }

void xview(std::ostream& os, const void* lp, size_t n, int m=4);

inline void wait(LPCSTR msg="next") { std::cout << msg << "> "; char sz[64]; gets_s(sz,sizeof(sz)); }
