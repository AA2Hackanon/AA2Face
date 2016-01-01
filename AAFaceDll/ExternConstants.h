#pragma once
#include <Windows.h>
extern const char* g_strModuleName;
extern "C" DWORD g_AA2Base;
extern DWORD g_AA2RedrawFlagTable;
extern const HWND* g_AA2MainWndHandle;
extern const HWND* g_AA2DialogHandles;
extern const HWND* g_AA2UpperDialogHandle;
extern float* g_AA2CurrZoom;
extern float* g_AA2MaxTilt;
float* g_AA2GetTilt();
int g_AA2GetCurrentDialogTab();

class BaseDialogClass {
protected:
	void* GetChoiceDataBuffer();
	const void* GetChoiceDataBuffer() const;
	HWND GetButtonListWnd(int offset,int buttonNumber) const;
	int GetButtonListCount(int offset) const;
};

class FaceDialogClass : public BaseDialogClass {
public:
	//ebx+48
	HWND GetFaceSlotButton(BYTE slot) const;
	int GetButtonCount() const;
	void SetChangeFlags();
};

/*
 * Emulates known behavior of the class used in the hair dialog init
 * functions etc. i should do one of those for face as well
 */
class HairDialogClass : public BaseDialogClass {
public:
	BYTE* HairOfTab(int tab);
	BYTE* FlipBoolOfTab(int tab);
	void SetHairChangeFlags(int tab);
	bool HairSlotExists(BYTE slot,int hairKind) const;
	bool HairSlotExists2(BYTE slot,int hairKind) const;
	const BYTE* GetHairSlotExistsField(int hairKind) const;
	HWND GetHairSlotButton(BYTE slot) const;
	HWND GetFlipButtonWnd() const;
	HWND GetHairSizeEditWnd() const;
	int GetButtonCount() const;
};

class FacedetailsDialogClass : public BaseDialogClass {
public:
	BYTE GetEyelidSlot() const;
	BYTE GetUpperEyelidSlot() const;
	BYTE GetLowerEyelidSlot() const;
	BYTE GetGlassesSlot() const;
	BYTE GetLipColorSlot() const;
	void SetGlassesSlot(BYTE slot);
	HWND GetGlassesButtonWnd(BYTE n) const;
	int GetGlassButtonCount() const;
	void SetChangeFlags();
};

class BodycolorDialogClass : public BaseDialogClass {
public:
	HWND GetTanButtonHwnd(BYTE n) const;
	int GetTanButtonCount() const;
	void SetChangeFlags();
	BYTE GetCurrentTanSlot() const;
	void SetCurrentTanSlot(BYTE slot);
};

float* GetMinZoom();
float* GetMaxZoom();

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
//note that HWND -> class is done by a call to GetPropW(HWND, L"__WND_BASE_CLASS__") (AA2Edit.exe+3100A4)

//starting of said virtual handler (the one for the face part)
//AA2Edit.exe+22410 - 8B 44 24 08           - mov eax,[esp+08]
//AA2Edit.exe+22414 - 3D 11010000           - cmp eax,00000111
//AA2Edit.exe+22419 - 77 53                 - ja AA2Edit.exe+2246E
//AA2Edit.exe+2241B - 74 2E                 - je AA2Edit.exe+2244B

//WM_INITDIALOG handler of face
//AA2Edit.exe+22429 - 51                    - push ecx
//AA2Edit.exe+2242A - E8 81FDFFFF           - call AA2Edit.exe+221B0

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

//function that handles WM_INITDIALOG
//AA2Edit.exe+28674 - E8 97F8FFFF           - call AA2Edit.exe+27F10
//AA2Edit.exe+28679 - 5F                    - pop edi
//AA2Edit.exe+2867A - 33 C0                 - xor eax,eax
//AA2Edit.exe+2867C - 5E                    - pop esi
//AA2Edit.exe+2867D - C2 1000               - ret 0010


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


