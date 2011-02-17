/**
 * @file  spplib/system/event.hpp
 * @brief �C�x���g�N���X
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <windows.h>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_3ECCDEBA_FAE8_40A7_AF90_EF6B90B1CB3A_INCLUDE_GUARD
#define SPPLIB_3ECCDEBA_FAE8_40A7_AF90_EF6B90B1CB3A_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// system library namespace
 namespace system {

/**
 * @brief �C�x���g�N���X
 * @code
 *  spplib::system::event e;
 *  printf("e.signaled() = %s\n", e.signaled() ? "true" : "false"); // e.signaled() = false
 *  printf("e.set()      = %s\n", e.set()      ? "true" : "false"); // e.set()      = true
 *  printf("e.signaled() = %s\n", e.signaled() ? "true" : "false"); // e.signaled() = true
 *  printf("e.reset()    = %s\n", e.reset()    ? "true" : "false"); // e.reset()    = true
 *  printf("e.signaled() = %s\n", e.signaled() ? "true" : "false"); // e.signaled() = false
 * @endcode
 */
class event
{
	HANDLE	m_handle;

	event(const event &);
	event operator = (const event &);

public:

	/**
	 * @brief     �R���X�g���N�^
	 * @param[in] manual_reset  true���w�肵���ꍇ�A�蓮�ł̃��Z�b�g���s���A�Q�ƌ�Ɏ����Ń��Z�b�g�͍s��Ȃ�
	 * @param[in] default_state �����l
	 */
	event(bool manual_reset = true, bool default_state = false)
		: m_handle( ::CreateEvent(NULL,
		                          manual_reset ? TRUE : FALSE,
		                          default_state ? TRUE : FALSE,
		                          NULL) )
	{
	}

	/// �f�X�g���N�^
	~event()
	{
		::CloseHandle(m_handle);
	}

	/**
	 * @brief  �C�x���g���Z�b�g
	 * @retval true  �����̐���I��
	 * @retval false ���������s����
	 */
	bool set()
	{
		return FALSE != ::SetEvent(m_handle);
	}

	/**
	 * @brief  �C�x���g�����Z�b�g
	 * @retval true  �����̐���I��
	 * @retval false ���������s����
	 */
	bool reset()
	{
		return FALSE != ::ResetEvent(m_handle);
	}

	/**
	 * @brief  �C�x���g�̌��ݏ�Ԃ��擾
	 * @retval true  �C�x���g���Z�b�g����Ă���
	 * @retval false �C�x���g���Z�b�g����Ă��Ȃ�
	 */
	bool signaled() const
	{
		return ::WaitForSingleObject(m_handle, 0) == WAIT_OBJECT_0;
	}

};

 } // namespace system
} // namespace spplib

#endif // !defined(SPPLIB_4264F8B3_7239_42E5_8BE0_5417F4D79090_INCLUDE_GUARD)
