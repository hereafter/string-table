#include <tchar.h>
#include <string>
#include <sstream>
#include <fstream>
#include <atlstr.h>
#include <windows.h>
#include <wil/resource.h>

using namespace std;
using namespace wil;

void replaceAll(wstring& str, const wstring& from, const wstring& to);

int _tmain(int argc, TCHAR** argv)
{
	if (argc <= 1)
	{
		return -1;
	}
    wstring dllPath = argv[1];
    unique_hmodule hm{ ::LoadLibraryEx(dllPath.c_str(), nullptr, LOAD_LIBRARY_AS_IMAGE_RESOURCE)};

	TCHAR szCsvPath[MAX_PATH] = { 0 };
	StrCpy(szCsvPath, dllPath.c_str());
	::PathRemoveExtension(szCsvPath);
	::StrCat(szCsvPath, L".csv");

	ofstream os{ szCsvPath };
	
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
	}

	return 0;
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