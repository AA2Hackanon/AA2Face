#include <Windows.h>
#include "Hooks.h"
#include "ExternConstants.h"
#include "Logger.h"
#include "Config.h"

namespace Opcodes {
	BYTE Call = 0xE8;
	BYTE Nop = 0x90;
	BYTE Jmp = 0xE9;
};


//RAII memalloc class
Memrights::Memrights(LPVOID addr,DWORD size) : addr(addr), size(size) {
	good = VirtualProtect(addr,size,PAGE_EXECUTE_READWRITE,&(this->oldRights)) == TRUE;
}
Memrights::~Memrights() {
	DWORD tmp;
	if (good) VirtualProtect(addr,size,oldRights,&tmp);
}

bool Hook(BYTE* code,int nBytes,std::initializer_list<std::initializer_list<DWORD>> ops) {
	using namespace Opcodes;
	Memrights writeRights(code,nBytes);
	if(writeRights.good) {
		for(const auto& outerIt : ops) {
			DWORD first = *outerIt.begin();
			bool call = false;
			if (first == Call) {
				//next will be an offset
				call = true;
			}
			for(DWORD it : outerIt) {
				if(call && it != Call) {
					//this is the offset
					call = false;
					DWORD offset = it -(DWORD)(code + 4);
					*(DWORD*)code = offset;
					code += 4;
				} else {
					*code++ = (BYTE)it;
				}
			}
		}
		return true;
	}
	else {
		LOGPRIO(Logger::Priority::ERR) << "Failed to hook location " << code << "\n";
		return false;
	}
}

extern "C" void getmessage_hook();
extern "C" void pregetmessage_hook();

/*
AA2Edit.exe+427F - 53                    - push ebx
AA2Edit.exe+4280 - 53                    - push ebx
AA2Edit.exe+4281 - 53                    - push ebx
AA2Edit.exe+4282 - 8D 44 24 40           - lea eax,[esp+40]
AA2Edit.exe+4286 - 50                    - push eax
AA2Edit.exe+4287 - FF 15 0C444E01        - call dword ptr[AA2Edit.exe+2C440C] <-- GetMessageW
AA2Edit.exe+428D - 85 C0                 - test eax,eax*/

void HookGeneral() {
	using namespace Opcodes;
	BYTE* AA2Base = (BYTE*)g_AA2Base;
	bool ret = true;
	ret &= Hook(AA2Base + 0x4287,6,{ {Call, (DWORD)getmessage_hook}, {Nop} });

	//AA2Edit.exe+425B - E8 10CC1800           - call AA2Edit.exe+190E70
	//AA2Edit.exe+4260 - 8B 3D 10444601        - mov edi,[AA2Edit.exe+2C4410]
	//AA2Edit.exe+4266 - 8B 35 00444601        - mov esi,[AA2Edit.exe+2C4400]
	//AA2Edit.exe+426C - 8D 64 24 00           - lea esp,[esp+00]
	ret &= Hook(AA2Base + 0x425B,5,{{Call, (DWORD)pregetmessage_hook }});

	if(!ret) {
		LOGPRIO(Logger::Priority::CRIT_ERR) << "At least one code modification failed in General hooks!\n";
	}
}

//-------FACE---------

extern "C" void facedialog_constructor_inject();
extern "C" void facedialog_refresh_face_inject();
extern "C" void facedialog_hooked_dialog_proc();
extern "C" void facedialog_load_face_inject();
extern "C" void facedialog_hooked_dialog_proc_afterinit();

