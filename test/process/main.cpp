// process.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <algorithm>
#include <string>
#include <vector>

#define SPPLIB_LOGGER_ENABLE_INLINE
#include "../../spplib/logger.hpp"
#include "../../spplib/process.hpp"

//////////////////////////////////////////////////////////////////////

int test_main()
{
	using namespace spplib::process;

	execute x = process("cmd /C dir") | process("sort") | process("find \"<DIR>\"") >> "ab.txt";
//	execute x = process("cmd /C dir") | process("sort");
//	execute x = process("test") | process("test in");

	x.run();
	x.wait();

//	process xx("find.exe /?");
//	xx.run();
//	xx.wait();

	return 0;
}

//////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	return test_main();
}

int WINAPI _tWinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPTSTR lpCmdLine, 
	int nCmdShow
)
{
	return test_main();
}
