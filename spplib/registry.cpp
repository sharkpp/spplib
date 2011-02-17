/**
 * @file  spplib/registry.cpp
 * @brief レジストリ操作オブジェクトクラス実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2009.
 */

#include "registry.hpp"

#ifdef SPPLIB_REGISTRY_ENABLE_INLINE
#define SPPLIB_REGISTR_INLINE inline
#else
#define SPPLIB_REGISTR_INLINE
#endif // defined(SPPLIB_REGISTRY_ENABLE_INLINE)

/// sharkpp class library namespace
namespace spplib {

/**
 * @brief キーのパスから定義済みの定数を返しサブキーを返す
 * @internal
 */
SPPLIB_REGISTR_INLINE
HKEY registry::search_defined_key_handle(LPCTSTR key_name, LPCTSTR * sub_key_name /* = NULL */)
{
	static const struct {
		LPTSTR	name;
		int		len;
		HKEY	handle;
	} table[] = {
		{ _T("HKCR"),						 4, HKEY_CLASSES_ROOT			},
		{ _T("HKEY_CLASSES_ROOT"),			17, HKEY_CLASSES_ROOT			},
		{ _T("HKCU"),						 4, HKEY_CURRENT_USER			},
		{ _T("HKEY_CURRENT_USER"),			17, HKEY_CURRENT_USER			},
		{ _T("HKLM"),						 4, HKEY_LOCAL_MACHINE			},
		{ _T("HKEY_LOCAL_MACHINE"),			18, HKEY_LOCAL_MACHINE			},
		{ _T("HKU"),						 3, HKEY_USERS					},
		{ _T("HKEY_USERS"),					10, HKEY_USERS					},
		{ _T("HKPD"),						 4, HKEY_PERFORMANCE_DATA		},
		{ _T("HKEY_PERFORMANCE_DATA"),		21, HKEY_PERFORMANCE_DATA		},
		{ _T("HKPT"),						 4, HKEY_PERFORMANCE_TEXT		},
		{ _T("HKEY_PERFORMANCE_TEXT"),		21, HKEY_PERFORMANCE_TEXT		},
		{ _T("HKPN"),						 4, HKEY_PERFORMANCE_NLSTEXT	},
		{ _T("HKEY_PERFORMANCE_NLSTEXT"),	24, HKEY_PERFORMANCE_NLSTEXT	},
#if(WINVER >= 0x0400)
		{ _T("HKCC"),						 4, HKEY_CURRENT_CONFIG			},
		{ _T("HKEY_CURRENT_CONFIG"),		19, HKEY_CURRENT_CONFIG			},
		{ _T("HKDD"),						 4, HKEY_DYN_DATA				},
		{ _T("HKEY_DYN_DATA"),				13, HKEY_DYN_DATA				},
#endif
	};
	for(int i = 0, num = sizeof(table)/sizeof(table[0]);
		i < num; i++)
	{
		if(	!::_tcsnccmp(table[i].name, key_name, table[i].len) &&
			table[i].len <= (int)::_tcslen(key_name) &&
			('\\' == key_name[table[i].len] || !key_name[table[i].len]) )
		{
			if( sub_key_name )
			{
				*sub_key_name = key_name + table[i].len + 1;
			}
			return table[i].handle;
		}
	}
	return NULL;
}

/** 
 * @brief デフォルトコンストラクタ
 */
SPPLIB_REGISTR_INLINE
registry::registry()
	: m_key_handle(NULL)
	, m_last_error(ERROR_SUCCESS)
{
}

/** 
 * @brief     コンストラクタ
 * @param[in] key_name    オープンするレジストリーキー
 * @param[in] access_mask アクセス権限
 * @param[in] must_exist  キーが存在していないといけないかどうか
 * @note      処理が失敗した場合は、int型の例外を発行します。
 */
registry::registry(LPCTSTR key_name, REGSAM access_mask, bool must_exist)
	: m_key_handle(NULL)
	, m_last_error(ERROR_SUCCESS)
{
	if( !select(key_name, access_mask, must_exist) )
	{
		throw get_last_error();
	}
}

/** 
 * @brief     コンストラクタ
 * @param[in] key_name    オープンするレジストリーキー
 * @param[in] access_mask アクセス権限
 * @param[in] must_exist  キーが存在していないといけないかどうか
 * @note      処理が失敗した場合は、int型の例外を発行します。
 */
registry::registry(const GUID & key_name, REGSAM access_mask, bool must_exist)
	: m_key_handle(NULL)
	, m_last_error(ERROR_SUCCESS)
{
	if( !select(key_name, access_mask, must_exist) )
	{
		throw get_last_error();
	}
}

/** 
 * @brief デストラクタ
 * @note  関連付けられているレジストリキーハンドルは閉じられます
 */
SPPLIB_REGISTR_INLINE
registry::~registry()
{
	close();
}

/** 
 * @brief     操作対象のレジストリキーを指定
 * @param[in] key_name    オープンするレジストリーキー
 * @param[in] access_mask アクセス権限
 * @param[in] must_exist  キーが存在していないといけないかどうか
 * @retval    true  成功
 * @retval    false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::select(LPCTSTR key_name, REGSAM access_mask /* = KEY_READ|KEY_WRITE */, bool must_exist /* = false */)
{
	LPCTSTR sub_key_name = NULL;
	HKEY parent_key_handle =
		search_defined_key_handle(key_name, &sub_key_name);

	// 定義済みキーハンドルの取得に失敗した場合は現在開いているキーを親としてサブキーを開く
	if( !parent_key_handle )
	{
		parent_key_handle = m_key_handle;
		sub_key_name = key_name;
	}

	HKEY temp_key_handle = NULL;

	if( must_exist )
	{
		m_last_error =
			::RegOpenKeyEx(parent_key_handle, sub_key_name, 0, access_mask, &temp_key_handle);
	}
	else
	{
		m_last_error =
			::RegCreateKeyEx(
				  parent_key_handle, sub_key_name, 0, NULL
				, REG_OPTION_NON_VOLATILE, access_mask, NULL, &temp_key_handle, NULL);
	}
	if( ERROR_SUCCESS == m_last_error )
	{
		if( m_key_handle )
		{
			if( !close() )
			{
				return false;
			}
		}
		m_key_handle = temp_key_handle;
	}

	return ERROR_SUCCESS == m_last_error;
}

