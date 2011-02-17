// simple_array.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <vector>

#define SPPLIB_LOGGER_ENABLE_INLINE
#include <spplib/logger.hpp>
#include <spplib/tester.hpp>

#include <spplib/encode/base64.hpp>
#include <spplib/encode/quoted_printable.hpp>

//#define SPPLIB_ENCODE_MIME_USE_BABEL
#include <spplib/encode/mime.hpp>

using namespace spplib;

	// http://search.cpan.org/~gaas/MIME-Base64-3.09/QuotedPrint.pm
#define x70 "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const static char *QP_TEST_DATA[][2] = {
		// plain ascii should not be encoded
		{ "\0", "\0" },
		{ "quoted printable", "quoted printable=\n" },
		// 8-bit chars should be encoded
		{ "v\xe5re kj\xe6re norske tegn b\xf8r \xe6res",
		  "v=E5re kj=E6re norske tegn b=F8r =E6res=\n" },
		// trailing space should be encoded
		{ "  ", "=20=20=\n" },
		{ "\tt\t", "\tt=09=\n" },
		{ "test  \ntest\n\t \t \n", "test=20=20\ntest\n=09=20=09=20\n" },
		// "=" is special an should be decoded
		{ "=30\n", "=3D30\n" },
		{ "\0\xff""0", "=00=FF0=\n" },
		// Very long lines should be broken (not more than 76 chars
		{ "The Quoted-Printable encoding is intended to represent data that largly consists of octets that correspond to printable characters in the ASCII character set.",
		  "The Quoted-Printable encoding is intended to represent data that largly con=\nsists of octets that correspond to printable characters in the ASCII charac=\nter set.=\n" },
		// Long lines after short lines were broken through 2.01.
		{ "short line\nIn America, any boy may become president and I suppose that's just one of the risks he takes. -- Adlai Stevenson",
		  "short line\nIn America, any boy may become president and I suppose that's just one of t=\nhe risks he takes. -- Adlai Stevenson=\n" },
		// My (roderick@argon.org) first crack at fixing that bug failed for
		// multiple long lines.
		{ "College football is a game which would be much more interesting if the faculty played instead of the students, and even more interesting if the\n"
		  "trustees played.  There would be a great increase in broken arms, legs, and necks, and simultaneously an appreciable diminution in the loss to humanity. -- H. L. Mencken",
		  "College football is a game which would be much more interesting if the facu=\n"
		  "lty played instead of the students, and even more interesting if the\n"
		  "trustees played.  There would be a great increase in broken arms, legs, and=\n"
		  "necks, and simultaneously an appreciable diminution in the loss to humanit=\n"
		  "y. -- H. L. Mencken=\n" },
		// Don't break a line that's near but not over 76 chars.
		{ x70 "!23"			, x70 "!23=\n" },
		{ x70 "!234"		, x70 "!234=\n" },
		{ x70 "!2345"		, x70 "!2345=\n" },
		{ x70 "!23456"		, x70 "!2345=\n6=\n" },
		{ x70 "!234567"		, x70 "!2345=\n67=\n" },
		{ x70 "!23456="		, x70 "!2345=\n6=3D=\n" },
		{ x70 "!23\n"		, x70 "!23\n" },
		{ x70 "!234\n"		, x70 "!234\n" },
		{ x70 "!2345\n"		, x70 "!2345\n" },
		{ x70 "!23456\n"	, x70 "!23456\n" },
		{ x70 "!234567\n"	, x70 "!2345=\n67\n" },
		{ x70 "!23456=\n"	, x70 "!2345=\n6=3D\n" },
		// Not allowed to break =XX escapes using soft line break
		{ x70 "===xxxxx" , x70 "=3D=\n=3D=3Dxxxxx=\n" },
		{ x70 "!===xxxx" , x70 "!=3D=\n=3D=3Dxxxx=\n" },
		{ x70 "!2===xxx" , x70 "!2=3D=\n=3D=3Dxxx=\n" },
		{ x70 "!23===xx" , x70 "!23=\n=3D=3D=3Dxx=\n" },
		{ x70 "!234===x" , x70 "!234=\n=3D=3D=3Dx=\n" },
		{ x70 "!2="      , x70 "!2=3D=\n" },
		{ x70 "!23="     , x70 "!23=\n=3D=\n" },
		{ x70 "!234="    , x70 "!234=\n=3D=\n" },
		{ x70 "!2345="   , x70 "!2345=\n=3D=\n" },
		{ x70 "!23456="  , x70 "!2345=\n6=3D=\n" },
		{ x70 "!2=\n"    , x70 "!2=3D\n" },
		{ x70 "!23=\n"   , x70 "!23=3D\n" },
		{ x70 "!234=\n"  , x70 "!234=\n=3D\n" },
		{ x70 "!2345=\n" , x70 "!2345=\n=3D\n" },
		{ x70 "!23456=\n", x70 "!2345=\n6=3D\n" },
		//                            ^
		//                    70123456|
		//                           max
		//                        line width

		// some extra special cases we have had problems with
		{ x70 "!2=x=x", x70 "!2=3D=\nx=3Dx=\n" },
		{ x70 "!2345$x70!2345$x70!23456\n", x70 "!2345=\n$x70!2345=\n$x70!23456\n" },
		// trailing whitespace
		{ "foo \t ", "foo=20=09=20=\n" },
		{ "foo\t \n \t", "foo=09=20\n=20=09=\n" },
	};

