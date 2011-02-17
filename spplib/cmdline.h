#pragma once // include guard for MSVC++

#ifndef INCLUDE_GUARD_SPPLIB_CMDLINE_H
#define INCLUDE_GUARD_SPPLIB_CMDLINE_H

#include <map>
#include <deque>
#include <string>

namespace spplib
{
	/*
	 * �g����
	 *
	 * [��`]
	 *	class myarg : public spplib::cmdline
	 *	{
	 *	public:
	 *		typedef enum {
	 *			hoge	= arg_type_base,
	 *			fuga,
	 *		} ARG_TYPE;
	 *
	 *	public:
	 *		myarg(char * cmdline_text) : cmdline(cmdline_text, true) {}
	 *
	 *		virtual void usage(char * s = NULL) {
	 *			// �g������\��
	 *			// ...
	 *		}
	 *
	 *	protected:
	 *		// ������ʂ̓o�^
	 *		virtual void register_arg_type() {
	 *			regist("--hoge",	hoge);
	 *			regist["--fuga=",	fuga);
	 *		}
	 *
	 *	};
	 *
	 * [�Ăяo��]
	 *	myarg arg("foo.exe --hoge --fuga=. bar.txt");
	 *	myarg::arg_array args = arg.parse();
	 *	if( args.empty() ) {
	 *		arg.usage();
	 *	} else {
	 *		for(myarg::arg_array::iterator p = args.begin();
	 *			p != args.end(); p++)
	 *		{
	 *			TRACE("%d: '%s'\n", p->type, p->value.c_str());
	 *			switch( p->type )
	 *			{
	 *			case myarg::hoge:
	 *				break;
	 *			case myarg::fuga:
	 *				break;
	 *			case myarg::file_name_base - 0:
	 *				break;
	 *			default:
	 *				arg.usage("invalid argument!");
	 *				goto abort;
	 *			}
	 *		}
	 *	}
	 *	abort:
	 *		;
	 */

	class cmdline
	{
	private:
		typedef std::deque<std::string>		ARGS;

		ARGS			m_Args;

		typedef std::map<std::string, int>	ARG_TYPE_MAP;

		ARG_TYPE_MAP	m_ArgType;

	public:
		typedef struct {
			int			type;
			std::string	value;
		} ARG_INFO;

		typedef enum {
			file_name_base = 0,	// ���̕����ɒl���������p����
			arg_type_base,
		} ARG_TYPE;

		typedef std::deque<ARG_INFO>	arg_array;

		arg_array		m_ArgInfo;

	public:
		// �R���X�g���N�^
		cmdline(char * cmdline_ptr, bool include_cmd_name);

		// �f�X�g���N�^
		virtual ~cmdline();

		// �����̉��
		arg_array parse();

		// �g������\��
		virtual void usage(const char * /* s */ = NULL) {}

	protected:
		// �����̕���
		void split_args(char * cmdline_ptr, bool include_cmd_name);
		// ������ʂ̓o�^
		virtual void register_arg_type() {}
		// ������ʂ̓o�^
		void regist(char * param, int type)
			{ m_ArgType[param] = type; }
	};


#endif // !defined(INCLUDE_GUARD_SPPLIB_CMDLINE_H)

} // namespace spplib

//EOF
