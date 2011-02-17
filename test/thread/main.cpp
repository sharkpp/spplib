// thread.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>

#include <spplib/system/event.hpp>
#include <spplib/system/mutex.hpp>
#include <spplib/system/thread.hpp>

spplib::system::mutex g_lock;

unsigned int test()
{
	TRACE(_T("%s() begin\n"), _T(__FUNCTION__));
	g_lock.lock();
	Sleep(5000);
	g_lock.unlock();
	TRACE(_T("%s() end\n"), _T(__FUNCTION__));
	return 0;
}

class test2 : public spplib::system::thread
{
private:
	virtual unsigned int procedure()
	{
		TRACE(_T("%s() begin\n"), _T(__FUNCTION__));
		Sleep(10);
		g_lock.lock();
		g_lock.unlock();
		TRACE(_T("%s() end\n"), _T(__FUNCTION__));
		return 0;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace spplib::system;

	event e = true;

	TRACE(_T("e.signaled() = %s\n"), e.signaled() ? _T("true") : _T("false"));
	TRACE(_T("e.set()      = %s\n"), e.set()      ? _T("true") : _T("false"));
	TRACE(_T("e.signaled() = %s\n"), e.signaled() ? _T("true") : _T("false"));
	TRACE(_T("e.reset()    = %s\n"), e.reset()    ? _T("true") : _T("false"));
	TRACE(_T("e.signaled() = %s\n"), e.signaled() ? _T("true") : _T("false"));

	//unsigned char tmp[16] = {0};
	//LONG *pl = (LONG*)(tmp + 2);
	//LONG & l = *pl;
	//InterlockedIncrement(&l);

	thread a( test );
	test2  b;

	a.run();
	b.run();

	a.wait();
	b.wait();

	return 0;
}

