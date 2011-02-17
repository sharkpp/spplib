#include "ini.h"
//#include "Macros.h"
//#include "HeapMem.h"

#pragma warning( disable : 4996 )

namespace spplib
{
 namespace file
 {

//--------------------------------------------------------------------
// �萔
//--------------------------------------------------------------------

#define INI_LINE_TYPE_KEY			0x00	// �L�[
#define INI_LINE_TYPE_SECTION		0x01	// �Z�N�V����
#define INI_LINE_TYPE_COMMENT		0x02	// �R�����g�s(���̂܂܏����o��)
#define INI_LINE_TYPE_DELETE		0x04	// �폜�\��
#define INI_LINE_TYPE_MEMALLOC		0x80	// ���������m�ۍς�
#define INI_LINE_TYPE_MEMALLOC_VAL	0x40	// ���������m�ۍς�(�l)

//--------------------------------------------------------------------
// �R���X�g���N�V����/�f�X�g���N�V����
//--------------------------------------------------------------------

ini::ini()
{
	memset(&m_szSectionName, 0, sizeof(m_szSectionName));
	memset(&m_szFileName, 0, sizeof(m_szFileName));

	m_nSelectSectionLine = 0;

	m_nLineCount	= 0;
	m_ptLineInfo	= NULL;

	m_dwFileMem		= 0;
	m_pszFileMem	= NULL;

	m_bWrite		= false;
}

ini::~ini()
{
	close();
}

//--------------------------------------------------------------------
// �����o�֐�
//--------------------------------------------------------------------

bool ini::open(LPCTSTR lpszFileName)
{
	lstrcpyn(m_szFileName, lpszFileName, _countof(m_szFileName));
	m_bWrite = false;
	return ReadFileCore(m_szFileName);
}

bool ini::close()
{
	int	nLine;

	WriteFileCore(m_szFileName);

	if(m_ptLineInfo)
	{
		for(nLine = 0; nLine < m_nLineCount; nLine++)
		{
			if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_MEMALLOC) {
				delete [] m_ptLineInfo[nLine].pszLine;
			}
			if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_MEMALLOC_VAL) {
				delete [] m_ptLineInfo[nLine].pszValue;
			}
		}
		delete [] m_ptLineInfo;
	}

	delete [] m_pszFileMem;

	*m_szSectionName= '\0';
	*m_szFileName	= '\0';
	return true;
}

