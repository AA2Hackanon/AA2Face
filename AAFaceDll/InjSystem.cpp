#include <intrin.h>
#include <string>
#include <sstream>
#include "InjSystem.h"
#include "InjBodycolor.h"
#include "InjFacedetails.h"
#include "GenUtils.h"

HWND g_cbSystemKeepCardFace = NULL;
HWND g_gbSystemPose = NULL;
HWND g_edSystemPose = NULL;
HWND g_udSystemPose = NULL;
HWND g_btSystemPose = NULL;
HWND g_cbSystemLockPose = NULL;
HWND g_cbSystemEyeTrack = NULL;

namespace {
	void* loc_barClass = NULL;
	//barClass+D8 is the outer flag,
	//flagtable+4 the other flag
	bool loc_applyClothes = false;
	int loc_playPos = -1; //-1 means dont change
};

struct ClothChoiceGUI {
	HWND m_gbClothing;

	HWND m_gbOutfitState;
	HWND m_edOutfitState, m_udOutfitState;
	HWND m_cbShirtState;
	HWND m_edShirtState, m_udShirtState;
	HWND m_cbSkirtState;
	HWND m_edSkirtState, m_udSkirtState;

	HWND m_gbOutfit;
	HWND m_rbSchoolOutfit;
	HWND m_rbSportOutfit;
	HWND m_rbSwimOutfit;
	HWND m_rbClubOutfit;
	HWND m_cbOutfitOverride;
	HWND m_edOutfitOverride, m_udOutfitOverride;

	HWND m_btApply;

	HWND m_currOutfitRb;

	DWORD GetSizeX() {return 365;}
	DWORD GetSizeY() {return 230;}

	ClothChoiceGUI() {}
	void Initialize(HWND parent, DWORD x, DWORD y, HFONT font) {
		m_gbClothing = CreateWindowExW(0,L"BUTTON",L"Clothes",WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			x,y,365,230,parent,0,(HINSTANCE)g_AA2Base,0);
		SendMessage(m_gbClothing,WM_SETFONT,(WPARAM)font,TRUE);

		x += 15;
		y += 15;

		m_gbOutfitState = CreateWindowExW(0,L"BUTTON",L"State",WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			x,y,200,115,parent,0,(HINSTANCE)g_AA2Base,0);
		SendMessage(m_gbOutfitState,WM_SETFONT,(WPARAM)font,TRUE);

		CreateUpDownControl(parent,x+10,y+20,60,25,m_edOutfitState,m_udOutfitState);
		SendMessage(m_edOutfitState,WM_SETFONT,(WPARAM)font,TRUE);
		m_cbShirtState = CreateWindowExW(0,L"BUTTON",L"Override Shirt",WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
			x+10,y+50,100,25,parent,0,(HINSTANCE)g_AA2Base,0);
		SendMessage(m_cbShirtState,WM_SETFONT,(WPARAM)font,TRUE);
		CreateUpDownControl(parent,x+130,y+50,60,25,m_edShirtState,m_udShirtState);
		SendMessage(m_edShirtState,WM_SETFONT,(WPARAM)font,TRUE);
		m_cbSkirtState = CreateWindowExW(0,L"BUTTON",L"Override Skirt",WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
			x+10,y+80,100,25,parent,0,(HINSTANCE)g_AA2Base,0);
		SendMessage(m_cbSkirtState,WM_SETFONT,(WPARAM)font,TRUE);
		CreateUpDownControl(parent,x+130,y+80,60,25,m_edSkirtState,m_udSkirtState);
		SendMessage(m_edSkirtState,WM_SETFONT,(WPARAM)font,TRUE);

		m_btApply = CreateWindowExW(0,L"BUTTON",L"Apply",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			x+220,y+55,60,25,parent,0,(HINSTANCE)g_AA2Base,0);
		SendMessage(m_btApply,WM_SETFONT,(WPARAM)font,TRUE);

		y += 120;

		m_gbOutfit = CreateWindowExW(0,L"BUTTON",L"Outfit",WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			x,y,340,85,parent,0,(HINSTANCE)g_AA2Base,0);
		SendMessage(m_gbOutfit,WM_SETFONT,(WPARAM)font,TRUE);

		m_rbSchoolOutfit = CreateWindowExW(0,L"BUTTON",L"School",WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			x+10,y+15,60,25,parent,0,(HINSTANCE)g_AA2Base,0);
		SendMessage(m_rbSchoolOutfit,WM_SETFONT,(WPARAM)font,TRUE);
		m_rbSportOutfit = CreateWindowExW(0,L"BUTTON",L"Sport",WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			x+75,y+15,60,25,parent,0,(HINSTANCE)g_AA2Base,0);
		SendMessage(m_rbSportOutfit,WM_SETFONT,(WPARAM)font,TRUE);
		m_rbSwimOutfit = CreateWindowExW(0,L"BUTTON",L"Swim",WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			x+140,y+15,60,25,parent,0,(HINSTANCE)g_AA2Base,0);
		SendMessage(m_rbSwimOutfit,WM_SETFONT,(WPARAM)font,TRUE);
		m_rbClubOutfit = CreateWindowExW(0,L"BUTTON",L"Club",WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			x+205,y+15,60,25,parent,0,(HINSTANCE)g_AA2Base,0);
		SendMessage(m_rbClubOutfit,WM_SETFONT,(WPARAM)font,TRUE);
		m_cbOutfitOverride = CreateWindowExW(0,L"BUTTON",L"Override",WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
			x+270,y+15,60,25,parent,0,(HINSTANCE)g_AA2Base,0);
		SendMessage(m_cbOutfitOverride,WM_SETFONT,(WPARAM)font,TRUE);
		CreateUpDownControl(parent,x+270,y+15+30,60,25,m_edOutfitOverride,m_udOutfitOverride);
		SendMessage(m_edOutfitOverride,WM_SETFONT,(WPARAM)font,TRUE);

		m_currOutfitRb = m_rbSchoolOutfit;
		SendMessage(m_rbSchoolOutfit,BM_SETCHECK,BST_CHECKED,0);

		//EnableWindow(m_cbOutfitOverride, FALSE);
		EnableWindow(m_edOutfitOverride,FALSE);
		EnableWindow(m_edShirtState,FALSE);
		EnableWindow(m_edSkirtState,FALSE);
	}

