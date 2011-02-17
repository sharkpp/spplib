/**
 * @file  spplib/system/thread.hpp
 * @brief スレッドクラス
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <windows.h>
#include <process.h>
#include <functional>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_DF537A69_F479_41BC_8EDB_3261F57B091C_INCLUDE_GUARD
#define SPPLIB_DF537A69_F479_41BC_8EDB_3261F57B091C_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// system library namespace
 namespace system {

/**
 * @brief スレッドクラス
 * @code
 *  unsigned int x() { for(int i = 0; i < 100000; i++); return 0; }
 *  spplib::system::thread th(x);
 *  th.run();
 *  th.wait();
 * @endcode
 */
class thread
{
 	struct placeholder {
		virtual ~placeholder() {};
		virtual unsigned int operator()() = 0;
	};
	template<typename Ty>
	struct holder : public placeholder {
		Ty m_fn;
		holder(Ty fn)
			: m_fn(fn) {}
		virtual ~holder() { }
		virtual unsigned int operator()()
			{ return m_fn(); }
	};
	template<typename Ty, typename Ta>
	struct holder2 : public placeholder {
		Ty m_fn;
		Ta m_cls;
		holder2(Ty fn, Ta cls)
			: m_fn(fn) , m_cls(cls) {}
		virtual ~holder2() { }
		virtual unsigned int operator()()
			{ return (m_cls->*m_fn)(); }
	};
	placeholder *	m_holder;

	HANDLE			m_handle;
	DWORD			m_thread_id;
	volatile bool	m_abort_signal;

protected:

	thread(const thread &);
	thread operator = (const thread &);

	/// スレッドプロシージャ呼び出し補助関数
	static unsigned int CALLBACK procedure_helper(void * param)
	{
		thread * this_ = static_cast<thread *>(param);
		return (*this_->m_holder)();
	}

protected:

	/**
	 * @brief  スレッドプロシージャ
	 * @return 戻り値
	 */
	virtual unsigned int procedure()
	{
		return 0;
	}

public:

	/// コンストラクタ
	thread()
		: m_handle(NULL)
		, m_abort_signal(false)
		, m_holder(NULL)
	{
		m_holder = new holder2<unsigned int (thread::*)(), thread*>(&thread::procedure, this);
	}

	/**
	 * @brief     コンストラクタ
	 * @param[in] fn スレッドプロシージャ
	 */
	template<typename Tfn>
	thread(Tfn fn)
		: m_handle(NULL)
		, m_abort_signal(false)
		, m_holder(new holder<Tfn>(fn))
	{
	}

	/// デストラクタ
	virtual ~thread()
	{
		destory();
		delete m_holder;
	}

	/**
	 * @brief     スレッドの破棄
	 * @param[in] wait  待機時間、初期値は処理が完了するまで待つ
	 * @retval    true  処理の正常終了
	 * @retval    false 処理が失敗した
	 */
	bool destory(unsigned int wait = INFINITE)
	{
		stop();
		if( ::WaitForSingleObject(m_handle, wait) == WAIT_TIMEOUT )
		{
			::TerminateThread(m_handle, 0);
		}
		::CloseHandle(m_handle);
		m_handle = NULL;
		m_abort_signal = false;
		return true;
	}

	/**
	 * @brief     スレッドの実行開始
	 * @retval    true  処理の正常終了
	 * @retval    false 処理が失敗した
	 */
	bool run()
	{
		if( m_handle )
		{
			destory();
		}
		m_handle =
			(HANDLE)::_beginthreadex(NULL, 0,
			                 procedure_helper,
			                 this,
			                 CREATE_SUSPENDED,
			                 (unsigned int *)&m_thread_id 
			                 );
		::ResumeThread(m_handle);
		return NULL != m_handle;
	}

	/**
	 * @brief     スレッドの終了待機
	 * @param[in] wait  待機時間、初期値は処理が完了するまで待つ
	 * @retval    true  処理の正常終了
	 * @retval    false 処理が失敗した
	 */
	bool wait(unsigned int wait = INFINITE) const
	{
		return ::WaitForSingleObject(m_handle, wait) == WAIT_OBJECT_0;
	}

	/**
	 * @brief     スレッドの実行状態を取得
	 * @retval    true  スレッドが実行中
	 * @retval    false スレッドは停止している
	 */
	bool running() const
	{
		return ::WaitForSingleObject(m_handle, 0) == WAIT_OBJECT_0;
	}

	/**
	 * @brief     スレッドの終了要求
	 * @retval    true  処理の正常終了
	 * @retval    false 処理が失敗した
	 */
	bool stop()
	{
		m_abort_signal = true;
		return true;
	}

	/**
	 * @brief     スレッドの終了要求が出ているか？
	 * @retval    true  終了が要求された
	 * @retval    false 終了要求は出ていない
	 */
	bool aborted() const
	{
		return m_abort_signal;
	}

};


 } // namespace system
} // namespace spplib

#endif // !defined(SPPLIB_DF537A69_F479_41BC_8EDB_3261F57B091C_INCLUDE_GUARD)
