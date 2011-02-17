/**
 * @file  spplib/process.hpp
 * @brief プロセス起動メソッド群
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <windows.h>	// 

#include <algorithm>
#include <vector>

#include "ref_count.hpp"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_72D16AF9_C675_4401_AD29_6294AC2CD3AB_INCLUDE_GUARD
#define SPPLIB_72D16AF9_C675_4401_AD29_6294AC2CD3AB_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// process function namespace
 namespace process {

//////////////////////////////////////////////////////////////////////

class redirect_object
{
protected:

	HANDLE m_handle;

public:

	redirect_object(HANDLE handle = NULL)
		: m_handle(handle)
//	{ TRACE(_T("%s(): m_handle = %p\n"), _T(__FUNCTION__), m_handle); }
	{ }

	virtual ~redirect_object()
//		{ ::CloseHandle(m_handle); TRACE(_T("%s(): m_handle = %p\n"), _T(__FUNCTION__), m_handle); }
		{ ::CloseHandle(m_handle); }

	virtual bool release()
		{ return true; }

	HANDLE get() const
		{ return m_handle; }

	bool empty() const
		{ return !m_handle; }

	virtual bool write(const void * data, size_t size)
	{
		DWORD dwWrite = 0;
		if( ::WriteFile(
				  m_handle
				, data, (DWORD)size
				, &dwWrite, NULL) != FALSE )
		{
			return true;
		}
		return false;
	}

	virtual bool read(void * data, size_t & size)
	{
		DWORD dwRead = 0;
		if( ::ReadFile(
				  m_handle
				, data, (DWORD)size
				, &dwRead, NULL) != FALSE )
		{
			size = dwRead;
			return true;
		}
		return false;
	}

};

//////////////////////////////////////////////////////////////////////

class pipe
	: public redirect_object
{
	pipe();

public:

	pipe(HANDLE handle)
		: redirect_object(handle) { }

	virtual ~pipe()
		{  }

	virtual bool read(void * data, size_t & size)
	{
		DWORD dwSize;
		if( ::PeekNamedPipe(m_handle, NULL, 0, NULL, &dwSize, NULL) &&
			0 < dwSize )
		{
			// 取得
			return
				::ReadFile(
					  m_handle
					, data, (std::min)(size, size_t(dwSize))
					, (LPDWORD)&size, NULL) != FALSE;
		}
		size = 0;
		return false;
	}

};

//////////////////////////////////////////////////////////////////////

class stdpipe
	: public redirect_object
{
	stdpipe();

public:

	stdpipe(DWORD nStdHandle)
		: redirect_object( ::GetStdHandle(nStdHandle) ) { }

	virtual ~stdpipe()
		{ m_handle = NULL; } // ※解放しちゃだめ！

	virtual bool read(void * data, size_t & size)
	{
		DWORD dwSize;
		if( ::PeekNamedPipe(m_handle, NULL, 0, NULL, &dwSize, NULL) &&
			0 < dwSize )
		{
			// 取得
			return
				::ReadFile(
					  m_handle
					, data, (std::min)(size, size_t(dwSize))
					, (LPDWORD)&size, NULL) != FALSE;
		}
		size = 0;
		return false;
	}

};

//////////////////////////////////////////////////////////////////////

class redirect
{
	typedef spplib::ref_count_ptr<redirect_object> redirect_object_type;
	redirect_object_type m_object;

public:

	redirect(redirect_object * object = NULL)
		: m_object(object)
	{ }

	~redirect()
		{ }

	const redirect & operator = (const redirect & src)
	{
		m_object = src.m_object;
		return *this;
	}

	bool empty() const
	{ return m_object.get() ? m_object->empty() : true; }

	HANDLE get() const
	{ return m_object.get() ? m_object->get() : NULL; }

	bool write(const void * data, size_t size)
		{ return m_object->write(data, size); }

	bool read(void * data, size_t & size)
		{ return m_object->read(data, size); }
};

//////////////////////////////////////////////////////////////////////

inline
redirect in(const TCHAR * fname)
{
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
	sa.bInheritHandle = TRUE;
	return redirect(new redirect_object( ::CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, &sa, CREATE_ALWAYS, 0, NULL) ));
}

inline
redirect out(const TCHAR * fname, bool append = false)
{
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
	sa.bInheritHandle = TRUE;
	HANDLE handle = ::CreateFile(fname, GENERIC_WRITE, FILE_SHARE_READ, &sa, append ? OPEN_ALWAYS : CREATE_ALWAYS, 0, NULL);
	if( append ) {
		LONG l = 0;
		::SetFilePointer(handle, 0, &l, FILE_END);
	}
	return redirect(new redirect_object( handle ));
}

//////////////////////////////////////////////////////////////////////

class process
{
public:
#ifdef _UNICODE
	std::wstring m_command;
	std::wstring m_arguments;
#else
	std::string m_command;
	std::string m_arguments;
#endif
	redirect m_stdin;
	redirect m_stdout;
	redirect m_stderr;

	redirect m_stdin_w;
	redirect m_stdout_r;

	HANDLE		m_handle;
	DWORD		m_id;

	HANDLE		m_thread_handle;
	DWORD		m_thread_id;

private:

	//redirect create_pipe(redirect & pipe_buff, bool read_pipe)
	//{
	//	HANDLE read_handle, write_handle;

	//	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
	//	sa.bInheritHandle = TRUE;

	//	if( ::CreatePipe(&read_handle, &write_handle, &sa, 0) )
	//	{
	//		DuplicateHandle( GetCurrentProcess(),read_pipe ? write_handle : read_handle, GetCurrentProcess(), NULL, NULL, FALSE, DUPLICATE_SAME_ACCESS);
	//		pipe_buff = redirect(new pipe(read_pipe ? read_handle : write_handle));
	//	//	::CloseHandle(read_pipe ? write_handle : read_handle);
	//	}
	//	return redirect(new pipe(read_pipe ? write_handle : read_handle));
	//}

public:

#ifdef _UNICODE
	process(const std::wstring & s)
#else
	process(const std::string & s)
#endif
		: m_command(s)
		, m_stdin(  new stdpipe(STD_INPUT_HANDLE) )
		, m_stdout( new stdpipe(STD_OUTPUT_HANDLE) )
		, m_stderr( new stdpipe(STD_ERROR_HANDLE) )
		, m_handle(NULL)
		, m_id(0)
		, m_thread_handle(NULL)
		, m_thread_id(0)
	{
	}

	process(const TCHAR * s = NULL)
		: m_command(s ? s : _T(""))
		, m_stdin(  new stdpipe(STD_INPUT_HANDLE) )
		, m_stdout( new stdpipe(STD_OUTPUT_HANDLE) )
		, m_stderr( new stdpipe(STD_ERROR_HANDLE) )
		, m_handle(NULL)
		, m_id(0)
		, m_thread_handle(NULL)
		, m_thread_id(0)
	{
	}

	~process()
	{
		::CloseHandle(m_handle);
		::CloseHandle(m_thread_handle);
	}

	const process & operator = (const process & src)
	{
		m_command		= src.m_command;
		m_stdin			= src.m_stdin;
		m_stdout		= src.m_stdout;
		m_stderr		= src.m_stderr;
		m_handle		= src.m_handle;
		m_id			= src.m_id;
		m_thread_handle	= src.m_thread_handle;
		m_thread_id		= src.m_thread_id;
		const_cast<process&>(src).m_thread_handle = NULL;
		const_cast<process&>(src).m_handle = NULL;
		return *this;
	}

	void set_stdin(const redirect & in)
	{
		m_stdin = in;
	}

	void set_stdout(const redirect & out)
	{
		m_stdout = out;
	}

	const redirect & get_stdin()
	{
	//	if( m_stdin.empty() )
	//	{
	//		return create_pipe(m_stdin, true);
	//	}
		return m_stdin;
	}

	redirect get_stdout()
	{
		HANDLE read_handle, write_handle;
		SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
		sa.bInheritHandle = TRUE;
		::CreatePipe(&read_handle, &write_handle, &sa, 0);
		m_stdout = redirect(new pipe(write_handle));
		return redirect(new pipe(read_handle));
	}

	bool run(bool suspend = false)
	{
#ifdef _UNICODE
		std::wstring params = m_command;
#else
		std::string params = m_command;
#endif
		PROCESS_INFORMATION	pi;
		STARTUPINFO			si;
		DWORD				wCreateFllags;

		::memset(&pi, 0, sizeof(pi));
		::memset(&si, 0, sizeof(si));

		si.cb			= sizeof(si);

		wCreateFllags	= NORMAL_PRIORITY_CLASS
						| (suspend ? CREATE_SUSPENDED : 0);

		if( m_stdin.get() ||
			m_stdout.get() ||
			m_stderr.get() )
		{
			si.dwFlags		|= STARTF_USESTDHANDLES;
			si.hStdInput	 = m_stdin.get()  ? m_stdin.get()  : ::GetStdHandle(STD_INPUT_HANDLE);
			si.hStdOutput	 = m_stdout.get() ? m_stdout.get() : ::GetStdHandle(STD_OUTPUT_HANDLE);
			si.hStdError	 = m_stderr.get() ? m_stderr.get() : ::GetStdHandle(STD_ERROR_HANDLE);

			si.dwFlags		|= STARTF_USESHOWWINDOW;
			si.wShowWindow	 = SW_HIDE;
			wCreateFllags	|= CREATE_NEW_CONSOLE;
		}

//		TRACE(_T("%08X,%08X %s\n"),m_stdin.get(),m_stdout.get(),m_command.c_str());

		if( !CreateProcess(
					NULL,		// pointer to name of executable module 
					(LPTSTR)params.c_str(),	// pointer to command line string
					NULL,					// pointer to process security attributes 
					NULL,					// pointer to thread security attributes 
					TRUE,					// handle inheritance flag 
					wCreateFllags,			// creation flags 
					NULL,					// pointer to new environment block 
					NULL,					// pointer to current directory name 
					&si,					// pointer to STARTUPINFO 
					&pi						// pointer to PROCESS_INFORMATION	
				) )
		{
			return false;
		}

		m_stdin  = redirect();
		m_stdout = redirect();

		m_handle = pi.hProcess;
		m_id     = pi.dwProcessId;

		m_thread_handle = pi.hThread;
		m_thread_id		= pi.dwThreadId;

		return true;
	}

	bool wait_for_initialize(unsigned int wait_time = INFINITE)
	{
		return FALSE != ::WaitForInputIdle(m_handle, wait_time);
	}

	bool wait(unsigned int wait_time = INFINITE)
	{
		return WAIT_OBJECT_0 == ::WaitForSingleObject(m_handle, wait_time);
	}

	bool resume()
	{
		return FALSE != ::ResumeThread(m_thread_handle);
	}

	unsigned int get_exit_code()
	{
		DWORD dwExitCode = 0;
		return ::GetExitCodeProcess(m_handle, &dwExitCode)
					? dwExitCode : (unsigned int)-1;
	}

	HANDLE handle() const
	{
		return m_handle;
	}

	static HANDLE get_handle(const process & v)
	{
		return v.handle();
	}
};

//////////////////////////////////////////////////////////////////////

class execute
{
public:
	std::vector<process> m_list;

public:

	execute() {}

	execute(const process & proc)
	{
		*this = proc;
	}

	execute & operator = (const process & proc)
	{
		m_list.push_back(proc);
		return *this;
	}

	bool wait(unsigned int wait_time = INFINITE)
	{
		std::vector<HANDLE> handles( m_list.size() );
		std::transform(m_list.begin(), m_list.end(), handles.begin(), process::get_handle);
		return WAIT_OBJECT_0
			== ::WaitForMultipleObjects(handles.size(), &handles[0], TRUE, wait_time);
	}

	void run()
	{
		TRACE(_T("-----------------\n"));
		for(std::vector<process>::iterator
				ite = m_list.begin(),
				last= m_list.end(); 
			ite != last; ++ite)
		{
			TRACE(_T(">>%s\n")
				, ite->m_command.c_str()
				);
		}

		// リダイレクトを登録
		for(std::vector<process>::iterator
				ite = m_list.begin(),
				last= m_list.end(); 
			ite != last; ++ite)
		{
			std::vector<process>::iterator
				ite2 = ite + 1;
			if( ite2 != last )
			{
				TRACE(_T(">>%s -> %s\n")
					, ite->m_command.c_str()
					, ite2->m_command.c_str()
					);
				// LEFT out -> in RIGHT
				ite2->set_stdin( ite->get_stdout() );
			}
		}

		for(std::vector<process>::iterator
				ite = m_list.begin(),
				last= m_list.end(); 
			ite != last; ++ite)
		{
			ite->run(true);
		}

		for(std::vector<process>::iterator
				ite = m_list.begin(),
				last= m_list.end(); 
			ite != last; ++ite)
		{
			ite->resume();
		}
	}
};

//////////////////////////////////////////////////////////////////////

inline
execute operator | (const process & a, const process & b)
{
	execute x;
	x.m_list.push_back(a);
	x.m_list.push_back(b);
	return x;
}

inline
execute operator | (const execute & a, const process & b)
{
	execute x = a;
	x.m_list.push_back(b);
	return x;
}

inline
execute operator | (const process & a, const execute & b)
{
	execute x;
	x.m_list.push_back(a);
	std::copy(b.m_list.begin(), b.m_list.end(), std::back_inserter(x.m_list));
	return x;
}

inline
execute operator | (const execute & a, const execute & b)
{
	execute x;
	std::copy(a.m_list.begin(), a.m_list.end(), std::back_inserter(x.m_list));
	std::copy(b.m_list.begin(), b.m_list.end(), std::back_inserter(x.m_list));
	return x;
}

inline
execute operator > (const execute & a, const TCHAR * b)
{
	execute x;
	std::copy(a.m_list.begin(), a.m_list.end(), std::back_inserter(x.m_list));
	x.m_list.back().set_stdout( out(b) );
	return x;
}

inline
execute operator >> (const execute & a, const TCHAR * b)
{
	execute x;
	std::copy(a.m_list.begin(), a.m_list.end(), std::back_inserter(x.m_list));
	x.m_list.back().set_stdout( out(b, true) );
	return x;
}

 } // process spplib
} // namespace spplib

#endif // !defined(SPPLIB_72D16AF9_C675_4401_AD29_6294AC2CD3AB_INCLUDE_GUARD)

//EOF
