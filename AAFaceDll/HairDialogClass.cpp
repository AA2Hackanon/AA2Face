#include "ExternConstants.h"
#include "Logger.h"

//AA2Edit.exe+28460 - 8B 47 44              - mov eax,[edi+44]
//AA2Edit.exe+28463 - 8B 48 24              - mov ecx,[eax+24]
//AA2Edit.exe+28466 - 53                    - push ebx
//AA2Edit.exe+28467 - 33 DB                 - xor ebx,ebx
//AA2Edit.exe+28469 - 39 19                 - cmp[ecx],ebx
//AA2Edit.exe+2846B - 75 05                 - jne AA2Edit.exe+28472
//AA2Edit.exe+2846D - 8B 40 30              - mov eax,[eax+30]
//AA2Edit.exe+28470 - EB 03                 - jmp AA2Edit.exe+28475
//AA2Edit.exe+28472 - 8B 40 2C              - mov eax,[eax+2C]
//AA2Edit.exe+28475 - 3B C3                 - cmp eax,ebx
//AA2Edit.exe+28477 - 75 04                 - jne AA2Edit.exe+2847D
//AA2Edit.exe+28479 - 32 C0                 - xor al,al
//AA2Edit.exe+2847B - 5B                    - pop ebx
//AA2Edit.exe+2847C - C3                    - ret
//AA2Edit.exe+2847D - 8B 48 28              - mov ecx,[eax+28]
//AA2Edit.exe+28480 - 3B CB                 - cmp ecx,ebx
//AA2Edit.exe+28482 - 74 F5                 - je AA2Edit.exe+28479
BYTE * HairDialogClass::HairOfTab(int tab)
{
	if (tab < 0 || tab > 3) {
		LOGPRIO(Logger::Priority::WARN) << "invalid tab parameter " << tab << "\n";
		return NULL;
	}
	/*BYTE* internclass = (BYTE*)this;
	DWORD eax1 = *(DWORD*)(internclass+0x44);
	DWORD ecx1 = *(DWORD*)((BYTE*)eax1 + 0x24);
	DWORD cmpval = *(DWORD*)ecx1;
	DWORD eax2;
	if (cmpval == 0) {
	eax2 = *(DWORD*)((BYTE*)eax1 + 0x30);
	}
	else {
	eax2 = *(DWORD*)((BYTE*)eax1 + 0x2C);
	}
	if (eax2 == 0) return NULL;
	DWORD ecx2 = *(DWORD*)((BYTE*)eax2 + 0x28);
	if (ecx2 == 0) return NULL;*/
	DWORD ecx2 = (DWORD)GetChoiceDataBuffer();
	BYTE* hairPtr = (BYTE*)ecx2 + 0x69C + tab; //note: +0x96C + 8 would be flip bool
	return hairPtr;
}

BYTE* HairDialogClass::FlipBoolOfTab(int tab)
{
	if (tab < 0 || tab > 3) {
		LOGPRIO(Logger::Priority::WARN) << "invalid tab parameter " << tab << "\n";
		return NULL;
	}
	DWORD ecx2 = (DWORD)GetChoiceDataBuffer();
	BYTE* hairPtr = (BYTE*)ecx2 + 0x69C + 8 + tab;
	return hairPtr;
}

//note: [[ecx+14] + (hairslot << 4)] = HWND of button with ecx = [edi+48] (see above)
HWND HairDialogClass::GetHairSlotButton(BYTE slot) const {
	if (slot >= GetButtonCount()) {
		LOGPRIO(Logger::Priority::WARN) << "tried to get button for slot " << slot << ", but button count is "
			<< GetButtonCount() << "\n";
		return NULL; //button does not exist
	}
	/*BYTE* classptr = (BYTE*)this;
	BYTE* ecx = (BYTE*)*(DWORD*)(classptr + 0x48);
	BYTE* ptr1 = (BYTE*)*(DWORD*)(ecx + 0x14);
	DWORD offset = slot << 4;*/
	return GetButtonListWnd(0x48,slot);
}

HWND HairDialogClass::GetAdjustmentSliderWnd() const
{
	//[esi+294], esi is this
	return *(HWND*)((BYTE*)this + 0x294);
}

HWND HairDialogClass::GetAdjustmentSliderEdit() const
{
	//[esi+298]
	return *(HWND*)((BYTE*)this + 0x298);
}



//AA2Edit.exe+173E0 - 8B 51 18              - mov edx,[ecx+18]
//AA2Edit.exe+173E3 - 2B 51 14              - sub edx,[ecx+14]
//AA2Edit.exe+173E6 - 53                    - push ebx
//AA2Edit.exe+173E7 - 8A 5C 24 08           - mov bl,[esp+08]
//AA2Edit.exe+173EB - C1 FA 04              - sar edx,04
//AA2Edit.exe+173EE - 3B C2                 - cmp eax,edx		edx becomes button count, eax is slot
int HairDialogClass::GetButtonCount() const {
	//a much easier method seems to be [edi+60]
	/*BYTE* classptr = (BYTE*)this;
	BYTE* ecx = (BYTE*)*(DWORD*)(classptr + 0x48);
	DWORD edx = *(DWORD*)(ecx + 0x18);
	edx -= *(DWORD*)(ecx + 0x14);
	edx >>= 4;
	return edx;*/
	return GetButtonListCount(0x48);
}

