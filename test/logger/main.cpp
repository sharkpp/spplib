// debug.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "../../spplib/logger.hpp"

int _tmain(int argc, _TCHAR* argv[])
{
//	SET_TRACE_OUTPUT_CONSOLE();
	SET_TRACE_OUTPUT_LOGFILE_FLASH("test.log");
	TRACE("test\n");
	for(int i = 0; i < 10000; i++)
		TRACE("test %d\n", i);
	return 0;
}

