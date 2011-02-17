/**
 * @file  spplib/memory.hpp
 * @brief ƒƒ‚ƒŠŠÖ˜AŠÖ”ŒQ
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_B8E12564_1A87_4993_A9E1_90846EFF2F26_INCLUDE_GUARD
#define SPPLIB_B8E12564_1A87_4993_A9E1_90846EFF2F26_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {

 /// memory module namespace
 namespace memory {

template<typename T>
inline T * realloc(T * data, size_t from_size, size_t to_size)
{
	if( to_size < from_size )
	{
		return data;
	}
	T * new_data = new T[to_size];
	::memcpy(new_data, data, from_size);
	delete [] data;
	return new_data;
}

 } // namespace string
} // namespace spplib

#endif // !defined(SPPLIB_B8E12564_1A87_4993_A9E1_90846EFF2F26_INCLUDE_GUARD)

//EOF
