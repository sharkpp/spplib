// main.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>
#include <shellapi.h> // ShellExecute
#include <conio.h> // getch

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>

#include <spplib/container/shmqueue.hpp>
#include <spplib/container/shmvector.hpp>
#include <spplib/container/shmvqueue.hpp>

#if defined(_MSC_VER) && 1400 <= _MSC_VER // VC++ .NET 2005 以上
#define pause()	_getch()
#else
#define pause()	getch()
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	const TCHAR SHARED_NAME[] = _T("test");

	if( argc < 2 )
	{
		_tprintf(_T("-- first process ----\n"));
		//
		TCHAR exe_path[MAX_PATH +1];
		::GetModuleFileName(NULL, exe_path, MAX_PATH);
//		::ShellExecute(NULL, _T("open"), exe_path, SHARED_NAME, _T("."), SW_SHOW);
	}
	else
	{
		_tprintf(_T("-- second process ----\n"));
		Sleep(1000);
	}

	using namespace spplib::container;

#if 0

//	shmvector<int> v(SHARED_NAME);
//	shmqueue<int> v(SHARED_NAME);
	shmvqueue v(SHARED_NAME);

	_tprintf(_T(">> size = %d\n"), v.size());
	_tprintf(_T(">> pop\n"));
	while( !v.empty() ) {
	//	_tprintf(_T(".back() = %d\n"), v.back());
		_tprintf(_T(".back() = %d\n"), v.back<int>());
		v.pop_back();
	}

	_tprintf(_T(">> push\n"));
	for(int i = 0; i < 10; i++) {
		_tprintf(_T(".push_back(%d)\n"), i);
		v.push_back(i);
	}

	_tprintf(_T(">> size = %d\n"), v.size());

#elif 0

//	shmqueue<int> v(SHARED_NAME);
	shmvqueue v(SHARED_NAME, (4+4)*10);

	_tprintf(_T(">> size = %d\n"), v.size());
	_tprintf(_T(">> pop\n"));
	while( !v.empty() ) {
	//	_tprintf(_T(".front() = %d\n"), v.front());
		_tprintf(_T(".front() = %d\n"), v.back<int>());
		v.pop_back();
//		_tprintf(_T(".front() = %d\n"), v.front<int>());
//		v.pop_front();
	}

	_tprintf(_T(">> push\n"));
	for(int i = 0; i < 10; i++) {
		_tprintf(_T(".push_back(%d)\n"), i);
	//	v.push_front(i);
		v.push_back(i);
	}

	_tprintf(_T(">> size = %d\n"), v.size());

#else

//	shmqueue<int> v(SHARED_NAME);
	shmvqueue v(SHARED_NAME, (4+4)*10);

	for(int i = 0; i < 100; ) {
		_tprintf(_T(">> size = %d\n"), v.size());
		_tprintf(_T(">> push\n"));
		for(int j = 0; j < 6; j++, i++) {
			_tprintf(_T(".push_back(%d)\n"), i);
			v.push_back(i);
		}
		_tprintf(_T(">> size = %d\n"), v.size());
		_tprintf(_T(">> pop\n"));
		for(int j = 0; j < 6; j++) {
			if( j & 1 ) {
				_tprintf(_T(".back() = %d\n"), v.back<int>());
				v.pop_back();
			} else {
				_tprintf(_T(".front() = %d\n"), v.front<int>());
				v.pop_front();
			}
		}
		_tprintf(_T(">> size = %d\n"), v.size());
		_tprintf(_T("----------------\n"));
	}

#endif

	pause();

	return 0;
}