void HookFace() {
	using namespace Opcodes;
	BYTE* AA2Base = (BYTE*)g_AA2Base;
	bool ret = true;
	//things we want to hook:
	//1: creation of the face-dialog, to also create our combo box:
	//AA2Edit.exe+2219F - FF 15 54436301        - call dword ptr [AA2Edit.exe+2C4354] (CreateDialogParamW)
	ret &= Hook(AA2Base + 0x2219F,6,{ {Call, (DWORD)facedialog_constructor_inject }, {Nop} });

	//2: load of the face-dialog to set our combo box to the loaded face
	//3: limit of the load face to make it actually load faces beyond 6
	//(these 2 are done in the same hook)
	//AA2Edit.exe+223D1 - 8A 82 6A040000        - mov al,[edx+0000046A]
	//AA2Edit.exe+223D7 - 3C 0B                 - cmp al,0B
	//we do not only want to put a call into here,
	//but also return a 1 (mov al, 01 [B0 01], ret [C3])
	ret &= Hook(AA2Base + 0x223D1,5,{ { Call, (DWORD)facedialog_load_face_inject },{ 0xB0, 0x01, 0xC3 } });

	//4: the (periodic) call to the function which determines which face is currently selected
	//(so that we can maybe return our combo box value instead)
	//AA2Edit.exe+222C7 - E8 D451FFFF           - call AA2Edit.exe+174A0
	ret &= Hook(AA2Base + 0x222C7,5,{ {Call, (DWORD)facedialog_refresh_face_inject } });

	//5: the dialogs message handler, so we can see if our edit box or one of the default buttons were clicked
	//AA2Edit.exe+22410 - 8B 44 24 08           - mov eax,[esp+08]
	//AA2Edit.exe+22414 - 3D 11010000           - cmp eax,00000111
	ret &= Hook(AA2Base + 0x22410,9,{ { Call, (DWORD)facedialog_hooked_dialog_proc },{ Nop, Nop, Nop, Nop } });

	//6: the WM_INITDIALOG handler for more initialsation stuff
	//AA2Edit.exe+22429 - 51                    - push ecx
	//AA2Edit.exe+2242A - E8 81FDFFFF           - call AA2Edit.exe+221B0
	ret &= Hook(AA2Base + 0x2242A,5,{ {Call, (DWORD)facedialog_hooked_dialog_proc_afterinit} });

	if (!ret) {
		LOGPRIO(Logger::Priority::CRIT_ERR) << "At least one code modification failed in Face hooks!\n";
	}
}

//-------HAIR---------

extern "C" void hairdialog_constructor_inject();
extern "C" void hairdialog_refresh_hair_inject();
extern "C" void hairdialog_init_hair_inject();
extern "C" void hairdialog_hooked_dialog_proc();
extern "C" void hairdialog_hooked_dialog_proc_afterinit();
extern "C" void hairdialog_invalid_hair_loaded();
extern "C" void hairdialog_hooked_loadhairinfo();

void HookHair() {
	using namespace Opcodes;
	BYTE* AA2Base = (BYTE*)g_AA2Base;
	bool ret = true;
	//things we want to hook:
	//1: creation of hair-dialog to create our edit field
	//AA2Edit.exe+27EEC - FF 15 54435B00        - call dword ptr[AA2Edit.exe+2C4354]
	ret &= Hook(AA2Base + 0x27EEC,6,{ {Call, (DWORD)hairdialog_constructor_inject }, { Nop } });

	//2: load of the hair-dialogs settings when a character is loaded or created so that we can
	//adjust our edits value accordingly
	//3: the limit function (again, both done in the same hook)
	//AA2Edit.exe+1C1CD - E8 8EC20000           - call AA2Edit.exe+28460 (this=edi) (open file)
	//and AA2Edit.exe+1C45A - E8 01C00000       - call AA2Edit.exe+28460 (this=edi) (create new character)
	ret &= Hook(AA2Base + 0x1C1CD,5,{ {Call, (DWORD)hairdialog_init_hair_inject } });
	ret &= Hook(AA2Base + 0x1C45A,5,{ { Call, (DWORD)hairdialog_init_hair_inject } });

	//4: the (periodic) call to the function which determines which hair is currently selected
	//AA2Edit.exe+280DA - E8 C1F3FEFF           - call AA2Edit.exe+174A0
	//AA2Edit.exe+280DF - 0FB6 96 A0020000      - movzx edx,byte ptr[esi+000002A0]
	//AA2Edit.exe+280E6 - 88 84 1A 9C060000     - mov[edx+ebx+0000069C],al
	ret &= Hook(AA2Base + 0x280DA, 5, { { Call, (DWORD)hairdialog_refresh_hair_inject } });

	//5: the dialog boxes message handler (remember to repilicate those instructions)
	//AA2Edit.exe+28650 - 8B 44 24 08           - mov eax,[esp+08]
	//AA2Edit.exe+28654 - 56                    - push esi
	ret &= Hook(AA2Base + 0x28650,5,{ { Call, (DWORD)hairdialog_hooked_dialog_proc } });

	//6: also, the WM_DIALOGINIT call; more precisely, after it, so we can do
	//stuff on the windows that it creates
	//AA2Edit.exe+28674 - E8 97F8FFFF           - call AA2Edit.exe+27F10
	ret &= Hook(AA2Base + 0x28674,5,{ { Call, (DWORD)hairdialog_hooked_dialog_proc_afterinit} });

	//7: some way to find out if a hair is valid. this jump seems to be taken whenever a hair is valid
	//AA2Edit.exe+119CF2 - 74 0C                 - je AA2Edit.exe+119D00
	//AA2Edit.exe+119CF4 - 8B 4C 24 1C           - mov ecx,[esp+1C]
	//AA2Edit.exe+119CF8 - C6 84 0E 5C020000 00  - mov byte ptr[esi+ecx+0000025C],00
	//AA2Edit.exe+119D00 - 33 D2                 - xor edx,edx
	/*if(!g_config.GetDisabledHairSkipInvalid()) {
		ret &= Hook(AA2Base + 0x119CF8,8,{ { Call, (DWORD)hairdialog_invalid_hair_loaded },{ Nop, Nop, Nop } });
	}*/
	//got a better one thats actually working. this one gets infos about all hairs of all tabs. these are our targets:
	//AA2Edit.exe+28CF0 - E8 4B050F00           - call AA2Edit.exe+119240
	//AA2Edit.exe+28ECA - E8 71030F00           - call AA2Edit.exe+119240
	//AA2Edit.exe+29180 - E8 BB000F00           - call AA2Edit.exe+119240
	//AA2Edit.exe+294BB - E8 80FD0E00           - call AA2Edit.exe+119240
	if (!g_config.IsDisabled(Config::DISABLE_HAIR_SKIPINVALID)) {
		ret &= Hook(AA2Base + 0x28CF0,5,{ { Call, (DWORD)hairdialog_hooked_loadhairinfo } });
		ret &= Hook(AA2Base + 0x28ECA,5,{ { Call, (DWORD)hairdialog_hooked_loadhairinfo } });
		ret &= Hook(AA2Base + 0x29180,5,{ { Call, (DWORD)hairdialog_hooked_loadhairinfo } });
		ret &= Hook(AA2Base + 0x294BB,5,{ { Call, (DWORD)hairdialog_hooked_loadhairinfo } });
	}
	
	if (!ret) {
		LOGPRIO(Logger::Priority::CRIT_ERR) << "At least one code modification failed in Hair hooks!\n";
	}
}

