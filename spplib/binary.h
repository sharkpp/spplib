#pragma once // include guard for MSVC++

#ifndef INCLUDE_GUARD_SPPLIB_BINARY_H
#define INCLUDE_GUARD_SPPLIB_BINARY_H

namespace spplib
{

	class binary
	{
	public:

	protected:
		unsigned char *	m_pbyBuffer;
		unsigned long	m_dwAlloc;
		unsigned long	m_dwSize;
		bool	m_bAttach;

	public:
		// �R���X�g���N�^
		binary();
		explicit binary(unsigned long dwSize);
		binary(const unsigned char * pbyData, unsigned long dwSize);
		// �R�s�[�R���X�g���N�^
		binary(const binary & src);

		// �f�X�g���N�^
		virtual ~binary();

		// �������̊m��
		void alloc(unsigned long dwSize, const unsigned char * pbyInit = NULL, unsigned long dwInitSize = 0);

		// �������̊g��
		bool expand(unsigned long dwSize);

		// �f�[�^��ǉ�
		binary & append(const unsigned char * pbyData, unsigned long dwSize);

		binary & append( const binary & src )
			{ return append(src.m_pbyBuffer, src.m_dwSize); }

		// �f�[�^����������
		unsigned long write(const binary & src, unsigned long dwOffset = 0)
			{ return write(src.m_pbyBuffer, src.m_dwSize, dwOffset); }

		unsigned long write(const unsigned char * pbyData, unsigned long dwSize, unsigned long dwOffset = 0);

		// �f�[�^��ǂݍ���
		binary read(unsigned long dwSize, unsigned long dwOffset = 0);

		// �w��͈͂Ƀf�[�^��}������
		void insert(const unsigned char * pbyData, unsigned long dwSize, unsigned long dwOffset = 0);

		// �w��͈͂̃f�[�^���폜���ċl�߂�
		unsigned long remove(unsigned long dwSize, unsigned long dwOffset = 0);

		// �N���A
		void clear()
			{ m_dwSize = 0; }

		// �L���ȃf�[�^�T�C�Y��Ԃ�
		unsigned long size() const
			{ return m_dwSize; }

		// �f�[�^�̐擪�|�C���^��Ԃ�
		unsigned char * get() const
			{ return m_pbyBuffer; }

		// unsigned char * �L���X�g
		operator unsigned char *() const
			{ return get(); }

		// += �I�y���[�^
		binary & operator += ( const binary & src )
			{ return append(src.get(), src.size()); }

		// = �I�y���[�^
		binary & operator = ( const binary & src )
			{ clear(); return append(src.m_pbyBuffer, src.m_dwSize); }

	protected:
		// �������̊J��
		void free();

	};

} // namespace spplib

#endif // !defined(INCLUDE_GUARD_SPPLIB_BINARY_H)

//EOF