//mov esi, [edi+4C] edi is hair class
//mov ecx, [esi+24] ecx is hair size edit field
HWND HairDialogClass::GetHairSizeEditWnd() const {
	BYTE* classptr = (BYTE*)this;
	BYTE* esi = *(BYTE**)(classptr+0x4C);
	HWND edHairSize = *(HWND*)(esi+0x24);
	return edHairSize;
}

//esi is this
//AA2Edit.exe+33983 - 8B B5 C0000000        - mov esi,[ebp+000000C0]
//AA2Edit.exe+33989 - 80 BE C4050000 00     - cmp byte ptr[esi+000005C4],00		//flag 1, apparently
//AA2Edit.exe+33990 - 0F84 F6000000         - je AA2Edit.exe+33A8C
//AA2Edit.exe+33996 - E8 1547FFFF           - call AA2Edit.exe+280B0
//AA2Edit.exe+3399B - C6 86 C4050000 00     - mov byte ptr[esi+000005C4],00
//AA2Edit.exe+339A2 - 8B 4D 3C              - mov ecx,[ebp+3C]					//and that second value again
//AA2Edit.exe+339A5 - 80 79 19 00           - cmp byte ptr[ecx+19],00
//AA2Edit.exe+339A9 - C6 44 24 17 01        - mov byte ptr[esp+17],01
//AA2Edit.exe+339AE - C6 44 24 16 01        - mov byte ptr[esp+16],01
//AA2Edit.exe+339B3 - C6 44 24 18 01        - mov byte ptr[esp+18],01
//AA2Edit.exe+339B8 - 74 25                 - je AA2Edit.exe+339DF
void HairDialogClass::SetHairChangeFlags(int tab) {
	if (tab < 0 ||tab > 3) {
		LOGPRIO(Logger::Priority::WARN) << "invalid tab parameter " << tab << "\n";
		return;
	}
	//still no idea what this this second flag is there for,
	//still just gonna set it and hope nothing breaks
	BYTE* classptr = (BYTE*)this;
	*(classptr + 0x5C4) = 1;
	//BYTE* tablePtr = *(BYTE**)(classptr + 0x3C); //this should be the same as g_AA2RedrawFlagTable
	BYTE* tablePtr = (BYTE*)g_AA2RedrawFlagTable;
	if (tab == 0) *(tablePtr + 0x16) = 1; //flag for front hair
	else if (tab == 3) *(tablePtr + 0x17) = 1; //flag for hair extension
	else if (tab == 1) *(tablePtr + 0x18) = 1; //flag for side hair
	else if (tab == 2) *(tablePtr + 0x19) = 1; //flag for back hair
											   //(now that i think about it, they are probably for the 4 tabs, respectively. whatever.)

}

//esi is this class
//AA2Edit.exe+28C02 - 8B 8E 9C020000        - mov ecx,[esi+0000029C]
//AA2Edit.exe+28C08 - 69 D2 88000000        - imul edx,edx,00000088		edx is tab
//AA2Edit.exe+28C0E - 03 D7                 - add edx,edi
//AA2Edit.exe+28C10 - 0FB6 84 32 A1020000   - movzx eax,byte ptr[edx+esi+000002A1]	this seems to be flip-bool-array
//AA2Edit.exe+28C18 - 50                    - push eax
//AA2Edit.exe+28C19 - 51                    - push ecx
//AA2Edit.exe+28C1A - FF D3                 - call ebx	this is EnableWindow (no idea why its loaded in ebx)
HWND HairDialogClass::GetFlipButtonWnd() const {
	return *(HWND*)((BYTE*)this + 0x29C);
}