extern "C" void facedetails_hooked_dialog_proc();
extern "C" void facedetails_hooked_dialog_proc_afterinit();
extern "C" void facedetails_afterinit();
extern "C" void facedetails_refresh_glasses_inject();
extern "C" void facedetails_refresh_lipcolor_inject();
extern "C" void facedetails_constructor_inject();
extern "C" void facedetails_init_hair_inject();

void HookFacedetails() {
	using namespace Opcodes;
	BYTE* AA2Base = (BYTE*)g_AA2Base;
	bool ret = true;

	//1: Dialog proc
	//AA2Edit.exe+273D0 - 55                    - push ebp
	//AA2Edit.exe+273D1 - 8B EC                 - mov ebp,esp
	//AA2Edit.exe+273D3 - 83 E4 F8              - and esp,-08
	ret &= Hook(AA2Base + 0x273D0,6,{ {Call, (DWORD)facedetails_hooked_dialog_proc}, {Nop} });


	//2: End of WM_INITDIALOG handler (note that HWNDs dont exist at this point yet)
	//AA2Edit.exe+273F9 - 57                    - push edi
	//AA2Edit.exe+273FA - E8 F1EFFFFF           - call AA2Edit.exe+263F0
	ret &= Hook(AA2Base + 0x273FA,5,{ {Call, (DWORD)facedetails_hooked_dialog_proc_afterinit} });
	

	//3: THe end of the actual initialisation (cause that isnt done after WM_INITIDIALOG. fuck you all.)
	//dont need this anymore
	//AA2Edit.exe+32708 - 55                    - push ebp
	//AA2Edit.exe+32709 - E8 82020000           - call AA2Edit.exe+32990
	ret &= Hook(AA2Base + 0x32709,5,{ {Call, (DWORD)facedetails_afterinit} });

	//4: The place where the current glasses are polled from
	//AA2Edit.exe+26D6D - 8B 7D 54              - mov edi,[ebp+54]
	//AA2Edit.exe+26D70 - E8 2B07FFFF           - call AA2Edit.exe+174A0
	//AA2Edit.exe+26D75 - 88 83 95060000        - mov[ebx+00000695],al
	ret &= Hook(AA2Base + 0x26D70,5,{ { Call, (DWORD)facedetails_refresh_glasses_inject } });

	//lip color poll place
	/*
	AA2Edit.exe+26D7B - 8B 7D 58              - mov edi,[ebp+58]
	AA2Edit.exe+26D7E - E8 1D07FFFF           - call AA2Edit.exe+174A0
	AA2Edit.exe+26D83 - 88 83 96060000        - mov[ebx+00000696],al
	*/
	ret &= Hook(AA2Base + 0x26D7E,5,{ { Call, (DWORD)facedetails_refresh_lipcolor_inject } });
	
	//5: Constructor of dialog again
	//AA2Edit.exe+2631A - FF 15 54435B01        - call dword ptr[AA2Edit.exe+2C4354]
	ret &= Hook(AA2Base + 0x2631A,6,{ {Call, (DWORD)facedetails_constructor_inject }, {Nop}});
	
	//6: facedetail limitations on load
	//AA2Edit.exe+1C44F - E8 9CAC0000           - call AA2Edit.exe+270F0 (create)
	//AA2Edit.exe+1C1C2 - E8 29AF0000           - call AA2Edit.exe+270F0 (on load)
	ret &= Hook(AA2Base + 0x1C44F,5,{ { Call, (DWORD)facedetails_init_hair_inject } });
	ret &= Hook(AA2Base + 0x1C1C2,5,{ { Call, (DWORD)facedetails_init_hair_inject } });
	
	if (!ret) {
		LOGPRIO(Logger::Priority::CRIT_ERR) << "At least one code modification failed in Facedetails hooks!\n";
	}
}

