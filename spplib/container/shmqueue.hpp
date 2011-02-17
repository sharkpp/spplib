/**
 * @file  spplib/container/shmqueue.hpp
 * @brief 共有メモリキューコンテナクラス定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include "../shared_memory.hpp"
#include <assert.h>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_BF57BED2_9ED7_47CD_9EC2_EFBF087FFFBE_INCLUDE_GUARD
#define SPPLIB_BF57BED2_9ED7_47CD_9EC2_EFBF087FFFBE_INCLUDE_GUARD

/// sharkpp class library namespace
namespace spplib {
 /// containers namespace
 namespace container {

/**
 * @brief 共有メモリキューコンテナクラス
 * @code
 *  spplib::container::shmqueue<int> que("test", 1024);
 *  for(int i = 0; i < 10; i++)
 *    que.push_back(i);
 *  for(int i = 0; i < 10; i++) {
 *    printf("%d\n", que.back());
 *    que.pop_back();
 *  }
 * @endcode
 */
template<typename T>
class shmqueue
{
public:

	typedef shmqueue<T> mine_type;
	typedef T           value_type;

	typedef unsigned int size_type;

private:

#pragma pack(push, 1)

	typedef struct {
		size_type	front;
		size_type	back;
	} SHM_HDR_TYPE;

#pragma pack(pop)

	// メモリ構造
	// +------------------+
	// | size_t front     |
	// | size_t back      |
	// +------------------+
	// | T [0]            |
	// :       :          :
	// | T [n]            |
	// :       :          :
	// | FREE AREA        |
	// +------------------+

	mutable spplib::shared_memory m_shm;
	const size_type               m_num;

	shmqueue(const shmqueue & );
	shmqueue & operator = (const shmqueue & );

public:

	shmqueue(const char * name, size_type num = 1024)
		: m_shm(name, sizeof(SHM_HDR_TYPE) + num * sizeof(value_type))
		, m_num(num)
	{
	}

	~shmqueue()
	{
	}

	bool get_exists_state() const
	{
		return m_shm.get_exists_state();
	}

	bool empty() const
	{
		SHM_HDR_TYPE* hdr = static_cast<SHM_HDR_TYPE*>(m_shm.lock());
		bool r = hdr->front == hdr->back;
		m_shm.unlock();
		return r;
	}

	size_t size() const
	{
		SHM_HDR_TYPE* hdr = static_cast<SHM_HDR_TYPE*>(m_shm.lock());
		size_type r = (hdr->back + m_num - hdr->front) % m_num;
		m_shm.unlock();
		return size_t(r);
	}

	value_type front()
	{
		SHM_HDR_TYPE* hdr = static_cast<SHM_HDR_TYPE*>(m_shm.lock());
		value_type*   ptr = reinterpret_cast<value_type*>(hdr + 1);
		value_type    r;
		if( hdr->front != hdr->back ) {
			r = ptr[hdr->front];
			m_shm.unlock();
		} else {
			m_shm.unlock();
			assert(false);
		}
		return r;
	}

	bool push_front(const value_type & v)
	{
		SHM_HDR_TYPE* hdr = static_cast<SHM_HDR_TYPE*>(m_shm.lock());
		value_type*   ptr = reinterpret_cast<value_type*>(hdr + 1);
		size_type new_front = (hdr->front + m_num - 1) % m_num;
		if( new_front != hdr->back ) {
			hdr->front = new_front;
			ptr[hdr->front] = v;
			m_shm.unlock();
			return true;
		} else {
			m_shm.unlock();
			return false;
		}
	}

	void pop_front()
	{
		SHM_HDR_TYPE* hdr = static_cast<SHM_HDR_TYPE*>(m_shm.lock());
		if( hdr->front != hdr->back ) {
			hdr->front = (hdr->front + 1) % m_num;
			m_shm.unlock();
		} else {
			m_shm.unlock();
			assert(false);
		}
	}

	value_type back()
	{
		SHM_HDR_TYPE* hdr = static_cast<SHM_HDR_TYPE*>(m_shm.lock());
		value_type*   ptr = reinterpret_cast<value_type*>(hdr + 1);
		value_type    r;
		if( hdr->front != hdr->back ) {
			r = ptr[(hdr->back + m_num - 1) % m_num];
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
		size_type new_back = (hdr->back + 1) % m_num;
		if( hdr->front != new_back ) {
			ptr[hdr->back] = v;
			hdr->back = new_back;
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
		if( hdr->front != hdr->back ) {
			hdr->back = (hdr->back + m_num - 1) % m_num;
			m_shm.unlock();
		} else {
			m_shm.unlock();
			assert(false);
		}
	}

};

 } // namespace container
} // namespace spplib

#endif // !defined(SPPLIB_BF57BED2_9ED7_47CD_9EC2_EFBF087FFFBE_INCLUDE_GUARD)
