
#include "stdafx.h"
#include "Directory.h"
#include "Path.h"
//#include <errno.h>


bool ComparePath(const wstring& path, const wstring& patt, size_t i, size_t j)
{
	while (i<path.length() && j<patt.length())
	{
		if (patt[j]=='?') { ++i; ++j; continue; }
		if (patt[j]=='*')
		{
			if (patt[j+1]==0) return true;
			while (i<path.length())
				if (ComparePath(path,patt,i++,j+1)==0) return true;
			return false;
		}
		if (path[i++]!=patt[j++]) return false;
	}
	return (i==path.length() && j==patt.length());
}

bool PathUp(wstring& path)
{
	if (path.size()<3) return false;
	wstring s = path; if (*s.rbegin()=='\\') s.pop_back();
	size_t k = s.rfind('\\'); if (k==wstring::npos || k<2) return false;
	path.erase(k+1);
	return true;
}

bool GetFullPath(wstring& path)
{
	wchar_t sz[MAX_PATH];
	if (GetFullPathNameW(path.c_str(),MAX_PATH,sz,NULL)==0) return false;
	path = sz;
	return true;
}

bool CreateDirs(wstring path)
{
	if (!IsFullPath(path) && !GetFullPath(path)) return false;
	if (*path.rbegin()!='\\') path.erase(path.rfind('\\')+1);
	if (DirExists(path.c_str())) return true;
	wstring ppath = path; if (!PathUp(ppath) || !CreateDirs(ppath)) return false;
	//cout << "  mkdir {" << path << "}" << endl;
	return CreateDirectory(path.c_str(),NULL)==TRUE;
}

bool GetDirectory(wstrvec& dset, wstrvec& fset, std::wstring path)
{
	Path fp(path);
	wstring ddir = utf8wc(fp.DriveDir());
	wstring patt = utf8wc(fp.NameExt());
	if (patt==L"*" || patt==L"*.*") patt.erase();
	else patt = towlower(patt);
	dset.clear(); fset.clear();

	WIN32_FIND_DATAW info;
	HANDLE h = FindFirstFileW((ddir + L"*").c_str(),&info);
	if (h==INVALID_HANDLE_VALUE) return false;

	do {
		if (info.cFileName[0]=='.')
		{
			if (info.cFileName[1]==0) continue;
			if (info.cFileName[1]=='.' && info.cFileName[2]==0) continue;
		}
		wstring item = ddir + info.cFileName;
		if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { dset.push_back(item); continue; }
		if (patt.empty() || ComparePath(towlower(info.cFileName),patt)) fset.push_back(item);
	} while(FindNextFileW(h,&info));
	FindClose(h);
	return true;
}

inline bool Error(wstring s) { cout << endl << "*** " << utf8c(s) << endl; return false; }
inline bool Error(wstring s1, wstring s2) { cout << endl << "*** " << utf8c(s1) << " " << utf8c(s2)<< endl; return false; }

bool RemoveAll(wstring path, bool subdir, bool view)
{
	if (path.size()>0)
	{
		if (*path.rbegin()!=DIRCHAR) path.push_back(DIRCHAR);
		if (!PathExists(path)) return true;
	}
	wstrvec dset,fset;
	if (!GetDirectory(dset, fset, path + L"*"))
		return (view ? Error(L"GetDirectory",path) : false);

	// remove files
	for (wstrvec::iterator fi=fset.begin(); fi!=fset.end(); ++fi)
		if (!DeleteFile((*fi).c_str())) return (view ? Error(L"Delete ",*fi) : false);

	// remove subdirs
	if (subdir)
	{
		for (wstrvec::iterator di=dset.begin(); di!=dset.end(); ++di)
			if (!RemoveAll(*di,subdir,view)) return false;
		if (!RemoveDirectory(path.c_str())) return (view ? Error(L"Remove ",path) : false);
	}
	return true;
}

wstring GetCurrentDir()
{
	wstring currDir;
	DWORD size = ::GetCurrentDirectoryW(0, NULL);
	if (size)
	{
		auto buff = new wchar_t[size+1];
		::GetCurrentDirectoryW(size+1, buff);
		currDir = buff; delete[] buff;
	}
	return currDir;
}

string CombinePath(const string& base, const string& path)
{
	if (IsFullPath(path)) return path;
	Path bp(base), pp(path);
	if (IsFullPath(base)) return bp.DriveDir() + pp.dir + pp.NameExt();
	if (!bp.name.empty()) bp.name.clear();
	return bp.dir + pp.dir + bp.name + pp.name + pp.ext;
}