//edi is the HairDialog class
//ecx is the tab and given as parameter (0-3 again)
//AA2Edit.exe+29BAB - 8B 4C 24 14           - mov ecx,[esp+14]
//AA2Edit.exe+29BAF - C1 E1 04              - shl ecx,04
//AA2Edit.exe+29BB2 - 8B 84 39 08050000     - mov eax,[ecx+edi+00000508]
//AA2Edit.exe+29BB9 - 8D 1C 39              - lea ebx,[ecx+edi]
//sets hair buttons to disable/enable depending on wether this slot exists (ecx is bool) (on tab switch) (only custom buttons tho)
//AA2Edit.exe+29BF8 - 8B 93 88050000        - mov edx,[ebx+00000588]
//AA2Edit.exe+29BFE - 80 3C 16  00          - cmp byte ptr[esi+edx],00	bool field of loaded hair
//AA2Edit.exe+29C02 - 0F95 C0               - setne al
//AA2Edit.exe+29C05 - 0FB6 C8               - movzx ecx,al
//AA2Edit.exe+29C08 - 51                    - push ecx
//AA2Edit.exe+29C09 - 8B 4F 48              - mov ecx,[edi+48]
//AA2Edit.exe+29C0C - 8B C6                 - mov eax,esi
//AA2Edit.exe+29C0E - E8 CDD7FEFF           - call AA2Edit.exe+173E0
//note: [[ecx+14] + (hairslot << 4)] = HWND of button
//more generally, ecx+14 seems to be a pointer to an array, where ecx+18 is the pointer to the first invalid(?)
//element of said array (it is defintely used to calculate wether the given slot has a button or not)

//AA2Edit.exe+D9FEF - 3B F7                 - cmp esi,edi
//AA2Edit.exe+D9FF1 - 74 0E                 - je AA2Edit.exe+DA001 jump if hair does not exist, somewhere nowhere.
//sources:
//this makes edi. esp+40 is a parameter which appears to be tab+2 [2 for front, 3 for side, 4 for back, 5 for extension]
//AA2Edit.exe+D9F47 - 8B 44 24 40           - mov eax,[esp+40]
//AA2Edit.exe+D9F4B - 8D 50 02              - lea edx,[eax+02]
//AA2Edit.exe+D9F4E - C1 E0 04              - shl eax,04
//AA2Edit.exe+D9F51 - 8B 7C 08 24           - mov edi,[eax+ecx+24]
//now were missing ecx from that. ecx is this and comes from
//AA2Edit.exe+118F1F - 8B 47 0C              - mov eax,[edi+0C]			edi = HairDialogClass
//AA2Edit.exe+118F27 - 8B 88 9C0A0000        - mov ecx,[eax+00000A9C]
//this makes esi. the call seems to be a void call, not even a thiscall. lets try that.
//AA2Edit.exe+D9FBD - E8 2E0B0000           - call AA2Edit.exe+DAAF0
//AA2Edit.exe+D9FC2 - 8B 30                 - mov esi,[eax]


//AA2Edit.exe+28E76 - E8 45120000           - call AA2Edit.exe+2A0C0
//this function pushes another bool into the HairExists field. it does so [edi+60] times, which can be increased
//to add more fields as well as more buttons.

//this part limits, at construction, the amount of front-hairs.
//AA2Edit.exe+28D10 - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+28D15 - 77 0B                 - ja AA2Edit.exe+28D22
//AA2Edit.exe+28D17 - 3B C3                 - cmp eax,ebx				ebx=0
//AA2Edit.exe+28D19 - 76 0E                 - jna AA2Edit.exe+28D29
//AA2Edit.exe+28D1B - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+28D20 - 76 09                 - jna AA2Edit.exe+28D2B
//AA2Edit.exe+28D22 - B8 87000000           - mov eax,00000087
//AA2Edit.exe+28D27 - EB 02                 - jmp AA2Edit.exe+28D2B
//AA2Edit.exe+28D29 - 33 C0                 - xor eax,eax
//AA2Edit.exe+28D2B - 8D 8E C4040000        - lea ecx,[esi+000004C4]
//AA2Edit.exe+28D31 - 50                    - push eax
//AA2Edit.exe+28D32 - 89 46 60              - mov[esi+60],eax

