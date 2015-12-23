#include <Windows.h>
#include "Injections.h"
#include "ExternConstants.h"

namespace Opcodes {
	BYTE Call = 0xE8;
	BYTE Nop = 0x90;
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
		return false;
	}
}

extern "C" void facedialog_constructor_inject();
extern "C" void facedialog_refresh_face_inject();
extern "C" void facedialog_hooked_dialog_proc();
extern "C" void facedialog_load_face_inject();


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

}

//-------HAIR---------

extern "C" void hairdialog_constructor_inject();
extern "C" void hairdialog_refresh_hair_inject();
extern "C" void hairdialog_init_hair_inject();
extern "C" void hairdialog_hooked_dialog_proc();

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

