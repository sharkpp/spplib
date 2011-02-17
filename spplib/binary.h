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
		// コンストラクタ
		binary();
		explicit binary(unsigned long dwSize);
		binary(const unsigned char * pbyData, unsigned long dwSize);
		// コピーコンストラクタ
		binary(const binary & src);

		// デストラクタ
		virtual ~binary();

		// メモリの確保
		void alloc(unsigned long dwSize, const unsigned char * pbyInit = NULL, unsigned long dwInitSize = 0);

		// メモリの拡張
		bool expand(unsigned long dwSize);

		// データを追加
		binary & append(const unsigned char * pbyData, unsigned long dwSize);

		binary & append( const binary & src )
			{ return append(src.m_pbyBuffer, src.m_dwSize); }

		// データを書き込む
		unsigned long write(const binary & src, unsigned long dwOffset = 0)
			{ return write(src.m_pbyBuffer, src.m_dwSize, dwOffset); }

		unsigned long write(const unsigned char * pbyData, unsigned long dwSize, unsigned long dwOffset = 0);

		// データを読み込む
		binary read(unsigned long dwSize, unsigned long dwOffset = 0);

		// 指定範囲にデータを挿入する
		void insert(const unsigned char * pbyData, unsigned long dwSize, unsigned long dwOffset = 0);

		// 指定範囲のデータを削除して詰める
		unsigned long remove(unsigned long dwSize, unsigned long dwOffset = 0);

		// クリア
		void clear()
			{ m_dwSize = 0; }

		// 有効なデータサイズを返す
		unsigned long size() const
			{ return m_dwSize; }

		// データの先頭ポインタを返す
		unsigned char * get() const
			{ return m_pbyBuffer; }

		// unsigned char * キャスト
		operator unsigned char *() const
			{ return get(); }

		// += オペレータ
		binary & operator += ( const binary & src )
			{ return append(src.get(), src.size()); }

		// = オペレータ
		binary & operator = ( const binary & src )
			{ clear(); return append(src.m_pbyBuffer, src.m_dwSize); }

	protected:
		// メモリの開放
		void free();

	};

} // namespace spplib

#endif // !defined(INCLUDE_GUARD_SPPLIB_BINARY_H)

//EOF
