/**
 * @file  spplib/registry.hpp
 * @brief ���W�X�g������I�u�W�F�N�g�N���X��`
 * @note SPPLIB_REGISTRY_DISABLE_STL_SUPPORT ���`����� stl::string �Ȃǂ��g�p�������\�b�h���g�p�ł��Ȃ��Ȃ�
 * @note SPPLIB_REGISTRY_ENABLE_INLINE ���w�b�_�t�@�C���̑O�ɒ�`����� .cpp ���w�b�_�ɓǂݍ��݃C�����C��������
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2009.
 */

#include <windows.h>
#include <tchar.h>
#include <wchar.h>

#ifndef SPPLIB_REGISTRY_DISABLE_STL_SUPPORT // STL ���g�p���Ȃ��悤�ɂ���
#include <string>
#endif // !defined(SPPLIB_REGISTRY_DISABLE_STL_SUPPORT)

#ifndef SPPLIB_FA2E1A64_9EA3_4FB3_A2EA_301EE907841C_INCLUDE_GUARD
#define SPPLIB_FA2E1A64_9EA3_4FB3_A2EA_301EE907841C_INCLUDE_GUARD

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

/// sharkpp class library namespace
namespace spplib {

/**
 * @brief ���W�X�g������I�u�W�F�N�g�N���X
 * @note SPPLIB_REGISTRY_DISABLE_STL_SUPPORT ���`����� stl::string �Ȃǂ��g�p�������\�b�h���g�p�ł��Ȃ��Ȃ�
 * @note SPPLIB_REGISTRY_ENABLE_INLINE ���w�b�_�t�@�C���̑O�ɒ�`����� .cpp ���w�b�_�ɓǂݍ��݃C�����C��������
 * @code
 *  spplib::registry reg;
 *  TCHAR buff[256];
 *  reg.select("HKCR\\Paint.Picture");
 *  reg.get("BrowserFlags", buff);
 *  printf("[HKCR\\Paint.Picture]\nBrowserFlags='%s'\n", buff);
 *  reg.close(); // optional
 * @endcode
 */
class registry
{
private:

	HKEY			m_key_handle;

	int				m_last_error;	///< �Ō�ɔ��������G���[���

private:

	/**
	 * @brief �L�[�̃p�X�����`�ς݂̒萔��Ԃ��T�u�L�[��Ԃ�
	 * @internal
	 */
	HKEY search_defined_key_handle(LPCTSTR key_name, LPCTSTR * sub_key_name = NULL);

public:

	/** 
	 * @brief �f�t�H���g�R���X�g���N�^
	 */
	registry();

	/** 
	 * @brief     �R���X�g���N�^
	 * @param[in] key_name    �I�[�v�����郌�W�X�g���[�L�[
	 * @param[in] access_mask �A�N�Z�X����
	 * @param[in] must_exist  �L�[�����݂��Ă��Ȃ��Ƃ����Ȃ����ǂ���
	 * @note      ���������s�����ꍇ�́Aint�^�̗�O�𔭍s���܂��B
	 */
	registry(LPCTSTR key_name, REGSAM access_mask = KEY_READ|KEY_WRITE, bool must_exist = false);

	/** 
	 * @brief     �R���X�g���N�^
	 * @param[in] key_name    �I�[�v�����郌�W�X�g���[�L�[
	 * @param[in] access_mask �A�N�Z�X����
	 * @param[in] must_exist  �L�[�����݂��Ă��Ȃ��Ƃ����Ȃ����ǂ���
	 * @note      ���������s�����ꍇ�́Aint�^�̗�O�𔭍s���܂��B
	 */
	registry(const GUID & key_name, REGSAM access_mask = KEY_READ|KEY_WRITE, bool must_exist = false);

