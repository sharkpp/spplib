// main.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <vector>
#include <objbase.h>
#include <urlmon.h>

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>
#include <spplib/string.hpp>
#include <spplib/file.hpp>
#include <spplib/fs/directory_iterator.hpp>

#include <spplib/utility/contents_type.hpp>

using namespace spplib;

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	//HRESULT hr;
	//hr = CoInitializeEx(0, COINIT_MULTITHREADED); 
	//if( FAILED(hr) )
	//{
	//	TRACE(_T("Failed initialize COM library(%08X)\r\n", hr);
	//	return 1;
	//}
	spplib::tstring mime_
		= utility::get_contents_type("test.zip", NULL, 0);

	TRACE(_T("%-30s : %s\n"), mime_.c_str(), "test.zip");

	for(fs::directory_iterator ite(_T("debug\\*")), end;
		ite != end; ++ite)
	{
		size_t len = 0;
		TCHAR *buff_ = NULL;
		if( file::get(ite.fullpath(), &buff_, &len) )
		{
			std::vector<TCHAR> buff(buff_, buff_ + len);
			delete [] buff_;

			if( !buff.empty() )
			{
				spplib::tstring mime
					= utility::get_contents_type(ite.fullpath(), &buff[0], buff.size());

				TRACE(_T("%-30s : %s\n"), mime.c_str(), ite.fullpath());
			}
		}
	}

//	CoUninitialize();

	return 0;
}

