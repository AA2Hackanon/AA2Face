#pragma once
#include <initializer_list>
#include "Injections.h"

namespace {
	template <typename T>
	bool HookSub(BYTE*& code, const T& p1) {
		static_assert(p1.size > 0);
		if(p1[0] == Opcodes::Call) {
			//if first is call opcode, assume second parameter to be
			//the function address of the function to call instead
			*code = Opcodes::Call;
			code++;
			*(DWORD*)code = p1[1] - (DWORD)(code + 4);
			code += 4;
		}
		else {
			//else, just copy the data
			for (int i = 0; i < sizeof(p1)/sizeof(p1[0]); i++) {
				*code++ = p1[i];
			}
		}
	}
	template<typename T, typename... Args>
	bool HookSub(BYTE*& code, const T& p1, const Args&... args) {
		HookSub(code,p1);
		HookSub(code,args);
	}
};

template <typename T,typename... Args>
bool Hook(BYTE* code,int nBytes,T p1,Args... args) {
	Memrights writeRights(code,nBytes);
	if(writeRights.good) {
		return HookSub(code,p1, args);
	}
	else {
		return false;
	}
}