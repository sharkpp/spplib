/**
 * @file  spplib/thunk.hpp
 * @brief �T���N�W�F�l���[�^��`�E����
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010-2012.
 */

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_AC7B9E21_2435_4031_AE45_DE16FE059E1B_INCLUDE_GUARD
#define SPPLIB_AC7B9E21_2435_4031_AE45_DE16FE059E1B_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {

class thunk
{
	class func_holder {
		typedef struct void_ {};
		template<typename T>
		struct static_invoker {
		};
		template<typename T, typename R>
		struct static_invoker<R(T::*)()> {
			typedef static_invoker<R(T::*)()> self_;
			static void* this_ptr;
			static R (T::*func_ptr)();
			static R invoke() {
TRACE("%s\n", __FUNCSIG__);
				R (T::*p)() = self_::func_ptr;
				return ((reinterpret_cast<T*>(self_::this_ptr))->*p)();
			}
		};
		template<typename T, typename R, typename A1>
		struct static_invoker<R(T::*)(A1)> {
			typedef static_invoker<R(T::*)(A1)> self_;
			static void* this_ptr;
			static R (T::*func_ptr)(A1);
			static R invoke(A1 a1) {
TRACE("%s\n", __FUNCSIG__);
				R (T::*p)(A1) = self_::func_ptr;
				return ((reinterpret_cast<T*>(self_::this_ptr))->*p)(a1);
			}
		};
		template<typename T, typename R, typename A1, typename A2>
		struct static_invoker<R(T::*)(A1,A2)> {
			typedef static_invoker<R(T::*)(A1,A2)> self_;
			static void* this_ptr;
			static R (T::*func_ptr)(A1,A2);
			static R invoke(A1 a1,A2 a2) {
TRACE("%s\n", __FUNCSIG__);
				R (T::*p)(A1,A2) = self_::func_ptr;
				return ((reinterpret_cast<T*>(self_::this_ptr))->*p)(a1,a2);
			}
		};
		template<typename T, typename R, typename A1, typename A2, typename A3>
		struct static_invoker<R(T::*)(A1,A2,A3)> {
			typedef static_invoker<R(T::*)(A1,A2,A3)> self_;
			static void* this_ptr;
			static R (T::*func_ptr)(A1,A2,A3);
			static R invoke(A1 a1,A2 a2,A3 a3) {
TRACE("%s\n", __FUNCSIG__);
				R (T::*p)(A1,A2,A3) = self_::func_ptr;
				return ((reinterpret_cast<T*>(self_::this_ptr))->*p)(a1,a2,a3);
			}
		};
		template<typename T, typename R, typename A1, typename A2, typename A3, typename A4>
		struct static_invoker<R(T::*)(A1,A2,A3,A4)> {
			typedef static_invoker<R(T::*)(A1,A2,A3,A4)> self_;
			static void* this_ptr;
			static R (T::*func_ptr)(A1,A2,A3,A4);
			static R invoke(A1 a1,A2 a2,A3 a3,A4 a4) {
TRACE("%s\n", __FUNCSIG__);
				R (T::*p)(A1,A2,A3,A4) = self_::func_ptr;
				return ((reinterpret_cast<T*>(self_::this_ptr))->*p)(a1,a2,a3,a4);
			}
		};
		template<typename T, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
		struct static_invoker<R(T::*)(A1,A2,A3,A4,A5)> {
			typedef static_invoker<R(T::*)(A1,A2,A3,A4,A5)> self_;
			static void* this_ptr;
			static R (T::*func_ptr)(A1,A2,A3,A4,A5);
			static R invoke(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) {
TRACE("%s\n", __FUNCSIG__);
				R (T::*p)(A1,A2,A3,A4,A5) = self_::func_ptr;
				return ((reinterpret_cast<T*>(self_::this_ptr))->*p)(a1,a2,a3,a4,a5);
			}
		};
	public:
		void* thunk_ptr;
		void* func_ptr;
		void* func_holder_ptr;
		void* this_holder_ptr;
	public:
		template<typename T, typename R>
		func_holder(R(T::*func)())
			: thunk_ptr      (reinterpret_cast<void *>(&static_invoker<R(T::*)()>::invoke))
			, func_ptr       (*reinterpret_cast<void **>(&func))
			, func_holder_ptr(reinterpret_cast<void *>(&static_invoker<R(T::*)()>::func_ptr))
			, this_holder_ptr(&static_invoker<R(T::*)()>::this_ptr)
		{
TRACE("%s\n", __FUNCSIG__);
		}
		template<typename T, typename R, typename A1>
		func_holder(R(T::*func)(A1))
			: thunk_ptr      (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1)>::invoke))
			, func_ptr       (*reinterpret_cast<void **>(&func))
			, func_holder_ptr(reinterpret_cast<void *>(&static_invoker<R(T::*)(A1)>::func_ptr))
			, this_holder_ptr(&static_invoker<R(T::*)(A1)>::this_ptr)
		{
TRACE("%s\n", __FUNCSIG__);
		}
		template<typename T, typename R, typename A1, typename A2>
		func_holder(R(T::*func)(A1,A2))
			: thunk_ptr      (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2)>::invoke))
			, func_ptr       (*reinterpret_cast<void **>(&func))
			, func_holder_ptr(reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2)>::func_ptr))
			, this_holder_ptr(&static_invoker<R(T::*)(A1,A2)>::this_ptr)
		{
TRACE("%s\n", __FUNCSIG__);
		}
		template<typename T, typename R, typename A1, typename A2, typename A3>
		func_holder(R(T::*func)(A1,A2,A3))
			: thunk_ptr      (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3)>::invoke))
			, func_ptr       (*reinterpret_cast<void **>(&func))
			, func_holder_ptr(reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3)>::func_ptr))
			, this_holder_ptr(&static_invoker<R(T::*)(A1,A2,A3)>::this_ptr)
		{
TRACE("%s\n", __FUNCSIG__);
		}
		template<typename T, typename R, typename A1, typename A2, typename A3, typename A4>
		func_holder(R(T::*func)(A1,A2,A3,A4))
			: thunk_ptr      (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3,A4)>::invoke))
			, func_ptr       (*reinterpret_cast<void **>(&func))
			, func_holder_ptr(reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3,A4)>::func_ptr))
			, this_holder_ptr(&static_invoker<R(T::*)(A1,A2,A3,A4)>::this_ptr)
		{
TRACE("%s\n", __FUNCSIG__);
		}
		template<typename T, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
		func_holder(R(T::*func)(A1,A2,A3,A4,A5))
			: thunk_ptr      (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3,A4,A5)>::invoke))
			, func_ptr       (*reinterpret_cast<void **>(&func))
			, func_holder_ptr(reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3,A4,A5)>::func_ptr))
			, this_holder_ptr(&static_invoker<R(T::*)(A1,A2,A3,A4,A5)>::this_ptr)
		{
TRACE("%s\n", __FUNCSIG__);
		}
	};

	typedef unsigned char u8;
	typedef unsigned int  u32;
	u8* m_code_top;  // �t�b�N�p�R�[�h�f�[�^�̐擪�A�h���X
	u8* m_code_last; // �t�b�N�p�R�[�h�f�[�^�̐擪�A�h���X
	u8* m_code;      // �t�b�N�p�R�[�h�f�[�^�̐擪�A�h���X

