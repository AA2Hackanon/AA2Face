#include <Windows.h>
#include "Injections.h"
#include "ExternConstants.h"

namespace {
	//RAII memalloc class
	class Memrights {
	private:
		DWORD oldRights;
		LPVOID addr;
		DWORD size;
	public:
		bool good;
		Memrights(LPVOID addr,DWORD size) : addr(addr), size(size) {
			good = VirtualProtect(addr,size,PAGE_EXECUTE_READWRITE,&(this->oldRights)) == TRUE;
		}
		~Memrights() {
			DWORD tmp;
			if (good) VirtualProtect(addr,size,oldRights,&tmp);
		}
	};
};

extern "C" void facedialog_constructor_inject();
extern "C" void facedialog_refresh_face_inject();
extern "C" void facedialog_hooked_dialog_proc();
extern "C" void facedialog_load_face_inject();

void HookDialogCreationProcParam() {
	//AA2Edit.exe+22410 - 8B 44 24 08           - mov eax,[esp+08]
	//AA2Edit.exe+22414 - 3D 11010000           - cmp eax,00000111
	//AA2Edit.exe+22419 - 77 53                 - ja AA2Edit.exe+2246E
	//AA2Edit.exe+2241B - 74 2E                 - je AA2Edit.exe+2244B

	BYTE* modIt = (BYTE*)g_AA2Base + 0x22410;
	Memrights writeRights(modIt,9);
	if (writeRights.good) {
		*modIt = 0xE8;
		modIt++;
		DWORD offset = (DWORD)(facedialog_hooked_dialog_proc)-(DWORD)(modIt + 4);
		*(DWORD*)modIt = offset;
		modIt+=4;
		//4 nops now
		for (int i = 0; i < 4; i++) *(modIt++) = 0x90;
	}
	else {
		MessageBox(NULL,"Could not hook creation: could not get write rights","Error",0);
	}
}

void HookDialogCreation() {
	//AA2Edit.exe+2219F - FF 15 54436301 - call dword ptr [AA2Edit.exe+2C4354] (CreateDialogParamW)
	BYTE* modIt = (BYTE*)g_AA2Base + 0x2219F;
	Memrights writeRights(modIt,6);
	if(writeRights.good) {
		//opcode is E8, followed by 4 byte offset from end of this call instruction (little endian),
		//followed by a nop (because this one is FF 15 <4 byte offset> cause its a jump table call
		*modIt = 0xE8;
		modIt++;
		DWORD offset = (DWORD)(facedialog_constructor_inject) - (DWORD)(modIt + 4);
		*(DWORD*)modIt = offset;
		modIt += 4;
		*modIt = 0x90;
	} else {
		MessageBox(NULL,"Could not hook creation: could not get write rights","Error",0);
	}
}

void HookFaceChoose() {
	//AA2Edit.exe+222C7 - E8 D451FFFF - call AA2Edit.exe+174A0
	BYTE* modIt = (BYTE*)g_AA2Base + 0x222C7;
	Memrights writeRights(modIt,5);
	if(writeRights.good) {
		*modIt = 0xE8;
		modIt++;
		DWORD offset = (DWORD)(facedialog_refresh_face_inject)-(DWORD)(modIt + 4);
		*(DWORD*)modIt = offset;
	} else {
		MessageBox(NULL,"Could not hook refresh: could not get write rights","Error",0);
	}
}

void HookFaceLoad() {
	//AA2Edit.exe+223D1 - 8A 82 6A040000        - mov al,[edx+0000046A]
	//AA2Edit.exe+223D7 - 3C 0B                 - cmp al,0B
	//we do not only want to put a call into here,
	//but also return a 1 (mov al, 01 [B0 01], ret [C3])
	BYTE* modIt = (BYTE*)g_AA2Base + 0x223D1;
	Memrights writeRights(modIt,8);
	if(writeRights.good) {
		*modIt = 0xE8;
		modIt++;
		DWORD offset = (DWORD)(facedialog_load_face_inject)-(DWORD)(modIt + 4);
		*(DWORD*)modIt = offset;
		modIt += 4;
		*(modIt++) = 0xB0;
		*(modIt++) = 0x01;
		*(modIt++) = 0xC3; //the ret
	}
}