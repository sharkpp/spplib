// main.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>
#include <conio.h> // getch
#include <string>
#include <errno.h>

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>

#include <spplib/string.hpp>

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace spplib;
	using namespace spplib::string;

	std::wstring ws;
	std::string   s;

	TRACE("%s\n", format("%d", 1234).c_str());

	s = "123abcABC";
	TRACE("tolower(\"%s\")  = \"%s\"\n", s.c_str(), tolower(s).c_str());
	TRACE("toupper(\"%s\")  = \"%s\"\n", s.c_str(), toupper(s).c_str());

	s = " abc ";
	TRACE("trim_left(\"%s\")    = \"%s\"\n", s.c_str(), trim_left(s).c_str());
	TRACE("trim_right(\"%s\")   = \"%s\"\n", s.c_str(), trim_right(s).c_str());
	TRACE("trim(\"%s\")         = \"%s\"\n", s.c_str(), trim(s).c_str());

	s = " abc abc ";
	TRACE("replace(\"%s\",\"%s\",\"%s\") = \"%s\"\n"
		, s.c_str(), "ab", "123", replace(s, "ab", "123").c_str());

	s = "aAあ";
	TRACE("mbtowc(\"%s\") = \"%s\"\n", s.c_str(), wctomb(mbtowc(s)).c_str());
	ws = L"aAあ\x5C\x99";
	TRACE("wctomb() = \"%s\"\n", /*ws.c_str(), */wctomb(ws).c_str());

	return 0;
}

