/**
 * @file  spplib/encode/base64.hpp
 * @brief BASE64デコーダ/エンコーダ実装
 * @see   http://tools.ietf.org/rfc/rfc3548.txt
 * @see   http://www5d.biglobe.ne.jp/~stssk/rfc/rfc3548j.html
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_4D5C3072_5DB8_45EA_A117_90D7033CE7D6_INCLUDE_GUARD
#define SPPLIB_4D5C3072_5DB8_45EA_A117_90D7033CE7D6_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// encode namespace
 namespace encode {
  /// base64 namespace
  namespace base64 {

template <size_t n = 0>
class size_of_encode {
	size_t value_;
public:
	size_of_encode(size_t len) : value_((len * 4 / 3 + 3) & ~3) {  }
	operator size_t () const { return value_; }
	enum { value = (n * 4 / 3 + 3) & ~3 };
};

inline
bool encoder(char * buff_, size_t * size, const void * data_, size_t len, bool padding = true)
{
	static const char TABLE[] = "ABCDEFGHIJKLMNOP"
	                            "QRSTUVWXYZabcdef"
	                            "ghijklmnopqrstuv"
	                            "wxyz0123456789+/=";

	char                * buff      = buff_;
	const unsigned char * data      = static_cast<const unsigned char*>(data_);
	const unsigned char * last      = data + len;
	const unsigned char * last_loop = last - 3; // ループ用の終端

	if( !size ||
		*size <= size_of_encode<>(len) )
	{
		return false;
	}

	*size -= size_of_encode<>(len) + 1;

	// BYTE  00000000  11111111  22222222
	// 6BIT  000000    1111      22      
	//             11      2222    333333
	// SRA     >> 2      >> 4      >> 6  
	// SLA     << 4      << 2      << 0  

	while( data <= last_loop )
	{
		*buff++ = TABLE[ (data[0] >> 2)];
		*buff++ = TABLE[((data[0] << 4) | (data[1] >> 4)) & 0x3F];
		*buff++ = TABLE[((data[1] << 2) | (data[2] >> 6)) & 0x3F];
		*buff++ = TABLE[  data[2]                         & 0x3F];
		data += 3;
	}

	// ビットの余り分を変換
	switch( (int)(last - data) )
	{
	case 2: {
		*buff++ = TABLE[ (data[0] >> 2)];
		*buff++ = TABLE[((data[0] << 4) | (data[1] >> 4)) & 0x3F];
		*buff++ = TABLE[((data[1] << 2)                 ) & 0x3F];
		break; }
	case 1: {
		*buff++ = TABLE[ (data[0] >> 2)];
		*buff++ = TABLE[((data[0] << 4)                 ) & 0x3F];
		break; }
	}

	if( padding )
	{
		// 変換後の文字列に余りがあれば補正
		switch( (((int)(buff - buff_) ^ 3) + 1) & 3 )
		{
		case 3: *buff++ = TABLE[64];
		case 2: *buff++ = TABLE[64];
		case 1: *buff++ = TABLE[64];
		}
	}

	*buff = 0;

	return true;
}

template <size_t n = 0>
class size_of_decode {
	size_t value_;
public:
	size_of_decode(size_t len) : value_(len * 3 / 4) {  }
	operator size_t () const { return value_; }
	enum { value = n * 3 / 4 };
};

inline
bool decoder(void * buff_, size_t * size, const char * data, size_t len)
{
	static const int TABLE[256] = {
			 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,62, 0, 0, 0,63,
			52,53,54,55,56,57,58,59,60,61, 0, 0, 0, 0, 0, 0,
			 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
			15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0, 0,
			 0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
			41,42,43,44,45,46,47,48,49,50,51, 0, 0, 0, 0, 0,
		};

	unsigned char * buff      = static_cast<unsigned char*>(buff_);
	const char    * last      = data + len;
	const char    * last_loop = last - 4; // ループ用の終端

	if( !size ||
		*size < size_of_decode<>(len) )
	{
		return false;
	}

	*size -= size_of_decode<>(len);

	// 6BIT  000000    1111      22      
	//             11      2222    333333
	// BYTE  00000000  11111111  22222222
	// SRA     << 2      << 4      << 6  
	// SLA     >> 4      >> 2      >> 0  

	while( data <= last_loop )
	{
		*buff++ = (unsigned char)((TABLE[data[0]] << 2)|(TABLE[data[1]] >> 4));
		*buff++ = (unsigned char)((TABLE[data[1]] << 4)|(TABLE[data[2]] >> 2));
		*buff++ = (unsigned char)((TABLE[data[2]] << 6)| TABLE[data[3]]      );
		data += 4;
	}

	// ビットの余り分を変換
	switch( (int)(last - data) )
	{
	case 3: {
		*buff++ = (unsigned char)((TABLE[data[0]] << 2)|(TABLE[data[1]] >> 4));
		*buff++ = (unsigned char)((TABLE[data[1]] << 4)|(TABLE[data[2]] >> 2));
		*buff++ = (unsigned char)((TABLE[data[2]] << 6)                      );
		break; }
	case 2: {
		*buff++ = (unsigned char)((TABLE[data[0]] << 2)|(TABLE[data[1]] >> 4));
		*buff++ = (unsigned char)((TABLE[data[1]] << 4)                      );
		break; }
	case 1: {
		*buff++ = (unsigned char) (TABLE[data[0]] << 2);
		break; }
	}

	return true;
}


inline
bool encoder(char * buff_, size_t size, const void * data_, size_t len, bool padding = true)
{
	return encoder(buff_, &size, data_, len, padding);
}

inline
bool decoder(void * buff_, size_t size, const char * data, size_t len)
{
	return decoder(buff_, &size, data, len);
}

  } // namespace base64
 } // namespace encode
} // namespace spplib

#endif // !defined(SPPLIB_4D5C3072_5DB8_45EA_A117_90D7033CE7D6_INCLUDE_GUARD)

//EOF
