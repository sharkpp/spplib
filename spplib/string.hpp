/**
 * @file  spplib/string.hpp
 * @brief 文字列関連操作関数群
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2009-2010.
 */

#include <windows.h>	// CharUpperBuff
#include <stdarg.h>	// va_list, va_start(), va_end()
#include <stdio.h>	// _vscprintf(), _vsntprintf(), _vsnprintf_s()
#include <tchar.h>	// 
#include <string>	// std::string
#include <vector>	// std::vector
#include <deque>	// std::deque

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_CE523A45_2980_468F_89F8_3F6976C97921_INCLUDE_GUARD
#define SPPLIB_CE523A45_2980_468F_89F8_3F6976C97921_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {

// std::string/std::wstring の自動切換え
#ifdef _UNICODE
	typedef std::wstring tstring;
#else
	typedef std::string  tstring;
#endif

 /// string module namespace
 namespace string {

//---------------------------------------------------------------
// 文字列のフォーマット化

inline
spplib::tstring format(const TCHAR *fmt, ...)
{
	va_list args; /* 可変個引数集 */
	spplib::tstring r;
	int len;

	va_start(args, fmt); /* 初期化 */

	len = _vsctprintf(fmt, args);
	r.resize(len, 0);
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__) // Safer C Library
	len = _vsntprintf_s((TCHAR*)r.data(), len + 1, len + 1, fmt, args);
#else
	len = _vsntprintf((TCHAR*)r.data(), len, len, fmt, args);
#endif

	va_end(args); /* 終了 */

	return r;
}

//---------------------------------------------------------------
// NULL文字削除

inline
void erase_null_char(spplib::tstring & s)
{
	// char* を渡されるとなぜかNULL文字がデータに含まれてしまい
	// NULL文字なしの変数との比較に失敗するので削除する
	spplib::tstring::size_type null_pos = s.find((spplib::tstring::value_type)0, 0);
	if( null_pos != spplib::tstring::npos && null_pos != s.size() ) {
		s.erase(null_pos);
	}
}

//---------------------------------------------------------------
// 文字列の小文字化

inline
spplib::tstring tolower(const spplib::tstring & s)
{
	spplib::tstring r = s;
	erase_null_char(r); // NULL文字削除
	::CharLowerBuff(
		  const_cast<TCHAR*>( r.data() )
		, static_cast<DWORD>( r.size() )
		);
	return r;
}

//---------------------------------------------------------------
// 文字列の大文字化

inline
spplib::tstring toupper(const spplib::tstring & s)
{
	spplib::tstring r = s;
	erase_null_char(r); // NULL文字削除
	CharUpperBuff(
		  const_cast<TCHAR*>( r.c_str() )
		, static_cast<DWORD>( r.size() )
		);
	return r;
}

//---------------------------------------------------------------

inline
std::wstring mbtowc(const std::string & s, DWORD dwFlags = 0)
{
	std::vector<std::string::value_type>  src(s.begin(), s.end());
	std::vector<std::wstring::value_type> dst;
	dst.resize( ::MultiByteToWideChar(CP_ACP, dwFlags, &src[0], (int)src.size(), NULL,    0)+1 );
	            ::MultiByteToWideChar(CP_ACP, dwFlags, &src[0], (int)src.size(), &dst[0], (int)dst.size());
	return std::wstring(dst.begin(), dst.end());
}

inline
std::string wctomb(const std::wstring & s, DWORD dwFlags = 0)
{
	std::vector<std::wstring::value_type> src(s.begin(), s.end());
	std::vector<std::string::value_type>  dst;
	dst.resize( ::WideCharToMultiByte(CP_ACP, dwFlags, &src[0], (int)src.size(), NULL,    0,               NULL, NULL) );
	            ::WideCharToMultiByte(CP_ACP, dwFlags, &src[0], (int)src.size(), &dst[0], (int)dst.size(), NULL, NULL);
	return std::string(dst.begin(), dst.end());
}

//---------------------------------------------------------------
// 空白文字の削除

