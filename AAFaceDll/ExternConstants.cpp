#include <Windows.h>
#include "ExternConstants.h"

const char* g_strModuleName = "AA2Edit.exe";
DWORD g_AA2Base = 0;
DWORD g_AA2RedrawFlagTable;

void ExternInit() {
	g_AA2Base = (DWORD)GetModuleHandle("AA2Edit.exe");
	g_AA2RedrawFlagTable = g_AA2Base + 0x353160;
}

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
	if (tab < 0 || tab > 3) return NULL;
	BYTE* internclass = (BYTE*)this;
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
	if (ecx2 == 0) return NULL;
	BYTE* hairPtr = (BYTE*)ecx2 + 0x69C + tab;
	return hairPtr;
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
void HairDialogClass::SetHairChangeFlags() {
	//still no idea what this this second flag is there for,
	//still just gonna set it and hope nothing breaks
	BYTE* classptr = (BYTE*)this;
	*(classptr + 0x5C4) = 1;
	BYTE* tablePtr = *(BYTE**)(classptr + 0x3C); //this should be the same as g_AA2RedrawFlagTable
	*(tablePtr + 0x19) = 1; //19 seems to be flag for hair
	*(tablePtr + 0x16) = 1; //apparently i also need to set the flag for 16
}