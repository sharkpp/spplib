/**
 * @file  spplib/fw/widget.hpp
 * @brief ウィジェットクラス定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <algorithm>
#include <cassert>
#include "core.hpp"
#include "../thunk_generator.hpp"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_A4DF6815_328F_40F7_86B5_6AF3492147EE_INCLUDE_GUARD
#define SPPLIB_A4DF6815_328F_40F7_86B5_6AF3492147EE_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

//リフレクション対象/REFLECTION
//#define WM_COMMAND_REFLECTION			
//#define WM_COMPAREITEM_REFLECTION		
//#define WM_CTLCOLORBTN_REFLECTION		
//#define WM_CTLCOLORDLG_REFLECTION		
//#define WM_CTLCOLOREDIT_REFLECTION		
//#define WM_CTLCOLORLISTBOX_REFLECTION	
//#define WM_CTLCOLORMSGBOX_REFLECTION	
//#define WM_CTLCOLORSCROLLBAR_REFLECTION	
//#define WM_CTLCOLORSTATIC_REFLECTION	
//#define WM_CTLCOLOR_REFLECTION			
//#define WM_DELETEITEM_REFLECTION		
//#define WM_DRAWITEM_REFLECTION			
//#define WM_HSCROLL_REFLECTION			
//#define WM_MEASUREITEM_REFLECTION		
#define WM_NOTIFY_REFLECTION			spplib::fw::widget::wm_notify_reflection()
//#define WM_VSCRLLL_REFLECTION			

/// sharkpp class library namespace
namespace spplib {
 /// framework namespace
 namespace fw {

#pragma warning(push)
#pragma warning(disable: 4355)

class widget
{
	thunk_generator<widget>	m_wnd_proc_thunk;

	typedef LRESULT (widget::*MESSAGE_THUNK_TYPE)(WPARAM wParam, LPARAM lParam);
	typedef std::map<UINT, MESSAGE_THUNK_TYPE> MESSAGE_MAP;

	typedef LRESULT (widget::*NOTIFY_MESSAGE_THUNK_TYPE)(UINT idControl, LPNMHDR pnmh);
	typedef std::map<std::pair<UINT,UINT>, NOTIFY_MESSAGE_THUNK_TYPE> NOTIFY_MESSAGE_MAP;

	typedef LRESULT (widget::*NOTIFY_REFLECTION_MESSAGE_THUNK_TYPE)(LPNMHDR pnmh);
	typedef std::map<UINT, NOTIFY_REFLECTION_MESSAGE_THUNK_TYPE> NOTIFY_REFLECTION_MESSAGE_MAP;

protected:

	HWND					m_hwnd;		///< ウインドウハンドル
	bool					m_created;	///< ウインドウハンドルが生成されたか(破棄が必要か)のフラグ
	bool					m_binding;	///< ウインドウハンドルがバインド(管理対象と)されているかのフラグ

	WNDPROC					m_old_wnd_proc;

	MESSAGE_MAP				m_message_map;
	NOTIFY_MESSAGE_MAP		m_notify_message_map;
	NOTIFY_REFLECTION_MESSAGE_MAP
							m_notify_reflection_message_map;

private:

	static void register_dialog(HWND hwnd)   { IsDialogMessage(NULL, hwnd, true); }
	static void unregister_dialog(HWND hwnd) { IsDialogMessage(NULL, hwnd, false); }
	static bool IsDialogMessage(MSG* lpmsg, HWND hwnd = NULL, bool add = false)
	{
		static std::vector<HWND> list;
		if( hwnd ) {
			if( add ) {
				list.push_back(hwnd);
			} else {
				std::vector<HWND>::iterator
					ite = std::find(list.begin(), list.end(), hwnd);
				if( ite != list.end() ) {
					list.erase(ite);
				}
			}
		} else if( lpmsg ) {
			for(std::vector<HWND>::iterator
					ite = list.begin(),
					last= list.end();
				ite != last; ++ite)
			{
				if( ::IsDialogMessage(*ite, lpmsg) ) {
					return true;
				}
			}
			return false;
		}
		return true;
	}

	LRESULT on_wm_notify(WPARAM wParam, LPARAM lParam)
	{
//		TRACE(_T("%08p %s(): %08X, %08X\n"), this, _T(__FUNCTION__), wParam, lParam);
		//
		LPNMHDR pnmh = reinterpret_cast<LPNMHDR>(lParam);
		//
		LRESULT lr = (LRESULT)::SendMessage(pnmh->hwndFrom, WM_NOTIFY_REFLECTION, wParam, lParam);
		if( lr ) {
			return lr;
		}
		// メッセージサンク
		NOTIFY_MESSAGE_MAP::iterator ite = m_notify_message_map.find(std::make_pair((UINT)wParam, pnmh->code));
		if( ite == m_notify_message_map.end() ) {
			ite =  m_notify_message_map.find(std::make_pair(0, pnmh->code));
			if( ite == m_notify_message_map.end() ) {
				return 0;
			}
		}
		return (this->*ite->second)((UINT)wParam, pnmh);
	}

	LRESULT on_wm_notify_reflection(WPARAM wParam, LPARAM lParam)
	{
//		TRACE(_T("%08p %s(): %08X, %08X\n"), this, _T(__FUNCTION__), wParam, lParam);
		//
		LPNMHDR pnmh = reinterpret_cast<LPNMHDR>(lParam);
		// メッセージサンク
		NOTIFY_REFLECTION_MESSAGE_MAP::iterator ite = m_notify_reflection_message_map.find(pnmh->code);
		if( ite == m_notify_reflection_message_map.end() ) {
			return 0;
		}
		return (this->*ite->second)(pnmh);
	}

	static UINT wm_notify_reflection() {
		static UINT message = ::RegisterWindowMessage(_T("WM_NOTIFY_REFLECTION"));
		return message;
	}

protected:

	void * wnd_proc_ptr()
	{
//TRACE(_T("%s %08p %08p\n"), _T(__FUNCTION__), this, (void*)m_wnd_proc_thunk.getCode());
		return (void*)m_wnd_proc_thunk.getCode();
	}

	static LRESULT CALLBACK wnd_proc_thunk(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return thunk_generator<widget>::thunk->wnd_proc(hwnd, uMsg, wParam, lParam);
	}

	virtual LRESULT CALLBACK wnd_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
//TRACE(_T("%s() %p %08X %08X %08X\n"), _T(__FUNCTION__), hwnd, uMsg, wParam, lParam);
		// メッセージサンク
		MESSAGE_MAP::iterator ite = m_message_map.find(uMsg);
		if( ite != m_message_map.end() ) {
			return (this->*ite->second)(wParam, lParam);
		}
		// サブクラス解除
		if( WM_DESTROY == uMsg ) {
			dettach();
		}
		return
			m_old_wnd_proc
				? ::CallWindowProc(m_old_wnd_proc, hwnd, uMsg, wParam, lParam)
				: ::DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	template<typename T>
	void append_event(UINT uMsg, LRESULT (T::*fn)(WPARAM,LPARAM))
		{ m_message_map[uMsg] = static_cast<MESSAGE_THUNK_TYPE>(fn); }

	template<typename T>
	void append_event(UINT uCode, LRESULT (T::*fn)(UINT,LPNMHDR))
		{ m_notify_message_map[std::make_pair((UINT)0, uCode)] = static_cast<NOTIFY_MESSAGE_THUNK_TYPE>(fn); }
	template<typename T>
	void append_event(UINT uCode, UINT idControl, LRESULT (T::*fn)(UINT,LPNMHDR))
		{ m_notify_message_map[std::make_pair(idControl, uCode)] = static_cast<NOTIFY_MESSAGE_THUNK_TYPE>(fn); }

	template<typename T>
	void append_reflection_event(UINT uCode, LRESULT (T::*fn)(LPNMHDR))
		{ m_notify_reflection_message_map[uCode] = static_cast<NOTIFY_REFLECTION_MESSAGE_THUNK_TYPE>(fn); }

	bool bind_hwnd(HWND hwnd, bool create = false, bool dialog_ = false)
	{
		if( hwnd ) {
			m_created = create;
			m_binding = true;
			m_hwnd = hwnd;
			if( dialog_ ) {
				register_dialog(hwnd);
			}
		}
		return NULL != hwnd;
	}

public:

	widget()
		: m_wnd_proc_thunk(this, wnd_proc_thunk) // C4355
		, m_hwnd(NULL)
		, m_created(false)
		, m_binding(false)
		, m_old_wnd_proc(NULL)
	{
		append_event(WM_NOTIFY,	&widget::on_wm_notify);
		append_event(WM_NOTIFY_REFLECTION,	&widget::on_wm_notify_reflection);
	}

//	widget(const widget & rhs)
//		: m_wnd_proc_thunk(this, wnd_proc_thunk) // C4355
//		, m_hwnd(rhs.m_hwnd)
//		, m_created(rhs.m_created)
//		, m_binding(rhs.m_binding)
//		, m_old_wnd_proc(rhs.m_old_wnd_proc)
//	{
//		const_cast<widget&>(rhs).m_created = false;
//	}

	explicit widget(UINT ctrl_id, const widget & parent)
		: m_wnd_proc_thunk(this, wnd_proc_thunk) // C4355
	{
		attach(ctrl_id, parent);
	}

	explicit widget(UINT ctrl_id, widget * parent = NULL)
		: m_wnd_proc_thunk(this, wnd_proc_thunk) // C4355
	{
		attach(ctrl_id, parent);
	}

	explicit widget(HWND hwnd, bool create = false)
		: m_wnd_proc_thunk(this, wnd_proc_thunk) // C4355
		, m_hwnd(hwnd)
		, m_created(create)
		, m_binding(true)
		, m_old_wnd_proc(NULL)
	{
	}

	virtual ~widget()
	{
		dettach();
		// ウインドウ破棄
		destroy();
	}

	void show(bool visible, bool none_activate = false)
	{
		::ShowWindow(m_hwnd, visible ? (none_activate ? SW_SHOWNA : SW_SHOW) : SW_HIDE);
	}

	bool destroy()
	{
		HWND hwnd = m_hwnd;
		            m_hwnd = NULL;
		unregister_dialog(hwnd);
		return !m_created || !hwnd
					? true
					: FALSE != ::DestroyWindow(hwnd);
	}

	void center(widget * target = NULL)
	{
		RECT rc;
		RECT rcOwner;

		// ダイアログのウィンドウサイズを取得
		::GetWindowRect(m_hwnd, &rc);

		// ディスクトップのウィンドウハンドルを取得
		HWND hwnd = target ? target->m_hwnd : ::GetDesktopWindow();
		::GetClientRect(hwnd, &rcOwner);

		// 足し引きする
		int x = (rcOwner.left + rcOwner.right)  / 2
			  - (rc.right     - rc.left)        / 2;
		int y = (rcOwner.top  + rcOwner.bottom) / 2
			  - (rc.bottom    - rc.top)         / 2;

		::SetWindowPos(m_hwnd, NULL, x, y, -1, -1,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
#ifdef _DEBUG
		::GetWindowRect(m_hwnd, &rc);
		assert( rc.left == x && rc.top == y );
#endif
	}

	HWND get_hwnd() const
	{
		return this ? m_hwnd : NULL;
	}

	bool valid() const
	{
		return this && NULL != m_hwnd &&
		       FALSE != ::IsWindow(m_hwnd);
	}

	widget get_dlg_item(UINT ctrl_id) const
	{
		assert(valid());
		widget w;
		w.m_hwnd = ::GetDlgItem(m_hwnd, ctrl_id);
		return w;
	}

	LRESULT sendmsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		assert(valid());
		return ::SendMessage(m_hwnd, uMsg, wParam, lParam);
	}

	void attach(UINT ctrl_id, const widget & parent)
	{
		assert(!valid());
		bool result = bind_hwnd( ::GetDlgItem(parent.m_hwnd, ctrl_id) );
		m_old_wnd_proc = (WNDPROC)::SetWindowLongPtr(m_hwnd, GWLP_WNDPROC,
		                                             (LONG_PTR)wnd_proc_ptr() );
		return;
	}

	void attach(UINT ctrl_id, widget * parent = NULL)
	{
		if( !parent ) {
			parent = thunk_generator<widget>::thunk;
			assert(parent);
		}
		attach(ctrl_id, *parent);
	}

	void dettach()
	{
		// プロシージャサンクの解除
		if( m_binding ) {
			::SetWindowLongPtr(m_hwnd, GWLP_WNDPROC,
			                   (LONG_PTR)(m_old_wnd_proc ? m_old_wnd_proc : DefWindowProc));
			m_old_wnd_proc = NULL;
		}
	}


	static int run()
	{
		MSG msg;
		while( ::GetMessage(&msg, NULL, 0, 0) )
		{
			if( IsDialogMessage(&msg) ) {
				continue;
			}
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		return 0;
	}
};

#pragma warning(pop)

 } // namespace fw
} // namespace spplib

#endif // !defined(SPPLIB_A4DF6815_328F_40F7_86B5_6AF3492147EE_INCLUDE_GUARD)
