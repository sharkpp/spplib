/**
 * @file  spplib/native/windows/error.hpp
 * @brief ログ出力ライブラリ定義
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2011.
 */

#include <windows.h>
#include <string>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_CC06F61B_C425_4CC1_87D5_60CB5D2447B1_INCLUDE_GUARD
#define SPPLIB_CC06F61B_C425_4CC1_87D5_60CB5D2447B1_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// logger library namespace
 namespace native {
  /// windows namespace
  namespace windows {
   /// error namespace
   namespace error {

inline
#if defined(UNICODE) || defined(_UNICODE)
const std::wstring to_string(DWORD error_id)
#else
const std::string to_string(DWORD error_id)
#endif
{
#if defined(UNICODE) || defined(_UNICODE)
	typedef std::wstring string;
#else
	typedef std::string string;
#endif
	struct scoped_HLOCAL {
		HLOCAL handle;
		scoped_HLOCAL()
			: handle(NULL)
		{}
		~scoped_HLOCAL()
			{ ::LocalFree(handle); }
		void * ptr()
			{ return &handle; }
		const string::value_type * get() const
			{ return reinterpret_cast<const string::value_type *>(handle); }
	} tmp;
	DWORD length;

	length = ::FormatMessage(
	                FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
	                NULL, error_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	                (LPTSTR)tmp.ptr(), 0, NULL);
	if( !length ) {
		return string();
	}
	return string(tmp.get());
}

inline
const std::string to_string()
{
	return to_string(::GetLastError());
}

   } // namespace error
  } // namespace windows
 } // namespace native
} // namespace spplib

#endif // !defined(SPPLIB_CC06F61B_C425_4CC1_87D5_60CB5D2447B1_INCLUDE_GUARD)
