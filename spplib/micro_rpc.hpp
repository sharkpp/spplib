/**
 * @file  spplib/micro_rpc.hpp
 * @brief マイクロRPCクラス定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <windows.h>
#include <objbase.h>
#include <spplib/container/shmvqueue.hpp>
#include <spplib/string.hpp>
#include <vector>
#include <map>

#ifndef SPPLIB_8D665796_0712_448F_A901_36D6ED599DBC_INCLUDE_GUARD
#define SPPLIB_8D665796_0712_448F_A901_36D6ED599DBC_INCLUDE_GUARD

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

/// sharkpp class library namespace
namespace spplib {
 /// micro RPC class library namespace
 namespace micro_rpc {

//--------------------------------------------------------------------
class core
{
protected:

	core() {}

	typedef int ConnectIdType;

#ifdef  _WIN64
	typedef unsigned int HANDLE_;
#else
	typedef unsigned int HANDLE_;
#endif

	typedef struct CommonInfoType {
		DWORD	process_id;
		HANDLE_	event_handle;
		int		queue_size;
	} CommonInfoType;

	typedef enum {
		FUNC_CONNECT = 0,
		FUNC_USER_BASE,
	};
};

//--------------------------------------------------------------------
class server
	: protected core
{
private:

	typedef struct ConnectInfoType {
		std::vector<std::string::value_type>
				name;
		int		size;
		spplib::container::shmvqueue
			*	queue;
		HANDLE	event_handle;
	} ConnectInfoType;

	class shared_mem_initializer {
		CommonInfoType m_common_info;
	public:
		shared_mem_initializer(size_t queue_size, HANDLE event_handle) {
			m_common_info.process_id     = ::GetCurrentProcessId();
			m_common_info.event_handle   = reinterpret_cast<HANDLE_>(event_handle);
			m_common_info.queue_size     = static_cast<int>(queue_size);
		}
		~shared_mem_initializer() {}
		operator const void * () const
			{ return reinterpret_cast<const void*>(&m_common_info); }
	};

	struct GetProcessHandle {
		HANDLE handle;
		GetProcessHandle(DWORD process_id)
			: handle(::OpenProcess(PROCESS_DUP_HANDLE, FALSE, process_id)) {}
		~GetProcessHandle() { ::CloseHandle(handle); }
		operator HANDLE () const
			{ return handle; }
	};

private:

	typedef void (server::*FuncType)();

	HANDLE			m_event_handle;
	bool			m_abort;

	spplib::shared_memory
					m_shmcommon;

	spplib::container::shmvqueue
					m_queue;
	spplib::container::shmvqueue
		*			m_result_queue;

	std::map<int, FuncType>
					m_vtbl;

	std::map<ConnectIdType, ConnectInfoType>
					m_connection;

	ConnectIdType	m_connect_index;

protected:

	server(const TCHAR * shared_name, size_t size)
		: m_event_handle(::CreateEvent(NULL, FALSE, FALSE, NULL))
		, m_abort(false)
		, m_shmcommon(spplib::tstring(spplib::tstring(shared_name) + _T("@common")).c_str(),
		              sizeof(CommonInfoType),
		              shared_mem_initializer(size, m_event_handle))
		, m_queue(shared_name, size)
		, m_result_queue(NULL)
		, m_connect_index(0)
	{
		define_function(FUNC_CONNECT, &server::connect);
	}

	template<class Class>
	bool define_function(int index, void (Class::*function)() )
	{
		m_vtbl[index] = reinterpret_cast<FuncType>(function);
		return true;
	}

	template<typename T>
	void pop(T & v)
	{
		m_queue.front(v);
		m_queue.pop_front();
	}

	void pop(void * buff, size_t size)
	{
		m_queue.front(buff, size);
		m_queue.pop_front();
	}

	template<typename T>
	void push(const T & v)
	{
		m_result_queue->push_back(v);
	}

	void push(const void * data, size_t size)
	{
		m_result_queue->push_back(data, size);
	}

	void lock()
	{
		m_result_queue->lock();
	}

	void unlock()
	{
		m_result_queue->unlock();
	}

	void abort()
	{
		m_abort = true;
	}

private:

	void connect()
	{
		DWORD   process_id;
		HANDLE_ event_handle;
		//
		ConnectInfoType info;
		                info.name.resize(32 + 1);
		                info.size = 0;
		pop(&info.name[0], info.name.size());
		pop(info.size);
		pop(process_id);
		pop(event_handle);
		//
		::DuplicateHandle(GetProcessHandle(process_id), (HANDLE)event_handle,
		                  ::GetCurrentProcess(), &info.event_handle,
		                  EVENT_ALL_ACCESS, FALSE, DUPLICATE_SAME_ACCESS);
#ifdef _UNICODE
		info.queue = new spplib::container::shmvqueue(spplib::string::mbtowc(std::string(&info.name[0])).c_str(), info.size);
#else
		info.queue = new spplib::container::shmvqueue(&info.name[0], info.size);
#endif
		ConnectIdType connect_index = m_connect_index++;
		m_connection[connect_index] = info;
		m_result_queue = info.queue;
		// 戻り値
		push(connect_index);
		::SetEvent(info.event_handle);
	}

public:

	virtual ~server()
	{
		::CloseHandle(m_event_handle);
		for(std::map<ConnectIdType, ConnectInfoType>::iterator
				ite = m_connection.begin(),
				last = m_connection.end();
			ite != last; ++ite)
		{
			delete ite->second.queue;
			::CloseHandle(ite->second.event_handle);
		}
		m_connection.clear();
	}

	bool wait()
	{
		switch( ::WaitForSingleObject(m_event_handle, 1000) )
		{
		case WAIT_OBJECT_0:
			while( !m_queue.empty() && !m_abort )
			{
				m_queue.lock();
				ConnectIdType id = m_queue.front<ConnectIdType>();
				                   m_queue.pop_front();
				int index = m_queue.front<int>();
				            m_queue.pop_front();
				if( FUNC_CONNECT != index ) {
					m_result_queue = m_connection[id].queue;
				}
				(this->*(m_vtbl[index]))();
				m_queue.unlock();
				if( FUNC_CONNECT != index ) {
					::SetEvent(m_connection[id].event_handle);
				}
			}
			break;
		case WAIT_TIMEOUT:
			break;
		default:
			return false;
		}
		return !m_abort;
	}

};

//--------------------------------------------------------------------
class client
	: protected core
{
private:

	class unique_shared_name {
		char    m_shared_name[33];
		wchar_t m_shared_name_w[33];
	public:
		unique_shared_name() {
			GUID guid;
			::CoCreateGuid(&guid);
			// ※ANSIで書込み
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__) // from ISO working draft but not VC 2005 or defined by VC 2005, but not in draft
			::sprintf_s(m_shared_name, "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X"
				, guid.Data1, guid.Data2, guid.Data3
				, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3]
				, guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
				);
			::swprintf_s(m_shared_name_w, L"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X"
				, guid.Data1, guid.Data2, guid.Data3
				, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3]
				, guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
				);
#else
			::sprintf(m_shared_name, "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X"
				, guid.Data1, guid.Data2, guid.Data3
				, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3]
				, guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
				);
			::swprintf(m_shared_name_w, L"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X"
				, guid.Data1, guid.Data2, guid.Data3
				, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3]
				, guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
				);
#endif
		}
		~unique_shared_name() { }
		operator const wchar_t *() const
			{ return m_shared_name_w; }
		operator const char *() const
			{ return m_shared_name; }
		size_t size() const
			{ return sizeof(m_shared_name); }
	};

	struct GetProcessHandle {
		HANDLE handle;
		GetProcessHandle(DWORD process_id)
			: handle(::OpenProcess(PROCESS_DUP_HANDLE, FALSE, process_id)) {}
		~GetProcessHandle() { ::CloseHandle(handle); }
		operator HANDLE () const
			{ return handle; }
	};

private:

	unique_shared_name
					m_unique_shared_name;

	spplib::shared_memory
					m_shmcommon;

	DWORD			m_server_process_id;

	spplib::container::shmvqueue
		*			m_upstream;
	ConnectIdType	m_upstream_id;
	HANDLE			m_upstream_event;

	spplib::container::shmvqueue
					m_downstream;
	HANDLE			m_downstream_event;

protected:

	client(const TCHAR * shared_name, size_t size)
		: m_unique_shared_name()
		, m_shmcommon(spplib::tstring(spplib::tstring(shared_name) + _T("@common")).c_str(),
		              sizeof(CommonInfoType))
		, m_upstream_id(-1)
		, m_upstream_event(NULL)
		, m_upstream(NULL)
		, m_downstream(m_unique_shared_name, size)
		, m_downstream_event(::CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		CommonInfoType* common = static_cast<CommonInfoType*>(m_shmcommon.lock());
		if( !common || !common->process_id ) {
			throw 0;
			return;
		}
		m_upstream          = new spplib::container::shmvqueue(shared_name, common->queue_size);
		m_server_process_id = common->process_id;
		m_upstream_event    = DuplicateHandle((HANDLE)common->event_handle);
		m_shmcommon.unlock();
		connect();
	}

	HANDLE DuplicateHandle(HANDLE src_handle, DWORD dwAddtionalOptions = 0)
	{
		HANDLE handle;
		::DuplicateHandle(GetProcessHandle(m_server_process_id), src_handle,
		                  ::GetCurrentProcess(), &handle,
		                  EVENT_ALL_ACCESS, FALSE, DUPLICATE_SAME_ACCESS|dwAddtionalOptions);
		return handle;
	}

public:

	virtual ~client()
	{
		delete m_upstream;
		::CloseHandle(m_upstream_event);
		::CloseHandle(m_downstream_event);
	}

	template<typename T>
	void push(const T & data)
	{
		m_upstream->push_back(data);
	}

	void push(const void * data, size_t size)
	{
		m_upstream->push_back(data, size);
	}

	template<typename T>
	void pop(T & v)
	{
		m_downstream.front(v);
		m_downstream.pop_front();
	}

	void pop(void * buff, size_t size)
	{
		m_downstream.front(buff, size);
		m_downstream.pop_front();
	}

	template<typename T>
	T ret()
	{
		T r;
		m_downstream.front(r);
		m_downstream.pop_front();
		return r;
	}

	void lock()
	{
		m_upstream->lock();
		push(m_upstream_id);
	}

	void call()
	{
		m_upstream->unlock();
		//
		::SetEvent(m_upstream_event);
		WaitForSingleObject(m_downstream_event, 10000);
	}

private:

	void connect()
	{
		lock();
		push(FUNC_CONNECT);
		push(static_cast<const char*>(m_unique_shared_name), m_unique_shared_name.size());
		push((int)m_downstream.capacity());
		push(GetCurrentProcessId());
		push((HANDLE_)m_downstream_event);
		call();
		// 戻り値
		pop(m_upstream_id);
	}

};


 } // namespace micro_rpc
} // namespace spplib

#endif // !defined(SPPLIB_8D665796_0712_448F_A901_36D6ED599DBC_INCLUDE_GUARD)