extern "C" void bodycolor_hooked_dialog_proc();
extern "C" void bodycolor_hooked_dialog_proc_afterinit();
extern "C" void bodycolor_refresh_tan_inject();
extern "C" void  bodycolor_refresh_nipcolor_inject();
extern "C" void  bodycolor_refresh_niptype_inject();
extern "C" void  bodycolor_refresh_pubhair_inject();
extern "C" void  bodycolor_refresh_mosaic_inject();
extern "C" void bodycolor_init_hair_inject();

void HookBodycolor() {
	using namespace Opcodes;
	BYTE* AA2Base = (BYTE*)g_AA2Base;
	bool ret = true;

	//1: Dialog Proc
	//AA2Edit.exe+21350 - 55                    - push ebp
	//AA2Edit.exe+21351 - 8B EC                 - mov ebp,esp
	//AA2Edit.exe+21353 - 83 E4 F8              - and esp,-08
	ret &= Hook(AA2Base + 0x21350,6,{ {Call, (DWORD)bodycolor_hooked_dialog_proc }, {Nop} });

	//2: after WM_INITDIALOG
	//AA2Edit.exe+21374 - 57                    - push edi
	//AA2Edit.exe+21375 - E8 76E9FFFF           - call AA2Edit.exe+1FCF0
	ret &= Hook(AA2Base + 0x21375,5,{ { Call, (DWORD)bodycolor_hooked_dialog_proc_afterinit } });

	//3: function that selects the current tan
	//AA2Edit.exe+20B51 - 8B 7B 54              - mov edi,[ebx+54]
	//AA2Edit.exe+20B54 - E8 4769FFFF           - call AA2Edit.exe+174A0
	//AA2Edit.exe+20B59 - 88 86 67040000        - mov[esi+00000467],al
	ret &= Hook(AA2Base + 0x20B54,5,{ {Call, (DWORD)bodycolor_refresh_tan_inject } });

	//nip type and color poll (in this order)
	/*
	AA2Edit.exe+20B12 - 8B 7B 48              - mov edi,[ebx+48]
	AA2Edit.exe+20B15 - E8 8669FFFF           - call AA2Edit.exe+174A0
	AA2Edit.exe+20B1A - 88 86 64040000        - mov [esi+00000464],al
	AA2Edit.exe+20B20 - 8B 7B 4C              - mov edi,[ebx+4C]
	AA2Edit.exe+20B23 - E8 7869FFFF           - call AA2Edit.exe+174A0
	AA2Edit.exe+20B28 - 88 86 65040000        - mov [esi+00000465],al
	*/
	ret &= Hook(AA2Base + 0x20B15,5,{ { Call, (DWORD)bodycolor_refresh_niptype_inject } });
	ret &= Hook(AA2Base + 0x20B23,5,{ { Call, (DWORD)bodycolor_refresh_nipcolor_inject } });

	//mosaic and pub hair poll (in this order). mosaic flag is 11, pub hair flag is B
	/*
	AA2Edit.exe+20B82 - 8B 7B 5C              - mov edi,[ebx+5C]
	AA2Edit.exe+20B85 - E8 1669FFFF           - call AA2Edit.exe+174A0
	AA2Edit.exe+20B8A - 88 86 69040000        - mov [esi+00000469],al
	AA2Edit.exe+20B90 - 8B 7B 60              - mov edi,[ebx+60]
	AA2Edit.exe+20B93 - E8 0869FFFF           - call AA2Edit.exe+174A0
	AA2Edit.exe+20B98 - 88 86 5C040000        - mov [esi+0000045C],al
	*/
	ret &= Hook(AA2Base + 0x20B85,5,{ { Call, (DWORD)bodycolor_refresh_mosaic_inject } });
	ret &= Hook(AA2Base + 0x20B93,5,{ { Call, (DWORD)bodycolor_refresh_pubhair_inject } });

	//AA2Edit.exe+1C185 - 8B 85 90000000        - mov eax,[ebp+00000090]
	//AA2Edit.exe+1C18B - E8 304E0000           - call AA2Edit.exe+20FC0
	//AA2Edit.exe+1C412 - 8B 85 90000000        - mov eax,[ebp+00000090]
	//AA2Edit.exe+1C418 - E8 A34B0000           - call AA2Edit.exe+20FC0
	ret &= Hook(AA2Base + 0x1C18B,5,{ { Call, (DWORD)bodycolor_init_hair_inject } });
	ret &= Hook(AA2Base + 0x1C418,5,{ { Call, (DWORD)bodycolor_init_hair_inject } });

	if(!ret) {
		LOGPRIO(Logger::Priority::CRIT_ERR) << "At least one modification failed in Bodycolor hooks!\n";
	}
}

