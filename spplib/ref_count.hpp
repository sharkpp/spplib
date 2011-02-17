/**
 * @file  spplib/ref_count.hpp
 * @brief 参照カウントテンプレート
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2009-2010.
 */

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_FCE67E7A_FC6E_40E9_A3A1_CC7EE1DA6121_INCLUDE_GUARD
#define SPPLIB_FCE67E7A_FC6E_40E9_A3A1_CC7EE1DA6121_INCLUDE_GUARD

/// sharkpp class library namespace
namespace spplib {

template<typename T>
class simple_ref_counter
{
private:

	int		size;		///< 確保サイズ
	T **	pointers;	///< NULL終端

public:

	simple_ref_counter(T* ptr = NULL)
		: size(1)
		, pointers(new T*[size])
	{
		::memset(pointers, 0, sizeof(T**) * size);
		if( ptr )
		{
			push(ptr);
		}
	}

	~simple_ref_counter()
	{
		delete [] pointers;
	}

	int ref_count() const
	{
		int i;
		for(i = 0; pointers[i]; i++)
			{ }
		return i;
	}

	void push(T * ptr)
	{
		int i;
		for(i = 0; pointers[i] && ptr != pointers[i]; i++)
			{ }
		if( ptr == pointers[i] )
		{
			return;
		}
		if( size <= i + 1 )
		{
			size += 1;
			T ** tmp_pointers = pointers;
			pointers = new T*[size];
			::memset(pointers + i, 0xFF, sizeof(T**) * (size - i));
			::memset(pointers + i, 0, sizeof(T**) * (size - i));
			::memcpy(pointers, tmp_pointers, sizeof(T**) * i);
			delete [] tmp_pointers;
		}
		pointers[i] = ptr;
	}

	void pop(T * ptr)
	{
		int i;
		for(i = 0; pointers[i] && ptr != pointers[i]; i++)
			{ }
#if defined(_DEBUG)
		if( !pointers[i] )
		{
			__asm int 3
		}
#endif
		::memcpy(pointers + i, pointers + i + 1, sizeof(T**) * (size - i - 1));

		if( !pointers[0] )
		{
			ptr->release();
			delete this;
		}
	}
};

// 単純な参照カウンタテンプレート(CRTP用)
template<typename T>
class ref_count_crtp
{
private:

	typedef ref_count_crtp<T> Tmy;

	simple_ref_counter<Tmy>*	m_ref_counter;

public:

	ref_count_crtp()
		: m_ref_counter(new simple_ref_counter<Tmy>())
	{
		m_ref_counter->push(this);
	}

	ref_count_crtp(const ref_count_crtp & value)
		: m_ref_counter(value.m_ref_counter)
	{
		m_ref_counter->push(this);
	}

	virtual ~ref_count_crtp()
	{
		m_ref_counter->pop(this);
	}

	virtual void release()
	{
	}

private:

	Tmy & operator = (const Tmy & value);

};

// 単純な参照カウンタテンプレート
template<typename T>
class ref_count
{
private:

	typedef ref_count<T> Tmy;

	simple_ref_counter<Tmy>*	m_ref_counter;

	T							m_value;

	friend class simple_ref_counter<Tmy>;

public:

	ref_count()
		: m_ref_counter(new simple_ref_counter<Tmy>())
	{
		m_ref_counter->push(this);
	}

	ref_count(const Tmy & value)
		: m_ref_counter(value.m_ref_counter)
	{
		m_ref_counter->push(this);
	}

	virtual ~ref_count()
	{
		m_ref_counter->pop(this);
	}

	Tmy & operator = (const Tmy & value)
	{
		if( this != &value )
		{
			m_ref_counter->pop(this);
			m_ref_counter = value.m_ref_counter;
			m_ref_counter->push(this);
			m_value = value.m_value;
		}
		return *this;
	}

	T * operator -> ()
	{
		return &m_value;
	}

private:

	T & operator = (const T & value);

	void release() { }
};


// 単純な参照カウンタテンプレート(ポインタ用)
template<typename T>
class ref_count_ptr
{
private:

	typedef ref_count_ptr<T> Tmy;

	simple_ref_counter<Tmy>*	m_ref_counter;

	T*							m_value;

	friend class simple_ref_counter<Tmy>;

public:

	ref_count_ptr()
		: m_ref_counter(new simple_ref_counter<Tmy>())
		, m_value(NULL)
	{
		m_ref_counter->push(this);
	}

	ref_count_ptr(T * value)
		: m_ref_counter(new simple_ref_counter<Tmy>())
		, m_value(value)
	{
		m_ref_counter->push(this);
	}

	ref_count_ptr(const Tmy & value)
		: m_ref_counter(value.m_ref_counter)
		, m_value(value.m_value)
	{
		m_ref_counter->push(this);
	}

	virtual ~ref_count_ptr()
	{
		m_ref_counter->pop(this);
	}

	Tmy & operator = (const Tmy & value)
	{
		if( this != &value )
		{
			m_ref_counter->pop(this);
			m_ref_counter = value.m_ref_counter;
			m_ref_counter->push(this);
			m_value = value.m_value;
		}
		return *this;
	}

	Tmy & operator = (const T * value)
	{
		m_ref_counter->pop(this);
		m_ref_counter = new simple_ref_counter<Tmy>();
		m_ref_counter->push(this);
		m_value = value;
		return *this;
	}

	T * operator -> ()
	{
		return m_value;
	}

	const T * operator -> () const
	{
		return m_value;
	}

	const T operator * () const
	{
		return *m_value;
	}

	const T * get() const
	{
		return m_value;
	}

private:

	void release()
	{
		delete [] m_value;
	}
};

} // namespace spplib

#endif // !defined(SPPLIB_FCE67E7A_FC6E_40E9_A3A1_CC7EE1DA6121_INCLUDE_GUARD)
