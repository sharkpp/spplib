/**
 * @file  spplib/tribool.hpp
 * @brief 3ステートブーリアン定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_CE9C295A_3A01_4431_BC41_F605750C653B_INCLUDE_GUARD
#define SPPLIB_CE9C295A_3A01_4431_BC41_F605750C653B_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {

typedef enum { false_, true_, indeterminate } tribool_type;

class tribool
{
	unsigned char m_state;
public:
	tribool(bool b)         : m_state(b ? 1 : 0)        {}
	tribool(tribool_type b) : m_state((unsigned char)b) {}

	tribool & operator = (bool b)         { m_state = b ? 1 : 0;        return *this; }
	tribool & operator = (tribool_type b) { m_state = (unsigned char)b; return *this; }

	operator bool   () { return 1 == m_state; }
	bool operator ! () { return 0 == m_state; }

	tribool operator == (const tribool & rhs) { return m_state == rhs.m_state; }
	tribool operator != (const tribool & rhs) { return m_state != rhs.m_state; }
	tribool operator && (const tribool & rhs) { return 1 == m_state && 1 == rhs.m_state ? true_ : 0 == m_state && 0 == rhs.m_state ? false_ : indeterminate; }
	tribool operator || (const tribool & rhs) { return 1 == m_state || 1 == rhs.m_state ? true_ : 2 == m_state || 2 == rhs.m_state ? indeterminate : false_; }
};

inline tribool operator == (bool            lhs, const tribool & rhs) { return tribool(lhs) == rhs; }
inline tribool operator == (const tribool & lhs, bool            rhs) { return lhs          == tribool(rhs); }
inline tribool operator == (const tribool & lhs, tribool_type    rhs) { return lhs          == tribool(rhs); }
inline tribool operator == (tribool_type    lhs, const tribool & rhs) { return tribool(lhs) == rhs; }

inline tribool operator != (bool            lhs, const tribool & rhs) { return tribool(lhs) != rhs; }
inline tribool operator != (const tribool & lhs, bool            rhs) { return lhs          != tribool(rhs); }
inline tribool operator != (const tribool & lhs, tribool_type    rhs) { return lhs          != tribool(rhs); }
inline tribool operator != (tribool_type    lhs, const tribool & rhs) { return tribool(lhs) != rhs; }

inline tribool operator && (bool            lhs, const tribool & rhs) { return tribool(lhs) && rhs; }
inline tribool operator && (const tribool & lhs, bool            rhs) { return lhs          && tribool(rhs); }
inline tribool operator && (const tribool & lhs, tribool_type    rhs) { return lhs          && tribool(rhs); }
inline tribool operator && (tribool_type    lhs, const tribool & rhs) { return tribool(lhs) && rhs; }

inline tribool operator || (bool            lhs, const tribool & rhs) { return tribool(lhs) || rhs; }
inline tribool operator || (const tribool & lhs, bool            rhs) { return lhs          || tribool(rhs); }
inline tribool operator || (const tribool & lhs, tribool_type    rhs) { return lhs          || tribool(rhs); }
inline tribool operator || (tribool_type    lhs, const tribool & rhs) { return tribool(lhs) || rhs; }

} // namespace spplib

#endif // !defined(SPPLIB_CE9C295A_3A01_4431_BC41_F605750C653B_INCLUDE_GUARD)

//EOF
