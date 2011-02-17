/**
 * @file  spplib/fs.hpp
 * @brief ディレクトリ操作関数群 定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2009-2010.
 */

#include <windows.h>
#include "fs/directory_iterator.hpp"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_1D70F7AF_6678_4998_A375_7136A9D0408B_INCLUDE_GUARD
#define SPPLIB_1D70F7AF_6678_4998_A375_7136A9D0408B_INCLUDE_GUARD

/// sharkpp class library namespace
namespace spplib {
 /// filesystem namespace
 namespace fs {

#if defined(_UNICODE)
static const size_t MAX_PATH_LEN = 32767;
#else
static const size_t MAX_PATH_LEN = MAX_PATH * 2;
#endif

static const TCHAR DIRECTORY_SEPARATOR[] = _T("\\");

/** 
 * @brief 指定された名前が現在(.)もしくは親(..)を示すでディレクトリ名か調べる
 * @param[in] name  検査対象の名前
 * @retval    true  現在(.)もしくは親(..)を示すでディレクトリ名
 * @retval    false それ以外
 */
inline
bool is_current_or_parent_directory(const TCHAR * name)
{
	return !::_tcscmp(_T("."),  name) ||
	       !::_tcscmp(_T(".."), name);
}

/** 
 * @brief ファイルの削除
 * @param[in] path  削除対象のファイル
 * @param[in] force 強制的に削除するか
 * @retval    true  処理成功
 * @retval    false 処理失敗
 * @note forceオプションは主に読み取り専用になっているファイルを削除するためです。
 */
inline
bool remove_file(const TCHAR * path, bool force = true)
{
	if( !path || !*path )
	{
		return false;
	}

	if( !::DeleteFile(path) ) {
		if( !force ) {
			return false;
		} else {
			::SetFileAttributes(path,
				::GetFileAttributes(path) & ~FILE_ATTRIBUTE_READONLY);
			if( !::DeleteFile(path) ) {
				return false;
			}
		}
	}

	return true;
}

/** 
 * @brief デイレクトリの削除
 * @param[in] path  削除対象のディレクトリ
 * @param[in] force 強制的に削除するか
 * @retval    true  処理成功
 * @retval    false 処理失敗
 * @note forceオプションは主に読み取り専用になっているファイルを含んだディレクトリを削除するためです。
 */
inline
bool remove_directory(const TCHAR * path, bool force = true)
{
	bool result = true;

	if( !path || !*path )
	{
		return false;
	}

	TCHAR* mask = new TCHAR[MAX_PATH_LEN +1];

#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__) // Safer C Library
	_tcsncpy_s(mask, MAX_PATH_LEN +1, path, MAX_PATH_LEN);
	_tcscat_s (mask, MAX_PATH_LEN +1, DIRECTORY_SEPARATOR);
	_tcscat_s (mask, MAX_PATH_LEN +1, _T("*.*"));
#else
	_tcsncpy(mask, path, MAX_PATH_LEN);
	_tcscat (mask, DIRECTORY_SEPARATOR);
	_tcscat (mask, _T("*.*"));
#endif

	for(directory_iterator ite(mask), end;
		ite != end; ++ite)
	{
		if( is_current_or_parent_directory( ite.filename() ) )
		{
			continue;
		}
		if( ite.is_dir() ) {
			result &= remove_directory(ite.fullpath(), force);
		} else {
			result &= remove_file(ite.fullpath(), force);
		}
	}

	result &= ::RemoveDirectory(path) ? true : false;

	delete [] mask;

	return result;
}

/** 
 * @brief ディレクトリを生成
 * @param[in] path_ 生成対象のパス
 * @retval    true  処理成功
 * @retval    false 処理失敗
 */
inline
bool create_directory(const TCHAR * path_)
{
	TCHAR path[MAX_PATH_LEN +1] = {0};

#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__) // Safer C Library
	_tcsncpy_s(path, MAX_PATH_LEN +1, path_, MAX_PATH_LEN);
#else
	_tcsncpy(path, path_, MAX_PATH_LEN);
#endif

	TCHAR *p = path;
	for(TCHAR *p_ = p; *p_;)
	{
#if defined(_UNICODE)
		p = _tcschr(p, *DIRECTORY_SEPARATOR);
		if( !p ) { p = path + _tcslen(path); }
		p_ = *p ? p + 1 : p;
#else
		for(; *p && *DIRECTORY_SEPARATOR != *p; p = ::CharNext(p));
		p_ = ::CharNext(p);
#endif
		*p = '\0';

		::CreateDirectory(path, NULL);

		if( p_ != p ) {
			*p = *DIRECTORY_SEPARATOR;
			p  = p_;
		}
	}

	return true;
}

/** 
 * @brief ファイルの存在を検査
 * @param[in] path  対象のパス
 * @retval    true  ファイルは存在する
 * @retval    false ファイルは存在しない
 */
inline
bool exist(const TCHAR * path)
{
	return ::SearchPath(NULL, path, NULL, 0, NULL, NULL) ? true : false;
}

/** 
 * @brief ファイルサイズを取得
 * @param[in] path  対象のパス
 * @retval    0以上 ファイルサイズ
 * @retval    -1    処理失敗
 */
inline
DWORD get_file_size(const TCHAR * path)
{
	HANDLE handle = ::CreateFile(path, 0, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
	                             NULL, OPEN_EXISTING, 0, NULL);
	DWORD size = ~0UL;

	if( INVALID_HANDLE_VALUE != handle )
	{
		size = ::GetFileSize(handle, NULL);
		::CloseHandle(handle);
	}

	return size;
}

/** 
 * @brief ファイルサイズを取得(4GB以上も取得可能)
 * @param[in] path  対象のパス
 * @retval    0以上 ファイルサイズ
 * @retval    -1    処理失敗
 */
inline
LONGLONG get_file_size64(const TCHAR * path)
{
	HANDLE handle = ::CreateFile(path, 0, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
	                             NULL, OPEN_EXISTING, 0, NULL);
	LARGE_INTEGER size = { ~0UL, ~0UL };

	if( INVALID_HANDLE_VALUE != handle )
	{
		size.QuadPart = ::GetFileSizeEx(handle, &size) ? size.QuadPart : -1;
		::CloseHandle(handle);
	}

	return size.QuadPart;
}

///////////////////////////////////////////////////////////////////////////////
// directory_iterator クラス 拡張

/** 
 * @brief ファイルサイズを取得
 * @param[in] ite   対象のパスを示す directory_iterator クラスオブジェクト
 * @retval    0以上 ファイルサイズ
 * @retval    -1    処理失敗
 */
inline
DWORD get_file_size(const directory_iterator & ite)
{
	return ite.size();
}

/** 
 * @brief ファイルサイズを取得(4GB以上も取得可能)
 * @param[in] ite   対象のパスを示す directory_iterator クラスオブジェクト
 * @retval    0以上 ファイルサイズ
 * @retval    -1    処理失敗
 */
inline
LONGLONG get_file_size64(const directory_iterator & ite)
{
	return ite.size64();
}

 } // namespace fs
} // namespace spplib

#endif // !defined(SPPLIB_1D70F7AF_6678_4998_A375_7136A9D0408B_INCLUDE_GUARD)
