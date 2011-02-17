/**
 * @file  spplib/fs/directory_iterator.hpp
 * @brief ディレクトリ列挙クラス 定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <windows.h>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_462440FA_E6FA_4860_9F32_3CA0192BB758_INCLUDE_GUARD
#define SPPLIB_462440FA_E6FA_4860_9F32_3CA0192BB758_INCLUDE_GUARD

/// sharkpp class library namespace
namespace spplib {
 /// filesysyem namespace
 namespace fs {

/**
 * @brief ディレクトリ列挙クラス
 * @code
 *   for(spplib::fs::directory_iterator ite("*.*"), end; 
 *       ite != end; ++ite) {
 *     printf("%s %s\r\n", ite.is_dir() ? "d" : "f", ite.filename());
 *   }
 * @endcode
 */
class directory_iterator
{
#if defined(_UNICODE)
	static const size_t MAX_PATH_LEN = 32767;
#else
	static const size_t MAX_PATH_LEN = MAX_PATH * 2;
#endif

	WIN32_FIND_DATA	m_fd;
	HANDLE			m_handle;
	bool			m_next_exist;
	mutable bool	m_full_path_valid;
	mutable TCHAR*	m_full_path;
	TCHAR*			m_path;
	TCHAR*			m_dirname;

	directory_iterator(const directory_iterator & );
	directory_iterator & operator = (const directory_iterator & );

public:

	/// コンストラクタ
	directory_iterator();

	/** 
	 * @brief コンストラクタ
	 * @param[in] path 対象のパス
	 */
	directory_iterator(const TCHAR * path);

	/// デストラクタ
	~directory_iterator();

	/** 
	 * @brief ディレクトリ内の次のエントリへ移動
	 * @return 次のエントリへ移動後のこのオブジェクト自体を返す
	 */
	directory_iterator & operator ++();

	/** 
	 * @brief 指定したインスタンスと一致していないかの比較
	 * @param[in] rhs   比較対照のインスタンス
	 * @retval    true  不一致
	 * @retval    false 一致
	 * @note 空のオブジェクトとの比較にしか意味を持ちません。
	 */
	bool operator != (const directory_iterator & rhs) const;

	/** 
	 * @brief 指定したパスを検索
	 * @param[in] path  対象のパス
	 * @retval    true  処理成功
	 * @retval    false 処理失敗
	 */
	bool find(const TCHAR * path);

	/** 
	 * @brief ディレクトリ内の先頭項目へ戻る
	 * @retval    true  処理成功
	 * @retval    false 処理失敗
	 */
	bool rewind();

	/** 
	 * @brief 現在のエントリがディレクトリかどうかを調べる
	 * @retval    true  現在のエントリがディレクトリ
	 * @retval    false 現在のエントリがディレクトリ以外
	 */
	bool is_dir() const;

	/** 
	 * @brief 現在のエントリのファイル名を取得
	 * @return 現在のエントリのファイル名
	 */
	const TCHAR * filename() const;

	/** 
	 * @brief 現在のエントリのフルパスを取得
	 * @return 現在のエントリのフルパス
	 */
	const TCHAR * fullpath() const;

	/** 
	 * @brief 現在のエントリのサイズを取得
	 * @retval  0以上 ファイルサイズ
	 * @retval -1     ファイルサイズの取得に失敗。サイズが4GB以上など。
	 */
	DWORD size() const;

	/** 
	 * @brief 現在のエントリのサイズを取得
	 * @retval  0以上 ファイルサイズ
	 * @retval -1     ファイルサイズの取得に失敗。サイズが4GB以上など。
	 * @note 4GB以上も取得可能
	 */
	LONGLONG size64() const;

	/** 
	 * @brief 現在のエントリの属性を取得
	 * @return 現在のエントリの属性
	 */
	DWORD attribute() const;

	/** 
	 * @brief 現在のエントリの作成時刻
	 * @return 作成時刻
	 */
	time_t ctime() const;

	/** 
	 * @brief 現在のエントリの更新時刻
	 * @return 更新時刻
	 */
	time_t mtime() const;

	/** 
	 * @brief 現在のエントリのアクセス時刻
	 * @return アクセス時刻
	 */
	time_t atime() const;

};

///////////////////////////////////////////////////////////////////////////////

