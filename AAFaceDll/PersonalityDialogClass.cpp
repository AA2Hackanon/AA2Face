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
BYTE* PersonalityDialogClass::GetButtonIndexPersonalityMap()
{
	//AA2Edit.exe+2D458 - 8B 93 9C010000        - mov edx,[ebx+0000019C]
	//AA2Edit.exe+2D45E - 8A 04 82              - mov al,[edx+eax*4]
	DWORD ptr1 = *(DWORD*)((BYTE*)this + 0x19C);
	return (BYTE*)ptr1;
}
