#include "ExternConstants.h"
#include "Logger.h"

//so, this seems to always be the same for all dialog classes

//with ebx coming together as described here, and ebp = parameter = [ebp+B8] with ebp = class
//AA2Edit.exe+26D10 - 55                    - push ebp
//AA2Edit.exe+26D11 - 8B 6C 24 08           - mov ebp,[esp+08]
//AA2Edit.exe+26D15 - 8B 45 44              - mov eax,[ebp+44]
//AA2Edit.exe+26D18 - 8B 48 24              - mov ecx,[eax+24]
//AA2Edit.exe+26D1B - 83 39 00              - cmp dword ptr[ecx],00
//AA2Edit.exe+26D1E - 75 05                 - jne AA2Edit.exe+26D25
//AA2Edit.exe+26D20 - 8B 40 30              - mov eax,[eax+30]
//AA2Edit.exe+26D23 - EB 03                 - jmp AA2Edit.exe+26D28
//AA2Edit.exe+26D25 - 8B 40 2C              - mov eax,[eax+2C]
//AA2Edit.exe+26D28 - 85 C0                 - test eax,eax
//AA2Edit.exe+26D2A - 75 06                 - jne AA2Edit.exe+26D32
//AA2Edit.exe+26D2C - 32 C0                 - xor al,al
//AA2Edit.exe+26D2E - 5D                    - pop ebp
//AA2Edit.exe+26D2F - C2 0400               - ret 0004
//AA2Edit.exe+26D32 - 53                    - push ebx
//AA2Edit.exe+26D33 - 8B 58 28              - mov ebx,[eax+28]
//AA2Edit.exe+26D36 - 85 DB                 - test ebx,ebx
//AA2Edit.exe+26D38 - 75 07                 - jne AA2Edit.exe+26D41

//OH MAN. so, heres the story.
//since i first had the "wrong" class, i thought B8 was neccessary.
//it turns out that [wrong+B8] points to the "real" class. so this class should not be
//doing the B8 thing.
//its also rather interisting tho.

//this function returns ebx
void * BaseDialogClass::GetChoiceDataBuffer()
{
	//oddly similar
	BYTE* internclass = (BYTE*)this;
	//DWORD ebp = *(DWORD*)(internclass+0xB8);
	DWORD eax1 = *(DWORD*)(internclass+0x44);
	DWORD ecx1 = *(DWORD*)((BYTE*)eax1 + 0x24);
	DWORD cmpval = *(DWORD*)ecx1;
	DWORD eax2;
	if (cmpval == 0) {
		eax2 = *(DWORD*)((BYTE*)eax1 + 0x30);
	}
	else {
		eax2 = *(DWORD*)((BYTE*)eax1 + 0x2C);
	}
	if (eax2 == 0) {
		LOGPRIO(Logger::Priority::WARN) << "first pointer became NULL!\n";
		return NULL;
	}
	DWORD ebx = *(DWORD*)((BYTE*)eax2 + 0x28);
	if (ebx == 0) {
		LOGPRIO(Logger::Priority::WARN) << "second pointer became NULL!\n";
		return NULL;
	}
	return (void*)ebx;
}

const void * BaseDialogClass::GetChoiceDataBuffer() const
{
	//oddly similar
	BYTE* internclass = (BYTE*)this;
	//DWORD ebp = *(DWORD*)(internclass+0xB8);
	DWORD eax1 = *(DWORD*)(internclass+0x44);
	DWORD ecx1 = *(DWORD*)((BYTE*)eax1 + 0x24);
	DWORD cmpval = *(DWORD*)ecx1;
	DWORD eax2;
	if (cmpval == 0) {
		eax2 = *(DWORD*)((BYTE*)eax1 + 0x30);
	}
	else {
		eax2 = *(DWORD*)((BYTE*)eax1 + 0x2C);
	}
	if (eax2 == 0) {
		LOGPRIO(Logger::Priority::WARN) << "first pointer became NULL!\n";
		return NULL;
	}
	DWORD ebx = *(DWORD*)((BYTE*)eax2 + 0x28);
	if (ebx == 0) {
		LOGPRIO(Logger::Priority::WARN) << "second pointer became NULL!\n";
		return NULL;
	}
	return (const void*)ebx;
}

//button lists also always followe the same pattern:
/*
BYTE* classptr = (BYTE*)this;
BYTE* ecx = (BYTE*)*(DWORD*)(classptr + 0x48);
BYTE* ptr1 = (BYTE*)*(DWORD*)(ecx + 0x14);
DWORD offset = slot << 4;
return *(HWND*)(ptr1 + offset);
or
DWORD edi = *(DWORD*)((BYTE*)this + 0x54);
DWORD esi = *(DWORD*)((BYTE*)edi + 0x14);
DWORD hwndptr = esi+0x10*n;
return *(HWND*)hwndptr;
so
its *this + [offset] for the list. then
*0x14 buttonstruct array
*0x18 is the first unused button; button struct has a size of 0x10.
*/
HWND BaseDialogClass::GetButtonListWnd(int offset, int buttonNumber) const
{
	DWORD edi = *(DWORD*)((BYTE*)this + offset);
	DWORD esi = *(DWORD*)((BYTE*)edi + 0x14);
	DWORD hwndptr = esi+0x10*buttonNumber;
	return *(HWND*)hwndptr;
}

int BaseDialogClass::GetButtonListCount(int offset) const
{
	DWORD edi = *(DWORD*)((BYTE*)this + offset);
	DWORD first = *(DWORD*)((BYTE*)edi + 0x14);
	DWORD last = *(DWORD*)((BYTE*)edi + 0x18);
	return (last-first)/0x10;
}