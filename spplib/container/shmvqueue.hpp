/**
 * @file  spplib/container/shmvqueue.hpp
 * @brief 共有メモリキューコンテナクラス定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include "../shared_memory.hpp"
#include <assert.h>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_870D4F1A_5C18_474A_B0FA_C8518B239487_INCLUDE_GUARD
#define SPPLIB_870D4F1A_5C18_474A_B0FA_C8518B239487_INCLUDE_GUARD

/// sharkpp class library namespace
namespace spplib {
 /// containers namespace
 namespace container {

/**
 * @brief 共有メモリキューコンテナクラス
 * @code
 *  spplib::container::shmqueue que("test", 1024);
 *  for(int i = 0; i < 10; i++)
 *    que.push_back(i);
 *  for(int i = 0; i < 10; i++) {
 *    printf("%d\n", que.back());
 *    que.pop_back();
 *  }
 * @endcode
 */
class shmvqueue
{
public:

	typedef unsigned int size_type;

private:

#pragma pack(push, 1)

	typedef struct {
		size_type	front;
		size_type	back_minus1;
		size_type	back;
		size_type	count;
	} SHM_HDR_TYPE;

	typedef struct {
		size_type	length;
	} SHM_DATA_HDR_TYPE;

	typedef struct : public SHM_DATA_HDR_TYPE {
		unsigned char data[1];
	} SHM_DATA_TYPE;

#pragma pack(pop)

	// メモリ構造
	// +-------------------+
	// | size_t front      |
	// | size_t back_minus1|
	// | size_t back       |
	// | size_t count      |
	// +-------------------+
	// | [0].length        |
	// | [0].data[]        |
	// :       :           :
	// | [n].length        |
	// | [n].data[]        |
	// :       :           :
	// | FREE AREA         |
	// +-------------------+

	mutable spplib::shared_memory m_shm;
	const size_type               m_length;

	mutable SHM_HDR_TYPE*         m_lock_ptr;

	shmvqueue(const shmvqueue & );
	shmvqueue & operator = (const shmvqueue & );

private:

	SHM_HDR_TYPE * lock_memory() const
	{
		return
			m_lock_ptr ? m_lock_ptr
			           : static_cast<SHM_HDR_TYPE*>(m_shm.lock());
	}

	void unlock_memory() const
	{
		if( !m_lock_ptr ) {
			m_shm.unlock();
		}
	}

public:

	shmvqueue(const TCHAR * name, size_t length = 1024 * 1024)
		: m_shm(name, static_cast<size_type>(sizeof(SHM_HDR_TYPE) + length + 1 /*番兵*/))
		, m_length(static_cast<size_type>(length + 1 /*番兵*/))
		, m_lock_ptr(NULL)
	{
	}

	~shmvqueue()
	{
	}

	bool get_exists_state() const
	{
		return m_shm.get_exists_state();
	}

	bool empty() const
	{
		SHM_HDR_TYPE* hdr = lock_memory();
		bool r = !hdr->count;
		unlock_memory();
		return r;
	}

	size_t size() const
	{
		SHM_HDR_TYPE* hdr = lock_memory();
		size_type r = hdr->count;
		unlock_memory();
		return size_t(r);
	}

	size_t capacity() const
	{
		return m_length - 1;
	}

	void lock()
	{
		if( !m_lock_ptr ) {
			m_lock_ptr = static_cast<SHM_HDR_TYPE*>(m_shm.lock());
		}
	}

	void unlock()
	{
		if( m_lock_ptr ) {
			m_lock_ptr = NULL;
			m_shm.unlock();
		}
	}

	template<typename T>
	T front()
	{
		T r;
		front(&r, sizeof(T));
		return r;
	}

	template<typename T>
	bool front(T & r)
	{
		return front(&r, sizeof(T));
	}

	bool front(void * buff, size_t size)
	{
		SHM_HDR_TYPE*  hdr = lock_memory();
		SHM_DATA_TYPE* ptr = reinterpret_cast<SHM_DATA_TYPE*>(
		                     reinterpret_cast<unsigned char*>(hdr + 1) + hdr->front);
		if( hdr->count && size == ptr->length ) {
			::memcpy(buff, ptr->data, size);
			unlock_memory();
			return true;
		} else {
			unlock_memory();
			assert(false);
			return false;
		}
	}

	template<typename T>
	bool push_front(const T & data)
	{
		return push_front(reinterpret_cast<unsigned char*>(&data),
		                  sizeof(T));
	}

