#include "cmdline.h"
#include <algorithm>	// find_if()
#include "debug.h"

#pragma warning( disable : 4996 )

namespace spplib
{

// コンストラクタ
cmdline::cmdline(char * cmdline_ptr, bool include_cmd_name)
{
	split_args(cmdline_ptr, include_cmd_name);
}

// デストラクタ
cmdline::~cmdline()
{
}

class arg_type_equal_cmp
{
	std::string	find_value;
public:
	arg_type_equal_cmp(const std::string & v)
		: find_value(v)
	{
		std::string::size_type pos;
		pos = find_value.find(":");
		find_value = (std::string::npos != pos)
						? find_value.substr(0, pos)
						: find_value;
		pos = find_value.find("=");
		find_value = (std::string::npos != pos)
						? find_value.substr(0, pos)
						: find_value;
		find_value = find_value;
	}
	bool operator ()(const std::pair<std::string, int> & v) const {
		return !v.first.compare(0, find_value.size(), find_value);
	}
};

// 引数の解析
cmdline::arg_array cmdline::parse()
{
	arg_array r;

	register_arg_type();

	int file_name_idx = 0;
	for(ARGS::iterator p = m_Args.begin();
		p != m_Args.end(); p++)
	{
		ARG_TYPE_MAP::iterator arg_type_ptr;
		ARG_INFO info;

		info.value = *p;

		arg_type_ptr = std::find_if(
							  m_ArgType.begin()
							, m_ArgType.end()
							, arg_type_equal_cmp(*p)
							);

		// 場合わけ
		//  定義    | 実際        | 結果
		//  --hoge  |             | ""
		//  --hoge= | --hoge=fuga | "fuga"
		//  --hoge: | --hoge fuga | "fuga"
		//          | fuga        | "fuga"

		if( arg_type_ptr != m_ArgType.end() ) {
			char c = arg_type_ptr->first[arg_type_ptr->first.size() -1];
			info.value.clear();
			if( ':' == c ) {
				p++;
				if( p != m_Args.end() ) {
					info.value = *p;
				} else {
					p--;
				}
			} else if( '=' == c ) {
				if( p->size() < arg_type_ptr->first.size() ) {
					info.value.clear();
				} else {
					info.value = p->substr(arg_type_ptr->first.size());
					if( info.value.size() && '\"' == info.value[0] ) {
						info.value = info.value.substr(1, info.value.size() - 2);
					}
				}
			}
			info.type = arg_type_ptr->second;
		} else if( '-' == (*p)[0] ) {
			usage("invalid argument!");
			r.clear();
			return r;
		} else {
			info.type = file_name_base + file_name_idx;
			file_name_idx--;
		}

		r.push_back(info);
	}

	if( r.empty() ) {
		usage();
	}
	return r;
}

// 引数の分割
void cmdline::split_args(char * cmdline_ptr, bool include_cmd_name)
{
	std::string tmp;
	bool bDQ = false;
	char *pa, *pb;

	// 引数の分割
	for(pa = pb = cmdline_ptr; *pa; pa++)
	{
		if('\"' == *pa) {
			bDQ = bDQ ? false : true;
		} else if(!bDQ && ' ' == *pa) {
			tmp.assign(pb, 0, (std::string::size_type)(pa - pb));
			pb = pa + 1;
			m_Args.push_back(tmp);
		}
	}
	if( pb < pa ) {
		tmp.assign(pb, 0, (std::string::size_type)(pa - pb));
		m_Args.push_back(tmp);
	}

	// 実行ファイル名を取り除く
	if( include_cmd_name && !m_Args.empty() ) {
		m_Args.pop_front();
	}

	// 文字列についている'\"'を削除する
	for(ARGS::iterator p = m_Args.begin();
		p != m_Args.end(); p++)
	{
		if( '\"' == (*p)[0] ) {
			*p = p->substr(1, p->size() - 2);
		}
	}
}

} // namespace spplib

//EOF
