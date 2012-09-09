// debug.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>

#include <xbyak/xbyak.h>
#include <spplib/thunk.hpp>

typedef struct TXX {
	int x;
	int y;
} TXX;

class test1
{
	int m_x;
public:
	test1(int x)
		: m_x(x)
	{
		TRACE("%s : %d\n", __FUNCSIG__,x);
	}
	void run0v() {
		TRACE("%s : %d\n", __FUNCSIG__,m_x);
	}
	int run0i() {
		TRACE("%s : %d\n", __FUNCSIG__,m_x);
		return 0;
	}
	void run1v(int x) {
		TRACE("%s : %d,%d\n", __FUNCSIG__,x,m_x);
	}
	int run1i(int x) {
		TRACE("%s : %d,%d\n", __FUNCSIG__,x,m_x);
		return x;
	}
	int run1t(TXX x) {
		TRACE("%s : {%d,%d},%d\n", __FUNCSIG__,x.x,x.y,m_x);
		return x.y;
	}
	int run1ct(const TXX& x) {
		TRACE("%s : {%d,%d},%d\n", __FUNCSIG__,x.x,x.y,m_x);
		return x.y;
	}
};

int test(int a)
{
	TRACE("%s : %d\n", __FUNCSIG__, a);
	return a * 2;
}

__forceinline
int /*__stdcall*/ test2(int a)
{
	TRACE("%s : %d\n", __FUNCSIG__, a);
	return a * 2;
}

int __stdcall test3(int a)
{
	TRACE("%s : %d\n", __FUNCSIG__, a);
	return a * 3;
}

int __fastcall test4(int a)
{
	TRACE("%s : %d\n", __FUNCSIG__, a);
	return a * 4;
}

//template<typename T>
//class foo
//{
//public:
//	static T m_x;
//	foo(T x){m_x=x;}
//	static void fuga(T x){TRACE("%d\n", x);}
//};
//template<typename T>
//T foo<T>::m_x = T();

int _tmain(int argc, _TCHAR* argv[])
{
	SET_TRACE_OUTPUT_DBGOUT();

	test1 v1(987);

	spplib::thunk t0v(&v1, &test1::run0v);
	spplib::thunk t0i(&v1, &test1::run0i);
	spplib::thunk t1v(&v1, &test1::run1v);
	spplib::thunk t1i(&v1, &test1::run1i);
	spplib::thunk t1t(&v1, &test1::run1t);
	spplib::thunk t1ct(&v1, &test1::run1ct);

	v1.run1v(123);
	v1.run1v(123);

	TXX z={111,222};
	v1.run1t(z);

	void(*p0v)()    = (void(*)()   )t0v.get_code();
	int (*p0i)()    = (int (*)()   )t0i.get_code();
	void(*p1v)(int) = (void(*)(int))t1v.get_code();
	int (*p1i)(int) = (int (*)(int))t1i.get_code();
	int (*p1t)(TXX) = (int (*)(TXX))t1t.get_code();
	int (*p1ct)(const TXX&) = (int (*)(const TXX&))t1ct.get_code();

	int r;

	    p0v();

	r = p0i();
	TRACE("r=%d\n",r);

	    p1v(123);

	r = p1i(123);
	TRACE("r=%d\n",r);

	TXX x ={111,222};
	r = p1t(x);
	TRACE("r=%d\n",r);

	r = p1ct(x);
	TRACE("r=%d\n",r);

	TRACE("----------- code injection test -----------\n");

	int (*test_)(int) = (int(*)(int))t1i.injection_code(test);
	r = test(5);
	TRACE("test() r=%d\n",r);
	r = test_(5);
	TRACE("test_() r=%d\n",r);

	int (*test2_)(int) = (int(*)(int))t1i.injection_code(test2);
	r = test2(10);
	TRACE("test() r=%d\n",r);
	r = test2_(10);
	TRACE("test2_() r=%d\n",r);

	int (__stdcall *test3_)(int) = (int(__stdcall *)(int))t1i.injection_code(test3, spplib::thunk::CALL_STDCALL);
	r = test3(10);
	TRACE("test3() r=%d\n",r);
	r = test3_(10);
	TRACE("test3_() r=%d\n",r);

	int (__fastcall *test4_)(int) = (int(__fastcall *)(int))t1i.injection_code(test4, spplib::thunk::CALL_FASTCALL);
	r = test4(10);
	TRACE("test4() r=%d\n",r);
	r = test4_(10);
	TRACE("test4_() r=%d\n",r);

	TRACE("----------- code uninjection test -----------\n");

	t1i.uninjection_code(test, test_);
	r = test(5);
	TRACE("test() r=%d\n",r);
	r = test_(5);
	TRACE("test_() r=%d\n",r);

	t1i.uninjection_code(test2, test2_);
	r = test2(10);
	TRACE("test2() r=%d\n",r);
	r = test2_(10);
	TRACE("test2_() r=%d\n",r);

	//t1i.uninjection_code(test3, test3_);
	//r = test3(10);
	//TRACE("test3() r=%d\n",r);
	//r = test3_(10);
	//TRACE("test3_() r=%d\n",r);

	//t1i.uninjection_code(test4, test4_);
	//r = test4(10);
	//TRACE("test4() r=%d\n",r);
	//r = test4_(10);
	//TRACE("test4_() r=%d\n",r);

	return 0;
}

