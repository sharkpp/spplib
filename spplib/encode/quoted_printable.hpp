/**
 * @file  spplib/encode/quoted_printable.hpp
 * @brief Quoted-printableデコーダ/エンコーダ実装
 * @see   http://tools.ietf.org/html/rfc2045#section-6.7
 * @see   http://www.asahi-net.com/~bd9y-ktu/htmlrel_f/dtd_f/rfc_f/rfc2045j.html#s.67
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_B7C02774_8617_4D8C_A7FD_B3682E5495D9_INCLUDE_GUARD
#define SPPLIB_B7C02774_8617_4D8C_A7FD_B3682E5495D9_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// encode namespace
 namespace encode {
  /// quoted_printable namespace
  namespace quoted_printable {

inline
size_t size_of_encode(const void * , size_t len, const char *charset)
{
	return 0;
}

inline
bool encoder(char * buff, size_t * size, const void * data_, size_t len)
{
	static const char HEX[] = "0123456789ABCDEF";
	static const int TABLE[256] = {
			 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 3, 0, 0, // 00 - 0F
			 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 10 - 1F
			 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 20 - 2F
			 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, // 30 - 3F
			 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 40 - 4F
			 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 50 - 5F
			 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 60 - 6F
			 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // 70 - 7F
		};
	static const int TABLE2[256] = {
			 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 2, 0, 0, // 00 - 0F
			 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 10 - 1F
			 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 20 - 2F
			 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, // 30 - 3F
			 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 40 - 4F
			 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 50 - 5F
			 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 60 - 6F
			 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, // 70 - 7F
		};

	const unsigned char * data      = static_cast<const unsigned char*>(data_);
	const unsigned char * last      = data + len;
	const unsigned char * sp_bt_pos = data;
	
	int count = 0;

	for(; data != last; ++data)
	{
		count += 3 - TABLE2[*data];

		if( 75 < count ) {
			if( sp_bt_pos < data ) {
TRACE("*");
			}
			count = 3 - TABLE2[*data];
			*buff++ = '=';
//			*buff++ = '\r';
			*buff++ = '\n';
		}

		switch( TABLE[*data] )
		{
		case 0: // =XX形式に変換
			sp_bt_pos = data + 1;
			*buff++ = '=';
			*buff++ = HEX[*data >> 4];
			*buff++ = HEX[*data & 15];
			break;
		case 1: // そのまま
			sp_bt_pos = data + 1;
			*buff++ = (const char)*data;
			break;
		case 2: // TAB、空行
			*buff++ = (const char)*data;
			break;
		case 3: // 改行
			if( sp_bt_pos < data ) {
TRACE("#");
				count -= (int)(data - sp_bt_pos);
				buff  -= (int)(data - sp_bt_pos);
				for(; sp_bt_pos != data; ++sp_bt_pos)
				{
					count += 3;
					if( 75 < count ) {
						count = 3;
						*buff++ = '=';
			//			*buff++ = '\r';
						*buff++ = '\n';
					}
					*buff++ = '=';
					*buff++ = HEX[*sp_bt_pos >> 4];
					*buff++ = HEX[*sp_bt_pos & 15];
				}
				--data;
				continue;
			}
		//	count = 3 - TABLE2[*data];
			count = 0;
			sp_bt_pos = data + 1;
			*buff++ = (const char)*data;
		//	*buff++ = '=';
		//	*buff++ = HEX[*data >> 4];
		//	*buff++ = HEX[*data & 15];
			break;
		}
	}
	if( len ) {
		if( sp_bt_pos < data ) {
TRACE("+");
			count -= (int)(data - sp_bt_pos);
			buff  -= (int)(data - sp_bt_pos);
			for(; sp_bt_pos != data; ++sp_bt_pos)
			{
				count += 3;
				if( 75 < count ) {
					count = 3;
					*buff++ = '=';
		//			*buff++ = '\r';
					*buff++ = '\n';
				}
				*buff++ = '=';
				*buff++ = HEX[*sp_bt_pos >> 4];
				*buff++ = HEX[*sp_bt_pos & 15];
			}
		}
		if( 0 < count ) {
			*buff++ = '=';
	//		*buff++ = '\r';
			*buff++ = '\n';
		}
	}
	return true;
}

/*
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
*/

inline
bool encoder(char * buff_, size_t size, const void * data_, size_t len)
{
	return encoder(buff_, &size, data_, len);
}

  } // namespace quoted_printable
 } // namespace encode
} // namespace spplib

#endif // !defined(SPPLIB_B7C02774_8617_4D8C_A7FD_B3682E5495D9_INCLUDE_GUARD)

//EOF
