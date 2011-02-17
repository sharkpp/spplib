// widget.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <stdio.h>
#include <windowsx.h>
#include "main.h"


#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>

#include <spplib/fw/dialog.hpp>
#include <spplib/fw/property_sheet.hpp>
#include <spplib/fw/controls.hpp>

#include <spplib/string.hpp>

#include <set>

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

class edit_cls : public spplib::fw::widget
{
private:
	LRESULT on_wm_rbutton_down(WPARAM wParam, LPARAM lParam)
	{
		TRACE(_T("WM_LBUTTON_DOWN\n"));
		return TRUE;
	}
public:
	edit_cls() : spplib::fw::widget()
	{
		append_event(WM_LBUTTONDOWN, &edit_cls::on_wm_rbutton_down);
	}
	virtual ~edit_cls() {}
};

////////////////////////////////////////////////////////////////////////////

class dialog1 : public spplib::fw::dialog
{
	edit_cls m_edit1;
	spplib::fw::native::spin_button_ctrl m_spin;
	spplib::fw::native::combobox m_cmb;
private:
	LRESULT on_wm_initdialog(WPARAM wParam, LPARAM lParam)
	{
TRACE(_T("%0p %08X\n"), m_hwnd, WM_INITDIALOG);
		center();
//		spplib::fw::widget edit1 = get_dlg_item(IDC_EDIT1);
		m_edit1.attach(IDC_EDIT1, *this);
	//	m_edit1.sendmsg(EM_SETLIMITTEXT, 128, 128);
		m_spin.attach(IDC_SPIN1, *this);
		m_spin.set_buddy(m_edit1);
	//	m_spin.set_base(10);
		m_spin.set_range(0, 100);
		m_spin.set_pos(0);
		m_cmb.attach(IDC_COMBO1, *this);
		m_cmb.add_string(_T("test1"));
		m_cmb.add_string(_T("test2"));
		m_cmb.add_string(_T("test3"));
		return TRUE;
	}
public:
	dialog1() : spplib::fw::dialog(IDD_PROP_PAGE1)
	{
		append_event(WM_INITDIALOG, &dialog1::on_wm_initdialog);
	}
	virtual ~dialog1() {}
};

class prop_page1: public spplib::fw::property_sheet_page
{
public:
	prop_page1() : spplib::fw::property_sheet_page(IDD_PROP_PAGE1) {}
};

