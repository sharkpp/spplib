/**
 * @file  spplib/container/shmvector.hpp
 * @brief 共有メモリベクターコンテナクラス定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include "../shared_memory.hpp"
#include <assert.h>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_ABF60B16_60D3_4F28_BAD6_5EDF13B2AB20_INCLUDE_GUARD
#define SPPLIB_ABF60B16_60D3_4F28_BAD6_5EDF13B2AB20_INCLUDE_GUARD

/// sharkpp class library namespace
namespace spplib {
 /// containers namespace
 namespace container {

/**
 * @brief 共有メモリオブジェクト
 * @code
 *  spplib::shared_memory mem("test", 1024);
 *  LPTSTR ptr = (LPTSTR)mem.lock();
 *  ::lstrcpy(ptr, "1234");
 *  mem.unlock();
 * @endcode
 */
template<typename T>
class shmvector
{
public:

	typedef shmvector<T> mine_type;
	typedef T            value_type;

	typedef unsigned int size_type;

private:

#pragma pack(push, 1)

	typedef struct {
		size_type	count;
	} SHM_HDR_TYPE;

#pragma pack(pop)

	// メモリ構造
	// +---------------+
	// | size_t count  |
	// +---------------+
	// | T [0]         |
	// :       :       :
	// | T [count - 1] |
	// :       :       :
	// | FREE AREA     |
	// +---------------+

	mutable spplib::shared_memory m_shm;
	const size_type               m_num;

	shmvector(const shmvector & );
	shmvector & operator = (const shmvector & );

public:

	shmvector(const char * name, size_type num = 1024)
		: m_shm(name, sizeof(SHM_HDR_TYPE) + num * sizeof(value_type))
		, m_num(num)
	{
	}

	~shmvector()
	{
	}

	bool get_exists_state() const
	{
		return m_shm.get_exists_state();
	}

	bool empty() const
	{
		SHM_HDR_TYPE* hdr = static_cast<SHM_HDR_TYPE*>(m_shm.lock());
		bool r = !hdr->count;
		m_shm.unlock();
		return r;
	}

	size_t size() const
	{
		SHM_HDR_TYPE* hdr = static_cast<SHM_HDR_TYPE*>(m_shm.lock());
		size_type r = hdr->count;
		m_shm.unlock();
		return size_t(r);
	}

	value_type back()
	{
		SHM_HDR_TYPE* hdr = static_cast<SHM_HDR_TYPE*>(m_shm.lock());
		value_type*   ptr = reinterpret_cast<value_type*>(hdr + 1);
		value_type    r;
		if( hdr->count ) {
			r = ptr[hdr->count - 1];
			m_shm.unlock();
		} else {
			m_shm.unlock();
			assert(false);
		}
		return r;
	}

	bool push_back(const value_type & v)
	{
		SHM_HDR_TYPE* hdr = static_cast<SHM_HDR_TYPE*>(m_shm.lock());
		value_type*   ptr = reinterpret_cast<value_type*>(hdr + 1);
		if( m_num != hdr->count ) {
			ptr[hdr->count] = v;
			hdr->count++;
			m_shm.unlock();
			return true;
		} else {
			m_shm.unlock();
			return false;
		}
	}

	void pop_back()
	{
		SHM_HDR_TYPE* hdr = static_cast<SHM_HDR_TYPE*>(m_shm.lock());
		if( 0 < hdr->count ) {
			hdr->count--;
			m_shm.unlock();
		} else {
			m_shm.unlock();
			assert(false);
		}
	}

};

 } // namespace container
} // namespace spplib

#endif // !defined(SPPLIB_ABF60B16_60D3_4F28_BAD6_5EDF13B2AB20_INCLUDE_GUARD)
