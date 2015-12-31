#include <Windows.h>
#include <fstream>
#include "InjGeneral.h"
#include "Logger.h"

HACCEL g_acKeys = NULL;
HWND g_dummyHotkeyWnd = NULL;


namespace {
	bool loc_keyNotProcessed = false;

	/*
	 * If focused is dialog1 or dialog2 or a child of either dialog1 or dialog2
	 */
	bool IsChildOrEquals(HWND focused, HWND dialog1) {
		return focused == dialog1 || IsChild(dialog1,focused);
	}
	bool IsChildOrEquals(HWND focused, HWND dialog1, HWND dialog2) {
		return focused == dialog1 || focused == dialog2 || IsChild(dialog1,focused) || IsChild(dialog2,focused);
	}

	bool CheckKeyContext(const Config::Hotkey& keyfunc, HWND focus) {
		//check context
		switch (keyfunc.context) {
		case Config::Hotkey::ALL:
			//always valid
			break;
		case Config::Hotkey::DIALOG_SYSTEM:
		case Config::Hotkey::DIALOG_FIGURE:
		case Config::Hotkey::DIALOG_CHEST:
		case Config::Hotkey::DIALOG_BODY_COLOR:
		case Config::Hotkey::DIALOG_FACE:
		case Config::Hotkey::DIALOG_EYES:
		case Config::Hotkey::DIALOG_EYE_COLOR:
		case Config::Hotkey::DIALOG_EYEBROWS:
		case Config::Hotkey::DIALOG_FACE_DETAILS:
		case Config::Hotkey::DIALOG_HAIR:
		case Config::Hotkey::DIALOG_HAIR_COLOR:
		case Config::Hotkey::DIALOG_CHARACTER:
		case Config::Hotkey::DIALOG_PERSONALITY:
		case Config::Hotkey::DIALOG_TRAITS:
			if (g_AA2DialogHandles[keyfunc.context] == NULL) return false;
			if(keyfunc.contextKind == Config::Hotkey::ACTIVE) {
				if(g_AA2GetCurrentDialogTab() != keyfunc.context) {
					return false;
				}
			}
			else if(keyfunc.contextKind == Config::Hotkey::FOCUS) {
				if (g_AA2GetCurrentDialogTab() != keyfunc.context) {
					return false;
				}
				if (!IsChildOrEquals(focus,g_AA2DialogHandles[keyfunc.context],*g_AA2UpperDialogHandle)) {
					g_Logger << Logger::Priority::INFO << "Window " << g_AA2DialogHandles[keyfunc.context] <<
						" was not in focus\r\n\t";
					HWND wnd = g_AA2DialogHandles[keyfunc.context];
					do {
						g_Logger << wnd << " -> ";
						wnd = GetParent(wnd);
					} while (wnd != NULL);
					g_Logger << "\r\n\t";
					wnd = *g_AA2UpperDialogHandle;
					do {
						g_Logger << wnd << " -> ";
						wnd = GetParent(wnd);
					} while (wnd != NULL);
					g_Logger << "\r\n\t";
					wnd = focus;
					do {
						g_Logger << wnd << " -> ";
						wnd = GetParent(wnd);
					} while (wnd != NULL);
					return false;
				}
				else {
					g_Logger << Logger::Priority::INFO << "Window " << g_AA2DialogHandles[keyfunc.context] <<
						" was in focus\r\n\t";
					HWND wnd = g_AA2DialogHandles[keyfunc.context];
					do {
						g_Logger << wnd << " -> ";
						wnd = GetParent(wnd);
					} while (wnd != NULL);
					g_Logger << "\r\n\t";
					wnd = *g_AA2UpperDialogHandle;
					do {
						g_Logger << wnd << " -> ";
						wnd = GetParent(wnd);
					} while (wnd != NULL);
					g_Logger << "\r\n\t";
					wnd = focus;
					do {
						g_Logger << wnd << " -> ";
						wnd = GetParent(wnd);
					} while (wnd != NULL);
				}
			}
			else {
				g_Logger << Logger::Priority::WARN << "Unrecognized context kind " << keyfunc.contextKind <<
					" for hotkey " << keyfunc.key << "\r\n";
			}
			break;
		case Config::Hotkey::DIALOG_ALL: {
			if (g_AA2DialogHandles[0] == NULL) return false;
			bool focused = false;
			for (int i = 0; i < 14; i++) {
				if (IsChildOrEquals(focus, g_AA2DialogHandles[i],*g_AA2UpperDialogHandle)) {
					focused = true;
					break;
				}
			}
			if (!focused) return false;
			break; }
		case Config::Hotkey::PREVIEW_WINDOW:
			if(keyfunc.contextKind == Config::Hotkey::ACTIVE) {
				if (!IsWindowEnabled(*g_AA2MainWndHandle)) {
					return false;
				}
			}
			else if(keyfunc.contextKind == Config::Hotkey::FOCUS) {
				if (focus != *g_AA2MainWndHandle) {
					return false;
				}
			}
			break;
		default:
			g_Logger << Logger::Priority::WARN << "Unrecognized context value " << keyfunc.context <<
				" for hotkey " << keyfunc.key << "\r\n";
		}
		return true;
	}

