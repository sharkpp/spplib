/**
 * @file  spplib/fs/directory_iterator.hpp
 * @brief �f�B���N�g���񋓃N���X ��`�E����
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010.
 */

#include <windows.h>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_462440FA_E6FA_4860_9F32_3CA0192BB758_INCLUDE_GUARD
#define SPPLIB_462440FA_E6FA_4860_9F32_3CA0192BB758_INCLUDE_GUARD

/// sharkpp class library namespace
namespace spplib {
 /// filesysyem namespace
 namespace fs {

/**
 * @brief �f�B���N�g���񋓃N���X
 * @code
 *   for(spplib::fs::directory_iterator ite("*.*"), end; 
 *       ite != end; ++ite) {
 *     printf("%s %s\r\n", ite.is_dir() ? "d" : "f", ite.filename());
 *   }
 * @endcode
 */
class directory_iterator
{
#if defined(_UNICODE)
	static const size_t MAX_PATH_LEN = 32767;
#else
	static const size_t MAX_PATH_LEN = MAX_PATH * 2;
#endif

	WIN32_FIND_DATA	m_fd;
	HANDLE			m_handle;
	bool			m_next_exist;
	mutable bool	m_full_path_valid;
	mutable TCHAR*	m_full_path;
	TCHAR*			m_path;
	TCHAR*			m_dirname;

	directory_iterator(const directory_iterator & );
	directory_iterator & operator = (const directory_iterator & );

public:

	/// �R���X�g���N�^
	directory_iterator();

	/** 
	 * @brief �R���X�g���N�^
	 * @param[in] path �Ώۂ̃p�X
	 */
	directory_iterator(const TCHAR * path);

	/// �f�X�g���N�^
	~directory_iterator();

	/** 
	 * @brief �f�B���N�g�����̎��̃G���g���ֈړ�
	 * @return ���̃G���g���ֈړ���̂��̃I�u�W�F�N�g���̂�Ԃ�
	 */
	directory_iterator & operator ++();

	/** 
	 * @brief �w�肵���C���X�^���X�ƈ�v���Ă��Ȃ����̔�r
	 * @param[in] rhs   ��r�ΏƂ̃C���X�^���X
	 * @retval    true  �s��v
	 * @retval    false ��v
	 * @note ��̃I�u�W�F�N�g�Ƃ̔�r�ɂ����Ӗ��������܂���B
	 */
	bool operator != (const directory_iterator & rhs) const;

	/** 
	 * @brief �w�肵���p�X������
	 * @param[in] path  �Ώۂ̃p�X
	 * @retval    true  ��������
	 * @retval    false �������s
	 */
	bool find(const TCHAR * path);

	/** 
	 * @brief �f�B���N�g�����̐擪���ڂ֖߂�
	 * @retval    true  ��������
	 * @retval    false �������s
	 */
	bool rewind();

	/** 
	 * @brief ���݂̃G���g�����f�B���N�g�����ǂ����𒲂ׂ�
	 * @retval    true  ���݂̃G���g�����f�B���N�g��
	 * @retval    false ���݂̃G���g�����f�B���N�g���ȊO
	 */
	bool is_dir() const;

	/** 
	 * @brief ���݂̃G���g���̃t�@�C�������擾
	 * @return ���݂̃G���g���̃t�@�C����
	 */
	const TCHAR * filename() const;

	/** 
	 * @brief ���݂̃G���g���̃t���p�X���擾
	 * @return ���݂̃G���g���̃t���p�X
	 */
	const TCHAR * fullpath() const;

	/** 
	 * @brief ���݂̃G���g���̃T�C�Y���擾
	 * @retval  0�ȏ� �t�@�C���T�C�Y
	 * @retval -1     �t�@�C���T�C�Y�̎擾�Ɏ��s�B�T�C�Y��4GB�ȏ�ȂǁB
	 */
	DWORD size() const;

	/** 
	 * @brief ���݂̃G���g���̃T�C�Y���擾
	 * @retval  0�ȏ� �t�@�C���T�C�Y
	 * @retval -1     �t�@�C���T�C�Y�̎擾�Ɏ��s�B�T�C�Y��4GB�ȏ�ȂǁB
	 * @note 4GB�ȏ���擾�\
	 */
	LONGLONG size64() const;

	/** 
	 * @brief ���݂̃G���g���̑������擾
	 * @return ���݂̃G���g���̑���
	 */
	DWORD attribute() const;

	/** 
	 * @brief ���݂̃G���g���̍쐬����
	 * @return �쐬����
	 */
	time_t ctime() const;

	/** 
	 * @brief ���݂̃G���g���̍X�V����
	 * @return �X�V����
	 */
	time_t mtime() const;

