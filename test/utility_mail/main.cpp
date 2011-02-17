// simple_array.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <vector>

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>

#include <spplib/utility/mail.hpp>

using namespace spplib;

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	// http://ja.wikipedia.org/wiki/%E3%83%A1%E3%83%BC%E3%83%AB%E3%82%A2%E3%83%89%E3%83%AC%E3%82%B9
	// 
	// -------------------------+------------------+---------------------------+---------+---------------------+--------------------------------+
	// 形式                     | RFC 5321での呼称 | 使用箇所(エンベロープ)    | RFC 5322での呼称              | 使用箇所（ヘッダ）[1]          |
	// -------------------------+------------------+---------------------------+---------+---------+-----------+--------------------------------+
	//  foo@example.com         | Mailbox          | -                         |         |         | addr-spec |                                |
	// -------------------------+------------------+---------------------------+         |         +-----------+                                |
	//  <foo@example.com>       |                  |                           |         | mailbox |           | From Sender Reply-To To Cc Bcc |
	// -------------------------+ -                + -                         |         |         | name-addr |                                |
	//  foo <foo@example.com>   |                  |                           | address |         |           |                                |
	// -------------------------+------------------+---------------------------+         +---------+-----------+--------------------------------+
	//  foobar:                 |                  |                           |         |                     |                                |
	//   foo <foo@example.com>, | -                | -                         |         | group               | Reply-To To Cc Bcc             |
	//   bar <bar@example.com>; |                  |                           |         |                     |                                |
	// -------------------------+------------------+---------------------------+---------+---------------------+--------------------------------+
	//  <foo@example.com>       | Path             | reverse-path forward-path | path                          | Return-Path                    |
	// -------------------------+------------------+---------------------------+---------+---------------------+--------------------------------+

#define TEST(in, cmp) \
		{ \
			tstring s1 = spplib::utility::extract_mail_address(in); \
			tstring in_ = spplib::string::replace(in, _T("\r\n"), _T("\\r\\n")); \
			if( cmp == s1 ) { \
				TRACE(_T("OK: \"%s\"\n"), in_.c_str()); \
			} else { \
				TRACE(_T("NG: \"%s\" ->\"%s\" (!=\"%s\")\n"), in_.c_str(), s1.c_str(), cmp); \
			} \
		}

	TEST(_T("foo@example.com"),			_T("foo@example.com"));
	TEST(_T("<foo@example.com>"),		_T("foo@example.com"));
	TEST(_T("foo <foo@example.com>"),	_T("foo@example.com"));
	TEST(_T("foobar:\r\n")
		 _T(" foo <foo@example.com>,\r\n")
		 _T(" bar <bar@example.com>;"), _T("foo@example.com"));
	TEST(_T("<foo@example.com>"),		_T("foo@example.com"));

	return 0;
}