	void ExecuteKeyFunction(const Config::Hotkey& keyfunc) {
		switch (keyfunc.func) {
		case Config::Hotkey::HAIR_FLIP:
			//hair dialog needs to exist
			if (g_AA2DialogHandles[9] == NULL) break;
			SendMessageW(g_AA2DialogHandles[9],HAIRMESSAGE_FLIPHAIR,0,0);
			break;
		case Config::Hotkey::HAIR_ADD:
			if (g_AA2DialogHandles[9] == NULL) break;
			SendMessageW(g_AA2DialogHandles[9],HAIRMESSAGE_ADDHAIR,(WPARAM)keyfunc.iparam,0);
			break;
		case Config::Hotkey::HAIR_SET:
			if (g_AA2DialogHandles[9] == NULL) break;
			SendMessageW(g_AA2DialogHandles[9],HAIRMESSAGE_SETHAIR,(WPARAM)keyfunc.iparam,0);
			break;
		case Config::Hotkey::HAIR_SIZEADD:
			if (g_AA2DialogHandles[9] == NULL) break;
			SendMessageW(g_AA2DialogHandles[9],HAIRMESSAGE_HAIRSIZEADD,(WPARAM)keyfunc.iparam,0);
			break;
		case Config::Hotkey::HAIR_SIZESET:
			if (g_AA2DialogHandles[9] == NULL) break;
			SendMessageW(g_AA2DialogHandles[9],HAIRMESSAGE_HAIRSIZESET,(WPARAM)keyfunc.iparam,0);
			break;
		case Config::Hotkey::FACEDETAILS_ADDGLASSES:
			if (g_AA2DialogHandles[8] == NULL) break;
			SendMessageW(g_AA2DialogHandles[8],FACEDETAILSMESSAGE_ADDGLASSES,(WPARAM)keyfunc.iparam,0);
			break;
		case Config::Hotkey::FACEDETAILS_SETGLASSES:
			if (g_AA2DialogHandles[8] == NULL) break;
			SendMessageW(g_AA2DialogHandles[8],FACEDETAILSMESSAGE_SETGLASSES,(WPARAM)keyfunc.iparam,0);
			break;
		case Config::Hotkey::BODYCOLOR_ADDTAN:
			if (g_AA2DialogHandles[3] == NULL) break;
			SendMessageW(g_AA2DialogHandles[8],BODYCOLORMESSAGE_ADDTAN,(WPARAM)keyfunc.iparam,0);
			break;
		case Config::Hotkey::BODYCOLOR_SETTAN:
			if (g_AA2DialogHandles[3] == NULL) break;
			SendMessageW(g_AA2DialogHandles[8],BODYCOLORMESSAGE_SETTAN,(WPARAM)keyfunc.iparam,0);
			break;
		case Config::Hotkey::ZOOM_ADD:
			if (g_AA2MainWndHandle == NULL) break;
			*g_AA2CurrZoom += keyfunc.fparam;
			break;
		case Config::Hotkey::ZOOM_SET:
			if (g_AA2MainWndHandle == NULL) break;
			*g_AA2CurrZoom = keyfunc.fparam;
			break;
		case Config::Hotkey::TILT_ADD: {
			if (g_AA2MainWndHandle == NULL) break;
			float* tiltptr = g_AA2GetTilt();
			if (tiltptr != NULL) {
				float tmpTilt = *tiltptr + keyfunc.fparam;
				if (tmpTilt > *g_AA2MaxTilt) {
					tmpTilt -= *g_AA2MaxTilt;
				}
				else if (tmpTilt < 0) {
					tmpTilt += *g_AA2MaxTilt;
				}
				*tiltptr = tmpTilt;
			}
			break; }
		case Config::Hotkey::TILT_SET: {
			if (g_AA2MainWndHandle == NULL) break;
			float* tiltptr = g_AA2GetTilt();
			if (tiltptr != NULL) {
				*tiltptr = keyfunc.fparam;
			}
			break; }
		case Config::Hotkey::NOP:
			//nop, just put it here so the logger doesnt complain
			break;
		default:
			g_Logger << Logger::Priority::WARN << "Unrecognized function value " << keyfunc.func <<
				" for hotkey " << keyfunc.key << "\r\n";
		}
	}
}

