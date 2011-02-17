/**
 * @file  spplib/usespi.hpp
 * @brief Susieプラグイン使用クラス
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_6AB990F7_853E_4A4E_99ED_EA328739648C_INCLUDE_GUARD
#define SPPLIB_6AB990F7_853E_4A4E_99ED_EA328739648C_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {

/// using susie plugin library namespace
namespace spi {

// Load() 用フラグ
typedef enum SPI_FIND_TYPE {
	SPI_FIND_SUSIE    = 1,	//Susieと同じフォルダ
	SPI_FIND_EXE      = 2,	//実行ファイルと同じフォルダ
	SPI_FIND_PATH_ENV = 4,	//環境変数PATH
	SPI_FIND_ALL      = 0xff,
};

// GetSpiVersion() 用フラグ
typedef enum SPI_TYPE {
	SPI_TYPE_UNKNOWN = 0,	//Unknown Susie Plugin Version
	SPI_TYPE_00IN    = 1,	//Normal - Import filter
	SPI_TYPE_00AM    = 2,	//Multi-picture - Archive extractor
};

#pragma pack(1)

// 画像情報
typedef struct
{
	int    left, top;  // 画像を展開する位置
	int    width;      // 画像の幅(pixel)
	int    height;     // 画像の高さ(pixel)
	WORD   x_density;  // 画素の水平方向密度
	WORD   y_density;  // 画素の垂直方向密度
	short  colorDepth; // １画素当たりのbit数
	HLOCAL hInfo;      // 画像内のテキスト情報
} PICTUREINFO;

// アーカイブデータ情報
typedef struct
{
	unsigned char method[8];     // 圧縮法の種類(method[0]=='\0'で終端)
	unsigned int  position;      // ファイル上での位置
	unsigned int  compsize;      // 圧縮されたサイズ
	unsigned int  filesize;      // 元のファイルサイズ
#if defined(_TIME64_T_DEFINED) || (defined(_TIME_T_DEFINED) && !defined(_USE_32BIT_TIME_T))
	__time32_t    timestamp;     // ファイルの更新日時
#else
	time_t        timestamp;     // ファイルの更新日時
#endif
	char          path[200];     // 相対パス
	char          filename[200]; // ファイルネーム
	unsigned int  crc;           // CRC
} FILEINFO;

#pragma pack()

// Susie plug-in API

#define SPIEXPORT WINAPI

#ifdef __cplusplus
extern "C" {
#endif

// 共通
typedef int (SPIEXPORT *SPI_GETPLUGININFO)(int, LPSTR, int );
typedef int (SPIEXPORT *SPI_ISSUPPORTED)(LPSTR, DWORD );
typedef int (SPIEXPORT *SPI_CONFIGURATIONDLG)(HWND, int );
static const char SPI_FUNC_GETPLUGININFO[]    = "GetPluginInfo";
static const char SPI_FUNC_ISSUPPORTED[]      = "IsSupported";
static const char SPI_FUNC_CONFIGURATIONDLG[] = "ConfigurationDlg";

typedef int (CALLBACK *PROGRESS)(int, int, long);

// 画像展開
typedef int (SPIEXPORT *SPI_GETPICTUREINFO)(LPSTR, long, unsigned int, PICTUREINFO *);
typedef int (SPIEXPORT *SPI_GETPICTURE)(LPSTR, long, unsigned int, HANDLE *, HANDLE *, PROGRESS, long );
typedef int (SPIEXPORT *SPI_GETPREVIEW)(LPSTR, long, unsigned int, HANDLE *, HANDLE *, PROGRESS, long );
static const char SPI_FUNC_GETPICTUREINFO[]   = "GetPictureInfo";
static const char SPI_FUNC_GETPICTURE[]       = "GetPicture";
static const char SPI_FUNC_GETPREVIEW[]       = "GetPreview";

// アーカイブ抽出
typedef int (SPIEXPORT *SPI_GETARCHIVEINFO)(LPSTR, long, unsigned int, HLOCAL *);
typedef int (SPIEXPORT *SPI_GETFILEINFO)(LPSTR, long, LPSTR, unsigned int, FILEINFO *);
typedef int (SPIEXPORT *SPI_GETFILE)(LPSTR, long, LPSTR, unsigned int, PROGRESS, long );
static const char SPI_FUNC_GETARCHIVEINFO[]   = "GetArchiveInfo";
static const char SPI_FUNC_GETFILEINFO[]      = "GetFileInfo";
static const char SPI_FUNC_GETFILE[]          = "GetFile";

#ifdef __cplusplus
}
#endif

// エラーコード
typedef enum SPI_ERROR_TYPE {
	SPI_ERROR_NOTIMPLEMENT = -1, // -1:実装されていない機能
	SPI_ERROR_SUCCEED,           //  0:正常終了
	SPI_ERROR_CANCEL,            //  1:展開キャンセル
	SPI_ERROR_UNKNOWNFORMAT,     //  2:未知のファイル形式
	SPI_ERROR_BROKENDATA,        //  3:データが壊れてる
	SPI_ERROR_MEMALLOC,          //  4:メモリーが確保できない
	SPI_ERROR_MEMORY,            //  5:メモリー関連のエラー
	SPI_ERROR_FILEREAD,          //  6:ファイル読み込みエラー
	SPI_ERROR_WINDOW,            //  7:窓が開けない (非公開)
	SPI_ERROR_ERROR,             //  8:内部エラー
	SPI_ERROR_FILEWRITE,         //  9:ファイル書き込みエラー (非公開)
	SPI_END_OF_FILE,             // 10:ファイル終端 (非公開)
};

// 入力形式
typedef enum SPI_INPUT_TYPE {
	SPI_INPUT_DISKFILE = 0,  // ディスクファイル
	SPI_INPUT_MEMORY   = 1,  // メモリ上のイメージ
};

// 出力形式
typedef enum SPI_OUTPUT_TYPE {
	SPI_OUTPUT_DISKFILE = 0,
	SPI_OUTPUT_MEMORY   = 0x0100,
};


class plugin
{
	friend class loader;

protected:

	HMODULE					m_module;
	SPI_TYPE				m_type;
	SPI_GETPLUGININFO		m_pGetPluginInfo;
	SPI_ISSUPPORTED			m_pIsSupported;
	SPI_CONFIGURATIONDLG	m_pConfigurationDlg;

	plugin();

	plugin(HMODULE module)
		: m_module(module)
		, m_type(SPI_TYPE_UNKNOWN)
	{
		const TCHAR SPI_00IN[] = _T("00IN"); //Normal - Import filter
		const TCHAR SPI_00AM[] = _T("00AM"); //Multi-picture - Archive extractor

		m_pGetPluginInfo    = (SPI_GETPLUGININFO)   ::GetProcAddress(module, SPI_FUNC_GETPLUGININFO);
		m_pIsSupported      = (SPI_ISSUPPORTED)     ::GetProcAddress(module, SPI_FUNC_ISSUPPORTED);
		m_pConfigurationDlg = (SPI_CONFIGURATIONDLG)::GetProcAddress(module, SPI_FUNC_CONFIGURATIONDLG);

		//プラグインバージョンをチェック
		TCHAR szBuff[256];
		if( m_pGetPluginInfo &&
			m_pGetPluginInfo(0, szBuff, sizeof(szBuff)/sizeof(szBuff[0]) ) )
		{
			m_type = !lstrcmpi(szBuff, SPI_00IN) ? SPI_TYPE_00IN : m_type;
			m_type = !lstrcmpi(szBuff, SPI_00AM) ? SPI_TYPE_00AM : m_type;
		}
	}

public:

	SPI_TYPE GetPluginVersion() const
	{
		return m_type;
	}

	int GetPluginInfo(int infono, LPSTR buf,int buflen)
	{
		return m_pGetPluginInfo(infono, buf, buflen);
	}

	int IsSupported(LPSTR filename, DWORD dw)
	{
		return m_pIsSupported(filename, dw);
	}

	SPI_ERROR_TYPE ConfigurationDlg(HWND parent, int fnc)
	{
		return !m_pConfigurationDlg ? SPI_ERROR_NOTIMPLEMENT
		                            : (SPI_ERROR_TYPE)m_pConfigurationDlg(parent, fnc);
	}

	operator HMODULE () const
	{
		return m_module;
	}
};

class picture
	: public plugin
{
	friend class loader;

	SPI_GETPICTUREINFO		m_pGetPictureInfo;
	SPI_GETPICTURE			m_pGetPicture;
	SPI_GETPREVIEW			m_pGetPreview;

	picture();

protected:

	picture(HMODULE module)
		: plugin(module)
	{
		m_pGetPictureInfo= (SPI_GETPICTUREINFO) ::GetProcAddress(module, SPI_FUNC_GETPICTUREINFO);
		m_pGetPicture    = (SPI_GETPICTURE)     ::GetProcAddress(module, SPI_FUNC_GETPICTURE);
		m_pGetPreview    = (SPI_GETPREVIEW)     ::GetProcAddress(module, SPI_FUNC_GETPREVIEW);
	}

public:

	SPI_ERROR_TYPE GetPictureInfo(LPSTR buf,long len,unsigned int flag, PICTUREINFO *lpInfo)
	{
		return !m_pGetPictureInfo ? SPI_ERROR_NOTIMPLEMENT
		                          : (SPI_ERROR_TYPE)m_pGetPictureInfo(buf, len, flag, lpInfo);
	}

	SPI_ERROR_TYPE GetPicture(LPSTR buf,long len,unsigned int flag, HANDLE *pHBInfo, HANDLE *pHBm,
	                          PROGRESS lpPrgressCallback,long lData )
	{
		return !m_pGetPicture ? SPI_ERROR_NOTIMPLEMENT
		                      : (SPI_ERROR_TYPE)m_pGetPicture(buf, len, flag, pHBInfo, pHBm, lpPrgressCallback, lData);
	}

	SPI_ERROR_TYPE GetPreview(LPSTR buf,long len,unsigned int flag, HANDLE *pHBInfo,HANDLE *pHBm,
	                          PROGRESS lpPrgressCallback,long lData )
	{
		return !m_pGetPreview ? SPI_ERROR_NOTIMPLEMENT
		                      : (SPI_ERROR_TYPE)m_pGetPreview(buf, len, flag, pHBInfo, pHBm, lpPrgressCallback, lData);
	}

};

class archive
	: public plugin
{
	friend class loader;

	SPI_GETARCHIVEINFO		m_pGetArchiveInfo;
	SPI_GETFILEINFO			m_pGetFileInfo;
	SPI_GETFILE				m_pGetFile;

	archive();

protected:

	archive(HMODULE module)
		: plugin(module)
	{
		m_pGetArchiveInfo= (SPI_GETARCHIVEINFO) ::GetProcAddress(module, SPI_FUNC_GETARCHIVEINFO);
		m_pGetFileInfo   = (SPI_GETFILEINFO)    ::GetProcAddress(module, SPI_FUNC_GETFILEINFO);
		m_pGetFile       = (SPI_GETFILE)        ::GetProcAddress(module, SPI_FUNC_GETFILE);
	}

public:

	SPI_ERROR_TYPE GetArchiveInfo(LPSTR buf,long len, unsigned int flag, HLOCAL *lphInf)
	{
		return !m_pGetArchiveInfo ? SPI_ERROR_NOTIMPLEMENT
		                          : (SPI_ERROR_TYPE)m_pGetArchiveInfo(buf, len, flag, lphInf);
	}

	SPI_ERROR_TYPE GetFileInfo(LPSTR buf, long len, LPSTR filename, unsigned int flag, FILEINFO *lpInfo)
	{
		return !m_pGetFileInfo ? SPI_ERROR_NOTIMPLEMENT
		                       : (SPI_ERROR_TYPE)m_pGetFileInfo(buf, len, filename, flag, lpInfo);
	}

	SPI_ERROR_TYPE GetFile(LPSTR src, long len, LPSTR dest, unsigned int flag, PROGRESS prgressCallback, long lData)
	{
		return !m_pGetFile ? SPI_ERROR_NOTIMPLEMENT
		                   : (SPI_ERROR_TYPE)m_pGetFile(src, len, dest, flag, prgressCallback, lData);
	}

};

class loader
{
	TCHAR		m_szSusiePath[MAX_PATH * 2 + 1];
	TCHAR		m_szExePath[MAX_PATH * 2 + 1];

	plugin**	m_hPlugins;
	int			m_nPluginNum;
	int			m_nPluginAlloc;

public:

	loader(int nFlags = SPI_FIND_ALL)
		: m_hPlugins(new plugin*[1])
		, m_nPluginNum(0)
		, m_nPluginAlloc(0)
	{
		Init();
		if( nFlags ) {
			Load(nFlags);
		}
	}
	
	~loader()
	{
		Free();
	}

	bool Load(int nFlags = SPI_FIND_ALL);

	plugin * IsSupported(LPCSTR lpFileName, DWORD dwHandle, bool *bMacBinary = NULL);

private:

	loader( const loader & );
	loader & operator = (const loader &);

	bool Init();
	void Free();

	bool LoadSPI(LPCTSTR lpszPath);

};

inline
void loader::Free()
{
	for(int i = 0; i < m_nPluginNum; i++) {
		::FreeLibrary( *m_hPlugins[i] );
		delete m_hPlugins[i];
	}
	delete [] m_hPlugins;
	m_hPlugins   = NULL;
	m_nPluginNum = m_nPluginAlloc = 0;
}

inline
bool loader::LoadSPI(LPCTSTR lpszPath)
{
	const TCHAR SPI_FINDMASK[]  = _T("*.spi");

	WIN32_FIND_DATA fd;
	TCHAR  szPath[MAX_PATH];
	HANDLE hFind = NULL;

	::_stprintf(szPath, _T("%s\\%s"), lpszPath, SPI_FINDMASK);

	hFind = ::FindFirstFile(szPath, &fd);
	if( INVALID_HANDLE_VALUE == hFind ) {
		return false;
	}
	do {
		if(0 == fd.nFileSizeHigh && 0 < fd.nFileSizeLow) {
			::_stprintf(szPath, _T("%s\\%s"), lpszPath, fd.cFileName);
			plugin inst( ::LoadLibrary(szPath) );
			if( NULL != inst ) {
				//プラグインバージョンをチェック
				if( SPI_TYPE_00IN == inst.GetPluginVersion() ||
				    SPI_TYPE_00AM == inst.GetPluginVersion() )
				{
					if( m_nPluginAlloc <= m_nPluginNum + 1 )
					{
						int nPluginAlloc = m_nPluginAlloc + 10;
						plugin **hPlugins = new plugin*[nPluginAlloc];
						::memcpy(hPlugins, m_hPlugins, sizeof(plugin*) * m_nPluginNum);
						delete [] m_hPlugins;
						m_hPlugins = hPlugins;
						m_nPluginAlloc = nPluginAlloc;
					}
					// 種別で生成クラスを分ける
					switch( inst.GetPluginVersion() )
					{
					case SPI_TYPE_00IN:
						m_hPlugins[m_nPluginNum] = new picture(inst);
						break;
					case SPI_TYPE_00AM:
						m_hPlugins[m_nPluginNum] = new archive(inst);
						break;
					default:;
					}
					m_nPluginNum++;
				}
				else ::FreeLibrary( inst );
			}
		}
	} while( ::FindNextFile(hFind, &fd) );
	::FindClose(hFind);
	
	return true;
}

inline
bool loader::Load(int nFlags)
{
	const SPI_FIND_TYPE eFindTarget[]
		= { SPI_FIND_SUSIE, SPI_FIND_EXE, SPI_FIND_PATH_ENV, SPI_FIND_ALL };

	TCHAR  szFolders[MAX_PATH];
	LPTSTR lpszFolders = NULL;

	Free();
	m_hPlugins = new plugin*[1];

	if( !nFlags ) {
		nFlags = SPI_FIND_EXE;
	}

//プラグインを登録
	for(int i = 0; SPI_FIND_ALL != eFindTarget[i]; i++)
	{
		if( !(nFlags & eFindTarget[i]) ) {
			continue;
		}

		switch( eFindTarget[i] ) {
			case SPI_FIND_EXE: //実行ファイルと同じフォルダ
				::memset(szFolders, 0, sizeof(szFolders));
				::_stprintf(szFolders, _T("%s"), m_szExePath);
				lpszFolders = szFolders;
				break;
			case SPI_FIND_SUSIE: //Susieと同じフォルダ
				::memset(szFolders, 0, sizeof(szFolders));
				::_stprintf(szFolders, _T("%s"), m_szSusiePath);
				lpszFolders = szFolders;
				break;
			case SPI_FIND_PATH_ENV: { //環境変数PATH
//				::GetEnvironmentVariable();
continue;
				break; }
			default:
				continue;
		}

		for(LPTSTR lpszFolder = lpszFolders; *lpszFolder;
			lpszFolder = lpszFolder + ::_tcslen(lpszFolder) + 1)
		{
			LoadSPI(lpszFolder);
		}
	}
//wsprintf(cFindPath, TEXT("%i/%i"), nSpiFileCount, nFileCount);
//MessageBox(NULL, cFindPath, "", 0);

	return TRUE;
}

inline
plugin * loader::IsSupported(LPCSTR lpFileName, DWORD dwHandle, bool *bMacBinary)
{
	DWORD  dwHandleMac = 0;

	if( !m_nPluginNum ) {
		return NULL;
	}

	if( bMacBinary ) {
		*bMacBinary = false;
	}

	if( dwHandle & 0xFFFF0000 ) { // For MacBinary
		dwHandleMac = dwHandle + 128;
	} // その他の場合はファイルハンドル

	for(int i = 0; i < m_nPluginNum; i++) {
		if( m_hPlugins[i]->IsSupported((LPSTR)lpFileName, dwHandle) )
		{
			return m_hPlugins[i];
		}
		else if( 0 < dwHandleMac &&
		         m_hPlugins[i]->IsSupported((LPSTR)lpFileName, dwHandleMac) )
		{
			if( bMacBinary ) {
				*bMacBinary = true;
			}
			return m_hPlugins[i];
		}
	}

	return NULL;
}

inline
bool loader::Init()
{
	const TCHAR REG_SPIPATH_KEY[]  = _T("Software\\Takechin\\Susie\\Plug-in");
	const TCHAR REG_SPIPATH_NAME[] = _T("Path");

	HKEY  hRegAccess = NULL;
	LONG  lResult = 0;
	DWORD dwSize;
	TCHAR *p,*pa;

//レジストリからSusie PlugInの場所を読み込む
	lResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, REG_SPIPATH_KEY, 0,
	                         KEY_QUERY_VALUE, &hRegAccess);
	if( ERROR_SUCCESS != lResult ) {
		return FALSE;
	}

	dwSize  = sizeof(m_szSusiePath) / sizeof(m_szSusiePath[0]) - 1;
	lResult = ::RegQueryValueEx(hRegAccess, REG_SPIPATH_NAME, NULL, NULL,
	                            (LPBYTE)m_szSusiePath, &dwSize);

	::RegCloseKey(hRegAccess);

//内部変数初期化

	::GetModuleFileName(NULL, m_szExePath,
	                    (DWORD)(sizeof(m_szExePath) / sizeof(m_szExePath[0]) - 1));
	//ファイル名を削除
	for(p = pa = m_szExePath; *p; p = ::CharNext(p)) {
		if( '\\' == *p )
			{ pa = p; }
	}
	*pa = '\0';

	return true;
}

#if 0

// 画像展開
typedef int (SPIEXPORT *SPI_GETPICTUREINFO)(LPSTR, long, unsigned int, PICTUREINFO *);
typedef int (SPIEXPORT *SPI_GETPICTURE)(LPSTR, long, unsigned int, HANDLE *, HANDLE *, PROGRESS , long );
typedef int (SPIEXPORT *SPI_GETPREVIEW)(LPSTR, long, unsigned int, HANDLE *, HANDLE *, PROGRESS, long );
#define SPI_FUNC_GETPICTUREINFO	TEXT("GetPictureInfo")
#define SPI_FUNC_GETPICTURE	TEXT("GetPicture")
#define SPI_FUNC_GETPREVIEW	TEXT("GetPreview")

// アーカイブ抽出
typedef int (SPIEXPORT *SPI_GETARCHIVEINFO)(LPSTR, long, unsigned int, HLOCAL *);
typedef int (SPIEXPORT *SPI_GETFILEINFO)(LPSTR, long, LPSTR, unsigned int, FILEINFO *);
typedef int (SPIEXPORT *SPI_GETFILE)(LPSTR, long, LPSTR, unsigned int, PROGRESS, long );
#define SPI_FUNC_GETARCHIVEINFO	TEXT("GetArchiveInfo")
#define SPI_FUNC_GETFILEINFO	TEXT("GetFileInfo")
#define SPI_FUNC_GETFILE	TEXT("GetFile")
#endif

} // namespace spi

} // namespace spplib

#endif // !defined(SPPLIB_6AB990F7_853E_4A4E_99ED_EA328739648C_INCLUDE_GUARD)
