#pragma once
#include <initializer_list>
#include <Windows.h>


//just to make it a little more readable
namespace Opcodes {
	extern BYTE Call;
	extern BYTE Nop;
};

class Memrights {
private:
	DWORD oldRights;
	LPVOID addr;
	DWORD size;
public:
	bool good;
	Memrights(LPVOID addr,DWORD size);
	~Memrights();
};

/*
 * A general Hook function to avoid constant code duplication.
 */
bool Hook(BYTE* code, int nBytes, std::initializer_list<std::initializer_list<DWORD>> ops);

void HookGeneral();
void HookSystem();
void HookBodycolor();
void HookFace();
void HookHair();
void HookFacedetails();

void HookLimits();

void HookTEMPTEST();
void HookHairMaxAmount();