/** 
 * @brief     操作対象のレジストリキーを指定(GUID)
 * @param[in] key_name    オープンするレジストリーキー
 * @param[in] access_mask アクセス権限
 * @param[in] must_exist  キーが存在していないといけないかどうか
 * @retval    true  成功
 * @retval    false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::select(const GUID & key_name, REGSAM access_mask /* = KEY_READ|KEY_WRITE */, bool must_exist /* = false */)
{
	TCHAR guid[128];

	if( NULL == m_key_handle )
	{
		m_last_error = ERROR_INVALID_HANDLE;
		return false;
	}

#if defined(_MSC_VER) && 1400 <= _MSC_VER // VC++ .NET 2005 以上
	::_stprintf_s(guid, _T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}")
		, key_name.Data1, key_name.Data2, key_name.Data3
		, key_name.Data4[0], key_name.Data4[1], key_name.Data4[2], key_name.Data4[3]
		, key_name.Data4[4], key_name.Data4[5], key_name.Data4[6], key_name.Data4[7]
		);
#else
	::_stprintf(guid, _T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}")
		, key_name.Data1, key_name.Data2, key_name.Data3
		, key_name.Data4[0], key_name.Data4[1], key_name.Data4[2], key_name.Data4[3]
		, key_name.Data4[4], key_name.Data4[5], key_name.Data4[6], key_name.Data4[7]
		);
#endif

	return select(guid, access_mask, must_exist);
}

/** 
 * @brief     指定のレジストリキーを削除
 * @param[in] key_name    オープンするレジストリーキー
 * @retval    true  成功
 * @retval    false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 * @note 削除対象のレジストリキーをselect()ですでに選択していた場合は以降の動作は不定になります。
 */
