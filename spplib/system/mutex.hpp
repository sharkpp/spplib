/**
 * @file  spplib/system/mutex.hpp
 * @brief ミューテックスクラス
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <windows.h>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_F06BB420_6978_4203_8BF8_830598B8CF9A_INCLUDE_GUARD
#define SPPLIB_F06BB420_6978_4203_8BF8_830598B8CF9A_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// system library namespace
 namespace system {

/**
 * @brief ミューテックスクラス
 * @code
 *  spplib::system::mutex mtx;
 *  printf("mtx.lock()   = %s\n", mtx.lock()   ? "true" : "false");
 *  for(int i = 0; i < 100000; i++);
 *  printf("mtx.unlock() = %s\n", mtx.unlock() ? "true" : "false");
 * @endcode
 */
class mutex
{
	LONG* m_lock;

	mutex(const mutex &);
	mutex operator = (const mutex &);

public:

	/**
	 * @brief     コンストラクタ
	 * @param[in] default_state 初期値
	 */
	mutex(bool default_state = false)
		: m_lock(static_cast<LONG*>(::_aligned_malloc(sizeof(LONG), 32)))
	{
		*m_lock = default_state ? 1 : 0;
	}

	/// デストラクタ
	~mutex()
	{
		::_aligned_free(m_lock);
	}

	/**
	 * @brief  ロック処理
	 * @retval true  処理の正常終了(ロックに成功)
	 */
	bool lock()
	{
		while( !try_lock() );
		return true;
	}

	/**
	 * @brief  ロック処理(失敗付き)
	 * @retval true  処理の正常終了(ロックに成功)
	 * @retval false 処理が失敗した(ロックに失敗)
	 */
	bool try_lock()
	{
		return 0 == ::InterlockedCompareExchange(m_lock, 1, 0);
	}

	/**
	 * @brief  ロック解除処理
	 * @retval true  処理の正常終了
	 * @retval false 処理が失敗した
	 */
	bool unlock()
	{
		return 1 == ::InterlockedCompareExchange(m_lock, 0, 1);
	}
};

 } // namespace system
} // namespace spplib

#endif // !defined(SPPLIB_F06BB420_6978_4203_8BF8_830598B8CF9A_INCLUDE_GUARD)
