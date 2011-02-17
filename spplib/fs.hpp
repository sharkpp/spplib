/**
 * @file  spplib/fs.hpp
 * @brief �f�B���N�g������֐��Q ��`�E����
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
 * @brief �w�肳�ꂽ���O������(.)�������͐e(..)�������Ńf�B���N�g���������ׂ�
 * @param[in] name  �����Ώۂ̖��O
 * @retval    true  ����(.)�������͐e(..)�������Ńf�B���N�g����
 * @retval    false ����ȊO
 */
inline
bool is_current_or_parent_directory(const TCHAR * name)
{
	return !::_tcscmp(_T("."),  name) ||
	       !::_tcscmp(_T(".."), name);
}

/** 
 * @brief �t�@�C���̍폜
 * @param[in] path  �폜�Ώۂ̃t�@�C��
 * @param[in] force �����I�ɍ폜���邩
 * @retval    true  ��������
 * @retval    false �������s
 * @note force�I�v�V�����͎�ɓǂݎ���p�ɂȂ��Ă���t�@�C�����폜���邽�߂ł��B
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
 * @brief �f�C���N�g���̍폜
 * @param[in] path  �폜�Ώۂ̃f�B���N�g��
 * @param[in] force �����I�ɍ폜���邩
 * @retval    true  ��������
 * @retval    false �������s
 * @note force�I�v�V�����͎�ɓǂݎ���p�ɂȂ��Ă���t�@�C�����܂񂾃f�B���N�g�����폜���邽�߂ł��B
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
 * @brief �f�B���N�g���𐶐�
 * @param[in] path_ �����Ώۂ̃p�X
 * @retval    true  ��������
 * @retval    false �������s
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
 * @brief �t�@�C���̑��݂�����
 * @param[in] path  �Ώۂ̃p�X
 * @retval    true  �t�@�C���͑��݂���
 * @retval    false �t�@�C���͑��݂��Ȃ�
 */
inline
bool exist(const TCHAR * path)
{
	return ::SearchPath(NULL, path, NULL, 0, NULL, NULL) ? true : false;
}

/** 
 * @brief �t�@�C���T�C�Y���擾
 * @param[in] path  �Ώۂ̃p�X
 * @retval    0�ȏ� �t�@�C���T�C�Y
 * @retval    -1    �������s
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
 * @brief �t�@�C���T�C�Y���擾(4GB�ȏ���擾�\)
 * @param[in] path  �Ώۂ̃p�X
 * @retval    0�ȏ� �t�@�C���T�C�Y
 * @retval    -1    �������s
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
// directory_iterator �N���X �g��

/** 
 * @brief �t�@�C���T�C�Y���擾
 * @param[in] ite   �Ώۂ̃p�X������ directory_iterator �N���X�I�u�W�F�N�g
 * @retval    0�ȏ� �t�@�C���T�C�Y
 * @retval    -1    �������s
 */
inline
DWORD get_file_size(const directory_iterator & ite)
{
	return ite.size();
}

/** 
 * @brief �t�@�C���T�C�Y���擾(4GB�ȏ���擾�\)
 * @param[in] ite   �Ώۂ̃p�X������ directory_iterator �N���X�I�u�W�F�N�g
 * @retval    0�ȏ� �t�@�C���T�C�Y
 * @retval    -1    �������s
 */
inline
LONGLONG get_file_size64(const directory_iterator & ite)
{
	return ite.size64();
}

 } // namespace fs
} // namespace spplib

#endif // !defined(SPPLIB_1D70F7AF_6678_4998_A375_7136A9D0408B_INCLUDE_GUARD)
