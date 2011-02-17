/**
 * @file  spplib/fw/property_sheet.hpp
 * @brief プロパティーシートクラス定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include "dialog.hpp"
#include <commctrl.h>
#include <vector>

#pragma comment(lib, "Comctl32.lib")

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_9763EFE6_5FD0_4F1C_B3AF_0E4AC988A1C5_INCLUDE_GUARD
#define SPPLIB_9763EFE6_5FD0_4F1C_B3AF_0E4AC988A1C5_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// framework namespace
 namespace fw {

#pragma warning(push)
#pragma warning(disable: 4355)

//////////////////////////////////////////////////////////////////////

class property_sheet_page
	: public spplib::fw::dialog
{
	friend class property_sheet;

	spplib::thunk_generator<property_sheet_page>
					m_prop_sheet_page_proc_thunk;

	PROPSHEETPAGE	m_psp;

private:

	void initialize_property_sheet_page()
	{
		::memset(&m_psp, 0, sizeof(m_psp));
		m_psp.dwSize		= sizeof(m_psp);
		m_psp.dwFlags		= PSP_DEFAULT | PSP_USECALLBACK /*| PSP_USEHICON */;
		m_psp.hInstance		= get_instance_handle();
		m_psp.pszTitle		= 0;
		m_psp.pfnDlgProc	= reinterpret_cast<DLGPROC>( dlg_proc_ptr() );
		m_psp.lParam		= 0;
		m_psp.pfnCallback	= reinterpret_cast<LPFNPSPCALLBACK>( m_prop_sheet_page_proc_thunk.getCode() );
		m_psp.pcRefParent	= 0;

		// メッセージフックを追加
		append_event(PSN_SETACTIVE,				&property_sheet_page::on_set_active);
		append_event(PSN_KILLACTIVE,			&property_sheet_page::on_kill_active);
		append_event(PSN_APPLY,					&property_sheet_page::on_apply);
		append_event(PSN_RESET,					&property_sheet_page::on_reset);
		append_event(PSN_HELP,					&property_sheet_page::on_help);
		append_event(PSN_WIZBACK,				&property_sheet_page::on_wizard_back);
		append_event(PSN_WIZNEXT,				&property_sheet_page::on_wizard_next);
		append_event(PSN_WIZFINISH,				&property_sheet_page::on_wizard_finish);
		append_event(PSN_QUERYCANCEL,			&property_sheet_page::on_query_cancel);
#if (_WIN32_IE >= 0x0400)
		append_event(PSN_GETOBJECT,				&property_sheet_page::on_get_object);
#endif // 0x0400
#if (_WIN32_IE >= 0x0500)
		append_event(PSN_TRANSLATEACCELERATOR,	&property_sheet_page::on_translate_accelerator);
		append_event(PSN_QUERYINITIALFOCUS,		&property_sheet_page::on_query_initial_focus);
#endif // 0x0500
	}

