/**
 * @file  spplib/container/simple_array.hpp
 * @brief �V���v���ȓ��I�z��N���X�e���v���[�g��`
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
 * @brief �V���v���ȓ��I�z��N���X�e���v���[�g
 * @tparam T �z��̗v�f�̌^
 * @code
 *  int a[] = { 1, 2, 3 };
 *  spplib::container::simple_array<int> v = a;
 *  for(spplib::container::simple_array<int>::iterator
 *          ite = v.begin(), last= v.end();
 *      ite != last; ++ite)
 *  {
 *    printf("%d,", *ite);
 *  }
 *  // 1,2,3, �ƕ\�������
 * @endcode
 */
template <typename T>
class simple_array
{
public:

	typedef simple_array<T>		My;					///< �e���v���[�g�C���X�^���X�̌^
	typedef T					value_type;			///< �z��v�f�^
	typedef T*					pointer;			///< �z��v�f�^�̃|�C���^
	typedef T*					iterator;			///< �C�e���[�^
	typedef T&					reference;			///< �z��v�f�^�̎Q�ƌ^
	typedef const T*			const_pointer;		///< �z��v�f�^��const �|�C���^
	typedef const T*			const_iterator;		///< const �C�e���[�^
	typedef const T&			const_reference;	///< �z��v�f�^��const �Q�ƌ^

private:

	pointer	m_data;		// �z��v�f�̃f�[�^
	int		m_length;	// �z��v�f��
	int		m_capacity;	// �z��v�f�m�ې�

public:

	//-----------------------------------------------------
	// �R���X�g���N�V�����E�f�X�g���N�V����
	//-----------------------------------------------------

	/// �R���X�g���N�^
	simple_array()
		: m_data(0)
		, m_length(0)
		, m_capacity(0)
	{
	}

	/**
	 * @brief �R���X�g���N�^
	 * @param[in] src �R�s�[���f�[�^
	 */
	simple_array(const My & src)
		: m_data(0)
		, m_length(0)
		, m_capacity(0)
	{
		copy(src.m_data, src.m_length);
	}

	/**
	 * @brief �R���X�g���N�^
	 * @param[in] first �R�s�[���f�[�^�̐擪
	 * @param[in] last  �R�s�[���f�[�^�̖���+1
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
	 * @brief �R���X�g���N�^
	 * @param[in] src  �R�s�[���z��
	 * @tparam    size �R�s�[���z��̗v�f��
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
	 * @brief �R���X�g���N�^
	 * @param[in] src  �R�s�[���z��
	 * @tparam    size �R�s�[���z��̗v�f��
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
	 * @brief �R���X�g���N�^
	 * @param[in] src  �R�s�[���|�C���^
	 * @param[in] size �R�s�[���̗v�f��
	 */
	simple_array(const_pointer src, int size)
		: m_data(0)
		, m_length(0)
		, m_capacity(0)
	{
		copy(src, size);
	}

	/// �f�X�g���N�^
	~simple_array()
	{
		delete [] m_data;
	}

	//-----------------------------------------------------
	// ���\�b�h
	//-----------------------------------------------------

	/**
	 * @brief �z�񂪋󂩂𒲂ׂ�
	 * @retval true  �z��͋�
	 * @retval false �z��͊���̗v�f�������Ă���
	 */
	bool empty() const
	{
		return 0 == m_length;
	}

	/**
	 * @brief �z��̗v�f�����擾
	 * @return �z��̗v�f��
	 */
	size_t size() const
	{
		return m_length;
	}

	/**
	 * @brief �z��̗v�f����ɂ���
	 * @note �������͊J�������킯�ł͂Ȃ��̂Œ���
	 */
	void clear()
	{
		m_length = 0;
	}

	/**
	 * @brief �z��̊g��
	 * @param[in] count �v�f��
	 * @note �g���͂���邪�k���͂���Ȃ��̂Œ���
	 */
	void reserve(int count)
	{
		if( m_capacity < count ) {
			pointer data= m_data;
			m_capacity  = count + count / 2;
			m_data      = new T [m_capacity + 1/*�ԕ�*/];
			copy(data, m_length);	// ���̃f�[�^���R�s�[
			delete [] data;
		}
	}