LRESULT CALLBACK DummyHotkeyWndProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	if (msg == WM_COMMAND && lparam == 0 && HIWORD(wparam) == 1) {
		//make sure we dont catch this key if the focus is in an edit field
		HWND focus = GetFocus();
		wchar_t classname[64];
		int length = GetClassNameW(focus,classname,64);
		if(length != 0) {
			if(_wcsicmp(classname, L"EDIT") == 0) {
				loc_keyNotProcessed = true;
				return DefWindowProcW(hwnd,msg,wparam,lparam);
			}
		}
		unsigned int i = LOWORD(wparam); //identifier
		const std::vector<Config::Hotkey>& keyfuncs = g_config.GetHotkeys();
		const Config::Hotkey& firstkey = keyfuncs[i];
		if(keyfuncs[i].key == -1) {
			g_Logger << Logger::Priority::WARN << "Function with invalid key executed.\r\n";
		}
		else {
			for (i; i < keyfuncs.size() && keyfuncs[i].SameKey(firstkey); i++) {
				const Config::Hotkey& keyfunc = keyfuncs[i];
				if (CheckKeyContext(keyfunc,focus)) {
					ExecuteKeyFunction(keyfunc);
				}
			}
		}
		return 0;
	}
	loc_keyNotProcessed = true;
	return DefWindowProcW(hwnd, msg, wparam, lparam);
}

void __cdecl PreGetMessageHook() {
	//create fake window for keys
	//(for some reason, i have to create it here it seems. else, TranslateAccelerator fails and gives Invalid handle value error.
	WNDCLASSEXW mc;
	memset(&mc,0,sizeof(WNDCLASSEXW));
	mc.lpszClassName = L"AA2EditKeyMessageWindow";
	mc.lpfnWndProc = DummyHotkeyWndProc;
	mc.hInstance = (HINSTANCE)g_AA2Base;
	mc.cbSize = sizeof(WNDCLASSEXW);
	RegisterClassExW(&mc);

	g_dummyHotkeyWnd = CreateWindowExW(0,L"AA2EditKeyMessageWindow",NULL,0,0,0,0,0,HWND_MESSAGE,0,(HINSTANCE)g_AA2Base,0);
	if (g_dummyHotkeyWnd == NULL) {
		int error = GetLastError();
		g_Logger << Logger::Priority::ERR << "Could not create Hotkey-Window: Code " << error << "\r\n";
	}
}

//return 0 to skip translate/dispatch message calls
int __cdecl GetMessageHook(MSG* msg) {
	if (g_acKeys != NULL) {
		if(TranslateAcceleratorW(g_dummyHotkeyWnd,g_acKeys,msg)) {
			if(loc_keyNotProcessed) {
				loc_keyNotProcessed = false;
				return 1;
			}
			return 0; //skip translate / disptach message calls etc
		}
		else {
			int i = GetLastError();
		}
	}
	return 1;
}