bool ini::ReadFileCore(LPCTSTR lpszFileName)
{
	HANDLE	hFile;
	TCHAR *	pa;
	TCHAR *	pb;
	DWORD	dwRead;
	int		nLine;

	m_nLineCount	= 0;
	m_nSelectSectionLine = 0;
	m_bWrite		= false;

	// �t�@�C���̓Ǎ�
	hFile = ::CreateFile(
				  lpszFileName, GENERIC_READ, FILE_SHARE_READ
				, NULL, OPEN_ALWAYS, 0, NULL);
	if(INVALID_HANDLE_VALUE == hFile) {
		return false;
	}
	m_dwFileMem		= ::GetFileSize(hFile, NULL);
	m_pszFileMem	= new TCHAR[m_dwFileMem + 1];
	::ReadFile(hFile, m_pszFileMem, m_dwFileMem, &dwRead, NULL);
	if(hFile) { CloseHandle(hFile), hFile = NULL; }
	// �s�����擾
	pa = m_pszFileMem;
	for(nLine = 0; *pa; pa = ::CharNext(pa))
	{
		if('\n' != *pa && '\r' != *pa) {
			continue;
		}
		nLine++;
		for(; *pa && ('\n' == *pa || '\r' == *pa); pa = ::CharNext(pa));
	}
	// �s�����
	m_nLineCount	= nLine + 1;
	m_ptLineInfo	= new INI_LINE_INFO[m_nLineCount];
	pa = pb = m_pszFileMem;
	m_ptLineInfo[0].wType	= INI_LINE_TYPE_SECTION;
	for(nLine = 1; *pa; pa = ::CharNext(pa))
	{
		if('\n' != *pa && '\r' != *pa) {
			continue;
		}

		m_ptLineInfo[nLine].wType	= INI_LINE_TYPE_KEY;
		m_ptLineInfo[nLine].pszLine = pb;
		// �s�����擾
		for(; *pb && (' ' == *pb || '\t' == *pb); pb = ::CharNext(pb));
		// �R�����g�s��
		m_ptLineInfo[nLine].wType |=
			('#' == *pb || ';' == *pb) ? INI_LINE_TYPE_COMMENT : 0;
		// �Z�N�V�������H
		m_ptLineInfo[nLine].wType |=
			('[' == *pb) ? INI_LINE_TYPE_SECTION : 0;

		if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_SECTION)
		{
			m_ptLineInfo[nLine].pszLine = ::CharNext(m_ptLineInfo[nLine].pszLine);

			for(; *pb && ']' != *pb && '\n' != *pb && '\r' != *pb; pb = ::CharNext(pb));
			*pb = '\0';
		}
		else if(!(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_COMMENT))
		{	// *pb �̓L�[�̍ŏ��̕����������Ă���
			for(; *pb && '=' != *pb && '\n' != *pb && '\r' != *pb; pb = ::CharNext(pb));
			m_ptLineInfo[nLine].pszValue = ::CharNext(pb);
			if('=' != *pb) {
				*(m_ptLineInfo[nLine].pszValue) = '\0';
			}
			*pb = '\0';
		}

		// ���̍s�ֈړ�
		pb = pa;
		for(; *pa && ('\n' == *pa || '\r' == *pa); pa = ::CharNext(pa));
		*pb = '\0';
		pb = pa;

		nLine++;
	}

//#ifdef _DEBUG // DllMain() ����Ăяo���ƃA�N�Z�X�ᔽ�𐶂�������
//for(nLine = 0; nLine < m_nLineCount; nLine++) {
//	printf("%02X:'%s'='%s'\n", m_ptLineInfo[nLine].wType, m_ptLineInfo[nLine].pszLine, m_ptLineInfo[nLine].pszValue);
//}
//printf("-----------\n");
//#endif

	return true;
}

bool ini::WriteFileCore(LPCTSTR lpszFileName)
{
	HANDLE	hFile;
	DWORD	dwRead;
	int		nLine;

	// �����o���K�v�Ȃ�
	if(!m_bWrite) {
		return true;
	}

	// �t�@�C���̓Ǎ�
	hFile = ::CreateFile(
				  lpszFileName, GENERIC_WRITE, 0
				, NULL, CREATE_ALWAYS, 0, NULL);
	if(INVALID_HANDLE_VALUE == hFile) {
		return false;
	}
	m_dwFileMem		= ::GetFileSize(hFile, NULL);
	m_pszFileMem	= new TCHAR[m_dwFileMem + 1];

#define WriteStringFile(h,s)	if(s){::WriteFile(h,s,lstrlen(s)*sizeof(TCHAR),&dwRead,NULL);}
#pragma warning(push)
#pragma warning(disable: 4127)	// C4127: ���������萔�ł��B

	for(nLine = 0; nLine < m_nLineCount; nLine++)
	{
		if(!m_ptLineInfo[nLine].pszLine) {
			continue;
		}

		if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_SECTION) {
		//	m_ptLineInfo[nLine].wType &= ~INI_LINE_TYPE_DELETE;
		}

		if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_DELETE) {
			continue;
		}

		if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_SECTION) {
			WriteStringFile(hFile, _T("["));
		}

		WriteStringFile(hFile, m_ptLineInfo[nLine].pszLine);

		if(!(m_ptLineInfo[nLine].wType & (INI_LINE_TYPE_SECTION|INI_LINE_TYPE_COMMENT))) {
			WriteStringFile(hFile, _T("="));
			WriteStringFile(hFile, m_ptLineInfo[nLine].pszValue);
		}

		if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_SECTION) {
			WriteStringFile(hFile, _T("]"));
		}

		WriteStringFile(hFile, _T("\r\n"));
	}

