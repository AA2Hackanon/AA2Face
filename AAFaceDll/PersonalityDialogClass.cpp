#include "ExternConstants.h"

HWND PersonalityDialogClass::GetPersonalityButtonHwnd(BYTE n) const {
	return GetButtonListWnd(0x4C, n);
}
int PersonalityDialogClass::GetPersonalityButtonCount() const {
	return GetButtonListCount(0x4C);
}

int PersonalityDialogClass::GetPersonalityButtonIndex() const
{
	return 0;
}

void PersonalityDialogClass::SetPersonalityButtonIndex(BYTE slot)
{
}

int PersonalityDialogClass::GetPersonalitySlot() const
{
	return *((BYTE*)GetChoiceDataBuffer() + 0x445);
}

void PersonalityDialogClass::SetPersonalitySlot(BYTE slot)
{
	*((BYTE*)GetChoiceDataBuffer() + 0x445) = slot;
}

//note that there is a difference between button index and the slot;
//the button index is put into this map and the result is put into the slot
DWORD* PersonalityDialogClass::GetButtonIndexPersonalityMap()
{
	//AA2Edit.exe+2D458 - 8B 93 9C010000        - mov edx,[ebx+0000019C]
	//AA2Edit.exe+2D45E - 8A 04 82              - mov al,[edx+eax*4]
	DWORD ptr1 = *(DWORD*)((BYTE*)this + 0x19C);
	return (DWORD*)ptr1;
}

PersonalityDialogClass::IllusionMapType* PersonalityDialogClass::GetPersonalityListFile() {
	//the code below gets the edi from above.
	//in it, edx is the PersonalityDialogClass instance,
	//and the ecx at the end is edi-1C (so that ecx+1C is edi from above).
	//AA2Edit.exe + 2DDE0 - 8B 42 44 - mov eax, [edx + 44]
	//AA2Edit.exe + 2DDE3 - 8B 48 24 - mov ecx, [eax + 24]
	//AA2Edit.exe + 2DDE6 - 83 39 00 - cmp dword ptr[ecx], 00 { 0 }
	//AA2Edit.exe + 2DDE9 - 89 54 24 14 - mov[esp + 14], edx
	//AA2Edit.exe + 2DDED - 75 05 - jne AA2Edit.exe + 2DDF4
	//AA2Edit.exe + 2DDEF - 8B 40 30 - mov eax, [eax + 30]
	//AA2Edit.exe + 2DDF2 - EB 03 - jmp AA2Edit.exe + 2DDF7
	//AA2Edit.exe + 2DDF4 - 8B 40 2C - mov eax, [eax + 2C]
	//AA2Edit.exe + 2DDF7 - 85 C0 - test eax, eax
	//AA2Edit.exe + 2DDF9 - 75 07 - jne AA2Edit.exe + 2DE02
	//AA2Edit.exe + 2DDFB - 32 C0 - xor al, al
	//AA2Edit.exe + 2DDFD - E9 33040000 - jmp AA2Edit.exe + 2E235
	//AA2Edit.exe + 2DE02 - 8B 48 28 - mov ecx, [eax + 28]
	//AA2Edit.exe + 2DE05 - 89 4C 24 40 - mov[esp + 40], ecx
	DWORD ptr1 = *(DWORD*)((BYTE*)this + 0x44);
	DWORD ptr2 = *(DWORD*)((BYTE*)ptr1 + 0x24);
	DWORD ptr3;
	if (*((DWORD*)ptr2) == 0) {
		ptr3 = *(DWORD*)((BYTE*)ptr1 + 0x30);
	}
	else {
		ptr3 = *(DWORD*)((BYTE*)ptr1 + 0x2C);
	}
	if (ptr3 == 0) return NULL;
	DWORD ptr4 = *(DWORD*)((BYTE*)ptr3 + 0x28);
	DWORD ptr5 = ptr4 + 0x1C;
	return (PersonalityDialogClass::IllusionMapType*)ptr5;
}