protected:

	static UINT CALLBACK prop_sheet_page_proc_thunk(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
	{
		return spplib::thunk_generator<property_sheet_page>::thunk->prop_sheet_page_proc(hwnd, uMsg, ppsp);
	}

	virtual UINT prop_sheet_page_proc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
	{
		TRACE(_T("%08p %s(): %08X, %08X, %08p\n"), this, _T(__FUNCTION__), hwnd, uMsg, ppsp);
		return TRUE;
	}

	virtual void initialize() { }
	virtual void finalize()   { }

	virtual LRESULT on_set_active(UINT idControl, LPNMHDR pnmh)            { TRACE(_T("%08p %s(): %08p\n"), this, _T(__FUNCTION__), pnmh); return 0; }
	virtual LRESULT on_kill_active(UINT idControl, LPNMHDR pnmh)           { TRACE(_T("%08p %s(): %08p\n"), this, _T(__FUNCTION__), pnmh); return 0; }
	virtual LRESULT on_apply(UINT idControl, LPNMHDR pnmh)                 { TRACE(_T("%08p %s(): %08p\n"), this, _T(__FUNCTION__), pnmh); return 0; }
	virtual LRESULT on_reset(UINT idControl, LPNMHDR pnmh)                 { TRACE(_T("%08p %s(): %08p\n"), this, _T(__FUNCTION__), pnmh); return 0; }
	virtual LRESULT on_help(UINT idControl, LPNMHDR pnmh)                  { TRACE(_T("%08p %s(): %08p\n"), this, _T(__FUNCTION__), pnmh); return 0; }
	virtual LRESULT on_wizard_back(UINT idControl, LPNMHDR pnmh)           { TRACE(_T("%08p %s(): %08p\n"), this, _T(__FUNCTION__), pnmh); return 0; }
	virtual LRESULT on_wizard_next(UINT idControl, LPNMHDR pnmh)           { TRACE(_T("%08p %s(): %08p\n"), this, _T(__FUNCTION__), pnmh); return 0; }
	virtual LRESULT on_wizard_finish(UINT idControl, LPNMHDR pnmh)         { TRACE(_T("%08p %s(): %08p\n"), this, _T(__FUNCTION__), pnmh); return 0; }
	virtual LRESULT on_query_cancel(UINT idControl, LPNMHDR pnmh)          { TRACE(_T("%08p %s(): %08p\n"), this, _T(__FUNCTION__), pnmh); return 0; }
	virtual LRESULT on_get_object(UINT idControl, LPNMHDR pnmh)            { TRACE(_T("%08p %s(): %08p\n"), this, _T(__FUNCTION__), pnmh); return 0; }
	virtual LRESULT on_translate_accelerator(UINT idControl, LPNMHDR pnmh) { TRACE(_T("%08p %s(): %08p\n"), this, _T(__FUNCTION__), pnmh); return 0; }
	virtual LRESULT on_query_initial_focus(UINT idControl, LPNMHDR pnmh)   { TRACE(_T("%08p %s(): %08p\n"), this, _T(__FUNCTION__), pnmh); return 0; }

public:

	property_sheet_page(UINT uResID = 0)
		: spplib::fw::dialog(uResID)
		, m_prop_sheet_page_proc_thunk(this, prop_sheet_page_proc_thunk)
	{
		initialize_property_sheet_page();
		m_psp.pszTemplate	= MAKEINTRESOURCE(uResID);
	}

	property_sheet_page(LPCTSTR lpszResName)
		: dialog(lpszResName)
		, m_prop_sheet_page_proc_thunk(this, prop_sheet_page_proc_thunk)
	{
		initialize_property_sheet_page();
		m_psp.pszTemplate	= lpszResName;
	}

	~property_sheet_page()
	{
	}

};

////////////////////////////////////////////////////////////////////////////