	void SwitchCheckboxWithEdit(HWND checkbox, HWND edit) {
		bool checked = SendMessage(checkbox,BM_GETCHECK,0,0) == BST_CHECKED;
		SendMessage(checkbox,BM_SETCHECK,checked ? BST_UNCHECKED : BST_CHECKED,0);
		EnableWindow(edit,checked ? FALSE : TRUE);
	}

	void DialogNotification(void* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam) {
		if (msg != WM_COMMAND || lparam == 0) return;
		HWND targetwnd = (HWND)lparam;
		DWORD ctrlId = LOWORD(wparam);
		DWORD notification = HIWORD(wparam);
		if (notification == BN_CLICKED) {
			if (targetwnd == m_cbShirtState) {
				//enable/disable associated edit box
				SwitchCheckboxWithEdit(m_cbShirtState,m_edShirtState);
			}
			else if(targetwnd == m_cbSkirtState) {
				SwitchCheckboxWithEdit(m_cbSkirtState,m_edSkirtState);
			}
			else if(targetwnd == m_cbOutfitOverride) {
				SwitchCheckboxWithEdit(m_cbOutfitOverride,m_edOutfitOverride);
			}
			else if(targetwnd == m_rbClubOutfit || targetwnd == m_rbSchoolOutfit || targetwnd == m_rbSwimOutfit || targetwnd == m_rbSportOutfit) {
				//check wnd, uncheck current one
				if(targetwnd != m_currOutfitRb) {
					SendMessage(m_currOutfitRb,BM_SETCHECK,BST_UNCHECKED,0);
					SendMessage(targetwnd,BM_SETCHECK,BST_CHECKED,0);
					m_currOutfitRb = targetwnd; 
				}
			}
			else if(targetwnd == m_btApply) {
				loc_applyClothes = true;
				BYTE* table = (BYTE*)g_AA2RedrawFlagTable;
				//barClass+D8 is the outer flag,
				//flagtable+4 the other flag
				*(table + 4) = 1;
				BYTE* tmp = *(BYTE**)((BYTE*)loc_barClass + 0x80);
				*((BYTE*)tmp + 0xD8) = 1;
			}
			else if(targetwnd == m_edOutfitOverride || targetwnd == m_edOutfitState || targetwnd == m_edShirtState || targetwnd == m_edSkirtState) {
				LimitEditInt(targetwnd,0,255);
			}
		}
	}

