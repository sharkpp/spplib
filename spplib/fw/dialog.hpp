/**
 * @file  spplib/fw/dialog.hpp
 * @brief ダイアログクラス定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include "widget.hpp"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_39B7D552_F5D7_4930_AD49_32EEE2123D0A_INCLUDE_GUARD
#define SPPLIB_39B7D552_F5D7_4930_AD49_32EEE2123D0A_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// framework namespace
 namespace fw {

class dialog
	: public widget
{
	thunk_generator<dialog>	m_dlg_proc_thunk;

	LPCTSTR					m_lpResName;

protected:

	void * dlg_proc_ptr()
	{
//TRACE(_T("%s %08p %08p\n"), _T(__FUNCTION__), this, (void*)m_dlg_proc_thunk.getCode());
		return (void*)m_dlg_proc_thunk.getCode();
	}

	static INT_PTR CALLBACK dlg_proc_thunk(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return thunk_generator<dialog>::thunk->dlg_proc(hwnd, uMsg, wParam, lParam);
	}

	virtual INT_PTR dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
//TRACE(_T("%s() %p %08X %08X %08X\n"), _T(__FUNCTION__), hwnd, uMsg, wParam, lParam);
		if( !m_hwnd ) {
			m_hwnd = hwnd;
		}
		MESSAGE_MAP::iterator ite = m_message_map.find(uMsg);
		if( ite != m_message_map.end() ) {
			LRESULT lr = (this->*ite->second)(wParam, lParam);
			// ダイアログはDWL_MSGRESULTで戻り値を返さないといけないので
			return SetDlgMsgResult(hwnd, DWLP_MSGRESULT, lr);
		}
		switch(uMsg) {
			case WM_COMMAND:
				switch( GET_WM_COMMAND_ID(wParam, lParam) ) {
					case IDOK:
					case IDCANCEL:
						::EndDialog(hwnd, GET_WM_COMMAND_ID(wParam, lParam));
						::PostQuitMessage(0);
						return TRUE;
				}
		}
		return FALSE;
	}

	LRESULT on_wm_initdialog(WPARAM wParam, LPARAM lParam)
	{
		UINT uFirstCtrlID = (UINT)wParam;
		TRACE(_T("%08p %s(): %08X, %08X\n"), this, _T(__FUNCTION__), wParam, lParam);
		if( !(WS_CHILD & ::GetWindowLongPtr(m_hwnd, GWL_STYLE)) ) {
			center();
		}
		initialize();
		return TRUE;
	}

	LRESULT on_wm_destroy(WPARAM wParam, LPARAM lParam)
	{
		TRACE(_T("%08p %s(): %08X, %08X\n"), this, _T(__FUNCTION__), wParam, lParam);
		finalize();
		return 0;
	}

	virtual void initialize() { }
	virtual void finalize()   { }

public:

	dialog(UINT uResID = 0)
		: m_dlg_proc_thunk(this, dlg_proc_thunk) // C4355
		, m_lpResName(MAKEINTRESOURCE(uResID))
	{
		append_event(WM_INITDIALOG,	&dialog::on_wm_initdialog);
		append_event(WM_DESTROY,	&dialog::on_wm_destroy);
	}

	dialog(LPCTSTR lpszResName)
		: m_dlg_proc_thunk(this, dlg_proc_thunk) // C4355
		, m_lpResName(lpszResName)
	{
		append_event(WM_INITDIALOG,	&dialog::on_wm_initdialog);
	}

	virtual ~dialog()
	{
		// プロシージャサンクの解除
		if( m_binding ) {
			::SetWindowLongPtr(m_hwnd, DWLP_DLGPROC, (LONG_PTR)NULL);
		}
	}

	bool create(UINT uResID = 0)
	{
		return create(uResID ? MAKEINTRESOURCE(uResID) : m_lpResName);
	}

	bool create(LPCTSTR lpszResName)
	{
		return bind_hwnd(
						::CreateDialog(get_instance_handle(),
						               lpszResName ? lpszResName : m_lpResName,
						               NULL, reinterpret_cast<DLGPROC>( dlg_proc_ptr() )),
						true, true
					);
	}

	INT_PTR modal(UINT uResID = 0)
	{
		return modal(uResID ? MAKEINTRESOURCE(uResID) : m_lpResName);
	}

	INT_PTR modal(LPCTSTR lpszResName)
	{
		return ::DialogBox(get_instance_handle(), lpszResName ? lpszResName : m_lpResName,
		                   NULL, reinterpret_cast<DLGPROC>( dlg_proc_ptr() ));
	}
};

 } // namespace fw
} // namespace spplib

#endif // !defined(SPPLIB_39B7D552_F5D7_4930_AD49_32EEE2123D0A_INCLUDE_GUARD)