bool registry::erase(LPCTSTR key_name)
{
	DWORD key_name_len = (DWORD)_tcslen(key_name);
	LPTSTR buff = new TCHAR[key_name_len + 1];
	LPTSTR parent_key_name = buff;
	LPTSTR target_key_name;

	if( !buff )
	{
		m_last_error = ERROR_OUTOFMEMORY;
		return false;
	}

	// 親キー名と対象のキー名とを分ける
#if defined(_MSC_VER) && 1400 <= _MSC_VER // VC++ .NET 2005 以上
	_tcsncpy_s(parent_key_name, key_name_len + 1, key_name, key_name_len);
#else
	_tcsncpy(parent_key_name, key_name, key_name_len);
#endif
	for(LPTSTR p = target_key_name = parent_key_name; *p; p = ::CharNext(p))
	{
		if( '\\' == *p ) { target_key_name = p; }
	}
	if( '\\' != *target_key_name )
	{
		parent_key_name = parent_key_name + key_name_len;
	}
	else
	{
		LPTSTR p = ::CharNext(target_key_name);
		*target_key_name = '\0';
		target_key_name  = p;
	}

	// 親キーを開く
	HKEY base_key_handle = m_key_handle;
	HKEY parent_key_handle = NULL;
	if( *parent_key_name ) // 絶対指定 or 相対指定(複数階層)
	{
		LPCTSTR sub_key_name = NULL;
		HKEY temp_key_handle =
			search_defined_key_handle(parent_key_name, &sub_key_name);

		// 定義済みキーハンドルの取得に失敗した場合は現在開いているキーを親としてサブキーを開く
		if( !temp_key_handle )
		{
			temp_key_handle = m_key_handle;
			sub_key_name = key_name;
		}

		m_last_error =
			::RegOpenKeyEx(temp_key_handle, sub_key_name, 0
				, KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_WRITE, &parent_key_handle);
	}
	else // 相対指定(単一階層)
	{
		parent_key_handle = m_key_handle;
	}

	HKEY key_handle = NULL;
	m_last_error =
		::RegOpenKeyEx(parent_key_handle, target_key_name, 0
			, KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_WRITE, &key_handle);
	if( ERROR_SUCCESS == m_last_error )
	{
		m_key_handle = key_handle;
		int error;
		DWORD max_sub_key_len = 0;
		// サブキーの最大文字数を取得
		m_last_error =
			::RegQueryInfoKey(key_handle, NULL, NULL, NULL, NULL
				, &max_sub_key_len, NULL, NULL, NULL, NULL, NULL, NULL);
		// サブキーを削除してゆく
		DWORD  sub_key_len  = max_sub_key_len + 1;
		LPTSTR sub_key_name = new TCHAR[sub_key_len];
		if( !parent_key_name )
		{
			m_last_error = ERROR_OUTOFMEMORY;
		}
		else
		{
			while( ERROR_SUCCESS
				== (error = ::RegEnumKeyEx(key_handle, 0, sub_key_name, &sub_key_len, NULL, NULL, NULL, NULL)) )
			{
				erase(sub_key_name);
				// 状態を復帰
				m_key_handle= key_handle;
				sub_key_len = max_sub_key_len + 1;
			}
			m_last_error =
				::RegDeleteKey(parent_key_handle, target_key_name);
			m_key_handle = NULL;
			delete [] sub_key_name;
		}
		m_last_error =
			::RegCloseKey(key_handle);
	}

	if( *parent_key_name ) // 絶対指定 or 相対指定(複数階層)
	{
		m_last_error =
			::RegCloseKey(parent_key_handle);
		m_key_handle = base_key_handle;
	}
	else
	{
		m_key_handle = parent_key_handle;
	}

	delete [] buff;

	return ERROR_SUCCESS == m_last_error;
}

