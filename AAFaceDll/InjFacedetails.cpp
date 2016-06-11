#include "InjFacedetails.h"
#include "Logger.h"
#include "GenUtils.h"
#include "SlotFile.h"

BoundUpDownControl<FacedetailsDialogClass,
				&FacedetailsDialogClass::GetGlassesButtonWnd,
				&FacedetailsDialogClass::GetGlassButtonCount,
				&FacedetailsDialogClass::GetGlassesSlot,
				&FacedetailsDialogClass::SetGlassesSlot,
				&FacedetailsDialogClass::SetChangeFlags>
	g_budGlasses("Glasses");
BoundUpDownControl<FacedetailsDialogClass,
				&FacedetailsDialogClass::GetLipColorButtonWnd,
				&FacedetailsDialogClass::GetLipColorButtonCount,
				&FacedetailsDialogClass::GetLipColorSlot,
				&FacedetailsDialogClass::SetLipColorSlot,
				&FacedetailsDialogClass::SetChangeFlags>
	g_budLipColor("Lip Color");

namespace {
	
};

void __cdecl InitGlassesSelector(HWND parent,HINSTANCE hInst) {
	//not used, called before anything is created
}

void __cdecl FacedetailsDialogNotification(FacedetailsDialogClass* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	if(msg == FACEDETAILSMESSAGE_ADDGLASSES) {
		int diff = (int)wparam;
		LOGPRIO(Logger::Priority::SPAM) << "recieved ADDGLASSES message with diff " << diff << "\n";
		g_budGlasses.IncChosenSlot(diff);
	}
	else if(msg == FACEDETAILSMESSAGE_SETGLASSES) {
		int set = (int)wparam;
		LOGPRIO(Logger::Priority::SPAM) << "recieved SETGLASSES message with slot " << set << "\n";
		g_budGlasses.SetChosenSlot(set);
	}
	else if (msg == WM_COMMAND && lparam != 0) {
		g_budGlasses.HookedWmCommandNotification(internclass,wnd,msg,wparam,lparam);
		g_budLipColor.HookedWmCommandNotification(internclass,wnd,msg,wparam,lparam);
		//lip opacity slider
		DWORD ctrlId = LOWORD(wparam);
		DWORD notification = HIWORD(wparam);
		HWND wnd = (HWND)lparam;
		if(wnd == internclass->GetLipOpacityEditWnd()) {
			if (notification == EN_UPDATE) {
				HWND nativeEdit = internclass->GetLipOpacityEditWnd();
				LimitEditInt(nativeEdit,0,255);
				int value = GetEditNumber(nativeEdit);
				LOGPRIO(Logger::Priority::SPAM) << "lip color edit was moved to " << value << "\n";
				internclass->SetLipOpacityGuiValue(value);
				internclass->SetChangeFlags();
			}
		}
	}
}

void __cdecl FacedetailsAfterDialogInit(FacedetailsDialogClass* internclass, HWND wnd) {
	g_budGlasses.Initialize(wnd,internclass,BoundUpDownPosition::RIGHT_OF_LAST);
	g_budLipColor.Initialize(wnd,internclass,BoundUpDownPosition::BELOW_LAST);
}

void __cdecl FacedetailsAfterInit(void* internclass) {
	//this is the "wrong" class, where *internclass+0xB8 is the detailsclass
}

int __cdecl GetGlassesSelectorIndex(FacedetailsDialogClass* internclass,int guiChosen) {
	return g_budGlasses.GetSelectorIndex(internclass,guiChosen);
}

int __cdecl GetLipColorSelectorIndex(FacedetailsDialogClass* internclass,int guiChosen) {
	return g_budLipColor.GetSelectorIndex(internclass,guiChosen);
}

void __cdecl InitFacedetailsTab(FacedetailsDialogClass* internclass,bool before) {
	g_budGlasses.InitTab(internclass,before);
	g_budLipColor.InitTab(internclass,before);
}

void FacedetailsDialogOnCharacterLoad() {
	
}

int __cdecl RandomFaceDetailsSelect(FacedetailsDialogClass* internclass) {
	BYTE randSlot;
	if (g_slotFile.ValidSlotCount(SlotFile::GLASSES)) {
		do {
			randSlot = rand()%256; 
		} while (!g_slotFile.SlotExists(SlotFile::GLASSES,randSlot));
			internclass->SetGlassesSlot(randSlot); 
			g_budGlasses.SetChosenSlot(randSlot); 
	}
}