class property_sheet
	: public widget
{
	// http://msdn.microsoft.com/en-us/library/aa930671.aspx
	typedef struct { 
		WORD dlgVer;
		WORD signature;
		DWORD helpID;
		DWORD exStyle;
		DWORD style;
		WORD cDlgItems;
		short x;
		short y;
		short cx;
		short cy;
	//	sz_Or_Ord menu;
	//	sz_Or_Ord windowClass;
	//	WCHAR title[titleLen];
	//	short pointsize;
	//	short weight;
	//	short bItalic;
	//	WCHAR font[fontLen];
	} DLGTEMPLATEEX_;

private:

	void initialize_property_sheet()
	{
		::memset(&m_psh, 0, sizeof(m_psh));
		m_psh.hInstance		= get_instance_handle();
		m_psh.dwSize		= sizeof(PROPSHEETHEADER);
		m_psh.dwFlags		= PSH_DEFAULT | PSH_USECALLBACK;
		m_psh.hwndParent	= NULL;
		m_psh.pfnCallback	= reinterpret_cast<PFNPROPSHEETCALLBACK>( m_prop_sheet_proc_thunk.getCode() );
	}

protected:

	spplib::thunk_generator<property_sheet>
					m_prop_sheet_proc_thunk;

	PROPSHEETHEADER	m_psh;

	std::vector<property_sheet_page*>
					m_prop_pages;

protected:

	static int CALLBACK prop_sheet_proc_thunk(HWND hwnd, UINT uMsg, LPARAM lParam)
	{
		spplib::thunk_generator<property_sheet>::thunk->prop_sheet_proc(hwnd, uMsg, lParam);
		return 0;
	}

	virtual void prop_sheet_proc(HWND hwnd, UINT uMsg, LPARAM lParam)
	{
		TRACE(_T("%08p %s(): %08X, %08X, %08X\n"), this, _T(__FUNCTION__), hwnd, uMsg, lParam);

		switch(uMsg)
		{
		case PSCB_PRECREATE:
			if( lParam ) {
				DLGTEMPLATEEX_*	pDlgTemplateEx = reinterpret_cast<DLGTEMPLATEEX_*>(lParam);
				DLGTEMPLATE *	pDlgTemplate   = reinterpret_cast<DLGTEMPLATE *>  (lParam);
				if( 0xFFFF == pDlgTemplateEx->signature ) {
					// pDlgTemplateEx points to an extended dialog template structure.
					pDlgTemplateEx->style |= DS_CENTER;
				} else {
					// This is a standard dialog template structure.
					pDlgTemplate->style |= DS_CENTER;
				}
			}
		//	on_precreate(reinterpret_cast<DLGTEMPLATEEX*>(lparam));
			break;
		case PSCB_INITIALIZED:
			bind_hwnd(hwnd);
			on_initialized();
			break;
#if (_WIN32_IE >= 0x560)
		case PSCB_BUTTONPRESSED:
			on_button_pressed(lParam);
			break;
#endif  // _WIN32_IE >= 0x560
		}
	}

	void setup(std::vector<PROPSHEETPAGE> & pages)
	{
		for(std::vector<property_sheet_page*>::iterator
				ite = m_prop_pages.begin(),
				last= m_prop_pages.end();
			ite != last; ++ite)
		{
			pages.push_back((*ite)->m_psp);
		}
		m_psh.dwFlags		|= PSH_PROPSHEETPAGE;
		m_psh.nPages		 = (UINT)pages.size();
		m_psh.ppsp 			 = &pages[0];
	}

	virtual void on_initialized()
	{
		center();
	}

	virtual void on_button_pressed(LPARAM lParam) {}

public:

	property_sheet()
		: m_prop_sheet_proc_thunk(this, prop_sheet_proc_thunk)
	{
		initialize_property_sheet();
	}

	property_sheet(widget * parent, UINT caption_id = 0, UINT icon_id = 0)
		: m_prop_sheet_proc_thunk(this, prop_sheet_proc_thunk)
	{
		initialize_property_sheet();
		m_psh.dwFlags		|= PSH_USEICONID | PSH_PROPTITLE;
		m_psh.hwndParent	= parent->get_hwnd();
		m_psh.pszIcon		= MAKEINTRESOURCE(icon_id);
		m_psh.pszCaption	= MAKEINTRESOURCE(caption_id);
	}

	property_sheet(widget * parent, LPCTSTR caption = 0, UINT icon_id = 0)
		: m_prop_sheet_proc_thunk(this, prop_sheet_proc_thunk)
	{
		initialize_property_sheet();
		m_psh.dwFlags		|= PSH_USEICONID | PSH_PROPTITLE;
		m_psh.hwndParent	= parent->get_hwnd();
		m_psh.pszIcon		= MAKEINTRESOURCE(icon_id);
		m_psh.pszCaption	= caption;
	}

	virtual ~property_sheet()
	{
	}


	bool set_icon(HICON icon_handle)
	{
		m_psh.dwFlags &= ~PSH_USEICONID;
		m_psh.hIcon    = icon_handle;
		return true;
	}

	bool set_icon(UINT icon_id)
	{
		return set_icon(MAKEINTRESOURCE(icon_id));
	}

	bool set_icon(LPCTSTR icon_id)
	{
		m_psh.dwFlags |= PSH_USEICONID;
		m_psh.pszIcon  = icon_id;
		return true;
	}


	bool set_caption(UINT caption_id)
	{
		return set_caption(MAKEINTRESOURCE(caption_id));
	}

	bool set_caption(LPCTSTR caption_id)
	{
		m_psh.dwFlags   |= PSH_PROPTITLE;
		m_psh.pszCaption = caption_id;
		return true;
	}


	bool clear_page()
	{
		m_prop_pages.clear();
		return true;
	}

	bool add_page(property_sheet_page * page)
	{
		m_prop_pages.push_back(page);
		return true;
	}

	bool create()
	{
		std::vector<PROPSHEETPAGE> tmp;
		setup(tmp);
		m_psh.dwFlags |= PSH_MODELESS;
		return NULL != (HWND)::PropertySheet(&m_psh);
	}

	bool modal()
	{
		std::vector<PROPSHEETPAGE> tmp;
		setup(tmp);
		m_psh.dwFlags &=~PSH_MODELESS;
		return -1 != ::PropertySheet(&m_psh);
	}
};

#pragma warning(pop)

 } // namespace fw
} // namespace spplib

#endif // !defined(SPPLIB_9763EFE6_5FD0_4F1C_B3AF_0E4AC988A1C5_INCLUDE_GUARD)