/** 
 * @brief  レジストリキーのクローズ
 * @retval true  成功
 * @retval false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::close()
{
	if( m_key_handle )
	{
		m_last_error
			= ::RegCloseKey(m_key_handle);
		m_key_handle = NULL;
	}

	return ERROR_SUCCESS == m_last_error;
}

/** 
 * @brief    レジストリ値の削除
 * @param[in]  value_name       削除するレジストリ値の名前
 * @retval     true  成功
 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
bool registry::remove(LPCTSTR value_name)
{
	m_last_error
		= ::RegDeleteValue(m_key_handle, value_name);

	return ERROR_SUCCESS == m_last_error;
}

/** 
 * @brief    レジストリ値の取得(文字列)
 * @param[in]  value_name       取得するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
 * @param[out] buff             取得用のバッファ
 * @param[in]  buff_size        取得用のバッファに確保済みのサイズ
 * @param[out] buff_result_size 読み取りサイズ
 * @retval     true  成功
 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::get(LPCTSTR value_name, LPTSTR buff, DWORD buff_size, DWORD * buff_result_size /* = NULL */)
{
	DWORD type = REG_NONE;

	m_last_error =
		::RegQueryValueEx(
				m_key_handle, value_name, NULL, &type, (LPBYTE)buff,
				&buff_size
			);

	if( buff_result_size )
	{
		*buff_result_size = buff_size;
	}

	return
		ERROR_SUCCESS == m_last_error &&
		(REG_SZ == type || REG_EXPAND_SZ == type || REG_MULTI_SZ == type);
}

#ifndef SPPLIB_REGISTRY_DISABLE_STL_SUPPORT

/** 
 * @brief    レジストリ値の取得(文字列)
 * @param[in]  value_name 取得するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
 * @param[out] buff       取得用のバッファ
 * @retval     true  成功
 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::get(LPCTSTR value_name,
#if !defined(_UNICODE)
                   std::string & buff
#else
                   std::wstring & buff
#endif
                   )
{
	LPTSTR buff_;
	DWORD type = REG_NONE;
	DWORD buff_size = 0;

	m_last_error =
		::RegQueryValueEx(
				m_key_handle, value_name, NULL, &type, NULL, &buff_size
			);
	if( ERROR_SUCCESS   != m_last_error &&
		ERROR_MORE_DATA != m_last_error )
	{
		return false;
	}
 
	buff_ = (LPTSTR)new BYTE[buff_size];
	m_last_error =
		::RegQueryValueEx(
				m_key_handle, value_name, NULL, &type, (LPBYTE)buff_,
				&buff_size
			);
	buff.assign(buff_);
	delete [] buff_;

	return
		ERROR_SUCCESS == m_last_error &&
		(REG_SZ == type || REG_EXPAND_SZ == type || REG_MULTI_SZ == type);
}

#endif // !defined(SPPLIB_REGISTRY_DISABLE_STL_SUPPORT)

/** 
 * @brief    レジストリ値の取得(int)
 * @param[in]  value_name 取得するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
 * @param[out] buff       取得用のバッファ
 * @retval     true  成功
 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::get(LPCTSTR value_name, int & buff)
{
	DWORD buff_ = (DWORD)buff;
	bool result = get(value_name, buff_);
	buff = (int)buff_;
	return result;
}

/** 
 * @brief    レジストリ値の取得(bool)
 * @param[in]  value_name 取得するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
 * @param[out] buff       取得用のバッファ
 * @retval     true  成功
 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::get(LPCTSTR value_name, bool & buff)
{
	DWORD buff_ = buff ? 1 : 0;
	bool result = get(value_name, buff_);
	buff = buff_ ? true : false;
	return result;
}

/** 
 * @brief    レジストリ値の取得(DWORD)
 * @param[in]  value_name 取得するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
 * @param[out] buff       取得用のバッファ
 * @retval     true  成功
 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::get(LPCTSTR value_name, DWORD & buff)
{
	DWORD type = REG_NONE;
	DWORD buff_size = sizeof(buff);

	m_last_error =
		::RegQueryValueEx(
				m_key_handle, value_name, NULL, &type, (LPBYTE)&buff,
				&buff_size
			);

	return
		ERROR_SUCCESS == m_last_error &&
		(REG_DWORD == type || /* REG_DWORD_LITTLE_ENDIAN == type || */ REG_DWORD_BIG_ENDIAN == type);
}

