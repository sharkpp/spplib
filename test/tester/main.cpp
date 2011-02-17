// main.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>

#include <spplib/tester.hpp>

using namespace spplib;

class test1: public spplib::tester::base<test1>
{
private:

	bool caseOK()     { /*TRACE("%s()\n", __FUNCTION__);*/ return true; }
	bool caseNG()     { /*TRACE("%s()\n", __FUNCTION__);*/ return false; }
	bool caseInitOK() { /*TRACE("%s()\n", __FUNCTION__);*/ return true; }
	bool caseInitNG() { /*TRACE("%s()\n", __FUNCTION__);*/ return true; }
	void caseFin()    { /*TRACE("%s()\n", __FUNCTION__);*/ }

public:

	test1()
	{
		append(_T("OK"), &test1::caseOK);
		append(_T("NG"), &test1::caseNG);
		append(_T("OK-InitOK"), &test1::caseOK, &test1::caseInitOK);
		append(_T("OK-InitNG"), &test1::caseOK, &test1::caseInitNG);
		append(_T("NG-InitOK"), &test1::caseNG, &test1::caseInitOK);
		append(_T("NG-InitNG"), &test1::caseNG, &test1::caseInitNG);
		append(_T("OK-InitOK-Fin"), &test1::caseOK, &test1::caseInitOK, &test1::caseFin);
		append(_T("OK-InitNG-Fin"), &test1::caseOK, &test1::caseInitNG, &test1::caseFin);
		append(_T("NG-InitOK-Fin"), &test1::caseNG, &test1::caseInitOK, &test1::caseFin);
		append(_T("NG-InitNG-Fin"), &test1::caseNG, &test1::caseInitNG, &test1::caseFin);
	}

};


int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	test1 x;

	x.run();

	return 0;
}

