// ctab.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "../utilib/Directory.h"
#include "../utilib/Path.h"
#include "../utilib/File.h"

void Process(wstring path, const wstrlist& proc);
void ProcessFile(wstring file, const wstrlist& proc);


static LPCSTR help =
	"\n"
	"Replace TAB/SPACE, and insert comment\n"
	"Usage: ctab [/tab n] [/sp n] [/c] file [..file]\n"
	"Parameters:\n"
	"    /tab n    replace n SPACE to TAB\n"
	"    /sp n     replace TAB to n SPACE\n"
	"    /c        comment each line by //\n"
	"    /cc       comment each line by ///\n"
	"    /ct       comment + TAB each line\n";
	//"    /s        subdirectory too\n";

int _tmain(int argc, wchar_t* argv[])
{
	if (argc<2) { cerr << help; return 0; }
	string err;
	try {
		wstrlist fset, proc;
		for (int j=1; j<argc; ++j)
		{
			wstring arg = argv[j];
			if (arg[0]=='/')
			{
				arg = towupper(arg);
				if (arg==L"/SP") { proc.push_back(arg); proc.push_back(wstring(argv[++j])); continue; }
				if (arg==L"/TAB") { proc.push_back(arg); proc.push_back(wstring(argv[++j])); continue; }
				if (arg==L"/C" || arg==L"/CC" || arg==L"/CT") { proc.push_back(arg); continue; }
				throw wstring(L"unexpected argument {") + argv[j] + L"}";
			}
			fset.push_back(arg);
		}
		// process
		if (proc.size()>0)
			for (wstrlist::iterator fi=fset.begin(); fi!=fset.end(); ++fi)
				Process(*fi, proc);

		return 0;
	}
	catch (LPCSTR s) { err = s; }
	catch (string& s) { err = s; }
	catch (LPCWSTR s) { err = utf8c(s); }
	catch (wstring& s) { err = utf8c(s); }
	catch (...) { err = "unknown exception"; }
	cerr << endl << "*** " << err << endl;
    return -1;
}

void Process(wstring path, const wstrlist& proc)
{
	wstrvec dset, fset;
	if (path.find('*')==wstring::npos) fset.push_back(path);
	else if (!GetDirectory(dset,fset,path)) throw wstring(L"path not found; ") + path;

	for (wstrvec::iterator fi=fset.begin(); fi!=fset.end(); ++fi)
		ProcessFile(*fi, proc);
}

static bool InsertSpace(string& s, int n)
{
	int k = 0; string so;
	for (size_t j=0; j<s.size(); ++j)
	{
		if (s[j]==' ') continue;
		if (s[j]!='\t') { so += s.substr(j); break; }
		do { so.push_back(' '); } while (so.size()%n); ++k;
	}
	if (k>0) { s = so; return true; }
	return false;
}

static bool InsertTab(string& s, int n)
{
	string sp(n,' ');
	size_t k = s.size();
	for (size_t j=0; j<s.size(); ++j)
	{
		if (strncmp(s.c_str()+j,sp.c_str(),n)!=0) break;
		s.erase(j,n); s.insert(j,"\t");
	}
	return s.size()<k;
}

void ProcessFile(wstring file, const wstrlist& proc)
{
	cout << utf8c(file) << endl;

	strvec text; text.reserve(1000);
	if (!File::ReadText(text, file.c_str())) throw "file not found";

	int m = 0;
	for (strvec::iterator li=text.begin(); li!=text.end(); ++li)
	{
		for (wstrlist::const_iterator pi=proc.begin(); pi!=proc.end(); ++pi)
		{
			if (*pi==L"/C") { (*li).insert(0,"//"); ++m; }
			else if (*pi==L"/CC") { (*li).insert(0,"///"); ++m; }
			else if (*pi==L"/CT") { (*li).insert(0,"//\t"); ++m; }
			else if (*pi==L"/TAB")
			{
				int n = _wtoi((*++pi).c_str()); if (n<2) throw "invalid TAB size";
				InsertSpace(*li, n); if (InsertTab(*li, n)) ++m;
			}
			else if (*pi==L"/SP")
			{
				int n = _wtoi((*++pi).c_str()); if (n<2) throw "invalid SPACE size";
				InsertTab(*li, n); if (InsertSpace(*li, n)) ++m;
			}
		}
	}
	if (m>0 && !File::WriteText(file.c_str(),text)) throw "write error";
}