/** 
 * @brief    レジストリ値の取得(unsigned long long)
 * @param[in]  value_name 取得するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
 * @param[out] buff       取得用のバッファ
 * @retval     true  成功
 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::get(LPCTSTR value_name, unsigned long long & buff)
{
	DWORD type = REG_NONE;
	DWORD buff_size = sizeof(buff);

	m_last_error =
		::RegQueryValueEx(
				m_key_handle, value_name, NULL, &type, (LPBYTE)&buff,
				&buff_size
			);

	return
		ERROR_SUCCESS == m_last_error &&
		(REG_QWORD == type || REG_QWORD_LITTLE_ENDIAN == type);
}

/** 
 * @brief    レジストリ値の設定(文字列)
 * @param[in]  value_name       設定するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
 * @param[out] data             設定するデータ
 * @param[in]  data_size        設定するデータのサイズ
 * @param[out] expand           trueを指定するとREG_SZではなくREG_EXPAND_SZで値をセットする
 * @retval     true  成功
 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::set(LPCTSTR value_name, LPCTSTR data, DWORD data_size /* = (DWORD)-1 */, bool expand /* = false */)
{
	DWORD type = REG_NONE;

	if( (DWORD)-1 == data_size )
	{
		data_size = (DWORD)::_tcslen(data);
	}

	m_last_error =
		::RegQueryValueEx(
				m_key_handle, value_name, NULL, &type, NULL, NULL
			);

	if( ERROR_SUCCESS == m_last_error )
	{
		if( DWORD(expand ? REG_EXPAND_SZ : REG_SZ) != type )
		{
			return false;
		}
	}
	else
	{
		type = expand ? REG_EXPAND_SZ : REG_SZ;
	}

	m_last_error =
		::RegSetValueEx(
				m_key_handle, value_name, NULL, type, (const LPBYTE)data,
				data_size * sizeof(TCHAR)
			);

	return ERROR_SUCCESS == m_last_error;
}

/** 
 * @brief    レジストリ値の設定(int)
 * @param[in]  value_name       設定するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
 * @param[out] data             設定するデータ
 * @retval     true  成功
 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::set(LPCTSTR value_name, int data)
{
	return set(value_name, DWORD(data));
}

/** 
 * @brief    レジストリ値の設定(bool)
 * @param[in]  value_name       設定するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
 * @param[out] data             設定するデータ
 * @retval     true  成功
 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::set(LPCTSTR value_name, bool data)
{
	return set(value_name, DWORD(data ? 1 : 0));
}

/** 
 * @brief    レジストリ値の設定(DWORD)
 * @param[in]  value_name       設定するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
 * @param[out] data             設定するデータ
 * @retval     true  成功
 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::set(LPCTSTR value_name, DWORD data)
{
	DWORD type = REG_NONE;

	m_last_error =
		::RegQueryValueEx(
				m_key_handle, value_name, NULL, &type, NULL, NULL
			);

	if( ERROR_SUCCESS == m_last_error )
	{
		if( REG_DWORD != type && /* REG_DWORD_LITTLE_ENDIAN != type &&*/ REG_DWORD_BIG_ENDIAN != type )
		{
			return false;
		}
	}
	else
	{
		type = REG_DWORD;
	}

	m_last_error =
		::RegSetValueEx(
				m_key_handle, value_name, NULL, type, (const LPBYTE)&data,
				sizeof(data)
			);

	return ERROR_SUCCESS == m_last_error;
}

/** 
 * @brief    レジストリ値の設定(unsigned long long)
 * @param[in]  value_name       設定するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
 * @param[out] data             設定するデータ
 * @retval     true  成功
 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
 */
SPPLIB_REGISTR_INLINE
bool registry::set(LPCTSTR value_name, unsigned long long data)
{
	DWORD type = REG_NONE;

	m_last_error =
		::RegQueryValueEx(
				m_key_handle, value_name, NULL, &type, NULL, NULL
			);

	if( ERROR_SUCCESS == m_last_error )
	{
		if( REG_QWORD != type && REG_QWORD_LITTLE_ENDIAN != type )
		{
			return false;
		}
	}
	else
	{
		type = REG_QWORD;
	}

	m_last_error =
		::RegSetValueEx(
				m_key_handle, value_name, NULL, type, (const LPBYTE)&data,
				sizeof(data)
			);

	return ERROR_SUCCESS == m_last_error;
}

} // namespace spplib
