/**
 * @file  spplib/utility/fs.hpp
 * @brief メール用ユーティリティ関数 宣言・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <windows.h>
#include <spplib/string.hpp>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_296AA7D7_38C6_4D7E_BF4A_2BB2B21F6DF1_INCLUDE_GUARD
#define SPPLIB_296AA7D7_38C6_4D7E_BF4A_2BB2B21F6DF1_INCLUDE_GUARD

/// sharkpp class library namespace
namespace spplib {
 /// utility namespace
 namespace utility {

inline
spplib::tstring extract_mail_address(const spplib::tstring & s)
{
	spplib::tstring::size_type pos1 = s.find(_T("<"));
	spplib::tstring::size_type pos2 = s.find(_T(">"));
	if( pos1 != spplib::tstring::npos &&
		pos2 != spplib::tstring::npos &&
		pos1 < pos2 )
	{
		return spplib::tstring(s.substr(pos1 + 1, pos2 - pos1 - 1));
	}
	return spplib::string::trim(s);
}

Message Header Extensions

 } // namespace utility
} // namespace spplib

#endif // !defined(SPPLIB_296AA7D7_38C6_4D7E_BF4A_2BB2B21F6DF1_INCLUDE_GUARD)
