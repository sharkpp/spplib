/**
 * @file  spplib/shared_memory.hpp
 * @brief 共有メモリオブジェクトクラス定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2009.
 */

#include <windows.h>

#ifndef SPPLIB_A2602BEB_BC27_40A4_AE17_A1CAC265DDED_INCLUDE_GUARD
#define SPPLIB_A2602BEB_BC27_40A4_AE17_A1CAC265DDED_INCLUDE_GUARD

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

/// sharkpp class library namespace
namespace spplib {

/**
 * @brief 共有メモリオブジェクト
 * @code
 *  spplib::shared_memory mem("test", 1024);
 *  LPTSTR ptr = (LPTSTR)mem.lock();
 *  ::lstrcpy(ptr, "1234");
 *  mem.unlock();
 * @endcode
 */
class shared_memory
{
private:

	HANDLE		m_handle;			///< 共有メモリのハンドル
	HANDLE		m_lock;				///< ロックオブジェクトのハンドル
	int			m_last_error;		///< 最後に発生したエラー状態
	DWORD		m_allocate_size;	///< 共有メモリの確保サイズ
	void *		m_locked_ptr;		///< ロックされているメモリ領域
	bool		m_already_exists;	///< すでに共有メモリが存在していたか

public:

	/** 
	 * @brief デフォルトコンストラクタ
	 */
	shared_memory()
		: m_handle(NULL)
		, m_lock(NULL)
		, m_last_error(ERROR_SUCCESS)
		, m_allocate_size(0)
		, m_locked_ptr(NULL)
		, m_already_exists(false)
	{
	}

	/** 
	 * @brief コンストラクタ
	 * @param[in] name          共有メモリ名
	 * @param[in] size          共有メモリサイズ
	 * @param[in] initial_value 初期値、デフォルトは 0x00 で埋められます
	 */
	shared_memory(LPCTSTR name, DWORD size, const void * initial_value = NULL)
		: m_handle(NULL)
		, m_lock(NULL)
		, m_last_error(ERROR_SUCCESS)
		, m_allocate_size(0)
		, m_locked_ptr(NULL)
		, m_already_exists(false)
	{
		create(name, size, initial_value);
	}

	/** 
	 * @brief デストラクタ
	 */
	~shared_memory()
	{
		close();
	}

	/** 
	 * @brief  最後に発生したエラー状態を取得
	 * @retval ERROR_SUCCESS エラーが発生していないことを示します
	 * @retval その他        その他のエラーが起こったことを示します
	 */
	int get_last_error() const
	{
		return m_last_error;
	}

	/** 
	 * @brief  生成時に共有メモリが存在したか？
	 * @retval true  すでに同じ名前の共有メモリは存在していた
	 * @retval false 存在していなかったので新たに作成された
	 */
	bool get_exists_state() const
	{
		return m_already_exists;
	}

	/** 
	 * @brief 共有メモリを作成
	 * @param[in] name          共有メモリ名
	 * @param[in] size          共有メモリサイズ
	 * @param[in] initial_value 初期値、デフォルトは 0x00 で埋められます
	 * @retval    true  成功
	 * @retval    false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool create(LPCTSTR name, DWORD size, const void * initial_value = NULL)
	{
		m_last_error = ERROR_SUCCESS;

		close();

		// ミューテックス用の名前を作成
		DWORD name_len = (DWORD)_tcslen(name);
		LPTSTR name_mutex = new TCHAR[name_len + 1 +1];
		::memcpy(name_mutex, name, sizeof(TCHAR) * name_len);
		name_mutex[name_len +0] = _T('`');
		name_mutex[name_len +1] = _T('\0');

		m_lock =
			::CreateMutex(
					NULL, TRUE, name_mutex
				);
		if( !m_lock ) {
			delete [] name_mutex;
			m_last_error = ::GetLastError();
			close();
			return false;
		}
		delete [] name_mutex;

		m_handle =
			::CreateFileMapping(
					INVALID_HANDLE_VALUE, NULL,
					PAGE_READWRITE,
					0, size, name
				);
		if( !m_handle ) {
			m_last_error = ::GetLastError();
			close();
			return false;
		}

		m_allocate_size = size;

		// 未初期化の場合は指定されたデータを使いメモリ領域を初期化する
		m_already_exists =
			ERROR_ALREADY_EXISTS == GetLastError();
		if( !m_already_exists )
		{
			void * ptr = lock();
			if( ptr ) {
				if( initial_value ) {
					::memcpy(ptr, initial_value, size);
				} else {
					::memset(ptr, 0, size);
				}
				unlock();
			}
		}

		::ReleaseMutex(m_lock);

		return true;
	}

	/** 
	 * @brief 共有メモリを破棄
	 * @retval    true  成功
	 * @retval    false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool close()
	{
		m_last_error = ERROR_SUCCESS;

		unlock();

		if( m_handle ) {
			if( !::CloseHandle(m_handle) ) {
				m_last_error = ::GetLastError();
			}
			m_handle = NULL;
		}
		if( m_lock ) {
			if( !::CloseHandle(m_lock) ) {
				m_last_error = ::GetLastError();
			}
			m_lock = NULL;
		}

		m_already_exists = false;

		return ERROR_SUCCESS == m_last_error;
	}

	/** 
	 * @brief 共有メモリをロックしメモリ領域のポインタを取得
	 * @retval 非NULL 成功
	 * @retval NULL   失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	void * lock()
	{
		unlock();

		::WaitForSingleObject(m_lock, INFINITE);

		m_locked_ptr =
			::MapViewOfFileEx(
					m_handle, FILE_MAP_ALL_ACCESS,
					0, 0, m_allocate_size, NULL
				);
		if( !m_locked_ptr ) {
			m_last_error = ::GetLastError();
		}
		return m_locked_ptr;
	}

	/** 
	 * @brief 共有メモリのロックを解除
	 * @retval true  成功
	 * @retval false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool unlock()
	{
		if( !m_locked_ptr ) {
			return false;
		}

		if( !::UnmapViewOfFile(m_locked_ptr) ) {
			m_last_error = ::GetLastError();
		}
		m_locked_ptr = NULL;

		::ReleaseMutex(m_lock);

		return true;
	}
};

} // namespace spplib

#endif // !defined(SPPLIB_A2602BEB_BC27_40A4_AE17_A1CAC265DDED_INCLUDE_GUARD)
