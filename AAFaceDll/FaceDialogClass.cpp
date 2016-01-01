#include "ExternConstants.h"
//ebx+48
HWND FaceDialogClass::GetFaceSlotButton(BYTE slot) const {
	return GetButtonListWnd(0x48,slot);
}
int FaceDialogClass::GetButtonCount() const {
	return GetButtonListCount(0x48);
}
void FaceDialogClass::SetChangeFlags() {
	//our edit has a selection change
	BYTE* classptr = (BYTE*)this;
	//apparently internclass+0x80 is a bool that is set to 1 if faces change, so lets just do that
	*(classptr+0x80) = 1;
	//this one also has to be set to 1 it seems. no idea why. lets hope it doesnt break.
	//BYTE* someVal = *(BYTE**)(classptr+0x3C);
	BYTE* someVal = (BYTE*)g_AA2RedrawFlagTable;
	*(someVal+0x12) = 1;
}