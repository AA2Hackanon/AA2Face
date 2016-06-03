#include "InjBodycolor.h"
#include "Logger.h"
#include "GenUtils.h"

BoundUpDownControl<BodycolorDialogClass,
	&BodycolorDialogClass::GetNipTypeButtonHwnd,
	&BodycolorDialogClass::GetNipTypeButtonCount,
	&BodycolorDialogClass::GetCurrentNipTypeSlot,
	&BodycolorDialogClass::SetCurrentNipTypeSlot,
	&BodycolorDialogClass::SetChangeFlags>
	g_budNipType("Nip Type");
BoundUpDownControl<BodycolorDialogClass,
	&BodycolorDialogClass::GetNipColorButtonHwnd,
	&BodycolorDialogClass::GetNipColorButtonCount,
	&BodycolorDialogClass::GetCurrentNipColorSlot,
	&BodycolorDialogClass::SetCurrentNipColorSlot,
	&BodycolorDialogClass::SetChangeFlags>
	g_budNipColor("Nip Color");
BoundUpDownControl<BodycolorDialogClass,
	&BodycolorDialogClass::GetTanButtonHwnd,
	&BodycolorDialogClass::GetTanButtonCount,
	&BodycolorDialogClass::GetCurrentTanSlot,
	&BodycolorDialogClass::SetCurrentTanSlot,
	&BodycolorDialogClass::SetChangeFlags>
	g_budTan("Tan");
BoundUpDownControl<BodycolorDialogClass,
	&BodycolorDialogClass::GetMosiacButtonHwnd,
	&BodycolorDialogClass::GetMosaicButtonCount,
	&BodycolorDialogClass::GetCurrentMosaicSlot,
	&BodycolorDialogClass::SetCurrentMosaicSlot,
	&BodycolorDialogClass::SetChangeFlags>
	g_budMosaic("Mosaic");
BoundUpDownControl<BodycolorDialogClass,
	&BodycolorDialogClass::GetPubHairButtonHwnd,
	&BodycolorDialogClass::GetPubHairButtonCount,
	&BodycolorDialogClass::GetCurrentPubHairSlot,
	&BodycolorDialogClass::SetCurrentPubHairSlot,
	&BodycolorDialogClass::SetChangeFlags>
	g_budPubHair("Pub Hair");

namespace {
	bool loc_characterLoaded = false;
};

void __cdecl BodycolorDialogNotification(BodycolorDialogClass* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	if(msg == BODYCOLORMESSAGE_ADDTAN) {
		int diff = (int)wparam;
		LOGPRIO(Logger::Priority::SPAM) << "recieved ADDTAN message with diff " << diff << "\n";
		g_budTan.IncChosenSlot(diff);
	}
	else if(msg == BODYCOLORMESSAGE_SETTAN) {
		int slot = (int)wparam;
		LOGPRIO(Logger::Priority::SPAM) << "recieved ADDTAN message with slot " << slot << "\n";
		g_budTan.SetChosenSlot(slot);
	}
	else if (msg == WM_COMMAND && lparam != 0) {
		g_budNipType.HookedWmCommandNotification(internclass,wnd,msg,wparam,lparam);
		g_budNipColor.HookedWmCommandNotification(internclass,wnd,msg,wparam,lparam);
		g_budTan.HookedWmCommandNotification(internclass,wnd,msg,wparam,lparam);
		g_budMosaic.HookedWmCommandNotification(internclass,wnd,msg,wparam,lparam);
		g_budPubHair.HookedWmCommandNotification(internclass,wnd,msg,wparam,lparam);
	}
}

void __cdecl BodycolorAfterDialogInit(BodycolorDialogClass* internclass,HWND wnd) {
	//nip type and color have to be next to each other
	int btnCnt = internclass->GetNipTypeButtonCount();
	HWND lastButton = internclass->GetNipTypeButtonHwnd(btnCnt - 1);
	RECT rct = GetRelativeRect(lastButton);
	DWORD xw = rct.right - rct.left;
	rct.left += xw,rct.right += xw;
	g_budNipType.Initialize(wnd,rct);
	rct.left += xw,rct.right += xw;
	g_budNipColor.Initialize(wnd,rct);
	g_budTan.Initialize(wnd,internclass,BoundUpDownPosition::RIGHT_OF_LAST);
	g_budMosaic.Initialize(wnd,internclass,BoundUpDownPosition::RIGHT_OF_LAST);
	g_budPubHair.Initialize(wnd,internclass,BoundUpDownPosition::RIGHT_OF_LAST);
}

int __cdecl GetTanSelectorIndex(BodycolorDialogClass* internclass,int guiChosen) {
	return g_budTan.GetSelectorIndex(internclass,guiChosen);
}

int __cdecl GetNipTypeSelectorIndex(BodycolorDialogClass* internclass,int guiChosen) {
	return g_budNipType.GetSelectorIndex(internclass,guiChosen);
}
int __cdecl GetNipColorSelectorIndex(BodycolorDialogClass* internclass,int guiChosen) {
	return g_budNipColor.GetSelectorIndex(internclass,guiChosen);
}

int __cdecl GetMosaicSelectorIndex(BodycolorDialogClass* internclass,int guiChosen) {
	return g_budMosaic.GetSelectorIndex(internclass,guiChosen);
}
int __cdecl GetPubHairSelectorIndex(BodycolorDialogClass* internclass,int guiChosen) {
	return g_budPubHair.GetSelectorIndex(internclass,guiChosen);
}

void __cdecl InitBodycolorTab(BodycolorDialogClass* internclass,bool before) {
	g_budNipType.InitTab(internclass,before);
	g_budNipColor.InitTab(internclass,before);
	g_budTan.InitTab(internclass,before);
	g_budMosaic.InitTab(internclass,before);
	g_budPubHair.InitTab(internclass,before);
}

void BodycolorDialogOnCharacterLoad() {
	loc_characterLoaded = true;
}