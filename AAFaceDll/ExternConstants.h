#pragma once
#include <Windows.h>
extern const char* g_strModuleName;
extern "C" DWORD g_AA2Base;

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


//WM_COMMAND part of the virtual handler of the hair-dialog
//AA2Edit.exe+286BC - 8B 44 24 14           - mov eax,[esp+14]
//AA2Edit.exe+286C0 - 50                    - push eax
//AA2Edit.exe+286C1 - 57                    - push edi

//setting hair after button press or when tabs are switched. ebx e [0:3] depending on tab
//AA2Edit.exe+280E6 - 88 84 1A 9C060000     - mov[edx+ebx+0000069C],al