#pragma warning(pop)
#undef WriteStringFile

#ifdef _DEBUG
for(nLine = 0; nLine < m_nLineCount; nLine++) {
	printf("%02X:'%s'='%s'\n", m_ptLineInfo[nLine].wType, m_ptLineInfo[nLine].pszLine, m_ptLineInfo[nLine].pszValue);
}
printf("-----------\n");
#endif

	if(hFile) { CloseHandle(hFile), hFile = NULL; }

	return true;
}

int ini::GetSectionLineCore(LPCTSTR lpszSection)
{
	int nLine;

	if(!lstrcmp(lpszSection, _T(""))) {
		return 0;
	}

	for(nLine = 0; nLine < m_nLineCount; nLine++)
	{
		if(!(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_SECTION)) {
			continue;
		}
		if(!lstrcmp(lpszSection, m_ptLineInfo[nLine].pszLine)) {
			return nLine;
		}
	}
	return -1;
}

bool ini::MarkDeleteSectionCore(int nLine)
{
	if(nLine < 0) {
		return false;
	}
	if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_DELETE) {
		return false;
	}
	// �ēx�}�[�N���Ȃ��悤�ɃZ�N�V�����Ƀ}�[�N��t����
	m_ptLineInfo[nLine].wType |= INI_LINE_TYPE_DELETE;

	// �擪(�Z�N�V��������)�ȊO�̓Z�N�V�����������Ă���̂Ŏ��̍s�ֈړ�
	if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_SECTION) {
		nLine++;
	}

	for(; nLine < m_nLineCount; nLine++)
	{
		if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_SECTION) {
			break;
		}
		if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_COMMENT) {
			continue;
		}
		m_ptLineInfo[nLine].wType |= INI_LINE_TYPE_DELETE;
	}
	return true;
}

void ini::MarkDelete()
{
	int	nLine;

	if(m_bWrite) {
		return;
	}
	m_bWrite = true;

	for(nLine = 0; nLine < m_nLineCount; nLine++)
	{
		if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_SECTION) {
//			break;
		}
		if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_COMMENT) {
			continue;
		}
		m_ptLineInfo[nLine].wType |= INI_LINE_TYPE_DELETE;
	}
}

void ini::selectsection(LPCTSTR lpszSectionName)
{
	lstrcpyn(m_szSectionName, lpszSectionName, _countof(m_szSectionName));
	m_nSelectSectionLine = GetSectionLineCore(m_szSectionName);
}

bool ini::GetCore(LPCTSTR lpszKeyName, LPTSTR lpszBuffer, DWORD dwLen, LPTSTR lpszDefault)
{
	int	nLine;

	nLine = m_nSelectSectionLine;

	if(nLine < 0) {
		goto abort;
	}

	// �擪(�Z�N�V��������)�ȊO�̓Z�N�V�����������Ă���̂Ŏ��̍s�ֈړ�
	if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_SECTION) {
		nLine++;
	}

	for(; nLine < m_nLineCount; nLine++)
	{
		if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_SECTION) {
			break;
		}
		if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_COMMENT) {
			continue;
		}
		if(lstrcmpi(m_ptLineInfo[nLine].pszLine, lpszKeyName)) {
			continue;
		}
		if(!m_ptLineInfo[nLine].pszValue) {
			continue;
		}
		lstrcpyn(lpszBuffer, m_ptLineInfo[nLine].pszValue, dwLen);
		return true;
	}

abort:
	lstrcpyn(lpszBuffer, lpszDefault, dwLen);
	return false;
}

