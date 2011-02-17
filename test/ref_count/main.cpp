// main.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <algorithm>

#define SPPLIB_LOGGER_ENABLE_INLINE
#include "../../spplib/logger.hpp"

// http://kkkon.hp.infoseek.co.jp/tips/VC/allMemoryLeak.shtml
#ifdef	__AFXWIN_H__            // MFCのウィンドウを使う場合に限定しています
#else
 #if defined(_DEBUG)
  #define _CRTDBG_MAP_ALLOC
// Macros for setting or clearing bits in the CRT debug flag 
#ifdef _DEBUG
#define  SET_CRT_DEBUG_FIELD(a)   _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define  CLEAR_CRT_DEBUG_FIELD(a) _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
#define  SET_CRT_DEBUG_FIELD(a)   ((void) 0)
#define  CLEAR_CRT_DEBUG_FIELD(a) ((void) 0)
#endif
//  void*  operator new(size_t size, const char *filename, int linenumber);
//  void   operator delete(void * _P, const char *filename, int linenumber);
 void* operator new(size_t size, const char *filename, int linenumber)
	{ return _malloc_dbg(size, _NORMAL_BLOCK, filename, linenumber); }
 void   operator delete(void * _P, const char *filename, int linenumber)
	{ _free_dbg(_P, _NORMAL_BLOCK); return; }
 #endif
 #include <malloc.h>
 #include <crtdbg.h>
#endif

#ifdef _DEBUG
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#endif

#ifdef _UNICODE
#define _T__FUNCTION__ _T(__FUNCTION__)
#else
#define _T__FUNCTION__ __FUNCTION__
#endif

//////////////////////////////////////////////////////////////////////

#include "../../spplib/ref_count.hpp"

typedef struct FOO_CRTP_TYPE
	: spplib::ref_count_crtp<FOO_CRTP_TYPE>
{
	int x;
	FOO_CRTP_TYPE() : x(0)		{ TRACE(_T("%08p: %s()\n"), this, _T__FUNCTION__); }
	~FOO_CRTP_TYPE()			{ TRACE(_T("%08p: %s()\n"), this, _T__FUNCTION__); }
	virtual void release()		{ TRACE(_T("%08p: %s()\n"), this, _T__FUNCTION__); }
} FOO_CRTP_TYPE;

typedef struct FOO_TYPE_BASE
{
	int x;
	FOO_TYPE_BASE() : x(0)	{ TRACE(_T("%08p: %s()\n"), this, _T__FUNCTION__); }
	~FOO_TYPE_BASE()		{ TRACE(_T("%08p: %s()\n"), this, _T__FUNCTION__); }
	void release()			{ TRACE(_T("%08p: %s()\n"), this, _T__FUNCTION__); }
} FOO_TYPE_BASE;
typedef spplib::ref_count<FOO_TYPE_BASE> FOO_TYPE;

typedef struct HOGE_BASE_TYPE
{
	int x;
	HOGE_BASE_TYPE() : x(0)		{ TRACE(_T("%08p: %s()\n"), this, _T__FUNCTION__); }
	virtual ~HOGE_BASE_TYPE()	{ TRACE(_T("%08p: %s()\n"), this, _T__FUNCTION__); }
} HOGE_BASE_TYPE;
typedef struct HOGE_SUB_TYPE : public HOGE_BASE_TYPE
{
	HOGE_SUB_TYPE()				{ TRACE(_T("%08p: %s()\n"), this, _T__FUNCTION__); }
	virtual ~HOGE_SUB_TYPE()	{ TRACE(_T("%08p: %s()\n"), this, _T__FUNCTION__); }
} HOGE_SUB_TYPE;
typedef spplib::ref_count_ptr<HOGE_BASE_TYPE> HOGE_TYPE;

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _DEBUG
	::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	TRACE(_T("------------- spplib::ref_count_crtp<T> -------------\n"));
	{
		FOO_CRTP_TYPE v1;
		v1.x = 1;
		{
			FOO_CRTP_TYPE v2 = v1;
			{
				FOO_CRTP_TYPE v3 = v1;
				FOO_CRTP_TYPE v4;
				{
					FOO_CRTP_TYPE v5 = v2;
				//	v5 = v3; // 指定不可
				//	v5 = v4; // 指定不可
				}
			}
		}
	}

	TRACE(_T("------------- spplib::ref_count<T> -------------\n"));
	{
		FOO_TYPE v1;
		v1->x = 1;
		{
			FOO_TYPE v2 = v1;
			{
				FOO_TYPE v3 = v1;
				FOO_TYPE v4;
				{
					FOO_TYPE v5 = v2;
					int xx = v5->x;
					v5 = v3;
					v5 = v4;
				}
			}
		}
	}

	TRACE(_T("------------- spplib::ref_count_ptr<T> -------------\n"));
	{
		HOGE_TYPE v1(new HOGE_SUB_TYPE);
		v1->x = 1;
		{
			HOGE_TYPE v2 = v1;
			{
				HOGE_TYPE v3 = v1;
				HOGE_TYPE v4(new HOGE_BASE_TYPE);
				{
					HOGE_TYPE v5 = v2;
					int xx = (*v5).x;
					HOGE_BASE_TYPE y = *v5;
					v5 = v3;
					v5 = v4;
				}
			}
		}
	}

	return 0;
}

