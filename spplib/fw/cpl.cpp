/**
 * @file  spplib/fw/cpl.cpp
 * @brief コントロールパネル フレームワーク クラス実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include "cpl.hpp"

/// sharkpp class library namespace
namespace spplib {
 /// framework namespace
 namespace fw {

static cpl * g_cpl = NULL;

cpl::cpl()
{
	g_cpl = this;
}

 } // namespace fw
} // namespace spplib

LONG CPlApplet(
    HWND hwndCPl,
    UINT uMsg,
    LPARAM lParam1,
    LPARAM lParam2
)
{
	return (*spplib::fw::g_cpl)(hwndCPl, uMsg, lParam1, lParam2);
}
