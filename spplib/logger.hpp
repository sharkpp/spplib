/**
 * @file  spplib/logger.hpp
 * @brief ログ出力ライブラリ定義
 * @note SPPLIB_LOGGER_ENABLE_INLINE をヘッダファイルの前に定義すると .cpp をヘッダに読み込みインライン化する
 * @note DISABLE_LOG を定義するとTRACEなどが空の処理に置き換えられる
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
 * @brief ログ出力 出力先基本クラス
 * @internal
 */
class redirect_base
{
public:
	virtual void operator ()(LPCTSTR data, int length = -1) = 0;
};

/// ログ出力 出力先なし
class redirect_none
	: public redirect_base
{
public:
	virtual void operator ()(LPCTSTR, int = -1) { }
};

/// ログ出力 出力先トレースログ
class redirect_dbgout
	: public redirect_base
{
public:
	virtual void operator ()(LPCTSTR data, int = -1);
};

/// ログ出力 出力先コンソール
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

/// ログ出力 出力先ログファイル
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
 * @brief     トレースの出力先を指定
 * @param[in] redirect 出力先
 */
void set_trace_output(redirect_base * redirect);

/**
 * @brief     トレース
 * @param[in] format 出力書式
 * @param[in] ...    出力内容
 */
void trace(LPCTSTR format, ...);

/**
 * @brief     条件付のトレース
 * @param[in] condition 出力条件
 * @param[in] format    出力書式
 * @param[in] ...       出力内容
 */
void trace_if(bool condition, LPCTSTR format, ...);

/**
 * @brief     ダンプトレース
 * @param[in] data 出力書式
 * @param[in] size 出力書式
 */
void trace_dump(const void * data, int size);

} // namespace logger

} // namespace spplib

#endif // defined(SPPLIB_LOGGER_ENABLE_INLINE)

//////////////////////////////////////////////////////////////////////
// 表記短縮マクロ

#undef  TRACE
#undef  TRACEIF

#if !defined(DISABLE_LOG)

/**
 * @brief トレース
 * @see    spplib::logger::trace
 */
#define TRACE	spplib::logger::trace

/**
 * @brief 条件付トレース
 * @see   spplib::logger::trace_if
 */
#define TRACEIF	spplib::logger::trace_if

/**
 * @brief ダンプトレース
 * @see    spplib::logger::trace_dump
 */
#define TRACE_DUMP	spplib::logger::trace_dump

/**
 * @brief ログ出力を無効にする
 * @see    spplib::logger::set_trace_output
 * @see    spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_NONE()	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_none)

/**
 * @brief ログ出力先をトレースにする
 * @see    spplib::logger::set_trace_output
 * @see    spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_DBGOUT()	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_dbgout)

/**
 * @brief ログ出力先をコンソールにする
 * @see    spplib::logger::set_trace_output
 * @see    spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_CONSOLE()	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_console)

/**
 * @brief     ログ出力先をログファイルにする
 * @param[in] file_name ログファイル名
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_LOGFILE(file_name)	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_file(file_name))

/**
 * @brief     ログ出力先を新規ログファイルにする
 * @param[in] file_name ログファイル名
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_NEW_LOGFILE(file_name)	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_file(file_name, true))

/**
 * @brief     ログ出力先をログファイルにする(毎回フラッシュ)
 * @param[in] file_name ログファイル名
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_LOGFILE_FLASH(file_name)	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_file(file_name, false, true))

/**
 * @brief     ログ出力先を新規ログファイルにする(毎回フラッシュ)
 * @param[in] file_name ログファイル名
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_NEW_LOGFILE_FLASH(file_name)	\
	spplib::logger::set_trace_output(new spplib::logger::redirect_file(file_name, true, true))

#else

/**
 * @brief トレース
 * @see    spplib::logger::trace
 */
#define TRACE	__noop

/**
 * @brief 条件付トレース
 * @see   spplib::logger::trace_if
 */
#define TRACEIF	__noop

/**
 * @brief ダンプトレース
 * @see    spplib::logger::trace_dump
 */
#define TRACE_DUMP	__noop

/**
 * @brief ログ出力を無効にする
 * @see    spplib::logger::set_trace_output
 * @see    spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_NONE()	__noop

/**
 * @brief ログ出力先をトレースにする
 * @see    spplib::logger::set_trace_output
 * @see    spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_DBGOUT()	__noop

/**
 * @brief ログ出力先をコンソールにする
 * @see    spplib::logger::set_trace_output
 * @see    spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_CONSOLE()	__noop

/**
 * @brief     ログ出力先をログファイルにする
 * @param[in] file_name ログファイル名
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_LOGFILE(file_name)	__noop

/**
 * @brief     ログ出力先を新規ログファイルにする
 * @param[in] file_name ログファイル名
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_NEW_LOGFILE(file_name)	__noop

/**
 * @brief     ログ出力先をログファイルにする(毎回フラッシュ)
 * @param[in] file_name ログファイル名
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_LOGFILE_FLASH(file_name)	__noop

/**
 * @brief     ログ出力先を新規ログファイルにする(毎回フラッシュ)
 * @param[in] file_name ログファイル名
 * @see       spplib::logger::set_trace_output
 * @see       spplib::logger::redirect_none
 */
#define SET_TRACE_OUTPUT_NEW_LOGFILE_FLASH(file_name)	__noop

#endif // !defined(DISABLE_LOG)

#endif // !defined(SPPLIB_4264F8B3_7239_42E5_8BE0_5417F4D79090_INCLUDE_GUARD)