	bool push_front(const void * data, size_t size)
	{
		SHM_HDR_TYPE*  hdr = lock_memory();
		size_type new_front = static_cast<size_type>(hdr->front - sizeof(SHM_DATA_HDR_TYPE) - size);
		if( m_length < new_front ) {
			new_front = static_cast<size_type>(m_length - sizeof(SHM_DATA_HDR_TYPE) - size);
		}
		if( (hdr->back + m_length - hdr->front) % m_length <
		    (hdr->back + m_length - new_front)  % m_length )
		{
			SHM_DATA_TYPE* ptr = reinterpret_cast<SHM_DATA_TYPE*>(
			                     reinterpret_cast<unsigned char*>(hdr + 1) + new_front);
			hdr->count++;
			hdr->front  = new_front;
			ptr->length = static_cast<size_type>(size);
			::memcpy(ptr->data, &data, size);
			if( 1 == hdr->count ) {
				hdr->back_minus1 = new_front;
			}
			unlock_memory();
			return true;
		} else {
			unlock_memory();
			return false;
		}
	}

	void pop_front()
	{
		SHM_HDR_TYPE* hdr = lock_memory();
		SHM_DATA_TYPE* ptr = reinterpret_cast<SHM_DATA_TYPE*>(
		                     reinterpret_cast<unsigned char*>(hdr + 1) + hdr->front);
		if( hdr->count ) {
			hdr->front += sizeof(SHM_DATA_HDR_TYPE) + ptr->length;
			if( m_length <= hdr->front ) {
				hdr->front = 0;
			}
			hdr->count--;
			unlock_memory();
		} else {
			unlock_memory();
			assert(false);
		}
	}

	template<typename T>
	T back()
	{
		T r;
		back(&r, sizeof(T));
		return r;
	}

	template<typename T>
	bool back(T & r)
	{
		return back(&r, sizeof(T));
	}

	bool back(void * buff, size_t size)
	{
		SHM_HDR_TYPE*  hdr = lock_memory();
		SHM_DATA_TYPE* ptr = reinterpret_cast<SHM_DATA_TYPE*>(
		                     reinterpret_cast<unsigned char*>(hdr + 1) + hdr->back_minus1);
		if( hdr->count && size == ptr->length ) {
			::memcpy(buff, ptr->data, size);
			unlock_memory();
			return true;
		} else {
			unlock_memory();
			assert(false);
			return false;
		}
	}

	template<typename T>
	bool push_back(const T & data)
	{
		return push_back(reinterpret_cast<const unsigned char*>(&data),
		                 sizeof(T));
	}

	bool push_back(const void * data, size_t size)
	{
		SHM_HDR_TYPE*  hdr = lock_memory();
		size_type new_back = static_cast<size_type>(hdr->back + sizeof(SHM_DATA_HDR_TYPE) + size);
		if( m_length <= new_back ) {
			new_back = 0;
		}
		if( (hdr->back + m_length - hdr->front) % m_length <
		    (new_back  + m_length - hdr->front) % m_length )
		{
			SHM_DATA_TYPE* ptr = reinterpret_cast<SHM_DATA_TYPE*>(
			                     reinterpret_cast<unsigned char*>(hdr + 1) + hdr->back);
			hdr->count++;
			hdr->back_minus1= hdr->back;
			hdr->back   = new_back;
			ptr->length = static_cast<size_type>(size);
			::memcpy(ptr->data, data, size);
			unlock_memory();
			return true;
		} else {
			unlock_memory();
			return false;
		}
	}

	void pop_back()
	{
		SHM_HDR_TYPE* hdr = lock_memory();
		if( hdr->count )
		{
			size_type new_back_minus1 = hdr->front;
			for(size_type seek = hdr->front;
				seek != hdr->back_minus1; )
			{
				SHM_DATA_TYPE* ptr = reinterpret_cast<SHM_DATA_TYPE*>(
				                     reinterpret_cast<unsigned char*>(hdr + 1) + seek);
				new_back_minus1 = seek;
				seek           += sizeof(SHM_DATA_HDR_TYPE) + ptr->length;
				if( m_length <= seek ) {
					seek = 0;
				}
			}
			hdr->back       = hdr->back_minus1;
			hdr->back_minus1= new_back_minus1;
			hdr->count--;
			unlock_memory();
		} else {
			unlock_memory();
			assert(false);
		}
	}

};

 } // namespace container
} // namespace spplib

#endif // !defined(SPPLIB_870D4F1A_5C18_474A_B0FA_C8518B239487_INCLUDE_GUARD)
