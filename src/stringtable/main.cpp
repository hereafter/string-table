#include <tchar.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <atlstr.h>
#include <windows.h>
#include <wil/resource.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <algorithm>
#include <conio.h>

using namespace std;
using namespace wil;

constexpr const TCHAR* wiki = L"https://github.com/hereafter/string-table/wiki";

bool is_launched_from_shell();
void replaceAll(wstring& str, const wstring& from, const wstring& to);
void usage();

int execute(int argc, const TCHAR** argv);

int _tmain(int argc, const TCHAR** argv)
{
	auto noshell = is_launched_from_shell();
	if (noshell)
	{
		SetConsoleTitle(L"StringTable");
	}

	auto code = execute(argc, argv);

	if (noshell)
	{
		ShellExecute(nullptr, L"open", wiki, nullptr, nullptr, SW_SHOW);
		cout << endl;
		cout << "Press any key to continue..." << endl;
		while (_kbhit() == 0)
		{
			Sleep(100);
		}
	}
	return code;
	
}

int execute(int argc, const TCHAR** argv)
{
	if (argc <= 1)
	{
		usage();
		return -1;
	}

	wstring dllPath = argv[1];
	unique_hmodule hm{ ::LoadLibraryEx(dllPath.c_str(), nullptr, LOAD_LIBRARY_AS_IMAGE_RESOURCE) };
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
		auto count = ::LoadString(hm.get(), i, szBuffer, MAX_PATH);
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

bool is_launched_from_shell()
{
	using namespace wil;

	DWORD ppid = 0;
	auto pid = ::GetCurrentProcessId();
	unique_handle snapshot{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) };

	PROCESSENTRY32 pe{ 0 };
	pe.dwSize = sizeof(pe);

	auto success = Process32First(snapshot.get(), &pe);
	if (!success) return false;
	do
	{
		if (pid == pe.th32ProcessID)
		{
			ppid = pe.th32ParentProcessID;
			break;
		}
	} while (Process32Next(snapshot.get(), &pe));

	if (ppid == 0) return false;

	unique_process_handle h{ ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ppid) };
	TCHAR szFilePath[MAX_PATH] = { 0 };
	DWORD filePathSize = MAX_PATH;
	::QueryFullProcessImageName(h.get(), 0, szFilePath, &filePathSize);

	auto fileName = ::PathFindFileName(szFilePath);
	TCHAR szFileName[MAX_PATH] = { 0 };
	StrCpy(szFileName, fileName);
	PathRemoveExtension(szFileName);
	wstring fn = szFileName;
	transform(fn.begin(), fn.end(), fn.begin(), ::towlower);

	if (fn == L"explorer" || fn == L"dllhost" || fn == L"sihost") return true;
	return false;
}