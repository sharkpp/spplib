/**
 * @file  spplib/encode/mime.hpp
 * @brief MIMEデコーダ/エンコーダ実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#if defined(SPPLIB_ENCODE_MIME_USE_BABEL)
#include <babel/babel.h>
#endif

#include <spplib/encode/base64.hpp>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_599E6331_B238_457D_882B_07D1A72CD861_INCLUDE_GUARD
#define SPPLIB_599E6331_B238_457D_882B_07D1A72CD861_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {
 /// encode namespace
 namespace encode {
  /// mime namespace
  namespace mime {

typedef enum {
	ENCODE_QUOTED_PRINTABLE,
	ENCODE_BASE64,
} ENCODE_TYPE;

inline
size_t size_of_encode(const void * , size_t len, const char *charset, ENCODE_TYPE encoding)
{
	size_t charset_len = (size_t)((const char*)::memchr(charset, 0, (size_t)-1) - charset);
	return 2 + charset_len + 1 + 2 + base64::size_of_encode<>(len) + 2;
}

inline
bool encoder(char * buff_, size_t * size, const void * data_, size_t len,
             const char *charset, ENCODE_TYPE encoding)
{
	const unsigned char * data = static_cast<const unsigned char *>(data_);
	char *                buff = buff_;
	size_t write_len = 0;

	// RFC2047
	//  encoded-word = "=?" charset "?" encoding "?" encoded-text "?="
	//  charset      = token  ; see section 3
	//  encoding     = token  ; see section 4
	//  token        = 1*<Any CHAR except SPACE, CTLs, and especials>
	//  especials    = "(" / ")" / "<" / ">" / "@" / "," / ";" / ":" / "
	//                 <"> / "/" / "[" / "]" / "?" / "." / "="
	//  encoded-text = 1*<Any printable ASCII character other than "?"
	//                    or SPACE>
	//                 ; (but see "Use of encoded-words in message
	//                 ; headers", section 5)

	*buff++ = '='; write_len++;
	*buff++ = '?'; write_len++;

	// charset
	for(; *charset; write_len++) {
		*buff++ = *charset++;
	}
	*buff++ = '?'; write_len++;

	switch( encoding ) {
///	case ENCODE_QUOTED_PRINTABLE:
//		break;
	case ENCODE_BASE64: {
		// encoding
		*buff++ = 'B'; write_len++;
		*buff++ = '?'; write_len++;
		// encoded-text
		size_t rest = *size - write_len;
		if( !base64::encoder(buff, &rest, data_, len) ) {
			return false;
		}
		buff      += *size - write_len - rest - 1; // ※'\0'の分 -1 する
		write_len += *size - write_len - rest - 1;
		break; }
	}

	*buff++ = '?'; write_len++;
	*buff++ = '='; write_len++;

	return true;
}

//template <size_t n = 0>
//class size_of_decode {
//	size_t value_;
//public:
//	size_of_decode(size_t len) : value_(len * 3 / 4) {  }
//	operator size_t () const { return value_; }
//	enum { value = n * 3 / 4 };
//};

inline
int decoder(void * buff_, size_t size, const char * data, size_t len)
{
	return true;
}


inline
bool encoder(char * buff_, size_t size, const void * data_, size_t len,
             const char *charset, ENCODE_TYPE encoding)
{
	return encoder(buff_, &size, data_, len, charset, encoding);
}

  } // namespace mime
 } // namespace encode
} // namespace spplib

#endif // !defined(SPPLIB_599E6331_B238_457D_882B_07D1A72CD861_INCLUDE_GUARD)

//EOF