//similar locations:
//AA2Edit.exe+28EEA - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+28EF5 - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+28EFC - B8 87000000           - mov eax,00000087

//AA2Edit.exe+291A0 - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+291AB - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+291B2 - B8 87000000           - mov eax,00000087

//AA2Edit.exe+294DB - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+294E6 - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+294ED - B8 87000000           - mov eax,00000087
//if we change all these to FF, we would have all the slots covered in the bool array.
//the game would also create more buttons, but only show the annoying 135 one
//shit. this crashes if the last ones are changed.

//main message loop
/*AA2Edit.exe+4287 - FF 15 0C444E01        - call dword ptr[AA2Edit.exe+2C440C]
AA2Edit.exe+428D - 85 C0                 - test eax,eax
AA2Edit.exe+428F - 74 4D                 - je AA2Edit.exe+42DE
AA2Edit.exe+4291 - 8B 15 DCC65801        - mov edx,[AA2Edit.exe+36C6DC]
AA2Edit.exe+4297 - 8D 4C 24 34           - lea ecx,[esp+34]
AA2Edit.exe+429B - 51                    - push ecx
AA2Edit.exe+429C - 52                    - push edx
AA2Edit.exe+429D - E8 DE671900           - call AA2Edit.exe+19AA80
AA2Edit.exe+42A2 - 85 C0                 - test eax,eax
AA2Edit.exe+42A4 - 75 CA                 - jne AA2Edit.exe+4270
AA2Edit.exe+42A6 - 8D 44 24 34           - lea eax,[esp+34]
AA2Edit.exe+42AA - 50                    - push eax
AA2Edit.exe+42AB - FF 15 08444E01        - call dword ptr[AA2Edit.exe+2C4408]
AA2Edit.exe+42B1 - 8D 4C 24 34           - lea ecx,[esp+34]
AA2Edit.exe+42B5 - 51                    - push ecx
AA2Edit.exe+42B6 - FF 15 04444E01        - call dword ptr[AA2Edit.exe+2C4404]*/


//code directly before the main message loop, called after the
//initial window is answered
//AA2Edit.exe+425B - E8 10CC1800           - call AA2Edit.exe+190E70
//AA2Edit.exe+4260 - 8B 3D 10444601        - mov edi,[AA2Edit.exe+2C4410]
//AA2Edit.exe+4266 - 8B 35 00444601        - mov esi,[AA2Edit.exe+2C4400]
//AA2Edit.exe+426C - 8D 64 24 00           - lea esp,[esp+00]


//this jump seems to be taken whenever a hair is valid
//AA2Edit.exe+119CF2 - 74 0C                 - je AA2Edit.exe+119D00
//AA2Edit.exe+119CF4 - 8B 4C 24 1C           - mov ecx,[esp+1C]
//AA2Edit.exe+119CF8 - C6 84 0E 5C020000 00  - mov byte ptr[esi+ecx+0000025C],00
//AA2Edit.exe+119D00 - 33 D2                 - xor edx,edx

//mov esi, [edi+4C] edi is hair class
//mov ecx, [esi+24] ecx is hair size edit field

//---------FACE DETAILS--------

//part that create the dialog
//AA2Edit.exe+2630E - 68 90AB4801           - push AA2Edit.exe+19AB90
//AA2Edit.exe+26313 - 50                    - push eax
//AA2Edit.exe+26314 - 68 A1000000           - push 000000A1
//AA2Edit.exe+26319 - 51                    - push ecx
//AA2Edit.exe+2631A - FF 15 54435B01        - call dword ptr[AA2Edit.exe+2C4354]
//AA2Edit.exe+26320 - 8B 56 40              - mov edx,[esi+40]
//AA2Edit.exe+26323 - 89 42 28              - mov[edx+28],eax


