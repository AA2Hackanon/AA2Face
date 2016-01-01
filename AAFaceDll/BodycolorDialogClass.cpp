#include "ExternConstants.h"
#include "Logger.h"

HWND BodycolorDialogClass::GetTanButtonHwnd(BYTE n) const{
	//0x54
	return GetButtonListWnd(0x54,n);
}
int BodycolorDialogClass::GetTanButtonCount() const{
	return GetButtonListCount(0x54);
}

void BodycolorDialogClass::SetChangeFlags()
{
	//this+0x240 is flag 1
	//flagtable + 0x10 is flag for tan
	*((BYTE*)this + 0x240) = 1;
	*(BYTE*)(g_AA2RedrawFlagTable+0x10) = 1;
}

BYTE BodycolorDialogClass::GetCurrentTanSlot() const{
	const void* buffer = GetChoiceDataBuffer();
	if (buffer == NULL) {
		LOGPRIO(Logger::Priority::WARN) << "first pointer became NULL!\n";
		return 0;
	}
	//AA2Edit.exe+20B59 - 88 86 67040000        - mov [esi+00000467],al
	return *((BYTE*)buffer + 0x467);
}

void BodycolorDialogClass::SetCurrentTanSlot(BYTE slot)
{
	void* buffer = GetChoiceDataBuffer();
	if (buffer == NULL) {
		LOGPRIO(Logger::Priority::WARN) << "first pointer became NULL!\n";
		return;
	}
	//AA2Edit.exe+20B59 - 88 86 67040000        - mov [esi+00000467],al
	*((BYTE*)buffer + 0x467) = slot;
}
