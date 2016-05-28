#include "ExternConstants.h"
#include "Logger.h"

HWND BodycolorDialogClass::GetTanButtonHwnd(BYTE n) const{
	//0x54
	return GetButtonListWnd(0x54,n);
}
int BodycolorDialogClass::GetTanButtonCount() const{
	return GetButtonListCount(0x54);
}

HWND BodycolorDialogClass::GetNipTypeButtonHwnd(BYTE n) const {
	return GetButtonListWnd(0x48,n);
}
int BodycolorDialogClass::GetNipTypeButtonCount() const {
	return GetButtonListCount(0x48);
}
HWND BodycolorDialogClass::GetNipColorButtonHwnd(BYTE n) const {
	return GetButtonListWnd(0x4C,n);
}
int BodycolorDialogClass::GetNipColorButtonCount() const {
	return GetButtonListCount(0x4C);
}
HWND BodycolorDialogClass::GetMosiacButtonHwnd(BYTE n) const {
	return GetButtonListWnd(0x5C,n);
}
int BodycolorDialogClass::GetMosaicButtonCount() const {
	return GetButtonListCount(0x5C);
}
HWND BodycolorDialogClass::GetPubHairButtonHwnd(BYTE n) const {
	return GetButtonListWnd(0x60,n);
}
int BodycolorDialogClass::GetPubHairButtonCount() const {
	return GetButtonListCount(0x60);
}
HWND BodycolorDialogClass::GetPubHairOpacityEditWnd() const {
	DWORD ptr1 = *(DWORD*)((BYTE*)this + 0x64);
	HWND handle = *(HWND*)(ptr1+0x24);
	return handle;
}

void BodycolorDialogClass::SetChangeFlags()
{
	//this+0x240 is flag 1
	//flagtable + 0x10 is flag for tan
	*((BYTE*)this + 0x240) = 1;
	*(BYTE*)(g_AA2RedrawFlagTable+0x10) = 1;
	*(BYTE*)(g_AA2RedrawFlagTable+0x8) = 1; //8 is both nip kinds
	*(BYTE*)(g_AA2RedrawFlagTable+0x11) = 1; //mosaic
	*(BYTE*)(g_AA2RedrawFlagTable+0xB) = 1; //pub hair
}

BYTE BodycolorDialogClass::GetCurrentNipTypeSlot() const {
	return GetChoiceFlag(0x464);
}
void BodycolorDialogClass::SetCurrentNipTypeSlot(BYTE slot) {
	SetChoiceFlag(0x464,slot);
}
BYTE BodycolorDialogClass::GetCurrentNipColorSlot() const {
	return GetChoiceFlag(0x465);
}
void BodycolorDialogClass::SetCurrentNipColorSlot(BYTE slot) {
	SetChoiceFlag(0x465,slot);
}

BYTE BodycolorDialogClass::GetCurrentTanSlot() const{
	//AA2Edit.exe+20B59 - 88 86 67040000        - mov [esi+00000467],al
	return GetChoiceFlag(0x467);
}

void BodycolorDialogClass::SetCurrentTanSlot(BYTE slot)
{
	//AA2Edit.exe+20B59 - 88 86 67040000        - mov [esi+00000467],al
	SetChoiceFlag(0x467,slot);
}

BYTE BodycolorDialogClass::GetCurrentMosaicSlot() const {
	return GetChoiceFlag(0x469);
}
void BodycolorDialogClass::SetCurrentMosaicSlot(BYTE slot) {
	SetChoiceFlag(0x469,slot);
}
BYTE BodycolorDialogClass::GetCurrentPubHairSlot() const {
	return GetChoiceFlag(0x45C);
}
void BodycolorDialogClass::SetCurrentPubHairSlot(BYTE slot) {
	SetChoiceFlag(0x45C,slot);
}
BYTE BodycolorDialogClass::GetCurrentPubHairOpacityValue() {
	return GetChoiceFlag(0x45D);
}
void BodycolorDialogClass::SetCurrentPubHairOpacityValue(BYTE value) {
	SetChoiceFlag(0x45D,value);
}
BYTE BodycolorDialogClass::GetCurrentPubHairOpacityGuiValue() {
	DWORD ptr1 = *(DWORD*)((BYTE*)this + 0x64);
	return *(BYTE*)(ptr1+0x10);
}
void BodycolorDialogClass::SetCurrentPubHairOpacityGuiValue(BYTE value) {
	DWORD ptr1 = *(DWORD*)((BYTE*)this + 0x64);
	*(BYTE*)(ptr1+0x10) = value;
}