class mylistview: public spplib::fw::native::listview
{
	spplib::tstring get_draw_stage_text(DWORD dwDrawStage) {
#define case_return(x)	\
	case x: return spplib::tstring(_T(#x))
		switch( dwDrawStage ) {
		case_return(CDDS_PREPAINT);
		case_return(CDDS_POSTPAINT);
		case_return(CDDS_PREERASE);
		case_return(CDDS_POSTERASE);
		case_return(CDDS_SUBITEM|CDDS_PREPAINT);
		case_return(CDDS_SUBITEM|CDDS_POSTPAINT);
		case_return(CDDS_SUBITEM|CDDS_PREERASE);
		case_return(CDDS_SUBITEM|CDDS_POSTERASE);
		case_return(CDDS_ITEMPREPAINT);
		case_return(CDDS_ITEMPOSTPAINT);
		case_return(CDDS_ITEMPREERASE);
		case_return(CDDS_ITEMPOSTERASE);
		case_return(CDDS_SUBITEM|CDDS_ITEMPREPAINT);
		case_return(CDDS_SUBITEM|CDDS_ITEMPOSTPAINT);
		case_return(CDDS_SUBITEM|CDDS_ITEMPREERASE);
		case_return(CDDS_SUBITEM|CDDS_ITEMPOSTERASE);
		case_return(CDDS_SUBITEM);
		}
		return spplib::tstring(_T("?"));
#undef case_return
	}

	spplib::tstring get_return_flags_text(DWORD dwReturnFlags) {
#define case_append(x)	\
	if(dwReturnFlags&x) r += (r.empty() ? _T("") : _T("|")) + spplib::tstring(_T(#x))
		spplib::tstring r;
		case_append(CDRF_DODEFAULT);
		case_append(CDRF_NEWFONT);
		case_append(CDRF_SKIPDEFAULT);
		case_append(CDRF_DOERASE);
		case_append(CDRF_SKIPPOSTPAINT);
		case_append(CDRF_NOTIFYPOSTPAINT);
		case_append(CDRF_NOTIFYITEMDRAW);
		case_append(CDRF_NOTIFYPOSTERASE);
		return r;
#undef case_return
	}

	DWORD m_dwLastDrawStage;
	DWORD m_dwLastReturnFlags;
	std::set<std::pair<DWORD,DWORD> > m_setN;
	std::map<DWORD,DWORD> m_mapN;
	DWORD m_dwN;

	LRESULT on_nm_customdraw(LPNMHDR pnmh)
	{
		const static DWORD return_flags[] = {
			CDRF_DODEFAULT,
			CDRF_NEWFONT,
			CDRF_SKIPDEFAULT,
			CDRF_DOERASE,
			CDRF_SKIPPOSTPAINT,
			CDRF_NOTIFYPOSTPAINT,
			CDRF_NOTIFYITEMDRAW,
			CDRF_NOTIFYPOSTERASE,
		};
		const static DWORD return_flags_prepaint[] = {
			CDRF_DODEFAULT,
			CDRF_NOTIFYITEMDRAW,
			CDRF_NOTIFYPOSTPAINT,
			CDRF_NOTIFYPOSTERASE,
	//	CDRF_DOERASE,
	//	CDRF_SKIPPOSTPAINT,
		};
		const static DWORD return_flags_itemprepaint[] = {
			CDRF_NEWFONT,
			CDRF_SKIPDEFAULT,
			CDRF_NOTIFYSUBITEMDRAW,
	//	CDRF_DOERASE,
	//	CDRF_SKIPPOSTPAINT,
		};

		LPNMCUSTOMDRAW lpnmcd = reinterpret_cast<LPNMCUSTOMDRAW>(pnmh);
	//	TRACE(_T("%5d %5d\n"), lpnmcd->dwDrawStage, lpnmcd->dwItemSpec);
		
	//	DWORD r = return_flags[m_dwN % _countof(return_flags)];
	//	DWORD r = 0;
	//	for(DWORD n = m_dwN; n ; n /= _countof(return_flags)) {
	//		r |= return_flags[n % _countof(return_flags)];
	//	}
	//	m_dwN++;

		//switch( lpnmcd->dwDrawStage )
		//{
		//case CDDS_PREPAINT:
		//	r &= CDRF_NOTIFYITEMDRAW|CDRF_NOTIFYPOSTERASE|CDRF_NOTIFYPOSTPAINT;
		//	break;
		//case CDDS_ITEMPREPAINT:
		//	r &= CDRF_NEWFONT|CDRF_SKIPDEFAULT;
		//	break;
		//}
		//if( lpnmcd->dwDrawStage != CDDS_PREPAINT						&&
		//	lpnmcd->dwDrawStage != CDDS_ITEMPREPAINT					&&
		//	lpnmcd->dwDrawStage != CDDS_ITEMPREPAINT|CDDS_PREPAINT		&&
		//	lpnmcd->dwDrawStage != CDDS_ITEMPREPAINT|CDDS_ITEMPREPAINT	)
		//{
		//	r &= ~(CDRF_NOTIFYITEMDRAW|CDRF_NOTIFYPOSTERASE|CDRF_NOTIFYPOSTPAINT);
		//}
		////if( CDDS_ITEMPREPAINT != lpnmcd->dwDrawStage ) {
		////	r &= ~(CDRF_NEWFONT|CDRF_SKIPDEFAULT);
		////}
		//if( CDRF_NOTIFYITEMDRAW|CDRF_NOTIFYPOSTERASE )

		if( m_mapN.end() == m_mapN.find(lpnmcd->dwDrawStage) ) {
			m_mapN[lpnmcd->dwDrawStage] = 0;
		}

		DWORD r = return_flags[m_mapN[lpnmcd->dwDrawStage] % _countof(return_flags)];
		m_mapN[lpnmcd->dwDrawStage]++;
		//switch(lpnmcd->dwDrawStage)
		//{
		//case CDDS_PREPAINT:
		//	r = return_flags_prepaint[m_dwN % _countof(return_flags_prepaint)];
		//	break;
		//case CDDS_ITEMPREPAINT:
		//case CDDS_ITEMPREPAINT|CDDS_SUBITEM:
		//	r = return_flags_itemprepaint[m_dwN % _countof(return_flags_itemprepaint)];
		//	break;			
		//}

		//DWORD r = 0;
		//int nLimit = _countof(return_flags) * 2;
		//do {
		//	if( nLimit < 0 ) {
		//		return CDRF_DODEFAULT;
		//	}
		//	r = return_flags[m_mapN[lpnmcd->dwDrawStage] % _countof(return_flags)];
		//	m_mapN[lpnmcd->dwDrawStage]++;
		//	nLimit--;
		//} while( m_setN.end() != m_setN.find(std::make_pair(lpnmcd->dwDrawStage,r)) );
		//m_setN.insert(std::make_pair(lpnmcd->dwDrawStage,r));

		if( !m_dwLastDrawStage /*||
			CDDS_POSTPAINT == lpnmcd->dwDrawStage*/ ) {
		//	m_dwLastReturnFlags = (DWORD)-1;
			//get_draw_stage_text(lpnmcd->dwDrawStage).c_str();
		//	TRACE(_T("--------------------------\n"));
		} else if( (DWORD)-1 != m_dwLastReturnFlags ) {
			TRACE(_T("  \"%s\" -> \"%s\" [label = \"%s\"];\n")
				, get_draw_stage_text(m_dwLastDrawStage).c_str()
				, get_draw_stage_text(lpnmcd->dwDrawStage).c_str()
				, get_return_flags_text(m_dwLastReturnFlags).c_str());
		}
		m_dwLastDrawStage = lpnmcd->dwDrawStage;
		m_dwLastReturnFlags = r;

		KillTimer(m_hwnd, 1);
		SetTimer(m_hwnd, 1, 100, NULL);

		return r;
	}

	LRESULT on_wm_timer(WPARAM,LPARAM)
	{
		if( m_dwLastDrawStage )
		{
			if( CDDS_PREPAINT != m_dwLastDrawStage ) {
				TRACE(_T("  \"%s\" -> \"\" [label = \"%s\"];\n")
					, get_draw_stage_text(m_dwLastDrawStage).c_str()
					, get_return_flags_text(m_dwLastReturnFlags).c_str());
			}
			m_dwLastDrawStage = 0;
		//	m_dwN = m_dwNstack[0];
		//	::memset(m_dwNstack, 0, sizeof(m_dwNstack));
		//	m_dwNstack[0] = m_dwN;
		//	m_dwN = 0;
		}
		KillTimer(m_hwnd, 1);
		InvalidateRect(m_hwnd, NULL, TRUE);
		return 0;
	}

public:
	mylistview()
		: m_dwLastDrawStage(0)
	{
	//	append_event(NM_CUSTOMDRAW, &mylistview::on_nm_customdraw);
		append_reflection_event(NM_CUSTOMDRAW, &mylistview::on_nm_customdraw);
		append_event(WM_TIMER, &mylistview::on_wm_timer);
	}
};

class prop_page2: public spplib::fw::property_sheet_page
{
	mylistview m_list;

	virtual void initialize()
	{
		m_list.attach(IDC_LIST1, *this);
		m_list.insert_column(0, _T("xxxx"));
		m_list.insert_column(0, _T("yyyy"));
		m_list.insert_column(0, _T("zzzz"));
		m_list.insert_item(0, _T("aaaa"));
		m_list.set_item(0, 1, _T("bbbb"));
		m_list.set_item(0, 2, _T("cccc"));
		m_list.set_column_width(0, LVSCW_AUTOSIZE_USEHEADER);
		m_list.set_column_width(1, LVSCW_AUTOSIZE_USEHEADER);
		m_list.set_column_width(2, LVSCW_AUTOSIZE_USEHEADER);
	}

public:
	prop_page2() : spplib::fw::property_sheet_page(IDD_PROP_PAGE2) {}
};

////////////////////////////////////////////////////////////////////////////

int APIENTRY _tWinMain(
		HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPTSTR    lpCmdLine,
		int       nCmdShow)
{
	InitCommonControls();

//	spplib::fw::dialog dlg(IDD_PROP_PAGE1);
//	dlg.create();
//	dlg.show(true);
//	dlg.modal();

	dialog1 dlg;
	dlg.create();
	dlg.show(true);
//	dlg.modal();

	spplib::fw::property_sheet ps;
	prop_page1 page1;
	prop_page2 page2;
	spplib::fw::property_sheet_page page3(IDD_PROP_PAGE3);

	ps.add_page(&page1);
	ps.add_page(&page2);
	ps.add_page(&page3);
	if( !ps.create() ) {
		TRACE(_T("property_sheet create faild!\n"));
	}

	spplib::fw::widget::run();

	return 0;
}

#if 0

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ここにコードを挿入してください。
	MSG msg;
	HACCEL hAccelTable;

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIDGET, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIDGET));

	// メイン メッセージ ループ:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIDGET));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIDGET);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 描画コードをここに追加してください...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// バージョン情報ボックスのメッセージ ハンドラです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

#endif
