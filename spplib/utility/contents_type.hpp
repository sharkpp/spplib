/**
 * @file  spplib/utility/contents_type.hpp
 * @brief コンテンツ種別関連関数群
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2009-2010.
 */

#define SPPLIB_REGISTRY_ENABLE_INLINE

#include <urlmon.h>
#include <spplib/string.hpp>
//#include <spplib/registry.hpp>

#pragma comment(lib, "urlmon.lib")

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_D3F149E7_5A25_4B3A_8D56_49B5E0D59174_INCLUDE_GUARD
#define SPPLIB_D3F149E7_5A25_4B3A_8D56_49B5E0D59174_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// utility namespace
 namespace utility {

/// コンテンツ種別の取得
inline
spplib::tstring get_contents_type(const spplib::tstring & url, const void * data, size_t size)
{
	LPWSTR mime_buff_ptr = NULL;
	spplib::tstring r;

	if( !data || !size )
	{
		//spplib::tstring::size_type dot_pos = url.rfind(_T("."), url.size());
		//spplib::tstring ext;
		//if( spplib::tstring::npos != dot_pos ) {
		//	ext = url.substr(dot_pos);
		//}
		//// レジストリから取得
		//spplib::registry reg;
		//reg.select(spplib::tstring("HKCR\\" + ext).c_str());
		//reg.get("Content Type", r);
		//reg.close(); // optional
		return r.empty() ? CFSTR_MIME_RAWDATA : r;
	}

#if !defined(_UNICODE)
	std::wstring url_ = string::mbtowc(url);
	HRESULT hr = ::FindMimeFromData(NULL, url_.c_str(), (void*)data, size, NULL, 0, &mime_buff_ptr, 0);
	r          = string::wctomb(mime_buff_ptr);
#else
	std::wstring fname_ = fname;
	HRESULT hr = ::FindMimeFromData(NULL, url.c_str(), (void*)data, size, NULL, 0, &mime_buff_ptr, 0);
	r          = mime_buff_ptr;
#endif

	::CoTaskMemFree(mime_buff_ptr);

	// 補正
	if( _T("application/x-msdownload") == r )
	{
		spplib::tstring::size_type dot_pos = url.rfind(_T("."), url.size());
		if( spplib::tstring::npos == dot_pos ) {
			r = CFSTR_MIME_RAWDATA;
		} else {
			r = _T("application/x-") + url.substr(dot_pos + 1);
		}
	}

	return r;
}

 } // namespace utility
} // namespace spplib

#endif // !defined(SPPLIB_D3F149E7_5A25_4B3A_8D56_49B5E0D59174_INCLUDE_GUARD)

//EOF
