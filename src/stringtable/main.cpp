#include <tchar.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <atlstr.h>
#include <windows.h>
#include <wil/resource.h>

using namespace std;
using namespace wil;

constexpr const TCHAR* wiki = L"https://github.com/hereafter/string-table/wiki";

void replaceAll(wstring& str, const wstring& from, const wstring& to);
void usage();

int _tmain(int argc, TCHAR** argv)
{
	if (argc <= 1)
	{
		usage();
		return -1;
	}

    wstring dllPath = argv[1];
    unique_hmodule hm{ ::LoadLibraryEx(dllPath.c_str(), nullptr, LOAD_LIBRARY_AS_IMAGE_RESOURCE)};
	if (hm == nullptr)
	{
		cout << "Unable to open " << (CW2A(dllPath.c_str(), CP_UTF8).m_psz) << endl;
		return -1;
	}

	TCHAR szCsvPath[MAX_PATH] = { 0 };
	StrCpy(szCsvPath, dllPath.c_str());
	::PathRemoveExtension(szCsvPath);
	::StrCat(szCsvPath, L".csv");
	::PathStripPath(szCsvPath);

	ofstream os{ szCsvPath };
	if (!os.is_open())
	{
		cout << "Unable to write to " << (CW2A(szCsvPath, CP_UTF8).m_psz) << endl;
		return -1;
	}
	
	int lines = 0;
	for (int i = 0; i < INT16_MAX; i++)
	{
		TCHAR szBuffer[MAX_PATH] = { 0 };
		auto count=::LoadString(hm.get(), i, szBuffer, MAX_PATH);
		if (count <= 0) continue;

		wstring value = szBuffer;
		replaceAll(value, L"\r\n", L"\\n");
		replaceAll(value, L"\n", L"\\n");
		replaceAll(value, L"\r", L"\\n");
		replaceAll(value, L"\"", L"\"\"");
		wstringstream ss;
		ss << i << ", \"" << value.c_str() << "\"" << endl;
		string text = CW2A(ss.str().c_str(), CP_UTF8).m_psz;
		os.write(text.c_str(), text.length());
		lines++;
	}

	if (lines == 1)
	{
		cout << lines << " string extracted." << endl;
	}
	else
	{
		cout << lines << " strings extracted." << endl;
	}

	return 0;
}

void usage()
{
	cout << "Export string table to csv from dlls." << endl;
	cout << endl << "STRINGTABLE [drive:][path][filename]" << endl;
	cout << endl << "  [drive:][path][filename]" << endl;
	cout << "  \tSpecifies drive, directory and file name to a dll file" << endl;
}

void replaceAll(wstring& str, const wstring& from, const wstring& to)
{
	if (from.empty()) return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != wstring::npos) 
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

