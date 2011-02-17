// test.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <errno.h>
#include <io.h>
#include <fcntl.h>

#define SPPLIB_LOGGER_ENABLE_INLINE
#include "../../../spplib/logger.hpp"

int _tmain(int argc, _TCHAR* argv[])
{
	bool bInMode = 1 < argc && !strcmpi(argv[1], "in");
	SET_TRACE_OUTPUT_LOGFILE_FLASH(bInMode ? "in.log" : "out.log");

	setvbuf( stdin,  NULL, _IONBF, 0 );
	setvbuf( stdout, NULL, _IONBF, 0 );

	TRACE( "[%08X] begin\n", GetCurrentProcessId());
	printf("[%08X] begin\n", GetCurrentProcessId());
	for(int i = 0; i < argc; i++) {
		TRACE( "[%08X] argv[%d] = \"%s\"\n", GetCurrentProcessId(), i, argv[i]);
		printf("[%08X] argv[%d] = \"%s\"\n", GetCurrentProcessId(), i, argv[i]);
	}
	if( bInMode )
	{
#ifdef _WIN32
		// binaryモードに変更
		if( _setmode( _fileno(stdin) , _O_BINARY ) == -1 ){
			fprintf( stderr , "System error [%d]\n" , errno );
			return 1 ;

		}
#endif
		while( !feof(stdin) && !ferror(stdin) )
		{
			char buff[128+1];
			if( fgets(buff, 128, stdin) ) {
				TRACE( "[%08X] %s", GetCurrentProcessId(), buff);
				printf("[%08X] %s", GetCurrentProcessId(), buff);
			}
		}
	}
	else
	{
		for(int i = 0; i < 100; i++) 
		{
			TRACE( "[%08X] line %d\n", GetCurrentProcessId(), i + 1);
			printf("[%08X] line %d\n", GetCurrentProcessId(), i + 1);
			Sleep(10);
		}
	}
	TRACE( "[%08X] quit\n", GetCurrentProcessId());
	printf("[%08X] quit\n", GetCurrentProcessId());
	return 0;
}