	/**
		These functions return the user input, or -1 if this feature is disabled
				**/

	int GetClothingState() {
		return GetEditNumber(m_edOutfitState);
	}
	int GetShirtState() {
		if(SendMessage(m_cbShirtState, BM_GETCHECK,0,0) == BST_CHECKED) {
			return GetEditNumber(m_edShirtState);
		}
		return -1;
	}
	int GetSkirtState() {
		if (SendMessage(m_cbSkirtState,BM_GETCHECK,0,0) == BST_CHECKED) {
			return GetEditNumber(m_edSkirtState);
		}
		return -1;
	}
	int GetOutfitN() {
		if		(m_currOutfitRb == m_rbSchoolOutfit)	return 0;
		else if (m_currOutfitRb == m_rbSportOutfit)		return 1;
		else if (m_currOutfitRb == m_rbSwimOutfit)		return 2;
		else if (m_currOutfitRb == m_rbClubOutfit)		return 3;
		return 0;
	}
	int GetOutfitOverride() {
		if(SendMessage(m_cbOutfitOverride, BM_GETCHECK,0,0) == BST_CHECKED) {
			return GetEditNumber(m_edOutfitOverride);
		}
		return -1;
	}
};

ClothChoiceGUI g_SystemClothesGui;

void __cdecl SystemDialogAfterDialogInit(void* internclass,HWND wnd) {
	//[this+4C] = save button handle
	HWND saveButton = *(HWND*)((BYTE*)internclass + 0x4C);
	RECT rct = GetRelativeRect(saveButton);
	rct.top -= 20;
	rct.bottom -= 20;
	g_cbSystemKeepCardFace = CreateWindowExW(0,L"BUTTON",L"Keep Cardface",WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_AUTOCHECKBOX,
		rct.left,rct.top,rct.right-rct.left,(rct.bottom-rct.top)/2,wnd,0,(HINSTANCE)g_AA2Base,0);
	if(g_cbSystemKeepCardFace == NULL) {
		int error = GetLastError();
		LOGPRIO(Logger::Priority::ERR) << "Could not create cardface checkbox! error " << error << "\n";
	}
	HFONT font = (HFONT)SendMessageW(saveButton,WM_GETFONT,0,0);
	SendMessage(g_cbSystemKeepCardFace,WM_SETFONT,(WPARAM)font, TRUE);

	//somewhere here
	DWORD x = rct.left - (rct.right-rct.left)*2;
	DWORD y = rct.top - 420;

	g_SystemClothesGui.Initialize(wnd,x,y,font);

	y += g_SystemClothesGui.GetSizeY() + 10;

	g_gbSystemPose = CreateWindowExW(0,L"BUTTON",L"Pose",WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		x,y,410,50,wnd,0,(HINSTANCE)g_AA2Base,0);
	SendMessage(g_gbSystemPose,WM_SETFONT,(WPARAM)font,TRUE);;
	CreateUpDownControl(wnd,x+10,y+15,45,25,g_edSystemPose,g_udSystemPose);
	SendMessage(g_edSystemPose,WM_SETFONT,(WPARAM)font,TRUE);
	g_btSystemPose = CreateWindowExW(0,L"BUTTON",L"Change Pose",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		x+60,y+15,100,25,wnd,0,(HINSTANCE)g_AA2Base,0);
	SendMessage(g_btSystemPose,WM_SETFONT,(WPARAM)font,TRUE);
	g_cbSystemEyeTrack = CreateWindowExW(0,L"BUTTON",L"Eye Tracking",WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_AUTO3STATE,
		x+170,y+15,100,25,wnd,0,(HINSTANCE)g_AA2Base,0);
	SendMessage(g_cbSystemEyeTrack,WM_SETFONT,(WPARAM)font,TRUE);
	g_cbSystemLockPose = CreateWindowExW(0,L"BUTTON",L"No Default Stance",WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_AUTOCHECKBOX,
		x+270,y+15,130,25,wnd,0,(HINSTANCE)g_AA2Base,0);
	SendMessage(g_cbSystemLockPose,WM_SETFONT,(WPARAM)font,TRUE);

	SendMessage(g_cbSystemEyeTrack,BM_SETCHECK,BST_INDETERMINATE,0);
}

