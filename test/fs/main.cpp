// directory.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>

#include <spplib/fs.hpp>
#include <spplib/fs/directory_iterator.hpp>

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace spplib::fs;

	create_directory(_T("test\\test2"));
	TRACE(_T("exist(\"%s\") = %s\n"), _T("test\\test2"), exist(_T("test\\test2")) ? _T("true") : _T("false"));

	remove_directory(_T("test"));
	TRACE(_T("exist(\"%s\") = %s\n"), _T("test\\test2"), exist(_T("test\\test2")) ? _T("true") : _T("false"));

	for(directory_iterator ite(_T("*")), end;
		ite != end; ++ite)
	{
		TRACE(_T("%s %8d %8d %8d %s\n")
			, ite.is_dir() ? _T("<DIR>") : _T("     ")
			, ite.size()
			, get_file_size(ite)
			, get_file_size(ite.fullpath())
			, ite.filename()
			);
	}

	return 0;
}

