/**
 * @file  spplib/logger.hpp
 * @brief ���O�o�̓��C�u������`
 * @note SPPLIB_LOGGER_ENABLE_INLINE ���w�b�_�t�@�C���̑O�ɒ�`����� .cpp ���w�b�_�ɓǂݍ��݃C�����C��������
 * @note DISABLE_LOG ���`�����TRACE�Ȃǂ���̏����ɒu����������
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2009.
 */

#include <windows.h>

#ifndef SPPLIB_4264F8B3_7239_42E5_8BE0_5417F4D79090_INCLUDE_GUARD
#define SPPLIB_4264F8B3_7239_42E5_8BE0_5417F4D79090_INCLUDE_GUARD

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {

/// logger library namespace
namespace logger {

/**
 * @brief ���O�o�� �o�͐��{�N���X
 * @internal
 */
class redirect_base
{
public:
	virtual void operator ()(LPCTSTR data, int length = -1) = 0;
};

/// ���O�o�� �o�͐�Ȃ�
class redirect_none
	: public redirect_base
{
public:
	virtual void operator ()(LPCTSTR, int = -1) { }
};

/// ���O�o�� �o�͐�g���[�X���O
class redirect_dbgout
	: public redirect_base
{
public:
	virtual void operator ()(LPCTSTR data, int = -1);
};

/// ���O�o�� �o�͐�R���\�[��
class redirect_console
	: public redirect_base
{
private:
	HANDLE m_stdout;
public:
	redirect_console();
	~redirect_console();
	virtual void operator ()(LPCTSTR data, int length = -1);
};

/// ���O�o�� �o�͐惍�O�t�@�C��
class redirect_file
	: public redirect_base
{
private:
	bool	m_flash_always;
	TCHAR	m_file_name[MAX_PATH+1];
	HANDLE	m_handle;
	redirect_file(){}
public:
	redirect_file(LPCTSTR file_name, bool create_new = false, bool flash_always = false);
	~redirect_file();
	virtual void operator ()(LPCTSTR data, int length = -1);
};

} // namespace logger

} // namespace spplib

//////////////////////////////////////////////////////////////////////

#ifdef SPPLIB_LOGGER_ENABLE_INLINE
#include "logger.cpp"
#else

/// sharkpp class library namespace
namespace spplib {

/// logger library namespace
namespace logger {

/**
 * @brief     �g���[�X�̏o�͐���w��
 * @param[in] redirect �o�͐�
 */
void set_trace_output(redirect_base * redirect);

/**
 * @brief     �g���[�X
 * @param[in] format �o�͏���
 * @param[in] ...    �o�͓��e
 */
void trace(LPCTSTR format, ...);

/**
 * @brief     �����t�̃g���[�X
 * @param[in] condition �o�͏���
 * @param[in] format    �o�͏���
 * @param[in] ...       �o�͓��e
 */
void trace_if(bool condition, LPCTSTR format, ...);

/**
 * @brief     �_���v�g���[�X
 * @param[in] data �o�͏���
 * @param[in] size �o�͏���
 */
void trace_dump(const void * data, int size);

} // namespace logger

} // namespace spplib

#endif // defined(SPPLIB_LOGGER_ENABLE_INLINE)

//////////////////////////////////////////////////////////////////////
// �\�L�Z�k�}�N��

#undef  TRACE
#undef  TRACEIF

#if !defined(DISABLE_LOG)

/**
 * @brief �g���[�X
 * @see    spplib::logger::trace
 */
#define TRACE	spplib::logger::trace

/**
 * @brief �����t�g���[�X
 * @see   spplib::logger::trace_if
 */
#define TRACEIF	spplib::logger::trace_if

/**
 * @brief �_���v�g���[�X
 * @see    spplib::logger::trace_dump
 */
#define TRACE_DUMP	spplib::logger::trace_dump

/**
 * @brief ���O�o�͂𖳌��ɂ���
 * @see    spplib::logger::set_trace_output
 * @see    spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_NONE()	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_none)

/**
 * @brief ���O�o�͐���g���[�X�ɂ���
 * @see    spplib::logger::set_trace_output
 * @see    spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_DBGOUT()	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_dbgout)

/**
 * @brief ���O�o�͐���R���\�[���ɂ���
 * @see    spplib::logger::set_trace_output
 * @see    spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_CONSOLE()	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_console)

/**
 * @brief     ���O�o�͐�����O�t�@�C���ɂ���
 * @param[in] file_name ���O�t�@�C����
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_LOGFILE(file_name)	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_file(file_name))

/**
 * @brief     ���O�o�͐��V�K���O�t�@�C���ɂ���
 * @param[in] file_name ���O�t�@�C����
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_NEW_LOGFILE(file_name)	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_file(file_name, true))

/**
 * @brief     ���O�o�͐�����O�t�@�C���ɂ���(����t���b�V��)
 * @param[in] file_name ���O�t�@�C����
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_LOGFILE_FLASH(file_name)	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_file(file_name, false, true))

/**
 * @brief     ���O�o�͐��V�K���O�t�@�C���ɂ���(����t���b�V��)
 * @param[in] file_name ���O�t�@�C����
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_NEW_LOGFILE_FLASH(file_name)	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_file(file_name, true, true))

#else

/**
 * @brief �g���[�X
 * @see    spplib::logger::trace
 */
#define TRACE	__noop

/**
 * @brief �����t�g���[�X
 * @see   spplib::logger::trace_if
 */
#define TRACEIF	__noop

/**
 * @brief �_���v�g���[�X
 * @see    spplib::logger::trace_dump
 */
#define TRACE_DUMP	__noop

/**
 * @brief ���O�o�͂𖳌��ɂ���
 * @see    spplib::logger::set_trace_output
 * @see    spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_NONE()	__noop

/**
 * @brief ���O�o�͐���g���[�X�ɂ���
 * @see    spplib::logger::set_trace_output
 * @see    spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_DBGOUT()	__noop

/**
 * @brief ���O�o�͐���R���\�[���ɂ���
 * @see    spplib::logger::set_trace_output
 * @see    spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_CONSOLE()	__noop

/**
 * @brief     ���O�o�͐�����O�t�@�C���ɂ���
 * @param[in] file_name ���O�t�@�C����
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_LOGFILE(file_name)	__noop

/**
 * @brief     ���O�o�͐��V�K���O�t�@�C���ɂ���
 * @param[in] file_name ���O�t�@�C����
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_NEW_LOGFILE(file_name)	__noop

/**
 * @brief     ���O�o�͐�����O�t�@�C���ɂ���(����t���b�V��)
 * @param[in] file_name ���O�t�@�C����
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_LOGFILE_FLASH(file_name)	__noop

/**
 * @brief     ���O�o�͐��V�K���O�t�@�C���ɂ���(����t���b�V��)
 * @param[in] file_name ���O�t�@�C����
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_NEW_LOGFILE_FLASH(file_name)	__noop

#endif // !defined(DISABLE_LOG)

#endif // !defined(SPPLIB_4264F8B3_7239_42E5_8BE0_5417F4D79090_INCLUDE_GUARD)
