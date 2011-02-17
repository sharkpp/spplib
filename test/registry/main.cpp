// registry.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#define SPPLIB_REGISTRY_ENABLE_INLINE
#include <spplib/registry.hpp>

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>

#include <map>
#include <spplib/apihook.h>

std::map<HKEY, int>	g_mapRegReport;

LSTATUS WINAPI Hook_RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
	LSTATUS (WINAPI *pfnOrig)(HKEY,LPCSTR,DWORD,REGSAM,PHKEY) = 
	      (LSTATUS (WINAPI *)(HKEY,LPCSTR,DWORD,REGSAM,PHKEY))
			GetProcAddress( GetModuleHandleA(_T("advapi32.dll")), _T("RegOpenKeyExA") );
	LSTATUS r = pfnOrig(hKey, lpSubKey, ulOptions, samDesired, phkResult);
	TRACE(">> RegOpenKeyExA(%p,\"%s\",%d,%x,%p) = %d,%p\n", hKey, lpSubKey, ulOptions, samDesired, phkResult, r, phkResult ? *phkResult : NULL);
	if( phkResult && *phkResult ) {
		g_mapRegReport[*phkResult] = (int)g_mapRegReport.size();
	}
	return r;
}

LSTATUS WINAPI Hook_RegCreateKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass,
							   DWORD dwOptions, REGSAM samDesired, CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							   PHKEY phkResult, LPDWORD lpdwDisposition)
{
	LSTATUS (WINAPI *pfnOrig)(HKEY,LPCSTR,DWORD,LPSTR,DWORD,REGSAM,CONST LPSECURITY_ATTRIBUTES,PHKEY,LPDWORD) = 
	      (LSTATUS (WINAPI *)(HKEY,LPCSTR,DWORD,LPSTR,DWORD,REGSAM,CONST LPSECURITY_ATTRIBUTES,PHKEY,LPDWORD))
			GetProcAddress( GetModuleHandleA(_T("advapi32.dll")), _T("RegCreateKeyExA") );
	LSTATUS r = pfnOrig(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
	TRACE(">> RegCreateKeyExA(%p,\"%s\",%d,\"%s\",%d,%x,%p,%p,%p) = %d,%p\n"
		, hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition, r, phkResult ? *phkResult : NULL);
	if( phkResult && *phkResult ) {
		g_mapRegReport[*phkResult] = (int)g_mapRegReport.size();
	}
	return r;
}

LSTATUS WINAPI Hook_RegDeleteKeyA(HKEY hKey, LPCSTR lpSubKey)
{
	LSTATUS (WINAPI *pfnOrig)(HKEY,LPCSTR) = 
	      (LSTATUS (WINAPI *)(HKEY,LPCSTR))
			GetProcAddress( GetModuleHandleA(_T("advapi32.dll")), _T("RegDeleteKeyA") );
	LSTATUS r = pfnOrig(hKey, lpSubKey);
	TRACE(">> RegDeleteKeyA(%p,\"%s\") = %d\n"
		, hKey, lpSubKey, r);
	return r;
}

LSTATUS WINAPI Hook_RegCloseKey(HKEY hKey)
{
	LSTATUS (WINAPI *pfnOrig)(HKEY) = 
	      (LSTATUS (WINAPI *)(HKEY))
			GetProcAddress( GetModuleHandleA(_T("advapi32.dll")), _T("RegCloseKey") );
	LSTATUS r = pfnOrig(hKey);
	TRACE(">> RegCloseKey(%p) = %d\n", hKey, r);
	std::map<HKEY, int>::iterator ite = 
		g_mapRegReport.find(hKey);
	if( ite != g_mapRegReport.end() ) {
		g_mapRegReport.erase(ite);
	}
	return r;
}

void InstallRegistryAPIHook()
{
	INSTALL_HOOK(_T("advapi32.dll"), _T("RegOpenKeyExA"),	Hook_RegOpenKeyExA,		NULL);
	INSTALL_HOOK(_T("advapi32.dll"), _T("RegCreateKeyExA"),	Hook_RegCreateKeyExA,	NULL);
	INSTALL_HOOK(_T("advapi32.dll"), _T("RegDeleteKeyA"),	Hook_RegDeleteKeyA,		NULL);
	INSTALL_HOOK(_T("advapi32.dll"), _T("RegCloseKey"),		Hook_RegCloseKey,		NULL);
}

void UninstallAndReportRegistryAPIHook()
{
	UNINSTALL_HOOK(_T("advapi32.dll"), _T("RegOpenKeyExA"),		Hook_RegOpenKeyExA,		NULL);
	UNINSTALL_HOOK(_T("advapi32.dll"), _T("RegCreateKeyExA"),	Hook_RegCreateKeyExA,	NULL);
	UNINSTALL_HOOK(_T("advapi32.dll"), _T("RegDeleteKeyA"),		Hook_RegDeleteKeyA,		NULL);
	UNINSTALL_HOOK(_T("advapi32.dll"), _T("RegCloseKey"),		Hook_RegCloseKey,		NULL);

	TRACE("-- REPORT --\n");
	for(std::map<HKEY, int>::iterator
			ite = g_mapRegReport.begin(),
			last= g_mapRegReport.end();
		ite != last; ++ite)
	{
		int error = ::RegCloseKey(ite->first);
		TRACE("%08p(%d)\n", ite->first, error);
	}
	TRACE("------------\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
	InstallRegistryAPIHook(); // ちゃんと開放できているかチェックするため

	spplib::registry reg;

//	(reg/"*"/"shell"/"binedit"/"command");
//	reg.get("*").get("shell").get("binedit").get("command");

	TCHAR buff_str[256];
	DWORD buff_dword;

#define RESET	\
	::memset(buff_str, 0, sizeof(buff_str)),	\
	buff_dword = 0

#define TEST(code, fmt, buff)	\
	{	\
		RESET;	\
		bool result = code;	\
		if( *fmt ) {	\
			TRACE("%s=" fmt "(%s), %d\n", #code, buff, result ? "true " : "false", reg.get_last_error());	\
		} else {	\
			TRACE("%s=(%s), %d\n", #code, result ? "true " : "false", reg.get_last_error());	\
		}	\
	}

	reg.select("HKEY_CLASSES_ROOT\\Paint.Picture", KEY_READ|KEY_WRITE);

	TEST( reg.get("", buff_str, 255),					"%s",		buff_str );
	TEST( reg.get("BrowserFlags", buff_dword),			"0x%04X",	buff_dword );
	TEST( reg.get("FriendlyTypeName", buff_str, 255),	"%s",		buff_str );
	TEST( reg.get("FriendlyTypeNameX", buff_str, 255),	"%s",		buff_str );
	TEST( reg.get("FriendlyTypeNameY", buff_str, 255),	"%s",		buff_str );
	TEST( reg.set("FriendlyTypeNameX", "test"),			"",			buff_str );
	TEST( reg.get("FriendlyTypeNameX", buff_str, 255),	"%s",		buff_str );
	TEST( reg.get("FriendlyTypeNameY", buff_dword),		"%d",		buff_dword );
	TEST( reg.set("FriendlyTypeNameY", 123),			"",			buff_dword );
	TEST( reg.get("FriendlyTypeNameY", buff_dword),		"%d",		buff_dword );

	reg.select("CLSID", KEY_READ|KEY_WRITE);

	TEST( reg.get("", buff_str, 255),					"%s",		buff_str );

//	reg.select("..\\shell\\open", KEY_READ|KEY_WRITE);

//	TEST( reg.get("MuiVerb", buff_str, 255),			"%s",		buff_str );

	reg.select("HKEY_CLASSES_ROOT\\Paint.Picture", KEY_READ|KEY_WRITE);
//	reg.remove("FriendlyTypeNameY");

	reg.select("HKEY_CLASSES_ROOT\\Paint.Picture\\shell2", KEY_READ|KEY_WRITE);
	GUID guid = {0x01234567,0x89AB,0xCDEF,{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}};
	reg.select(guid, KEY_READ|KEY_WRITE);

	reg.select("HKEY_CLASSES_ROOT\\Paint.Picture\\shell3", KEY_READ, true);
	TEST( reg.get("aaa", buff_str, 255),				"%s",		buff_str );

	DWORD dwDebugLog = 0;
	reg.close();
	reg.select(_T("HKCU\\Software\\sharkpp\\Lhaplus_x64_shell_extension"), KEY_READ, true);
	reg.get(_T("DebugLog"), dwDebugLog);

//	Sleep(5000);

	reg.select("HKEY_CLASSES_ROOT\\Paint.Picture");
	TRACE(">>\n");
	reg.erase("HKEY_CLASSES_ROOT\\Paint.Picture\\shell2");
//	reg.erase("shell2");
	TRACE("<<\n");
//	reg.select("shell2", KEY_READ|KEY_WRITE);
//	reg.select("..", KEY_READ|KEY_WRITE);

	reg.remove("FriendlyTypeNameX");
	reg.remove("FriendlyTypeNameY");

	reg.close();

	UninstallAndReportRegistryAPIHook(); // ちゃんと開放できているかチェックするため

	return 0;
}

