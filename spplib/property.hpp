/**
 * @file  spplib/property.hpp
 * @brief プロパティテンプレート定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_645FA524_1A0B_4BBE_8F7A_1E7E1811F13C_INCLUDE_GUARD
#define SPPLIB_645FA524_1A0B_4BBE_8F7A_1E7E1811F13C_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {

// RW
template<typename T, typename To, const T & (To::*setter)(const T & ), const T & (To::*getter)() const>
class rw_property
{
	To * m_owner;
private:
	rw_property(const rw_property & );
	const rw_property & operator = (const rw_property & );
public:
	explicit rw_property(To * owner) : m_owner(owner) {}
	~rw_property() {}

	const T & set(const T & rhs)         { return (m_owner->*setter)(rhs); }
	const T & operator = (const T & rhs) { return (m_owner->*setter)(rhs); }

	const T & get() const                { return (m_owner->*getter)();    }
	operator const T & () const          { return (m_owner->*getter)();    }
};

// RO
template<typename T, typename To, const T & (To::*getter)() const>
class ro_property
{
	To * m_owner;
private:
	ro_property(const ro_property & );
	const ro_property & operator = (const ro_property & );
	const T & set(const T & rhs);
	const T & operator = (const T & rhs);
public:
	explicit ro_property(To * owner) : m_owner(owner) {}
	~ro_property() {}

	const T & get() const                { return (m_owner->*getter)();    }
	operator const T & () const          { return (m_owner->*getter)();    }
};

// WO
template<typename T, typename To, const T & (To::*setter)(const T & )>
class wo_property
{
	To * m_owner;
private:
	wo_property(const wo_property & );
	const wo_property & operator = (const wo_property & );
	const T & get() const;
	operator const T & () const;
public:
	explicit wo_property(To * owner) : m_owner(owner) {}
	~wo_property() {}

	const T & set(const T & rhs)         { return (m_owner->*setter)(rhs); }
	const T & operator = (const T & rhs) { return (m_owner->*setter)(rhs); }
};

} // namespace spplib

#endif // !defined(SPPLIB_645FA524_1A0B_4BBE_8F7A_1E7E1811F13C_INCLUDE_GUARD)

//EOF
