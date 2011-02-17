/**
 * @file  spplib/shared_memory.hpp
 * @brief ���L�������I�u�W�F�N�g�N���X��`�E����
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
 * @brief ���L�������I�u�W�F�N�g
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

	HANDLE		m_handle;			///< ���L�������̃n���h��
	HANDLE		m_lock;				///< ���b�N�I�u�W�F�N�g�̃n���h��
	int			m_last_error;		///< �Ō�ɔ��������G���[���
	DWORD		m_allocate_size;	///< ���L�������̊m�ۃT�C�Y
	void *		m_locked_ptr;		///< ���b�N����Ă��郁�����̈�
	bool		m_already_exists;	///< ���łɋ��L�����������݂��Ă�����

public:

	/** 
	 * @brief �f�t�H���g�R���X�g���N�^
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
	 * @brief �R���X�g���N�^
	 * @param[in] name          ���L��������
	 * @param[in] size          ���L�������T�C�Y
	 * @param[in] initial_value �����l�A�f�t�H���g�� 0x00 �Ŗ��߂��܂�
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
	 * @brief �f�X�g���N�^
	 */
	~shared_memory()
	{
		close();
	}

	/** 
	 * @brief  �Ō�ɔ��������G���[��Ԃ��擾
	 * @retval ERROR_SUCCESS �G���[���������Ă��Ȃ����Ƃ������܂�
	 * @retval ���̑�        ���̑��̃G���[���N���������Ƃ������܂�
	 */
	int get_last_error() const
	{
		return m_last_error;
	}

	/** 
	 * @brief  �������ɋ��L�����������݂������H
	 * @retval true  ���łɓ������O�̋��L�������͑��݂��Ă���
	 * @retval false ���݂��Ă��Ȃ������̂ŐV���ɍ쐬���ꂽ
	 */
	bool get_exists_state() const
	{
		return m_already_exists;
	}

	/** 
	 * @brief ���L���������쐬
	 * @param[in] name          ���L��������
	 * @param[in] size          ���L�������T�C�Y
	 * @param[in] initial_value �����l�A�f�t�H���g�� 0x00 �Ŗ��߂��܂�
	 * @retval    true  ����
	 * @retval    false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool create(LPCTSTR name, DWORD size, const void * initial_value = NULL)
	{
		m_last_error = ERROR_SUCCESS;

		close();

		// �~���[�e�b�N�X�p�̖��O���쐬
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

		// ���������̏ꍇ�͎w�肳�ꂽ�f�[�^���g���������̈������������
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
	 * @brief ���L��������j��
	 * @retval    true  ����
	 * @retval    false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
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
	 * @brief ���L�����������b�N���������̈�̃|�C���^���擾
	 * @retval ��NULL ����
	 * @retval NULL   ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
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
	 * @brief ���L�������̃��b�N������
	 * @retval true  ����
	 * @retval false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
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
