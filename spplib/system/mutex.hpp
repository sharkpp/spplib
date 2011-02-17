/**
 * @file  spplib/system/mutex.hpp
 * @brief �~���[�e�b�N�X�N���X
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
 * @brief �~���[�e�b�N�X�N���X
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
	 * @brief     �R���X�g���N�^
	 * @param[in] default_state �����l
	 */
	mutex(bool default_state = false)
		: m_lock(static_cast<LONG*>(::_aligned_malloc(sizeof(LONG), 32)))
	{
		*m_lock = default_state ? 1 : 0;
	}

	/// �f�X�g���N�^
	~mutex()
	{
		::_aligned_free(m_lock);
	}

	/**
	 * @brief  ���b�N����
	 * @retval true  �����̐���I��(���b�N�ɐ���)
	 */
	bool lock()
	{
		while( !try_lock() );
		return true;
	}

	/**
	 * @brief  ���b�N����(���s�t��)
	 * @retval true  �����̐���I��(���b�N�ɐ���)
	 * @retval false ���������s����(���b�N�Ɏ��s)
	 */
	bool try_lock()
	{
		return 0 == ::InterlockedCompareExchange(m_lock, 1, 0);
	}

	/**
	 * @brief  ���b�N��������
	 * @retval true  �����̐���I��
	 * @retval false ���������s����
	 */
	bool unlock()
	{
		return 1 == ::InterlockedCompareExchange(m_lock, 0, 1);
	}
};

 } // namespace system
} // namespace spplib

#endif // !defined(SPPLIB_F06BB420_6978_4203_8BF8_830598B8CF9A_INCLUDE_GUARD)
