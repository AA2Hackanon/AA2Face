#include "InjSystem.h"
#include "InjBodycolor.h"
#include "InjFacedetails.h"

void __cdecl SystemDialogNotification(void* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	if(msg == WM_INITDIALOG) {
		
	}
	else if(msg == WM_COMMAND && lparam != 0) {

	}
}

void __cdecl SystemDialogCharacterCreated() {
	FacedetailsDialogOnCharacterLoad();
	BodycolorDialogOnCharacterLoad();
}

void __cdecl SystemDialogCharacterLoaded() {
	FacedetailsDialogOnCharacterLoad();
	BodycolorDialogOnCharacterLoad();
}