//WRONGstart of virtual handler
//AA2Edit.exe+34160 - 8B 44 24 08           - mov eax,[esp+08]
//AA2Edit.exe+34164 - 56                    - push esi
//AA2Edit.exe+34165 - 8B F1                 - mov esi,ecx
//AA2Edit.exe+34167 - 3D 10010000           - cmp eax,00000110

//WRONGend of WM_INITDIALOG handler (void, but eax-thiscall/)
//AA2Edit.exe+34249 - E8 82FAFFFF           - call AA2Edit.exe+33CD0

//(fuck this one dialog that also grabs message, i got the wrong handler there)
//actual handler i think
//AA2Edit.exe+273D0 - 55                    - push ebp
//AA2Edit.exe+273D1 - 8B EC                 - mov ebp,esp
//AA2Edit.exe+273D3 - 83 E4 F8              - and esp,-08
//AA2Edit.exe+273D6 - 51                    - push ecx

//actual WM_INITDIALOG handler
//AA2Edit.exe+273F9 - 57                    - push edi
//AA2Edit.exe+273FA - E8 F1EFFFFF           - call AA2Edit.exe+263F0

//series of these calls are filling eyelid etc, 695 is glasses
//AA2Edit.exe+26D62 - E8 3907FFFF           - call AA2Edit.exe+174A0
//AA2Edit.exe+26D67 - 88 83 94060000        - mov[ebx+00000694],al //lower eyelid
//AA2Edit.exe+26D6D - 8B 7D 54              - mov edi,[ebp+54]
//AA2Edit.exe+26D70 - E8 2B07FFFF           - call AA2Edit.exe+174A0
//AA2Edit.exe+26D75 - 88 83 95060000        - mov[ebx+00000695],al //glasses
//AA2Edit.exe+26D7B - 8B 7D 58              - mov edi,[ebp+58]
//AA2Edit.exe+26D7E - E8 1D07FFFF           - call AA2Edit.exe+174A0
//AA2Edit.exe+26D83 - 88 83 96060000        - mov[ebx+00000696],al //lip color


//with ebx coming together as described here, and ebp = parameter = [ebp+B8] with ebp = class
//AA2Edit.exe+26D10 - 55                    - push ebp
//AA2Edit.exe+26D11 - 8B 6C 24 08           - mov ebp,[esp+08]
//AA2Edit.exe+26D15 - 8B 45 44              - mov eax,[ebp+44]
//AA2Edit.exe+26D18 - 8B 48 24              - mov ecx,[eax+24]
//AA2Edit.exe+26D1B - 83 39 00              - cmp dword ptr[ecx],00
//AA2Edit.exe+26D1E - 75 05                 - jne AA2Edit.exe+26D25
//AA2Edit.exe+26D20 - 8B 40 30              - mov eax,[eax+30]
//AA2Edit.exe+26D23 - EB 03                 - jmp AA2Edit.exe+26D28
//AA2Edit.exe+26D25 - 8B 40 2C              - mov eax,[eax+2C]
//AA2Edit.exe+26D28 - 85 C0                 - test eax,eax
//AA2Edit.exe+26D2A - 75 06                 - jne AA2Edit.exe+26D32
//AA2Edit.exe+26D2C - 32 C0                 - xor al,al
//AA2Edit.exe+26D2E - 5D                    - pop ebp
//AA2Edit.exe+26D2F - C2 0400               - ret 0004
//AA2Edit.exe+26D32 - 53                    - push ebx
//AA2Edit.exe+26D33 - 8B 58 28              - mov ebx,[eax+28]

//the glasses buttons function (call AA2Edit.exe+174A0):
//with edi = [ebp+54] from above,
//esi = [edi+14] is the first struct button, [edi+18] is the first unused button struct (as known from hair)
//esi is than += 0x10ed, while [esi] is the hwnd of the button. neato.

//call to this function initialises face details dialog, with ebp being the class ptr
//AA2Edit.exe+32708 - 55                    - push ebp
//AA2Edit.exe+32709 - E8 82020000           - call AA2Edit.exe+32990

