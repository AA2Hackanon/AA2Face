#pragma once
#include <Windows.h>
extern const char* g_strModuleName;
extern "C" DWORD g_AA2Base;
extern DWORD g_AA2RedrawFlagTable;

/*
 * Emulates known behavior of the class used in the hair dialog init
 * functions etc. i should do one of those for face as well
 */
class HairDialogClass {
public:
	BYTE* HairOfTab(int tab);
	void SetHairChangeFlags();
};

void ExternInit();

//functions we should be hooking:

//constructor of face dialog:
//AA2Edit.exe+22193 - 68 90AB5200           - push AA2Edit.exe+19AB90 //push dialog handler (not that its really used much)
//[...]
//AA2Edit.exe+2219F - FF 15 54436301        - call dword ptr [AA2Edit.exe+2C4354] (CreateDialogParamW)
//where ecx = param = hInst (pushed right before call)

//call that deterimnes which image has been clicked:
//AA2Edit.exe+222C7 - E8 D451FFFF           - call AA2Edit.exe+174A0
//return value is then put into variable that will be used to select the face
//AA2Edit.exe+222CC - 88 86 6A040000        - mov[esi+0000046A],al

//start of the dialog handler (general function, redirects to virtual call later i think)
//AA2Edit.exe+19AB90 - 53                    - push ebx
//AA2Edit.exe+19AB91 - 8B 5C 24 08           - mov ebx,[esp+08]
//AA2Edit.exe+19AB95 - 55                    - push ebp
//AA2Edit.exe+19AB96 - 56                    - push esi
//AA2Edit.exe+19AB97 - 57                    - push edi
//AA2Edit.exe+19AB98 - 68 A4004600           - push AA2Edit.exe+3100A4
//AA2Edit.exe+19AB9D - 53                    - push ebx
//AA2Edit.exe+19AB9E - FF 15 60434100        - call dword ptr[AA2Edit.exe+2C4360]

//starting of said virtual handler (the one for the face part)
//AA2Edit.exe+22410 - 8B 44 24 08           - mov eax,[esp+08]
//AA2Edit.exe+22414 - 3D 11010000           - cmp eax,00000111
//AA2Edit.exe+22419 - 77 53                 - ja AA2Edit.exe+2246E
//AA2Edit.exe+2241B - 74 2E                 - je AA2Edit.exe+2244B

//in the system menu, this is a call to the WM_COMMAND handler (no thiscall!)
//void(?) handler(sysclass* c, HWND cwnd (for some reason), LPARAM lparam)
//controll identifier of the new character button seems to be 2731, while open file is 272F
//AA2Edit.exe+1BB19 - E8 22000000           - call AA2Edit.exe+1BB40

//part of a function which, at character load, limits the face to 0B if (BYTE)face > 0xB (in a really
//stupid inefficient way). we can use this function both to set our face in the selection field,
//and to let it actually load the other faces.
//this function should return 1 on success, and failure can only occurr with nullpointers, which cant be
//at this point anymore. we should probably just return 1.
//AA2Edit.exe+223D1 - 8A 82 6A040000        - mov al,[edx+0000046A]
//AA2Edit.exe+223D7 - 3C 0B                 - cmp al,0B
//AA2Edit.exe+223D9 - 77 0B                 - ja AA2Edit.exe+223E6
//AA2Edit.exe+223DB - 0FB6 C8               - movzx ecx,al

/////////////////////////////

//constructor of hair dialog:
//AA2Edit.exe+27EEC - FF 15 54435B00        - call dword ptr[AA2Edit.exe+2C4354]

//start of virtual handler of hair-dialog
//AA2Edit.exe+28650 - 8B 44 24 08           - mov eax,[esp+08]
//AA2Edit.exe+28654 - 56                    - push esi
//AA2Edit.exe+28655 - 57                    - push edi
//AA2Edit.exe+28656 - 8B F9                 - mov edi,ecx
//AA2Edit.exe+28658 - 3D 11010000           - cmp eax,00000111


//WM_COMMAND part of the virtual handler of the hair-dialog. 
//AA2Edit.exe+286BC - 8B 44 24 14           - mov eax,[esp+14]
//AA2Edit.exe+286C0 - 50                    - push eax <-- lparam
//AA2Edit.exe+286C1 - 57                    - push edi <-- class ptr
//AA2Edit.exe+286C2 - E8 59000000           - call AA2Edit.exe+28720


