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
		template<typename T, typename R, typename A1>
		struct static_invoker {
			typedef static_invoker<T,R,A1> self_;
			static void* this_ptr;
			static R (T::*func_ptr)(A1);
			static R invoke(A1 a1) {
				R (T::*p)(A1) = self_::func_ptr;
				return ((reinterpret_cast<T*>(self_::this_ptr))->*p)(a1);
			}
		};
	public:
		void* thunk_ptr;
		void* func_ptr;
		void* func_holder_ptr;
		void* this_holder_ptr;
	public:

	//	template<typename T, typename R>
	//	member_func_thunk(R(T::*func)())
	//		: m_func(*reinterpret_cast<void **>(&func))
	//	{ TRACE("%s\n", __FUNCSIG__); }

		template<typename T, typename R, typename A1>
		fuga(R(T::*func)(A1))
			: thunk_ptr      (reinterpret_cast<void *>(&static_invoker<T,R,A1>::invoke))
			, func_ptr       (*reinterpret_cast<void **>(&func))
			, func_holder_ptr(reinterpret_cast<void *>(&static_invoker<T,R,A1>::func_ptr))
			, this_holder_ptr(&static_invoker<T,R,A1>::this_ptr)
		{
			TRACE("%s\n", __FUNCSIG__);
		}

	//	template<typename T, typename R, typename A1, typename A2>
	//	member_func_thunk(R(T::*func)(A1,A2))
	//		: m_func(*reinterpret_cast<void **>(&func))
	//	{ TRACE("%s\n", __FUNCSIG__); }

	//	template<typename T, typename R, typename A1, typename A2, typename A3>
	//	member_func_thunk(R(T::*func)(A1,A2,A3))
	//		: m_func(*reinterpret_cast<void **>(&func))
	//	{ TRACE("%s\n", __FUNCSIG__); }
	};
	void* m_p;
public:
	hoge(){TRACE("%s\n", __FUNCSIG__);}
	hoge(void* this_, const fuga& f)
	{
		TRACE("%s\n", __FUNCSIG__);
//		for(size_t i = 0; i < f.countof_param(); i++) {
//			push(ptr[esp+f.sizeof_param(i)]);
//		}
//		push(ptr[esp+4]);
		//mov(ecx, reinterpret_cast<intptr_t>(this_));
	//	call(f.get());
	//	jmp(f.get());
		//ret();

		// *thunk = this_ptr;
		mov(dword[f.this_holder_ptr], (DWORD)this_);
		// *thunk = this_ptr;
		mov(dword[f.func_holder_ptr], (DWORD)f.func_ptr);
		// long_jump func
		jmp(f.thunk_ptr);
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

template<typename T, typename R, typename A1>
void* hoge::fuga::static_invoker<T,R,A1>::this_ptr = NULL;

template<typename T, typename R, typename A1>
R (T::* hoge::fuga::static_invoker<T,R,A1>::func_ptr)(A1) = NULL;

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

//	foo<int>::fuga(10);

	test1 v1(987);
//	spplib::thunk_generator t1(&v1, &test1::run);
//	(t1.get_code())(123);

//	hoge t0v(&v1, &test1::run0v);
//	hoge t0i(&v1, &test1::run0i);
//	hoge t1v(&v1, &test1::run1v);
	hoge t1i(&v1, &test1::run1i);
//	hoge t1t(&v1, &test1::run1t);

	v1.run1v(123);
	v1.run1v(123);

	TXX z={111,222};
	v1.run1t(z);

//	void(*p0v)()    = (void(*)()   )t0v.get_code();
//	int (*p0i)()    = (int (*)()   )t0i.get_code();
//	void(*p1v)(int) = (void(*)(int))t1v.get_code();
	int (*p1i)(int) = (int (*)(int))t1i.get_code();
//	int (*p1t)(TXX) = (int (*)(TXX))t1t.get_code();

	int r;

//	    p0v();

//	r = p0i();
//	TRACE("r=%d\n",r);

//	    p1v(123);

	r = p1i(123);
	TRACE("r=%d\n",r);

//	TXX x ={111,222};
//	r = p1t(x);
//	TRACE("r=%d\n",r);

	return 0;
}