//sets the current tab (0 = system etc). esi is class of the header dialog window
//AA2Edit.exe+341A6 - 88 46 74              - mov[esi+74],al

//start of virtual handler of body color tab
//AA2Edit.exe+21350 - 55                    - push ebp
//AA2Edit.exe+21351 - 8B EC                 - mov ebp,esp
//AA2Edit.exe+21353 - 83 E4 F8              - and esp,-08

//WM_INITDIALOG handler of body color tab
//AA2Edit.exe+21374 - 57                    - push edi
//AA2Edit.exe+21375 - E8 76E9FFFF           - call AA2Edit.exe+1FCF0

//sets the current tan when a new one selected
//AA2Edit.exe+20B51 - 8B 7B 54              - mov edi,[ebx+54]
//AA2Edit.exe+20B54 - E8 4769FFFF           - call AA2Edit.exe+174A0
//AA2Edit.exe+20B59 - 88 86 67040000        - mov[esi+00000467],al

//again, the esi for the tan byte (its always the same, is it)
/*AA2Edit.exe+20AE0 - 53                    - push ebx
AA2Edit.exe+20AE1 - 8B 5C 24 08           - mov ebx,[esp+08]
AA2Edit.exe+20AE5 - 8B 43 44              - mov eax,[ebx+44]
AA2Edit.exe+20AE8 - 8B 48 24              - mov ecx,[eax+24]
AA2Edit.exe+20AEB - 83 39 00              - cmp dword ptr [ecx],00
AA2Edit.exe+20AEE - 75 05                 - jne AA2Edit.exe+20AF5
AA2Edit.exe+20AF0 - 8B 40 30              - mov eax,[eax+30]
AA2Edit.exe+20AF3 - EB 03                 - jmp AA2Edit.exe+20AF8
AA2Edit.exe+20AF5 - 8B 40 2C              - mov eax,[eax+2C]
AA2Edit.exe+20AF8 - 85 C0                 - test eax,eax
AA2Edit.exe+20AFA - 75 06                 - jne AA2Edit.exe+20B02
AA2Edit.exe+20AFC - 32 C0                 - xor al,al
AA2Edit.exe+20AFE - 5B                    - pop ebx
AA2Edit.exe+20AFF - C2 0400               - ret 0004
AA2Edit.exe+20B02 - 56                    - push esi
AA2Edit.exe+20B03 - 8B 70 28              - mov esi,[eax+28]
AA2Edit.exe+20B06 - 85 F6                 - test esi,esi
AA2Edit.exe+20B08 - 75 07                 - jne AA2Edit.exe+20B11
AA2Edit.exe+20B0A - 5E                    - pop esi
AA2Edit.exe+20B0B - 32 C0                 - xor al,al
AA2Edit.exe+20B0D - 5B                    - pop ebx
AA2Edit.exe+20B0E - C2 0400               - ret 0004
AA2Edit.exe+20B11 - 57                    - push edi
AA2Edit.exe+20B12 - 8B 7B 48              - mov edi,[ebx+48]*/

//start of system dialog handler
//AA2Edit.exe+1BAD0 - 8B 44 24 08           - mov eax,[esp+08]
//AA2Edit.exe+1BAD4 - 3D 11010000           - cmp eax,00000111

//last call of (successful) open file dialog click:
//AA2Edit.exe+1C26C - E8 9F73FEFF           - call AA2Edit.exe+3610

//exit (last jump) of create character button
//AA2Edit.exe+1C4C1 - E9 97FAFFFF           - jmp AA2Edit.exe+1BF5D




//misc:
//AA2Edit.exe+196CA7 - D9 82 88010000        - fld dword ptr[edx+00000188]
//AA2Edit.exe+196CE0 - D9 82 8C010000        - fld dword ptr[edx+0000018C]
//load the min/max values for zooming, respectively for zoom limit
/*
push AA2Edit.exe+344F40
[esp+04] note the esp
[eax+E314] can be NULL if character is not loaded, make sure to check that
[edi+20]
[esi+04]
lots of carrying
[edx+188] / [edx+18C]
*/