//SHIT, THESE ONLY WORK FOR THE 134 THAT ARE DISPLAYED
//edi is the HairDialog class
//ecx is the tab and given as parameter (0-3 again)
//AA2Edit.exe+29BAB - 8B 4C 24 14           - mov ecx,[esp+14]
//AA2Edit.exe+29BAF - C1 E1 04              - shl ecx,04
//AA2Edit.exe+29BB2 - 8B 84 39 08050000     - mov eax,[ecx+edi+00000508]
//AA2Edit.exe+29BB9 - 8D 1C 39              - lea ebx,[ecx+edi]
//sets hair buttons to disable/enable depending on wether this slot exists (ecx is bool) (on tab switch) (only custom buttons tho)
//AA2Edit.exe+29BF8 - 8B 93 88050000        - mov edx,[ebx+00000588]
//AA2Edit.exe+29BFE - 80 3C 16  00          - cmp byte ptr[esi+edx],00	esi is hair slot
//AA2Edit.exe+29C02 - 0F95 C0               - setne al
//AA2Edit.exe+29C05 - 0FB6 C8               - movzx ecx,al
//AA2Edit.exe+29C08 - 51                    - push ecx
//AA2Edit.exe+29C09 - 8B 4F 48              - mov ecx,[edi+48]
//AA2Edit.exe+29C0C - 8B C6                 - mov eax,esi
//AA2Edit.exe+29C0E - E8 CDD7FEFF           - call AA2Edit.exe+173E0
const BYTE* HairDialogClass::GetHairSlotExistsField(int tab) const {
	if (tab < 0 || tab > 3) {
		LOGPRIO(Logger::Priority::WARN) << "invalid tab parameter " << tab << "\n";
		return NULL; //we only have these 4 tabs
	}
										 //i could write this a little more idiomatic, but i want to go 100% sure that this function does
										 //exactly what the asm code does.
	BYTE* classptr = (BYTE*)this;
	BYTE* somePtr = classptr + (tab << 4);
	BYTE* slotField = (BYTE*)*(DWORD*)(somePtr + 0x588);
	return slotField;
}


//unfortunately, this field is only generated for the 134 tabs visible. If i force him to
//generate it for 255, it crashes (for the last tab, anyway)
bool HairDialogClass::HairSlotExists(BYTE slot,int tab) const {
	const BYTE* slotField = GetHairSlotExistsField(tab);
	if (slotField == NULL) return false;
	BYTE b = *(slotField + slot);
	return b != 0;
}

//AA2Edit.exe+D9FEF - 3B F7                 - cmp esi,edi
//AA2Edit.exe+D9FF1 - 74 0E                 - je AA2Edit.exe+DA001 jump if hair does not exist, somewhere nowhere.
//sources:
//this makes edi. esp+40 is a parameter which appears to be tab+2 [2 for front, 3 for side, 4 for back, 5 for extension]
//AA2Edit.exe+D9F47 - 8B 44 24 40           - mov eax,[esp+40]
//AA2Edit.exe+D9F4B - 8D 50 02              - lea edx,[eax+02]
//AA2Edit.exe+D9F4E - C1 E0 04              - shl eax,04
//AA2Edit.exe+D9F51 - 8B 7C 08 24           - mov edi,[eax+ecx+24]	<--- here is edi
//AA2Edit.exe+D9F55 - 8B 44 24 4C           - mov eax,[esp+4C]
//AA2Edit.exe+D9F59 - 50                    - push eax
//AA2Edit.exe+D9F5A - 51                    - push ecx
//AA2Edit.exe+D9F5B - C1 E2 04              - shl edx,04
//AA2Edit.exe+D9F5E - 8B 2C 0A              - mov ebp,[edx+ecx]
//now were missing ecx from that. ecx is this and comes from
//AA2Edit.exe+118F1F - 8B 47 0C              - mov eax,[edi+0C]			edi = HairDialogClass
//AA2Edit.exe+118F27 - 8B 88 9C0A0000        - mov ecx,[eax+00000A9C]
//and this makes esi. the call takes 3 parameters, but doesnt clear  them from the stack (neither does the caller, for some reason)
//AA2Edit.exe+D9FB3 - 57                    - push edi				<-- we allready have this one
//AA2Edit.exe+D9FB4 - 89 46 0C              - mov[esi+0C],eax
//AA2Edit.exe+D9FB7 - 8D 44 24 60           - lea eax,[esp+60]
//AA2Edit.exe+D9FBB - 55                    - push ebp				<--- can get from above
//AA2Edit.exe+D9FBC - 50                    - push eax				<--- param. apparently, function returns this.
//					apparently, its a something**, and function will write into *something the value
//AA2Edit.exe+D9FBD - E8 2E0B0000           - call AA2Edit.exe+DAAF0
//AA2Edit.exe+D9FC2 - 8B 30                 - mov esi,[eax]
//edi is constant, so the function has to do the magic. i have to have overseen a parameter there.
/*bool HairDialogClass::HairSlotExists2(BYTE slot,int tab) {
if (tab < 0 || tab > 3) return false;
BYTE* classptr = (BYTE*)this;
DWORD* (__cdecl *ptrFunction)(void**, DWORD, DWORD);
ptrFunction = (DWORD* (__cdecl*)(void**,DWORD,DWORD))(g_AA2Base + 0xDAAF0);
//so, lets start with edi and ebp
DWORD eax = *(DWORD*)(classptr + 0x40);
DWORD ecx = *(DWORD*)((BYTE*)eax + 0xA9C); //thats the eax part
eax = (tab + 2) << 4;
DWORD edi = *(DWORD*)((BYTE*)ecx + eax + 0x24);
return edi == esi; //i really hope this works.
}*/