/**
 * @file  spplib/logger.cpp
 * @brief ログ出力ライブラリ実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2009.
 */

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#include "logger.hpp"

#ifdef SPPLIB_LOGGER_ENABLE_INLINE
#define SPPLIB_LOGGER_INLINE inline
#else
#define SPPLIB_LOGGER_INLINE
#endif // defined(SPPLIB_LOGGER_ENABLE_INLINE)

/// sharkpp class library namespace
namespace spplib {

/// logger library namespace
namespace logger {

static const int MAX_TRACE_BUFFER = 2048;

SPPLIB_LOGGER_INLINE
void redirect_dbgout::operator ()(LPCTSTR data, int /* = -1*/)
{
	::OutputDebugString(data);
}

SPPLIB_LOGGER_INLINE
redirect_console::redirect_console()
{
	::AllocConsole();
	m_stdout = ::GetStdHandle(STD_OUTPUT_HANDLE);
}

SPPLIB_LOGGER_INLINE
redirect_console::~redirect_console()
{
	::CloseHandle(m_stdout);
}

SPPLIB_LOGGER_INLINE
void redirect_console::operator ()(LPCTSTR data, int length /* = -1*/)
{
	if( length < 0 )
		{ length = (int)::_tcslen(data); }
	DWORD write;
	::WriteFile(m_stdout, data, length * sizeof(TCHAR), &write, NULL);
}

SPPLIB_LOGGER_INLINE
redirect_file::redirect_file(LPCTSTR file_name, bool create_new /* = false */, bool flash_always /* = false */)
	: m_handle( ::CreateFile(file_name, GENERIC_WRITE, FILE_SHARE_READ, NULL
					, create_new ? CREATE_ALWAYS : OPEN_ALWAYS, NULL, 0) )
	, m_flash_always(flash_always)
{
	if( flash_always )
	{ // 毎回フラッシュ処理を行う場合は 新規 or 追記 どちらの場合も一旦開いて閉じる
		::CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
	}
#if defined(_MSC_VER) && 1400 <= _MSC_VER // VC++ .NET 2005 以上
	::_tcsncpy_s(m_file_name, file_name, MAX_PATH);
#else
	::_tcsncpy(m_file_name, file_name, MAX_PATH);
#endif
}

SPPLIB_LOGGER_INLINE
redirect_file::~redirect_file()
{
	if( m_handle != INVALID_HANDLE_VALUE )
	{
		::CloseHandle(m_handle);
	}
}

SPPLIB_LOGGER_INLINE
void redirect_file::operator ()(LPCTSTR data, int length /* = -1*/)
{
	DWORD write;
	if( length < 0 )
	{
		length = (int)::_tcslen(data);
	}
	if( m_flash_always )
	{
		m_handle = ::CreateFile(m_file_name, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, 0);
		LARGE_INTEGER tmp = { 0, 0 };
		::SetFilePointerEx(m_handle, tmp, NULL, FILE_END);
	}
	::WriteFile(m_handle, data, length * sizeof(TCHAR), &write, NULL);
//	::FlushFileBuffers(m_handle);
	if( m_flash_always )
	{
		::CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
	}
}

#ifndef DISABLE_LOG

struct output_holder {
	redirect_base * redirect;
	output_holder() : redirect(new redirect_dbgout) {  }
	~output_holder() { delete redirect; }
	void set(redirect_base * ptr) { delete redirect; redirect = ptr; }
} output_holder;

/**
 * @brief     トレースの出力先を指定
 * @param[in] redirect 出力先
 */
SPPLIB_LOGGER_INLINE
void set_trace_output(redirect_base * redirect)
{
	output_holder.set(redirect);
}

/**
 * @brief     トレース
 * @param[in] format 出力先
 * @param[in] args   出力内容
 * @internal
 */
SPPLIB_LOGGER_INLINE
void trace_(LPCTSTR format, const va_list & args)
{
	LPTSTR	buff = 0;
	int		len = 0;

#if defined(_MSC_VER)
	len = _vsctprintf(format, args);
#else
	len = MAX_TRACE_BUFFER;
#endif
	if( len < 0 ) {
		return;
	}

	try {
		buff = new TCHAR[len + 1];
		if( !buff )
			{ return; }
	} catch(...) {
		return;
	}

#if defined(_MSC_VER) && 1400 <= _MSC_VER // VC++ .NET 2005 以上
	len = _vsntprintf_s(buff, len + 1, len, format, args);
#else
	len = _vsntprintf(buff, len, format, args);
#endif

	buff[len] = '\0';

	(*output_holder.redirect)(buff);

	delete [] buff;
}

/**
 * @brief     トレース
 * @param[in] format 出力書式
 * @param[in] ...    出力内容
 */
SPPLIB_LOGGER_INLINE
void trace(LPCTSTR format, ...)
{
	va_list args;
	va_start(args, format);
	trace_(format, args);
	va_end(args);
}

/**
 * @brief     条件付のトレース
 * @param[in] condition 出力条件
 * @param[in] format    出力書式
 * @param[in] ...       出力内容
 */
SPPLIB_LOGGER_INLINE
void trace_if(bool condition, LPCTSTR format, ...)
{
	if( condition )
	{
		va_list args;
		va_start(args, format);
		trace_(format, args);
		va_end(args);
	}
}

/**
 * @brief     ダンプトレース
 * @param[in] data 出力書式
 * @param[in] size 出力書式
 */
SPPLIB_LOGGER_INLINE
void trace_dump(const void * data, int size)
{
	// 出力フォーマット
	//           11111111112222222222333333333344444444445555555555666666666677
	// 012345678901234567890123456789012345678901234567890123456789012345678901
	// ADDR: +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F ----------------
	// XXXX: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XXXXXXXXXXXXXXXX

	const unsigned char * data_ = (const unsigned char *)data;
	TCHAR buff[71+2];
	LPTSTR hex_ptr, dump_ptr;
#if defined(_MSC_VER) && 1400 <= _MSC_VER // VC++ .NET 2005 以上
	LPTSTR last;
#endif

	// ヘッダ出力
	(*output_holder.redirect)(
		_T("ADDR: +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F ----------------\r\n"));

	//
	for(int addr = 0; addr < size; )
	{
#if defined(_MSC_VER) && 1400 <= _MSC_VER // VC++ .NET 2005 以上
		::_stprintf_s(buff, sizeof(buff)/sizeof(buff[0])/*_countof(buff)*/, _T("%04X:"), addr & 0xFFFF);
		last     = buff + sizeof(buff)/sizeof(buff[0]);
#else
		::_stprintf(buff, _T("%04X: "), addr & 0xFFFF);
#endif
		hex_ptr  = buff + 5;
		dump_ptr = buff + 54;
		for(int offset = 0; offset < 16; offset++, addr++, ++data_)
		{
			if( addr < size )
			{
#if defined(_MSC_VER) && 1400 <= _MSC_VER // VC++ .NET 2005 以上
				::_stprintf_s(hex_ptr,  int(buff + 54 - hex_ptr),  _T(" %02X"), *data_);
				::_stprintf_s(dump_ptr, int(last - dump_ptr), _T("%c")
					, 0x20 <= *data_ && *data_ <= 0x7E ? *data_ : _T('.'));
#else
				::_stprintf(hex_ptr,  _T(" %02X"), *data_);
				::_stprintf(dump_ptr, _T("%c")
					, 0x20 <= *data_ && *data_ <= 0x7E ? *data_ : _T('.'));
#endif
			}
			else
			{
#if defined(_MSC_VER) && 1400 <= _MSC_VER // VC++ .NET 2005 以上
				::_stprintf_s(hex_ptr,  int(buff + 54 - hex_ptr),  _T(" --"));
				::_stprintf_s(dump_ptr, int(last - dump_ptr), _T(" "));
#else
				::_stprintf(hex_ptr,  _T(" --"));
				::_stprintf(dump_ptr, _T(" "));
#endif
			}
			hex_ptr += 3;
			dump_ptr++;
		}
		*hex_ptr = _T(' ');
#if defined(_MSC_VER) && 1400 <= _MSC_VER // VC++ .NET 2005 以上
		::_stprintf_s(dump_ptr, int(last - dump_ptr), _T("\r\n"));
#else
		::_stprintf(dump_ptr, _T("\r\n"));
#endif
		// １行出力
		(*output_holder.redirect)(buff);
	}
}

#endif

} // namespace logger

} // namespace spplib