inline
directory_iterator::directory_iterator()
	: m_handle(INVALID_HANDLE_VALUE)
	, m_next_exist(false)
	, m_full_path_valid(false)
	, m_full_path(new TCHAR[MAX_PATH_LEN + 1])
	, m_path(new TCHAR[MAX_PATH_LEN + 1])
	, m_dirname(new TCHAR[MAX_PATH_LEN + 1])
{
	::memset(m_full_path, 0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
	::memset(m_path,      0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
	::memset(m_dirname,   0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
}

inline
directory_iterator::directory_iterator(const TCHAR * path)
	: m_handle(INVALID_HANDLE_VALUE)
	, m_next_exist(false)
	, m_full_path_valid(false)
	, m_full_path(new TCHAR[MAX_PATH_LEN + 1])
	, m_path(new TCHAR[MAX_PATH_LEN + 1])
	, m_dirname(new TCHAR[MAX_PATH_LEN + 1])
{
	::memset(m_full_path, 0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
	::memset(m_path,      0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
	::memset(m_dirname,   0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));

	find(path);
}

inline
directory_iterator::~directory_iterator()
{
	if( INVALID_HANDLE_VALUE != m_handle )
	{
		::FindClose(m_handle);
	}
	delete [] m_full_path;
	delete [] m_path;
	delete [] m_dirname;
}

inline
directory_iterator & directory_iterator::operator ++()
{
	if( m_next_exist ) {
		m_next_exist = FALSE != ::FindNextFile(m_handle, &m_fd);
		m_full_path_valid = false;
	}
	return *this;
}

bool directory_iterator::operator != (const directory_iterator & rhs) const
{
	return m_next_exist != rhs.m_next_exist;
}

inline
bool directory_iterator::find(const TCHAR * path)
{
	// リセット
	if( INVALID_HANDLE_VALUE != m_handle )
	{
		::FindClose(m_handle);
	}
	::memset(m_full_path, 0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
	::memset(m_path,      0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
	::memset(m_dirname,   0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));

	// 検索パスを保存
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__) // Safer C Library
	if( m_path != path ) {
		_tcsncpy_s(m_path, MAX_PATH_LEN, path, MAX_PATH_LEN);
	}
	_tcsncpy_s(m_dirname, MAX_PATH_LEN, path, MAX_PATH_LEN);
#else
	if( m_path != path ) {
		_tcsncpy(m_path, path, MAX_PATH_LEN);
	}
	_tcsncpy(m_dirname, path, MAX_PATH_LEN);
#endif

	// ディレクトリを取得
#if defined(_UNICODE)
	TCHAR *p = _tcsrchr(m_dirname, '\\');
	if( !p ) { p = m_dirname; }
	else     { ++p; }
#else
	TCHAR *p = m_dirname; // '\\' 終端
	for(TCHAR *pa = m_dirname; *pa; pa = ::CharNext(pa) ) {
		if( '\\' == *pa ) { p = CharNext(pa); }
	}
#endif
	*p = '\0';
	if( p == m_dirname ) {
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__) // Safer C Library
		_tcsncpy_s(m_dirname, MAX_PATH_LEN, _T(".\\"), MAX_PATH_LEN);
#else
		_tcsncpy(m_dirname, _T(".\\"), MAX_PATH_LEN);
#endif
	}

	m_handle = ::FindFirstFile(m_path, &m_fd);
	m_next_exist = INVALID_HANDLE_VALUE != m_handle;

	return m_next_exist;
}

inline
bool directory_iterator::rewind()
{
	return find(m_path);
}

inline
bool directory_iterator::is_dir() const
{
	return 0 != (m_fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

inline
const TCHAR * directory_iterator::filename() const
{
	return m_fd.cFileName;
}

inline
const TCHAR * directory_iterator::fullpath() const
{
	if( !m_full_path_valid ) {
		m_full_path_valid = true;
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__) // Safer C Library
		_tcsncpy_s(m_full_path, MAX_PATH_LEN, m_dirname,      MAX_PATH_LEN);
		_tcsncat_s(m_full_path, MAX_PATH_LEN, m_fd.cFileName, MAX_PATH_LEN);
#else
		_tcsncpy(m_full_path, m_dirname,      MAX_PATH_LEN);
		_tcsncat(m_full_path, m_fd.cFileName, MAX_PATH_LEN);
#endif
	}
	return m_full_path;
}

inline
DWORD directory_iterator::size() const
{
	return m_fd.nFileSizeLow;
}

inline
LONGLONG directory_iterator::size64() const
{
	return ((LONGLONG)m_fd.nFileSizeHigh << 32) | m_fd.nFileSizeLow;
}

inline
DWORD directory_iterator::attribute() const
{
	return m_fd.dwFileAttributes;
}

inline
time_t directory_iterator::ctime() const
{
	
	return (time_t)(( ((PLARGE_INTEGER)(&m_fd.ftCreationTime))->QuadPart - 116444736000000000) / 10000000);
}

inline
time_t directory_iterator::mtime() const
{
	return (time_t)(( ((PLARGE_INTEGER)(&m_fd.ftLastWriteTime))->QuadPart - 116444736000000000) / 10000000);
}

inline
time_t directory_iterator::atime() const
{
	return (time_t)(( ((PLARGE_INTEGER)(&m_fd.ftLastAccessTime))->QuadPart - 116444736000000000) / 10000000);
}

 } // namespace directory
} // namespace spplib

#endif // !defined(SPPLIB_462440FA_E6FA_4860_9F32_3CA0192BB758_INCLUDE_GUARD)
