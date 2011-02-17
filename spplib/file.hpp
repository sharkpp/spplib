#pragma once // include guard for MSVC++

#ifndef INCLUDE_GUARD_SPPLIB_FILE_H
#define INCLUDE_GUARD_SPPLIB_FILE_H

#include <windows.h>
#include <stdio.h>

namespace spplib
{
 namespace file
 {

	//---------------------------------------------------------------
	// ファイルへの書き込み

	inline
	bool get(const char * filename, void ** buff, size_t * size, int offset = 0)
	{
		size_t read_size = 0;
		FILE *	fp = NULL;

		if( !size ) {
			size = &read_size;
		}

		// ファイルオープン
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__) // from ISO working draft but not VC 2005 or defined by VC 2005, but not in draft
		if( 0 != ::fopen_s(&fp, filename, "rb") )
#else
		if( !(fp = ::fopen(filename, "rb")) )
#endif
		{
			return false;
		}

		if( buff ) 
		{
			// サイズ取得
			if( !*size ) {
				::fseek(fp, 0, SEEK_END);
				*size = ::ftell(fp);
				::rewind(fp);
			}

			// メモリ確保
			if( !*buff ) {
				*buff = (void*)new unsigned char[*size];
			}

			if( offset ) {
				::fseek(fp, (long)offset, SEEK_CUR);
			}

			*size = ::fread(*buff, 1, *size, fp);

			::fclose(fp);
		}

		return true;
	}

	template<typename T>
	inline
	bool get(const char * filename, T * buff, size_t * size)
		{ return get(filename, (void**)buff, size); }

	//---------------------------------------------------------------
	// ファイルへの書き込み

	inline
	bool put(const char * filename, const void * data, size_t size, int offset = 0)
	{
		FILE *	fp = NULL;

		// ファイルオープン
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__) // from ISO working draft but not VC 2005 or defined by VC 2005, but not in draft
		if( 0 != (::fopen_s(&fp, filename, "r+b")) )
#else
		if( !(fp = ::fopen(filename, "r+b")) )
#endif
		{
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__) // from ISO working draft but not VC 2005 or defined by VC 2005, but not in draft
			if( 0 != (::fopen_s(&fp, filename, "wb")) )
#else
			if( !(fp = ::fopen(filename, "wb")) )
#endif
			{
				return false;
			}
		}

		if( offset ) {
			::fseek(fp, (long)offset, SEEK_CUR);
		}

		::fwrite(data, 1, size, fp);

		::fclose(fp);

		return true;
	}

 } // namespace file
} // namespace spplib

#endif // !defined(INCLUDE_GUARD_SPPLIB_FILE_H)

//EOF
