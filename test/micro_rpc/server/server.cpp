// server.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>

#include <spplib/micro_rpc.hpp>

//--------------------------------------------------------------------

class server
	: public spplib::micro_rpc::server
{
	typedef enum {
		FUNC_TEST1 = FUNC_USER_BASE,
		FUNC_TEST2,
		FUNC_QUIT,
	};

public:

	server(const TCHAR * shared_name, int size)
		: spplib::micro_rpc::server(shared_name, size)
	{
		define_function(FUNC_TEST1, &server::test1);
		define_function(FUNC_TEST2, &server::test2);
		define_function(FUNC_QUIT,  &server::quit);
	}

protected:

	void test1()
	{
		int a, b, r;
		pop(a);
		pop(b);
		r = a + b;
		push(r);
	}

	void test2()
	{
		int a, b, r;
		pop(a);
		pop(b);
		r = a * b;
		push(r);
	}

	void quit()
	{
		abort();
	}

};

int _tmain(int argc, _TCHAR* argv[])
{
	TRACE(_T("%s(%d): %s() %u\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__), ::GetTickCount());

	Sleep(1000);

	TRACE(_T("%s(%d): %s() %u\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__), ::GetTickCount());

	server o(_T("test@@@@"), 1024);

	TRACE(_T("%s(%d): %s() %u\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__), ::GetTickCount());

	while( o.wait() );

	TRACE(_T("%s(%d): %s() %u\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__), ::GetTickCount());

	return 0;
}