//setting hair after button press or when tabs are switched. edx e [0:3] depending on tab
//AA2Edit.exe+280DA - E8 C1F3FEFF           - call AA2Edit.exe+174A0
//AA2Edit.exe+280DF - 0FB6 96 A0020000      - movzx edx,byte ptr[esi+000002A0]
//AA2Edit.exe+280E6 - 88 84 1A 9C060000     - mov[edx+ebx+0000069C],al

//AA2Edit.exe+1C1C7 - 8B BD C0000000        - mov edi,[ebp+000000C0] (open file)
//AA2Edit.exe+1C1CD - E8 8EC20000           - call AA2Edit.exe+28460
//,
//AA2Edit.exe+1C454 - 8B BD C0000000        - mov edi,[ebp+000000C0] (new character)
//AA2Edit.exe+1C45A - E8 01C00000           - call AA2Edit.exe+28460 (this=edi)
//this part is apparently limiting back hair on load (note how 69C became 69E because back hair has ebx=2).
//there are similar parts before and after this one for the other tabs
//note that edi is the this pointer, the same which is used in the virtual handler.
//if we change the border to 255 on creation, he creates one more icon only. that sucks.
//cause that would have been really nice.
//also, ebx is 0 in this context
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
//AA2Edit.exe+28484 - 0FB6 91 9C060000      - movzx edx,byte ptr[ecx+0000069C]
//[...]
//AA2Edit.exe+2853B - 0FB6 91 9E060000      - movzx edx,byte ptr[ecx+0000069E]		this loads the face index into edx
//AA2Edit.exe+28542 - 8B 77 6C              - mov esi,[edi+6C]
//AA2Edit.exe+28545 - 4E                    - dec esi								this seems to be the maximum allowed index (135)
//AA2Edit.exe+28546 - 3B F2                 - cmp esi,edx
//AA2Edit.exe+28548 - 7C 1D                 - jnge AA2Edit.exe+28567					take this jump if border < hair index
//AA2Edit.exe+2854A - 8B B7 A8050000        - mov esi,[edi+000005A8]
//AA2Edit.exe+28550 - 38 1C 32              - cmp[edx+esi],bl						no idea what this is. looks like a bit field. full of 1s.
//AA2Edit.exe+28553 - 75 22                 - jne AA2Edit.exe+28577
//AA2Edit.exe+28555 - 38 58 40              - cmp[eax+40],bl
//AA2Edit.exe+28558 - 0F95 C0               - setne al
//AA2Edit.exe+2855B - 8D 44 00 01           - lea eax,[eax+eax+01]
//AA2Edit.exe+2855F - 88 81 9E060000        - mov[ecx+0000069E],al
//AA2Edit.exe+28565 - EB 10                 - jmp AA2Edit.exe+28577
//AA2Edit.exe+28567 - 38 58 40              - cmp[eax+40],bl
//AA2Edit.exe+2856A - 0F95 C2               - setne dl
//AA2Edit.exe+2856D - 8D 54 12 01           - lea edx,[edx+edx+01]
//AA2Edit.exe+28571 - 88 91 9E060000        - mov[ecx+0000069E],dl
//AA2Edit.exe+28577 - 0FB6 81 9E060000      - movzx eax,byte ptr[ecx+0000069E]
//used borders:
//[edi+60] for ebx=0
//[edi+64] for ebx=1
//[edi+6C] for ebx=2
//[edi+68] for ebx=3 (now that makes sense, doesnt it...)
//changing them afterwards seems to cause crashes though (presumably cause he expects the buttons to be there?)
//how about we try to nop the nge jumps, maybe that works
//AA2Edit.exe+28492 - 7C 0B                 - jnge AA2Edit.exe+2849F
//AA2Edit.exe+284FD - 7C 0B                 - jnge AA2Edit.exe+2850A
//AA2Edit.exe+28548 - 7C 1D                 - jnge AA2Edit.exe+28567
//AA2Edit.exe+285CE - 7C 0B                 - jnge AA2Edit.exe+285DB
