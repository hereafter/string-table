#include <iostream>
#include <tchar.h>
#include <string>
#include <sstream>
#include <windows.h>
#include <vector>
#include <atlstr.h>
#include <wil/resource.h>

using namespace std;
using namespace wil;

int main(int argc, char** argv)
{
    if (argc <= 1) return -1;   
    auto dllPath1 = argv[1];
    wstring dllPath = CA2W(dllPath1, CP_UTF8).m_psz;

    unique_hmodule hm{ ::LoadLibraryEx(dllPath.c_str(), nullptr, LOAD_LIBRARY_AS_IMAGE_RESOURCE) };
	
	vector<wstring> names;

	//vector<wstring> types;
	//EnumResourceTypes(hm.get(), [](HMODULE h, LPWSTR lpType,
	//	LONG_PTR lParam)
	//	{
	//		auto& results = *(vector<wstring>*)lParam;
	//		
	//		return TRUE;
	//	}, (LONG_PTR)&types);

	
	EnumResourceNamesEx(hm.get(), RT_STRING, [](HMODULE h, LPCWSTR lpType,
		LPWSTR lpName, LONG_PTR lParam)
		{
			vector<wstring>& results = *(vector<wstring>*)lParam;

			if (IS_INTRESOURCE(lpName))
			{
				TCHAR szBuffer[MAX_PATH] = { 0 };
				//results.push_back(to_wstring((int)lpName));
				::LoadString(h, (uint32_t)lpName, szBuffer, MAX_PATH);

				results.push_back(szBuffer);
			}
			else
			{
				results.push_back(lpName);
			}

			

			return TRUE;
		}, (LONG_PTR)&names, 0, 0);

	return 0;

}