bool ini::PutCore(LPCTSTR lpszKeyName, LPCTSTR lpszValue)
{
	LPINI_LINE_INFO	ptLineInfo;
	int		nLineCount, nLine, nLen, nAdd;

#ifdef _DEBUG
//for(int n = 0; n < m_nLineCount; n++) {
//	printf("%02X:'%s'='%s'\n", m_ptLineInfo[n].wType, m_ptLineInfo[n].pszLine, m_ptLineInfo[n].pszValue);
//}
//printf("-----------\n");
#endif

	MarkDelete();

	nLine = m_nSelectSectionLine;

	if(nLine < 0) {
		nAdd	= 2;
		nLine	= m_nLineCount;
		m_nSelectSectionLine = nLine;
		goto add_key;
	}

//	MarkDeleteSectionCore(nLine);

	// �擪(�Z�N�V��������)�ȊO�̓Z�N�V�����������Ă���̂Ŏ��̍s�ֈړ�
	if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_SECTION) {
		m_ptLineInfo[nLine].wType &= ~INI_LINE_TYPE_DELETE;
		nLine++;
	}

	for(; nLine < m_nLineCount; nLine++)
	{
		if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_SECTION) {
			break;
		}
		if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_COMMENT) {
			continue;
		}
		if(lstrcmpi(m_ptLineInfo[nLine].pszLine, lpszKeyName)) {
			continue;
		}
		if(!m_ptLineInfo[nLine].pszValue) {
			continue;
		}
		goto put_key;
	}
	nAdd = 1;

add_key:

	// �g��
	nLineCount = m_nLineCount + nAdd;
	ptLineInfo = new INI_LINE_INFO[nLineCount];
	// �}������ʒu���O���R�s�[
	::memcpy(ptLineInfo, m_ptLineInfo, nLine * sizeof(INI_LINE_INFO));
	// �}������ʒu������R�s�[
	::memcpy(ptLineInfo + nLine + 1, m_ptLineInfo + nLine, (m_nLineCount - nLine) * sizeof(INI_LINE_INFO));
	delete [] m_ptLineInfo;
	m_ptLineInfo = ptLineInfo;
	m_nLineCount = nLineCount;

	if(1 < nAdd)
	{ // �Z�N�V�����쐬
		// �������m��
		nLen = lstrlen(m_szSectionName);
		m_ptLineInfo[nLine].pszLine = new TCHAR[nLen + 1];
		// �l�Z�b�g
		lstrcpyn(m_ptLineInfo[nLine].pszLine, m_szSectionName, nLen + 1);
		m_ptLineInfo[nLine].wType = INI_LINE_TYPE_MEMALLOC | /*INI_LINE_TYPE_DELETE |*/ INI_LINE_TYPE_SECTION;
		nLine++;
	}

	// �L�[�������[���郁�����̊m��
	nLen = lstrlen(lpszKeyName);
	m_ptLineInfo[nLine].pszLine = new TCHAR[nLen + 1];
	lstrcpyn(m_ptLineInfo[nLine].pszLine, lpszKeyName, nLen + 1);
	m_ptLineInfo[nLine].wType = INI_LINE_TYPE_MEMALLOC;

put_key:
// �L�[�̏o��
	// ���łɃ������m�ۍς݂Ȃ烁�������J��
	if(m_ptLineInfo[nLine].wType & INI_LINE_TYPE_MEMALLOC_VAL) {
		delete [] m_ptLineInfo[nLine].pszValue;
	}
	// �������m��
	nLen = lstrlen(lpszValue);
	m_ptLineInfo[nLine].pszValue = new TCHAR[nLen + 1];
	// �l�Z�b�g
	lstrcpyn(m_ptLineInfo[nLine].pszValue, lpszValue, nLen + 1);
	m_ptLineInfo[nLine].wType |= INI_LINE_TYPE_MEMALLOC_VAL;
	m_ptLineInfo[nLine].wType &= ~INI_LINE_TYPE_DELETE;

#ifdef _DEBUG
for(int n = 0; n < m_nLineCount; n++) {
	printf("%02X:'%s'='%s'\n", m_ptLineInfo[n].wType, m_ptLineInfo[n].pszLine, m_ptLineInfo[n].pszValue);
}
printf("-----------\n");
#endif

	return true;
}

