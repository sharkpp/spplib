/**
 * @file  spplib/fw/core.hpp
 * @brief フレームワークコア定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <windows.h>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_F010A24D_31C2_4125_AEC4_D33FE7812A87_INCLUDE_GUARD
#define SPPLIB_F010A24D_31C2_4125_AEC4_D33FE7812A87_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// framework namespace
 namespace fw {

struct instance_handle_holder
{
private:
	friend HINSTANCE initialize_instance_handle(HINSTANCE handle);
	friend HINSTANCE get_instance_handle();
private:
	static HINSTANCE set_instance_handle(HINSTANCE handle)
	{
		static HINSTANCE instance_handle = handle ? handle : ::GetModuleHandle(NULL);
		return instance_handle;
	}
};

inline
static HINSTANCE initialize_instance_handle(HINSTANCE handle)
{
	return instance_handle_holder::set_instance_handle(handle);
}

inline
static HINSTANCE get_instance_handle()
{
	return instance_handle_holder::set_instance_handle(NULL);
}

#pragma warning(pop)

 } // namespace fw
} // namespace spplib

#endif // !defined(SPPLIB_F010A24D_31C2_4125_AEC4_D33FE7812A87_INCLUDE_GUARD)