extern "C" void systemdialog_hooked_dialog_proc_afterinit();
extern "C" void systemdialog_hooked_dialog_proc();
extern "C" void systemdialog_character_created();
extern "C" void systemdialog_character_opened();
extern "C" void systemdialog_before_character_saved();
extern "C" void systemdialog_after_character_saved();

extern "C" void systemdialog_bar_hooked_dialog_proc();
extern "C" void systemdialog_load_character_3dobject();
extern "C" void systemdialog_load_change_outfit();
extern "C" void systemdialog_load_overwrite_outfit();
extern "C" void systemdialog_load_shirt_state();
extern "C" void systemdialog_load_skirt_state();
extern "C" void systemdialog_load_restore_skirt_state();
extern "C" void systemdialog_pose_cancel_hook();
extern "C" void systemdialog_pose_eye_track();

void HookSystem() {
	using namespace Opcodes;
	BYTE* AA2Base = (BYTE*)g_AA2Base;
	bool ret = true;
	//last call of (successful) open file dialog click:
	//AA2Edit.exe+1C26C - E8 9F73FEFF           - call AA2Edit.exe+3610
	ret &= Hook(AA2Base + 0x1C26C,5,{ {Call, (DWORD)systemdialog_character_opened} });

	//exit (last jump) of create character button
	//AA2Edit.exe+1C4C1 - E9 97FAFFFF           - jmp AA2Edit.exe+1BF5D
	ret &= Hook(AA2Base + 0x1C4C1,5,{ { Call, (DWORD)systemdialog_character_created } });

	//before saving character (create file call)
	//AA2Edit.exe+1261D5 - FF 15 B041C600        - call dword ptr[AA2Edit.exe+2C41B0]{ ->kernel32.CreateFileW }
	ret &= Hook(AA2Base + 0x1261D5,6,{ { Call, (DWORD)systemdialog_before_character_saved }, {Nop} });

	//after saving character (close file call)
	//AA2Edit.exe+12620A - FF 15 8441C600        - call dword ptr [AA2Edit.exe+2C4184] { ->kernel32.CloseHandle }
	ret &= Hook(AA2Base + 0x12620A,6,{ { Call, (DWORD)systemdialog_after_character_saved },{ Nop } });

	//init dialog to create our check box
	//AA2Edit.exe+1BAEC - E8 FFFCFFFF           - call AA2Edit.exe+1B7F0
	ret &= Hook(AA2Base + 0x1BAEC,5,{ { Call, (DWORD)systemdialog_hooked_dialog_proc_afterinit } });

	//dialog proc to see button presses (especially for cloth stuff)
	//AA2Edit.exe+1BAD0 - 8B 44 24 08           - mov eax,[esp+08]
	//AA2Edit.exe+1BAD4 - 3D 11010000           - cmp eax,00000111
	ret &= Hook(AA2Base + 0x1BAD0,9,{ { Call, (DWORD)systemdialog_hooked_dialog_proc }, {Nop,Nop,Nop,Nop} });
	

	//tab bar dialog proc (need it for clothes flag)
	//AA2Edit.exe+34160 - 8B 44 24 08           - mov eax,[esp+08]
	//AA2Edit.exe+34164 - 56                    - push esi
	//AA2Edit.exe+34165 - 8B F1                 - mov esi,ecx
	//AA2Edit.exe+34167 - 3D 10010000           - cmp eax,00000110
	ret &= Hook(AA2Base + 0x34160,5,{ { Call, (DWORD)systemdialog_bar_hooked_dialog_proc } });

	
	//cloth hooks:
	//AA2Edit.exe+332E9 - 51                    - push ecx
	//AA2Edit.exe+332EA - 8B CB                 - mov ecx,ebx
	//AA2Edit.exe+332EC - FF D2                 - call edx
	ret &= Hook(AA2Base + 0x332E9,5,{ { Call, (DWORD)systemdialog_load_character_3dobject } });


	//al contains the outfit that is to be shown (0 = schoool, 1 = sport, 2 = bath, 3 = club)
	/*AA2Edit.exe+10059F - 8A 43 44              - mov al,[ebx+44]
	AA2Edit.exe+1005A2 - 8B 73 28              - mov esi,[ebx+28]*/
	ret &= Hook(AA2Base + 0x10059F,6,{ { Call, (DWORD)systemdialog_load_change_outfit },{Nop} });

	/*AA2Edit.exe+1005D3 - 8A 10                 - mov dl,[eax]
	AA2Edit.exe+1005D5 - 89 44 24 38           - mov [esp+38],eax { writes pointer to struct that contains cloth slot to
	wear (dword ptr [eax] = slot). note that chaning this slot will change clothes when saved. note that merely changing
	dl will not suffice, [eax] needs to be changed temporarily
	}
	*/
	ret &= Hook(AA2Base + 0x1005D3,6,{ { Call, (DWORD)systemdialog_load_overwrite_outfit },{Nop} });
	
	//directly before the shirt and legs are opened. ebp+8 is cloth state from above
	/*AA2Edit.exe+1006DA - 89 44 24 30           - mov[esp+30],eax
	AA2Edit.exe+1006DE - 8B FF                 - mov edi,edi*/
	ret &= Hook(AA2Base + 0x1006DA,6,{ { Call, (DWORD)systemdialog_load_shirt_state }, {Nop} });

	//the test is the first instruction to look at the skirt
	/*AA2Edit.exe+10190C - E8 5F9D0100           - call AA2Edit.exe+11B670			: 1 parameter, no thiscall
	AA2Edit.exe+101911 - F6 45 08 01           - test byte ptr [ebp+08],01 { 1 }
	*/
	ret &= Hook(AA2Base + 0x10190C,5,{ { Call, (DWORD)systemdialog_load_skirt_state } });

	//the first instruction to stop looking at the skirt
	/*AA2Edit.exe+101C29 - 8B 43 30              - mov eax,[ebx+30]
	AA2Edit.exe+101C2C - 8B 40 28              - mov eax,[eax+28]*/
	ret &= Hook(AA2Base + 0x101C29,6,{ { Call, (DWORD)systemdialog_load_restore_skirt_state },{ Nop } });

	/*AA2Edit.exe+1006FB - 83 3F 03              - cmp dword ptr [edi],03 { 3 }
	AA2Edit.exe+1006FE - 0F8F 87000000         - jg AA2Edit.exe+10078B { specialy handleded clothes?*/
	//to
	/*AA2Edit.exe+1006FE - E9 88000000           - jmp AA2Edit.exe+10078B
	AA2Edit.exe+100703 - 90                    - nop */
	ret &= Hook(AA2Base + 0x1006FE,6,{ { 0xE9, 0x88, 0x00, 0x00, 0x00, 0x90} });
	
	//pose cancelation-hook and the limit breaker:
	/*AA2Edit.exe+33BEE - 80 7C 24 16 00        - cmp byte ptr [esp+16],00 { 0 }*/
	ret &= Hook(AA2Base + 0x33BEE,5,{ { Call, (DWORD)systemdialog_pose_cancel_hook } });
	//AA2Edit.exe+35FD3 - 0F87 D0000000         - ja AA2Edit.exe+360A9
	//the last jump of the switch ends at the eax,3 (for some retarded reason),
	//i have to make it jump after that instead, like this
	/*AA2Edit.exe+35FEA - 77 24                 - ja AA2Edit.exe+36010
	AA2Edit.exe+35FEC - 90                    - nop
	AA2Edit.exe+35FED - 90                    - nop
	AA2Edit.exe+35FEE - 90                    - nop
	AA2Edit.exe+35FEF - 90                    - nop*/
	ret &= Hook(AA2Base + 0x35FEA,6,{ { 0x77, 0x24, 0x90, 0x90, 0x90, 0x90} });
	
	//eye track hook
	/*AA2Edit.exe+1ADFA2 - 66 39 9E 0E100000     - cmp[esi+0000100E],bx
	AA2Edit.exe+1ADFA9 - 75 06                 - jne AA2Edit.exe+1ADFB1*/
	ret &= Hook(AA2Base + 0x1ADFA2,7,{ { Call, (DWORD)systemdialog_pose_eye_track }, {Nop, Nop} });

	if (!ret) {
		LOGPRIO(Logger::Priority::CRIT_ERR) << "At least one modification failed in System hooks!\n";
	}
}

