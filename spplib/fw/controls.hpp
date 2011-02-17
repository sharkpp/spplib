/**
 * @file  spplib/fw/controls.hpp
 * @brief ウィジェットクラス定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include "widget.hpp"
#include "../tribool.hpp"
#include <commctrl.h>
#include <vector>
#include <string>

#pragma comment(lib, "Comctl32.lib")

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_465F9279_68C0_44CA_A8F0_9F8FCC04C7BC_INCLUDE_GUARD
#define SPPLIB_465F9279_68C0_44CA_A8F0_9F8FCC04C7BC_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// framework namespace
 namespace fw {

  namespace native {

//////////////////////////////////////////////////////////////////////
/// ボタン
class button : public widget
{
public:
	button() : widget() {}
	virtual ~button() {}

	void    click()                                  { sendmsg(BM_CLICK, 0, 0); }
	int     get_check()                              { return (int)sendmsg(BM_GETCHECK, 0, 0); }
	HGDIOBJ get_image(int type = IMAGE_BITMAP)       { return (HGDIOBJ)sendmsg(BM_GETIMAGE, 0, 0); }
	int     get_state()                              { return (int)sendmsg(BM_GETSTATE, 0, 0); }
	void    set_check(int check)                     { sendmsg(BM_SETCHECK, (LPARAM)check, 0); }
	void    set_dont_click(bool enable)              { sendmsg(BM_SETDONTCLICK, (WPARAM)(enable ? TRUE : FALSE), 0); }
	HGDIOBJ set_image(int type, HGDIOBJ obj)         { return (HGDIOBJ)sendmsg(BM_SETIMAGE, (LPARAM)type, (LPARAM)obj); }
	void    set_state(bool state)                    { sendmsg(BM_SETSTATE, (WPARAM)(state ? TRUE : FALSE), 0); }
	void    set_style(int style, bool redraw = true) { sendmsg(BM_SETSTYLE, (WPARAM)style, (LPARAM)(redraw ? TRUE : FALSE)); }
//	void    get_ideal_size()                         { sendmsg(BCM_GETIDEALSIZE,        0, 0); }
//	void    get_image_list()                         { sendmsg(BCM_GETIMAGELIST,        0, 0); }
//	void    get_note()                               { sendmsg(BCM_GETNOTE,             0, 0); }
//	void    get_note_length()                        { sendmsg(BCM_GETNOTELENGTH,       0, 0); }
//	void    get_split_info()                         { sendmsg(BCM_GETSPLITINFO,        0, 0); }
//	void    get_text_margin()                        { sendmsg(BCM_GETTEXTMARGIN,       0, 0); }
//	void    set_drop_down_state()                    { sendmsg(BCM_SETDROPDOWNSTATE,    0, 0); }
//	void    set_image_list()                         { sendmsg(BCM_SETIMAGELIST,        0, 0); }
//	void    set_note()                               { sendmsg(BCM_SETNOTE,             0, 0); }
//	void    set_shield()                             { sendmsg(BCM_SETSHIELD,           0, 0); }
//	void    set_split_info()                         { sendmsg(BCM_SETSPLITINFO,        0, 0); }
//	void    set_text_margin()                        { sendmsg(BCM_SETTEXTMARGIN,       0, 0); }
};

//////////////////////////////////////////////////////////////////////
/// コンボボックス
class combobox : public widget
{
public:
	combobox() : widget() {}
	virtual ~combobox() {}

	int   add_string(const TCHAR * string)                          { return (int)sendmsg(CB_ADDSTRING, 0, (LPARAM)string); }
	int   add_string(const std::string & string)                    { return add_string((const TCHAR *)string.c_str()); }
	int   add_data(DWORD_PTR data)                                  { return (int)sendmsg(CB_ADDSTRING, 0, (LPARAM)data); }
	int   delete_string(int index)                                  { return (int)sendmsg(CB_DELETESTRING, (WPARAM)index, 0); }
	int   dir(int attributes, const TCHAR * path)                   { return (int)sendmsg(CB_DIR, (WPARAM)attributes, (LPARAM)path); }
	int   dir(int attributes, const std::string & path)             { return dir(attributes, (const TCHAR *)path.c_str()); }
	int   find_string(int index, const TCHAR * search_string)       { return (int)sendmsg(CB_FINDSTRING, (WPARAM)index, (LPARAM)search_string); }
	int   find_string(const TCHAR * search_string)                  { return find_string(-1, search_string); }
	int   find_string_exact(int index, const TCHAR * search_string) { return (int)sendmsg(CB_FINDSTRINGEXACT, (WPARAM)index, (LPARAM)search_string); }
	int   find_string_exact(const TCHAR * search_string)            { return find_string_exact(-1, search_string); }
	bool  get_combobox_info(COMBOBOXINFO & cbi)                     { return 0 != sendmsg(CB_GETCOMBOBOXINFO, 0, (LPARAM)&cbi); }
	int   get_count()                                               { return (int)sendmsg(CB_GETCOUNT, 0, 0); }
	bool  get_cue_banner(WCHAR * text, size_t cc)                   { return 1 == sendmsg(CB_GETCUEBANNER, (WPARAM)text, (LPARAM)cc); }
	int   get_cur_sel()                                             { return (int)sendmsg(CB_GETCURSEL, 0, 0); }
	bool  get_dropped_control_rect(RECT & rc)                       { return 0 != sendmsg(CB_GETDROPPEDCONTROLRECT, 0, (LPARAM)&rc); }
	bool  get_dropped_state()                                       { return TRUE == sendmsg(CB_GETDROPPEDSTATE, 0, 0); }
	int   get_dropped_width()                                       { return (int)sendmsg(CB_GETDROPPEDWIDTH, 0, 0); }
	std::pair<DWORD, DWORD> get_edit_sel()                          { DWORD start, end;
	                                                                  sendmsg(CB_GETEDITSEL, (WPARAM)&start, (LPARAM)&end);
	                                                                  return std::make_pair(start, end); }
	bool  get_extended_ui()                                         { return TRUE == sendmsg(CB_GETEXTENDEDUI, 0, 0); }
	int   get_horizon_talextent()                                   { return (int)sendmsg(CB_GETHORIZONTALEXTENT, 0, 0); }
	DWORD_PTR get_item_data(int index)                              { return (DWORD_PTR)sendmsg(CB_GETITEMDATA, (WPARAM)index, 0); }
	void* get_item_data_ptr(int index)                              { return (void*)sendmsg(CB_GETITEMDATA, (WPARAM)index, 0); }
	int   get_item_height(int index)                                { return (int)sendmsg(CB_GETITEMHEIGHT, (WPARAM)index, 0); }
	int   get_selection_field_height()                              { return get_item_height(-1); }
	bool  get_item_text(int index, TCHAR * buff, size_t cc)         { int item_len = get_item_text_len(index);
	                                                                  if( CB_ERR != item_len && item_len < cc ) {
	                                                                      return item_len == (int)sendmsg(CB_GETLBTEXT, (WPARAM)index, (LPARAM)buff);
	                                                                  }
	                                                                  return false; }
	std::string get_item_text(int index)                            { int item_len = get_item_text_len(index);
	                                                                  if( 0 <= item_len ) {
	                                                                      std::vector<TCHAR> buff(item_len);
	                                                                      if( item_len == (int)sendmsg(CB_GETLBTEXT, (WPARAM)index, (LPARAM)&buff[0]) ) {
	                                                                          return std::string(buff.begin(), buff.end());
	                                                                      }
	                                                                  }
	                                                                  return std::string(); }
	int   get_item_text_len(int index)                              { return (int)sendmsg(CB_GETLBTEXTLEN, (WPARAM)index, 0); }
	DWORD get_locale()                                              { return (DWORD)sendmsg(CB_GETLOCALE, 0, 0); }
	int   get_min_visible()                                         { return (int)sendmsg(CB_GETMINVISIBLE, 0, 0); }
	int   get_top_index()                                           { return (int)sendmsg(CB_GETTOPINDEX, 0, 0); }
	int   init_storage(int num, size_t size)                        { return (int)sendmsg(CB_INITSTORAGE, (WPARAM)num, (LPARAM)size); }
	int   insert_string(int index, const TCHAR * string)            { return (int)sendmsg(CB_INSERTSTRING, (WPARAM)index, (LPARAM)string); }
	int   insert_string(int index, const std::string & string)      { return insert_string(index, (const TCHAR *)string.c_str()); }
	int   insert_stringdata(int index, DWORD_PTR data)              { return (int)sendmsg(CB_INSERTSTRING, (WPARAM)index, (LPARAM)data); }
	void  limit_text(size_t cc)                                     { sendmsg(CB_LIMITTEXT, (WPARAM)cc, 0); }
	void  reset_content()                                           { sendmsg(CB_RESETCONTENT, 0, 0); }
	int   select_string(int index, const TCHAR * search_string)     { return (int)sendmsg(CB_SELECTSTRING, (WPARAM)index, (LPARAM)search_string); }
	int   select_string(const TCHAR * search_string)                { return select_string(-1, search_string); }
	bool  set_cue_banner(const WCHAR * text)                        { return 1 == sendmsg(CB_SETCUEBANNER, 0, (LPARAM)text); }
	int   set_cur_sel(int index)                                    { return (int)sendmsg(CB_SETCURSEL, (WPARAM)index, 0); }
	int   set_dropped_width(int width)                              { return (int)sendmsg(CB_SETDROPPEDWIDTH, (WPARAM)width, 0); }
	bool  set_edit_sel(DWORD start, DWORD end)                      { return TRUE == sendmsg(CB_SETEDITSEL, 0, MAKELPARAM((WORD)start, (WORD)end)); }
	bool  set_extended_ui(bool enable_extend_ui)                    { return CB_OKAY == sendmsg(CB_SETEXTENDEDUI, (WPARAM)(enable_extend_ui ? TRUE : FALSE), 0); }
	void  set_horizontalextent(int scrollable_width)                { sendmsg(CB_SETHORIZONTALEXTENT, (WPARAM)scrollable_width, 0); }
	bool  set_item_data(int index, DWORD_PTR data)                  { return CB_ERR != sendmsg(CB_SETITEMDATA, (WPARAM)index, (LPARAM)data); }
	bool  set_item_height(int index, int height)                    { return CB_ERR != sendmsg(CB_SETITEMHEIGHT, (WPARAM)index, (LPARAM)height); }
	bool  set_item_height(int height)                               { return set_item_height(-1, height); }
	DWORD set_locale(DWORD locale_value)                            { return (DWORD)sendmsg(CB_SETLOCALE, (WPARAM)locale_value, 0); }
	bool  set_min_visible(int min_visible_item_num)                 { return TRUE == sendmsg(CB_SETMINVISIBLE, (WPARAM)min_visible_item_num, 0); }
	bool  set_top_index(int index)                                  { return 0 == sendmsg(CB_SETTOPINDEX, (WPARAM)index, 0); }
	void  show_dropdown(bool shown)                                 { sendmsg(CB_SHOWDROPDOWN, (WPARAM)(shown ? TRUE : FALSE), 0); }
};

//////////////////////////////////////////////////////////////////////
/// スピンボタンコントロール
class spin_button_ctrl : public widget
{
public:
	spin_button_ctrl() : widget() {}
	virtual ~spin_button_ctrl() {}

	size_t get_accel(UDACCEL * puda, size_t cc)       { return (size_t)sendmsg(UDM_GETACCEL, (WPARAM)cc, (LPARAM)puda); }
	bool   set_accel(const UDACCEL * puda, size_t cc) { return sendmsg(UDM_SETACCEL, (WPARAM)cc, (LPARAM)puda) ? true : false; }
	int    get_base()                                 { return (int)sendmsg(UDM_GETBASE, 0, 0); }
	int    set_base(int base_value)                   { return (int)sendmsg(UDM_SETBASE, (WPARAM)base_value, 0); }
	widget get_buddy()                                { return widget((HWND)sendmsg(UDM_GETBUDDY, 0, 0)); }
	widget set_buddy(const widget & w)                { return widget((HWND)sendmsg(UDM_SETBUDDY, (WPARAM)w.get_hwnd(), 0)); }
	int    get_pos()                                  { BOOL success = FALSE;
	                                                    return (int)sendmsg(UDM_GETPOS32, 0, (LPARAM)&success); }
	int    set_pos(int pos)                           { return (int)sendmsg(UDM_SETPOS32, 0, (LPARAM)pos); }
	std::pair<int,int> get_range()                    { std::pair<int,int> result;
	                                                    sendmsg(UDM_GETRANGE32, (WPARAM)&result.first, (LPARAM)&result.second);
	                                                    return result; }
	void   set_range(int lower, int upper)            { sendmsg(UDM_SETRANGE32, (WPARAM)lower, (LPARAM)upper); }
	bool   get_unicode_format()                       { return sendmsg(UDM_GETUNICODEFORMAT, 0, 0) ? true : false; }
	bool   set_unicode_format(bool use_unicode)       { return sendmsg(UDM_SETUNICODEFORMAT, (WPARAM)(use_unicode ? TRUE : FALSE), 0) ? true : false; }
};

//////////////////////////////////////////////////////////////////////
/// リストビュー
class listview : public widget
{
public:
	listview() : widget() {}
	virtual ~listview() {}

	int approximate_view_rect()                                          { return (int)sendmsg(LVM_APPROXIMATEVIEWRECT, 0, 0); }
	int arrange()                                                        { return (int)sendmsg(LVM_ARRANGE, 0, 0); }
	int cancel_edit_label()                                              { return (int)sendmsg(LVM_CANCELEDITLABEL, 0, 0); }
	int create_drag_image()                                              { return (int)sendmsg(LVM_CREATEDRAGIMAGE, 0, 0); }
	int delete_all_items()                                               { return (int)sendmsg(LVM_DELETEALLITEMS, 0, 0); }
	int delete_column()                                                  { return (int)sendmsg(LVM_DELETECOLUMN, 0, 0); }
	int delete_item()                                                    { return (int)sendmsg(LVM_DELETEITEM, 0, 0); }
	int edit_label()                                                     { return (int)sendmsg(LVM_EDITLABEL, 0, 0); }
	int enable_group_view()                                              { return (int)sendmsg(LVM_ENABLEGROUPVIEW, 0, 0); }
	int ensure_visible()                                                 { return (int)sendmsg(LVM_ENSUREVISIBLE, 0, 0); }
	int find_item()                                                      { return (int)sendmsg(LVM_FINDITEM, 0, 0); }
	int get_background_color()                                           { return (int)sendmsg(LVM_GETBKCOLOR, 0, 0); }
	int get_background_image()                                           { return (int)sendmsg(LVM_GETBKIMAGE, 0, 0); }
	int get_callback_mask()                                              { return (int)sendmsg(LVM_GETCALLBACKMASK, 0, 0); }
	int get_column()                                                     { return (int)sendmsg(LVM_GETCOLUMN, 0, 0); }
	int get_column_order_array()                                         { return (int)sendmsg(LVM_GETCOLUMNORDERARRAY, 0, 0); }
	int get_column_width(int index)                                      { return (int)sendmsg(LVM_GETCOLUMNWIDTH, (WPARAM)index, 0); }
	int get_count_per_page()                                             { return (int)sendmsg(LVM_GETCOUNTPERPAGE, 0, 0); }
	int get_edit_control()                                               { return (int)sendmsg(LVM_GETEDITCONTROL, 0, 0); }
	int get_empty_text()                                                 { return (int)sendmsg(LVM_GETEMPTYTEXT, 0, 0); }
	int get_extended_listview_style()                                    { return (int)sendmsg(LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0); }
	int get_focused_group()                                              { return (int)sendmsg(LVM_GETFOCUSEDGROUP, 0, 0); }
	int get_footer_info()                                                { return (int)sendmsg(LVM_GETFOOTERINFO, 0, 0); }
	int get_footer_item()                                                { return (int)sendmsg(LVM_GETFOOTERITEM, 0, 0); }
	int get_footer_item_rect()                                           { return (int)sendmsg(LVM_GETFOOTERITEMRECT, 0, 0); }
	int get_footer_rect()                                                { return (int)sendmsg(LVM_GETFOOTERRECT, 0, 0); }
	int get_group_count()                                                { return (int)sendmsg(LVM_GETGROUPCOUNT, 0, 0); }
	int get_group_info()                                                 { return (int)sendmsg(LVM_GETGROUPINFO, 0, 0); }
	int get_group_info_by_index()                                        { return (int)sendmsg(LVM_GETGROUPINFOBYINDEX, 0, 0); }
	int get_group_metrics()                                              { return (int)sendmsg(LVM_GETGROUPMETRICS, 0, 0); }
	int get_group_rect()                                                 { return (int)sendmsg(LVM_GETGROUPRECT, 0, 0); }
	int get_group_state()                                                { return (int)sendmsg(LVM_GETGROUPSTATE, 0, 0); }
	int get_header()                                                     { return (int)sendmsg(LVM_GETHEADER, 0, 0); }
	int get_hot_cursor()                                                 { return (int)sendmsg(LVM_GETHOTCURSOR, 0, 0); }
	int get_hot_item()                                                   { return (int)sendmsg(LVM_GETHOTITEM, 0, 0); }
	int get_hover_time()                                                 { return (int)sendmsg(LVM_GETHOVERTIME, 0, 0); }
	int get_image_list()                                                 { return (int)sendmsg(LVM_GETIMAGELIST, 0, 0); }
	int get_insert_mark()                                                { return (int)sendmsg(LVM_GETINSERTMARK, 0, 0); }
	int get_insert_mark_color()                                          { return (int)sendmsg(LVM_GETINSERTMARKCOLOR, 0, 0); }
	int get_insert_mark_rect()                                           { return (int)sendmsg(LVM_GETINSERTMARKRECT, 0, 0); }
	int get_incremental_search_string()                                  { return (int)sendmsg(LVM_GETISEARCHSTRING, 0, 0); }
	int get_item()                                                       { return (int)sendmsg(LVM_GETITEM, 0, 0); }
	int get_item_count()                                                 { return (int)sendmsg(LVM_GETITEMCOUNT, 0, 0); }
	int get_item_index_rect()                                            { return (int)sendmsg(LVM_GETITEMINDEXRECT, 0, 0); }
	int get_item_position()                                              { return (int)sendmsg(LVM_GETITEMPOSITION, 0, 0); }
	int get_item_rect()                                                  { return (int)sendmsg(LVM_GETITEMRECT, 0, 0); }
	int get_item_spacing()                                               { return (int)sendmsg(LVM_GETITEMSPACING, 0, 0); }
	int get_item_state()                                                 { return (int)sendmsg(LVM_GETITEMSTATE, 0, 0); }
	int get_item_text()                                                  { return (int)sendmsg(LVM_GETITEMTEXT, 0, 0); }
	int get_next_item()                                                  { return (int)sendmsg(LVM_GETNEXTITEM, 0, 0); }
	int get_next_item_index()                                            { return (int)sendmsg(LVM_GETNEXTITEMINDEX, 0, 0); }
	int get_number_of_work_areas()                                       { return (int)sendmsg(LVM_GETNUMBEROFWORKAREAS, 0, 0); }
	int get_origin()                                                     { return (int)sendmsg(LVM_GETORIGIN, 0, 0); }
	int get_outline_color()                                              { return (int)sendmsg(LVM_GETOUTLINECOLOR, 0, 0); }
	int get_selected_column()                                            { return (int)sendmsg(LVM_GETSELECTEDCOLUMN, 0, 0); }
	int get_selected_count()                                             { return (int)sendmsg(LVM_GETSELECTEDCOUNT, 0, 0); }
	int get_selection_mark()                                             { return (int)sendmsg(LVM_GETSELECTIONMARK, 0, 0); }
	int get_string_width()                                               { return (int)sendmsg(LVM_GETSTRINGWIDTH, 0, 0); }
	int get_subitem_rect()                                               { return (int)sendmsg(LVM_GETSUBITEMRECT, 0, 0); }
	int get_text_background_color()                                      { return (int)sendmsg(LVM_GETTEXTBKCOLOR, 0, 0); }
	int get_text_color()                                                 { return (int)sendmsg(LVM_GETTEXTCOLOR, 0, 0); }
	int get_tile_info()                                                  { return (int)sendmsg(LVM_GETTILEINFO, 0, 0); }
	int get_tile_view_info()                                             { return (int)sendmsg(LVM_GETTILEVIEWINFO, 0, 0); }
	int get_tooltips()                                                   { return (int)sendmsg(LVM_GETTOOLTIPS, 0, 0); }
	int get_top_index()                                                  { return (int)sendmsg(LVM_GETTOPINDEX, 0, 0); }
	int get_unicode_format()                                             { return (int)sendmsg(LVM_GETUNICODEFORMAT, 0, 0); }
	int get_view()                                                       { return (int)sendmsg(LVM_GETVIEW, 0, 0); }
	int get_view_rect()                                                  { return (int)sendmsg(LVM_GETVIEWRECT, 0, 0); }
	int get_work_areas()                                                 { return (int)sendmsg(LVM_GETWORKAREAS, 0, 0); }
	int has_group()                                                      { return (int)sendmsg(LVM_HASGROUP, 0, 0); }
	int hit_test()                                                       { return (int)sendmsg(LVM_HITTEST, 0, 0); }
//	int insert_column()                                                  { return (int)sendmsg(LVM_INSERTCOLUMN, 0, 0); }
	int insert_column(int index, LPCTSTR title)                          { LVCOLUMN lvc = { LVCF_TEXT, 0, 0, (LPTSTR)title, 0, index };
	                                                                       return (int)sendmsg(LVM_INSERTCOLUMN, (WPARAM)index, (LPARAM)&lvc); }
	int insert_group()                                                   { return (int)sendmsg(LVM_INSERTGROUP, 0, 0); }
	int insert_group_sorted()                                            { return (int)sendmsg(LVM_INSERTGROUPSORTED, 0, 0); }
//	int insert_item()                                                    { return (int)sendmsg(LVM_INSERTITEM, 0, 0); }
	int insert_item(int index, LPCTSTR text)                             { LVITEM lvi = { LVIF_TEXT, index, 0, 0, 0, (LPTSTR)text };
	                                                                       return (int)sendmsg(LVM_INSERTITEM, 0, (LPARAM)&lvi); }
	int insert_mark_hit_test()                                           { return (int)sendmsg(LVM_INSERTMARKHITTEST, 0, 0); }
	int is_group_view_enabled()                                          { return (int)sendmsg(LVM_ISGROUPVIEWENABLED, 0, 0); }
	int is_item_visible()                                                { return (int)sendmsg(LVM_ISITEMVISIBLE, 0, 0); }
	int map_id_to_index()                                                { return (int)sendmsg(LVM_MAPIDTOINDEX, 0, 0); }
	int map_index_to_id()                                                { return (int)sendmsg(LVM_MAPINDEXTOID, 0, 0); }
	int move_group()                                                     { return (int)sendmsg(LVM_MOVEGROUP, 0, 0); }
	int move_item_to_group()                                             { return (int)sendmsg(LVM_MOVEITEMTOGROUP, 0, 0); }
	int redraw_items()                                                   { return (int)sendmsg(LVM_REDRAWITEMS, 0, 0); }
	int remove_all_groups()                                              { return (int)sendmsg(LVM_REMOVEALLGROUPS, 0, 0); }
	int remove_group()                                                   { return (int)sendmsg(LVM_REMOVEGROUP, 0, 0); }
	int scroll()                                                         { return (int)sendmsg(LVM_SCROLL, 0, 0); }
	int set_background_color()                                           { return (int)sendmsg(LVM_SETBKCOLOR, 0, 0); }
	int set_background_image()                                           { return (int)sendmsg(LVM_SETBKIMAGE, 0, 0); }
	int set_callback_mask()                                              { return (int)sendmsg(LVM_SETCALLBACKMASK, 0, 0); }
	int set_column()                                                     { return (int)sendmsg(LVM_SETCOLUMN, 0, 0); }
	int set_column_order_array()                                         { return (int)sendmsg(LVM_SETCOLUMNORDERARRAY, 0, 0); }
	int set_column_width(int index, int cx)                              { return (int)sendmsg(LVM_SETCOLUMNWIDTH, (WPARAM)index, (LPARAM)cx); }
	int set_extended_listview_style()                                    { return (int)sendmsg(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, 0); }
	int set_group_info()                                                 { return (int)sendmsg(LVM_SETGROUPINFO, 0, 0); }
	int set_group_metrics()                                              { return (int)sendmsg(LVM_SETGROUPMETRICS, 0, 0); }
	int set_hot_cursor()                                                 { return (int)sendmsg(LVM_SETHOTCURSOR, 0, 0); }
	int set_hot_item()                                                   { return (int)sendmsg(LVM_SETHOTITEM, 0, 0); }
	int set_hover_time()                                                 { return (int)sendmsg(LVM_SETHOVERTIME, 0, 0); }
	int set_icon_spacing()                                               { return (int)sendmsg(LVM_SETICONSPACING, 0, 0); }
	int set_image_list()                                                 { return (int)sendmsg(LVM_SETIMAGELIST, 0, 0); }
	int set_infotip()                                                    { return (int)sendmsg(LVM_SETINFOTIP, 0, 0); }
	int set_insert_mark()                                                { return (int)sendmsg(LVM_SETINSERTMARK, 0, 0); }
	int set_insert_mark_color()                                          { return (int)sendmsg(LVM_SETINSERTMARKCOLOR, 0, 0); }
//	int set_item()                                                       { return (int)sendmsg(LVM_SETITEM, 0, 0); }
	int set_item(int index, int sub_index, LPCTSTR text)                 { LVITEM lvi = { LVIF_TEXT, index, sub_index, 0, 0, (LPTSTR)text };
	                                                                       return (int)sendmsg(LVM_SETITEM, 0, (LPARAM)&lvi); }
	int set_item_count()                                                 { return (int)sendmsg(LVM_SETITEMCOUNT, 0, 0); }
	int set_item_index_state()                                           { return (int)sendmsg(LVM_SETITEMINDEXSTATE, 0, 0); }
	int set_item_position()                                              { return (int)sendmsg(LVM_SETITEMPOSITION, 0, 0); }
	int set_item_position32()                                            { return (int)sendmsg(LVM_SETITEMPOSITION32, 0, 0); }
	int set_item_state()                                                 { return (int)sendmsg(LVM_SETITEMSTATE, 0, 0); }
	int set_item_text()                                                  { return (int)sendmsg(LVM_SETITEMTEXT, 0, 0); }
	int set_outline_color()                                              { return (int)sendmsg(LVM_SETOUTLINECOLOR, 0, 0); }
	int set_selected_column()                                            { return (int)sendmsg(LVM_SETSELECTEDCOLUMN, 0, 0); }
	int set_selection_mark()                                             { return (int)sendmsg(LVM_SETSELECTIONMARK, 0, 0); }
	int set_text_background_color()                                      { return (int)sendmsg(LVM_SETTEXTBKCOLOR, 0, 0); }
	int set_text_color()                                                 { return (int)sendmsg(LVM_SETTEXTCOLOR, 0, 0); }
	int set_tile_info()                                                  { return (int)sendmsg(LVM_SETTILEINFO, 0, 0); }
	int set_tile_view_info()                                             { return (int)sendmsg(LVM_SETTILEVIEWINFO, 0, 0); }
//	int set_tile_width()                                                 { return (int)sendmsg(LVM_SETTILEWIDTH, 0, 0); }
	int set_tooltips()                                                   { return (int)sendmsg(LVM_SETTOOLTIPS, 0, 0); }
	int set_unicode_format()                                             { return (int)sendmsg(LVM_SETUNICODEFORMAT, 0, 0); }
	int set_view()                                                       { return (int)sendmsg(LVM_SETVIEW, 0, 0); }
	int set_work_areas()                                                 { return (int)sendmsg(LVM_SETWORKAREAS, 0, 0); }
	int sort_groups()                                                    { return (int)sendmsg(LVM_SORTGROUPS, 0, 0); }
	int sort_items()                                                     { return (int)sendmsg(LVM_SORTITEMS, 0, 0); }
	int sort_items_ex()                                                  { return (int)sendmsg(LVM_SORTITEMSEX, 0, 0); }
	int subitem_hit_test()                                               { return (int)sendmsg(LVM_SUBITEMHITTEST, 0, 0); }
	int update()                                                         { return (int)sendmsg(LVM_UPDATE, 0, 0); }

};

  } // namespace native

//////////////////////////////////////////////////////////////////////
/// ボタン
class button
	: public native::button
{
};

//////////////////////////////////////////////////////////////////////
/// チェックボックス
class checkbox
	: public native::button
{
protected:

	//bool draw(HDC hdc, const RECT & rect, LPCTSTR caption, spplib::tribool checked, bool grayed, bool highlighted, bool pressed)
	//{
	//	HTHEME theme_handle = ::OpenThemeData(hwndButton, L"Button");
	//	HRESULT hr;
	//	RECT rc = rect, rcContent;

	//	int state = true == checked ? (!grayed ? CBS_CHECKEDDISABLED
	//	                                       : pressed ? CBS_CHECKEDPRESSED
	//	                                                 : highlighted ? CBS_CHECKEDHOT
	//	                                                               : CBS_CHECKEDNORMAL)
	//	          : false== checked ? (!grayed ? CBS_UNCHECKEDDISABLED
	//	                                       : pressed ? CBS_UNCHECKEDPRESSED
	//	                                                 : highlighted ? CBS_UNCHECKEDHOT
	//	                                                               : CBS_UNCHECKEDNORMAL)
	//	                            : (!grayed ? CBS_MIXEDDISABLED
	//	                                       : pressed ? CBS_MIXEDPRESSED
	//	                                                 : highlighted ? CBS_MIXEDHOT
	//	                                                               : CBS_MIXEDNORMAL)
	//	                            ;

	//	if( theme_handle )
	//	{
	//		hr = ::DrawThemeBackground(theme_handle, hdc, BP_BUTTON, state, &rc, 0);
	//		// Always check your result codes.

	//		hr = ::GetThemeBackgroundContentRect(theme_handle, hdc, BP_BUTTON, state, &rc, &rcContent);
	//		hr = ::DrawThemeText(theme_handle, hdc, BP_BUTTON, state, caption, -1, DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &rcContent);

	//		::CloseTheme(theme_handle);
	//	}
	//}

public:

	bool check(bool check)
	{
		bool checked = is_checked();
		set_check(check ? BST_CHECKED : BST_UNCHECKED);
		return checked;
	}

	bool is_checked()
	{
		return BST_CHECKED == get_check();
	}
};







 } // namespace fw
} // namespace spplib

#endif // !defined(SPPLIB_465F9279_68C0_44CA_A8F0_9F8FCC04C7BC_INCLUDE_GUARD)
