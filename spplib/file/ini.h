#pragma once // include guard for MSVC++

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <tchar.h>
#include <wchar.h>
#include <string>

#ifndef INCLUDE_GUARD_DF06FFA3_F513_44C8_B512_D041899FBD8F
#define INCLUDE_GUARD_DF06FFA3_F513_44C8_B512_D041899FBD8F

namespace spplib
{
 namespace file
 {

//--------------------------------------------------------------------
// 定数
//--------------------------------------------------------------------

static const int MAX_SECTION_NAME = 128;

//--------------------------------------------------------------------
// 構造体定義
//--------------------------------------------------------------------

typedef struct {
	WORD	wType;		// 行の属性
	TCHAR *	pszLine;	// 行ポインタ
	TCHAR *	pszValue;	// キーの値ポインタ
} INI_LINE_INFO, *LPINI_LINE_INFO;

//--------------------------------------------------------------------
// クラス定義
//--------------------------------------------------------------------

class ini
{
public:
	ini();
	virtual ~ini();

	void selectsection(LPCTSTR lpszSectionName);

	bool open(LPCTSTR lpszFileName);

	bool close();

	bool get(LPCTSTR lpszKeyName, LPTSTR lpszBuffer, DWORD dwLen, LPTSTR lpszDefault = _T(""));
	bool put(LPCTSTR lpszKeyName, LPCTSTR lpszValue);

	bool get(LPCTSTR lpszKeyName, std::string & strBuffer, LPTSTR lpszDefault = _T(""));
	bool put(LPCTSTR lpszKeyName, const std::string & strValue);

	bool get(LPCTSTR lpszKeyName, int & nBuffer, int nDefault = 0);
	bool put(LPCTSTR lpszKeyName, int nValue);

	bool get(LPCTSTR lpszKeyName, DWORD & dwBuffer, DWORD dwDefault = 0);
	bool put(LPCTSTR lpszKeyName, DWORD dwValue);

	bool get(LPCTSTR lpszKeyName, WORD & wBuffer, WORD wDefault = 0);
	bool put(LPCTSTR lpszKeyName, WORD wValue);

	bool get(LPCTSTR lpszKeyName, bool & bBuffer, bool bDefault = true);
	bool put(LPCTSTR lpszKeyName, bool bValue);

protected:

	bool GetCore(LPCTSTR lpszKeyName, LPTSTR lpszBuffer, DWORD dwLen, LPTSTR lpszDefault);
	bool PutCore(LPCTSTR lpszKeyName, LPCTSTR lpszValue);

	bool ReadFileCore(LPCTSTR lpszFileName);
	bool WriteFileCore(LPCTSTR lpszFileName);

	int  GetSectionLineCore(LPCTSTR lpszSection);
	bool MarkDeleteSectionCore(int nLine);

	void MarkDelete();

private:
	TCHAR		m_szSectionName[MAX_SECTION_NAME];
	TCHAR		m_szFileName[MAX_PATH];
	int			m_nSelectSectionLine;
	int			m_nLineCount;
	LPINI_LINE_INFO
				m_ptLineInfo;
	DWORD		m_dwFileMem;
	TCHAR *		m_pszFileMem;
	bool		m_bWrite;
};

 } // namespace string
} // namespace spplib

#endif // !defined(INCLUDE_GUARD_DF06FFA3_F513_44C8_B512_D041899FBD8F)

//EOF