	/**
	 * @brief �z��v�f�����w��
	 * @param[in] newsize �v�f��
	 * @param[in] val     ���݂̗v�f�����瑝�������Ɏg�p����鏉�����f�[�^
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
	 * @brief �w�肵�����e�ƒ��g�����ւ�
	 * @param[in,out] src ����ւ���v�f
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
	 * @brief �擪�C�e���[�^���擾
	 * @return �擪�C�e���[�^
	 */
	iterator begin()
	{
		return m_data;
	}

	/**
	 * @brief �擪�C�e���[�^���擾
	 * @return �擪�C�e���[�^
	 */
	const_iterator begin() const
	{
		return m_data;
	}

	/**
	 * @brief �����C�e���[�^���擾
	 * @return �����C�e���[�^
	 */
	iterator end()
	{
		return m_data + m_length;
	}

	/**
	 * @brief �����C�e���[�^���擾
	 * @return �����C�e���[�^
	 */
	const_iterator end() const
	{
		return m_data + m_length;
	}

	//-----------------------------------------------------
	// operator ���\�b�h
	//-----------------------------------------------------

	/**
	 * @brief �z��̗v�f���擾
	 * @param[in] index �Y����
	 * @return �z��̗v�f
	 */
	reference operator [] (int index)
	{
		assert( 0 <= index && index < m_length );
		return m_data[index];
	}

	/**
	 * @brief �z��̗v�f���擾
	 * @param[in] index �Y����
	 * @return �z��̗v�f
	 */
	const_reference operator [] (int index) const
	{
		assert( 0 <= index && index < m_length );
		return m_data[index];
	}

	/**
	 * @brief T*�ɃL���X�g
	 * @return T*
	 */
	operator pointer ()
	{
		return begin();
	}

	/**
	 * @brief T*�ɃL���X�g
	 * @return T*
	 */
	operator const_pointer () const
	{
		return begin();
	}

	/**
	 * @brief �w��z��Ƃ̍����z���Ԃ�
	 * @param[in] src ��������z��
	 * @return �������ꂽ�z��
	 */
	My operator + (const My & src)
	{
		My r = *this;
		return ( r += src );
	}

	/**
	 * @brief �w��z���������
	 * @param[in] src �R�s�[���̔z��
	 * @return ������̎��g�̃C���X�^���X
	 */
	My & operator = (const My & src)
	{
		copy(src.m_data, src.m_length);
		return *this;
	}

	/**
	 * @brief �w��z���������
	 * @param[in] src  �R�s�[���̔z��
	 * @tparam    size �R�s�[���̔z��̗v�f��
	 * @return ������̎��g�̃C���X�^���X
	 */
	template<int size>
	My & operator = (T (&src)[size])
	{
		copy(src, size);
		return *this;
	}

	/**
	 * @brief �w��z���������
	 * @param[in] src  �R�s�[���̔z��
	 * @tparam    size �R�s�[���̔z��̗v�f��
	 * @return ������̎��g�̃C���X�^���X
	 */
	template<int size>
	My & operator = (T const (&src)[size])
	{
		copy(src, size);
		return *this;
	}

	/**
	 * @brief �w��v�f�ƍ�������
	 * @param[in] src ��������v�f
	 * @return ������̎��g�̃C���X�^���X
	 */
	My & operator += (const T & src)
	{
		copy(&src, 1, m_length);
		return *this;
	}

	/**
	 * @brief �w��z��ƍ�������
	 * @param[in] src ��������z��
	 * @return ������̎��g�̃C���X�^���X
	 */
	My & operator += (const My & src)
	{
		copy(src.m_data, src.m_length, m_length);
		return *this;
	}

private:

	/**
	 * @brief �z��f�[�^�̃R�s�[
	 * @param[in]  src    �R�s�[���̔z��v�f
	 * @param[in]  length �R�s�[���̔z��v�f��
	 * @param[in]  offset �R�s�[��̃I�t�Z�b�g�A�����l�� 0
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
	 * @brief �z��v�f�̃R�s�[
	 * @param[out] dest �R�s�[��̔z��v�f
	 * @param[in]  src  �R�s�[���̔z��v�f
	 * @return ������̎��g�̃C���X�^���X
	 * @note ����s�̃G���[(C2440)�����������ꍇ�̓e���v���[�g�̕������ꉻ���s���Ă�������
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