	/** 
	 * @brief �f�X�g���N�^
	 * @note  �֘A�t�����Ă��郌�W�X�g���L�[�n���h���͕����܂�
	 */
	~registry();

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
	 * @brief  ���W�X�g���L�[���J���Ă��邩�ۂ��̏�Ԃ��擾
	 * @retval true  �J���Ă���
	 * @retval false �J���Ă��Ȃ�
	 */
	bool opened()
	{
		return false != m_key_handle;
	}

	/** 
	 * @brief  HKEY�ւ̃L���X�g
	 * @return �֘A�t���Ă���L�[�n���h��
	 */
	operator HKEY () const
	{
		return m_key_handle;
	}

	/** 
	 * @brief     ����Ώۂ̃��W�X�g���L�[���w��
	 * @param[in] key_name    �I�[�v�����郌�W�X�g���[�L�[
	 * @param[in] access_mask �A�N�Z�X����
	 * @param[in] must_exist  �L�[�����݂��Ă��Ȃ��Ƃ����Ȃ����ǂ���
	 * @retval    true  ����
	 * @retval    false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool select(LPCTSTR key_name, REGSAM access_mask = KEY_READ|KEY_WRITE, bool must_exist = false);

	/** 
	 * @brief     ����Ώۂ̃��W�X�g���L�[���w��(GUID)
	 * @param[in] key_name    �I�[�v�����郌�W�X�g���[�L�[
	 * @param[in] access_mask �A�N�Z�X����
	 * @param[in] must_exist  �L�[�����݂��Ă��Ȃ��Ƃ����Ȃ����ǂ���
	 * @retval    true  ����
	 * @retval    false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool select(const GUID & key_name, REGSAM access_mask = KEY_READ|KEY_WRITE, bool must_exist = false);

	/** 
	 * @brief     �w��̃��W�X�g���L�[���폜
	 * @param[in] key_name    �I�[�v�����郌�W�X�g���[�L�[
	 * @retval    true  ����
	 * @retval    false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 * @note �폜�Ώۂ̃��W�X�g���L�[��select()�ł��łɑI�����Ă����ꍇ�͈ȍ~�̓���͕s��ɂȂ�܂��B
	 */
	bool erase(LPCTSTR key_name);

	/** 
	 * @brief  ���W�X�g���L�[�̃N���[�Y
	 * @retval true  ����
	 * @retval false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool close();

	/** 
	 * @brief    ���W�X�g���l�̍폜
	 * @param[in]  value_name       �폜���郌�W�X�g���l�̖��O
	 * @retval     true  ����
	 * @retval     false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool remove(LPCTSTR value_name);

	/** 
	 * @brief    ���W�X�g���l�̎擾(������)
	 * @param[in]  value_name       �擾���郌�W�X�g���l�̖��O�ANULL�������͋󕶎���ɂ����"(����)"��ǂݎ��
	 * @param[out] buff             �擾�p�̃o�b�t�@
	 * @param[in]  buff_size        �擾�p�̃o�b�t�@�Ɋm�ۍς݂̃T�C�Y
	 * @param[out] buff_result_size �ǂݎ��T�C�Y
	 * @retval     true  ����
	 * @retval     false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool get(LPCTSTR value_name, LPTSTR buff, DWORD buff_size, DWORD * buff_result_size = NULL);

#ifndef SPPLIB_REGISTRY_DISABLE_STL_SUPPORT

	/** 
	 * @brief    ���W�X�g���l�̎擾(������)
	 * @param[in]  value_name �擾���郌�W�X�g���l�̖��O�ANULL�������͋󕶎���ɂ����"(����)"��ǂݎ��
	 * @param[out] buff       �擾�p�̃o�b�t�@
	 * @retval     true  ����
	 * @retval     false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
#if !defined(_UNICODE)
	bool get(LPCTSTR value_name, std::string & buff);
#else
	bool get(LPCTSTR value_name, std::wstring & buff);
#endif

#endif // !defined(SPPLIB_REGISTRY_DISABLE_STL_SUPPORT)

	/** 
	 * @brief    ���W�X�g���l�̎擾(int)
	 * @param[in]  value_name �擾���郌�W�X�g���l�̖��O�ANULL�������͋󕶎���ɂ����"(����)"��ǂݎ��
	 * @param[out] buff       �擾�p�̃o�b�t�@
	 * @retval     true  ����
	 * @retval     false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool get(LPCTSTR value_name, int & buff);

	/** 
	 * @brief    ���W�X�g���l�̎擾(bool)
	 * @param[in]  value_name �擾���郌�W�X�g���l�̖��O�ANULL�������͋󕶎���ɂ����"(����)"��ǂݎ��
	 * @param[out] buff       �擾�p�̃o�b�t�@
	 * @retval     true  ����
	 * @retval     false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool get(LPCTSTR value_name, bool & buff);

	/** 
	 * @brief    ���W�X�g���l�̎擾(DWORD)
	 * @param[in]  value_name �擾���郌�W�X�g���l�̖��O�ANULL�������͋󕶎���ɂ����"(����)"��ǂݎ��
	 * @param[out] buff       �擾�p�̃o�b�t�@
	 * @retval     true  ����
	 * @retval     false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool get(LPCTSTR value_name, DWORD & buff);

	/** 
	 * @brief    ���W�X�g���l�̎擾(unsigned long long)
	 * @param[in]  value_name �擾���郌�W�X�g���l�̖��O�ANULL�������͋󕶎���ɂ����"(����)"��ǂݎ��
	 * @param[out] buff       �擾�p�̃o�b�t�@
	 * @retval     true  ����
	 * @retval     false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool get(LPCTSTR value_name, unsigned long long & buff);

	/** 
	 * @brief    ���W�X�g���l�̐ݒ�(������)
	 * @param[in]  value_name       �ݒ肷�郌�W�X�g���l�̖��O�ANULL�������͋󕶎���ɂ����"(����)"��ǂݎ��
	 * @param[out] data             �ݒ肷��f�[�^
	 * @param[in]  data_size        �ݒ肷��f�[�^�̃T�C�Y
	 * @param[out] expand           true���w�肷���REG_SZ�ł͂Ȃ�REG_EXPAND_SZ�Œl���Z�b�g����
	 * @retval     true  ����
	 * @retval     false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool set(LPCTSTR value_name, LPCTSTR data, DWORD data_size = (DWORD)-1, bool expand = false);

	/** 
	 * @brief    ���W�X�g���l�̐ݒ�(int)
	 * @param[in]  value_name       �ݒ肷�郌�W�X�g���l�̖��O�ANULL�������͋󕶎���ɂ����"(����)"��ǂݎ��
	 * @param[out] data             �ݒ肷��f�[�^
	 * @retval     true  ����
	 * @retval     false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool set(LPCTSTR value_name, int data);

	/** 
	 * @brief    ���W�X�g���l�̐ݒ�(DWORD)
	 * @param[in]  value_name       �ݒ肷�郌�W�X�g���l�̖��O�ANULL�������͋󕶎���ɂ����"(����)"��ǂݎ��
	 * @param[out] data             �ݒ肷��f�[�^
	 * @retval     true  ����
	 * @retval     false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool set(LPCTSTR value_name, DWORD data);

	/** 
	 * @brief    ���W�X�g���l�̐ݒ�(bool)
	 * @param[in]  value_name       �ݒ肷�郌�W�X�g���l�̖��O�ANULL�������͋󕶎���ɂ����"(����)"��ǂݎ��
	 * @param[out] data             �ݒ肷��f�[�^
	 * @retval     true  ����
	 * @retval     false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool set(LPCTSTR value_name, bool data);

	/** 
	 * @brief    ���W�X�g���l�̐ݒ�(unsigned long long)
	 * @param[in]  value_name       �ݒ肷�郌�W�X�g���l�̖��O�ANULL�������͋󕶎���ɂ����"(����)"��ǂݎ��
	 * @param[out] data             �ݒ肷��f�[�^
	 * @retval     true  ����
	 * @retval     false ���s�A�ڍׂȃG���[���擾����ɂ� get_last_error() ���Ăяo���܂�
	 */
	bool set(LPCTSTR value_name, unsigned long long data);

};

} // namespace spplib

#ifdef SPPLIB_REGISTRY_ENABLE_INLINE
#include "registry.cpp"
#endif // defined(SPPLIB_REGISTRY_ENABLE_INLINE)

#endif // !defined(SPPLIB_FA2E1A64_9EA3_4FB3_A2EA_301EE907841C_INCLUDE_GUARD)
