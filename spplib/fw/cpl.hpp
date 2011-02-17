/**
 * @file  spplib/fw/cpl.hpp
 * @brief コントロールパネル フレームワーク クラス定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <windows.h>
#include <tchar.h>
#include <cpl.h>
//#include "spplib/logger.hpp"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_F63C7CE6_D9BF_4770_BE09_4693C0AF1343_INCLUDE_GUARD
#define SPPLIB_F63C7CE6_D9BF_4770_BE09_4693C0AF1343_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// framework namespace
 namespace fw {

class cpl
{
public:

	cpl();

	virtual ~cpl()
	{
	}

	LONG operator()(HWND hwndCPl, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
	{
		static const TCHAR *CPL_TYPE[] = {
			_T("****"),
			_T("CPL_INIT"),     _T("CPL_GETCOUNT"), _T("CPL_INQUIRE"),  
			_T("CPL_SELECT"),   _T("CPL_DBLCLK"),   _T("CPL_STOP"),     
			_T("CPL_EXIT"),     _T("CPL_NEWINQUIRE"), 
		};
//		TRACE(_T("%S(%p,%s(%u),%d,%d)\n"), __FUNCTION__, hwndCPl, CPL_TYPE[uMsg % 9], uMsg, lParam1, lParam2);

		switch(uMsg)
		{
		case CPL_INIT:
			return Init() ? TRUE : FALSE;

		case CPL_GETCOUNT:
			return GetCount();

		case CPL_INQUIRE:
			Inquire(LONG(lParam1), (LPCPLINFO)lParam2);
			break;

	//	case CPL_SELECT: // is not used
	//		break;

		case CPL_DBLCLK:
			Dblclk(LONG(lParam1), (LONG_PTR)lParam2);
			break;

		case CPL_STOP:
			Stop(LONG(lParam1), (LONG_PTR)lParam2);
			break;

		case CPL_EXIT:
			Exit();
			break;

		case CPL_NEWINQUIRE:
			NewInquire(LONG(lParam1), (LPCPLINFO)lParam2);
			break;

		default:
			return FALSE;
		}

		return TRUE;
	}
	
protected:

	virtual bool Init() { return true; }
	virtual LONG GetCount() { return 1; }
	virtual void Inquire(LONG nDlgIndex, LPCPLINFO lpCplInfo) {}
	virtual void Dblclk(LONG nDlgIndex, LONG_PTR lData) {}
	virtual void Stop(LONG nDlgIndex, LONG_PTR lData) {}
	virtual void Exit() {}
	virtual void NewInquire(LONG nDlgIndex, LPCPLINFO lpCplInfo) {}

protected:

};

 } // namespace fw
} // namespace spplib

#endif // !defined(SPPLIB_F63C7CE6_D9BF_4770_BE09_4693C0AF1343_INCLUDE_GUARD)
