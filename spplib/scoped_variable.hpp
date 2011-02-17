/**
 * @file  spplib/scoped_variable.hpp
 * @brief 解放メソッド指定可能変数クラス
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_5D02EBF5_6CD1_4258_A483_0D4FBF7E893D_INCLUDE_GUARD
#define SPPLIB_5D02EBF5_6CD1_4258_A483_0D4FBF7E893D_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {

/**
 * @brief 解放メソッド指定可能変数クラス
 * @tparam T 変数の型
 * @code
 *   spplib::scoped_variable<>
 * @endcode
 */
template<typename T>
class scoped_variable
{
	scoped_variable();
	scoped_variable(const scoped_variable<T> &);
	scoped_variable<T> & operator = (const scoped_variable<T> &);

private:

	template<typename Ty>
	class placeholder {
		placeholder<T> & operator = (const placeholder<T> &);
	protected:
		const Ty & m_value;
	public:
		placeholder(const Ty & v) : m_value(v) {}
		virtual ~placeholder() {}
		const Ty & get() const { return m_value; }
	};
	template<typename Ty, typename Tfn>
	struct holder : public placeholder<Ty> {
		Tfn  m_fn;
		holder(const Ty & v, Tfn fn)
			: placeholder(v), m_fn(fn)
		{}
		virtual ~holder() { m_fn(m_value); }
	};
	placeholder<T> * m_holder;

public:

	template<typename Tfn>
	scoped_variable(const T & v, Tfn fn)
		: m_holder(new holder<T, Tfn>(v, fn))
	{
	}

	~scoped_variable()
	{
		delete m_holder;
	}

	const T & get() const
	{
		return m_holder->get();
	}

	operator const T & () const
	{
		return m_holder->get();
	}
};

} // namespace spplib

#endif // !defined(SPPLIB_5D02EBF5_6CD1_4258_A483_0D4FBF7E893D_INCLUDE_GUARD)
