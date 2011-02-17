/**
 * @file  spplib/container/simple_array.hpp
 * @brief シンプルな動的配列クラステンプレート定義
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2009-2010.
 */

#include <assert.h>

#ifndef SPPLIB_338D3AAA_1724_4222_807F_D99A50154402_INCLUDE_GUARD
#define SPPLIB_338D3AAA_1724_4222_807F_D99A50154402_INCLUDE_GUARD

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// containers namespace
 namespace container {

/**
 * @brief シンプルな動的配列クラステンプレート
 * @tparam T 配列の要素の型
 * @code
 *  int a[] = { 1, 2, 3 };
 *  spplib::container::simple_array<int> v = a;
 *  for(spplib::container::simple_array<int>::iterator
 *          ite = v.begin(), last= v.end();
 *      ite != last; ++ite)
 *  {
 *    printf("%d,", *ite);
 *  }
 *  // 1,2,3, と表示される
 * @endcode
 */
template <typename T>
class simple_array
{
public:

	typedef simple_array<T>		My;					///< テンプレートインスタンスの型
	typedef T					value_type;			///< 配列要素型
	typedef T*					pointer;			///< 配列要素型のポインタ
	typedef T*					iterator;			///< イテレータ
	typedef T&					reference;			///< 配列要素型の参照型
	typedef const T*			const_pointer;		///< 配列要素型のconst ポインタ
	typedef const T*			const_iterator;		///< const イテレータ
	typedef const T&			const_reference;	///< 配列要素型のconst 参照型

private:

	pointer	m_data;		// 配列要素のデータ
	int		m_length;	// 配列要素長
	int		m_capacity;	// 配列要素確保数

public:

	//-----------------------------------------------------
	// コンストラクション・デストラクション
	//-----------------------------------------------------

	/// コンストラクタ
	simple_array()
		: m_data(0)
		, m_length(0)
		, m_capacity(0)
	{
	}

	/**
	 * @brief コンストラクタ
	 * @param[in] src コピー元データ
	 */
	simple_array(const My & src)
		: m_data(0)
		, m_length(0)
		, m_capacity(0)
	{
		copy(src.m_data, src.m_length);
	}

	/**
	 * @brief コンストラクタ
	 * @param[in] first コピー元データの先頭
	 * @param[in] last  コピー元データの末尾+1
	 * @code
	 *  int a[] = { 1, 2, 3 };
	 *  spplib::simple_array<int> b(a, a + 3);
	 * @endcode
	 */
	simple_array(const_iterator first, const_iterator last)
		: m_data(0)
		, m_length(0)
		, m_capacity(0)
	{
		copy(first, last - first);
	}

	/**
	 * @brief コンストラクタ
	 * @param[in] src  コピー元配列
	 * @tparam    size コピー元配列の要素数
	 */
	template<int size>
	simple_array(T (&src)[size])
		: m_data(0)
		, m_length(0)
		, m_capacity(0)
	{
		copy(src, size);
	}

	/**
	 * @brief コンストラクタ
	 * @param[in] src  コピー元配列
	 * @tparam    size コピー元配列の要素数
	 */
	template<int size>
	simple_array(T const (&src)[size])
		: m_data(0)
		, m_length(0)
		, m_capacity(0)
	{
		copy(src, size);
	}

	/**
	 * @brief コンストラクタ
	 * @param[in] src  コピー元ポインタ
	 * @param[in] size コピー元の要素数
	 */
	simple_array(const_pointer src, int size)
		: m_data(0)
		, m_length(0)
		, m_capacity(0)
	{
		copy(src, size);
	}

	/// デストラクタ
	~simple_array()
	{
		delete [] m_data;
	}

	//-----------------------------------------------------
	// メソッド
	//-----------------------------------------------------

	/**
	 * @brief 配列が空かを調べる
	 * @retval true  配列は空
	 * @retval false 配列は幾つかの要素を持っている
	 */
	bool empty() const
	{
		return 0 == m_length;
	}

	/**
	 * @brief 配列の要素数を取得
	 * @return 配列の要素数
	 */
	size_t size() const
	{
		return m_length;
	}

	/**
	 * @brief 配列の要素を空にする
	 * @note メモリは開放されるわけではないので注意
	 */
	void clear()
	{
		m_length = 0;
	}

	/**
	 * @brief 配列の拡張
	 * @param[in] count 要素数
	 * @note 拡張はされるが縮小はされないので注意
	 */
	void reserve(int count)
	{
		if( m_capacity < count ) {
			pointer data= m_data;
			m_capacity  = count + count / 2;
			m_data      = new T [m_capacity + 1/*番兵*/];
			copy(data, m_length);	// 元のデータをコピー
			delete [] data;
		}
	}

	/**
	 * @brief 配列要素数を指定
	 * @param[in] newsize 要素数
	 * @param[in] val     現在の要素数から増えた時に使用される初期化データ
	 */
	void resize(int newsize, const_reference val = T())
	{
		reserve(newsize);
		for(int i = m_length; i < newsize; i++)
		{
			m_data[i] = val;
		}
		m_length = newsize;
	}

	/**
	 * @brief 指定した内容と中身を入れ替え
	 * @param[in,out] src 入れ替える要素
	 * @code
	 *  spplib::simple_array<int> foo;
	 *  foo.resize(1024);
	 *  spplib::simple_array<int>().swap(foo);
	 * @endcode
	 */
	void swap(My & src)
	{
		pointer	data    = src.m_data;
		int		length  = src.m_length;
		int		capacity= src.m_capacity;
		src.m_data    = m_data;
		src.m_length  = m_length;
		src.m_capacity= m_capacity;
		m_data    = data;
		m_length  = length;
		m_capacity= capacity;
	}


	/**
	 * @brief 先頭イテレータを取得
	 * @return 先頭イテレータ
	 */
	iterator begin()
	{
		return m_data;
	}

	/**
	 * @brief 先頭イテレータを取得
	 * @return 先頭イテレータ
	 */
	const_iterator begin() const
	{
		return m_data;
	}

	/**
	 * @brief 末尾イテレータを取得
	 * @return 末尾イテレータ
	 */
	iterator end()
	{
		return m_data + m_length;
	}

	/**
	 * @brief 末尾イテレータを取得
	 * @return 末尾イテレータ
	 */
	const_iterator end() const
	{
		return m_data + m_length;
	}

	//-----------------------------------------------------
	// operator メソッド
	//-----------------------------------------------------

	/**
	 * @brief 配列の要素を取得
	 * @param[in] index 添え字
	 * @return 配列の要素
	 */
	reference operator [] (int index)
	{
		assert( 0 <= index && index < m_length );
		return m_data[index];
	}

	/**
	 * @brief 配列の要素を取得
	 * @param[in] index 添え字
	 * @return 配列の要素
	 */
	const_reference operator [] (int index) const
	{
		assert( 0 <= index && index < m_length );
		return m_data[index];
	}

	/**
	 * @brief T*にキャスト
	 * @return T*
	 */
	operator pointer ()
	{
		return begin();
	}

	/**
	 * @brief T*にキャスト
	 * @return T*
	 */
	operator const_pointer () const
	{
		return begin();
	}

	/**
	 * @brief 指定配列との合成配列を返す
	 * @param[in] src 合成する配列
	 * @return 生成された配列
	 */
	My operator + (const My & src)
	{
		My r = *this;
		return ( r += src );
	}

	/**
	 * @brief 指定配列を代入する
	 * @param[in] src コピー元の配列
	 * @return 合成後の自身のインスタンス
	 */
	My & operator = (const My & src)
	{
		copy(src.m_data, src.m_length);
		return *this;
	}

	/**
	 * @brief 指定配列を代入する
	 * @param[in] src  コピー元の配列
	 * @tparam    size コピー元の配列の要素数
	 * @return 合成後の自身のインスタンス
	 */
	template<int size>
	My & operator = (T (&src)[size])
	{
		copy(src, size);
		return *this;
	}

	/**
	 * @brief 指定配列を代入する
	 * @param[in] src  コピー元の配列
	 * @tparam    size コピー元の配列の要素数
	 * @return 合成後の自身のインスタンス
	 */
	template<int size>
	My & operator = (T const (&src)[size])
	{
		copy(src, size);
		return *this;
	}

	/**
	 * @brief 指定要素と合成する
	 * @param[in] src 合成する要素
	 * @return 合成後の自身のインスタンス
	 */
	My & operator += (const T & src)
	{
		copy(&src, 1, m_length);
		return *this;
	}

	/**
	 * @brief 指定配列と合成する
	 * @param[in] src 合成する配列
	 * @return 合成後の自身のインスタンス
	 */
	My & operator += (const My & src)
	{
		copy(src.m_data, src.m_length, m_length);
		return *this;
	}

private:

	/**
	 * @brief 配列データのコピー
	 * @param[in]  src    コピー元の配列要素
	 * @param[in]  length コピー元の配列要素数
	 * @param[in]  offset コピー先のオフセット、初期値は 0
	 */
	virtual void copy(const_pointer src, int length, int offset = 0)
	{
		reserve(offset + length);
		m_length = offset + length;
		for(int i = 0; i < length; i++) {
			copy(m_data + offset + i, src + i);
		}
	}

	/**
	 * @brief 配列要素のコピー
	 * @param[out] dest コピー先の配列要素
	 * @param[in]  src  コピー元の配列要素
	 * @return 合成後の自身のインスタンス
	 * @note 代入不可のエラー(C2440)が発生した場合はテンプレートの部分特殊化を行ってください
	 * @code
	 *   typedef spplib::simple_array<TCHAR[MAX_PATH+1]> PATH_ARRAY;
	 *   void PATH_ARRAY::copy(pointer dest, const_pointer src)
	 *   {
	 *     ::memcpy(dest, src, sizeof(*dest));
	 *   }
	 *   PATH_ARRAY array;
	 * @endcode
	 */
	virtual void copy(pointer dest, const_pointer src)
	{
		*dest = *src;
	}

};

 } // namespace container
} // namespace spplib

//////////////////////////////////////////////////////////////////////

#endif // !defined(SPPLIB_338D3AAA_1724_4222_807F_D99A50154402_INCLUDE_GUARD)