class qp_unit_test
	: public spplib::tester::base<qp_unit_test>
{
private:

	size_t m_index;
	spplib::tstring	m_last_message;

	bool EncodeCaseInit()
	{
		return m_index < sizeof(QP_TEST_DATA)/sizeof(QP_TEST_DATA[0]);
	}

	void EncodeCaseFin()
	{
		if( !m_last_message.empty() )
		{
			TRACE("%s\n", m_last_message.c_str());
			m_last_message.clear();
		}
		m_index++;
	}

	bool EncodeCase()
	{
		using namespace spplib::encode;
		size_t TEST_DATA_LEN = strlen(QP_TEST_DATA[m_index][0]);
		std::vector<char> buff(1024);//base64::size_of_encode<>(TEST_DATA_LEN) + 1);
		quoted_printable::encoder(&buff[0], buff.size(), QP_TEST_DATA[m_index][0], TEST_DATA_LEN);

	//	std::vector<char> buff2(base64::size_of_decode<>(buff.size() - 1) + 1);
	//	quoted_printable::decoder(&buff2[0], buff2.size(), &buff[0], buff.size() - 1);

		if( !strcmp(QP_TEST_DATA[m_index][1], &buff[0]) ) {
		//	TRACE(_T("OK: >%s<\n"), QP_TEST_DATA[m_index][0]);
			return true;
		} else {
		//	TRACE(_T("NG: >%s<\n    >%s<\n    >%s<\n"), QP_TEST_DATA[m_index][0], QP_TEST_DATA[m_index][1], &buff[0]);
		//	m_last_message
			return false;
		}
	//	TRACE(_T("%-10s -> %-15s -> %-10s\n"), QP_TEST_DATA[m_index], &buff[0], &buff2[0]);
	}

public:

	qp_unit_test()
		: spplib::tester::base<qp_unit_test>("quoted-printable")
		, m_index(0)
	{
		append(_T("plain ascii should not be encoded"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("plain ascii should not be encoded"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("8-bit chars should be encoded"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("trailing space should be encoded"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("trailing space should be encoded"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("trailing space should be encoded"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("\"=\" is special an should be decoded"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("\"=\" is special an should be decoded"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Very long lines should be broken (not more than 76 chars"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Long lines after short lines were broken through 2.01."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("first crack at fixing that bug failed for multiple long lines."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Don't break a line that's near but not over 76 chars."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Don't break a line that's near but not over 76 chars."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Don't break a line that's near but not over 76 chars."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Don't break a line that's near but not over 76 chars."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Don't break a line that's near but not over 76 chars."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Don't break a line that's near but not over 76 chars."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Don't break a line that's near but not over 76 chars."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Don't break a line that's near but not over 76 chars."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Don't break a line that's near but not over 76 chars."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Don't break a line that's near but not over 76 chars."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Don't break a line that's near but not over 76 chars."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Don't break a line that's near but not over 76 chars."), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("Not allowed to break =XX escapes using soft line break"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("some extra special cases we have had problems with"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
		append(_T("trailing whitespace"), &qp_unit_test::EncodeCase, &qp_unit_test::EncodeCaseInit, &qp_unit_test::EncodeCaseFin);
	}

};

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	using namespace spplib::encode;

	char *TEST_DATA[] = {
		"A",		"AB",		"ABC",
		"ABCD",		"ABCDE",	"ABCDEF",
		"ABCDEFG",	"ABCDEFGH",	"ABCDEFGHI",
	};

	//---------------------------------
	// BASE64 test
	//---------------------------------
	for(size_t i = 0; i < _countof(TEST_DATA); i++)
	{
		size_t TEST_DATA_LEN = strlen(TEST_DATA[i]);
		std::vector<char> buff(base64::size_of_encode<>(TEST_DATA_LEN) + 1);
		base64::encoder(&buff[0], buff.size(), TEST_DATA[i], TEST_DATA_LEN);

		std::vector<char> buff2(base64::size_of_decode<>(buff.size() - 1) + 1);
		base64::decoder(&buff2[0], buff2.size(), &buff[0], buff.size() - 1);

		TRACE(_T("%-10s -> %-15s -> %-10s\n"), TEST_DATA[i], &buff[0], &buff2[0]);
	}

	//---------------------------------
	// quoted-printable test
	//---------------------------------
	qp_unit_test x;
	x.run();
	//int n = 0
	//for(size_t i = 0; i < _countof(TEST_DATA2); i++)
	//{
	//	size_t TEST_DATA_LEN = strlen(TEST_DATA2[i][0]);
	//	std::vector<char> buff(1024);//base64::size_of_encode<>(TEST_DATA_LEN) + 1);
	//	quoted_printable::encoder(&buff[0], buff.size(), TEST_DATA2[i][0], TEST_DATA_LEN);

	////	std::vector<char> buff2(base64::size_of_decode<>(buff.size() - 1) + 1);
	////	quoted_printable::decoder(&buff2[0], buff2.size(), &buff[0], buff.size() - 1);

	//	if( !strcmp(TEST_DATA2[i][1], &buff[0]) ) {
	//		TRACE(_T("OK: >%s<\n"), TEST_DATA2[i][0]);
	//	} else {
	//		TRACE(_T("NG: >%s<\n    >%s<\n    >%s<\n"), TEST_DATA2[i][0], TEST_DATA2[i][1], &buff[0]);
	//	}
	////	TRACE(_T("%-10s -> %-15s -> %-10s\n"), TEST_DATA2[i], &buff[0], &buff2[0]);
	//}

	//---------------------------------
	// MIME test
	//---------------------------------
	for(size_t i = 0; i < _countof(TEST_DATA); i++)
	{
		size_t TEST_DATA_LEN = strlen(TEST_DATA[i]);
		std::vector<char> buff(256);//MIME::size_of_encode<>(TEST_DATA_LEN) + 1);
		mime::encoder(&buff[0], buff.size(), TEST_DATA[i], TEST_DATA_LEN, "ISO-2022-JP", mime::ENCODE_BASE64);

		std::vector<char> buff2(1);//MIME::size_of_decode<>(buff.size() - 1) + 1);
	//	MIME::decoder(&buff2[0], buff2.size(), &buff[0], buff.size() - 1);

		TRACE(_T("%-10s -> %-15s -> %-10s\n"), TEST_DATA[i], &buff[0], &buff2[0]);
	}

	return 0;
}