public:

	thunk(){}

	thunk(void* this_, const func_holder& f)
	{
#if (defined(WIN32) || defined(_WIN32)) && (!defined(WIN64) || !defined(_WIN64))
		m_code_top  = m_code = (u8*)VirtualAlloc(NULL, 256, MEM_COMMIT, PAGE_READWRITE);
		m_code_last = m_code_top + 256;
		// mov dword ptr [f.this_holder_ptr], this_
		*m_code++       = 0xC7;
		*m_code++       = 0x05;
		*((u32*)m_code) = (u32)f.this_holder_ptr; m_code += sizeof(u32);
		*((u32*)m_code) = (u32)this_;             m_code += sizeof(u32);
		// mov dword ptr [f.func_holder_ptr], f.func_ptr
		*m_code++       = 0xC7;
		*m_code++       = 0x05;
		*((u32*)m_code) = (u32)f.func_holder_ptr; m_code += sizeof(u32);
		*((u32*)m_code) = (u32)f.func_ptr;        m_code += sizeof(u32);
		// jmp func
		*m_code++       = 0xE9;
		*((u32*)m_code) = (u32)f.thunk_ptr - (u32)m_code - 4; m_code += sizeof(u32);
		// �y�[�W���������s�݂̂ɕύX
		DWORD op;
		::VirtualProtect(m_code_top, DWORD(m_code - m_code_top), PAGE_EXECUTE, &op);
#else
#error "not impliment!"
#endif
	}

	~thunk()
	{
		::VirtualFree(m_code_top, 0, MEM_RELEASE);
	}

	void* get_code()
	{
		return (void*)m_code_top;
	}

	// �t�b�N�p�̃R�[�h��}��
	void* injection_code(void * func, int x)
	{
		return (void*)m_code_top;
//		u8* code_top = m_code;
//
//		DWORD op, op2;
//		// �֐��t�b�N�̂��߈�U�������������݂ł���悤�ɂ���
//		VirtualProtect(func, 5, PAGE_EXECUTE_READWRITE, &op);
//
//		// ���R�[�h�ǂݍ��݊֐������O�Ɋ֐������荞�܂�
//		u8* code = (u8*)func;
//
//		// ��jmp���߂����邱�Ƃ�O��ɂ��Ă���...
//		if( 0&&0xE9 != *code )
//		{
//			// ���ό��m�̂��߃y�[�W���������ɖ߂�
//			VirtualProtect(code, 5, op, &op);
//			return NULL;
//		}
//
//		VirtualProtect(m_code_top, 64, PAGE_EXECUTE_READWRITE, &op2);
//
//		memcpy(m_code, code, x);
//		m_code += x;
//
//		u8* jmp_addr  = (u8*)((u32)code + x);
//	//	u8* jmp_addr  = (u8*)((u32)code + *((u32*)(code+1)) + 5);
//		*code++       = 0xE9; // jmp
//		*((u32*)code) = (u32)m_code_top - (u32)code - 4;
//
//		// ���ό��m�̂��߃y�[�W���������ɖ߂�
//		VirtualProtect(code, 5, op, &op);
//
//		*m_code++       = 0xE9; // jmp
//		*((u32*)m_code) = (u32)jmp_addr - (u32)m_code - 4; m_code += sizeof(u32);
//
//		// �y�[�W���������s�݂̂ɕύX
//		::VirtualProtect(m_code_top, DWORD(m_code - m_code_top), PAGE_EXECUTE, &op2);
//
//		return code_top;
	}
};

