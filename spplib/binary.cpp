#include <windows.h>
#include "binary.h"

namespace spplib
{

// コンストラクタ
binary::binary()
	: m_pbyBuffer(NULL)
	, m_dwSize(0)
	, m_dwAlloc(0)
	, m_bAttach(false)
{
}

// コンストラクタ
binary::binary(unsigned long dwSize)
	: m_pbyBuffer(NULL)
	, m_dwSize(0)
	, m_dwAlloc(0)
	, m_bAttach(false)
{
	alloc(dwSize);
}

// コンストラクタ
binary::binary(const unsigned char * pbyData, unsigned long dwSize)
	: m_pbyBuffer(NULL)
	, m_dwSize(0)
	, m_dwAlloc(0)
	, m_bAttach(false)
{
	append(pbyData, dwSize);
}

// コピーコンストラクタ
binary::binary(const binary & src)
	: m_pbyBuffer(NULL)
	, m_dwSize(0)
	, m_dwAlloc(0)
	, m_bAttach(false)
{
	append(src.m_pbyBuffer, src.m_dwSize);
}

// デストラクタ
binary::~binary()
{
	free();
}

// メモリの確保
void binary::alloc(unsigned long dwSize, const unsigned char * pbyInit /* = NULL */, unsigned long dwInitSize /* = 0 */)
{
	free();

	m_dwAlloc	= dwSize;
	m_dwSize	= dwSize;

	if( !dwSize ) {
		return;
	}

	m_pbyBuffer	= new BYTE[m_dwAlloc];

	if( !m_pbyBuffer ) {
		return;
	}

	::memset(m_pbyBuffer, 0, m_dwSize);
	// 初期データの指定
	if( pbyInit ) {
		dwInitSize = !dwInitSize ? dwSize : dwInitSize;
		dwInitSize = min(dwInitSize, dwSize);
		::memcpy(m_pbyBuffer, pbyInit, dwInitSize);
	}
}

// メモリの開放
void binary::free()
{
	if( m_pbyBuffer && m_bAttach ) {
		delete [] m_pbyBuffer;
	}
	m_bAttach	= false;
	m_pbyBuffer	= NULL;
	m_dwSize	= 0;
	m_dwAlloc	= 0;
}

// メモリの拡張
bool binary::expand(unsigned long dwSize)
{
	unsigned char * pbyBuff = NULL;

	if( m_bAttach ) {
		return false;
	}

	if(dwSize < m_dwAlloc) {
		m_dwSize = dwSize;
		return true;
	}

	pbyBuff	= new BYTE[dwSize];

	if( !pbyBuff ) {
		return false;
	}

	::memset(pbyBuff, 0, dwSize);

	if( m_pbyBuffer ) {
		::memcpy(pbyBuff, m_pbyBuffer, m_dwSize);
		delete [] m_pbyBuffer;
	}

	m_pbyBuffer	= pbyBuff;
	m_dwAlloc	= dwSize;
	m_dwSize	= dwSize;

	return true;
}

// データを追加
binary & binary::append(const unsigned char * pbyData, unsigned long dwSize)
{
	if( !pbyData || !dwSize ) {
		return *this;
	}

	if( m_dwAlloc < m_dwSize + dwSize )
	{
		unsigned char * pbyOldBuff = m_pbyBuffer;
		m_dwAlloc = m_dwSize + dwSize + 1024;
		m_pbyBuffer = new BYTE[m_dwAlloc];
		if( pbyOldBuff && m_dwSize ) {
			::memcpy(m_pbyBuffer, pbyOldBuff, m_dwSize);
			delete [] pbyOldBuff;
		}
	}

	if( m_pbyBuffer ) {
		::memcpy(m_pbyBuffer + m_dwSize, pbyData, dwSize);
		m_dwSize += dwSize;
	}

	return *this;
}

// データを書き込む
unsigned long binary::write(const unsigned char * pbyData, unsigned long dwSize, unsigned long dwOffset /* = 0 */)
{
	if( !pbyData ||
		m_dwSize <= dwOffset + dwSize
		)
	{
		return 0;
	}

	::memcpy(m_pbyBuffer + dwOffset, pbyData, dwSize);

	return dwSize;
}

// データを読み込む
binary binary::read(unsigned long dwSize, unsigned long dwOffset /* = 0 */)
{
	binary buff;

	if( m_dwSize <= dwOffset )
	{
		return buff;
	}

	buff.append( m_pbyBuffer + dwOffset, min(dwSize, m_dwSize - dwOffset) );

	return buff;
}

// 指定範囲にデータを挿入する
void binary::insert(const unsigned char * /*pbyData*/, unsigned long /*dwSize*/, unsigned long /*dwOffset*/ /* = 0 */)
{
	// 未実装
	throw;
}

// 指定範囲のデータを削除して詰める
unsigned long binary::remove(unsigned long dwSize, unsigned long dwOffset /* = 0 */)
{
	if( m_dwSize <= dwOffset )
	{
		return 0;
	}

	unsigned long dwRemoveLen;

	dwRemoveLen = (m_dwSize < dwOffset + dwSize ? 0 : dwSize);

	::memcpy(
		  m_pbyBuffer + dwOffset
		, m_pbyBuffer + dwOffset + dwRemoveLen
		, m_dwSize    - dwOffset - dwRemoveLen 
		);
	m_dwSize -= dwRemoveLen;

	return dwRemoveLen;
}

} // namespace spplib

//EOF
