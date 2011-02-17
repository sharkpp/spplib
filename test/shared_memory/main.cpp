// shared_memory.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>
#include <shellapi.h> // ShellExecute
#include <conio.h> // getch
#include "../../spplib/shared_memory.hpp"

#if defined(_MSC_VER) && 1400 <= _MSC_VER // VC++ .NET 2005 以上
#define pause()	_getch()
#else
#define pause()	getch()
#endif

#ifdef _WIN64
static const TCHAR PLATFORM_TEXT[] = _T("x64");
#else
static const TCHAR PLATFORM_TEXT[] = _T("x86");
#endif

static const TCHAR FIRST_TEXT[]  = _T("text from first process");
static const TCHAR SECOND_TEXT[] = _T("text from second process");
//static const TCHAR FIRST_TEXT[]  = _T("1234");
//static const TCHAR SECOND_TEXT[] = _T("5678");

void SecondProcess(LPCTSTR lpszName);

void FirstProcess(LPCTSTR lpszName)
{
	spplib::shared_memory mem(lpszName, 1024);

	LPTSTR ptr = (LPTSTR)mem.lock();
	::lstrcpy(ptr, FIRST_TEXT);
	mem.unlock();

	_tprintf(_T("mem.get_exists_state() = %d\n"), mem.get_exists_state());

	if( (int)::ShellExecute(NULL, _T("open"), _T("shared_memory_.exe"), lpszName, _T("."), SW_SHOW) <= 32 )
	{
		TCHAR exe_path[MAX_PATH +1];
		::GetModuleFileName(NULL, exe_path, MAX_PATH);
		::ShellExecute(NULL, _T("open"), exe_path, lpszName, _T("."), SW_SHOW);
	}

//	pause();

	_tprintf(_T("wait start\n"));

	while( !memcmp(FIRST_TEXT, (LPCTSTR)mem.lock(), sizeof(FIRST_TEXT) ) )
	{
		mem.unlock();
		Sleep(1);
	}

	_tprintf(_T(">>%s\n"), (LPTSTR)mem.lock());
	mem.unlock();

	_tprintf(_T("please hit any key\n"));
	pause();
}

void SecondProcess(LPCTSTR lpszName)
{
	spplib::shared_memory mem(lpszName, 1024);

	_tprintf(_T(">>%s\n"), (LPTSTR)mem.lock());
	mem.unlock();

	_tprintf(_T("mem.get_exists_state() = %d\n"), mem.get_exists_state());

	Sleep(2000);
//	pause();

	LPTSTR ptr = (LPTSTR)mem.lock();
	::lstrcpy(ptr, SECOND_TEXT);
	mem.unlock();

	_tprintf(_T("please hit any key\n"));
	pause();
}

int _tmain(int argc, _TCHAR* argv[])
{
	if( argc < 2 )
	{
		_tprintf(_T("-- first process(%s) ----\n"), PLATFORM_TEXT);
		FirstProcess(_T("test"));
	}
	else
	{
		_tprintf(_T("-- second process(%s) ----\n"), PLATFORM_TEXT);
		SecondProcess(argv[1]);
	}

	return 0;
}

