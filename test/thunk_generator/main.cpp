// debug.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <spplib/thunk_generator.hpp>
#include <xbyak/xbyak.h>

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>

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
	int run1t(const TXX& x) {
		TRACE("%s : {%d,%d},%d\n", __FUNCSIG__,x.x,x.y,m_x);
		return x.y;
	}
};

class hoge : public Xbyak::CodeGenerator
{
	class fuga {
		void* m_func;
		size_t m_param_num;
		size_t m_param_size[32];
	public:
		template<typename T, typename R>                                        fuga(R(T::*func)())         : m_func(*reinterpret_cast<void **>(&func)), m_param_num(0) { TRACE("%s\n", __FUNCSIG__); }
		template<typename T, typename R, typename A1>                           fuga(R(T::*func)(A1))       : m_func(*reinterpret_cast<void **>(&func)), m_param_num(1) { TRACE("%s\n", __FUNCSIG__); m_param_size[0] = sizeof(A1); }
		template<typename T, typename R, typename A1, typename A2>              fuga(R(T::*func)(A1,A2))    : m_func(*reinterpret_cast<void **>(&func)), m_param_num(2) { TRACE("%s\n", __FUNCSIG__); m_param_size[0] = sizeof(A1); m_param_size[1] = sizeof(A2); }
		template<typename T, typename R, typename A1, typename A2, typename A3> fuga(R(T::*func)(A1,A2,A3)) : m_func(*reinterpret_cast<void **>(&func)), m_param_num(3) { TRACE("%s\n", __FUNCSIG__); m_param_size[0] = sizeof(A1); m_param_size[1] = sizeof(A2); m_param_size[2] = sizeof(A3); }
		void* get() const { return m_func; }
		size_t countof_param() const { return m_param_num; }
		size_t sizeof_param(size_t index) const { return m_param_size[index]; }
	};
public:
	hoge(){TRACE("%s\n", __FUNCSIG__);}
	hoge(void* this_, const fuga& f)
	{
		TRACE("%s\n", __FUNCSIG__);
		for(size_t i = 0; i < f.countof_param(); i++) {
			push(ptr[esp+f.sizeof_param(i)]);
		}
//		push(ptr[esp+4]);
        mov(ecx, reinterpret_cast<intptr_t>(this_));
        call(f.get());
        ret();
	}
	~hoge()
	{
	}
	void* get_code()
	{
		TRACE("%s\n", __FUNCSIG__);
		return (void*)getCode();
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	SET_TRACE_OUTPUT_DBGOUT();

	test1 v1(987);
//	spplib::thunk_generator t1(&v1, &test1::run);
//	(t1.get_code())(123);

	hoge t0v(&v1, &test1::run0v);
	hoge t0i(&v1, &test1::run0i);
	hoge t1v(&v1, &test1::run1v);
	hoge t1i(&v1, &test1::run1i);
	hoge t1t(&v1, &test1::run1t);

	v1.run1i(123);
	v1.run1i(123);

	TXX z={111,222};
	v1.run1t(z);

	void(*p0v)()    = (void(*)()   )t0v.get_code();
	int (*p0i)()    = (int (*)()   )t0i.get_code();
	void(*p1v)(int) = (void(*)(int))t1v.get_code();
	int (*p1i)(int) = (int (*)(int))t1i.get_code();
	int (*p1t)(TXX) = (int (*)(TXX))t1t.get_code();

	int r;
	    p0v();
	r = p0i(); TRACE("r=%d\n",r);
	    p1v(123);
	r = p1i(123); TRACE("r=%d\n",r);
	TXX x ={111,222};
	r = p1t(x); TRACE("r=%d\n",r);

	return 0;
}