	/** 
	 * @brief ���݂̃G���g���̃A�N�Z�X����
	 * @return �A�N�Z�X����
	 */
	time_t atime() const;

};

///////////////////////////////////////////////////////////////////////////////

inline
directory_iterator::directory_iterator()
	: m_handle(INVALID_HANDLE_VALUE)
	, m_next_exist(false)
	, m_full_path_valid(false)
	, m_full_path(new TCHAR[MAX_PATH_LEN + 1])
	, m_path(new TCHAR[MAX_PATH_LEN + 1])
	, m_dirname(new TCHAR[MAX_PATH_LEN + 1])
{
	::memset(m_full_path, 0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
	::memset(m_path,      0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
	::memset(m_dirname,   0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
}

inline
directory_iterator::directory_iterator(const TCHAR * path)
	: m_handle(INVALID_HANDLE_VALUE)
	, m_next_exist(false)
	, m_full_path_valid(false)
	, m_full_path(new TCHAR[MAX_PATH_LEN + 1])
	, m_path(new TCHAR[MAX_PATH_LEN + 1])
	, m_dirname(new TCHAR[MAX_PATH_LEN + 1])
{
	::memset(m_full_path, 0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
	::memset(m_path,      0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
	::memset(m_dirname,   0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));

	find(path);
}

inline
directory_iterator::~directory_iterator()
{
	if( INVALID_HANDLE_VALUE != m_handle )
	{
		::FindClose(m_handle);
	}
	delete [] m_full_path;
	delete [] m_path;
	delete [] m_dirname;
}

inline
directory_iterator & directory_iterator::operator ++()
{
	if( m_next_exist ) {
		m_next_exist = FALSE != ::FindNextFile(m_handle, &m_fd);
		m_full_path_valid = false;
	}
	return *this;
}

bool directory_iterator::operator != (const directory_iterator & rhs) const
{
	return m_next_exist != rhs.m_next_exist;
}

inline
bool directory_iterator::find(const TCHAR * path)
{
	// ���Z�b�g
	if( INVALID_HANDLE_VALUE != m_handle )
	{
		::FindClose(m_handle);
	}
	::memset(m_full_path, 0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
	::memset(m_path,      0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));
	::memset(m_dirname,   0, sizeof(TCHAR) * (MAX_PATH_LEN + 1));

	// �����p�X��ۑ�
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__) // Safer C Library
	if( m_path != path ) {
		_tcsncpy_s(m_path, MAX_PATH_LEN, path, MAX_PATH_LEN);
	}
	_tcsncpy_s(m_dirname, MAX_PATH_LEN, path, MAX_PATH_LEN);
#else
	if( m_path != path ) {
		_tcsncpy(m_path, path, MAX_PATH_LEN);
	}
	_tcsncpy(m_dirname, path, MAX_PATH_LEN);
#endif

	// �f�B���N�g�����擾
#if defined(_UNICODE)
	TCHAR *p = _tcsrchr(m_dirname, '\\');
	if( !p ) { p = m_dirname; }
	else     { ++p; }
#else
	TCHAR *p = m_dirname; // '\\' �I�[
	for(TCHAR *pa = m_dirname; *pa; pa = ::CharNext(pa) ) {
		if( '\\' == *pa ) { p = CharNext(pa); }
	}
#endif
	*p = '\0';
	if( p == m_dirname ) {
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__) // Safer C Library
		_tcsncpy_s(m_dirname, MAX_PATH_LEN, _T(".\\"), MAX_PATH_LEN);
#else
		_tcsncpy(m_dirname, _T(".\\"), MAX_PATH_LEN);
#endif
	}

	m_handle = ::FindFirstFile(m_path, &m_fd);
	m_next_exist = INVALID_HANDLE_VALUE != m_handle;

	return m_next_exist;
}

inline
bool directory_iterator::rewind()
{
	return find(m_path);
}

inline
bool directory_iterator::is_dir() const
{
	return 0 != (m_fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

inline
const TCHAR * directory_iterator::filename() const
{
	return m_fd.cFileName;
}

inline
const TCHAR * directory_iterator::fullpath() const
{
	if( !m_full_path_valid ) {
		m_full_path_valid = true;
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__) // Safer C Library
		_tcsncpy_s(m_full_path, MAX_PATH_LEN, m_dirname,      MAX_PATH_LEN);
		_tcsncat_s(m_full_path, MAX_PATH_LEN, m_fd.cFileName, MAX_PATH_LEN);
#else
		_tcsncpy(m_full_path, m_dirname,      MAX_PATH_LEN);
		_tcsncat(m_full_path, m_fd.cFileName, MAX_PATH_LEN);
#endif
	}
	return m_full_path;
}

inline
DWORD directory_iterator::size() const
{
	return m_fd.nFileSizeLow;
}

inline
LONGLONG directory_iterator::size64() const
{
	return ((LONGLONG)m_fd.nFileSizeHigh << 32) | m_fd.nFileSizeLow;
}

inline
DWORD directory_iterator::attribute() const
{
	return m_fd.dwFileAttributes;
}

inline
time_t directory_iterator::ctime() const
{
	
	return (time_t)(( ((PLARGE_INTEGER)(&m_fd.ftCreationTime))->QuadPart - 116444736000000000) / 10000000);
}

inline
time_t directory_iterator::mtime() const
{
	return (time_t)(( ((PLARGE_INTEGER)(&m_fd.ftLastWriteTime))->QuadPart - 116444736000000000) / 10000000);
}

inline
time_t directory_iterator::atime() const
{
	return (time_t)(( ((PLARGE_INTEGER)(&m_fd.ftLastAccessTime))->QuadPart - 116444736000000000) / 10000000);
}

 } // namespace directory
} // namespace spplib

#endif // !defined(SPPLIB_462440FA_E6FA_4860_9F32_3CA0192BB758_INCLUDE_GUARD)
