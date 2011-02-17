/**
 * @file  spplib/registry.hpp
 * @brief レジストリ操作オブジェクトクラス定義
 * @note SPPLIB_REGISTRY_DISABLE_STL_SUPPORT を定義すると stl::string などを使用したメソッドが使用できなくなる
 * @note SPPLIB_REGISTRY_ENABLE_INLINE をヘッダファイルの前に定義すると .cpp をヘッダに読み込みインライン化する
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2009.
 */

#include <windows.h>
#include <tchar.h>
#include <wchar.h>

#ifndef SPPLIB_REGISTRY_DISABLE_STL_SUPPORT // STL を使用しないようにする
#include <string>
#endif // !defined(SPPLIB_REGISTRY_DISABLE_STL_SUPPORT)

#ifndef SPPLIB_FA2E1A64_9EA3_4FB3_A2EA_301EE907841C_INCLUDE_GUARD
#define SPPLIB_FA2E1A64_9EA3_4FB3_A2EA_301EE907841C_INCLUDE_GUARD

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

/// sharkpp class library namespace
namespace spplib {

/**
 * @brief レジストリ操作オブジェクトクラス
 * @note SPPLIB_REGISTRY_DISABLE_STL_SUPPORT を定義すると stl::string などを使用したメソッドが使用できなくなる
 * @note SPPLIB_REGISTRY_ENABLE_INLINE をヘッダファイルの前に定義すると .cpp をヘッダに読み込みインライン化する
 * @code
 *  spplib::registry reg;
 *  TCHAR buff[256];
 *  reg.select("HKCR\\Paint.Picture");
 *  reg.get("BrowserFlags", buff);
 *  printf("[HKCR\\Paint.Picture]\nBrowserFlags='%s'\n", buff);
 *  reg.close(); // optional
 * @endcode
 */
class registry
{
private:

	HKEY			m_key_handle;

	int				m_last_error;	///< 最後に発生したエラー状態

private:

	/**
	 * @brief キーのパスから定義済みの定数を返しサブキーを返す
	 * @internal
	 */
	HKEY search_defined_key_handle(LPCTSTR key_name, LPCTSTR * sub_key_name = NULL);

public:

	/** 
	 * @brief デフォルトコンストラクタ
	 */
	registry();

	/** 
	 * @brief     コンストラクタ
	 * @param[in] key_name    オープンするレジストリーキー
	 * @param[in] access_mask アクセス権限
	 * @param[in] must_exist  キーが存在していないといけないかどうか
	 * @note      処理が失敗した場合は、int型の例外を発行します。
	 */
	registry(LPCTSTR key_name, REGSAM access_mask = KEY_READ|KEY_WRITE, bool must_exist = false);

	/** 
	 * @brief     コンストラクタ
	 * @param[in] key_name    オープンするレジストリーキー
	 * @param[in] access_mask アクセス権限
	 * @param[in] must_exist  キーが存在していないといけないかどうか
	 * @note      処理が失敗した場合は、int型の例外を発行します。
	 */
	registry(const GUID & key_name, REGSAM access_mask = KEY_READ|KEY_WRITE, bool must_exist = false);

	/** 
	 * @brief デストラクタ
	 * @note  関連付けられているレジストリキーハンドルは閉じられます
	 */
	~registry();

	/** 
	 * @brief  最後に発生したエラー状態を取得
	 * @retval ERROR_SUCCESS エラーが発生していないことを示します
	 * @retval その他        その他のエラーが起こったことを示します
	 */
	int get_last_error() const
	{
		return m_last_error;
	}

	/** 
	 * @brief  レジストリキーを開いているか否かの状態を取得
	 * @retval true  開いている
	 * @retval false 開いていない
	 */
	bool opened()
	{
		return false != m_key_handle;
	}

	/** 
	 * @brief  HKEYへのキャスト
	 * @return 関連付けているキーハンドル
	 */
	operator HKEY () const
	{
		return m_key_handle;
	}

	/** 
	 * @brief     操作対象のレジストリキーを指定
	 * @param[in] key_name    オープンするレジストリーキー
	 * @param[in] access_mask アクセス権限
	 * @param[in] must_exist  キーが存在していないといけないかどうか
	 * @retval    true  成功
	 * @retval    false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool select(LPCTSTR key_name, REGSAM access_mask = KEY_READ|KEY_WRITE, bool must_exist = false);

	/** 
	 * @brief     操作対象のレジストリキーを指定(GUID)
	 * @param[in] key_name    オープンするレジストリーキー
	 * @param[in] access_mask アクセス権限
	 * @param[in] must_exist  キーが存在していないといけないかどうか
	 * @retval    true  成功
	 * @retval    false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool select(const GUID & key_name, REGSAM access_mask = KEY_READ|KEY_WRITE, bool must_exist = false);

	/** 
	 * @brief     指定のレジストリキーを削除
	 * @param[in] key_name    オープンするレジストリーキー
	 * @retval    true  成功
	 * @retval    false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 * @note 削除対象のレジストリキーをselect()ですでに選択していた場合は以降の動作は不定になります。
	 */
	bool erase(LPCTSTR key_name);