//this also doesnt work. crashes the game.
void HookHairMaxAmount() {
	BYTE* AA2Base = (BYTE*)g_AA2Base;
	bool ret = true;
	//this part limits, at construction, the amount of front-hairs.
	//AA2Edit.exe+28D10 - 3D 87000000           - cmp eax,00000087
	//AA2Edit.exe+28D1B - 3D 87000000           - cmp eax,00000087
	//AA2Edit.exe+28D22 - B8 87000000           - mov eax,00000087
	ret &= Hook(AA2Base + 0x28D11,4,{ {0xFE, 00, 00, 00} });
	ret &= Hook(AA2Base + 0x28D1C,4,{ { 0xFE, 00, 00, 00 } });
	ret &= Hook(AA2Base + 0x28D23,4,{ { 0xFE, 00, 00, 00 } });
	//similar locations:
	//AA2Edit.exe+28EEA - 3D 87000000           - cmp eax,00000087
	//AA2Edit.exe+28EF5 - 3D 87000000           - cmp eax,00000087
	//AA2Edit.exe+28EFC - B8 87000000           - mov eax,00000087
	ret &= Hook(AA2Base + 0x28EEB,4,{ { 0xFE, 00, 00, 00 } });
	ret &= Hook(AA2Base + 0x28EF6,4,{ { 0xFE, 00, 00, 00 } });
	ret &= Hook(AA2Base + 0x28EFD,4,{ { 0xFE, 00, 00, 00 } });
	//AA2Edit.exe+291A0 - 3D 87000000           - cmp eax,00000087
	//AA2Edit.exe+291AB - 3D 87000000           - cmp eax,00000087
	//AA2Edit.exe+291B2 - B8 87000000           - mov eax,00000087
	ret &= Hook(AA2Base + 0x291A1,4,{ { 0xFE, 00, 00, 00 } });
	ret &= Hook(AA2Base + 0x291AC,4,{ { 0xFE, 00, 00, 00 } });
	ret &= Hook(AA2Base + 0x291B3,4,{ { 0xFE, 00, 00, 00 } });
	//AA2Edit.exe+294DB - 3D 87000000           - cmp eax,00000087
	//AA2Edit.exe+294E6 - 3D 87000000           - cmp eax,00000087
	//AA2Edit.exe+294ED - B8 87000000           - mov eax,00000087
	/*ret &= Hook(AA2Base + 0x294DC,4,{ { 0xFE, 00, 00, 00 } });
	ret &= Hook(AA2Base + 0x294E7,4,{ { 0xFE, 00, 00, 00 } });
	ret &= Hook(AA2Base + 0x294EE,4,{ { 0xFE, 00, 00, 00 } });*/
}

