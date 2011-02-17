// simple_array.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#define SPPLIB_LOGGER_ENABLE_INLINE

#include <spplib/logger.hpp>

#include <spplib/tribool.hpp>

using namespace spplib;

// http://www.kmonos.net/alang/boost/classes/tribool.html

#define dump(v)	\
	dump_(_T(#v), v)

void dump_(LPCTSTR title, tribool b )
{
	TRACE(_T("%-15s: "), title);
	if( b )       TRACE(_T("True         "), title);
	else if( !b ) TRACE(_T("False        "), title);
	else          TRACE(_T("Indeterminate"), title);

	TRACE(_T(" | "));
	if( true == b )               TRACE(_T("True         "), title);
	else if( false == b )         TRACE(_T("False        "), title);
	else if( indeterminate == b ) TRACE(_T("Indeterminate"), title);
	else                          TRACE(_T("             "), title);

	TRACE(_T(" | "));
	if( false != b && indeterminate != b )     TRACE(_T("True         "), title);
	else if( true != b && indeterminate != b ) TRACE(_T("False        "), title);
	else if( true != b && false != b )         TRACE(_T("Indeterminate"), title);
	else                                       TRACE(_T("             "), title);

	TRACE(_T("\n"));
}

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	tribool x = indeterminate;

	dump(true);
	dump(false);
	dump(x);
	dump(x     && x);
	dump(false && x);
	dump(true  && x);
	dump(x     || x);
	dump(false || x);
	dump(true  || x);

	return 0;
}

