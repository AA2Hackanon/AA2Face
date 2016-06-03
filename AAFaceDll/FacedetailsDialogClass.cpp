#include "ExternConstants.h"

//all bytes:
//ebx+0x692
BYTE FacedetailsDialogClass::GetEyelidSlot() const {
	return *((BYTE*)GetChoiceDataBuffer() + 0x692);
}
//ebx+0x693
BYTE FacedetailsDialogClass::GetUpperEyelidSlot() const {
	return *((BYTE*)GetChoiceDataBuffer() + 0x693);
}
//ebx+0x694
BYTE FacedetailsDialogClass::GetLowerEyelidSlot() const {
	return *((BYTE*)GetChoiceDataBuffer() + 0x694);
}
//ebx+0x695
BYTE FacedetailsDialogClass::GetGlassesSlot() const {
	return *((BYTE*)GetChoiceDataBuffer() + 0x695);
}
//ebx+0x696
//why is this slot based anyway, wtf
BYTE FacedetailsDialogClass::GetLipColorSlot() const {
	return *((BYTE*)GetChoiceDataBuffer() + 0x696);
}
void FacedetailsDialogClass::SetLipColorSlot(BYTE slot) {
	*((BYTE*)GetChoiceDataBuffer() + 0x696) = slot;
}

void FacedetailsDialogClass::SetGlassesSlot(BYTE slot)
{
	*((BYTE*)GetChoiceDataBuffer() + 0x695) = slot;
}
BYTE FacedetailsDialogClass::GetLipOpacityValue() const {
	return GetChoiceFlag(0x697);
}
void FacedetailsDialogClass::SetLipOpacityValue(BYTE value) {
	SetChoiceFlag(0x697,value);
}

//AA2Edit.exe+3390E - 8B B5 B8000000        - mov esi,[ebp+000000B8]
//AA2Edit.exe+33914 - 80 BE 9C010000 00     - cmp byte ptr[esi+0000019C],00
//AA2Edit.exe+3391B - 74 66                 - je AA2Edit.exe+33983
//also
//note that ebp in this context is the wrong class, but +3C on this class is the same, apparently
//AA2Edit.exe+3394F - 8B 45 3C              - mov eax,[ebp+3C]
//AA2Edit.exe+33952 - 80 78 14 00           - cmp byte ptr[eax+14],00
//AA2Edit.exe+33956 - 74 18                 - je AA2Edit.exe+33970

void FacedetailsDialogClass::SetChangeFlags()
{
	//DWORD esi = *(DWORD*)((BYTE*)this + 0xB8);
	BYTE* flagPtr = ((BYTE*)this + 0x19C);
	*flagPtr = 1;
	//BYTE* eax = *(BYTE**)((BYTE*)this + 0x3C);
	BYTE* eax = (BYTE*)g_AA2RedrawFlagTable;
	*(eax+0x14) = 1; //glasses
	*(eax+0x13) = 1; //lip color
}

int FacedetailsDialogClass::GetGlassButtonCount()const {
	//DWORD ebp = *(DWORD*)((BYTE*)this + 0xB8);
	/*DWORD edi = *(DWORD*)((BYTE*)this + 0x54);
	DWORD first = *(DWORD*)((BYTE*)edi + 0x14);
	DWORD last = *(DWORD*)((BYTE*)edi + 0x18);
	return (last-first)/0x10;*/
	return GetButtonListCount(0x54);
}

//and ebp = parameter = [ebp+B8] with ebp = class
//with edi = [ebp+54] from above,
//esi = [edi+14] is the first struct button, [edi+18] is the first unused button struct (as known from hair)
//esi is than += 0x10ed, while [esi] is the hwnd of the button. neato.
HWND FacedetailsDialogClass::GetGlassesButtonWnd(BYTE n) const {
	//DWORD ebp = *(DWORD*)((BYTE*)this + 0xB8);
	/*DWORD edi = *(DWORD*)((BYTE*)this + 0x54);
	DWORD esi = *(DWORD*)((BYTE*)edi + 0x14);
	DWORD hwndptr = esi+0x10*n;
	return *(HWND*)hwndptr;*/
	return GetButtonListWnd(0x54,n);
}


HWND FacedetailsDialogClass::GetLipColorButtonWnd(BYTE n) const {
	return GetButtonListWnd(0x58,n);
}
int FacedetailsDialogClass::GetLipColorButtonCount() const {
	return GetButtonListCount(0x58);
}

HWND FacedetailsDialogClass::GetLipOpacityEditWnd() const {
	//[0A0C0A40+5C]->0A0A09D0
	//[0A0A09D0+24]->handle 
	DWORD ptr1 = *(DWORD*)((BYTE*)this + 0x5C);
	HWND handle = *(HWND*)(ptr1+0x24);
	return handle;
}

HWND FacedetailsDialogClass::GetLipOpacityTrackWnd() const {
	DWORD ptr1 = *(DWORD*)((BYTE*)this + 0x5C);
	HWND handle = *(HWND*)(ptr1+0x14);
	return handle;
}

BYTE FacedetailsDialogClass::GetLipOpacityGuiValue() const {
	DWORD ptr1 = *(DWORD*)((BYTE*)this + 0x5C);
	return *(BYTE*)(ptr1+0x10);
}
void FacedetailsDialogClass::SetLipOpacityGuiValue(BYTE b) {
	DWORD ptr1 = *(DWORD*)((BYTE*)this + 0x5C);
	*(BYTE*)(ptr1+0x10) = b;
}