inline
spplib::tstring trim_left(const spplib::tstring & s, const TCHAR * trim_str = _T(" \t\r\n"))
{
	spplib::tstring trim_s = trim_str;
	spplib::tstring r = s;
	spplib::tstring::size_type pos;
	erase_null_char(r); // NULL文字削除
	pos = r.find_first_not_of(trim_s, 0);
	if( spplib::tstring::npos != pos ) {
		r.erase(0, pos);
	}
	return r;
}

inline
spplib::tstring trim_right(const spplib::tstring & s, const TCHAR * trim_str = _T(" \t\r\n"))
{
	spplib::tstring trim_s = trim_str;
	spplib::tstring r = s;
	spplib::tstring::size_type pos;
	erase_null_char(r); // NULL文字削除
	pos = r.find_last_not_of(trim_s);
	if( spplib::tstring::npos != pos &&
		r.size() - 1 != pos) {
		r.erase(pos + 1);
	}
	return r;
}

inline
spplib::tstring trim(const spplib::tstring & s, const TCHAR * trim_str = _T(" \t\r\n"))
{
	return trim_right(trim_left(s, trim_str), trim_str);
}

//---------------------------------------------------------------
// 文字列の置換

inline
spplib::tstring replace(const spplib::tstring & s, const spplib::tstring & from, const spplib::tstring & to)
{
	spplib::tstring r = s;
	for(spplib::tstring::size_type pos = r.find(from, 0);
		spplib::tstring::npos != pos;
		pos = r.find(from, pos + to.size()) )
	{
		r.replace(pos, from.size(), to);
	}
	return r;
}

//---------------------------------------------------------------
// 指定文字数での分割

//inline
//std::deque<spplib::tstring> separate(const spplib::tstring & s, size_t n)
//{
//	std::deque<spplib::tstring> r;
//	spplib::tstring::size_type offset = 0;
//	spplib::tstring::size_type length = s.size();
//	for(; offset < length; offset += n) {
//		r.push_back(s.substr(offset, n));
//	}
//	return r;
//}

template<typename T>
inline
T separate(const typename T::value_type & s, size_t n)
{
	T r;
	typename T::value_type::size_type offset = 0;
	typename T::value_type::size_type length = s.size();
	for(; offset < length; offset += n) {
		r.push_back(s.substr(offset, n));
	}
	return r;
}

//---------------------------------------------------------------
// 文字列による分割

//inline
//std::deque<spplib::tstring> split(const spplib::tstring & s, const TCHAR * split_str)
//{
//	std::deque<spplib::tstring> r;
//	spplib::tstring::size_type pos, last_pos = 0;
//	spplib::tstring split_s = split_str;
//	for(pos = s.find(split_s, 0);
//		spplib::tstring::npos != pos;
//		pos = s.find(split_s, pos + split_s.size()) )
//	{
//		r.push_back(s.substr(last_pos, pos - last_pos));
//		last_pos = pos + split_s.size();
//	}
//	r.push_back(s.substr(last_pos));
//	return r;
//}

template<typename T>
inline
T split(const typename T::value_type & s, const typename T::value_type::pointer split_str)
{
	T r;
	typename T::value_type::size_type pos, last_pos = 0;
	typename T::value_type            split_s = split_str;
	for(pos = s.find(split_s, 0);
		typename T::value_type::npos != pos;
		pos = s.find(split_s, pos + split_s.size()) )
	{
		r.push_back(s.substr(last_pos, pos - last_pos));
		last_pos = pos + split_s.size();
	}
	r.push_back(s.substr(last_pos));
	return r;
}

//---------------------------------------------------------------
// 文字列による結合

template<typename T>
inline
typename T::value_type join(const T & s, const typename T::value_type::pointer join_str)
{
	typename T::value_type r;
	for(typename T::const_iterator
			ite = s.begin(),
			last= s.end();
		ite != last; ++ite)
	{
		r += *ite + join_str;
	}
	return r;
}

//---------------------------------------------------------------
// 指定文字列をn回繰り返した文字列を取得

inline
spplib::tstring repeat(size_t n, const TCHAR * repeat_str = _T(" "))
{
	spplib::tstring r;
	for(size_t i = 0; i != n; i++)
	{
		r += repeat_str;
	}
	return r;
}

 } // namespace string
} // namespace spplib

#endif // !defined(SPPLIB_CE523A45_2980_468F_89F8_3F6976C97921_INCLUDE_GUARD)

//EOF