	/** 
	 * @brief  レジストリキーのクローズ
	 * @retval true  成功
	 * @retval false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool close();

	/** 
	 * @brief    レジストリ値の削除
	 * @param[in]  value_name       削除するレジストリ値の名前
	 * @retval     true  成功
	 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool remove(LPCTSTR value_name);

	/** 
	 * @brief    レジストリ値の取得(文字列)
	 * @param[in]  value_name       取得するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
	 * @param[out] buff             取得用のバッファ
	 * @param[in]  buff_size        取得用のバッファに確保済みのサイズ
	 * @param[out] buff_result_size 読み取りサイズ
	 * @retval     true  成功
	 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool get(LPCTSTR value_name, LPTSTR buff, DWORD buff_size, DWORD * buff_result_size = NULL);

#ifndef SPPLIB_REGISTRY_DISABLE_STL_SUPPORT

	/** 
	 * @brief    レジストリ値の取得(文字列)
	 * @param[in]  value_name 取得するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
	 * @param[out] buff       取得用のバッファ
	 * @retval     true  成功
	 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
#if !defined(_UNICODE)
	bool get(LPCTSTR value_name, std::string & buff);
#else
	bool get(LPCTSTR value_name, std::wstring & buff);
#endif

#endif // !defined(SPPLIB_REGISTRY_DISABLE_STL_SUPPORT)

	/** 
	 * @brief    レジストリ値の取得(int)
	 * @param[in]  value_name 取得するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
	 * @param[out] buff       取得用のバッファ
	 * @retval     true  成功
	 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool get(LPCTSTR value_name, int & buff);

	/** 
	 * @brief    レジストリ値の取得(bool)
	 * @param[in]  value_name 取得するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
	 * @param[out] buff       取得用のバッファ
	 * @retval     true  成功
	 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool get(LPCTSTR value_name, bool & buff);

	/** 
	 * @brief    レジストリ値の取得(DWORD)
	 * @param[in]  value_name 取得するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
	 * @param[out] buff       取得用のバッファ
	 * @retval     true  成功
	 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool get(LPCTSTR value_name, DWORD & buff);

	/** 
	 * @brief    レジストリ値の取得(unsigned long long)
	 * @param[in]  value_name 取得するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
	 * @param[out] buff       取得用のバッファ
	 * @retval     true  成功
	 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool get(LPCTSTR value_name, unsigned long long & buff);

	/** 
	 * @brief    レジストリ値の設定(文字列)
	 * @param[in]  value_name       設定するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
	 * @param[out] data             設定するデータ
	 * @param[in]  data_size        設定するデータのサイズ
	 * @param[out] expand           trueを指定するとREG_SZではなくREG_EXPAND_SZで値をセットする
	 * @retval     true  成功
	 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool set(LPCTSTR value_name, LPCTSTR data, DWORD data_size = (DWORD)-1, bool expand = false);

	/** 
	 * @brief    レジストリ値の設定(int)
	 * @param[in]  value_name       設定するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
	 * @param[out] data             設定するデータ
	 * @retval     true  成功
	 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool set(LPCTSTR value_name, int data);

	/** 
	 * @brief    レジストリ値の設定(DWORD)
	 * @param[in]  value_name       設定するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
	 * @param[out] data             設定するデータ
	 * @retval     true  成功
	 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool set(LPCTSTR value_name, DWORD data);

	/** 
	 * @brief    レジストリ値の設定(bool)
	 * @param[in]  value_name       設定するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
	 * @param[out] data             設定するデータ
	 * @retval     true  成功
	 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool set(LPCTSTR value_name, bool data);

	/** 
	 * @brief    レジストリ値の設定(unsigned long long)
	 * @param[in]  value_name       設定するレジストリ値の名前、NULLもしくは空文字列にすると"(既定)"を読み取る
	 * @param[out] data             設定するデータ
	 * @retval     true  成功
	 * @retval     false 失敗、詳細なエラーを取得するには get_last_error() を呼び出します
	 */
	bool set(LPCTSTR value_name, unsigned long long data);

};

} // namespace spplib

#ifdef SPPLIB_REGISTRY_ENABLE_INLINE
#include "registry.cpp"
#endif // defined(SPPLIB_REGISTRY_ENABLE_INLINE)

#endif // !defined(SPPLIB_FA2E1A64_9EA3_4FB3_A2EA_301EE907841C_INCLUDE_GUARD)