LRESULT __cdecl SystemDialogNotification(void* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	if(msg == POSEMESSAGE_ADDPOSE) {
		int diff = (int)wparam;
		SetEditNumber(g_edSystemPose,GetEditNumber(g_edSystemPose) + diff);
	}
	else if(msg == POSEMESSAGE_APPLYPOSE) {
		loc_playPos = GetEditNumber(g_edSystemPose);
		return 0;
	}
	else if(msg == WM_INITDIALOG) {
		
	}
	else if(msg == WM_COMMAND && lparam != 0) {
		HWND targetwnd = (HWND)lparam;
		DWORD ctrlId = LOWORD(wparam);
		DWORD notification = HIWORD(wparam);
		g_SystemClothesGui.DialogNotification(internclass,wnd,msg,wparam,lparam);
		if(notification == BN_CLICKED && targetwnd == g_btSystemPose) {
			loc_playPos = GetEditNumber(g_edSystemPose);
		}
	}
	return -1;
}

void __cdecl TopBarDialogNotification(void* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	//note that this catches all notifications, especially INITDIALOG, so this loc var is always valid
	loc_barClass = internclass;
}

struct CharacterPng {
	BYTE* buffer = NULL;
	DWORD dataStart = 0;
	DWORD size = 0;

	void BuildFromFile(const wchar_t* path) {
		if (buffer != NULL) {
			delete buffer;
			buffer = NULL; 
		}
		//bff+4=c03
		HANDLE file = CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if (file == NULL || file == INVALID_HANDLE_VALUE) return;
		DWORD tmp;
		DWORD size = GetFileSize(file,&tmp);
		BYTE* fileBuffer = new BYTE[size];
		ReadFile(file,fileBuffer,size,&tmp,NULL);
		CloseHandle(file);

		//so, here is what i could gather about how they save stuff:
		//after the IEND chunk, there is a field, total length 0xC03,
		//that holds the data. yes, after the IEND chunk.
		//after that, there is ANOTHER IEND chunk,
		//and then another 4 byte value of some sort.
		//seriously, you have pngs with their insane chunk feature
		//and illusion does this shit? i cant even

		//so, we are going to have to find the IEND chunk first
		DWORD iendOffset = 0;
		for (iendOffset = 8; iendOffset < size; iendOffset++) {
			DWORD chunkLength = *(DWORD*)(fileBuffer+iendOffset);
			chunkLength = _byteswap_ulong(chunkLength); //big endian
														//check chunk type
			if (strncmp((char*)fileBuffer+iendOffset+4,"IEND",4) == 0) {
				//found IEND
				break;
			}
			iendOffset += chunkLength + 12 - 1; //-1 cause i++ of for loop
			continue;
		}
		if (iendOffset >= size) {
			delete fileBuffer;
			return; //didnt find IEND Oo
		}
		iendOffset += 12; //skip iend offset (always has a length if 0 by definition)

						  //from experience, the following bytes should now follow:
						  //81 79 83 47 83 66 83 42 83 62 83 67 81 7A
						  //which are apparently supposed to be japanaese things in some encoding.
						  //cant read jack shit out of the binary, guess its oddly encoded or encrypted or some shit.

		const BYTE dataHeader[] = { 0x81, 0x79, 0x83, 0x47, 0x83, 0x66, 0x83, 0x42, 0x83, 0x62, 0x83, 0x67, 0x81, 0x7A, 0x00, 0x00 };
		for (int i = 0; i < sizeof(dataHeader); i++) if (fileBuffer[iendOffset+i] != dataHeader[i]) {
			//failed this test
			delete fileBuffer;
			return;
		}

		//great. second iend is gonna be at the end, im sure. not even gonna check, i really am sure.
		this->buffer = fileBuffer;
		this->dataStart = iendOffset;
		this->size = size;
	}