bool ini::get(LPCTSTR lpszKeyName, LPTSTR lpszBuffer, DWORD dwLen, LPTSTR lpszDefault)
{
	return GetCore(lpszKeyName, lpszBuffer, dwLen, lpszDefault);
}

bool ini::put(LPCTSTR lpszKeyName, LPCTSTR lpszValue)
{
	return PutCore(lpszKeyName, lpszValue);
}

bool ini::get(LPCTSTR lpszKeyName, std::string & strBuffer, LPTSTR lpszDefault)
{
	bool result;
	strBuffer.resize(1024);
	result = GetCore(lpszKeyName, (LPTSTR)strBuffer.data(), 1024, lpszDefault);
	std::string::size_type null_pos = strBuffer.find((std::string::value_type)0, 0);
	if( null_pos != std::string::npos && null_pos != strBuffer.size() ) {
		strBuffer.erase(null_pos);
	}
	return result;
}

bool ini::put(LPCTSTR lpszKeyName, const std::string & strValue)
{
	return PutCore(lpszKeyName, (LPCTSTR)strValue.c_str());
}

bool ini::get(LPCTSTR lpszKeyName, int & nBuffer, int nDefault)
{
	TCHAR	szBuffer[64], szBuffer2[64];
	bool	bResult;
	nBuffer = nDefault;
	_stprintf(szBuffer2, _T("%d"), nDefault);
	bResult = GetCore(
				lpszKeyName,
				szBuffer, (DWORD)_countof(szBuffer) - 1,
				szBuffer2);
	_stscanf(szBuffer, _T("%d"), &nBuffer);
	return bResult;
}

bool ini::put(LPCTSTR lpszKeyName, int nValue)
{
	TCHAR	szBuffer[64];
	_stprintf(szBuffer, _T("%d"), nValue);
	return PutCore(lpszKeyName, szBuffer);
}

bool ini::get(LPCTSTR lpszKeyName, DWORD & dwBuffer, DWORD dwDefault)
{
	TCHAR	szBuffer[64], szBuffer2[64];
	bool	bResult;
	dwBuffer = dwDefault;
	_stprintf(szBuffer2, _T("%d"), dwDefault);
	bResult = GetCore(
				lpszKeyName,
				szBuffer, (DWORD)_countof(szBuffer) - 1,
				szBuffer2);
	_stscanf(szBuffer, _T("%d"), &dwBuffer);
	return bResult;
}

bool ini::put(LPCTSTR lpszKeyName, DWORD dwValue)
{
	TCHAR	szBuffer[64];
	_stprintf(szBuffer, _T("%d"), dwValue);
	return PutCore(lpszKeyName, szBuffer);
}

bool ini::get(LPCTSTR lpszKeyName, WORD & wBuffer, WORD wDefault)
{
	DWORD	dwValue;
	bool	bResult;
	bResult = get(
				lpszKeyName,
				dwValue, (DWORD)wDefault);
	wBuffer = (WORD)dwValue;
	return bResult;
}

bool ini::put(LPCTSTR lpszKeyName, WORD wValue)
{
	return put(lpszKeyName, (DWORD)wValue);
}

bool ini::get(LPCTSTR lpszKeyName, bool & bBuffer, bool bDefault)
{
	TCHAR	szBuffer[64];
	bool	bResult;
	bResult = GetCore(
				lpszKeyName,
				szBuffer, (DWORD)_countof(szBuffer) - 1,
				bDefault ? _T("true") : _T("false"));
	if(!lstrcmpi(_T("true"), szBuffer) || !lstrcmpi(_T("1"), szBuffer)) {
		bBuffer = true;
	} else {
		bBuffer = false;
	}
	return bResult;
}

bool ini::put(LPCTSTR lpszKeyName, bool bValue)
{
	return PutCore(lpszKeyName, bValue ? _T("true") : _T("false"));
}

 } // namespace string
} // namespace spplib

// EOF