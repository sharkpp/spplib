// client.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>

#include <spplib/micro_rpc.hpp>

//--------------------------------------------------------------------

class client
	: public spplib::micro_rpc::client
{
	typedef enum {
		FUNC_TEST1 = FUNC_USER_BASE,
		FUNC_TEST2,
		FUNC_QUIT,
	};

public:

	client(const TCHAR * shared_name, int size)
		: spplib::micro_rpc::client(shared_name, size)
	{
	}

	int test1(int a, int b)
	{
		lock();
		push(FUNC_TEST1);
		push(a);
		push(b);
		call();
		return ret<int>();
	}

	void test2(int a, int b, int *r)
	{
		lock();
		push(FUNC_TEST2);
		push(a);
		push(b);
		call();
		pop(*r);
	}

	void quit()
	{
		lock();
		push(FUNC_QUIT);
		call();
	}

};

int _tmain(int argc, _TCHAR* argv[])
{
	//TRACE(_T("%s(%d): %s() %u\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__), ::GetTickCount());

	//Sleep(1000);

	TRACE(_T("%s(%d): %s() %u\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__), ::GetTickCount());

	client *o = NULL;
	for(;; Sleep(100)) {
		try {
			o = new client(_T("test@@@@"), 16*1024*1024);
		} catch(...) {
			continue;
		}
		break;
	}

	TRACE(_T("test1(2, 3) = %d\n"), o->test1(2, 3));
	int n;
	o->test2(2, 3, &n);
	TRACE(_T("test2(2, 3, &n) = %d\n"), n);

	o->quit();

	TRACE(_T("%s(%d): %s() %u\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__), ::GetTickCount());

	delete o;

	return 0;
}