	~CharacterPng() {
		delete buffer;
	}
};

CharacterPng loc_lastOpened;
const wchar_t* loc_lastPath;

std::wstring BuildBackupName(const wchar_t* path, const wchar_t* postfixName, bool timestamp) {
	const std::wstring backupFolder = L"Backup\\";

	std::wstring backupPath(L"\\\\?\\");
	backupPath += path;
	backupPath.reserve(512);
	size_t filenameStart = backupPath.find_last_of(L'\\');
	size_t filenameEnd = backupPath.find_last_of(L'.');
	if (filenameStart == std::wstring::npos) {
		filenameStart = 0;
	}
	else filenameStart++;
	if(filenameEnd == std::wstring::npos) {
		filenameEnd = backupPath.size();
	}
	std::wstring filename = backupPath.substr(filenameStart,filenameEnd - filenameStart);
	backupPath.resize(filenameStart);

	std::wstringstream backupSuffix(L"");
	backupSuffix << backupFolder << filename << L"_" << postfixName;
	if(timestamp) {
		backupSuffix << L"_";
		SYSTEMTIME time;
		GetLocalTime(&time);
		
		backupSuffix << time.wYear << L".";
		backupSuffix << time.wMonth << L".";
		backupSuffix << time.wDay << L"[";
		backupSuffix << time.wHour << L";";
		backupSuffix << time.wMinute << L"](";
		backupSuffix << time.wSecond << L";";
		backupSuffix << time.wMilliseconds << L")";
	}
	backupSuffix << L".png";
	backupPath += backupSuffix.str();
	return backupPath;
}

void __cdecl BeforeCharacterOverwrite(const wchar_t* path) {
	loc_lastPath = path;
	if (path == NULL || path[0] == '\0') return;
	if(!g_config.IsDisabled(Config::DISABLE_PRE_BACKUP)) {
		//make backup
		std::wstring backupName = BuildBackupName(path,L"pre",!g_config.IsDisabled(Config::DISABLE_BACKUP_UNIQUE));
		int filenameStart = backupName.find_last_of(L'\\');
		if (filenameStart == std::wstring::npos) filenameStart = 0;
		std::wstring backupPath = backupName.substr(0,filenameStart);
		if(!DirExists(backupPath.c_str())) {
			BOOL ret = CreateDirectoryW((LPCWSTR)backupPath.c_str(),NULL);
			if (ret == FALSE) {
				int error = GetLastError();
				LOGPRIO(Logger::Priority::WARN) << "Failed to create backup folder: " << error << "\r\n";
			}
		}
		BOOL ret = CopyFileW((LPCWSTR)path,(LPCWSTR)backupName.c_str(),FALSE);
		if(ret == FALSE) {
			int error = GetLastError();
			LOGPRIO(Logger::Priority::WARN) << "Failed to create backup: " << error << "\r\n";
		}
	}
	if(SendMessageW(g_cbSystemKeepCardFace, BM_GETCHECK,0,0) == BST_UNCHECKED) {
		return;
	}
	loc_lastOpened.BuildFromFile(path);
}
void __cdecl AfterCharacterOverwrite() {
	if (loc_lastPath == NULL || loc_lastPath[0] == '\0') return;
	if (!g_config.IsDisabled(Config::DISABLE_POST_BACKUP)) {
		//make backup
		std::wstring backupName = BuildBackupName(loc_lastPath,L"post",!g_config.IsDisabled(Config::DISABLE_BACKUP_UNIQUE));
		int filenameStart = backupName.find_last_of(L'\\');
		if (filenameStart == std::wstring::npos) filenameStart = 0;
		std::wstring backupPath = backupName.substr(0,filenameStart);
		if (!DirExists(backupPath.c_str())) {
			BOOL ret = CreateDirectoryW((LPCWSTR)backupPath.c_str(),NULL);
			if (ret == FALSE) {
				int error = GetLastError();
				LOGPRIO(Logger::Priority::WARN) << "Failed to create backup folder: " << error << "\r\n";
			}
		}
		BOOL ret = CopyFileW((LPCWSTR)loc_lastPath,(LPCWSTR)backupName.c_str(),FALSE);
		if (ret == FALSE) {
			int error = GetLastError();
			LOGPRIO(Logger::Priority::WARN) << "Failed to create backup: " << error << "\r\n";
		}
	}
	if (SendMessageW(g_cbSystemKeepCardFace,BM_GETCHECK,0,0) == BST_UNCHECKED) {
		return;
	}
	if (loc_lastOpened.buffer == NULL) return;
	CharacterPng postPng;
	postPng.BuildFromFile(loc_lastPath);
	if (postPng.buffer == NULL) return;

	//we're gonna make us a third png with the data of the second and the ...non-data of the first
	HANDLE newFile = CreateFileW(loc_lastPath,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL);
	//write old files first part
	DWORD written;
	WriteFile(newFile,loc_lastOpened.buffer,loc_lastOpened.dataStart,&written,NULL);
	//write new files rest
	WriteFile(newFile,postPng.buffer + postPng.dataStart,postPng.size-postPng.dataStart,&written,NULL);

	CloseHandle(newFile);
}