void HookLimits() {
	BYTE* AA2Base = (BYTE*)g_AA2Base;
	bool ret = true;
	//so, since jmps are relative, its always 7F 09 -> EB 12
	//locations to look at:
	//nip:
	//AA2Edit.exe+20B37 - 7F 09                 - jg AA2Edit.exe+20B42
	ret &= Hook(AA2Base + 0x20B37,2,{ {0xEB },{ 0x12 } });
	//tan:
	//AA2Edit.exe+20B68 - 7F 09                 - jg AA2Edit.exe+20B73
	ret &= Hook(AA2Base + 0x20B68,2,{ { 0xEB },{ 0x12 } });
	//pu hair:
	//AA2Edit.exe+20BA8 - 7F 09                 - jg AA2Edit.exe+20BB3
	ret &= Hook(AA2Base + 0x20BA8,2,{ { 0xEB },{ 0x12 } });
	//brow:
	//AA2Edit.exe+25BAA - 7F 09                 - jg AA2Edit.exe+25BB5
	ret &= Hook(AA2Base + 0x25BAA,2,{ { 0xEB },{ 0x12 } });
	//lips:
	//AA2Edit.exe+26D92 - 7F 09                 - jg AA2Edit.exe+26D9D
	ret &= Hook(AA2Base + 0x26D92,2,{ { 0xEB },{ 0x12 } });
	//the pu hair, brow and lip values are reset to the 100 bound at load,
	//the other 3 were allready fixed by the unlimited launcher.
	//so we will have to change that as well.
	//and i want to change it that way, so i can enable/disable these limits without
	//being dependend on the other injects
	//brow:
	/*AA2Edit.exe+25E78 - 8A 83 8D060000        - mov al,[ebx+0000068D]
	AA2Edit.exe+25E7E - 3C 64                 - cmp al,64 { 100 }
	AA2Edit.exe+25E80 - 77 0B                 - ja AA2Edit.exe+25E8D
	AA2Edit.exe+25E82 - 0FB6 C8               - movzx ecx,al
	AA2Edit.exe+25E85 - 85 C9                 - test ecx,ecx
	AA2Edit.exe+25E87 - 7E 10                 - jle AA2Edit.exe+25E99
	AA2Edit.exe+25E89 - 3C 64                 - cmp al,64 { 100 }
	AA2Edit.exe+25E8B - 76 07                 - jna AA2Edit.exe+25E94
	AA2Edit.exe+25E8D - B8 64000000           - mov eax,00000064 { 100 }
	AA2Edit.exe+25E92 - EB 07                 - jmp AA2Edit.exe+25E9B
	AA2Edit.exe+25E94 - 0FB6 C0               - movzx eax,al
	AA2Edit.exe+25E97 - EB 02                 - jmp AA2Edit.exe+25E9B
	AA2Edit.exe+25E99 - 33 C0                 - xor eax,eax*/
	//gotta EB 19 this time
	ret &= Hook(AA2Base + 0x25E80,2,{ { 0xEB },{ 0x19 } });
	//pu hair (not copying all this time, just EB 19 it)
	//AA2Edit.exe+2111A - 77 0B                 - ja AA2Edit.exe+21127
	ret &= Hook(AA2Base + 0x2111A,2,{ { 0xEB },{ 0x19 } });
	//lip
	//AA2Edit.exe+271F4 - 77 0B                 - ja AA2Edit.exe+27201
	ret &= Hook(AA2Base + 0x271F4,2,{ { 0xEB },{ 0x19 } });

}

void HookTEMPTEST() {
	//AA2Edit.exe+28492 - 7C 0B                 - jnge AA2Edit.exe+2849F
	//AA2Edit.exe+284FD - 7C 0B                 - jnge AA2Edit.exe+2850A
	//AA2Edit.exe+28548 - 7C 1D                 - jnge AA2Edit.exe+28567
	//AA2Edit.exe+285CE - 7C 0B                 - jnge AA2Edit.exe+285DB
	DWORD places[] = { 0x28492, 0x284FD, 0x28548, 0x285CE };
	for (int i = 0; i < sizeof(places)/sizeof(places[0]); i++) {
		BYTE* modIt = (BYTE*)g_AA2Base + places[i];
		Memrights writeRights(modIt,2);
		if (writeRights.good) {
			*modIt = 0x90;
			modIt++;
			*modIt = 0x90;
		}
		else {
			MessageBox(NULL,"Could not hook hair load: could not get write rights","Error",0);
		}
	}

}

