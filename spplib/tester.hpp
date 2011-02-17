/**
 * @file  spplib/tester.hpp
 * @brief ユニットテストライブラリ
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <windows.h>
#include <tchar.h>
#include <vector>

#include <spplib/logger.hpp>
#include <spplib/string.hpp>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_24A741E7_6E88_48F5_A3EE_2FCBA5038908_INCLUDE_GUARD
#define SPPLIB_24A741E7_6E88_48F5_A3EE_2FCBA5038908_INCLUDE_GUARD

/// sharkpp class library namespace
namespace spplib {

/// tester namespace
namespace tester {

template<typename T>
class base
{
	typedef bool (T::*TEST_CASE_METHOD_TYPE)(void);
	typedef bool (T::*TEST_CASE_INIT_METHOD_TYPE)(void);
	typedef void (T::*TEST_CASE_FIN_METHOD_TYPE)(void);
	typedef struct {
		spplib::tstring				caption;
		TEST_CASE_METHOD_TYPE		fn;
		TEST_CASE_INIT_METHOD_TYPE	fnInit;
		TEST_CASE_FIN_METHOD_TYPE	fnFin;
	} TEST_CASE_ITEM_TYPE;
	typedef std::vector<TEST_CASE_ITEM_TYPE> TEST_CASE_STORE_TYPE;

	spplib::tstring	m_caption;

	size_t	m_success;
	size_t	m_fail;

	TEST_CASE_STORE_TYPE m_test_case;

protected:

	void append(LPCTSTR caption, TEST_CASE_METHOD_TYPE fn, TEST_CASE_INIT_METHOD_TYPE fnInit = NULL, TEST_CASE_FIN_METHOD_TYPE fnFin = NULL)
	{
		TEST_CASE_ITEM_TYPE tmp = {
				caption, fn, fnInit, fnFin,
			};
		m_test_case.push_back(tmp);
	}

public:

	base(LPCTSTR caption = _T(""))
		: m_caption(caption)
		, m_success(0)
		, m_fail(0)
	{
	}

	virtual ~base()
	{
	}

	bool result() const
		{ return !m_fail; }

	size_t total() const
		{ return m_success + m_fail; }

	size_t successed() const
		{ return m_success; }

	size_t failed() const
		{ return m_fail; }

	bool run()
	{
		TRACE("%s ----------------------------------------\n", m_caption.c_str());
		for(TEST_CASE_STORE_TYPE::iterator
				ite = m_test_case.begin(),
				last= m_test_case.end();
			ite != last; ++ite)
		{
			bool result = false;
			if( ite->fnInit ) {
				if( !((static_cast<T*>(this))->*(ite->fnInit))() ) {
					result = false;
				} else {
					result = ((static_cast<T*>(this))->*(ite->fn))();
				}
			} else {
				result = ((static_cast<T*>(this))->*(ite->fn))();
			}
			if( result ) {
				m_success++;
				TRACE("[OK] : %s\n", ite->caption.c_str());
			} else {
				m_fail++;
				TRACE("[NG] : %s\n", ite->caption.c_str());
			}
			if( ite->fnFin ) {
				((static_cast<T*>(this))->*(ite->fnFin))();
			}
		}
		TRACE("total: successed = %d / failed = %d\n", m_success, m_fail);
		TRACE("----------------------------------------\n");
		return result();
	}

};

} // namespace tester
} // namespace spplib

#endif // !defined(SPPLIB_24A741E7_6E88_48F5_A3EE_2FCBA5038908_INCLUDE_GUARD)