void __cdecl SystemDialogCharacterCreated() {
	FacedetailsDialogOnCharacterLoad();
	BodycolorDialogOnCharacterLoad();
}

void __cdecl SystemDialogCharacterLoaded() {
	FacedetailsDialogOnCharacterLoad();
	BodycolorDialogOnCharacterLoad();
}

namespace {
	int loc_clothingShirtOverrideOriginal;
};

int __cdecl GetClothingState(int original) {
	if (loc_applyClothes) {
		return g_SystemClothesGui.GetClothingState();
	}
	return original;
}

int __cdecl GetOutfitToDisplay(int original) {
	if (loc_applyClothes) {
		return g_SystemClothesGui.GetOutfitN();
	}
	return original;
}
int __cdecl GetOverwriteOutfitToDisplay(int original) {
	if (loc_applyClothes) {
		int overrite = g_SystemClothesGui.GetOutfitOverride();
		if(overrite != -1) {
			return overrite;
		}
	}
	return original;
}
int __cdecl GetOutfitShirtState(int original) {
	if (loc_applyClothes) {
		loc_clothingShirtOverrideOriginal = original;
		int overrite = g_SystemClothesGui.GetShirtState();
		if (overrite != -1) {
			return overrite;
		}
	}
	return original;
}

int __cdecl GetOutfitSkirtState(int original) {
	if (loc_applyClothes) {
		int overrite = g_SystemClothesGui.GetSkirtState();
		if (overrite != -1) {
			return overrite;
		}
		return loc_clothingShirtOverrideOriginal; //cause original would be the shirt override in this case
	}
	return original;
}

void __cdecl AfterOutfitCallback() {
	loc_applyClothes = false;
}

int __cdecl GetPlayPosePosition() {
	if(loc_playPos != -1) {
		int tmp = loc_playPos;
		loc_playPos = -1;
		return tmp;
	}
	else if(SendMessageW(g_cbSystemLockPose, BM_GETCHECK,0,0) == BST_CHECKED) {
		return -2; //-2 means skip entirely
	}
	return -1; //-1 means do normally
}

int __cdecl GetEyeTrackState() {
	int state = SendMessageW(g_cbSystemEyeTrack,BM_GETCHECK,0,0);
	if (state == BST_INDETERMINATE) return -1;
	else if (state == BST_CHECKED) return 1;
	else return 0;
}