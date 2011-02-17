// module_loader.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include <windows.h>
#include "../../spplib/module_loader.hpp"

class test
	: public spplib::module_loader<test>
{
public:

	test()
		: module_loader(_T("kernel32"))
	{
		def("GetTickCount", GetTickCount);
	}

	DWORD (WINAPI *GetTickCount)(VOID);
};

int _tmain(int argc, _TCHAR* argv[])
{
	test x;

	DWORD tick = x.GetTickCount();

	return 0;
}

