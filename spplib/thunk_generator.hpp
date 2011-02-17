/**
 * @file  spplib/thunk_generator.hpp
 * @brief サンクジェネレータ定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <xbyak/xbyak.h>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_AC7B9E21_2435_4031_AE45_DE16FE059E1B_INCLUDE_GUARD
#define SPPLIB_AC7B9E21_2435_4031_AE45_DE16FE059E1B_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {

// 
template<typename T>
struct thunk_generator : public Xbyak::CodeGenerator {
	static T * thunk;
	thunk_generator(void * this_ptr, void * func)
	{
#ifndef XBYAK64
		// *thunk = this_ptr;
		mov(dword[&thunk], (DWORD)this_ptr);
		// long_jump func
		jmp(func);
#else
		// *thunk = this_ptr;
		mov(rax,        (unsigned long long)&thunk);
		mov(qword[rax], (unsigned long long)this_ptr );
		// long_jump func
		mov(rax,        (unsigned long long)func);
		jmp(rax);
#endif
	}
};

template<typename T>
T * thunk_generator<T>::thunk = NULL;

} // namespace spplib

#endif // !defined(SPPLIB_AC7B9E21_2435_4031_AE45_DE16FE059E1B_INCLUDE_GUARD)