template<typename T, typename R> void*   thunk::func_holder::static_invoker<R(T::*)()>::this_ptr    = NULL;
template<typename T, typename R> R (T::* thunk::func_holder::static_invoker<R(T::*)()>::func_ptr)() = NULL;

template<typename T, typename R, typename A1> void*   thunk::func_holder::static_invoker<R(T::*)(A1)>::this_ptr      = NULL;
template<typename T, typename R, typename A1> R (T::* thunk::func_holder::static_invoker<R(T::*)(A1)>::func_ptr)(A1) = NULL;

template<typename T, typename R, typename A1, typename A2> void*   thunk::func_holder::static_invoker<R(T::*)(A1,A2)>::this_ptr         = NULL;
template<typename T, typename R, typename A1, typename A2> R (T::* thunk::func_holder::static_invoker<R(T::*)(A1,A2)>::func_ptr)(A1,A2) = NULL;

template<typename T, typename R, typename A1, typename A2, typename A3> void*   thunk::func_holder::static_invoker<R(T::*)(A1,A2,A3)>::this_ptr            = NULL;
template<typename T, typename R, typename A1, typename A2, typename A3> R (T::* thunk::func_holder::static_invoker<R(T::*)(A1,A2,A3)>::func_ptr)(A1,A2,A3) = NULL;

template<typename T, typename R, typename A1, typename A2, typename A3, typename A4> void*   thunk::func_holder::static_invoker<R(T::*)(A1,A2,A3,A4)>::this_ptr               = NULL;
template<typename T, typename R, typename A1, typename A2, typename A3, typename A4> R (T::* thunk::func_holder::static_invoker<R(T::*)(A1,A2,A3,A4)>::func_ptr)(A1,A2,A3,A4) = NULL;

template<typename T, typename R, typename A1, typename A2, typename A3, typename A4, typename A5> void*   thunk::func_holder::static_invoker<R(T::*)(A1,A2,A3,A4,A5)>::this_ptr                  = NULL;
template<typename T, typename R, typename A1, typename A2, typename A3, typename A4, typename A5> R (T::* thunk::func_holder::static_invoker<R(T::*)(A1,A2,A3,A4,A5)>::func_ptr)(A1,A2,A3,A4,A5) = NULL;

} // namespace spplib

#endif // !defined(SPPLIB_AC7B9E21_2435_4031_AE45_DE16FE059E1B_INCLUDE_GUARD)
