// simple_array.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <algorithm> // std::distance
#include <string>

#define SPPLIB_LOGGER_ENABLE_INLINE

#include "../../spplib/container/simple_array.hpp"
#include "../../spplib/logger.hpp"
//#include <vector>

using namespace spplib::container;

void simple_array<TCHAR[MAX_PATH+1]>::
	copy(pointer dest, const_pointer src)
{
	::memcpy(dest, src, sizeof(*dest));
}

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
//	SET_TRACE_OUTPUT_DBGOUT();

	int a[] = {1,2,3};
	const int b[] = {4,5,6,7,8};
	const int c[] = {3,4,5,6};
	simple_array<int> v(a, a + 3);
	const simple_array<int> x = b;
	//std::vector<TCHAR> vv(a, a + 3);

//	x.resize(x.size() - 1); // 文字列の場合は'\0'が最後に含まれてしまうのでここでは削除

	TRACE(_T("-- method -----\n"));
	TRACE(_T(".size()  = %d\n"), v.size());
	TRACE(_T("(TCHAR*) = %p\n"), (int*)v);
	TRACE(_T(".begin() = %p\n"), v.begin());
	TRACE(_T(".end()   = %p\n"), v.end());
	TRACE(_T("&[0]     = %p\n"), &v[0]);

	TRACE(_T("-- iterator -----\n"));
	for(simple_array<int>::iterator
			ite = v.begin(), first = v.begin(), last= v.end();
		ite != last; ++ite)
	{
		TRACE(_T("%2d:%d\n"), std::distance(first, ite), *ite);
	}

	TRACE(_T("-- operator [] -----\n"));
	for(int i = 0, len = v.size(); i < len; i++)
	{
		TRACE(_T("%2d:%d\n"), i, v[i]);
	}

	v += x;

	TRACE(_T("-- iterator -----\n"));
	for(simple_array<int>::iterator
			ite = v.begin(), first = v.begin(), last= v.end();
		ite != last; ++ite)
	{
		TRACE(_T("%2d:%d\n"), std::distance(first, ite), *ite);
	}

	v += b;

	TRACE(_T("-- iterator -----\n"));
	for(simple_array<int>::iterator
			ite = v.begin(), first = v.begin(), last= v.end();
		ite != last; ++ite)
	{
		TRACE(_T("%2d:%d\n"), std::distance(first, ite), *ite);
	}

	v = x;

	TRACE(_T("-- iterator -----\n"));
	for(simple_array<int>::iterator
			ite = v.begin(), first = v.begin(), last= v.end();
		ite != last; ++ite)
	{
		TRACE(_T("%2d:%d\n"), std::distance(first, ite), *ite);
	}

	v = c;

	TRACE(_T("-- iterator -----\n"));
	for(simple_array<int>::iterator
			ite = v.begin(), first = v.begin(), last= v.end();
		ite != last; ++ite)
	{
		TRACE(_T("%2d:%d\n"), std::distance(first, ite), *ite);
	}

	simple_array<int>().swap(v);

	simple_array<std::string> sv;

	sv += std::string("abc");
	sv += std::string("test");

	TRACE(_T("-- iterator -----\n"));
	for(simple_array<std::string>::iterator
			ite = sv.begin(), first = sv.begin(), last= sv.end();
		ite != last; ++ite)
	{
#if defined(UNICODE)
		TRACE(_T("%2d:%S\n"), std::distance(first, ite), ite->c_str());
#else
		TRACE(_T("%2d:%s\n"), std::distance(first, ite), ite->c_str());
#endif
	}

	simple_array<TCHAR[MAX_PATH+1]> paths;
	TCHAR path[MAX_PATH+1];

	lstrcpy(path, _T("123")); paths += path;
	lstrcpy(path, _T("456")); paths += path;
	lstrcpy(path, _T("aaa")); paths += path;

	TRACE(_T("-- iterator -----\n"));
	for(simple_array<TCHAR[MAX_PATH+1]>::iterator
			ite = paths.begin(), first = paths.begin(), last= paths.end();
		ite != last; ++ite)
	{
		TRACE(_T("%2d:%s\n"), std::distance(first, ite), ite);
	}

	return 0;
}

