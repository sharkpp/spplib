// main.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>

#include <spplib/property.hpp>

using namespace spplib;

class test
{
	int         m_rw;
	const int & rw_setter(const int & n) { TRACE("%s(): update %d -> %d\n", __FUNCTION__, m_rw, n); m_rw = n; return m_rw; }
	const int & rw_getter() const        { TRACE("%s(): result %d\n",       __FUNCTION__, m_rw);              return m_rw; }

	int         m_ro;
	const int & ro_getter() const        { TRACE("%s(): result %d\n",       __FUNCTION__, m_ro);              return m_ro; }

	int         m_wo;
	const int & wo_setter(const int & n) { TRACE("%s(): update %d -> %d\n", __FUNCTION__, m_wo, n); m_wo = n; return m_wo; }

	test * get_this() { return this; }

public:

	test()
		: rw(get_this()), m_rw(1)
		, ro(get_this()), m_ro(2)
		, wo(get_this()), m_wo(3)
//		, xx(get_this())
	{
	}

	rw_property<int, test, &test::rw_setter, &test::rw_getter> rw;
	ro_property<int, test,                   &test::ro_getter> ro;
	wo_property<int, test, &test::wo_setter>                   wo;

};

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	test v;
	int  n;

	v.rw = 123;
	n    = v.rw;

	v.wo = 123;
	n    = v.ro;

	return 0;
}

