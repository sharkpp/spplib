/**
 * @file  spplib/fw/resource.hpp
 * @brief リソース関連クラス定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include "core.hpp"
#include <string>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_35A044E5_AE2B_4AEA_99DE_214FD33F709E_INCLUDE_GUARD
#define SPPLIB_35A044E5_AE2B_4AEA_99DE_214FD33F709E_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// framework namespace
 namespace fw {
  /// resource namespace
  namespace resource {

template<typename T>
class basic_string : public T
{

	void load_string(UINT id, HINSTANCE instance);

public:

	basic_string(UINT id)
	{
		load_string(id, get_instance_handle());
	}

	basic_string(UINT id, HINSTANCE module_instance)
	{
		load_string(id, module_instance);
	}


};

template<>
inline
void basic_string<std::string>::load_string(UINT id, HINSTANCE instance)
{
	DWORD size_of_string_table = 
		::SizeofResource(instance,
			::FindResourceA(instance, MAKEINTRESOURCEA(id / 16 + 1), (LPSTR)RT_STRING));
	if( size_of_string_table )
	{
		CHAR *pszBuff = new CHAR[size_of_string_table];
		DWORD length = (DWORD)::LoadStringA(instance, id, pszBuff, size_of_string_table);
		assign(pszBuff, pszBuff + length);
		delete [] pszBuff;
	}
}

template<>
inline
void basic_string<std::wstring>::load_string(UINT id, HINSTANCE instance)
{
	DWORD size_of_string_table = 
		::SizeofResource(instance,
			::FindResourceW(instance, MAKEINTRESOURCEW(id / 16 + 1), (LPWSTR)RT_STRING));
	if( size_of_string_table )
	{
		WCHAR *pszBuff = new WCHAR[size_of_string_table];
		DWORD length = (DWORD)::LoadStringW(instance, id, pszBuff, size_of_string_table);
		assign(pszBuff, pszBuff + length);
		delete [] pszBuff;
	}
}

typedef basic_string< std::string  > string;
typedef basic_string< std::wstring > wstring;

#ifdef _UNICODE
typedef wstring tstring;
#else
typedef string  tstring;
#endif

  } // resource fw
 } // namespace fw
} // namespace spplib

#endif // !defined(SPPLIB_35A044E5_AE2B_4AEA_99DE_214FD33F709E_INCLUDE_GUARD)
